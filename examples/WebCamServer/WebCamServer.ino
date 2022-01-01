/*
  AutoConnect combined with ESP32 Camera Sensor.

  Before running this sketch, you need to upload the webcamview.html file
  contained in the data folder to the ESP32 module using ESP32 upload tool
  with SPIFFS.

  Copyright (c) 2021 Hieromon Ikasamo.
  This software is released under the MIT License.
  https://opensource.org/licenses/MIT

  This example is an interface to the ESP32 Camera Driver, and contains an HTTP
  server that can convert frame data captured by the ESP32 Camera Driver into
  JPEG format and respond as HTTP content.
  The supported camera sensor devices and general limitations follow the ESP32
  Camera Driver specification.

  It is the HTTP Server instance of EDP-IDF that sends out the JPEG image
  frames captured by the camera sensor as HTTP content. The HTTP server, which
  is invoked by ESP32Cam class, can coexist with the ESP32 Arduino core
  WebServer library, and can be used in sketches with a web interface, such as
  AutoConnect, by isolating the ports of each http server.

  Related information is as follows:
    ESP32 Camera Driver Repository on the GitHub:
      https://github.com/espressif/esp32-camera
    ESP-IDF HTTP Server documentation:
      https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_http_server.html

  Note:
    1. AutoConnect v1.3.2 or later is required to experience this sketch.
    2. It is necessary to upload the webcamview.html file that the data folder
       contains to the SPIFFS file system of the ESP module. To upload the
       webcamview.html file, use ESP32 Sketch Data Upload tool menu from the
       Arduino IDE, or the upload facility via esptool.py built into the build
       systems such as PlatformIO.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <AutoConnect.h>
#include "ESP32WebCam.h"

// Image sensor settings page
const char  CAMERA_SETUP_PAGE[] = R"*(
{
  "title": "Camera",
  "uri": "/_setting",
  "menu": true,
  "element": [
    {
      "name": "css",
      "type": "ACStyle",
      "value": ".noorder label{display:inline-block;min-width:150px;padding:5px;} .noorder select{width:160px} .magnify{width:20px}"
    },
    {
      "name": "res",
      "type": "ACSelect",
      "label": "Resolution",
      "option": [
        "UXGA(1600x1200)",
        "SXGA(1280x1024)",
        "XGA(1024x768)",
        "SVGA(800x600)",
        "VGA(640x480)",
        "CIF(400x296)",
        "QVGA(320x240)",
        "HQVGA(240x176)",
        "QQVGA(160x120)"
      ],
      "selected": 4
    },
    {
      "name": "qua",
      "type": "ACRange",
      "label": "Quality",
      "value": 10,
      "min": 10,
      "max": 63,
      "magnify": "infront"
    },
    {
      "name": "con",
      "type": "ACRange",
      "label": "Contrast",
      "value": 0,
      "min": -2,
      "max": 2,
      "magnify": "infront"
    },
    {
      "name": "bri",
      "type": "ACRange",
      "label": "Brightness",
      "value": 0,
      "min": -2,
      "max": 2,
      "magnify": "infront"
    },
    {
      "name": "sat",
      "type": "ACRange",
      "label": "Saturation",
      "value": 0,
      "min": -2,
      "max": 2,
      "magnify": "infront"
    },
    {
      "name": "se",
      "type": "ACSelect",
      "label": "Special Effect",
      "option": [
        "No Effect",
        "Negative",
        "Grayscale",
        "Red Tint",
        "Green Tint",
        "Blue Tint",
        "Sepia"
      ],
      "selected": 1
    },
    {
      "name": "awb",
      "type": "ACCheckbox",
      "label": "AWB",
      "labelposition": "infront",
      "checked": true
    },
    {
      "name": "wbg",
      "type": "ACCheckbox",
      "label": "AWB Gain",
      "labelposition": "infront",
      "checked": true
    },
    {
      "name": "wbm",
      "type": "ACSelect",
      "label": "WB Mode",
      "option": [
        "Auto",
        "Sunny",
        "Cloudy",
        "Office",
        "Home"
      ],
      "selected": 1
    },
    {
      "name": "aec",
      "type": "ACCheckbox",
      "label": "AEC SENSOR",
      "labelposition": "infront",
      "checked": true
    },
    {
      "name": "dsp",
      "type": "ACCheckbox",
      "label": "AEC DSP",
      "labelposition": "infront",
      "checked": true
    },
    {
      "name": "ael",
      "type": "ACRange",
      "label": "AE Level",
      "value": 0,
      "min": -2,
      "max": 2,
      "magnify": "infront"
    },
    {
      "name": "exp",
      "type": "ACRange",
      "label": "Exposure",
      "value": 204,
      "min": 0,
      "max": 1200,
      "magnify": "infront",
      "style": "margin-left:20px;width:110px"
    },
    {
      "name": "agc",
      "type": "ACCheckbox",
      "label": "AGC",
      "labelposition": "infront",
      "checked": true
    },
    {
      "name": "agv",
      "type": "ACRange",
      "label": "AGC Gain (Nx)",
      "value": 5,
      "min": 1,
      "max": 31,
      "magnify": "infront"
    },
    {
      "name": "acl",
      "type": "ACRange",
      "label": "Gain Ceiling (2^)",
      "value": 0,
      "min": 1,
      "max": 7,
      "magnify": "infront"
    },
    {
      "name": "bpc",
      "type": "ACCheckbox",
      "label": "DPC Black",
      "labelposition": "infront",
      "checked": true
    },
    {
      "name": "wpc",
      "type": "ACCheckbox",
      "label": "DPC White",
      "labelposition": "infront",
      "checked": true
    },
    {
      "name": "gma",
      "type": "ACCheckbox",
      "label": "GMA enable",
      "labelposition": "infront",
      "checked": true
    },
    {
      "name": "lec",
      "type": "ACCheckbox",
      "label": "Lense Correction",
      "labelposition": "infront",
      "checked": true
    },
    {
      "name": "hmi",
      "type": "ACCheckbox",
      "label": "H-Mirror",
      "labelposition": "infront"
    },
    {
      "name": "vfl",
      "type": "ACCheckbox",
      "label": "V-Flip",
      "labelposition": "infront"
    },
    {
      "name": "dcw",
      "type": "ACCheckbox",
      "label": "DCW (Downsize EN)",
      "labelposition": "infront"
    },
    {
      "name": "set",
      "type": "ACSubmit",
      "value": "SET",
      "uri": "/set"
    }
  ]
}
)*";

// Transition destination for CAMERA_SETUP_PAGE
// It will invoke the handler as setSensor function for setting the image sensor.
// The `response` is ana attribute added in AutoConnect v1.3.2 to suppress the
// HTTP response from AutoConnect. AutoConnectAux handlers with the
// `"response":false` can return their own HTTP response.
const char  CAMERA_SETUP_EXEC[] = R"*(
{
  "title": "Camera",
  "uri": "/set",
  "response": false,
  "menu": false
}
)*";

/**
 * Specify one of the following supported sensor modules to be actually applied.
 * However, not all of the devices have been fully tested. Activating the timer-
 * shot may cause a WDT RESET during streaming.
 */
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

// AutoConnectAUx entry point
const char*  const _sensorUrl = "/_setting";
const char*  const _setUrl = "/set";

// Endpoint that leads request to the root page to webcamview.html
const char*  const _viewerUrl = "/webcam"; 

// Viewer-UI content
const char*  const _webcamserver_html = "/webcamview.html";

// Specifying the time zone and assigning NTP.
// Required to add the correct local time to the export file name of the
// captured image. This assignment needs to be localized.
// This sketch works even if you omit the NTP server specification. In that
// case, the suffix timestamp of the captured image file is the elapsed time
// since the ESP module was powered on.
const char*  const _tz = "JST-9";
const char*  const _ntp1 = "ntp.nict.jp";
const char*  const _ntp2 = "ntp.jst.mfeed.ad.jp";

// You can change the URL assigned to interface with ESP32WebCam according to
// your needs without the sketch code modification.
// Allows changes to each endpoint by specifying the following build_option in
// the platformio.ini file of the PlatformIO build system
//
// - platformio.ini
//   build_flags =
//     -DESP32CAM_DEFAULT_PATH_CAPTURE='"path you wish to CAPTURE's endpoint"'
//     -DESP32CAM_DEFAULT_PATH_PROMPT='"path you wish to PROMPT's endpoint"'
//     -DESP32CAM_DEFAULT_PATH_STREAM='"path you wish to STREAM's endpoint"'

// Choose the file system properly to fit the SD card interface of the ESP32
// module you are using.
// In typical cases, SD is used for the VSPI interface, and MMC is used for the
// HS2 interface.
//const char*  const _fs = "sd";
const char*  const _fs = "mmc";

// ESP32WebCam instance; Hosts a different web server than the one hosted by
// AutoConect.
// If you want to assign a different port than the default, uncomment the
// following two lines and enable _cameraServerPort value. _cameraServerPort is
// the port on which the http server started by ESP32WebCam will listen.
//const uint16_t _cameraServerPort = 3000; // Default is 3000.
//ESP32WebCam webcam(_cameraServerPort);
ESP32WebCam webcam;

// Declare AutoConnect as usual.
// Routing in the root page and webcamview.html natively uses the request
// handlers of the ESP32 WebServer class, so it explicitly instantiates the
// ESP32 WebServer.
WebServer   portalServer;
AutoConnect portal(portalServer);
AutoConnectConfig config;

// Assemble the query string for webcamview.html according the configuration
// by the sketch.
// The webcamview.html accepts the following query parameters:
// - host=HOSTNAME                            (default: 0.0.0.0)
// - port=PORT_NUMBER                         (default: 3000)
// - stream=STREAMING_PATH                    (default: _stream)
// - capture=CAPTURING_PATH                   (default: _capture)
// - prompt=PROMPTING_PATH_FOR_REMOTE_CONTROL (default: _prompt)
// - setting=AUTOCONNECTAUX_SETTING_PATH      (default: undefined)
// - fs={sd|mmc}                              (default: mmc)
// - ac=AUTOCONNECT_ROOT_PATH                 (default: _ac) 
// - title=VIEWER_PAGE_HEADER_TITLE           (default: undefined)
String viewerUrl(void) {
  String  url = String(_viewerUrl)
              + "?host=" + WiFi.localIP().toString()
              + "&port=" + String(webcam.getServerPort())
              + "&capture=" + String(webcam.getCapturePath())
              + "&prompt=" + String(webcam.getPromptPath())
              + "&stream=" + String(webcam.getStreamPath())
              + "&fs=" + String(_fs)
              + "&ac=" AUTOCONNECT_URI
              + "&setting=" + String(_sensorUrl)
              + "&title=" + String(WiFi.getHostname());
  return url;
}

// Directs the request to the root to webcamview.html.
// This function exists only to assemble the query string.
void handleRootPage(void) {
  portalServer.sendHeader("Location", viewerUrl());
  portalServer.send(302, "text/plain", "");
  portalServer.client().stop();
}

// Read webcamview.html from SPIFFS and send it to the client as response.
// The query string given by handleRootPage is taken over tby the 302-redirect.
void handleViewPage(void) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi is disconnected.");
    Serial.println("Viewer will not be available until connection is restored.");
    portalServer.send(500, "text/plain", "WiFi is not connected. Viewer will not be available until connection is restored.");
    return;
  }

  File viewPage = SPIFFS.open(_webcamserver_html, FILE_READ);
  if (viewPage) {
    portalServer.streamFile(viewPage, "text/html");
    close(viewPage);
  }
  else {
    Serial.printf("Viewer %s could not load, check your data folder and upload it.\n", _webcamserver_html);
    portalServer.send(500, "text/plain", String(_webcamserver_html) + " open failed");
  }
}

// AutoConnectAux handler for CAMERA_SETUP_EXEC
// Read the camera status via the ESP32 Camera Driver and update it with the
// sensor parametes defined in each AutoConnect element in CAMERA_SETUP?PAGE.
String setSensor(AutoConnectAux& aux, PageArgument& args) {
  AutoConnectAux& cameraSetup = *portal.aux(_sensorUrl);
  camera_status_t status;

  // Take over the current settings
  webcam.sensor().getStatus(&status);

  // Framesize
  const String& resolution = cameraSetup["res"].as<AutoConnectSelect>().value();
  if (resolution == "UXGA(1600x1200)")
    status.framesize = FRAMESIZE_UXGA;
  else if (resolution == "SXGA(1280x1024)")
    status.framesize = FRAMESIZE_SXGA;
  else if (resolution == "XGA(1024x768)")
    status.framesize = FRAMESIZE_XGA;
  else if (resolution == "SVGA(800x600)")
    status.framesize = FRAMESIZE_SVGA;
  else if (resolution == "VGA(640x480)")
    status.framesize = FRAMESIZE_VGA;
  else if (resolution == "CIF(400x296)")
    status.framesize = FRAMESIZE_CIF;
  else if (resolution == "QVGA(320x240)")
    status.framesize = FRAMESIZE_QVGA;
  else if (resolution == "HQVGA(240x176)")
    status.framesize = FRAMESIZE_HQVGA;
  else if (resolution == "QQVGA(160x120)")
    status.framesize = FRAMESIZE_QQVGA;

  // Pixel granularity
  status.quality = cameraSetup["qua"].as<AutoConnectRange>().value;

  // Color solid adjustment
  status.contrast = cameraSetup["con"].as<AutoConnectRange>().value;
  status.brightness = cameraSetup["bri"].as<AutoConnectRange>().value;
  status.saturation = cameraSetup["sat"].as<AutoConnectRange>().value;

  // SE
  const String& se = cameraSetup["se"].as<AutoConnectSelect>().value();
  if (se == "No Effect")
    status.special_effect = 0;
  if (se == "Negative")
    status.special_effect = 1;
  if (se == "Grayscale")
    status.special_effect = 2;
  if (se == "Red Tint")
    status.special_effect = 3;
  if (se == "Green Tint")
    status.special_effect = 4;
  if (se == "Blue Tint")
    status.special_effect = 5;
  if (se == "Sepia")
    status.special_effect = 6;

  // White Balance effection
  status.awb = cameraSetup["awb"].as<AutoConnectCheckbox>().checked ? 1 : 0;
  status.awb_gain = cameraSetup["wbg"].as<AutoConnectCheckbox>().checked ? 1 : 0;
  const String& wbMode = cameraSetup["wbm"].as<AutoConnectSelect>().value();
  if (wbMode == "Auto")
    status.wb_mode = 0;
  else if (wbMode == "Sunny")
    status.wb_mode = 1;
  else if (wbMode == "Cloudy")
    status.wb_mode = 2;
  else if (wbMode == "Office")
    status.wb_mode = 3;
  else if (wbMode == "Home")
    status.wb_mode = 4;

  // Automatic Exposure Control, Turn off AEC to set the exposure level.
  status.aec = cameraSetup["aec"].as<AutoConnectCheckbox>().checked ? 1 : 0;
  status.aec2 = cameraSetup["dsp"].as<AutoConnectCheckbox>().checked ? 1 : 0;
  status.ae_level = cameraSetup["ael"].as<AutoConnectRange>().value;
  status.aec_value = cameraSetup["exp"].as<AutoConnectRange>().value;

  // Automatic Gain Control
  status.agc = cameraSetup["agc"].as<AutoConnectCheckbox>().checked ? 1 : 0;
  status.agc_gain = cameraSetup["agv"].as<AutoConnectRange>().value - 1;
  status.gainceiling = cameraSetup["acl"].as<AutoConnectRange>().value - 1;

  // Gamma (GMA) function is to compensate for the non-linear characteristics of
  // the sensor. Raw gamma compensates the image in the RAW domain.
  status.raw_gma = cameraSetup["gma"].as<AutoConnectCheckbox>().checked ? 1 : 0;
  
  // Defect pixel cancellation, Black pixel and White pixel
  status.bpc = cameraSetup["bpc"].as<AutoConnectCheckbox>().checked ? 1 : 0;
  status.wpc = cameraSetup["wpc"].as<AutoConnectCheckbox>().checked ? 1 : 0;

  // Lense correction, According to the area where each pixel is located,
  // the module calculates a gain for the pixel, correcting each pixel with its
  // gain calculated to compensate for the light distribution due to lens curvature.
  status.lenc = cameraSetup["lec"].as<AutoConnectCheckbox>().checked ? 1 : 0;

  // Mirror and Flip
  status.hmirror = cameraSetup["hmi"].as<AutoConnectCheckbox>().checked ? 1 : 0;
  status.vflip = cameraSetup["vfl"].as<AutoConnectCheckbox>().checked ? 1 : 0;

  // Reducing image frame UXGA to XGA
  status.dcw = cameraSetup["dcw"].as<AutoConnectCheckbox>().checked ? 1 : 0;

  // Reflecting the setting values on the sensor
  if (!webcam.sensor().setStatus(status))
    Serial.println("Failed to set camera sensor");

  // Sends a redirect to forward to the root page displaying the streaming from
  // the camera.
  aux.redirect(viewerUrl().c_str());

  return String();
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
 
  // Start SD or SD_MMC to save the captured image to the SD card equipped with
  // the ESP32-CAM module. SD.begin() or SD_MMC.begin() does not need to be
  // stated explicitly; the ESP32Cam class will internally detect the SD card
  // mount status and automatically start SD or SD_MMC if it is not mounted.
  // However, starting SD or SD_MMC beforehand will speed up the response to
  // saving the capture file and avoid session timeouts.
  // SD_MMC.begin(); // or SD.begin();

  // Note that this sketch places the viewer HTML page in SPIFFS, so run
  // SPIFFS.begin before starting Web Server.
  SPIFFS.begin();

  // Initialize the image sensor during the start phase of the sketch.
  esp_err_t err = webcam.sensorInit(model);
  if (err != ESP_OK)
    Serial.printf("Camera init failed 0x%04x\n", err);

  // Loading the image sensor configurarion UI provided by AutoConnectAux.
  portal.load(FPSTR(CAMERA_SETUP_PAGE));
  if (portal.load(FPSTR(CAMERA_SETUP_EXEC)))
    portal.on(_setUrl, setSensor);

  // The various configuration settings of AutoConnect are also useful for
  // using the ESP32WebCam class.
  config.autoReconnect = true;
  config.reconnectInterval = 1;
  config.ota = AC_OTA_BUILTIN;
  portal.config(config);

  if (portal.begin()) {
    // Allow hostname.local reach in the local segment by mDNS.
    MDNS.begin(WiFi.getHostname());
    MDNS.addService("http", "tcp", 80);
    Serial.printf("AutoConnect portal %s.local started\n", WiFi.getHostname());

    // By configuring NTP, the timestamp appended to the capture filename will
    // be accurate. But this procedure is optional. It does not affect ESP32Cam
    // execution.
    configTzTime(_tz, _ntp1 ,_ntp2);

    // Activate ESP32WebCam while WiFi is connected.
    err = webcam.startCameraServer();

    if (err == ESP_OK) {
      // This sketch has two endpoints. One assigns the root page as the
      // entrance, and the other assigns a redirector to lead to the viewer-UI
      // which we have placed in SPIFFS with the name webcamview.html.
      portalServer.on("/", handleRootPage);
      portalServer.on(_viewerUrl, handleViewPage);
      Serial.printf("Camera streaming server  %s ready, port %u\n", WiFi.localIP().toString().c_str(), webcam.getServerPort());
      Serial.printf("Endpoint Capture:%s, Prompt:%s, Stream:%s\n", webcam.getCapturePath(), webcam.getPromptPath(), webcam.getStreamPath());
    }
    else
      Serial.printf("Camera server start failed 0x%04x\n", err);
  }
}

void loop() {
  // The handleClient is needed for WebServer class hosted with AutoConnect.
  // ESP-IDF Web Server component launched by the ESP32WebCam continues in a
  // separate task.
  portal.handleClient();

  // Allow CPU to switch to other tasks.
  delay(1);
}
