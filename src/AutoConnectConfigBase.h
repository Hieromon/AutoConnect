/**
 * A base structure of AutoConnect configuration settings.
 * @file AutoConnectConfigBase.h
 * @author hieromon@gmail.com
 * @version 1.4.0
 * @date 2022-07-29
 * @copyright MIT license.
 */

#ifndef _AUTOCONNECTCONFIGBASE_H_
#define _AUTOCONNECTCONFIGBASE_H_

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif
#include "AutoConnectDefs.h"
#include "AutoConnectTypes.h"
#include "AutoConnectCredential.h"

class AutoConnectConfigBase {
 public:
  /**
   *  AutoConnectConfig default constructor.
   *  SSID for the captive portal access point assumes AUTOCONNECT_APID which
   *  assigned from macro. Password is same as above too.
   */
  AutoConnectConfigBase() :
    apip(AUTOCONNECT_AP_IP),
    gateway(AUTOCONNECT_AP_GW),
    netmask(AUTOCONNECT_AP_NM),
    apid(String(F(AUTOCONNECT_APID))),
    psk(String(F(AUTOCONNECT_PSK))),
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
    preserveAPMode(false),
    preserveIP(false),
    beginTimeout(AUTOCONNECT_TIMEOUT),
    portalTimeout(AUTOCONNECT_CAPTIVEPORTAL_TIMEOUT),
    menuItems(AC_MENUITEM_CONFIGNEW | AC_MENUITEM_OPENSSIDS | AC_MENUITEM_DISCONNECT | AC_MENUITEM_RESET | AC_MENUITEM_HOME),
    reconnectInterval(0),
    ticker(false),
    tickerPort(AUTOCONNECT_TICKER_PORT),
    tickerOn(AUTOCONNECT_TICKER_LEVEL),
    auth(AC_AUTH_NONE),
    authScope(AC_AUTHSCOPE_AUX),
    username(String("")),
    password(String("")),
    hostName(String("")),
    homeUri(AUTOCONNECT_HOMEURI),
    title(AUTOCONNECT_MENU_TITLE),
    staip(static_cast<uint32_t>(0)),
    staGateway(static_cast<uint32_t>(0)),
    staNetmask(static_cast<uint32_t>(0)),
    dns1(static_cast<uint32_t>(0)),
    dns2(static_cast<uint32_t>(0)) {}
  /**
   *  Configure by SSID for the captive portal access point and password.
   */
  AutoConnectConfigBase(const char* ap, const char* password, const unsigned long portalTimeout = 0, const uint8_t channel = AUTOCONNECT_AP_CH) :
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
    preserveAPMode(false),
    preserveIP(false),
    beginTimeout(AUTOCONNECT_TIMEOUT),
    portalTimeout(portalTimeout),
    menuItems(AC_MENUITEM_CONFIGNEW | AC_MENUITEM_OPENSSIDS | AC_MENUITEM_DISCONNECT | AC_MENUITEM_RESET | AC_MENUITEM_HOME),
    reconnectInterval(0),
    ticker(false),
    tickerPort(AUTOCONNECT_TICKER_PORT),
    tickerOn(AUTOCONNECT_TICKER_LEVEL),
    auth(AC_AUTH_NONE),
    authScope(AC_AUTHSCOPE_AUX),
    username(String("")),
    password(String("")),
    hostName(String("")),
    homeUri(AUTOCONNECT_HOMEURI),
    title(AUTOCONNECT_MENU_TITLE),
    staip(static_cast<uint32_t>(0)),
    staGateway(static_cast<uint32_t>(0)),
    staNetmask(static_cast<uint32_t>(0)),
    dns1(static_cast<uint32_t>(0)),
    dns2(static_cast<uint32_t>(0)) {}

  ~AutoConnectConfigBase() {}

  AutoConnectConfigBase& operator=(const AutoConnectConfigBase& o) {
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
    preserveAPMode = o.preserveAPMode;
    preserveIP = o.preserveIP;
    beginTimeout = o.beginTimeout;
    portalTimeout = o.portalTimeout;
    menuItems = o.menuItems;
    reconnectInterval = o.reconnectInterval;
    ticker = o.ticker;
    tickerPort = o.tickerPort;
    tickerOn = o.tickerOn;
    auth = o.auth;
    authScope = o.authScope;
    username = o.username;
    password = o.password;
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
  bool      preserveAPMode;     /**< Keep existing AP WiFi mode if captive portal won't be started. */
  bool      preserveIP;         /**< IP configurations in AutoConnectConfig take precedence over the IP information contained in the stored credentials. */
  unsigned long beginTimeout;   /**< Timeout value for WiFi.begin */
  unsigned long portalTimeout;  /**< Timeout value for stay in the captive portal */
  uint16_t  menuItems;          /**< A compound value of the menu items to be attached */
  uint8_t   reconnectInterval;  /**< Auto-reconnect attempt interval uint */
  bool      ticker;             /**< Drives LED flicker according to WiFi connection status. */
  uint8_t   tickerPort;         /**< GPIO for flicker */
  uint8_t   tickerOn;           /**< A signal for flicker turn on */
  AC_AUTH_t auth;               /**< Enable authentication */
  uint16_t  authScope;          /**< Authetication scope */
  String    username;           /**< User name for authentication */
  String    password;           /**< Authentication password */
  String    hostName;           /**< host name */
  String    homeUri;            /**< A URI of user site */
  String    title;              /**< Menu title */
  IPAddress staip;              /**< Station static IP address */
  IPAddress staGateway;         /**< Station gateway address */
  IPAddress staNetmask;         /**< Station subnet mask */
  IPAddress dns1;               /**< Primary DNS server */
  IPAddress dns2;               /**< Secondary DNS server */
};

#endif // !_AUTOCONNECTCONFIGBASE_H_
