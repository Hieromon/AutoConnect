/**
 * AutoConnectOTA class implementation.
 * @file   AutoConnectOTA.cpp
 * @author hieromon@gmail.com
 * @version    1.2.2
 * @date   2020-12-11
 * @copyright  MIT license.
 */

#include <functional>
#include <stdio.h>
#if defined(ARDUINO_ARCH_ESP8266)
#include <WiFiUdp.h>
#include <Updater.h>
#ifdef AUTOCONNECT_UPLOAD_ASFIRMWARE_USE_REGEXP
#include <regex.h>
#endif
#elif defined(ARDUINO_ARCH_ESP32)
extern "C" {
#include <esp_spiffs.h>
}
#include <Update.h>
#ifdef AUTOCONNECT_UPLOAD_ASFIRMWARE_USE_REGEXP
#include <regex>
#include <esp_pthread.h>
#include <thread>
#endif
#endif
#include <StreamString.h>
#include "AutoConnectOTA.h"
#include "AutoConnectOTAPage.h"

// Preserve a valid global Filesystem instance.
// It allows the interface to the actual filesystem for migration to LittleFS.
#ifdef AUTOCONNECT_USE_SPIFFS
namespace AutoConnectFS { SPIFFST &OTAFS = SPIFFS; };
#else
namespace AutoConnectFS { SPIFFST &OTAFS = LittleFS; };
#endif

/**
 * A destructor. Release the OTA operation pages.
 */
AutoConnectOTA::~AutoConnectOTA() {
  _auxUpdate.reset(nullptr);
  _auxResult.reset(nullptr);
}

/**
 * Request authentication with an OTA page access
 * @param  auth  Authentication method
 */
void AutoConnectOTA::authentication(const AC_AUTH_t auth) {
  if (_auxUpdate)
    _auxUpdate->authentication(auth);
}

/**
 * Attach the AutoConnectOTA to hosted AutoConnect which constitutes
 * the update process. This function creates an OTA operation page as
 * AutoConnectAux instance and allows it to receive binary updates.
 * @param  portal A reference of AutoConnect
 */
void AutoConnectOTA::attach(AutoConnect& portal) {
  AutoConnectAux* updatePage;

  updatePage = new AutoConnectAux(String(FPSTR(_pageUpdate.uri)), String(FPSTR(_pageUpdate.title)), _pageUpdate.menu);
  _buildAux(updatePage, &_pageUpdate, lengthOf(_elmUpdate));
  _auxUpdate.reset(updatePage);

  updatePage = new AutoConnectAux(String(FPSTR(_pageResult.uri)), String(FPSTR(_pageResult.title)), _pageResult.menu);
  _buildAux(updatePage, &_pageResult, lengthOf(_elmResult));
  _auxResult.reset(updatePage);

  _auxResult->on(std::bind(&AutoConnectOTA::_updated, this, std::placeholders::_1, std::placeholders::_2));
  _auxResult->onUpload<AutoConnectOTA>(*this);

  portal.join(*_auxUpdate.get());
  portal.join(*_auxResult.get());
}

/**
 * Create the update operation pages using a predefined page structure
 * with two structures as ACPage_t and ACElementProp_t which describe
 * for AutoConnectAux configuration.
 * This function receives instantiated AutoConnectAux, instantiates
 * defined AutoConnectElements by ACPage_t, and configures it into
 * received AutoConnectAux.
 * @param  aux        An instantiated AutoConnectAux that will configure according to ACPage_t.
 * @param  page       Pre-defined ACPage_t
 * @param  elementNum Number of AutoConnectElements to configure.  
 */
void AutoConnectOTA::_buildAux(AutoConnectAux* aux, const AutoConnectOTA::ACPage_t* page, const size_t elementNum) {
  for (size_t n = 0; n < elementNum; n++) {
    if (page->element[n].type == AC_Button) {
      AutoConnectButton* element = new AutoConnectButton;
      element->name = String(FPSTR(page->element[n].name));
      if (page->element[n].value)
        element->value = String(FPSTR(page->element[n].value));
      if (page->element[n].peculiar)
        element->action = String(FPSTR(page->element[n].peculiar));
      aux->add(reinterpret_cast<AutoConnectElement&>(*element));
    }
    else if (page->element[n].type == AC_Element) {
      AutoConnectElement* element = new AutoConnectElement;
      element->name = String(FPSTR(page->element[n].name));
      if (page->element[n].value)
        element->value = String(FPSTR(page->element[n].value));
      aux->add(reinterpret_cast<AutoConnectElement&>(*element));
    }
    else if (page->element[n].type == AC_File) {
      AutoConnectFile* element = new AutoConnectFile;
      element->name = String(FPSTR(page->element[n].name));
      element->label = String(FPSTR(page->element[n].peculiar));
      element->store = ACFile_t::AC_File_Extern;
      aux->add(reinterpret_cast<AutoConnectElement&>(*element));
    }
    else if (page->element[n].type ==  AC_Style) {
      AutoConnectStyle* element = new AutoConnectStyle;
      element->name = String(FPSTR(page->element[n].name));
      if (page->element[n].value)
        element->value = String(FPSTR(page->element[n].value));
      aux->add(reinterpret_cast<AutoConnectElement&>(*element));
    }
    else if (page->element[n].type == AC_Text) {
      AutoConnectText* element = new AutoConnectText;
      element->name = String(FPSTR(page->element[n].name));
      if (page->element[n].value)
        element->value = String(FPSTR(page->element[n].value));
      if (page->element[n].peculiar)
        element->style = String(FPSTR(page->element[n].peculiar));
      aux->add(reinterpret_cast<AutoConnectText&>(*element));
    }
  }
}

/**
 * Check the space needed for the update
 * This function overrides AutoConnectUploadHandler::_open.
 * @param  filename An updater bin file name
 * @param  mode     File access mode, but it is not be used.
 * @return true     Ready for update
 * @return false    Not enough FLASH space to update.
 */
bool AutoConnectOTA::_open(const char* filename, const char* mode) {
  AC_UNUSED(mode);
  bool  bc;

  _binName = String(filename + sizeof('/'));
#ifdef ARDUINO_ARCH_ESP8266
  WiFiUDP::stopAll();
#endif

// Uploading filename extension used as the criterion for uploading
// destination is fixed as the default that is assigned by
// AUTOCONNECT_UPLOAD_ASFIRMWARE.
// AUTOCONNECT_UPLOAD_ASFIRMWARE_USE_REGEXP allows you to use regular
// expressions with the extension to determine the uploading destination.
#ifndef AUTOCONNECT_UPLOAD_ASFIRMWARE_USE_REGEXP
  String  asBin(F(AUTOCONNECT_UPLOAD_ASFIRMWARE));
  _binName.toLowerCase();
  asBin.toLowerCase();
  _dest = _binName.endsWith(asBin) == true ? OTA_DEST_FIRM : OTA_DEST_FILE;
#else
// It is recommended to avoid introducing regular expressions in the
// judgment to make the file a firmware upload as much as possible.
#if defined(ARDUINO_ARCH_ESP8266)
  regex_t preg;
  if (!regcomp(&preg, AUTOCONNECT_UPLOAD_ASFIRMWARE, REG_ICASE | REG_EXTENDED | REG_NOSUB)) {
    regmatch_t  p_match[1];
    _dest = !regexec(&preg, _binName.c_str(), 1, p_match, 0) ? OTA_DEST_FIRM : OTA_DEST_FILE;
    regfree(&preg);
  }
  else {
    _setError("regex failed:" AUTOCONNECT_UPLOAD_ASFIRMWARE);
    return false;
  }
#elif defined(ARDUINO_ARCH_ESP32)
// Especially on the ESP32 platform, std::regex fattens the elf, also
// requires many stack area. It easily eats up the FreeRTOS stack
// (8KB by default) allocated to an Arduino loop thread.
// To avoid this, AutoConnectOTA spawns a thread exclusively for regular
// expression evaluation just. But it comes with a heavy price and no meaning.
  esp_pthread_cfg_t t_cfg;
  if (esp_pthread_get_cfg(&t_cfg) != ESP_OK) {
    // t_cfg = esp_pthread_get_default_config();
  }
  t_cfg.stack_size = 1024 * 10;
  esp_pthread_set_cfg(&t_cfg);
  std::thread t_regex([&]() {
    const std::regex  re(std::string(AUTOCONNECT_UPLOAD_ASFIRMWARE), std::regex_constants::icase | std::regex_constants::extended | std::regex_constants::nosubs);
    _dest = std::regex_match(_binName.c_str(), re) ? OTA_DEST_FIRM : OTA_DEST_FILE;
  });
  t_regex.join();
#endif
#endif

  AC_DBG("OTA:%s %s\n", _dest == OTA_DEST_FIRM ? "app" : "fs", _binName.c_str());
  if (_dest == OTA_DEST_FIRM) {
    uint32_t  maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    // It only supports FLASH as a sketch area for updating.
    bc = Update.begin(maxSketchSpace, U_FLASH);
  }
  else {
    // Allocate fs::FS according to the pinned Filesystem.
    _fs = &AutoConnectFS::OTAFS;
#if defined(ARDUINO_ARCH_ESP8266)
    FSInfo  info;
    if (_fs->info(info))
      bc = true;
    else
      bc = _fs->begin();
#elif defined(ARDUINO_ARCH_ESP32)
    if (esp_spiffs_mounted(NULL))
      bc = true;
    else
      bc = _fs->begin(true);
#endif
    if (bc) {
      _file = _fs->open(filename, "w");
      if (!_file) {
        bc = false;
        _setError((_binName + String(F(" open failed"))).c_str());
      }
    }
    else
      _setError("FS mount failed");
  }

  if (bc) {
    if (_tickerPort != -1)
      pinMode(static_cast<uint8_t>(_tickerPort), OUTPUT);
    _status = OTA_START;
    AC_DBG("%s up%s start\n", filename, _dest == OTA_DEST_FIRM ? "dating" : "loading");
  }
  return bc;
}

/**
 * Writes received updater to the flash.
 * This function overrides AutoConnectUploadHandler::_write.
 * @param  buf  Buffer address where received update file was stored.
 * @param  size Size to be written.
 * @return      the amount written
 */
size_t AutoConnectOTA::_write(const uint8_t *buf, const size_t size) {
  size_t  wsz = 0;
  if (_tickerPort != -1)
    digitalWrite(_tickerPort, digitalRead(_tickerPort) ^ 0x01);
  if (!_err.length()) {
    _status = OTA_PROGRESS;

    if (_dest == OTA_DEST_FIRM) {
      wsz = Update.write(const_cast<uint8_t*>(buf), size);
      if (wsz != size)
        _setError();
    }
    else {
      wsz = _file.write(buf, size);
      if (wsz != size)
        _setError("Incomplete writing");
    }
  }
  return wsz;
}

/**
 * All bytes are written, this call writes the config to reboot.
 * If there is an error this will clear everything.
 * This function overrides AutoConnectUploadHandler::_close.
 * @param  status Updater binary upload completion status.
 */
void AutoConnectOTA::_close(const HTTPUploadStatus status) {
  AC_DBG("OTA up%s ", _dest == OTA_DEST_FIRM ? "date" : "load");
  if (!_err.length()) {
    if (status == UPLOAD_FILE_END) {

      bool  ec = true;
      if (_dest == OTA_DEST_FIRM)
        ec = Update.end(true);
      else {
        _file.close();
      }
      if (ec) {
        _status = OTA_SUCCESS;
        AC_DBG_DUMB("succeeds");
      }
      else {
        _setError();
        AC_DBG_DUMB("flash failed");
        Update.end(false);
      }
    }
    else {
      _setError("Aborted");
      AC_DBG_DUMB("aborted");
    }
  }
  AC_DBG_DUMB(". %s\n", _err.c_str());
  if (_tickerPort != -1)
    digitalWrite(_tickerPort, !_tickerOn);
}

/**
 * Callback of the update operation page as AutoConnectAux.
 * Reflect the flash result of Update class to the page.
 * @param  result Upload post-process page
 * @param  args   Unused
 * @return        Empty string
 */
String AutoConnectOTA::_updated(AutoConnectAux& result, PageArgument& args) {
  AC_UNUSED(args);
  PGM_P   stColor;
  String  st;

  // Build an updating result caption.
  // Change the color of the bin name depending on the result of the update.
  if (_status == OTA_SUCCESS) {
    st = _dest == OTA_DEST_FIRM ? String(F(AUTOCONNECT_TEXT_OTASUCCESS)) : String(F(AUTOCONNECT_TEXT_OTAUPLOADED));
    stColor = PSTR("3d7e9a");
    // Notify to the handleClient of loop() thread that it can reboot.
    _status = OTA_RIP;
  }
  else {
    st = String(F(AUTOCONNECT_TEXT_OTAFAILURE)) + _err;
    stColor = PSTR("e66157");
  }
  result["bin"].as<AutoConnectText>().value = _binName;
  result["bin"].as<AutoConnectText>().style += String(stColor);
  result["result"].as<AutoConnectText>().value = st;

  // AutoConnectOTA result page generates a transition after reboot
  // according to the error code from the Update class. By setting the
  // error code of the Update class into the rc element, this page will
  // automatically GET the homepage of the updated sketch.
  // When for firmware updating, it's a numeric and will induce redirect
  // to HOME according to the module restarts.
  // When for file uploading, it's not numeric and has the effect of
  // staying on the upload page.
  result["rc"].value = _dest == OTA_DEST_FIRM ? String(Update.getError()) : String('L');
  return String("");
}

/**
 * Save the last error
 */
void AutoConnectOTA::_setError(void) {
  StreamString  eStr;
  Update.printError(eStr);
  _err = String(eStr.c_str());
  _status = OTA_FAIL;
}

void AutoConnectOTA::_setError(const char* err) {
  _err = String(err);
  _status = OTA_FAIL;
}
