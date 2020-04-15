/**
 * AutoConnectOTA class implementation.
 * @file   AutoConnectOTA.cpp
 * @author hieromon@gmail.com
 * @version    1.1.5
 * @date   2020-04-09
 * @copyright  MIT license.
 */

#include <functional>
#if defined(ARDUINO_ARCH_ESP8266)
#include <WiFiUdp.h>
#include <Updater.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <Update.h>
#endif
#include <StreamString.h>
#include "AutoConnectOTA.h"
#include "AutoConnectOTAPage.h"

/**
 * A destructor. Release the OTA operation pages.
 */
AutoConnectOTA::~AutoConnectOTA() {
  _auxUpdate.reset(nullptr);
  _auxResult.reset(nullptr);
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
  _binName = String(strchr(filename, '/') + sizeof(char));
#ifdef ARDUINO_ARCH_ESP8266
  WiFiUDP::stopAll();
#endif
  uint32_t  maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
  // It only supports FLASH as a sketch area for updating.
  if (Update.begin(maxSketchSpace, U_FLASH)) {
    if (_tickerPort != -1)
      pinMode(static_cast<uint8_t>(_tickerPort), OUTPUT);
    _status = OTA_START;
    AC_DBG("%s updating start\n", filename);
    return true;
  }
  _setError();
  return false;
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
    wsz = Update.write(const_cast<uint8_t*>(buf), size);
    if (wsz != size)
      _setError();
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
  AC_DBG("OTA update");
  if (!_err.length()) {
    if (status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        _status = OTA_SUCCESS;
        AC_DBG_DUMB(" succeeds, turn to reboot");
      }
      else {
        _setError();
        AC_DBG_DUMB(" flash failed");
      }
    }
    else {
      AC_DBG_DUMB(" aborted");
    }
  }
  AC_DBG_DUMB(". %s\n", _err.c_str());
  if (_err.length())
      Update.end(false);
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
    st = String(F(AUTOCONNECT_TEXT_OTASUCCESS));
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
  result["rc"].value.replace("%d", String(Update.getError()));
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
