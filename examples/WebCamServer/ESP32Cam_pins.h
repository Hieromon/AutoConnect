/*
  A part of ESP32 Camera Sensor interface class for Arduino.
  Date: 2021-12-27

  Copyright (c) 2021 Hieromon Ikasamo.
  This software is released under the MIT License.
  https://opensource.org/licenses/MIT

  Defines the wiring connection between camera sensor signals and GPIO that
  depends on various ESP32 modules. This structure is static and is privately
  deployed by the esp32cam class internally. The sketch does not need to access
  this definition directly, and the esp32cam class constructor will select
  appropriately pin assignment by the specified each ESP32 module.
  
  Related information is as follows:
    ESP32 Camera Driver Repository on the GitHub: https://github.com/espressif/esp32-camera
*/

#ifndef _ESP32CAM_PINS_H_
#define _ESP32CAM_PINS_H_

// WiFi throughput is reduced when certain frequencies are offered from XCLK,
// and we can see this phenomenon at 5MHz, 10MHz, and 20MHz.
// https://github.com/sohtamei/docs/blob/master/ESP32CameraIssue.md
// https://github.com/espressif/esp32-camera/issues/150
// The cause is not clear, but it shifts XCLK phase from the tuned frequency
// to avoid WiFi weakening occurs.
#ifndef ESP32CAM_XCLK_FREQ
#define ESP32CAM_XCLK_FREQ  16500000
// #define ESP32CAM_XCLK_FREQ  20000000
#endif // !ESP32CAM_XCLK_FREQ

const ESP32Cam::_pinsAssign_t ESP32Cam::_pinsMap[] = {
  { CAMERA_MODEL_WROVER_KIT, true, {  // Has PSRAM
    /* PWDN_GPIO_NUM  */  -1,
    /* RESET_GPIO_NUM */  -1,
    /* XCLK_GPIO_NUM  */  21,
    /* SIOD_GPIO_NUM  */  26,
    /* SIOC_GPIO_NUM  */  27,
    /* Y9_GPIO_NUM    */  35,
    /* Y8_GPIO_NUM    */  34,
    /* Y7_GPIO_NUM    */  39,
    /* Y6_GPIO_NUM    */  36,
    /* Y5_GPIO_NUM    */  19,
    /* Y4_GPIO_NUM    */  18,
    /* Y3_GPIO_NUM    */   5,
    /* Y2_GPIO_NUM    */   4,
    /* VSYNC_GPIO_NUM */  25,
    /* HREF_GPIO_NUM  */  23,
    /* PCLK_GPIO_NUM  */  22 }
  },
  { CAMERA_MODEL_ESP_EYE, true, { // Has PSRAM
    /* PWDN_GPIO_NUM  */  -1,
    /* RESET_GPIO_NUM */  -1,
    /* XCLK_GPIO_NUM  */  4,
    /* SIOD_GPIO_NUM  */  18,
    /* SIOC_GPIO_NUM  */  23,
    /* Y9_GPIO_NUM    */  36,
    /* Y8_GPIO_NUM    */  37,
    /* Y7_GPIO_NUM    */  38,
    /* Y6_GPIO_NUM    */  39,
    /* Y5_GPIO_NUM    */  35,
    /* Y4_GPIO_NUM    */  14,
    /* Y3_GPIO_NUM    */  13,
    /* Y2_GPIO_NUM    */  34,
    /* VSYNC_GPIO_NUM */  5,
    /* HREF_GPIO_NUM  */  27,
    /* PCLK_GPIO_NUM  */  25 }
  },
  { CAMERA_MODEL_M5STACK_PSRAM, true, { // Has PSRAM
    /* PWDN_GPIO_NUM  */  -1,
    /* RESET_GPIO_NUM */  15,
    /* XCLK_GPIO_NUM  */  27,
    /* SIOD_GPIO_NUM  */  25,
    /* SIOC_GPIO_NUM  */  23,
    /* Y9_GPIO_NUM    */  19,
    /* Y8_GPIO_NUM    */  36,
    /* Y7_GPIO_NUM    */  18,
    /* Y6_GPIO_NUM    */  39,
    /* Y5_GPIO_NUM    */   5,
    /* Y4_GPIO_NUM    */  34,
    /* Y3_GPIO_NUM    */  35,
    /* Y2_GPIO_NUM    */  32,
    /* VSYNC_GPIO_NUM */  22,
    /* HREF_GPIO_NUM  */  26,
    /* PCLK_GPIO_NUM  */  21 }
  },
  { CAMERA_MODEL_M5STACK_V2_PSRAM, true, { // M5Camera version B Has PSRAM
    /* PWDN_GPIO_NUM  */  -1,
    /* RESET_GPIO_NUM */  15,
    /* XCLK_GPIO_NUM  */  27,
    /* SIOD_GPIO_NUM  */  22,
    /* SIOC_GPIO_NUM  */  23,
    /* Y9_GPIO_NUM    */  19,
    /* Y8_GPIO_NUM    */  36,
    /* Y7_GPIO_NUM    */  18,
    /* Y6_GPIO_NUM    */  39,
    /* Y5_GPIO_NUM    */   5,
    /* Y4_GPIO_NUM    */  34,
    /* Y3_GPIO_NUM    */  35,
    /* Y2_GPIO_NUM    */  32,
    /* VSYNC_GPIO_NUM */  25,
    /* HREF_GPIO_NUM  */  26,
    /* PCLK_GPIO_NUM  */  21 }
  },
  { CAMERA_MODEL_M5STACK_WIDE, true, {  // Has PSRAM
    /* PWDN_GPIO_NUM  */  -1,
    /* RESET_GPIO_NUM */  15,
    /* XCLK_GPIO_NUM  */  27,
    /* SIOD_GPIO_NUM  */  22,
    /* SIOC_GPIO_NUM  */  23,
    /* Y9_GPIO_NUM    */  19,
    /* Y8_GPIO_NUM    */  36,
    /* Y7_GPIO_NUM    */  18,
    /* Y6_GPIO_NUM    */  39,
    /* Y5_GPIO_NUM    */   5,
    /* Y4_GPIO_NUM    */  34,
    /* Y3_GPIO_NUM    */  35,
    /* Y2_GPIO_NUM    */  32,
    /* VSYNC_GPIO_NUM */  25,
    /* HREF_GPIO_NUM  */  26,
    /* PCLK_GPIO_NUM  */  21 }
  },
  { CAMERA_MODEL_M5STACK_ESP32CAM, false, { // No PSRAM
    /* PWDN_GPIO_NUM  */  -1,
    /* RESET_GPIO_NUM */  15,
    /* XCLK_GPIO_NUM  */  27,
    /* SIOD_GPIO_NUM  */  25,
    /* SIOC_GPIO_NUM  */  23,
    /* Y9_GPIO_NUM    */  19,
    /* Y8_GPIO_NUM    */  36,
    /* Y7_GPIO_NUM    */  18,
    /* Y6_GPIO_NUM    */  39,
    /* Y5_GPIO_NUM    */   5,
    /* Y4_GPIO_NUM    */  34,
    /* Y3_GPIO_NUM    */  35,
    /* Y2_GPIO_NUM    */  17,
    /* VSYNC_GPIO_NUM */  22,
    /* HREF_GPIO_NUM  */  26,
    /* PCLK_GPIO_NUM  */  21 }
  },
  { CAMERA_MODEL_M5STACK_UNITCAM, false, {  // No PSRAM
    /* PWDN_GPIO_NUM  */  -1,
    /* RESET_GPIO_NUM */  15,
    /* XCLK_GPIO_NUM  */  27,
    /* SIOD_GPIO_NUM  */  25,
    /* SIOC_GPIO_NUM  */  23,
    /* Y9_GPIO_NUM    */  19,
    /* Y8_GPIO_NUM    */  36,
    /* Y7_GPIO_NUM    */  18,
    /* Y6_GPIO_NUM    */  39,
    /* Y5_GPIO_NUM    */   5,
    /* Y4_GPIO_NUM    */  34,
    /* Y3_GPIO_NUM    */  35,
    /* Y2_GPIO_NUM    */  32,
    /* VSYNC_GPIO_NUM */  22,
    /* HREF_GPIO_NUM  */  26,
    /* PCLK_GPIO_NUM  */  21 }
  },
  { CAMERA_MODEL_AI_THINKER, true, {  // Has PSRAM
    /* PWDN_GPIO_NUM  */  32,
    /* RESET_GPIO_NUM */  -1,
    /* XCLK_GPIO_NUM  */   0,
    /* SIOD_GPIO_NUM  */  26,
    /* SIOC_GPIO_NUM  */  27,
    /* Y9_GPIO_NUM    */  35,
    /* Y8_GPIO_NUM    */  34,
    /* Y7_GPIO_NUM    */  39,
    /* Y6_GPIO_NUM    */  36,
    /* Y5_GPIO_NUM    */  21,
    /* Y4_GPIO_NUM    */  19,
    /* Y3_GPIO_NUM    */  18,
    /* Y2_GPIO_NUM    */   5,
    /* VSYNC_GPIO_NUM */  25,
    /* HREF_GPIO_NUM  */  23,
    /* PCLK_GPIO_NUM  */  22 }
  },
  { CAMERA_MODEL_TTGO_T_JOURNAL, false, { // No PSRAM
    /* PWDN_GPIO_NUM  */   0,
    /* RESET_GPIO_NUM */  15,
    /* XCLK_GPIO_NUM  */  27,
    /* SIOD_GPIO_NUM  */  25,
    /* SIOC_GPIO_NUM  */  23,
    /* Y9_GPIO_NUM    */  19,
    /* Y8_GPIO_NUM    */  36,
    /* Y7_GPIO_NUM    */  18,
    /* Y6_GPIO_NUM    */  39,
    /* Y5_GPIO_NUM    */   5,
    /* Y4_GPIO_NUM    */  34,
    /* Y3_GPIO_NUM    */  35,
    /* Y2_GPIO_NUM    */  17,
    /* VSYNC_GPIO_NUM */  22,
    /* HREF_GPIO_NUM  */  26,
    /* PCLK_GPIO_NUM  */  21 }
  }
};

#endif // !_ESP32CAM_PINS_H_
