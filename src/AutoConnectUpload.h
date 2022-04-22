/**
 * The upload wrapper base class definition and the default up-loader
 * class declarations.
 * @file AutoConnectUpload.h
 * @author hieromon@gmail.com
 * @version  1.3.5
 * @date 2022-03-28
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTUPLOAD_H_
#define _AUTOCONNECTUPLOAD_H_

#include <functional>
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
  // Uploading progress status
  typedef enum {
    AC_UPLOAD_IDLE,
    AC_UPLOAD_END,
    AC_UPLOAD_ABORTED,
    AC_UPLOAD_ERROR_OPEN,
    AC_UPLOAD_ERROR_WRITE
  } AC_UPLOADStatus_t;

  // Callback functions to notify the upload status
  typedef std::function<void(void)>     StartExit_ft;
  typedef std::function<void(void)>     EndExit_ft;
  typedef std::function<void(uint8_t)>  ErrorExit_ft;
  typedef std::function<void(unsigned int, unsigned int)> ProgressExit_ft;

  explicit AutoConnectUploadHandler() : _cbStart(NULL), _cbEnd(NULL), _cbError(NULL), _cbProgress(NULL), _status(AC_UPLOAD_IDLE), _ulAmount(0) {}
  virtual ~AutoConnectUploadHandler() {}
  AutoConnectUploadHandler& onStart(StartExit_ft fn) { _cbStart = fn; return *this; };          /**< Register a callback for OTA start */
  AutoConnectUploadHandler& onEnd(EndExit_ft fn) { _cbEnd = fn; return *this; };                /**< Register a callback for OTA end */
  AutoConnectUploadHandler& onError(ErrorExit_ft fn) { _cbError = fn; return *this; };          /**< Register a callback for OTA error */
  AutoConnectUploadHandler& onProgress(ProgressExit_ft fn) { _cbProgress = fn; return *this; }  /**< Register a callback for OTA in progress */
  virtual void upload(const String& requestUri, const HTTPUpload& upload);
  AC_UPLOADStatus_t status(void) { return _status; }

protected:
  virtual bool    _open(const char* filename, const char* mode) = 0;
  virtual size_t  _write(const uint8_t *buf, const size_t size) = 0;
  virtual void    _close(const HTTPUploadStatus status) = 0;
  virtual void    _setError(const char* err);

  // Callback functions to notify the upload status
  StartExit_ft    _cbStart;
  EndExit_ft      _cbEnd;
  ErrorExit_ft    _cbError;
  ProgressExit_ft _cbProgress;  

  AC_UPLOADStatus_t _status;
  size_t  _ulAmount;              /**< Cumulative amount uploaded */
  String  _err;                   /**< Occurred error stamp */
};

#endif // !_AUTOCONNECTUPLOAD_H_
