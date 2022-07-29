/**
 * The default upload handler implementation.
 * @file AutoConnectUploadImpl.h
 * @author hieromon@gmail.com
 * @version  1.3.6
 * @date 2022-07-27
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTUPLOADIMPL_H_
#define _AUTOCONNECTUPLOADIMPL_H_

#if defined(ARDUINO_ARCH_ESP8266)
#include <core_version.h>
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif
#include <SPI.h>
#include <SD.h>
#include "AutoConnectDefs.h"
#include "AutoConnectUpload.h"
#include "AutoConnectFS.h"
// Types branching to be code commonly for the file system classes with
// ESP8266 and ESP32.
#if defined(ARDUINO_ARCH_ESP8266)
typedef SDClass       SDClassT;   // SD:File system class
typedef File          SDFileT;    // SD:File class
#elif defined(ARDUINO_ARCH_ESP32)
typedef fs::SDFS      SDClassT;
typedef SDFile        SDFileT;
#endif

namespace AutoConnectUtil {
AC_HAS_FUNC(end);

template<typename T>
typename std::enable_if<AutoConnectUtil::has_func_end<T>::value, void>::type end(T* media) {
  AC_DBG("Ensure SD unmounting\n");
  media->end();
}

template<typename T>
typename std::enable_if<!AutoConnectUtil::has_func_end<T>::value, void>::type end(T* media) {
  (void)(media);
}
}

/**
 * Handles the default upload process depending on the upload status.
 * This handler function supports the status of UPLOAD_FILE_START,
 * UPLOAD_FILE_WRITE, UPLOAD_FILE_END and calls open, write and
 * close processing respectively.
 * @param  requestUri A reference to the upload request uri.
 * @param  upload     A reference of HTTPUpload entity.
 */
void AutoConnectUploadHandler::upload(const String& requestUri, const HTTPUpload& upload) {
  AC_UNUSED(requestUri);

  switch (upload.status) {
  case UPLOAD_FILE_START: {
    _status = AC_UPLOAD_IDLE;
    _ulAmount = 0;
    if (_cbStart)   // Notify an OTA status change
      _cbStart();
    String  absFilename = "/" + upload.filename;
    if (!_open(absFilename.c_str(), "w")) {
      _status = AC_UPLOAD_ERROR_OPEN;
      _setError(nullptr);
    }
    break;
  }
  case UPLOAD_FILE_WRITE: {
    size_t  wsz = _write(upload.buf, upload.currentSize);
    if ((int)wsz != -1) {
      _ulAmount += wsz;
      if (_cbProgress)
        _cbProgress(_ulAmount, wsz);
    }
    else {
      _status = AC_UPLOAD_ERROR_WRITE;
      _setError(nullptr);
    }
    break;
  }
  case UPLOAD_FILE_END:
  case UPLOAD_FILE_ABORTED:
    _close(upload.status);
    if (_status == AC_UPLOAD_IDLE) {
      if (upload.status == UPLOAD_FILE_ABORTED) {
        _status = AC_UPLOAD_ABORTED;
        _setError(nullptr);
      }
      else
        _status = AC_UPLOAD_END;
    }
    if (_cbEnd)
      _cbEnd();
    break;
  }
}

/**
 * Save the last upload error code and call the declared error exit.
 * @param err  Error description
 */
void AutoConnectUploadHandler::_setError(const char* err) {
  if (err) {
    AC_DBG("ACFile err: %s\n", err);
    _err = String(err);
  }
  if (_cbError)
    _cbError((uint8_t)_status);
} 

// Default handler for uploading to the standard SPIFFS class embedded in the core.
class AutoConnectUploadFS : public AutoConnectUploadHandler {
 public:
  explicit AutoConnectUploadFS(AutoConnectFS::FS& media) : _media(&media), _mounted(false) {}
  ~AutoConnectUploadFS() { _close(HTTPUploadStatus::UPLOAD_FILE_END); }

 protected:
  bool  _open(const char* filename, const char* mode) override {
    _mounted = AutoConnectFS::_isMounted(_media);
    if (!_mounted) {
      AC_DBG("%s is not mounted, ensure mount\n", AUTOCONNECT_STRING_DEPLOY(AUTOCONNECT_APPLIED_FILESYSTEM));
      if (!_media->begin(AUTOCONNECT_FS_INITIALIZATION)) {
        AC_DBG("%s mount failed\n", AUTOCONNECT_STRING_DEPLOY(AUTOCONNECT_APPLIED_FILESYSTEM));
        return false;
      }
    }
    _file = _media->open(filename, mode);
    return _file != false;      
  }

  size_t  _write(const uint8_t* buf, const size_t size) override {
    if (_file)
      return _file.write(buf, size);
    else
      return -1;
  }

  void  _close(const HTTPUploadStatus status) override {
    AC_UNUSED(status);
    if (_file)
      _file.close();
    if (!_mounted)
      _media->end();
  }

 private:
  AutoConnectFS::FS*  _media;       /**< Actual Filesystem */
  fs::File            _file;        /**< File instace */
  bool                _mounted;     /**< Need to end of the filesystem */
};

// Fix to be compatibility with backward for ESP8266 core 2.5.1 or later
// SD pin assignment for AutoConnectFile
#ifndef AUTOCONNECT_SD_CS
#if defined(ARDUINO_ARCH_ESP8266)
#ifndef SD_CHIP_SELECT_PIN
#define SD_CHIP_SELECT_PIN      SS
#endif
#define AUTOCONNECT_SD_CS       SD_CHIP_SELECT_PIN
#elif defined(ARDUINO_ARCH_ESP32)
#define AUTOCONNECT_SD_CS       SS
#endif
#endif // !AUTOCONNECT_SD_CS
// Derivation of SCK frequency and ensuring SD.begin compatibility
#ifdef ARDUINO_ARCH_ESP8266
#if defined(SD_SCK_HZ)
#define AC_SD_SPEED(s)  SD_SCK_HZ(s)
#else
#define AC_SD_SPEED(s)  s
#endif
#endif

// Default handler for uploading to the standard SD class embedded in the core.
class AutoConnectUploadSD : public AutoConnectUploadHandler {
 public:
  explicit AutoConnectUploadSD(SDClassT& media, const uint8_t cs = AUTOCONNECT_SD_CS, const uint32_t speed = AUTOCONNECT_SD_SPEED) : _media(&media), _cs(cs), _speed(speed) {}
  ~AutoConnectUploadSD() { _close(HTTPUploadStatus::UPLOAD_FILE_END); }

 protected:
  bool  _open(const char* filename, const char* mode) override {
    const char* sdVerify;
#if defined(ARDUINO_ARCH_ESP8266)
    if (_media->begin(_cs, AC_SD_SPEED(_speed))) {
      uint8_t oflag = *mode == 'w' ? FILE_WRITE : FILE_READ;
      uint8_t sdType = _media->type();      
      switch (sdType) {
      case 1: // SD_CARD_TYPE_SD1
        sdVerify = (const char*)"MMC";
        break;
      case 2: // SD_CARD_TYPE_SD2
        sdVerify = (const char*)"SDSC";
        break;
      case 3: // SD_CARD_TYPE_SDHC
        sdVerify = (const char*)"SDHC";
        break;
      default:
        sdVerify = (const char*)"UNKNOWN";
        break;
      }
#elif defined(ARDUINO_ARCH_ESP32)
    if (_media->begin(_cs, SPI, _speed)) {
      const char* oflag = mode;
      uint8_t sdType = _media->cardType();
      switch (sdType) {
      case CARD_NONE:
        sdVerify = (const char*)"No card";
        break;
      case CARD_MMC:
        sdVerify = (const char*)"MMC";
        break;
      case CARD_SD:
        sdVerify = (const char*)"SDSC";
        break;
      case CARD_SDHC:
        sdVerify = (const char*)"SDHC";
        break;
      default:
        sdVerify = (const char*)"UNKNOWN";
        break;
      }
#endif
#ifndef AC_DEBUG
      AC_UNUSED(sdVerify);
#endif
      AC_DBG("%s mounted\n", sdVerify);
      _file = _media->open(filename, oflag);
      return _file != false;
    }
    AC_DBG("SD mount failed\n");
    return false;
  }

  size_t  _write(const uint8_t* buf, const size_t size) override {
    if (_file)
      return _file.write(buf, size);
    else
      return -1;
  }

  void  _close(const HTTPUploadStatus status) override {
    AC_UNUSED(status);
    if (_file)
      _file.close();
    AutoConnectUtil::end<SDClassT>(_media);
  }

 private:
  SDClassT* _media;
  SDFileT   _file;
  uint8_t   _cs;
  uint32_t  _speed;
};

#endif // !_AUTOCONNECTUPLOADIMPL_H_
