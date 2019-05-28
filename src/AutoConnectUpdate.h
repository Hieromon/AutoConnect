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
 * 2. Daialog pages handler
 *   In the dialog page, AUTOCONNECT_URI_UPDATE, AUTOCONNECT_URI_UPDATE_ACT,
 *   AUTOCONNECT_URI_UPDATE_RESULT are assigned and there is a page
 *   handler for each.
 * 3. Attach to the AutoConnect.
 *   Attaching the AutoConnectUpdate class to AutoConnect makes the
 *   sketch binary update function available, and the operation dialog
 *   pages are incorporated into the AutoConnect menu.
 * @file AutoConnectUpdate.h
 * @author hieromon@gmail.com
 * @version  0.9.10
 * @date 2019-05-14
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTUPDATE_H_
#define _AUTOCONNECTUPDATE_H_

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
#include <WebSocketsServer.h>
#include "AutoConnectDefs.h"
#if defined(AUTOCONNECT_USE_UPDATE)
#ifndef AUTOCONNECT_USE_JSON
#define AUTOCONNECT_USE_JSON
#endif
#endif
#include "AutoConnect.h"

// Support LED flashing only the board with built-in LED.
#ifdef LED_BUILTIN
#define UPDATE_SETLED(s)  do {setLedPin(LED_BUILTIN, s);} while(0)
#else
#define UPDATE_SETLED(s)  do {} while(0)
#endif

// Indicate an update process loop 
typedef enum AC_UPDATESTATUS {
  UPDATE_RESET,             /**< Update process ended, need to reset */
  UPDATE_IDLE,              /**< Update process has not started */
  UPDATE_START,             /**< Update process has been started */
  UPDATE_SUCCESS,           /**< Update successfully completed */
  UPDATE_NOAVAIL,           /**< No available update */
  UPDATE_FAIL               /**< Update fails */
} AC_UPDATESTATUS_t;

class AutoConnectUpdate : public HTTPUpdateClass {
 public:
  explicit AutoConnectUpdate(const String& host = String(""), const uint16_t port = AUTOCONNECT_UPDATE_PORT, const String& uri = String("."), const int timeout = AUTOCONNECT_UPDATE_TIMEOUT)
    : HTTPUpdateClass(timeout), host(host), port(port), uri(uri), _status(UPDATE_IDLE), _binName(String()), _period(0) {
    UPDATE_SETLED(LOW);     /**< LED blinking during the update that is the default. */
    rebootOnUpdate(false);  /**< Default reboot mode */
  }
  AutoConnectUpdate(AutoConnect& portal, const String& host = String(""), const uint16_t port = AUTOCONNECT_UPDATE_PORT, const String& uri = String("."), const int timeout = AUTOCONNECT_UPDATE_TIMEOUT)
    : HTTPUpdateClass(timeout), host(host), port(port), uri(uri), _status(UPDATE_IDLE), _binName(String()), _period(0) {
    UPDATE_SETLED(LOW);
    rebootOnUpdate(false);
    attach(portal);
  }
  ~AutoConnectUpdate();
  void  attach(AutoConnect& portal);  /**< Attach the update class to AutoConnect */
  void  enable(void);       /**< Enable the AutoConnectUpdate */
  void  disable(void);      /**< Disable the AutoConnectUpdate */
  void  handleUpdate(void); /**< Behaves the update process */
  bool  isEnable(void) { return _catalog ? _catalog->isMenu() : false; } /**< Returns current updater effectiveness */
  AC_UPDATESTATUS_t  status(void) { return _status; }   /**< reports the current update behavior status */
  AC_UPDATESTATUS_t  update(void);    /**< behaves update */

  String    host;           /**< Available URL of Update Server */
  uint16_t  port;           /**< Port number of the update server */
  String    uri;            /**< The path on the update server that contains the sketch binary to be updated */

  //
  typedef enum {
    UPDATEDIALOG_LOADER,
    UPDATEDIALOG_METER
  } AC_UPDATEDIALOG_t;

 protected:
  // Attribute definition of the element to be placed on the update page.
  typedef struct {
    const ACElement_t type;
    const char*  name;      /**< Name to assign to AutoConenctElement */
    const char*  value;     /**< Value owned by an element */
    const char*  peculiar;  /**< Specific ornamentation for the element */
  } ACElementProp_t;

  // Attributes to treat included update pages as AutoConnectAux.
  typedef struct {
    const char*  uri;       /**< URI for the page */
    const char*  title;     /**< Menut title of update page */
    const bool   menu;      /**< Whether to display in menu */
    const ACElementProp_t* element;
  } ACPage_t;

  template<typename T, size_t N> constexpr
  size_t  lengthOf(T(&)[N]) noexcept { return N; }
  void    _buildAux(AutoConnectAux* aux, const AutoConnectUpdate::ACPage_t* page, const size_t elementNum);
  String  _onCatalog(AutoConnectAux& catalog, PageArgument& args);
  String  _onUpdate(AutoConnectAux& update, PageArgument& args);
  String  _onResult(AutoConnectAux& result, PageArgument& args);
  void    _wsEvent(uint8_t client, WStype_t event, uint8_t* payload, size_t length);
  void    _inProgress(size_t amount, size_t size);

  std::unique_ptr<AutoConnectAux> _catalog;   /**< A catalog page for internally generated update binaries */
  std::unique_ptr<AutoConnectAux> _progress;  /**< An update in-progress page */  
  std::unique_ptr<AutoConnectAux> _result;    /**< A update result page  */

  std::unique_ptr<WebSocketsServer> _ws;      /**< Reports the update progress measure */
  uint8_t _wsClient;                          /**< WebSocket client id */
  bool    _wsConnected;                       /**< WebSocket connection status */
  size_t  _amount;                            /**< Received amound bytes */
  size_t  _binSize;                           /**< Updater binary size */

 private:
  AC_UPDATEDIALOG_t _dialog;                  /**< The type of updating dialog displayed on the client */
  AC_UPDATESTATUS_t _status;                  /**< Status of update processing during the cycle of receiving a request */
  String            _binName;                 /**< .bin name to update */
  unsigned long     _period;                  /**< Duration of WiFiClient holding for the connection with the update server */
  std::unique_ptr<WiFiClient> _WiFiClient;    /**< Provide to HTTPUpdate class */

  static const ACPage_t         _auxCatalog   PROGMEM;
  static const ACElementProp_t  _elmCatalog[] PROGMEM;

  static const ACPage_t         _auxProgress   PROGMEM;
  static const ACElementProp_t  _elmProgress[] PROGMEM;

  static const ACPage_t         _auxResult   PROGMEM;
  static const ACElementProp_t  _elmResult[] PROGMEM;
};

#endif // _AUTOCONNECTUPDATE_H_
