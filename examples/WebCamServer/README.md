## WebCamServer

The sketch featuring a UI that involves using AutoConnect together with the [ESP32 Camera Driver](https://github.com/espressif/esp32-camera). It has the convenience of AutoConnect as well.

It is necessary to upload the `webcamview.html` file that the data folder contains to the **SPIFFS** file system of the ESP module. To upload the `webcamview.html` file, use ESP32 Sketch Data Upload tool menu from the Arduino IDE, or the upload facility via esptool.py built into the build systems such as PlatformIO.

In order to adapt this example sketch to your environment, you will need to change the following in the sketch to suit your situation.

- Select an image sensor model that is suitable for the ESP32 module you are using:

```cpp
const ESP32Cam::CameraId  model = ESP32Cam::CameraId::CAMERA_MODEL_AI_THINKER;
// const ESP32Cam::CameraId  model = ESP32Cam::CameraId::CAMERA_MODEL_ESP_EYE;
// const ESP32Cam::CameraId  model = ESP32Cam::CameraId::CAMERA_MODEL_M5STACK_ESP32CAM;
// const ESP32Cam::CameraId  model = ESP32Cam::CameraId::CAMERA_MODEL_M5STACK_NO_PSRAM;
// const ESP32Cam::CameraId  model = ESP32Cam::CameraId::CAMERA_MODEL_M5STACK_PSRAM;
// const ESP32Cam::CameraId  model = ESP32Cam::CameraId::CAMERA_MODEL_M5STACK_UNITCAM;
// const ESP32Cam::CameraId  model = ESP32Cam::CameraId::CAMERA_MODEL_M5STACK_V2_PSRAM;
// const ESP32Cam::CameraId  model = ESP32Cam::CameraId::CAMERA_MODEL_M5STACK_WIDE;
// const ESP32Cam::CameraId  model = ESP32Cam::CameraId::CAMERA_MODEL_TTGO_T_JOURNAL;
// const ESP32Cam::CameraId  model = ESP32Cam::CameraId::CAMERA_MODEL_WROVER_KIT;
```

- Specify the most favorable NTP server for your region:

```cpp
// Required to add the correct local time to the export file name of the
// captured image. This assignment needs to be localized.
// This sketch works even if you omit the NTP server specification. In that
// case, the suffix timestamp of the captured image file is the elapsed time
// since the ESP module was powered on.
const char*  const _tz = "JST-9";
const char*  const _ntp1 = "ntp.nict.jp";
const char*  const _ntp2 = "ntp.jst.mfeed.ad.jp";
```

- Choose the file system properly to fit the SD card interface of the ESP32 module you are using; In typical cases, SD is used for the VSPI interface, and MMC is used for the HS2 interface:

```cpp
//const char*  const _fs = "sd";
const char*  const _fs = "mmc";
```

- Allows you to change the port number of the streaming server:

```cpp
const uint16_t _cameraServerPort = 3000; // Default is 3000.
ESP32WebCam webcam(_cameraServerPort);
```

A detailed how to for this example sketch is provided in the [AutoConnect documentation](https://hieromon.github.io/AutoConnect/esp32cam.html).
