/*
  ESP32 Camera Sensor interface class for Arduino.
  Date: 2021-12-27
  
  Copyright (c) 2021 Hieromon Ikasamo.
  This software is released under the MIT License.
  https://opensource.org/licenses/MIT

  This class is an interface to the ESP32 Camera Driver, and contains an HTTP
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
*/

#ifndef _ESP32WEBCAM_H_
#define _ESP32WEBCAM_H_

#include <memory>
#include "ESP32Cam.h"

// Default HTTP Server port
#ifndef ESP32CAM_DEFAULT_HTTPPORT
#define ESP32CAM_DEFAULT_HTTPPORT     3000
#endif // !ESP32CAM_DEFAULT_HTTPPORT

// Pathname that respond for capturing
#ifndef ESP32CAM_DEFAULT_PATH_CAPTURE
#define ESP32CAM_DEFAULT_PATH_CAPTURE  "/_capture"
#endif // !ESP32CAM_DEFAULT_PATH_CAPTURE

// Pathname that prompts ESP32Cam to invoke the function
#ifndef ESP32CAM_DEFAULT_PATH_PROMPT
#define ESP32CAM_DEFAULT_PATH_PROMPT   "/_prompt"
#endif // !ESP32CAM_DEFAULT_PATH_PROMPT

// Pathname that respond for streaming
#ifndef ESP32CAM_DEFAULT_PATH_STREAM
#define ESP32CAM_DEFAULT_PATH_STREAM   "/_stream"
#endif // !ESP32CAM_DEFAULT_PATH_STREAM

// Definition of operand for remote command to be executed by prompt handler
#define ESP32CAM_PROMPT_OPERAND_KEY_FILENAME        "filename"
#define ESP32CAM_PROMPT_OPERAND_KEY_FILESYSTEM      "fs"
#define ESP32CAM_PROMPT_OPERAND_KEY_PERIOD          "period"
#define ESP32CAM_PROMPT_OPERAND_KEY_MEMBERFUNCTION  "mf"
#define ESP32CAM_PROMPT_OPERAND_VALUE_DISABLETIMER  "distimer"
#define ESP32CAM_PROMPT_OPERAND_VALUE_ENABLETIMER   "entimer"
#define ESP32CAM_PROMPT_OPERAND_VALUE_LOAD          "load"
#define ESP32CAM_PROMPT_OPERAND_VALUE_ONESHOT       "oneshot"
#define ESP32CAM_PROMPT_OPERAND_VALUE_SAVE          "save"
#define ESP32CAM_PROMPT_OPERAND_VALUE_SD            "sd"
#define ESP32CAM_PROMPT_OPERAND_VALUE_MMC           "mmc"
#define ESP32CAM_PROMPT_OPERAND_VALUE_TIMERSHOT     "timershot"

class ESP32WebCam {
 public:
  explicit ESP32WebCam(const uint16_t port = ESP32CAM_DEFAULT_HTTPPORT);
  explicit ESP32WebCam(const ESP32Cam::CameraId model, const uint16_t port = ESP32CAM_DEFAULT_HTTPPORT);
  ~ESP32WebCam();

  esp_err_t  sensorInit(void) { return sensor().init(); }
  esp_err_t  sensorInit(const ESP32Cam::CameraId model) { return sensor().init(model); }
  ESP32Cam&  sensor(void) { return *(_esp32cam.get()); }
  const char* getCapturePath(void) const { return _capturePath.c_str(); }
  const char* getPromptPath(void) const { return _promptPath.c_str(); }
  const char* getStreamPath(void) const { return _streamPath.c_str(); }
  httpd_handle_t  getServerHandle(void) const { return _stream_d; }
  uint16_t  getServerPort(void) const { return _port; }
  void      setServerPort(const uint16_t port) { _port = port; }
  bool      isServerStarted(void) const { return _stream_d != nullptr; }
  void      setCapturePath(const char* path) { _capturePath = path; }
  void      setPromptPath(const char* path) { _promptPath = path; }
  void      setStreamPath(const char* path) { _streamPath = path; }
  esp_err_t startCameraServer(const uint16_t port);
  esp_err_t startCameraServer(void) { return startCameraServer(nullptr, nullptr, nullptr, _port); }
  esp_err_t startCameraServer(const char* streamPath) { return startCameraServer(streamPath, nullptr, nullptr, _port); }
  esp_err_t startCameraServer(const char* streamPath, const uint16_t port) { return startCameraServer(streamPath, nullptr, nullptr, port); }
  esp_err_t startCameraServer(const char* streamPath, const char* capturePath, const char* promptPath) { return startCameraServer(streamPath, capturePath, promptPath, _port); }
  esp_err_t startCameraServer(const char* streamPath, const char* capturePath, const char* promptPath, const uint16_t port);
  void      stopCameraServer(void);

 protected:
  static esp_err_t _captureHandler(httpd_req_t* req);
  static esp_err_t _promptHandler(httpd_req_t* req);
  static esp_err_t _streamHandler(httpd_req_t* req);
  static esp_err_t _sendResponse(httpd_req_t* req, const uint16_t status, const char* mime, const char* body, ssize_t bodyLen = 0);
  std::unique_ptr<ESP32Cam>  _esp32cam;

 private:
  typedef struct {
    httpd_req_t*  req;
    size_t  len;
  } _jpgChunking_t;
  static size_t _jpgEncodeStream(void* arg, size_t index, const void* data, size_t len);
  static long _random(long max);
  uint16_t        _port;      /**< HTTP Server port */
  httpd_handle_t  _stream_d;  /**< HTTP Server it activated */
  String _capturePath;        /**< Pathname to reply with a one-shot JPEG image */
  String _promptPath;         /**< Pathname to prompt the ESP32Cam instance to perform the function */
  String _streamPath;         /**< Pathname to play continuous Motion-JPEG frames. */

  // MIME type and frame boundary for transferring Motion-JPEG frames.
  static const char _ACCESS_CONTROL_ALLOW_ORIGIN[];
  static const char _CONTENT_DISPOSITION[];
  static const char _CONTENT_TYPE[];
  static const char _CONTENT_BOUNDARY[];
  static const char _CONTENT_PARTHEADER[]; 
};

#endif // !_ESP32WEBCAM_H_
