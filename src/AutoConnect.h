/**
 *	Declaration of AutoConnect class and accompanying AutoConnectConfig class.
 *	@file	AutoConnect.h
 *	@author	hieromon@gmail.com
 *	@version	0.9.9
 *	@date	2019-05-25
 *	@copyright	MIT license.
 */

#ifndef _AUTOCONNECT_H_
#define _AUTOCONNECT_H_

#include <vector>
#include <memory>
#include <functional>
#include <DNSServer.h>
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
extern "C" {
#include <user_interface.h>
}
using WebServerClass = ESP8266WebServer;
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
using WebServerClass = WebServer;
#endif
#include <EEPROM.h>
#include <PageBuilder.h>
#include "AutoConnectDefs.h"
#include "AutoConnectPage.h"
#include "AutoConnectCredential.h"
#include "AutoConnectAux.h"

/**< A type to save established credential at WiFi.begin automatically. */
typedef enum AC_SAVECREDENTIAL {
  AC_SAVECREDENTIAL_NEVER,
  AC_SAVECREDENTIAL_AUTO
} AC_SAVECREDENTIAL_t;

typedef enum AC_ONBOOTURI {
  AC_ONBOOTURI_ROOT,
  AC_ONBOOTURI_HOME
} AC_ONBOOTURI_t;

class AutoConnectConfig {
 public:
  /**
   *  AutoConnectConfig default constructor.
   *  SSID for the captive portal access point assumes AUTOCONNECT_APID which
   *  assigned from macro. Password is same as above too.
   */
  AutoConnectConfig() :
    apip(AUTOCONNECT_AP_IP),
    gateway(AUTOCONNECT_AP_GW),
    netmask(AUTOCONNECT_AP_NM),
    apid(String(AUTOCONNECT_APID)),
    psk(String(AUTOCONNECT_PSK)),
    channel(AUTOCONNECT_AP_CH),
    hidden(0),
    autoSave(AC_SAVECREDENTIAL_AUTO),
    bootUri(AC_ONBOOTURI_ROOT),
    boundaryOffset(AC_IDENTIFIER_OFFSET),
    uptime(AUTOCONNECT_STARTUPTIME),
    autoRise(true),
    autoReset(true),
    autoReconnect(false),
    immediateStart(false),
    retainPortal(false),
    portalTimeout(AUTOCONNECT_CAPTIVEPORTAL_TIMEOUT),
    hostName(String("")),
    homeUri(AUTOCONNECT_HOMEURI),
    title(AUTOCONNECT_MENU_TITLE),
    staip(0U),
    staGateway(0U),
    staNetmask(0U),
    dns1(0U),
    dns2(0U) {}
  /**
   *  Configure by SSID for the captive portal access point and password.
   */
  AutoConnectConfig(const char* ap, const char* password, const unsigned long portalTimeout = 0, const uint8_t channel = AUTOCONNECT_AP_CH) :
    apip(AUTOCONNECT_AP_IP),
    gateway(AUTOCONNECT_AP_GW),
    netmask(AUTOCONNECT_AP_NM),
    apid(String(ap)),
    psk(String(password)),
    channel(channel),
    hidden(0),
    autoSave(AC_SAVECREDENTIAL_AUTO),
    bootUri(AC_ONBOOTURI_ROOT),
    boundaryOffset(AC_IDENTIFIER_OFFSET),
    uptime(AUTOCONNECT_STARTUPTIME),
    autoRise(true),
    autoReset(true),
    autoReconnect(false),
    immediateStart(false),
    retainPortal(false),
    portalTimeout(portalTimeout),
    hostName(String("")),
    homeUri(AUTOCONNECT_HOMEURI),
    title(AUTOCONNECT_MENU_TITLE),
    staip(0U),
    staGateway(0U),
    staNetmask(0U),
    dns1(0U),
    dns2(0U) {}

  ~AutoConnectConfig() {}

  AutoConnectConfig& operator=(const AutoConnectConfig& o) {
    apip = o.apip;
    gateway = o.gateway;
    netmask = o.netmask;
    apid = o.apid;
    psk = o.psk;
    channel = o.channel;
    hidden = o.hidden;
    autoSave = o.autoSave;
    bootUri = o.bootUri;
    boundaryOffset = o.boundaryOffset;
    uptime = o.uptime;
    autoRise = o.autoRise;
    autoReset = o.autoReset;
    autoReconnect = o.autoReconnect;
    immediateStart = o.immediateStart;
    retainPortal = o.retainPortal;
    portalTimeout = o.portalTimeout;
    hostName = o.hostName;
    homeUri = o.homeUri;
    title = o.title;
    staip = o.staip;
    staGateway = o.staGateway;
    staNetmask = o.staNetmask;
    dns1 = o.dns1;
    dns2 = o.dns2;
    return *this;
  }

  IPAddress apip;               /**< SoftAP IP address */
  IPAddress gateway;            /**< SoftAP gateway address */
  IPAddress netmask;            /**< SoftAP subnet mask */
  String    apid;               /**< SoftAP SSID */
  String    psk;                /**< SoftAP password */
  uint8_t   channel;            /**< SoftAP used wifi channel */
  uint8_t   hidden;             /**< SoftAP SSID hidden */
  AC_SAVECREDENTIAL_t  autoSave;  /**< Auto save credential */
  AC_ONBOOTURI_t  bootUri;      /**< An uri invoking after reset */
  uint16_t  boundaryOffset;     /**< The save storage offset of EEPROM */
  int       uptime;             /**< Length of start up time */
  bool      autoRise;           /**< Automatic starting the captive portal */
  bool      autoReset;          /**< Reset ESP8266 module automatically when WLAN disconnected. */
  bool      autoReconnect;      /**< Automatic reconnect with past SSID */
  bool      immediateStart;     /**< Skips WiFi.begin(), start portal immediately */
  bool      retainPortal;       /**< Even if the captive portal times out, it maintains the portal state. */
  unsigned long portalTimeout;  /**< Timeout value for stay in the captive portal */
  String    hostName;           /**< host name */
  String    homeUri;            /**< A URI of user site */
  String    title;              /**< Menu title */
  IPAddress staip;              /**< Station static IP address */
  IPAddress staGateway;         /**< Station gateway address */
  IPAddress staNetmask;         /**< Station subnet mask */
  IPAddress dns1;               /**< Primary DNS server */
  IPAddress dns2;               /**< Secondary DNS server */
};

typedef std::vector<std::reference_wrapper<AutoConnectAux>> AutoConnectAuxVT;

class AutoConnect {
 public:
  AutoConnect();
  AutoConnect(WebServerClass& webServer);
  ~AutoConnect();
  AutoConnectAux* aux(const String& uri) const;
  bool  config(AutoConnectConfig& Config);
  bool  config(const char* ap, const char* password = nullptr);
  void  home(const String& uri);
  bool  begin(void);
  bool  begin(const char* ssid, const char* passphrase = nullptr, unsigned long timeout = AUTOCONNECT_TIMEOUT);
  void  end(void);
  void  handleClient(void);
  void  handleRequest(void);
  WebServerClass& host(void);
  void  join(AutoConnectAux& aux);
  void  join(AutoConnectAuxVT auxVector);
  bool  on(const String& uri, const AuxHandlerFunctionT handler, AutoConnectExitOrder_t order = AC_EXIT_AHEAD);
  String where(void) const { return _auxUri; }

  /** For AutoConnectAux described in JSON */
#ifdef AUTOCONNECT_USE_JSON
  bool  load(PGM_P aux);
  bool  load(const __FlashStringHelper* aux);
  bool  load(const String& aux);
  bool  load(Stream& aux);
#endif // !AUTOCONNECT_USE_JSON

  typedef std::function<bool(IPAddress)>  DetectExit_ft;
  void  onDetect(DetectExit_ft fn);
  void  onNotFound(WebServerClass::THandlerFunction fn);

 protected:
  enum _webServerAllocateType {
    AC_WEBSERVER_PARASITIC,
    AC_WEBSERVER_HOSTED
  };
  typedef enum _webServerAllocateType  AC_WEBSERVER_TYPE;
  typedef enum {
    AC_RECONNECT_SET,
    AC_RECONNECT_RESET
  } AC_STARECONNECT_t;
  void  _initialize(void);
  bool  _config(void);
  void  _startWebServer(void);
  void  _startDNSServer(void);
  void  _handleNotFound(void);
  bool  _loadAvailCredential(void);
  void  _stopPortal(void);
  bool  _classifyHandle(HTTPMethod mothod, String uri);
  void  _handleUpload(const String& requestUri, const HTTPUpload& upload);
  void  _purgePages(void);
  virtual PageElement*  _setupPage(String uri);
#ifdef AUTOCONNECT_USE_JSON
  template<typename T>
  bool  _parseJson(T in);
  bool  _load(JsonVariant& aux);
#endif // !AUTOCONNECT_USE_JSON

  /** Request handlers implemented by Page Builder */
  String  _induceConnect(PageArgument& args);
  String  _induceDisconnect(PageArgument& args);
  String  _induceReset(PageArgument& args);
  String  _invokeResult(PageArgument& args);

  /** For portal control */
  bool  _captivePortal(void);
  bool  _hasTimeout(unsigned long timeout);
  bool  _isIP(String ipStr);
  wl_status_t _waitForConnect(unsigned long timeout);
  void  _waitForEndTransmission(void);
  void  _disconnectWiFi(bool wifiOff);
  void  _setReconnect(const AC_STARECONNECT_t order);

  /** Utilities */
  static uint32_t      _getChipId(void);
  static uint32_t      _getFlashChipRealSize(void);
  static String        _toMACAddressString(const uint8_t mac[]);
  static unsigned int  _toWiFiQuality(int32_t rssi);
  DetectExit_ft        _onDetectExit;
  WebServerClass::THandlerFunction _notFoundHandler;
  size_t               _freeHeapSize;

  /** Servers which works in concert. */
  std::unique_ptr<WebServerClass> _webServer;
  std::unique_ptr<DNSServer>      _dnsServer;
  AC_WEBSERVER_TYPE               _webServerAlloc;

  /**
   *  Dynamically hold one page of AutoConnect menu.
   *  Every time a GET/POST HTTP request occurs, an AutoConnect
   *  menu page corresponding to the URI is generated.
   */
  PageBuilder*  _responsePage;
  PageElement*  _currentPageElement;

  /** Extended pages made up with AutoConnectAux */
  std::unique_ptr<AutoConnectAux> _aux;
  String        _auxUri;        /**< Last accessed AutoConnectAux */
  String        _prevUri;       /**< Previous generated page uri */

  /** Saved configurations */
  AutoConnectConfig     _apConfig;
  struct station_config _credential;
  uint8_t       _hiddenSSIDCount;
  int16_t       _scanCount;
  uint8_t       _connectCh;
  unsigned long _connectTimeout;
  unsigned long _portalAccessPeriod;

  /** The control indicators */
  bool  _rfConnect;             /**< URI /connect requested */
  bool  _rfDisconnect;          /**< URI /disc requested */
  bool  _rfReset;               /**< URI /reset requested */
  wl_status_t   _rsConnect;     /**< connection result */
#ifdef ARDUINO_ARCH_ESP32
  WiFiEventId_t _disconnectEventId; /**< STA disconnection event handler registered id  */
#endif

  /** HTTP header information of the currently requested page. */
  IPAddress     _currentHostIP; /**< host IP address */
  String        _uri;           /**< Requested URI */
  String        _redirectURI;   /**< Redirect destination */
  String        _menuTitle;     /**< Title string of the page */

  /** PegeElements of AutoConnect site. */
  static const char _CSS_BASE[] PROGMEM;
  static const char _CSS_UL[] PROGMEM;
  static const char _CSS_ICON_LOCK[] PROGMEM;
  static const char _CSS_INPUT_BUTTON[] PROGMEM;
  static const char _CSS_INPUT_TEXT[] PROGMEM;
  static const char _CSS_TABLE[] PROGMEM;
  static const char _CSS_SPINNER[] PROGMEM;
  static const char _CSS_LUXBAR[] PROGMEM;
  static const char _ELM_HTML_HEAD[] PROGMEM;
  static const char _ELM_MENU_PRE[] PROGMEM;
  static const char _ELM_MENU_AUX[] PROGMEM;
  static const char _ELM_MENU_POST[] PROGMEM;
  static const char _PAGE_STAT[] PROGMEM;
  static const char _PAGE_CONFIGNEW[] PROGMEM;
  static const char _PAGE_CONNECTING[] PROGMEM;
  static const char _PAGE_OPENCREDT[] PROGMEM;
  static const char _PAGE_SUCCESS[] PROGMEM;
  static const char _PAGE_RESETTING[] PROGMEM;
  static const char _PAGE_DISCONN[] PROGMEM;
  static const char _PAGE_FAIL[] PROGMEM;
  static const char _PAGE_404[] PROGMEM;

  static const struct PageTranserModeST {
    const char*              uri;
    const TransferEncoding_t transMode;
    const size_t             rSize;
  } _pageBuildMode[];

  /** Token handlers for PageBuilder */
  String _token_CSS_BASE(PageArgument& args);
  String _token_CSS_UL(PageArgument& args);
  String _token_CSS_ICON_LOCK(PageArgument& args);
  String _token_CSS_INPUT_BUTTON(PageArgument& args);
  String _token_CSS_INPUT_TEXT(PageArgument& args);
  String _token_CSS_TABLE(PageArgument& args);
  String _token_CSS_SPINNER(PageArgument& args);
  String _token_CSS_LUXBAR(PageArgument& args);
  String _token_HEAD(PageArgument& args);
  String _token_MENU_PRE(PageArgument& args);
  String _token_MENU_AUX(PageArgument& args);
  String _token_MENU_POST(PageArgument& args);
  String _token_ESTAB_SSID(PageArgument& args);
  String _token_WIFI_MODE(PageArgument& args);
  String _token_WIFI_STATUS(PageArgument& args);
  String _token_STATION_STATUS(PageArgument& args);
  String _token_LOCAL_IP(PageArgument& args);
  String _token_SOFTAP_IP(PageArgument& args);
  String _token_GATEWAY(PageArgument& args);
  String _token_NETMASK(PageArgument& args);
  String _token_AP_MAC(PageArgument& args);
  String _token_STA_MAC(PageArgument& args);
  String _token_CHANNEL(PageArgument& args);
  String _token_DBM(PageArgument& args);
  String _token_CPU_FREQ(PageArgument& args);
  String _token_FLASH_SIZE(PageArgument& args);
  String _token_CHIP_ID(PageArgument& args);
  String _token_FREE_HEAP(PageArgument& args);
  String _token_LIST_SSID(PageArgument& args);
  String _token_SSID_COUNT(PageArgument& args);
  String _token_HIDDEN_COUNT(PageArgument& args);
  String _token_OPEN_SSID(PageArgument& args);
  String _token_UPTIME(PageArgument& args);
  String _token_BOOTURI(PageArgument& args);
  String _token_CURRENT_SSID(PageArgument& args);

 private:
  static const  String  _emptyString; /**< An empty string alloaction  **/

#if defined(ARDUINO_ARCH_ESP8266)
  friend class ESP8266WebServer;
#elif defined(ARDUINO_ARCH_ESP32)
  friend class WebServer;
#endif

  friend class AutoConnectAux;
};

#endif  // _AUTOCONNECT_H_
