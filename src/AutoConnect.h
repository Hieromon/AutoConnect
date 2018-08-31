/**
 *	Declaration of AutoConnect class and accompanying AutoConnectConfig class.
 *	@file	AutoConnect.h
 *	@author	hieromon@gmail.com
 *	@version	0.9.5
 *	@date	2018-08-27
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
#include "AutoConnectPage.h"
#include "AutoConnectCredential.h"

// Uncomment the following AC_DEBUG to enable debug output.
#define AC_DEBUG

// Debug output destination can be defined externally with AC_DEBUG_PORT
#ifndef AC_DEBUG_PORT
#define AC_DEBUG_PORT Serial
#endif
#ifdef AC_DEBUG
#define AC_DBG(...) do {AC_DEBUG_PORT.print("[AC] "); AC_DEBUG_PORT.printf( __VA_ARGS__ );} while (0)
#else
#define AC_DBG(...)
#endif

#ifndef AUTOCONNECT_APID
#if defined(ARDUINO_ARCH_ESP8266)
#define AUTOCONNECT_APID  "esp8266ap"
#elif defined(ARDUINO_ARCH_ESP32)
#define AUTOCONNECT_APID  "esp32ap"
#endif
#endif

#ifndef AUTOCONNECT_PSK
#define AUTOCONNECT_PSK   "12345678"
#endif

#ifndef AUTOCONNECT_AP_IP
#define AUTOCONNECT_AP_IP 0x01F4A8C0  //*< 192.168.244.1 */
#endif // !AUTOCONNECT_AP_IP
#ifndef AUTOCONNECT_AP_GW
#define AUTOCONNECT_AP_GW 0x01F4A8C0  //*< 192.168.244.1 */
#endif // !AUTOCONNECT_AP_GW
#ifndef AUTOCONNECT_AP_NM
#define AUTOCONNECT_AP_NM 0x00FFFFFF  //*< 255.255.255.0 */
#endif // !AUTOCONNECT_AP_NM

#ifndef AUTOCONNECT_URI
#define AUTOCONNECT_URI         "/_ac"
#endif

#ifndef AUTOCONNECT_HOMEURI
#define AUTOCONNECT_HOMEURI     "/"
#endif

#ifndef AUTOCONNECT_MENU_TITLE
#define AUTOCONNECT_MENU_TITLE  "AutoConnect"
#endif
#define AUTOCONNECT_MENU_TITLE_CONNETED  "Connected"

#define AUTOCONNECT_URI_CONFIG  AUTOCONNECT_URI "/config"
#define AUTOCONNECT_URI_CONNECT AUTOCONNECT_URI "/connect"
#define AUTOCONNECT_URI_RESULT  AUTOCONNECT_URI "/result"
#define AUTOCONNECT_URI_OPEN    AUTOCONNECT_URI "/open"
#define AUTOCONNECT_URI_DISCON  AUTOCONNECT_URI "/disc"
#define AUTOCONNECT_URI_RESET   AUTOCONNECT_URI "/reset"
#define AUTOCONNECT_URI_SUCCESS AUTOCONNECT_URI "/success"
#define AUTOCONNECT_URI_FAIL    AUTOCONNECT_URI "/fail"

#ifndef AUTOCONNECT_TIMEOUT
#define AUTOCONNECT_TIMEOUT     30000
#endif

#ifndef AUTOCONNECT_STARTUPTIME
#define AUTOCONNECT_STARTUPTIME 10
#endif

#ifndef AUTOCONNECT_HTTPPORT
#define AUTOCONNECT_HTTPPORT    80
#endif

#ifndef AUTOCONNECT_DNSPORT
#define AUTOCONNECT_DNSPORT     53
#endif

/**< A type to save established credential at WiFi.begin automatically. */
typedef enum AC_SAVECREDENTIAL {
  AC_SAVECREDENTIAL_NEVER,
  AC_SAVECREDENTIAL_AUTO
} AC_SAVECREDENTIAL_t;

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
    channel(1),
    hidden(0),
    autoSave(AC_SAVECREDENTIAL_AUTO),
    boundaryOffset(AC_IDENTIFIER_OFFSET),
    uptime(AUTOCONNECT_STARTUPTIME),
    autoRise(true),
    autoReset(true),
    autoReconnect(false),
    homeUri(AUTOCONNECT_HOMEURI),
    staip(0U),
    staGateway(0U),
    staNetmask(0U),
    dns1(0U),
    dns2(0U) {}
  /**
   *  Configure by SSID for the captive portal access point and password.
   */
  AutoConnectConfig(const char* ap, const char* password) :
    apip(AUTOCONNECT_AP_IP),
    gateway(AUTOCONNECT_AP_GW),
    netmask(AUTOCONNECT_AP_NM),
    apid(String(ap)),
    psk(String(password)),
    channel(1),
    hidden(0),
    autoSave(AC_SAVECREDENTIAL_AUTO),
    boundaryOffset(AC_IDENTIFIER_OFFSET),
    uptime(AUTOCONNECT_STARTUPTIME),
    autoRise(true),
    autoReset(true),
    autoReconnect(false),
    homeUri(AUTOCONNECT_HOMEURI),
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
    boundaryOffset = o.boundaryOffset;
    uptime = o.uptime;
    autoRise = o.autoRise;
    autoReset = o.autoReset;
    autoReconnect = o.autoReconnect;
    homeUri = o.homeUri;
    staip = o.staip;
    staGateway = o.staGateway;
    staNetmask = o.staNetmask;
    dns1 = o.dns1;
    dns2 = o.dns2;
    return *this;
  }

  IPAddress apip;       /**< SoftAP IP address */
  IPAddress gateway;    /**< SoftAP gateway address */
  IPAddress netmask;    /**< SoftAP subnet mask */
  String    apid;       /**< SoftAP SSID */
  String    psk;        /**< SoftAP password */
  uint8_t   channel;    /**< SoftAP used wifi channel */
  uint8_t   hidden;     /**< SoftAP SSID hidden */
  AC_SAVECREDENTIAL_t  autoSave;  /**< Auto save credential */
  uint16_t  boundaryOffset; /**< The save storage offset of EEPROM */
  int       uptime;     /**< Length of start up time */
  bool      autoRise;   /**< Automatic starting the captive portal */
  bool      autoReset;  /**< Reset ESP8266 module automatically when WLAN disconnected. */
  bool      autoReconnect;  /**< Automatic reconnect with past SSID */
  String    homeUri;    /**< A URI of user site */
  IPAddress staip;      /**< Station static IP address */
  IPAddress staGateway; /**< Station gateway address */
  IPAddress staNetmask; /**< Station subnet mask */
  IPAddress dns1;       /**< Primary DNS server */
  IPAddress dns2;       /**< Secondary DNS server */
};

class AutoConnect {
 public:
  AutoConnect();
  AutoConnect(WebServerClass& webServer);
  ~AutoConnect();
  bool  config(AutoConnectConfig& Config);
  bool  config(const char* ap, const char* password = nullptr);
  void  home(String uri);
  bool  begin();
  bool  begin(const char* ssid, const char* passphrase = nullptr, unsigned long timeout = AUTOCONNECT_TIMEOUT);
  void  end();
  void  handleClient();
  void  handleRequest();
  WebServerClass& host();

  typedef std::function<bool(IPAddress)>  DetectExit_ft;
  void  onDetect(DetectExit_ft fn);
  void  onNotFound(WebServerClass::THandlerFunction fn);

 protected:
  enum _webServerAllocateType {
    AC_WEBSERVER_PARASITIC,
    AC_WEBSERVER_HOSTED
  };
  typedef enum _webServerAllocateType  AC_WEBSERVER_TYPE;
  void  _initialize();
  bool  _config();
  void  _startWebServer();
  void  _startDNSServer();
  void  _handleNotFound();
  bool  _loadAvailCredential();
  void  _stopPortal();
  bool  _classifyHandle(HTTPMethod mothod, String uri);
  PageElement*  _setupPage(String uri);

  /** Request handlers implemented by Page Builder */
  String  _induceConnect(PageArgument& args);
  String  _induceDisconnect(PageArgument& args);
  String  _induceReset(PageArgument& args);
  String  _invokeResult(PageArgument& args);

  /** For portal control */
  bool  _captivePortal();
  bool  _isIP(String ipStr);
  wl_status_t _waitForConnect(unsigned long timeout);
  void  _disconnectWiFi(bool wifiOff);

  /** Utilities */
  static uint32_t      _getChipId();
  static uint32_t      _getFlashChipRealSize();
  static String        _toMACAddressString(const uint8_t mac[]);
  static unsigned int  _toWiFiQuality(int32_t rssi);
  DetectExit_ft        _onDetectExit;
  WebServerClass::THandlerFunction _notFoundHandler;

  std::unique_ptr<WebServerClass> _webServer;
  std::unique_ptr<DNSServer>        _dnsServer;
  AC_WEBSERVER_TYPE                 _webServerAlloc;

  PageBuilder*  _responsePage;
  PageElement*  _currentPageElement;
  
   /** configurations */
  AutoConnectConfig     _apConfig;
  struct station_config _credential;
  uint8_t       _hiddenSSIDCount;
  unsigned long _portalTimeout;

  /** The control indicators */
  bool  _rfConnect;       /**< URI /connect requested */
  bool  _rfDisconnect;    /**< URI /disc requested */
  bool  _rfReset;         /**< URI /reset requested */

  String        _uri;
  String        _redirectURI;
  IPAddress     _currentHostIP;
  String        _menuTitle;

  /** PegeElements of AutoConnect site. */
  static const char _CSS_BASE[] PROGMEM;
  static const char _CSS_UL[] PROGMEM;
  static const char _CSS_ICON_LOCK[] PROGMEM;
  static const char _CSS_INPUT_BUTTON[] PROGMEM;
  static const char _CSS_INPUT_TEXT[] PROGMEM;
  static const char _CSS_TABLE[] PROGMEM;
  static const char _CSS_LUXBAR[] PROGMEM;
  static const char _ELM_HTML_HEAD[] PROGMEM;
  static const char _ELM_MENU[] PROGMEM;
  static const char _PAGE_STAT[] PROGMEM;
  static const char _PAGE_CONFIGNEW[] PROGMEM;
  static const char _PAGE_OPENCREDT[] PROGMEM;
  static const char _PAGE_SUCCESS[] PROGMEM;
  static const char _PAGE_RESETTING[] PROGMEM;
  static const char _PAGE_DISCONN[] PROGMEM;
  static const char _PAGE_FAIL[] PROGMEM;
  static const char _PAGE_404[] PROGMEM;

  /** Token handlers for PageBuilder */
  String _token_CSS_BASE(PageArgument& args);
  String _token_CSS_UL(PageArgument& args);
  String _token_CSS_ICON_LOCK(PageArgument& args);
  String _token_CSS_INPUT_BUTTON(PageArgument& args);
  String _token_CSS_INPUT_TEXT(PageArgument& args);
  String _token_CSS_TABLE(PageArgument& args);
  String _token_CSS_LUXBAR(PageArgument& args);
  String _token_HEAD(PageArgument& args);
  String _token_MENU(PageArgument& args);
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
  String _token_HIDDEN_COUNT(PageArgument& args);
  String _token_OPEN_SSID(PageArgument& args);
  String _token_UPTIME(PageArgument& args);

#if defined(ARDUINO_ARCH_ESP8266)
  friend class ESP8266WebServer;
#elif defined(ARDUINO_ARCH_ESP32)
  friend class WebServer;
#endif
};

#endif  // _AUTOCONNECT_H_
