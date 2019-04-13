/**
 * The upload wrapper base class definition and the default up-loader
 * class declarations.
 * @file AutoConnectUpload.h
 * @author hieromon@gmail.com
 * @version  0.9.8
 * @date 2019-03-19
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTUPLOAD_H_
#define _AUTOCONNECTUPLOAD_H_

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#endif

/**
 * Uploader base class. This class is a wrapper for the AutoConnectUpload
 * class, and only the upload member function is implemented.
 */
class AutoConnectUploadHandler {
 public:
  explicit AutoConnectUploadHandler() {}
  virtual ~AutoConnectUploadHandler() {}
  virtual void upload(const String& requestUri, const HTTPUpload& upload);

protected:
  virtual bool   _open(const char* filename, const char* mode) = 0;
  virtual size_t _write(const uint8_t *buf, const size_t size) = 0;
  virtual void   _close(void) = 0;
};

#endif // !_AUTOCONNECTUPLOAD_H_
