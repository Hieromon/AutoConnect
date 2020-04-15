/**
 *	Declaration of AutoConnect class and accompanying AutoConnectConfig class.
 *	@file	AutoConnect.h
 *	@author	hieromon@gmail.com
 *	@version	1.1.5
 *	@date	2020-04-01
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
#include "AutoConnectTicker.h"
#include "AutoConnectAux.h"

// The realization of AutoConnectOTA is effective only by the explicit
#include "AutoConnectOTA.h"
class AutoConnectOTA;  // Reference to avoid circular

// The realization of AutoConnectUpdate is effective only by the explicit
// definition of AUTOCONNECT_USE_UPDATE
#include "AutoConnectUpdate.h"
class AutoConnectUpdate;  // Reference to avoid circular

/**< A type to save established credential at WiFi.begin automatically. */
typedef enum AC_SAVECREDENTIAL {
  AC_SAVECREDENTIAL_NEVER,
  AC_SAVECREDENTIAL_AUTO
} AC_SAVECREDENTIAL_t;

/**< URI that can be specified to AutoConnectConfig::bootUri. */
typedef enum AC_ONBOOTURI {
  AC_ONBOOTURI_ROOT,
  AC_ONBOOTURI_HOME
} AC_ONBOOTURI_t;

/** WiFi connection principle, it specifies the order of WiFi connecting with saved credentials. */
typedef enum AC_PRINCIPLE {
  AC_PRINCIPLE_RECENT,
  AC_PRINCIPLE_RSSI
} AC_PRINCIPLE_t;

/**< An enumerated type of the designated menu items. */
typedef enum AC_MENUITEM {
  AC_MENUITEM_NONE       = 0x0000,
  AC_MENUITEM_CONFIGNEW  = 0x0001,
  AC_MENUITEM_OPENSSIDS  = 0x0002,
  AC_MENUITEM_DISCONNECT = 0x0004,
  AC_MENUITEM_RESET      = 0x0008,
  AC_MENUITEM_HOME       = 0x0010,
  AC_MENUITEM_UPDATE     = 0x0020,
  AC_MENUITEM_DEVINFO    = 0x0040
} AC_MENUITEM_t;

/**< Specifier for using built-in OTA */
typedef enum AC_OTA {
  AC_OTA_EXTRA,
  AC_OTA_BUILTIN
} AC_OTA_t;

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
    minRSSI(AUTOCONNECT_MIN_RSSI),
    autoSave(AC_SAVECREDENTIAL_AUTO),
    bootUri(AC_ONBOOTURI_ROOT),
    principle(AC_PRINCIPLE_RECENT),
    boundaryOffset(AC_IDENTIFIER_OFFSET),
    uptime(AUTOCONNECT_STARTUPTIME),
    autoRise(true),
    autoReset(true),
    autoReconnect(false),
    immediateStart(false),
    retainPortal(false),
    portalTimeout(AUTOCONNECT_CAPTIVEPORTAL_TIMEOUT),
    menuItems(AC_MENUITEM_CONFIGNEW | AC_MENUITEM_OPENSSIDS | AC_MENUITEM_DISCONNECT | AC_MENUITEM_RESET | AC_MENUITEM_UPDATE | AC_MENUITEM_HOME),
    ticker(false),
    tickerPort(AUTOCONNECT_TICKER_PORT),
    tickerOn(LOW),
    ota(AC_OTA_EXTRA),
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
    minRSSI(AUTOCONNECT_MIN_RSSI),
    autoSave(AC_SAVECREDENTIAL_AUTO),
    bootUri(AC_ONBOOTURI_ROOT),
    principle(AC_PRINCIPLE_RECENT),
    boundaryOffset(AC_IDENTIFIER_OFFSET),
    uptime(AUTOCONNECT_STARTUPTIME),
    autoRise(true),
    autoReset(true),
    autoReconnect(false),
    immediateStart(false),
    retainPortal(false),
    portalTimeout(portalTimeout),
    menuItems(AC_MENUITEM_CONFIGNEW | AC_MENUITEM_OPENSSIDS | AC_MENUITEM_DISCONNECT | AC_MENUITEM_RESET | AC_MENUITEM_UPDATE | AC_MENUITEM_HOME),
    ticker(false),
    tickerPort(AUTOCONNECT_TICKER_PORT),
    tickerOn(LOW),
    ota(AC_OTA_EXTRA),
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
    minRSSI=o.minRSSI;
    autoSave = o.autoSave;
    bootUri = o.bootUri;
    principle = o.principle;
    boundaryOffset = o.boundaryOffset;
    uptime = o.uptime;
    autoRise = o.autoRise;
    autoReset = o.autoReset;
    autoReconnect = o.autoReconnect;
    immediateStart = o.immediateStart;
    retainPortal = o.retainPortal;
    portalTimeout = o.portalTimeout;
    menuItems = o.menuItems;
    ticker = o.ticker;
    tickerPort = o.tickerPort;
    tickerOn = o.tickerOn;
    ota = o.ota;
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
  int16_t   minRSSI;            /**< Lowest WiFi signal strength (RSSI) that can be connected. */
  AC_SAVECREDENTIAL_t  autoSave;  /**< Auto save credential */
  AC_ONBOOTURI_t  bootUri;      /**< An uri invoking after reset */
  AC_PRINCIPLE_t  principle;    /**< WiFi connection principle */  
  uint16_t  boundaryOffset;     /**< The save storage offset of EEPROM */
  int       uptime;             /**< Length of start up time */
  bool      autoRise;           /**< Automatic starting the captive portal */
  bool      autoReset;          /**< Reset ESP8266 module automatically when WLAN disconnected. */
  bool      autoReconnect;      /**< Automatic reconnect with past SSID */
  bool      immediateStart;     /**< Skips WiFi.begin(), start portal immediately */
  bool      retainPortal;       /**< Even if the captive portal times out, it maintains the portal state. */
  unsigned long portalTimeout;  /**< Timeout value for stay in the captive portal */
  uint16_t  menuItems;          /**< A compound value of the menu items to be attached */
  bool      ticker;             /**< Drives LED flicker according to WiFi connection status. */
  uint8_t   tickerPort;         /**< GPIO for flicker */
  uint8_t   tickerOn;           /**< A signal for flicker turn on */
  AC_OTA_t  ota;                /**< Attach built-in OTA */
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
  inline void enableMenu(const uint16_t items) { _apConfig.menuItems |= items; }
  inline void disableMenu(const uint16_t items) { _apConfig.menuItems &= (0xffff ^ items); }

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
  typedef enum {
    AC_RECONNECT_SET,
    AC_RECONNECT_RESET
  } AC_STARECONNECT_t;
  bool  _config(void);
  bool  _configSTA(const IPAddress& ip, const IPAddress& gateway, const IPAddress& netmask, const IPAddress& dns1, const IPAddress& dns2);
  String _getBootUri(void);
  bool  _getConfigSTA(station_config_t* config);
  void  _startWebServer(void);
  void  _startDNSServer(void);
  void  _handleNotFound(void);
  bool  _loadAvailCredential(const char* ssid, const AC_PRINCIPLE_t principle = AC_PRINCIPLE_RECENT, const bool excludeCurrent = false);
  void  _stopPortal(void);
  bool  _classifyHandle(HTTPMethod mothod, String uri);
  void  _handleUpload(const String& requestUri, const HTTPUpload& upload);
  void  _purgePages(void);
  virtual PageElement*  _setupPage(String& uri);
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
  String               _attachMenuItem(const AC_MENUITEM_t item);
  static uint32_t      _getChipId(void);
  static uint32_t      _getFlashChipRealSize(void);
  static String        _toMACAddressString(const uint8_t mac[]);
  static unsigned int  _toWiFiQuality(int32_t rssi);
  DetectExit_ft        _onDetectExit;
  WebServerClass::THandlerFunction _notFoundHandler;
  size_t               _freeHeapSize;

  /** Servers which works in concert. */
  typedef std::unique_ptr<WebServerClass, std::function<void(WebServerClass *)> > WebserverUP;
  WebserverUP _webServer = WebserverUP(nullptr, std::default_delete<WebServerClass>());
  std::unique_ptr<DNSServer>      _dnsServer;

  /**
   *  Dynamically hold one page of AutoConnect menu.
   *  Every time a GET/POST HTTP request occurs, an AutoConnect
   *  menu page corresponding to the URI is generated.
   */
  std::unique_ptr<PageBuilder> _responsePage;
  std::unique_ptr<PageElement> _currentPageElement;

  /** Extended pages made up with AutoConnectAux */
  AutoConnectAux* _aux = nullptr; /**< A top of registered AutoConnectAux */
  String        _auxUri;        /**< Last accessed AutoConnectAux */
  String        _prevUri;       /**< Previous generated page uri */
  /** Available updater, only reset by AutoConnectUpdate::attach is valid */
  std::unique_ptr<AutoConnectUpdate>  _update;
  /** OTA updater */
  std::unique_ptr<AutoConnectOTA>     _ota;

  /** Saved configurations */
  AutoConnectConfig  _apConfig;
  station_config_t   _credential;
  uint8_t       _hiddenSSIDCount;
  int16_t       _scanCount;
  uint8_t       _connectCh;
  unsigned long _connectTimeout;
  unsigned long _portalAccessPeriod;

  /** The control indicators */
  bool  _rfConnect = false;     /**< URI /connect requested */
  bool  _rfDisconnect = false;  /**< URI /disc requested */
  bool  _rfReset = false;       /**< URI /reset requested */
  wl_status_t   _rsConnect;     /**< connection result */
#ifdef ARDUINO_ARCH_ESP32
  WiFiEventId_t _disconnectEventId = -1; /**< STA disconnection event handler registered id  */
#endif
  std::unique_ptr<AutoConnectTicker>  _ticker;  /**< */

  /** HTTP header information of the currently requested page. */
  IPAddress     _currentHostIP; /**< host IP address */
  String        _uri;           /**< Requested URI */
  String        _redirectURI;   /**< Redirect destination */
  String        _menuTitle;     /**< Title string of the page */

  /** PageElements of AutoConnect site. */
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
  String _token_CONFIG_STAIP(PageArgument& args);
  String _token_OPEN_SSID(PageArgument& args);
  String _token_UPTIME(PageArgument& args);
  String _token_BOOTURI(PageArgument& args);
  String _token_CURRENT_SSID(PageArgument& args);

 private:
  static const  String  _emptyString; /**< An empty string allocation  **/

#if defined(ARDUINO_ARCH_ESP8266)
  friend ESP8266WebServer;
#elif defined(ARDUINO_ARCH_ESP32)
  friend class WebServer;
#endif

  friend class AutoConnectAux;
  friend class AutoConnectUpdate;
};

#endif  // _AUTOCONNECT_H_
