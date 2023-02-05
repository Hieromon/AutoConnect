/**
 * AutoConnectCore class declaration is instantiated through a template
 * containing an AutoConnectConfigBase instance. This class defines members
 * that are limited to AutoConnect basic functionality.
 * @file AutoConnectCore.hpp
 * @author hieromon@gmail.com
 * @version 1.4.2
 * @date 2023-02-05
 * @copyright MIT license.
 */

#ifndef _AUTOCONNECTCORE_HPP_
#define _AUTOCONNECTCORE_HPP_

#include <vector>
#include <memory>
#include <functional>
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
extern "C" {
#include <user_interface.h>
}
using WebServer = ESP8266WebServer;
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#endif
#include <DNSServer.h>
#include <EEPROM.h>
#include <PageBuilder.h>
#include "AutoConnectDefs.h"
#include "AutoConnectTypes.h"
#include "AutoConnectPage.h"
#include "AutoConnectCredential.h"
#include "AutoConnectTicker.h"
#include "AutoConnectConfigBase.h"

template<typename T>
class AutoConnectCore {
 public:
  typedef enum {
    AC_IDLE           = 0x00, /**< .... ....  Initial state */
    AC_ESTABLISHED    = 0x01, /**< .... ...1  Connection successful. */
    AC_AUTORECONNECT  = 0x02, /**< .... ..1.  The autoReconnect was applied. */
    AC_TIMEOUT        = 0x04, /**< .... .1..  Connection timeout. */
    AC_INTERRUPT      = 0x08, /**< .... 1...  Connection interrupted due to an indication with the exit. */
    AC_CAPTIVEPORTAL  = 0x40, /**< .1.. ....  Captive portal is available. */
    AC_INPROGRESS     = 0x80  /**< 1... ....  WiFi.begin in progress. */
  } AC_PORTALSTATE_t;         /**< AutoConnect::begin and handleClient status of the portal during the period. */

  AutoConnectCore();
  explicit AutoConnectCore(WebServer& webServer);
  virtual ~AutoConnectCore();
  bool  begin(void);
  bool  begin(const char* ssid, const char* passphrase = nullptr, unsigned long timeout = 0);
  bool  config(T& config);
  bool  config(const char* ap, const char* password = nullptr);
  void  disconnect(const bool wifiOff = false, const bool clearConfig = false);
  inline void disableMenu(const uint16_t items) { _apConfig.menuItems &= (0xffff ^ items); }
  inline void enableMenu(const uint16_t items) { _apConfig.menuItems |= items; }
  virtual void  end(void);
  T&  getConfig(void) { return _apConfig; }
  bool  getCurrentCredential(station_config_t* staConfig);
  uint16_t  getEEPROMUsedSize(void);
  void  handleClient(void);
  void  handleRequest(void);
  void  home(const String& uri);
  WebServer& host(void);
  bool  isPortalAvailable(void) const { return portalStatus() & AC_CAPTIVEPORTAL; }
  uint8_t portalStatus(void) const { return _portalStatus; }

  typedef std::function<bool(IPAddress&)> DetectExit_ft;
  typedef std::function<void(IPAddress&)> ConnectExit_ft;
  typedef std::function<bool(void)>       WhileCaptivePortalExit_ft;
  typedef std::function<bool(String&)>    WhileConnectingExit_ft;
  void  onDetect(DetectExit_ft fn);
  void  onConnect(ConnectExit_ft fn);
  void  onNotFound(WebServer::THandlerFunction fn);
  void  whileCaptivePortal(WhileCaptivePortalExit_ft fn);
  void  whileConnecting(WhileConnectingExit_ft fn);
  template<typename U = AUTOCONNECT_APPLIED_FILECLASS>
  bool  saveCredential(const char* filename = "/" AC_IDENTIFIER, U& fs = AUTOCONNECT_APPLIED_FILESYSTEM);
  template<typename U = AUTOCONNECT_APPLIED_FILECLASS>
  bool  restoreCredential(const char* filename = "/" AC_IDENTIFIER, U& fs = AUTOCONNECT_APPLIED_FILESYSTEM);

 protected:
  typedef enum {
    AC_RECONNECT_SET,
    AC_RECONNECT_RESET
  } AC_STARECONNECT_t;
  typedef enum {
    AC_SEEKMODE_ANY,
    AC_SEEKMODE_NEWONE,
    AC_SEEKMODE_CURRENT
  } AC_SEEKMODE_t;
  void  _authentication(bool allow);
  void  _authentication(bool allow, const HTTPAuthMethod method);
  bool  _configAP(void);
  bool  _configSTA(const IPAddress& ip, const IPAddress& gateway, const IPAddress& netmask, const IPAddress& dns1, const IPAddress& dns2);
  String _getBootUri(void);
  bool  _getConfigSTA(station_config_t* config);
  bool  _loadAvailCredential(const char* ssid, const AC_PRINCIPLE_t principle = AC_PRINCIPLE_RECENT, const bool excludeCurrent = false);
  bool  _loadCurrentCredential(char* ssid, char* password, const AC_PRINCIPLE_t principle, const bool excludeCurrent);
  void  _restoreSTA(const station_config_t& staConfig);
  bool  _seekCredential(const AC_PRINCIPLE_t principle, const AC_SEEKMODE_t mode);
  void  _startWebServer(void);
  void  _startDNSServer(void);
  void  _stopDNSServer(void);
  void  _stopPortal(void);
  bool  _classifyHandle(HTTPMethod mothod, String uri);
  void  _handleNotFound(void);
  void  _purgePages(void);
  virtual PageElement*  _setupPage(String& uri);

  /** Request handlers implemented by Page Builder */
  String  _induceConnect(PageArgument& args);
  String  _induceDisconnect(PageArgument& args);
  String  _induceReset(PageArgument& args);
  String  _invokeResult(PageArgument& args);
  String  _promptDeleteCredential(PageArgument& args);

  /** For portal control */
  bool  _captivePortal(void);
  bool  _hasTimeout(unsigned long timeout);
  bool  _isIP(const String& ipStr);
  bool  _isPersistent(void);
  void  _softAP(void);
  wl_status_t _waitForConnect(unsigned long timeout);
  void  _waitForEndTransmission(void);
  void  _setReconnect(const AC_STARECONNECT_t order);

  /** Utilities */
  String              _attachMenuItem(const AC_MENUITEM_t item);
  static uint32_t     _getChipId(void);
  static uint32_t     _getFlashChipRealSize(void);
  static String       _getSystemUptime(void);
  static String       _toMACAddressString(const uint8_t mac[]);
  static unsigned int _toWiFiQuality(int32_t rssi);
  ConnectExit_ft      _onConnectExit;
  DetectExit_ft       _onDetectExit;
  WhileCaptivePortalExit_ft _whileCaptivePortal;
  WhileConnectingExit_ft    _whileConnecting;
  WebServer::THandlerFunction  _notFoundHandler;
  size_t              _freeHeapSize;

  /** Servers which works in concert. */
  typedef std::unique_ptr<WebServer, std::function<void(WebServer *)> > WebserverUP;
  WebserverUP _webServer = WebserverUP(nullptr, std::default_delete<WebServer>());
  std::unique_ptr<DNSServer>    _dnsServer;

  /**
   *  Dynamically hold one page of AutoConnect menu.
   *  Every time a GET/POST HTTP request occurs, an AutoConnect
   *  menu page corresponding to the URI is generated.
   */
  std::unique_ptr<PageBuilder>  _responsePage;
  std::unique_ptr<PageElement>  _currentPageElement;

  /** Saved configurations */
  T _apConfig;
  station_config_t   _credential;
  uint8_t       _hiddenSSIDCount;
  int16_t       _scanCount;
  uint8_t       _connectCh;
  unsigned long _portalAccessPeriod;
  unsigned long _attemptPeriod;
  String        _indelibleSSID;

  /** The control indicators */
  bool  _rfAdHocBegin = false;  /**< Specified with AutoConnect::begin */
  bool  _rfConnect = false;     /**< URI /connect requested */
  bool  _rfDisconnect = false;  /**< URI /disc requested */
  bool  _rfReset = false;       /**< URI /reset requested */
  wl_status_t   _rsConnect;     /**< connection result */
#ifdef ARDUINO_ARCH_ESP32
  WiFiEventId_t _disconnectEventId = -1;  /**< STA disconnection event handler registered id  */
#endif
  uint8_t       _portalStatus;  /**< Status in the portal */

  /** Only available with ticker enabled */
  std::unique_ptr<AutoConnectTicker>  _ticker;

  /** HTTP header information of the currently requested page. */
  IPAddress     _currentHostIP; /**< host IP address */
  String        _uri;           /**< Requested URI */
  String        _redirectURI;   /**< Redirect destination */
  String        _menuTitle;     /**< Title string of the page */

  /** PageElements of AutoConnect site. */
  static const char _CSS_BASE[] PROGMEM;
  static const char _CSS_LUXBAR_BODY[] PROGMEM;
  static const char _CSS_LUXBAR_HEADER[] PROGMEM;
  static const char _CSS_LUXBAR_BGR[] PROGMEM;
  static const char _CSS_LUXBAR_ANI[] PROGMEM;
  static const char _CSS_LUXBAR_MEDIA[] PROGMEM;
  static const char _CSS_LUXBAR_ITEM[] PROGMEM;
  static const char _CSS_UL[] PROGMEM;
  static const char _CSS_ICON_LOCK[] PROGMEM;
  static const char _CSS_ICON_TRASH[] PROGMEM;
  static const char _CSS_INPUT_BUTTON[] PROGMEM;
  static const char _CSS_INPUT_TEXT[] PROGMEM;
  static const char _CSS_TABLE[] PROGMEM;
  static const char _CSS_SPINNER[] PROGMEM;
  static const char _ELM_HTML_HEAD[] PROGMEM;
  static const char _ELM_MENU_PRE[] PROGMEM;
  static const char _ELM_MENU_POST[] PROGMEM;
  static const char _PAGE_STAT[] PROGMEM;
  static const char _PAGE_CONFIGNEW[] PROGMEM;
  static const char _PAGE_CONNECTING[] PROGMEM;
  static const char _PAGE_OPENCREDT[] PROGMEM;
  static const char _PAGE_DELREQ[] PROGMEM;
  static const char _PAGE_SUCCESS[] PROGMEM;
  static const char _PAGE_RESETTING[] PROGMEM;
  static const char _PAGE_RESULT[] PROGMEM;
  static const char _PAGE_DISCONN[] PROGMEM;
  static const char _PAGE_FAIL[] PROGMEM;
  static const char _PAGE_404[] PROGMEM;

  /** Token handlers for PageBuilder */
  String _token_CSS_BASE(PageArgument& args);
  String _token_CSS_ICON_LOCK(PageArgument& args);
  String _token_CSS_ICON_TRASH(PageArgument& args);
  String _token_CSS_INPUT_BUTTON(PageArgument& args);
  String _token_CSS_INPUT_TEXT(PageArgument& args);
  String _token_CSS_LUXBAR_BODY(PageArgument& args);
  String _token_CSS_LUXBAR_HEADER(PageArgument& args);
  String _token_CSS_LUXBAR_BGR(PageArgument& args);
  String _token_CSS_LUXBAR_ANI(PageArgument& args);
  String _token_CSS_LUXBAR_MEDIA(PageArgument& args);
  String _token_CSS_LUXBAR_ITEM(PageArgument& args);
  String _token_CSS_SPINNER(PageArgument& args);
  String _token_CSS_TABLE(PageArgument& args);
  String _token_CSS_UL(PageArgument& args);
  String _token_MENU_AUX(PageArgument& args);
  String _token_MENU_LIST(PageArgument& args);
  String _token_MENU_POST(PageArgument& args);
  String _token_MENU_PRE(PageArgument& args);
  String _token_AP_MAC(PageArgument& args);
  String _token_BOOTURI(PageArgument& args);
  String _token_CHANNEL(PageArgument& args);
  String _token_CHIP_ID(PageArgument& args);
  String _token_CONFIG_STAIP(PageArgument& args);
  String _token_CPU_FREQ(PageArgument& args);
  String _token_CURRENT_SSID(PageArgument& args);
  String _token_DBM(PageArgument& args);
  String _token_ESTAB_SSID(PageArgument& args);
  String _token_FLASH_SIZE(PageArgument& args);
  String _token_FREE_HEAP(PageArgument& args);
  String _token_GATEWAY(PageArgument& args);
  String _token_HEAD(PageArgument& args);
  String _token_HIDDEN_COUNT(PageArgument& args);
  String _token_LIST_SSID(PageArgument& args);
  String _token_LOCAL_IP(PageArgument& args);
  String _token_NETMASK(PageArgument& args);
  String _token_OPEN_SSID(PageArgument& args);
  String _token_SOFTAP_IP(PageArgument& args);
  String _token_SSID_COUNT(PageArgument& args);
  String _token_STA_MAC(PageArgument& args);
  String _token_STATION_STATUS(PageArgument& args);
  String _token_SYSTEM_UPTIME(PageArgument &args);
  String _token_UPTIME(PageArgument& args);
  String _token_WIFI_MODE(PageArgument& args);
  String _token_WIFI_STATUS(PageArgument& args);
  static const  String  _emptyString; /**< An empty string allocation  **/

  typedef enum {
    AP_MAC,
    BOOTURI,
    CHANNEL,
    CHIP_ID,
    CONFIG_IP,
    CPU_FREQ,
    CSS_BASE,
    CSS_ICON_LOCK,
    CSS_ICON_TRASH,
    CSS_INPUT_BUTTON,
    CSS_INPUT_TEXT,
    CSS_LUXBAR_ANI,
    CSS_LUXBAR_BGR,
    CSS_LUXBAR_BODY,
    CSS_LUXBAR_HEADER,
    CSS_LUXBAR_ITEM,
    CSS_LUXBAR_MEDIA,
    CSS_SPINNER,
    CSS_TABLE,
    CSS_UL,
    CUR_SSID,
    DBM,
    DELREQ,
    DISCONNECT,
    ESTAB_SSID,
    FLASH_SIZE,
    FREE_HEAP,
    GATEWAY,
    HEAD,
    HIDDEN_COUNT,
    LIST_SSID,
    LOCAL_IP,
    MENU_AUX,
    MENU_LIST,
    MENU_POST,
    MENU_PRE,
    NETMASK,
    OPEN_SSID,
    REQ,
    RESET,
    RESULT,
    SOFTAP_IP,
    SSID_COUNT,
    STA_MAC,
    STATION_STATUS,
    SYSTEM_UPTIME,
    UPTIME,
    WIFI_MODE,
    WIFI_STATUS
  } AC_PAGETOKEN_t;

  typedef struct {
    AC_PAGETOKEN_t  tokenId;
    PGM_P token;
  } AC_PAGETOKENSTRING_t;

  typedef struct {
    AC_PAGETOKEN_t  tokenId;
    HandleFuncT handler;
  } AC_PAGETOKENREGISTRY_t;

  typedef struct {
    PGM_P uri;
    PGM_P menuTitle;
    PGM_P mold;
    uint16_t  appMenus;
    bool  authRequired;
    std::vector<AC_PAGETOKEN_t> tokenId;
  } AC_PAGECONST_t;

  std::vector<AC_PAGETOKENREGISTRY_t> _tokenRegistry = {
    {AP_MAC, std::bind(&AutoConnectCore<T>::_token_AP_MAC, this, std::placeholders::_1)},
    {BOOTURI, std::bind(&AutoConnectCore<T>::_token_BOOTURI, this, std::placeholders::_1)},
    {CHANNEL, std::bind(&AutoConnectCore<T>::_token_CHANNEL, this, std::placeholders::_1)},
    {CHIP_ID, std::bind(&AutoConnectCore<T>::_token_CHIP_ID, this, std::placeholders::_1)},
    {CONFIG_IP, std::bind(&AutoConnectCore<T>::_token_CONFIG_STAIP, this, std::placeholders::_1)},
    {CPU_FREQ, std::bind(&AutoConnectCore<T>::_token_CPU_FREQ, this, std::placeholders::_1)},
    {CSS_BASE, std::bind(&AutoConnectCore<T>::_token_CSS_BASE, this, std::placeholders::_1)},
    {CSS_ICON_LOCK, std::bind(&AutoConnectCore<T>::_token_CSS_ICON_LOCK, this, std::placeholders::_1)},
    {CSS_ICON_TRASH, std::bind(&AutoConnectCore<T>::_token_CSS_ICON_TRASH, this, std::placeholders::_1)},
    {CSS_INPUT_BUTTON, std::bind(&AutoConnectCore<T>::_token_CSS_INPUT_BUTTON, this, std::placeholders::_1)},
    {CSS_INPUT_TEXT, std::bind(&AutoConnectCore<T>::_token_CSS_INPUT_TEXT, this, std::placeholders::_1)},
    {CSS_LUXBAR_ANI, std::bind(&AutoConnectCore<T>::_token_CSS_LUXBAR_ANI, this, std::placeholders::_1)},
    {CSS_LUXBAR_BGR, std::bind(&AutoConnectCore<T>::_token_CSS_LUXBAR_BGR, this, std::placeholders::_1)},
    {CSS_LUXBAR_BODY, std::bind(&AutoConnectCore<T>::_token_CSS_LUXBAR_BODY, this, std::placeholders::_1)},
    {CSS_LUXBAR_HEADER, std::bind(&AutoConnectCore<T>::_token_CSS_LUXBAR_HEADER, this, std::placeholders::_1)},
    {CSS_LUXBAR_ITEM, std::bind(&AutoConnectCore<T>::_token_CSS_LUXBAR_ITEM, this, std::placeholders::_1)},
    {CSS_LUXBAR_MEDIA, std::bind(&AutoConnectCore<T>::_token_CSS_LUXBAR_MEDIA, this, std::placeholders::_1)},
    {CSS_SPINNER, std::bind(&AutoConnectCore<T>::_token_CSS_SPINNER, this, std::placeholders::_1)},
    {CSS_TABLE, std::bind(&AutoConnectCore<T>::_token_CSS_TABLE, this, std::placeholders::_1)},
    {CSS_UL, std::bind(&AutoConnectCore<T>::_token_CSS_UL, this, std::placeholders::_1)},
    {CUR_SSID, std::bind(&AutoConnectCore<T>::_token_CURRENT_SSID, this, std::placeholders::_1)},
    {DBM, std::bind(&AutoConnectCore<T>::_token_DBM, this, std::placeholders::_1)},
    {DELREQ, std::bind(&AutoConnectCore<T>::_promptDeleteCredential, this, std::placeholders::_1)},
    {DISCONNECT, std::bind(&AutoConnectCore<T>::_induceDisconnect, this, std::placeholders::_1)},
    {ESTAB_SSID, std::bind(&AutoConnectCore<T>::_token_ESTAB_SSID, this, std::placeholders::_1)},
    {FLASH_SIZE, std::bind(&AutoConnectCore<T>::_token_FLASH_SIZE, this, std::placeholders::_1)},
    {FREE_HEAP, std::bind(&AutoConnectCore<T>::_token_FREE_HEAP, this, std::placeholders::_1)},
    {GATEWAY, std::bind(&AutoConnectCore<T>::_token_GATEWAY, this, std::placeholders::_1)},
    {HEAD, std::bind(&AutoConnectCore<T>::_token_HEAD, this, std::placeholders::_1)},
    {HIDDEN_COUNT, std::bind(&AutoConnectCore<T>::_token_HIDDEN_COUNT, this, std::placeholders::_1)},
    {LIST_SSID, std::bind(&AutoConnectCore<T>::_token_LIST_SSID, this, std::placeholders::_1)},
    {LOCAL_IP, std::bind(&AutoConnectCore<T>::_token_LOCAL_IP, this, std::placeholders::_1)},
    {MENU_AUX, std::bind(&AutoConnectCore<T>::_token_MENU_AUX, this, std::placeholders::_1)},
    {MENU_LIST, std::bind(&AutoConnectCore<T>::_token_MENU_LIST, this, std::placeholders::_1)},
    {MENU_POST, std::bind(&AutoConnectCore<T>::_token_MENU_POST, this, std::placeholders::_1)},
    {MENU_PRE, std::bind(&AutoConnectCore<T>::_token_MENU_PRE, this, std::placeholders::_1)},
    {NETMASK, std::bind(&AutoConnectCore<T>::_token_NETMASK, this, std::placeholders::_1)},
    {OPEN_SSID, std::bind(&AutoConnectCore<T>::_token_OPEN_SSID, this, std::placeholders::_1)},
    {REQ, std::bind(&AutoConnectCore<T>::_induceConnect, this, std::placeholders::_1)},
    {RESET, std::bind(&AutoConnectCore<T>::_induceReset, this, std::placeholders::_1)},
    {SOFTAP_IP, std::bind(&AutoConnectCore<T>::_token_SOFTAP_IP, this, std::placeholders::_1)},
    {SSID_COUNT, std::bind(&AutoConnectCore<T>::_token_SSID_COUNT, this, std::placeholders::_1)},
    {STA_MAC, std::bind(&AutoConnectCore<T>::_token_STA_MAC, this, std::placeholders::_1)},
    {STATION_STATUS, std::bind(&AutoConnectCore<T>::_token_STATION_STATUS, this, std::placeholders::_1)},
    {SYSTEM_UPTIME, std::bind(&AutoConnectCore<T>::_token_SYSTEM_UPTIME, this, std::placeholders::_1)},
    {UPTIME, std::bind(&AutoConnectCore<T>::_token_UPTIME, this, std::placeholders::_1)},
    {WIFI_MODE, std::bind(&AutoConnectCore<T>::_token_WIFI_MODE, this, std::placeholders::_1)},
    {WIFI_STATUS, std::bind(&AutoConnectCore<T>::_token_WIFI_STATUS, this, std::placeholders::_1)}
  };
  static AC_PAGETOKENSTRING_t _tokenStringify[] PROGMEM;
  static AC_PAGECONST_t _pageConst[];

 private:
  inline bool _isValidAP(const station_config_t& config, const uint8_t item) const;
  inline void _reconnectDelay(const uint32_t ms);

  // The following members are used to separate AutoConnectAux-dependent
  // processes from AutoConnectCore that were mingled with AutoConnect.
  // These are treated as dummy calls in AutoConnectCore.
  virtual inline void _attachOTA(void) {}
  virtual inline void _enableUpdate(void) {}
  virtual inline bool _handleOTA(void) { return false; }
  virtual inline bool _handleUpdate(void) { return false; }
  virtual inline void _registerOnUpload(PageBuilder* page) { AC_UNUSED(page); }
  virtual inline void _releaseAux(const String& uri) { AC_UNUSED(uri); }
  virtual inline void _saveCurrentUri(const String& uri) { AC_UNUSED(uri); }
  virtual inline String _mold_MENU_AUX(PageArgument& args) { AC_UNUSED(args); return String(""); }
};

#endif  // _AUTOCONNECTCORE_HPP_
