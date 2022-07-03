/*
  ESP32 Camera Sensor interface class implementation.
  Date: 2021-12-27
  
  Copyright (c) 2021 Hieromon Ikasamo.
  This software is released under the MIT License.
  https://opensource.org/licenses/MIT

  This class is an interface to the ESP32 Camera Driver, and contains an HTTP
  server that can convert frame data captured by the ESP32 Camera Driver into
  JPEG format and respond as HTTP content.
  The supported camera sensor devices and general limitations follow the ESP32
  Camera Driver specification.

  Related information is as follows:
    ESP32 Camera Driver Repository on the GitHub: https://github.com/espressif/esp32-camera
*/

#include <esp32-hal.h>
#include <esp32-hal-log.h>
#include <img_converters.h>
#include "ESP32Cam.h"
#include "ESP32Cam_pins.h"

// The interrupt handler for the timer shot uses a static Mutex and a pointer
// to an ESP32Cam instance.
namespace ESP32Cam_internal {

ESP32Cam* esp32cam;
volatile  SemaphoreHandle_t xMutex = NULL;

};

// Name of a dummy file to check if the file system is mounted.
const char ESP32Cam::_mountProbe[]= { ESP32CAM_FS_MOUNTPROBE };

/**
 * ESP32Cam constractor
 */
ESP32Cam::ESP32Cam() : _sdFile(nullptr), _mounted(MOUNT_NONE), _cameraId(CAMERA_MODEL_UNKNOWN), _pins(nullptr), _sensor(nullptr), _hwTimer(nullptr) {
  ESP32Cam_internal::esp32cam = nullptr;
}

/**
 * ESP32Cam constractor
 * Depending on the sensor device type, determine the ESP32 module's wiring
 * pin assignment.
 * @param  model  The model ID of the sensor to be used.
 */
ESP32Cam::ESP32Cam(const CameraId model) : _sdFile(nullptr), _mounted(MOUNT_NONE), _cameraId(model), _pins(nullptr), _hwTimer(nullptr) {
  ESP32Cam_internal::esp32cam = nullptr;
}

/**
 * ESP32Cam destructor
 * Sensor deinitialization and purge hardware timer.
 */
ESP32Cam::~ESP32Cam() {
  esp_camera_deinit();
  _purgeTimer();
}

/**
 * Device initialization detects and configures camera over I2C interface,
 * allocates framebuffer and DMA buffers, initializes parallel I2S input,
 * and sets up DMA descriptors.
 * @param  model  The model ID of the sensor to be used.
 */
esp_err_t ESP32Cam::init(const CameraId model) {
  // Identify the valid pin-map that a model parameter represent.
  _cameraId = CAMERA_MODEL_UNKNOWN;
  for (uint8_t id = 0; id < (sizeof(_pinsMap) / sizeof(_pinsMap[0])); id++) {
    if (_pinsMap[id].id == model) {
      _cameraId = model;
      _pins = &_pinsMap[id].pins;
      break;
    }
  }

  if (_cameraId == CAMERA_MODEL_UNKNOWN)
    return ESP_ERR_CAMERA_NOT_SUPPORTED;

  // Sensor configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = _pins->y2_gpio_num;
  config.pin_d1 = _pins->y3_gpio_num;
  config.pin_d2 = _pins->y4_gpio_num;
  config.pin_d3 = _pins->y5_gpio_num;
  config.pin_d4 = _pins->y6_gpio_num;
  config.pin_d5 = _pins->y7_gpio_num;
  config.pin_d6 = _pins->y8_gpio_num;
  config.pin_d7 = _pins->y9_gpio_num;
  config.pin_xclk = _pins->xclk_gpio_num;
  config.pin_pclk = _pins->pclk_gpio_num;
  config.pin_vsync = _pins->vsync_gpio_num;
  config.pin_href = _pins->href_gpio_num;
  config.pin_sscb_sda = _pins->siod_gpio_num;
  config.pin_sscb_scl = _pins->sioc_gpio_num;
  config.pin_pwdn = _pins->pwdn_gpio_num;
  config.pin_reset = _pins->reset_gpio_num;
  config.xclk_freq_hz = ESP32CAM_XCLK_FREQ;
  config.pixel_format = PIXFORMAT_JPEG;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality for
  // larger pre-allocated frame buffer.
  if (psramFound()) {
    _psram = true;
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
#if defined(ESP_IDF_VERSION_MAJOR) && ESP_IDF_VERSION_MAJOR>=4
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_LATEST;
#endif
  } else {
    _psram = false;
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
#if defined(ESP_IDF_VERSION_MAJOR) && ESP_IDF_VERSION_MAJOR>=4
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
#endif
  }

  // The pull-up required for wiring for ESP_EYE, but has not been verified.
  if (_cameraId == CAMERA_MODEL_ESP_EYE) {
    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
  }

  // Camera initialization
  esp_err_t err = esp_camera_init(&config);
  if (err == ESP_OK) {
    _sensor = esp_camera_sensor_get();
    if (_sensor->id.PID == OV3660_PID) {
      _sensor->set_vflip(_sensor, 1); // flip it back
      _sensor->set_brightness(_sensor, 1); // up the brightness just a bit
      _sensor->set_saturation(_sensor, -2); // lower the saturation
    }
    _sensor->set_framesize(_sensor, FRAMESIZE_SVGA);

    if (_cameraId == CAMERA_MODEL_M5STACK_WIDE || _cameraId == CAMERA_MODEL_M5STACK_ESP32CAM) {
      _sensor->set_vflip(_sensor, 1);
      _sensor->set_hmirror(_sensor, 1);
    }
  }
  else
    log_e("Camera %d init failed 0x%04x\n", (int)_cameraId, err);

  // Create semaphore to inform us when the timer has fired
  ESP32Cam_internal::xMutex = xSemaphoreCreateMutex();
  xSemaphoreGive(ESP32Cam_internal::xMutex);

  ESP32Cam_internal::esp32cam = this;
  return err;
}

/**
 * Get the current frame size. It returns the frame size of the current image
 * that can be captured.
 * @return Enum value of framesize_t defined in ESP32 Camera driver
 */
framesize_t ESP32Cam::getFramesize(void) {
  if (_sensor)
    return _sensor->status.framesize;
  return FRAMESIZE_INVALID;
}

/**
 * Get the current frame height. It returns the frame height of the current
 * image that can be captured.
 * @return Height by pixel unit
 */
uint16_t ESP32Cam::getFrameHeight(void) {
  uint16_t  height;

  switch (getFramesize()) {
  case FRAMESIZE_96X96:
    height = 96;
    break;
  case FRAMESIZE_QQVGA:
    height = 120;
    break;
  case FRAMESIZE_QCIF:
    height = 144;
    break;
  case FRAMESIZE_HQVGA:
    height = 176;
    break;
  case FRAMESIZE_240X240:
  case FRAMESIZE_QVGA:
    height = 240;
    break;
  case FRAMESIZE_CIF:
    height = 296;
    break;
  case FRAMESIZE_HVGA:
    height = 320;
    break;
  case FRAMESIZE_VGA:
    height = 480;
    break;
  case FRAMESIZE_SVGA:
    height = 600;
    break;
  case FRAMESIZE_XGA:
    height = 768;
    break;
  case FRAMESIZE_HD:
    height = 720;
    break;
  case FRAMESIZE_SXGA:
    height = 1024;
    break;
  case FRAMESIZE_UXGA:
    height = 1200;
    break;
  case FRAMESIZE_FHD:
    height = 1080;
    break;
  case FRAMESIZE_P_HD:
    height = 1280;
    break;
  case FRAMESIZE_P_3MP:
  case FRAMESIZE_QXGA:
    height = 1536;
    break;
  case FRAMESIZE_QHD:
    height = 1440;
    break;
  case FRAMESIZE_WQXGA:
    height = 1600;
    break;
  case FRAMESIZE_P_FHD:
  case FRAMESIZE_QSXGA:
    height = 1920;
    break;
  default:
    height = 0;
  }
  return height;
}

/**
 * Get the current frame width. It returns the frame height of the current
 * image that can be captured.
 * @return Width by pixel unit
 */
uint16_t ESP32Cam::getFrameWidth(void) {
  uint16_t  width;

  switch (getFramesize()) {
  case FRAMESIZE_96X96:
    width = 96;
    break;
  case FRAMESIZE_QQVGA:
    width = 160;
    break;
  case FRAMESIZE_QCIF:
    width = 176;
    break;
  case FRAMESIZE_HQVGA:
  case FRAMESIZE_240X240:
    width = 240;
    break;
  case FRAMESIZE_QVGA:
    width = 320;
    break;
  case FRAMESIZE_CIF:
  case FRAMESIZE_HVGA:
    width = 400;
    break;
  case FRAMESIZE_VGA:
    width = 640;
    break;
  case FRAMESIZE_SVGA:
    width = 800;
    break;
  case FRAMESIZE_XGA:
    width = 1024;
    break;
  case FRAMESIZE_HD:
  case FRAMESIZE_SXGA:
    width = 1280;
    break;
  case FRAMESIZE_UXGA:
    width = 1600;
    break;
  case FRAMESIZE_FHD:
    width = 1920;
    break;
  case FRAMESIZE_P_HD:
    width = 720;
    break;
  case FRAMESIZE_P_3MP:
    width = 864;
    break;
  case FRAMESIZE_QXGA:
    width = 2048;
    break;
  case FRAMESIZE_QHD:
  case FRAMESIZE_WQXGA:
  case FRAMESIZE_QSXGA:
    width = 2560;
    break;
  case FRAMESIZE_P_FHD:
    width = 1080;
    break;
  default:
    width = 0;
  }
  return width;
}

/**
 * Load the image sensor settings from NVS with specified key.
 * @param  key     The name of the NVS key attached to the settings to load. If
 * key value is nullptr, assumes ESP32CAM_NVS_KEYNAME as default.
 * @return ESP_OK
 */
esp_err_t ESP32Cam::loadSettings(const char* key) {
  if (!key)
    key = ESP32CAM_NVS_KEYNAME;
  return esp_camera_load_from_nvs(key);
}

/**
 * Save the image sensor settings to NVS with specified key.
 * @param  key     The name of the NVS key attached to the settings to load. If
 * key value is nullptr, assumes ESP32CAM_NVS_KEYNAME as default.
 * @return ESP_OK
 */
esp_err_t ESP32Cam::saveSettings(const char* key) {
  if (!key)
    key = ESP32CAM_NVS_KEYNAME;
  return esp_camera_save_to_nvs(key);
}

/**
 * Set the frame size which capturing with the image sensor.
 * QQVGA-QXGA Do not use sizes above QVGA when not JPEG.
 * @param  framesize Enum value of framesize_t defined in ESP32 Camera driver
 * @return ESP_OK    Frame size is now set.
 * @return ESP_FAIL  framesize is invalid.
 */
esp_err_t ESP32Cam::setFramesize(const framesize_t framesize) {
  if (_sensor) {
    if (_sensor->pixformat == PIXFORMAT_JPEG)
      return _sensor->set_framesize(_sensor, framesize);
    return ESP_ERR_CAMERA_FAILED_TO_SET_FRAME_SIZE;
  }
  return ESP_ERR_CAMERA_FAILED_TO_SET_FRAME_SIZE;
}

/**
 * Set image data format output from the sensor.
 * @param  format   Enum value of pixformat_t defined in ESP32 Camera driver
 * @return ESP_OK   Pixel format is now set.
 * @return ESP_FAIL format is invalid.
 */
esp_err_t ESP32Cam::setImageFormat(const pixformat_t format) {
  if (_sensor)
    return _sensor->set_pixformat(_sensor, format) == 0 ? ESP_OK : ESP_FAIL;
  return ESP_FAIL;
}

/**
 * Get the content of camera_status_t structure from the image sensor.
 * @param  status   Buffer for storing the acquired camara_status_t
 * @return ESP_OK   Got sensor status.
 * @return ESP_FAIL Error has occurred.
 */
esp_err_t ESP32Cam::getStatus(camera_status_t* status) {
  _sensor = esp_camera_sensor_get();
  if (_sensor) {
    memcpy(status, (const void*)&_sensor->status, sizeof (camera_status_t));
    return ESP_OK;
  }
  return ESP_FAIL;
}

/**
 * Set the content of camera_status_t structure to the image sensor.
 * @param  status   Buffer for storing the acquired camara_status_t
 * @return ESP_OK   The sensor configuration now is set.
 * @return ESP_FAIL Error has occurred.
 */
esp_err_t ESP32Cam::setStatus(const camera_status_t& status) {
  if (!_sensor)
    return ESP_FAIL;
  
  esp_err_t rc = setFramesize(status.framesize);
  if (_sensor->set_quality(_sensor, status.quality))
    rc = ESP_FAIL;
  if (_sensor->set_brightness(_sensor, status.brightness))
    rc = ESP_FAIL;
  if (_sensor->set_contrast(_sensor, status.contrast))
    rc = ESP_FAIL;
  if (_sensor->set_saturation(_sensor, status.saturation))
    rc = ESP_FAIL;
  if (_sensor->set_sharpness(_sensor, status.sharpness))
    rc = ESP_FAIL;
  if (_sensor->set_denoise(_sensor, status.denoise))
    rc = ESP_FAIL;
  if (_sensor->set_special_effect(_sensor, status.special_effect))
    rc = ESP_FAIL;
  if (_sensor->set_whitebal(_sensor, status.awb))
    rc = ESP_FAIL;
  if (_sensor->set_wb_mode(_sensor, status.wb_mode))
    rc = ESP_FAIL;
  if (_sensor->set_awb_gain(_sensor, status.awb_gain))
    rc = ESP_FAIL;
  if (_sensor->set_exposure_ctrl(_sensor, status.aec))
    rc = ESP_FAIL;
  if (_sensor->set_aec2(_sensor, status.aec2))
    rc = ESP_FAIL;
  if (_sensor->set_ae_level(_sensor, status.ae_level))
    rc = ESP_FAIL;
  if (_sensor->set_aec_value(_sensor, status.aec_value))
    rc = ESP_FAIL;
  if (_sensor->set_gain_ctrl(_sensor, status.agc))
    rc = ESP_FAIL;
  if (_sensor->set_agc_gain(_sensor, status.agc_gain))
    rc = ESP_FAIL;
  if (_sensor->set_gainceiling(_sensor, (gainceiling_t)status.gainceiling))
    rc = ESP_FAIL;
  if (_sensor->set_bpc(_sensor, status.bpc))
    rc = ESP_FAIL;
  if (_sensor->set_wpc(_sensor, status.wpc))
    rc = ESP_FAIL;
  if (_sensor->set_raw_gma(_sensor, status.raw_gma))
    rc = ESP_FAIL;
  if (_sensor->set_lenc(_sensor, status.lenc))
    rc = ESP_FAIL;
  if (_sensor->set_hmirror(_sensor, status.hmirror))
    rc = ESP_FAIL;
  if (_sensor->set_vflip(_sensor, status.vflip))
    rc = ESP_FAIL;
  if (_sensor->set_dcw(_sensor, status.dcw))
    rc = ESP_FAIL;
  if (_sensor->set_colorbar(_sensor, status.colorbar))
    rc = ESP_FAIL;

  return rc;
}

/**
 * Take a one-shot image and save it to the SD card.
 * This function writes to a file compliant with the following SD library,
 * and the SD card must be connected to the ESP32 module via SPI.
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/SD
 * @param  sd        fs::SDFS instance that handles the destination SD card.
 * @param  filename  File name for saving the captured image.
 * @return ESP_OK    Image file saved.
 * @return ESP_ERR_INVALID_STATE  SD card is not mounted.
 * @return ESP_FAIL  Any other error occurred.
 */
esp_err_t ESP32Cam::oneShot(fs::SDFS& sd, const char* filename) {
  // Allow one-shot writes even if SD.begin() is not running in the user sketch.
  // To do so, it detects the card mount state only once and tries to start the
  // SD file system.
  if (!_autoMount(MOUNT_SD, reinterpret_cast<fs::FS*>(&sd)))
    return ESP_ERR_INVALID_STATE;
  return _oneShot(filename);
}

/**
 * Take a one-shot image and save it to the MMC.
 * This function writes to a file compliant with the following SD library,
 * and The SD card needs to be connected to the ESP32 module via HS2 wiring.
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/SD
 * @param  mmc       fs::SDMMCFS instance that handles the destination MMC.
 * @param  filename  File name for saving the captured image.
 * @return ESP_OK    Image file saved.
 * @return ESP_ERR_INVALID_STATE  SD card is not mounted.
 * @return ESP_FAIL  Any other error occurred.
 */
esp_err_t ESP32Cam::oneShot(fs::SDMMCFS& mmc, const char* filename) {
  // Allow one-shot writes even if SDMMC.begin() is not running in the user
  // sketch. To do so, it detects the card mount state only once and tries to
  // start the SDMMC file system.
  if (!_autoMount(MOUNT_SDMMC, reinterpret_cast<fs::FS*>(&mmc)))
    return ESP_ERR_INVALID_STATE;
  return _oneShot(filename);
}

/**
 * Exclusively get semaphore and output the image from the image sensor to a
 * file. If the filename parameter is nullptr, it will assume a file name with
 * a timestamp prefixed with a fixed string defined by
 * the ESP32CAM_GLOBAL_IDENTIFIER macro.
 * @param  sd        fs::FS filesystem
 * @param  filename  File name for saving the captured image.
 * @return ESP_OK    Image file saved.
 * @return ESP_FAIL  Any other error occurred.
 */
esp_err_t ESP32Cam::_oneShot(const char* filename) {
  char  fn[sizeof(ESP32CAM_GLOBAL_IDENTIFIER) + 20]{"/" ESP32CAM_GLOBAL_IDENTIFIER};
  // Assume the file name to be written
  if (!filename) {
    _appendTimestamp(fn);
    filename = fn;
  }
  if (!strlen(filename)) {
    _appendTimestamp(fn);
    filename = fn;
  }

  // Start taking a one-shot
  esp_err_t rc = ESP_FAIL;
  if (enq(portMAX_DELAY) == pdTRUE) {
    rc = _export(filename, nullptr);
    deq();
  }
  return rc;
}

/**
 * Repeatedly takes a one-shot image at a specified cycle and saves it to the
 * SD card. Saves the captured image to a file whose name consists of the
 * prefix and timestamp suffix specified by the filename argument. If the
 * filename argument does not exist, the prefix will be assumed a fixed string
 * defined by the ESP32CAM_GLOBAL_IDENTIFIER macro directive in ESP32Cam.h.
 * This function writes to a file compliant with the following SD library,
 * and the SD card must be connected to the ESP32 module via SPI.
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/SD
 * @param  period         Period to shoot in seconds.
 * @param  sd             fs::SD filesystem
 * @param  filenamePrefix File name for saving the captured image.
 * @return ESP_OK         Image file saved.
 * @return ESP_FAIL       Any other error occurred.
 */
esp_err_t ESP32Cam::timerShot(const unsigned long period, fs::SDFS& sd, const char* filenamePrefix) {
  // Allow one-shot writes even if SD.begin() is not running in the user sketch.
  // To do so, it detects the card mount state only once and tries to start the
  // SD file system.
  if (!_autoMount(MOUNT_SD, reinterpret_cast<fs::FS*>(&sd)))
    return ESP_ERR_INVALID_STATE;
  _timerShot(period, filenamePrefix);
  return ESP_OK;
}

/**
 * Repeatedly takes a one-shot image at a specified cycle and saves it to the
 * MMC. Saves the captured image to a file whose name consists of the prefix
 * and timestamp suffix specified by the filename argument. If the filename
 * argument does not exist, the prefix will be assumed a fixed string defined
 * by the ESP32CAM_GLOBAL_IDENTIFIER macro directive in ESP32Cam.h.
 * This function writes to a file compliant with the following SD library,
 * and The SD card needs to be connected to the ESP32 module via HS2 wiring.
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/SD
 * @param  period         Period to shoot in seconds.
 * @param  mmc            fs::SDMMC filesystem
 * @param  filenamePrefix File name for saving the captured image.
 * @return ESP_OK         Image file saved.
 * @return ESP_FAIL       Any other error occurred.
 */
esp_err_t ESP32Cam::timerShot(const unsigned long period, fs::SDMMCFS& mmc, const char* filenamePrefix) {
  // Allow one-shot writes even if SDMMC.begin() is not running in the user
  // sketch. To do so, it detects the card mount state only once and tries to
  // start the SDMMC file system.
  if (!_autoMount(MOUNT_SDMMC, reinterpret_cast<fs::FS*>(&mmc)))
    return ESP_ERR_INVALID_STATE;
  _timerShot(period, filenamePrefix);
  return ESP_OK;
}

/**
 * Exclusively get semaphore and repeatedly takes a one-shot image.
 * The cycle for the timer shot is generated by the hardware timer provided by
 * the ESP32 module. This function initializes the timer for periodic execution
 * of the timer interrupt function for taking pictures.
 * @param  period         Period to shoot in seconds.
 * @param  filenamePrefix File name for saving the captured image.
 */
void ESP32Cam::_timerShot(const unsigned long period, const char* filenamePrefix) {
  if (!period)
    return;

  if (!filenamePrefix)
    filenamePrefix = "/" ESP32CAM_GLOBAL_IDENTIFIER;
  if (!strlen(filenamePrefix))
    filenamePrefix = "/" ESP32CAM_GLOBAL_IDENTIFIER;

  // Save the global variables for the ISR and cancel the timer residue.
  _captureName = filenamePrefix;
  _purgeTimer();

  // Register a task to secure and shoot a hardware timer.
  _hwTimer = timerBegin(ESP32CAM_OCCUPIED_TIMER, getApbFrequency() / 1000000UL, true);
  timerAttachInterrupt(_hwTimer, &ESP32Cam::_timerShotISR, true);
  timerAlarmWrite(_hwTimer, period * 1000000UL, true);

  // Start an alarm
  ESP32Cam_internal::esp32cam = this;
  xSemaphoreGive(ESP32Cam_internal::xMutex);
  timerAlarmEnable(_hwTimer);
}

/**
 * A hardware timer interrupt routine is called when the shooting cycle of the
 * timerShot is reached. Since this ISR needs to finish with minimal processing,
 * it only creates a task that will take a one-shot. 
 */
void IRAM_ATTR ESP32Cam::_timerShotISR(void) {
  if (xTaskCreateUniversal(&ESP32Cam::_timerShotTask, ESP32CAM_GLOBAL_IDENTIFIER, ESP32CAM_TIMERTASK_STACKSIZE, (void*)ESP32Cam_internal::esp32cam, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE) != pdPASS) {
    log_e("TimerShot task failed\n");
  }
}

/**
 * The timer shot task operation runs as a task generated by the interrupt
 * routine. It reserves a semaphore in the first phase of the function to avoid
 * conflicting image sensor access with other tasks. The secured semaphore will
 * not be released until the captured image is exported to SD. In addition, the
 * timer will also stop temporarily.
 * @param  pvParameters  Current instance of ESP32Cam
 */
void ESP32Cam::_timerShotTask(void* pvParametes) {
  // Take a Mutex that avoids contention for image sensor resources.
  if (enq(portMAX_DELAY) == pdTRUE) {

    // Receive current instance of ESP32Cam
    ESP32Cam* esp32cam = reinterpret_cast<ESP32Cam*>(pvParametes);

    // Temporarily stop the timer during execution to suppress task overlap.
    timerStop(esp32cam->_hwTimer);

    // Assemble the file name of the destination of the captured image.
    char  fn[esp32cam->_captureName.length() + sizeof('\0') + 20];
    strcpy(fn, esp32cam->_captureName.c_str());
    esp32cam->_appendTimestamp(fn);

    // Capture and export to SD
    esp32cam->_export(fn, nullptr);

    // Release the resource and restart the timer increment.
    timerStart(esp32cam->_hwTimer);
    deq();
    // Purge itself
    vTaskDelete(NULL);
  }
}

/**
 * Temporarily stops the timerShot cycle.
 */
void ESP32Cam::disableTimerShot(void) {
  if (_hwTimer) {
    timerAlarmDisable(_hwTimer);
    switch (_mounted) {
    case MOUNT_SD: {
      fs::SDFS* sd = reinterpret_cast<fs::SDFS*>(_sdFile);
      sd->end();
      break;
    }
    case MOUNT_SDMMC: {
      fs::SDMMCFS*  sdmmc = reinterpret_cast<fs::SDMMCFS*>(_sdFile);
      sdmmc->end();
      break;
    }
    case MOUNT_NONE:
      break;
    }
  }
}

/**
 * Resume the shooting cycle of timerShot that has been temporarily stopped.
 */
void ESP32Cam::enableTimerShot(void) {
  if (_hwTimer) {
    switch (_mounted) {
    case MOUNT_SD: {
      fs::SDFS* sd = reinterpret_cast<fs::SDFS*>(_sdFile);
      sd->begin();
      break;
    }
    case MOUNT_SDMMC: {
      fs::SDMMCFS*  sdmmc = reinterpret_cast<fs::SDMMCFS*>(_sdFile);
      sdmmc->begin();
      break;
    }
    case MOUNT_NONE:
      break;
    }
    timerAlarmEnable(_hwTimer);
  }
}

/**
 * Release a semaphore.
 */
void ESP32Cam::deq(void) {
  xSemaphoreGive(ESP32Cam_internal::xMutex);
}

/**
 * Take a semaphore to prevent resource collisions with the image sensor.
 * ESP32Cam uses a semaphore for each of oneShot and timerShot to prevent image
 * sensor resources from colliding while performing each other's tasks. The enq
 * function reserves a semaphore to wait for a subsequent oneShot or timerShot.
 * @param  ms      File name to output the captured image
 * @return pdTRUE  Semaphore was reserved.
 * @return !pdTRUE Semaphore was not released within the specified `ms` time.
 */
portBASE_TYPE ESP32Cam::enq(TickType_t ms) {
  if (ms != portMAX_DELAY)
    ms = ms / portTICK_PERIOD_MS;
  return xSemaphoreTake(ESP32Cam_internal::xMutex, ms);
}

/**
 * Adds a timestamp that is part of the destination filename of the captured
 * image. The getLocalTime posted internally does not synchronize with the 
 * internet time. In order to reflect the exact time in the time stamp, it is
 * necessary to synchronize with Time Server using configTzTime.
 * @parama  eol  Timestamp string output destination.
 * @return  Ouput size
 */
size_t ESP32Cam::_appendTimestamp(char* str) {
  struct tm timeInfo;

  getLocalTime(&timeInfo);
  strftime(str + strlen(str), 20, "%FT%H_%M_%S", &timeInfo);
  return strlen(str);
}

/**
 * Detect the mount status of the SD card and remount it if it has been
 * unplugged once. This avoids sector information mismatch due to SD card
 * removal. It detects the mounting of the SD card by opening a dummy file in
 * write mode. For this reason, this strategy cannot correctly detect a
 * write-protected SD card.
 * This function also absorbs the different types of SDFS and SDMMCFS.
 * Maintains a pointer to the actual SD file system as an fs::FS variant and
 * notes the SD card type. When remounting, casts to the actual file system
 * according to the noted type.
 * @param  sdType  Actual mounted SD filesystem type
 * @param  sdFile  Filesystem that's mounted.
 * @return true    Filesystem mounted
 * @return false   Filesystem could not mounted
 */
bool ESP32Cam::_autoMount(const SDType_t sdType, fs::FS* sdFile) {
  bool  rc = false;

  switch (sdType) {
  case MOUNT_SD: {
    fs::SDFS* sd = reinterpret_cast<fs::SDFS*>(sdFile);
    if (sd->cardType() == CARD_NONE)
      rc = sd->begin();
    else {
      if (!sd->open(_mountProbe, FILE_WRITE)) {
        log_d("SD mount removed, remounting\n");
        sd->end();
        rc = sd->begin();
      }
      else
        rc = sd->remove(_mountProbe);
    }
    if (rc)
      _sdFile = sd;
    break;
  }
  case MOUNT_SDMMC: {
    fs::SDMMCFS*  sdmmc = reinterpret_cast<fs::SDMMCFS*>(sdFile);
    if (sdmmc->cardType() == CARD_NONE)
      rc = sdmmc->begin();
    else {
      if (!sdmmc->open(_mountProbe, FILE_WRITE)) {
        log_d("SDMMC mount removed, remounting\n");
        sdmmc->end();
        rc = sdmmc->begin();
      }
      else
        rc = sdmmc->remove(_mountProbe);
    }
    if (rc)
      _sdFile = sdmmc;
    break;
  }
  case MOUNT_NONE:
    break;
  }
  _mounted = rc ? sdType : MOUNT_NONE;
    
  return rc;
}

/**
 * Output the captured image to the _sd file system indicated by the current
 * instance.
 * If the file name to be exported does not have a proper extension, it will be
 * given an extension according to the ESP32CAM_EXPORT_FILEEXTENSION definition.
 * It also recovers mount points once lost by removing and inserting the SD card
 * by recursive invocation.
 * @param  filename    File name to output the captured image
 * @param  frameBuffer In the case of recursive calls for mount point recovery,
 * it will have the already allocated frame buffer address. Otherwise, nullptr.
 * @return ESP_OK      Successfully output
 * @return ESP_FAIL    Error has occurred
 */
esp_err_t ESP32Cam::_export(const char* filename, camera_fb_t* frameBuffer) {
  esp_err_t rc;

  // Obtain the captured frame buffer.
  // When called recursively, the frameBuffer will have already captured the image.
  if (!frameBuffer)
    frameBuffer = esp_camera_fb_get();

  if (frameBuffer) {
    // Give a proper extension to half-baked filename.
    char  fn[strlen(filename) + sizeof(ESP32CAM_EXPORT_FILEEXTENSION)];
    strcpy(fn, filename);
    const char* ext = strchr(fn, '.');
    bool  bc = ext != nullptr;
    if (bc)
      bc = strcmp(ext, ESP32CAM_EXPORT_FILEEXTENSION) == 0;
    if (!bc)
      strcat(fn, ESP32CAM_EXPORT_FILEEXTENSION);
    // Export
    File  pic = _sdFile->open(fn, FILE_WRITE);
    if (pic) {
      pic.write(frameBuffer->buf, frameBuffer->len);
      pic.close();
      rc = ESP_OK;
    }
    else {
      // If it fails to open the first time, it will try to remount the SD card;
      // if the SD card is just removed and inserted, this attempt will recover
      // the mount point.
      if (_mounted != MOUNT_NONE) {
        if ((rc = _autoMount(_mounted, _sdFile)) == ESP_OK)
          return _export(fn, frameBuffer);
        esp_camera_fb_return(frameBuffer);
        return rc;
      }
      log_e("SD %s open failed\n", fn);
      rc = ESP_ERR_NOT_FOUND;
    }
    esp_camera_fb_return(frameBuffer);
  }
  else {
    log_e("failed to esp_camera_fb_get\n");
    rc = ESP_FAIL;
  }

  return rc;
}

/**
 * Release a timer resource
 */
void ESP32Cam::_purgeTimer(void) {
  if (_hwTimer) {
    if (enq(portMAX_DELAY) == pdTRUE) {
      timerStop(_hwTimer);
      timerDetachInterrupt(_hwTimer);
      timerEnd(_hwTimer);
      _hwTimer = nullptr;
      deq();
    }
  }
}
