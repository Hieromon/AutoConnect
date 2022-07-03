/**
 * Declaration of AutoConnectUpdate class.
 * The AutoConnectUpdate class is a class for updating sketch binary
 * via OTA and inherits the HTTPUpdate class of the arduino core.
 * It declares the class implementations of both core libraries as
 * HTTPUpdateClass to absorb differences between ESP8266 and ESP32
 * class definitions.
 * The AutoConnectUpdate class add ons three features to the HTTPupdate
 * class.
 * 1. Dialog pages for operating the update.
 *   The dialog pages are all AutoConnectAux, and they select available
 *   sketch binary, display during update processing, and display
 *   update results.
 * 2. Dialog pages handler
 *   In the dialog page, AUTOCONNECT_URI_UPDATE, AUTOCONNECT_URI_UPDATE_ACT,
 *   AUTOCONNECT_URI_UPDATE_RESULT are assigned and there is a page
 *   handler for each.
 * 3. Attach to the AutoConnect.
 *   Attaching the AutoConnectUpdate class to AutoConnect makes the
 *   sketch binary update function available, and the operation dialog
 *   pages are incorporated into the AutoConnect menu.
 * @file AutoConnectUpdate.h
 * @author hieromon@gmail.com
 * @version  1.2.3
 * @date 2021-01-23
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTUPDATE_H_
#define _AUTOCONNECTUPDATE_H_

#include "AutoConnectDefs.h"
#ifdef AUTOCONNECT_USE_UPDATE
#ifndef AUTOCONNECT_USE_JSON
#define AUTOCONNECT_USE_JSON
#endif
#include <memory>
#define NO_GLOBAL_HTTPUPDATE
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
using HTTPUpdateClass = ESP8266HTTPUpdate;
#elif defined(ARDUINO_ARCH_ESP32)
#include <HTTPClient.h>
#include <HTTPUpdate.h>
using HTTPUpdateClass = HTTPUpdate;
#endif
// #include <WebSocketsServer.h>
// Quote the true AutoConnectUpdate class according to AUTOCONNECT_USE_UPDATE.
#define AutoConnectUpdate  AutoConnectUpdateAct
#else // !AUTOCONNECT_USE_UPDATE!
#define AutoConnectUpdate  AutoConnectUpdateVoid
#endif
#include "AutoConnect.h"

// Support LED flashing only the board with built-in LED.
#if defined(BUILTIN_LED) || defined(LED_BUILTIN)
#define AC_SETLED(s)    do {setLedPin(AUTOCONNECT_TICKER_PORT, s);} while(0)
#else
#define AC_SETLED(s)    do {} while(0)
#endif

// Indicate an update process loop 
typedef enum AC_UPDATESTATUS {
  UPDATE_RESET,             /**< Update process ended, need to reset */
  UPDATE_IDLE,              /**< Update process has not started */
  UPDATE_START,             /**< Update process has been started */
  UPDATE_PROGRESS,          /**< Update process in progress */
  UPDATE_SUCCESS,           /**< Update successfully completed */
  UPDATE_NOAVAIL,           /**< No available update */
  UPDATE_FAIL               /**< Update fails */
} AC_UPDATESTATUS_t;

class AutoConnectUpdateVoid {
 public:
  explicit AutoConnectUpdateVoid(const String& host = String(""), const uint16_t port = AUTOCONNECT_UPDATE_PORT, const String& uri = String("."), const int timeout = AUTOCONNECT_UPDATE_TIMEOUT, const uint8_t ledOn = LOW) {
    AC_UNUSED(host);
    AC_UNUSED(port);
    AC_UNUSED(uri);
    AC_UNUSED(timeout);
    AC_UNUSED(ledOn);
  }
  AutoConnectUpdateVoid(AutoConnect& portal, const String& host = String(""), const uint16_t port = AUTOCONNECT_UPDATE_PORT, const String& uri = String("."), const int timeout = AUTOCONNECT_UPDATE_TIMEOUT, const uint8_t ledOn = LOW) {
    AC_UNUSED(portal);
    AC_UNUSED(host);
    AC_UNUSED(port);
    AC_UNUSED(uri);
    AC_UNUSED(timeout);
    AC_UNUSED(ledOn);
  }
  virtual ~AutoConnectUpdateVoid() {}
  virtual void  attach(AutoConnect& portal) { AC_UNUSED(portal); }
  virtual void  enable(void) {}
  virtual void  disable(const bool activate = false) { AC_UNUSED(activate); }
  virtual void  handleUpdate(void) {}
  virtual bool  isEnabled(void) { return false; }
  virtual AC_UPDATESTATUS_t  status(void) { return UPDATE_IDLE; }
  virtual AC_UPDATESTATUS_t  update(void) { return UPDATE_IDLE; }
};

#ifdef AUTOCONNECT_USE_UPDATE

class AutoConnectUpdateAct : public AutoConnectUpdateVoid, public HTTPUpdateClass {
 public:
  explicit AutoConnectUpdateAct(const String& host = String(""), const uint16_t port = AUTOCONNECT_UPDATE_PORT, const String& uri = String("."), const int timeout = AUTOCONNECT_UPDATE_TIMEOUT, const uint8_t ledOn = AUTOCONNECT_TICKER_LEVEL)
    : HTTPUpdateClass(timeout), host(host), port(port), uri(uri), _amount(0), _binSize(0), _enable(false), _dialog(UPDATEDIALOG_LOADER), _status(UPDATE_IDLE), _binName(String()), _webServer(nullptr) {
    AC_SETLED(ledOn);       /**< LED blinking during the update that is the default. */
    rebootOnUpdate(false);  /**< Default reboot mode */
  }
  AutoConnectUpdateAct(AutoConnect& portal, const String& host = String(""), const uint16_t port = AUTOCONNECT_UPDATE_PORT, const String& uri = String("."), const int timeout = AUTOCONNECT_UPDATE_TIMEOUT, const uint8_t ledOn = AUTOCONNECT_TICKER_LEVEL)
    : HTTPUpdateClass(timeout), host(host), port(port), uri(uri), _amount(0), _binSize(0), _enable(false), _dialog(UPDATEDIALOG_LOADER), _status(UPDATE_IDLE), _binName(String()), _webServer(nullptr) {
    AC_SETLED(ledOn);
    rebootOnUpdate(false);
    attach(portal);
  }
  ~AutoConnectUpdateAct();
  void  attach(AutoConnect& portal) override;  /**< Attach the update class to AutoConnect */
  void  enable(void) override;       /**< Enable the AutoConnectUpdateAct */
  void  disable(const bool activate = false) override;      /**< Disable the AutoConnectUpdateAct */
  void  handleUpdate(void) override; /**< Behaves the update process */
  bool  isEnabled(void) override { return _auxCatalog ? _auxCatalog->isMenu() : false; } /**< Returns current updater effectiveness */
  AC_UPDATESTATUS_t  status(void) override { return _status; }   /**< reports the current update behavior status */
  AC_UPDATESTATUS_t  update(void) override;    /**< behaves update */

  String    host;           /**< Available URL of Update Server */
  uint16_t  port;           /**< Port number of the update server */
  String    uri;            /**< The path on the update server that contains the sketch binary to be updated */

  // Indicate the type of progress dialog
  typedef enum {
    UPDATEDIALOG_LOADER,    /**< Cyclic loader icon */
    UPDATEDIALOG_METER      /**< Progress meter */
  } AC_UPDATEDIALOG_t;

 protected:
  template<typename T, size_t N> constexpr
  size_t  lengthOf(T(&)[N]) noexcept { return N; }
  void    _buildAux(AutoConnectAux* aux, const AutoConnectAux::ACPage_t* page, const size_t elementNum);
  String  _onCatalog(AutoConnectAux& catalog, PageArgument& args);
  String  _onUpdate(AutoConnectAux& update, PageArgument& args);
  String  _onResult(AutoConnectAux& result, PageArgument& args);
  void    _inProgress(size_t amount, size_t size);  /**< UpdateClass::THandlerFunction_Progress */

  std::unique_ptr<AutoConnectAux> _auxCatalog;   /**< A catalog page for internally generated update binaries */
  std::unique_ptr<AutoConnectAux> _auxProgress;  /**< An update in-progress page */  
  std::unique_ptr<AutoConnectAux> _auxResult;    /**< A update result page  */

  size_t  _amount;              /**< Received amount bytes */
  size_t  _binSize;             /**< Updater binary size */

 private:
  void    _progress(void);      /**< A Handler that returns progress status to the web client */

  bool              _enable;    /**< Validation status of the Update class */
  AC_UPDATEDIALOG_t _dialog;    /**< The type of updating dialog displayed on the client */
  AC_UPDATESTATUS_t _status;    /**< Status of update processing during the cycle of receiving a request */
  String            _binName;   /**< .bin name to update */
  String            _errString; /**< error text reservation */
  WebServerClass*   _webServer; /**< Hosted WebServer for XMLHttpRequest */

  static const AutoConnectAux::ACPage_t         _pageCatalog  PROGMEM;
  static const AutoConnectAux::ACElementProp_t  _elmCatalog[] PROGMEM;

  static const AutoConnectAux::ACPage_t         _pageProgress  PROGMEM;
  static const AutoConnectAux::ACElementProp_t  _elmProgress[] PROGMEM;

  static const AutoConnectAux::ACPage_t         _pageResult  PROGMEM;
  static const AutoConnectAux::ACElementProp_t  _elmResult[] PROGMEM;

#if defined(ARDUINO_ARCH_ESP8266)
  friend ESP8266WebServer;
#elif defined(ARDUINO_ARCH_ESP32)
  friend class WebServer;
#endif
};

#endif // !AUTOCONNECT_USE_UPDATE 
#endif // _AUTOCONNECTUPDATE_H_
