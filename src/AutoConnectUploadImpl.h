/**
 * The default upload handler implementation.
 * @file AutoConnectUploadImpl.h
 * @author hieromon@gmail.com
 * @version  0.9.9
 * @date 2019-05-25
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTUPLOADIMPL_H_
#define _AUTOCONNECTUPLOADIMPL_H_

#if defined(ARDUINO_ARCH_ESP8266)
#include <core_version.h>
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <SPIFFS.h>
#endif
#include <SPI.h>
#include <SD.h>
#define FS_NO_GLOBALS
#include <FS.h>

// Types branching to be code commonly for the file system classes with
// ESP8266 and ESP32.
#if defined(ARDUINO_ARCH_ESP8266)
typedef fs::FS        SPIFFST;    // SPIFFS:File system class
typedef fs::File      SPIFileT;   // SPIFFS:File class
typedef SDClass       SDClassT;   // SD:File system class
typedef File          SDFileT;    // SD:File class
#elif defined(ARDUINO_ARCH_ESP32)
typedef fs::SPIFFSFS  SPIFFST;
typedef fs::File      SPIFileT;
typedef fs::SDFS      SDClassT;
typedef SDFile        SDFileT;
#endif

#include "AutoConnectDefs.h"
#include "AutoConnectUpload.h"

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
    String  absFilename = "/" + upload.filename;
    (void)_open(absFilename.c_str(), "w");
    break;
  }
  case UPLOAD_FILE_WRITE:
    (void)_write(upload.buf, (const size_t)upload.currentSize);
    break;
  case UPLOAD_FILE_ABORTED:
  case UPLOAD_FILE_END:
    _close();
    break;
  }
}

// Default handler for uploading to the standard SPIFFS class embedded in the core.
class AutoConnectUploadFS : public AutoConnectUploadHandler {
 public:
  explicit AutoConnectUploadFS(SPIFFST& media) : _media(&media) {}
  ~AutoConnectUploadFS() { _close(); }

 protected:
  bool  _open(const char* filename, const char* mode) override {
#if defined(ARDUINO_ARCH_ESP8266)
    if (_media->begin()) {
#elif defined(ARDUINO_ARCH_ESP32)
    if (_media->begin(true)) {
#endif
      _file = _media->open(filename, mode);
      return _file != false;      
    }
    AC_DBG("SPIFFS mount failed\n");
    return false;
  }

  size_t  _write(const uint8_t* buf, const size_t size) override {
    if (_file)
      return _file.write(buf, size);
    else
      return -1;
  }

  void  _close(void) override {
    if (_file)
      _file.close();
    _media->end();
  }

 private:
  SPIFFST*  _media;
  SPIFileT  _file; 
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
  ~AutoConnectUploadSD() { _close(); }

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

  void  _close(void) override {
    if (_file)
      _file.close();
#if defined(ARDUINO_ARCH_ESP32) || (defined(ARDUINO_ARCH_ESP8266) && (!defined(ARDUINO_ESP8266_RELEASE_2_4_0) && !defined(ARDUINO_ESP8266_RELEASE_2_4_1) && !defined(ARDUINO_ESP8266_RELEASE_2_4_2)))
    _media->end();
#endif
  }

 private:
  SDClassT* _media;
  SDFileT   _file;
  uint8_t   _cs;
  uint32_t  _speed;
};

#endif // !_AUTOCONNECTUPLOADIMPL_H_
