/*
  ESP32 Camera Sensor interface class for Arduino.
  Date: 2021-12-24
  
  Copyright (c) 2021 Hieromon Ikasamo.
  This software is released under the MIT License.
  https://opensource.org/licenses/MIT

  This class is an interface to the ESP32 Camera Driver.
  The supported camera sensor devices and general limitations follow the ESP32
  Camera Driver specification.

  Related information is as follows:
    ESP32 Camera Driver Repository on the GitHub: https://github.com/espressif/esp32-camera
*/

#ifndef _ESP32CAM_H_
#define _ESP32CAM_H_

#include <esp_err.h>
#include <esp_camera.h>
#include <esp_http_server.h>
#include <esp32-hal-timer.h>
#include <SD.h>
#include <SD_MMC.h>

// The type of hardware timer used by the timer shot
#ifndef ESP32CAM_OCCUPIED_TIMER
#define ESP32CAM_OCCUPIED_TIMER       1
#endif // !ESP32CAM_OCCUPIED_TIMER

// Default stack size fo Timer-Shot task
#ifndef ESP32CAM_TIMERTASK_STACKSIZE
#define ESP32CAM_TIMERTASK_STACKSIZE  8192
#endif // !ESP32CAM_TIMERTASK_STACKSIZE

// When using the CORE_DEBUG_LEVEL macro switch to log the esp32cam class,
// TAG is an empty string.
#ifndef ESP32CAM_LOGE_TAG
#define ESP32CAM_LOGE_TAG             "ESP32Cam"
#endif // !ESP32CAM_LOGE_TAG
#ifndef ESP32CAM_GLOBAL_IDENTIFIER
#define ESP32CAM_GLOBAL_IDENTIFIER    ESP32CAM_LOGE_TAG
#endif // !ESP32CAM_GLOBAL_IDENTIFIER

// Default extension for a exporting captured image.
#ifndef ESP32CAM_EXPORT_FILEEXTENSION
#define ESP32CAM_EXPORT_FILEEXTENSION ".jpg"
#endif // !ESP32CAM_EXPORT_FILEEXTENSION

// NVS key of ESP32Cam to save the sensor configuration settings.
#ifndef ESP32CAM_NVS_KEYNAME
#define ESP32CAM_NVS_KEYNAME          ESP32CAM_GLOBAL_IDENTIFIER
#endif // !ESP32CAM_NVS_KEYNAME

// Name of a dummy file to check if the file system is mounted.
#ifndef ESP32CAM_FS_MOUNTPROBE
#define ESP32CAM_FS_MOUNTPROBE        "/_~" ESP32CAM_GLOBAL_IDENTIFIER "~"
#endif //!ESP32CAM_FS_MOUNTPROBE

class ESP32Cam {
 public:
  typedef enum {
    CAMERA_MODEL_WROVER_KIT,
    CAMERA_MODEL_ESP_EYE,
    CAMERA_MODEL_M5STACK_NO_PSRAM,
    CAMERA_MODEL_M5STACK_PSRAM,
    CAMERA_MODEL_M5STACK_V2_PSRAM,
    CAMERA_MODEL_M5STACK_WIDE,
    CAMERA_MODEL_M5STACK_ESP32CAM,
    CAMERA_MODEL_M5STACK_UNITCAM,
    CAMERA_MODEL_AI_THINKER,
    CAMERA_MODEL_TTGO_T_JOURNAL,
    CAMERA_MODEL_UNKNOWN
  } CameraId;   // Supported ESP32 module series

  ESP32Cam();
  explicit ESP32Cam(const CameraId model);
  ~ESP32Cam();

  esp_err_t getStatus(camera_status_t* status);
  framesize_t getFramesize(void);
  uint16_t  getFrameHeight(void);
  uint16_t  getFrameWidth(void);
  esp_err_t init(void) { return init(_cameraId); }
  esp_err_t init(const CameraId model);
  esp_err_t loadSettings(const char* key = nullptr);
  esp_err_t saveSettings(const char* key = nullptr);
  esp_err_t setStatus(const camera_status_t& status);
  esp_err_t setFramesize(const framesize_t framesize);
  esp_err_t setImageFormat(const pixformat_t format);

  esp_err_t oneShot(fs::SDFS& sd, const char* filename = nullptr);
  esp_err_t oneShot(fs::SDMMCFS& mmc, const char* filename = nullptr);
  esp_err_t timerShot(const unsigned long period, fs::SDFS& sd, const char* filenamePrefix = nullptr);
  esp_err_t timerShot(const unsigned long period, fs::SDMMCFS& mmc, const char* filenamePrefix = nullptr);
  void      disableTimerShot(void);
  void      enableTimerShot(void);
  static void deq(void);
  static portBASE_TYPE  enq(TickType_t ms);

 protected:
  typedef enum {
    MOUNT_NONE,
    MOUNT_SD,
    MOUNT_SDMMC
  } SDType_t;

  bool      _autoMount(const SDType_t sdType, fs::FS* sdFile);
  esp_err_t _oneShot(const char* filename);
  void      _timerShot(const unsigned long period, const char* filename);
  static void IRAM_ATTR _timerShotISR(void);
  static void _timerShotTask(void* pvParametes);
  size_t    _appendTimestamp(char* str);
  esp_err_t _export(const char* filename, camera_fb_t* frameBuffer);
  void      _purgeTimer(void);

  fs::FS*   _sdFile;          /**< Current SD filesystem */
  SDType_t  _mounted;         /**< Type of mounted SD */

 private:
  typedef struct {
    const int8_t  pwdn_gpio_num;
    const int8_t  reset_gpio_num;
    const int8_t  xclk_gpio_num;
    const int8_t  siod_gpio_num;
    const int8_t  sioc_gpio_num;
    const int8_t  y9_gpio_num;
    const int8_t  y8_gpio_num;
    const int8_t  y7_gpio_num;
    const int8_t  y6_gpio_num;
    const int8_t  y5_gpio_num;
    const int8_t  y4_gpio_num;
    const int8_t  y3_gpio_num;
    const int8_t  y2_gpio_num;
    const int8_t  vsync_gpio_num;
    const int8_t  href_gpio_num;
    const int8_t  pclk_gpio_num;
  } _pins_t;

  typedef struct {
    const CameraId  id;       /**< Type of sensors that the esp32 module is equipped with */
    const bool  psram;        /**< PSRAM equipped or not */
    _pins_t     pins;         /**< GPIO pins assignment array */
  } _pinsAssign_t;            /**< Sensor device pins assignment table */

  CameraId      _cameraId;    /**< Type of sensor modules */
  const _pins_t*  _pins;      /**< GPIO pins assignment index */
  bool          _psram;       /**< Whether PSRAM is actually installed or not */
  sensor_t*     _sensor;      /**< The currnet sensor characteristics */
  hw_timer_t*   _hwTimer;     /**< HW Timer for Timer-Shot */
  String  _captureName;       /**< Name of the file where the captured image will be saved. */

  static const _pinsAssign_t _pinsMap[];  // The pin assignment for wiring to the sensor is static.
  static const char _mountProbe[];  // Name of a dummy file to check if the file system is mounted.
};

#endif // !_ESP32CAM_H_
