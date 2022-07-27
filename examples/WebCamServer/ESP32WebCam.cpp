/*
  ESP32 Camera Sensor Web interface class implementation.
  Date: 2022-12-28, Updated: 2022-07-27
  
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
  is invoked by the esp32cam class, can coexist with the ESP32 Arduino core
  WebServer library, and can be used in sketches with a web interface, such as
  AutoConnect, by isolating the ports of each server.

  Related information is as follows:
    ESP32 Camera Driver Repository on the GitHub: https://github.com/espressif/esp32-camera
    ESP-IDF HTTP Server documentation:
      https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_http_server.html
*/

#include <esp32-hal.h>
#include <FS.h>
#include <SD.h>
#include <SD_MMC.h>
#include "ESP32WebCam.h"

namespace ESP32WebCam_internal {

ESP32WebCam*  esp32webcam = nullptr;

};

// The ESP32Cam class is for streaming image data captured by ESP32-CAM on the
// Arduino framework via HTTP-based real-time transfer and uses one of the MIMEs,
// multipart/x-mixed-replace. https://www.w3.org/Protocols/rfc1341/7_2_Multipart.html
// The body part of the response is made up of multiple resources in order,
// and the dividing point of resource is defined by the following string.
const char  ESP32WebCam::_ACCESS_CONTROL_ALLOW_ORIGIN[] = "Access-Control-Allow-Origin";
const char  ESP32WebCam::_CONTENT_DISPOSITION[] = "Content-Disposition";
const char  ESP32WebCam::_CONTENT_TYPE[] = "multipart/x-mixed-replace;boundary=%s";
const char  ESP32WebCam::_CONTENT_BOUNDARY[] = "\r\n--%s\r\n";
const char  ESP32WebCam::_CONTENT_PARTHEADER[] = "Content-Type:image/jpeg\r\nContent-Length:%u\r\n\r\n";

/**
 * ESP32WebCam constractor
 * Depending on the sensor device type, determine the ESP32 module's wiring
 * pin assignment.
 * @param  model  The model ID of the sensor to be used.
 */
ESP32WebCam::ESP32WebCam(const uint16_t port) : _port(port), _stream_d(nullptr) {
  _esp32cam.reset(new ESP32Cam);
}

/**
 * ESP32WebCam constractor
 * Depending on the sensor device type, determine the ESP32 module's wiring
 * pin assignment.
 * @param  model  The model ID of the sensor to be used.
 */
ESP32WebCam::ESP32WebCam(const ESP32Cam::CameraId model, const uint16_t port) : _port(port), _stream_d(nullptr) {
  _esp32cam.reset(new ESP32Cam(model));
}

/**
 * ESP32WebCam destructor
 * Stop HTTP Server and purge ESP32Cam instance
 */
ESP32WebCam::~ESP32WebCam() {
  stopCameraServer();
  _esp32cam.reset();
}

/**
 * Shutdown the Web server
 */
void ESP32WebCam::stopCameraServer(void) {
  if (_stream_d) {
    httpd_stop(_stream_d);
    _stream_d = nullptr;
  }
}

/**
 * Start Web server that allows to stream motion JPEG. The port number can be
 * specified.
 * @param  port       HTTP port number
 * @return ESP_OK
 * @return ESP_ERR_HTTPD_TASK
 */
esp_err_t ESP32WebCam::startCameraServer(const uint16_t port) {
  setServerPort(port);
  return startCameraServer(nullptr, nullptr, nullptr);
}

/**
 * Start Web server that allows to stream motion JPEG.
 * Streaming handler, capture handler and port number can be specified.
 * @param  streamPath  Specifies the Path of an endpoint that allows motion JPEG streaming.
 * @param  capturePath Specifies the Path of an endpoint that allows taking a still image.
 * @param  promptPath  Specifies the Path of an endpoint that allows remote controlling.
 * @param  port        HTTP port number
 * @return ESP_OK
 * @return ESP_ERR_HTTPD_TASK
 */
esp_err_t ESP32WebCam::startCameraServer(const char* streamPath, const char* capturePath, const char* promptPath, const uint16_t port) {
  esp_err_t rc;

  if (_stream_d) { // The HTTP server is already up and running.
    log_i("httpd has already started\n");
    return ESP_FAIL;
  }

  // If streamPath is null, the default endpoint will be applied.
  if (!streamPath) {
    if (!_streamPath.length())
      setStreamPath(ESP32CAM_DEFAULT_PATH_STREAM);
  }

  // If capturePath is null, the default endpoint will be applied.
  if (!capturePath) {
    if (!_capturePath.length())
      setCapturePath(ESP32CAM_DEFAULT_PATH_CAPTURE);
  }

  // If promptPath is null, the default endpoint will be applied.
  if (!promptPath) {
    if (!_promptPath.length())
      setPromptPath(ESP32CAM_DEFAULT_PATH_PROMPT);
  }

  // Configure the httpd
  ESP32WebCam_internal::esp32webcam = this;
  httpd_config_t  serverConfig = HTTPD_DEFAULT_CONFIG();
  _port = port;
  serverConfig.server_port = _port;
  // The requested number of handlers is 3 which is occupied by _capture, _prompt, and _stream.
  serverConfig.max_uri_handlers = 3;
  // The default stack size 4096 of the httpd task is insufficient to restart the SD
  // file system. Increase the stack size.
  serverConfig.stack_size = 8192;

  // Start httpd with the port number
  rc = httpd_start(&_stream_d, &serverConfig);

  if (rc == ESP_OK) {
    // Register streamUrl handler
    httpd_uri_t streamUri = {
      .uri = _streamPath.c_str(),
      .method = HTTP_GET,
      .handler = ESP32WebCam::_streamHandler,
      .user_ctx = nullptr,
      .is_websocket = false,
      .handle_ws_control_frames = false,
      .supported_subprotocol = nullptr
    };
    // Register captureUrl handler
    httpd_uri_t captureUri = {
      .uri = _capturePath.c_str(),
      .method = HTTP_GET,
      .handler = ESP32WebCam::_captureHandler,
      .user_ctx = nullptr,
      .is_websocket = false,
      .handle_ws_control_frames = false,
      .supported_subprotocol = nullptr
    };
    // Register promptUrl handler
    httpd_uri_t promptUri = {
      .uri = _promptPath.c_str(),
      .method = HTTP_GET,
      .handler = ESP32WebCam::_promptHandler,
      .user_ctx = nullptr,
      .is_websocket = false,
      .handle_ws_control_frames = false,
      .supported_subprotocol = nullptr
    };
    rc = httpd_register_uri_handler(_stream_d, &captureUri);
    if (rc != ESP_OK)
      log_e("%s handler could not register 0x%04x\n", _capturePath.c_str(), rc);
    rc = httpd_register_uri_handler(_stream_d, &promptUri);
    if (rc != ESP_OK)
      log_e("%s handler could not register 0x%04x\n", _promptPath.c_str(), rc);
    rc = httpd_register_uri_handler(_stream_d, &streamUri);
    if (rc != ESP_OK)
      log_e("%s handler could not register 0x%04x\n", _streamPath.c_str(), rc);
  }
  else
    log_e("httpd could not start 0x%04x\n", rc);

  return rc;
}

/**
 * Responds with a response content consisting of a single body.
 * The response to this function will contain the Access-Control-Allow-Origin: ANY header.
 * @param  req     HTTP request
 * @param  status  HTTP response code to be sent.
 * @param  mime    MIME type of the body to be sent content.
 * @param  body    The content body to be sent.
 * @param  bodyLen Content body length. It is optional.
 */
esp_err_t ESP32WebCam::_sendResponse(httpd_req_t* req, uint16_t status, const char* mime, const char* body, ssize_t bodyLen) {
  const char* cStatus = "";

  switch (status) {
  case 200:
    cStatus = HTTPD_200;
    break;
  case 202:
    cStatus = "202 Accepted";
    break;
  case 400:
    cStatus = HTTPD_400;
    break;
  case 500:
    cStatus = HTTPD_500;
    break;
  }
  if (!bodyLen)
    bodyLen = strlen(body);
  httpd_resp_set_status(req, cStatus);
  httpd_resp_set_type(req, mime);
  httpd_resp_set_hdr(req, ESP32WebCam::_ACCESS_CONTROL_ALLOW_ORIGIN, "*");
  if (status > 299) {
    log_e("%s\n", body);
  }
  return httpd_resp_send(req, body, bodyLen);
}

/**
 * Chunking send converted JPEG image. This function will call back from frame2jpg_cb.
 * It writes the bytes of the output JPEG
 */
size_t ESP32WebCam::_jpgEncodeStream(void* arg, size_t index, const void* data, size_t len) {
  _jpgChunking_t* j = reinterpret_cast<_jpgChunking_t*>(arg);

  if (!index)
    j->len = 0;
  if (httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK)
      return 0;

  j->len += len;
  return len;
}

/**
 * Capturing a still image from the image sensor and transferring it via HTTP
 * content. This function is an internal, static entry and URI handler.
 * If the format of the current sensor is other than JPEG, the response code
 * 200 will be sent along with a simple text message.
 * Also, if the capture of the image frame fails, a 500 response will be
 * returned to the client.
 * @param  req      The httpd_req_t structure passed from httpd.
 * @return ESP_OK   Image data has sent
 * @return ESP_FAIL
 */
esp_err_t ESP32WebCam::_captureHandler(httpd_req_t* req) {
  esp_err_t rc;

  camera_fb_t*  fb = esp_camera_fb_get();
  if (fb) {
    if (fb->format == PIXFORMAT_JPEG) {
      // Specifies it can be displayed inside the Web page.
      httpd_resp_set_hdr(req, ESP32WebCam::_CONTENT_DISPOSITION, "inline");
      rc = _sendResponse(req, 200, "image/jpeg", reinterpret_cast<const char*>(fb->buf), fb->len);
    }
    else {
      // Respond with JPEG converted buffer.
      httpd_resp_set_status(req, HTTPD_200);
      httpd_resp_set_type(req, "image/jpeg");
      httpd_resp_set_hdr(req, ESP32WebCam::_CONTENT_DISPOSITION, "inline");
      httpd_resp_set_hdr(req, ESP32WebCam::_ACCESS_CONTROL_ALLOW_ORIGIN, "*");

      _jpgChunking_t  jChunk = {req, 0};
      rc = frame2jpg_cb(fb, 80, _jpgEncodeStream, &jChunk) ? ESP_OK : ESP_FAIL;
      httpd_resp_send_chunk(req, NULL, 0);
    }
    esp_camera_fb_return(fb);
  }
  else {
    _sendResponse(req, HTTPD_500_INTERNAL_SERVER_ERROR, "text/plain", "failed to esp_camera_fb_get");
    rc = ESP_FAIL;
  }
  return rc;
}

/**
 * Request handler that prompts ESP32Cam function execution by an HTTP request.
 * Support for ESP32Cam function execution is provided by the query string
 * that the HTTP request contains. It also returns an HTTP status of 202 or 400,
 * depending on the result of the instruction execution. The HTTP status 400
 * (Bad Request) is usually generated when there are missing parameters required
 * for the execution instruction in the query, or when there is an error in the
 * syntax.
 * Requests to the prompt handler are valid for a query string of the following form:
 * - Take one-shot and export to SD or SD_MMC with a captured jpeg image file.
 * mf=oneshot[&fs={sd|mmc}][&filename=<FILENAME>]
 * - Periodically shoot and export to SD or SD_MMC with the captured jpeg image files.
 * mf=timershot[&fs={sd|mmc}][&filename=<FILENAME>][&period=<TIME INTERVAL>]
 * - Resume timer for periodically shooting
 * mf=entimer
 * - Suspend timer for periodically shooting
 * mf=distimer
 * - Load the image sensor settings from the NVS
 * mf=load
 * - Save the current settign of image sensor to the NVS
 * mf=save
 */
esp_err_t ESP32WebCam::_promptHandler(httpd_req_t* req) {
  uint16_t  status;
  char  res[16];
  esp_err_t rc = ESP_FAIL;

  size_t  qLen =  httpd_req_get_url_query_len(req);
  if (qLen > 0) {
    char  query[qLen + sizeof('\0')] = { '\0', };
    if ((rc = httpd_req_get_url_query_str(req, query, sizeof(query))) == ESP_OK) {
      char  mf[17] = { '\0', };
      char  fs[6] = { '\0', };
      char  filename[128] = { '\0', };
      char  period[13] = { '\0', };

      httpd_query_key_value(query, ESP32CAM_PROMPT_OPERAND_KEY_MEMBERFUNCTION, mf, sizeof(mf));
      httpd_query_key_value(query, ESP32CAM_PROMPT_OPERAND_KEY_FILESYSTEM, fs, sizeof(fs));
      httpd_query_key_value(query, ESP32CAM_PROMPT_OPERAND_KEY_FILENAME, filename, sizeof(filename));
      httpd_query_key_value(query, ESP32CAM_PROMPT_OPERAND_KEY_PERIOD, period, sizeof(period));

      // Parse query parameters and invoke the specified command
      if (!strcmp(ESP32CAM_PROMPT_OPERAND_VALUE_ONESHOT, mf)) {
        if (!strcmp(ESP32CAM_PROMPT_OPERAND_VALUE_MMC, fs) || !strlen(fs))
          rc = ESP32WebCam_internal::esp32webcam->_esp32cam.get()->oneShot(SD_MMC, filename);
        else if (!strcmp(ESP32CAM_PROMPT_OPERAND_VALUE_SD, fs))
          rc = ESP32WebCam_internal::esp32webcam->_esp32cam.get()->oneShot(SD, filename);
        else
          rc = ESP_ERR_INVALID_ARG;
      }
      else if (!strcmp(ESP32CAM_PROMPT_OPERAND_VALUE_TIMERSHOT, mf)) {
        unsigned long nPeriod = atol(period);
        if (!strcmp(ESP32CAM_PROMPT_OPERAND_VALUE_MMC, fs) || !strlen(fs))
          rc = ESP32WebCam_internal::esp32webcam->_esp32cam.get()->timerShot(nPeriod, SD_MMC, filename);
        else if (!strcmp(ESP32CAM_PROMPT_OPERAND_VALUE_SD, fs))
          rc = ESP32WebCam_internal::esp32webcam->_esp32cam.get()->timerShot(nPeriod, SD, filename);
        else
          rc = ESP_ERR_INVALID_ARG;
      }
      else if (!strcmp(ESP32CAM_PROMPT_OPERAND_VALUE_DISABLETIMER, mf)) {
        ESP32WebCam_internal::esp32webcam->_esp32cam.get()->disableTimerShot();
        rc = ESP_OK;
      }
      else if (!strcmp(ESP32CAM_PROMPT_OPERAND_VALUE_ENABLETIMER, mf)) {
        ESP32WebCam_internal::esp32webcam->_esp32cam.get()->enableTimerShot();
        rc = ESP_OK;
      }
      else if (!strcmp(ESP32CAM_PROMPT_OPERAND_VALUE_LOAD, mf)) {
        rc = ESP32WebCam_internal::esp32webcam->_esp32cam.get()->loadSettings();
      }
      else if (!strcmp(ESP32CAM_PROMPT_OPERAND_VALUE_SAVE, mf)) {
        rc = ESP32WebCam_internal::esp32webcam->_esp32cam.get()->saveSettings();
      }
      else {
        rc = ESP_ERR_NOT_SUPPORTED;
      }
    }
  }

  // Send response as a result of a prompt command
  if (rc == ESP_OK) {
    status = 202;
    strcpy(res, "ESP_OK");
  }
  else {
    status = 400;
    sprintf(res, "ESP_FAIL 0x%04x", rc);
  }
  rc = _sendResponse(req, status, "text/plain", res);

  return rc;
}

/**
 * The URL handler for streaming repeatedly sends the image data captured by
 * the sensor as a series of JPEG frames (Motion-JPEG) of multipart content.
 * @param  req      The httpd_req_t structure passed from httpd.
 * @return ESP_OK   Image data has sent
 * @return ESP_FAIL
 */
esp_err_t ESP32WebCam::_streamHandler(httpd_req_t* req) {
  static const char bcharsnospace[] = "'()+,-./0123456789:=?abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char  boundary[33];
  char  mime[sizeof(_CONTENT_TYPE) + sizeof(boundary)];
  char  contentBoundary[sizeof(boundary) + 6];

  // Assemble a boundary string from pseudo-random numbers.
  srand(esp_timer_get_time());
  uint8_t c = 0;
  while (c < sizeof(boundary) - 1)
    boundary[c++] = bcharsnospace[(int)_random(sizeof(bcharsnospace) - 1)];
  boundary[c] = '\0';
  sprintf(mime, _CONTENT_TYPE, (const char*)boundary);
  sprintf(contentBoundary, _CONTENT_BOUNDARY, (const char*)boundary);

  // Send headers
  // HTTP/1.1 200
  // Content-Type: multipart/x-mixed-replace;boundary={contentBoundary}
  // Access-Control-Allow-Origin: *
  // X-Framerate: 60
  httpd_resp_set_type(req, mime);
  httpd_resp_set_hdr(req, ESP32WebCam::_ACCESS_CONTROL_ALLOW_ORIGIN, "*");
  httpd_resp_set_hdr(req, "X-Framerate", "60");

  // Repeated transmission of captured JPEG frames
  esp_err_t rc = ESP_OK;
  while (rc == ESP_OK) {
    uint8_t*  jpegBuffer;
    size_t    jpegSize;

    // Indicates the output buffer of the image sensor.
    camera_fb_t*  fb = esp_camera_fb_get();

    if (fb) {
      if (fb->format == PIXFORMAT_JPEG) {
        // If the pixel format of the image sensor is JPEG, it can be read
        // directly from the frame buffer.
        jpegBuffer = fb->buf;
        jpegSize = fb->len;
      }
      else {
        // If the scene is not in JPEG format, it will attempt to revert to
        // JPEG, but this does not always work. Since most of the failures are
        // due to lack of memory, you can avoid conversion errors by reducing
        // the JPEG quality of the resulting image.
        bool  jpegcc = frame2jpg(fb, 80, &jpegBuffer, &jpegSize);
        esp_camera_fb_return(fb);
        fb = nullptr;
        if (!jpegcc) {
          _sendResponse(req, 500, "text/plain", "_streamHandler failed to frame2jpg");
          rc = ESP_FAIL;
          break;
        }
      }
    }
    else {
      _sendResponse(req, 500, "text/plain", "_streamHandler failed to esp_camera_fb_get");
      rc = ESP_FAIL;
      break;
    }

    // The body of the content is sent in chunks.
    // Transfer-Encoding: chunked
    // [CRLF]
    // --{contentBoundary}
    // Content-Type: image/jpeg
    // Content-Length: {jpegSize}
    // [CRLF]
    // JPEG Frame - One shot from {jpegBuffer}
    // REPEAT IMMEDIATELY FROM [CRLF]--{contentBoundary}
    if (ESP32Cam::enq(5000) == pdTRUE) {
      if ((rc = httpd_resp_send_chunk(req, contentBoundary, sizeof(contentBoundary))) == ESP_OK) {
        char  partialHeader[sizeof(_CONTENT_PARTHEADER) + 12];
        int   hdrSize = sprintf(partialHeader, _CONTENT_PARTHEADER, jpegSize);
        if ((rc = httpd_resp_send_chunk(req, (const char*)partialHeader, hdrSize)) == ESP_OK) {
          rc = httpd_resp_send_chunk(req, (const char*)jpegBuffer, jpegSize);
        }
      }
      ESP32Cam::deq();
    }

    if (fb) {
      esp_camera_fb_return(fb);
      fb = nullptr;
      jpegBuffer = nullptr;
    }
    else if (jpegBuffer) {
      free(jpegBuffer);
      jpegBuffer = nullptr;
    }
  }

  return rc;
}

/**
 * Generates pseudo-random numbers.
 * This function is only quoted for boundary string generation for multipart
 * content.
 * @param  max  Maximum range of random numbers to be generated.
 * @return A generated random number.
 */
long ESP32WebCam::_random(long max) {
  uint32_t  x = esp_random();
  uint64_t  m = uint64_t(x) * uint64_t(max);
  uint32_t  l = uint32_t(m);
  if (l < max) {
    uint32_t  t = -max;
    if (t >= max) {
      t -= max;
      if (t >= max)
        t %= max;
    }
    while (l < t) {
      x = esp_random();
      m = uint64_t(x) * uint64_t(max);
      l = uint32_t(m);
    }
  }
  return m >> 32;
}
