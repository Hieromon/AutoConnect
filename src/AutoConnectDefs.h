/**
 * Predefined AutoConnect configuration parameters.
 * @file AutoConnectDefs.h
 * @author hieromon@gmail.com
 * @version  1.3.4
 * @date 2022-02-09
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTDEFS_H_
#define _AUTOCONNECTDEFS_H_

// Uncomment the following AC_DEBUG to enable debug output.
//#define AC_DEBUG

// Debug output destination can be defined externally with AC_DEBUG_PORT
#ifndef AC_DEBUG_PORT
#define AC_DEBUG_PORT Serial
#endif // !AC_DEBUG_PORT
#ifdef AC_DEBUG
#define AC_DBG_DUMB(fmt, ...) do {AC_DEBUG_PORT.printf_P((PGM_P)PSTR(fmt), ## __VA_ARGS__ );} while (0)
#define AC_DBG(fmt, ...) do {AC_DEBUG_PORT.printf_P((PGM_P)PSTR("[AC] " fmt), ## __VA_ARGS__ );} while (0)
#else
#define AC_DBG(...) do {(void)0;} while(0)
#define AC_DBG_DUMB(...) do {(void)0;} while(0)
#endif // !AC_DEBUG

// Setting ESP-IDF logging verbosity for ESP32 platform
// This setting has no effect on the ESP8266 platform
// Uncomment the following AC_USE_ESPIDFLOG to activate ESP_LOGV output.
#if defined(ARDUINO_ARCH_ESP8266)
#ifdef AC_USE_ESPIDFLOG
#undef AC_USE_ESPIDFLOG
#endif
#elif defined(ARDUINO_ARCH_ESP32)
#ifndef AC_USE_ESPIDFLOG
//#define AC_USE_ESPIDFLOG
#endif // !AC_USE_ESPIDFLOG
#endif

// Indicator to specify that AutoConnectAux handles elements with JSON.
// Comment out the AUTOCONNECT_USE_JSON macro to detach the ArduinoJson.
#ifndef AUTOCONNECT_NOUSE_JSON
#define AUTOCONNECT_USE_JSON

// Indicator of whether to use the AutoConnectUpdate feature.
#define AUTOCONNECT_USE_UPDATE
#endif

// Declaration to enable AutoConnectConfigAux.
// AC_USE_CONFIGAUX must be enabled along with AUTOCONNECT_USE_JSON
// to enable AutoConnectConfigAux.
//#define AC_USE_CONFIGAUX
#if defined(AC_USE_CONFIGAUX) && defined(AUTOCONNECT_USE_JSON)
#define AUTOCONNECT_USE_CONFIGAUX
#endif

// The AC_USE_SPIFFS and AC_USE_LITTLEFS macros declare which filesystem
// to apply. Their definitions are contradictory to each other and you
// cannot activate both at the same time.
//#define AC_USE_SPIFFS
//#define AC_USE_LITTLEFS
// Each platform supported by AutoConnect has a default file system,
// which is LittleFS for ESP8266 and SPIFFS for ESP32. Neither AC_USE_SPIFFS
// nor AC_USE_LITTLE_FS needs to be explicitly defined as long as you use
// the default file system. The default file system for each platform is assumed.
// SPIFFS has deprecated on EP8266 core. AC_USE_SPIFFS flag indicates
// that the migration to LittleFS has not completed.

// Whether or not it points to the target access point is determined by
// matching the SSID or BSSID. The default key to collate is BSSID.
// The BSSID is usually fixed to the MAC address unique to its AP,
// but when using some mobile hotspots, the BSSID may change even for
// the same access point.
// If you operate inconvenience in aiming at the access point by BSSID,
// you can change the collation key to SSID by uncommenting the below.
// If the AUTOCONNECT_APKEY_SSID is defined at compile-time, the access
// point will be collated by the SSID.
//#define AUTOCONNECT_APKEY_SSID

// Predefined parameters
// SSID that Captive portal started.
#ifndef AUTOCONNECT_APID
#if defined(ARDUINO_ARCH_ESP8266)
#define AUTOCONNECT_APID  "esp8266ap"
#elif defined(ARDUINO_ARCH_ESP32)
#define AUTOCONNECT_APID  "esp32ap"
#endif // !ARDUINO_ARCH_ESP8266
#endif // !AUTOCONNECT_APID

// Password that Captive portal started.
#ifndef AUTOCONNECT_PSK
#define AUTOCONNECT_PSK   "12345678"
#endif // !AUTOCONNECT_PSK

#ifndef AUTOCONNECT_AP_IP
// #define AUTOCONNECT_AP_IP 0x01F4A8C0  //*< 192.168.244.1 */
#define AUTOCONNECT_AP_IP 0x011CD9AC  //*< 172.217.28.1 */
#endif // !AUTOCONNECT_AP_IP
#ifndef AUTOCONNECT_AP_GW
// #define AUTOCONNECT_AP_GW 0x01F4A8C0  //*< 192.168.244.1 */
#define AUTOCONNECT_AP_GW 0x011CD9AC  //*< 172.217.28.1 */
#endif // !AUTOCONNECT_AP_GW
#ifndef AUTOCONNECT_AP_NM
#define AUTOCONNECT_AP_NM 0x00FFFFFF  //*< 255.255.255.0 */
#endif // !AUTOCONNECT_AP_NM
#ifndef AUTOCONNECT_AP_CH
#define AUTOCONNECT_AP_CH 1
#endif // !AUTOCONNECT_AP_CH

// AutoConnect menu root path
#ifndef AUTOCONNECT_URI
#define AUTOCONNECT_URI         "/_ac"
#endif // !AUTOCONNECT_URI

// Root URI of home path prepared by user sketch
#ifndef AUTOCONNECT_HOMEURI
#define AUTOCONNECT_HOMEURI     "/"
#endif // !AUTOCONNECT_HOMEURI

// AutoConnectAux form argument name
#ifndef AUTOCONNECT_AUXURI_PARAM
#define AUTOCONNECT_AUXURI_PARAM  "_acuri"
#endif // !AUTOCONNECT_AUXURI_PARAM

// AutoConnect menu title
#ifndef AUTOCONNECT_MENU_TITLE
#define AUTOCONNECT_MENU_TITLE  "AutoConnect"
#endif // !AUTOCONNECT_MENU_TITLE

// URIs of AutoConnect menu collection
#define AUTOCONNECT_URI_CONFIG  AUTOCONNECT_URI "/config"
#define AUTOCONNECT_URI_CONNECT AUTOCONNECT_URI "/connect"
#define AUTOCONNECT_URI_RESULT  AUTOCONNECT_URI "/result"
#define AUTOCONNECT_URI_OPEN    AUTOCONNECT_URI "/open"
#define AUTOCONNECT_URI_DELETE  AUTOCONNECT_URI "/del"
#define AUTOCONNECT_URI_DISCON  AUTOCONNECT_URI "/disc"
#define AUTOCONNECT_URI_RESET   AUTOCONNECT_URI "/reset"
#define AUTOCONNECT_URI_SUCCESS AUTOCONNECT_URI "/success"
#define AUTOCONNECT_URI_FAIL    AUTOCONNECT_URI "/fail"
#define AUTOCONNECT_URI_UPDATE  AUTOCONNECT_URI "/update"
#define AUTOCONNECT_URI_UPDATE_ACT      AUTOCONNECT_URI "/update_act"
#define AUTOCONNECT_URI_UPDATE_PROGRESS AUTOCONNECT_URI "/update_progress"
#define AUTOCONNECT_URI_UPDATE_RESULT   AUTOCONNECT_URI "/update_result"
#define AUTOCONNECT_URI_CONFIGAUX AUTOCONNECT_URI "/acconfig"

// Number of seconds in a unit time [s]
#ifndef AUTOCONNECT_UNITTIME
#define AUTOCONNECT_UNITTIME    30
#endif

// Time-out limitation when AutoConnect::begin [ms]
#ifndef AUTOCONNECT_TIMEOUT
#define AUTOCONNECT_TIMEOUT     30000
#endif // !AUTOCONNECT_TIMEOUT

// Waiting time [ms] to go into autoReconnect
// Defined with 0, suppress be delayed.
#ifndef AUTOCONNECT_RECONNECT_DELAY
// This definition will be eventually pulled out since the issue will
// be gone with the WiFi lib of the arduino-esp32 core.
// Related issue #292
#define AUTOCONNECT_RECONNECT_DELAY   0
#endif // !AUTOCONNECT_RECONNECT_DELAY

// Captive portal timeout value [ms]
#ifndef AUTOCONNECT_CAPTIVEPORTAL_TIMEOUT
#define AUTOCONNECT_CAPTIVEPORTAL_TIMEOUT 0
#endif // !AUTOCONNECT_CAPTIVEPORTAL_TIMEOUT

// Advance wait time [s]
#ifndef AUTOCONNECT_STARTUPTIME
#define AUTOCONNECT_STARTUPTIME (AUTOCONNECT_TIMEOUT/1000)
#endif // !AUTOCONNECT_STARTUPTIME

// Response wait time until requesting a result of connection attempt, uint:[ms]
#ifndef AUTOCONNECT_RESPONSE_WAITTIME
#define AUTOCONNECT_RESPONSE_WAITTIME 2000
#endif // !AUTOCONNECT_RESPONSE_WAITTIME

// Default HTTP port
#ifndef AUTOCONNECT_HTTPPORT
#define AUTOCONNECT_HTTPPORT    80
#endif // !AUTOCONNECT_HTTPPORT

// DNS port
#ifndef AUTOCONNECT_DNSPORT
#define AUTOCONNECT_DNSPORT     53
#endif // !AUTOCONNECT_DNSPORT

// Each page of AutoConnect is http transferred by the content transfer
// mode of Page Builder.
// AUTOCONNECT_HTTP_TRANSFER defines default the Transfer-encoding with
// PageBuilder::TransferEncoding_t.
#ifndef AUTOCONNECT_HTTP_TRANSFER
#define AUTOCONNECT_HTTP_TRANSFER     ByteStream
#endif // !AUTOCONNECT_HTTP_TRANSFER

// Number of unit lines in the page that lists available SSIDs
#ifndef AUTOCONNECT_SSIDPAGEUNIT_LINES
#define AUTOCONNECT_SSIDPAGEUNIT_LINES  5
#endif // !AUTOCONNECT_SSIDPAGEUNIT_LINES

// SPI transfer speed for SD [Hz]
#ifndef AUTOCONNECT_SD_SPEED
#define AUTOCONNECT_SD_SPEED    4000000
#endif // !AUTOCONNECT_SD_SPEED

// Flicker signal related factors
// Flicker cycle during AP operation [ms]
#ifndef AUTOCONNECT_FLICKER_PERIODAP
#define AUTOCONNECT_FLICKER_PERIODAP  1000
#endif // !AUTOCONNECT_FLICKER_PERIODAP
// Flicker cycle while WiFi is not connected [ms]
#ifndef AUTOCONNECT_FLICKER_PERIODDC
#define AUTOCONNECT_FLICKER_PERIODDC  (AUTOCONNECT_FLICKER_PERIODAP << 1)
#endif // !AUTOCONNECT_FLICKER_PERIODDC
// Flicker pulse width during AP operation (8bit resolution)
#ifndef AUTOCONNECT_FLICKER_WIDTHAP
#define AUTOCONNECT_FLICKER_WIDTHAP   96
#endif // !AUTOCONNECT_FLICKER_WIDTHAP
// Flicker pulse width while WiFi is not connected (8bit resolution)
#ifndef AUTOCONNECT_FLICKER_WIDTHDC
#define AUTOCONNECT_FLICKER_WIDTHDC   16
#endif // !AUTOCONNECT_FLICKER_WIDTHDISCON
// Ticker port
#ifndef AUTOCONNECT_TICKER_PORT
#if defined(BUILTIN_LED) || defined(LED_BUILTIN)
#define AUTOCONNECT_TICKER_PORT       LED_BUILTIN
#else  // Native pin for the arduino
#define AUTOCONNECT_TICKER_PORT       2
#endif
#endif // !AUTOCONNECT_TICKER_PORT
// Ticker active signal level value that the board dependent LED turns on.
// As a typical example, the ON signal of built-in LED such as the
// NodeMCU is LOW and the HIGH for the NodeMCU-32S as another example.
#ifndef AUTOCONNECT_TICKER_LEVEL
#ifdef ARDUINO_ARCH_ESP32
#define AUTOCONNECT_TICKER_LEVEL      HIGH
#else
#define AUTOCONNECT_TICKER_LEVEL      LOW
#endif
#endif // !AUTOCONNECT_TICKER_LEVEL

// Lowest WiFi signal strength (RSSI) that can be connected.
#ifndef AUTOCONNECT_MIN_RSSI
#define AUTOCONNECT_MIN_RSSI          -120  // No limit
#endif // !AUTOCONNECT_MIN_RSSI

// ArduinoJson buffer size
#ifndef AUTOCONNECT_JSONBUFFER_SIZE
#define AUTOCONNECT_JSONBUFFER_SIZE     256
#endif // !AUTOCONNECT_JSONBUFFER_SIZE
#ifndef AUTOCONNECT_JSONDOCUMENT_SIZE
#define AUTOCONNECT_JSONDOCUMENT_SIZE   (8 * 1024)
#endif // !AUTOCONNECT_JSONDOCUMENT_SIZE
#ifndef AUTOCONNECT_JSONPSRAM_SIZE
#define AUTOCONNECT_JSONPSRAM_SIZE      (16* 1024)
#endif // !AUTOCONNECT_JSONPSRAM_SIZE

// Available HTTP port number for the update
#ifndef AUTOCONNECT_UPDATE_PORT
#define AUTOCONNECT_UPDATE_PORT       8000
#endif // !AUTOCONNECT_UPDATE_PORT

// HTTP client timeout limitation for the update [ms]
#ifndef AUTOCONNECT_UPDATE_TIMEOUT
#define AUTOCONNECT_UPDATE_TIMEOUT    8000
#endif // !AUTOCONNECT_UPDATE_TIMEOUT

// Maximum wait time until transitioning  AutoConnectUpdate dialog page [ms]
#ifndef AUTOCONNECT_UPDATE_DURATION
#define AUTOCONNECT_UPDATE_DURATION   180000
#endif // !AUTOCONNECT_UPDATE_DURATION

// Interval time of progress status periodical inquiry [ms]
#ifndef AUTOCONNECT_UPDATE_INTERVAL
#define AUTOCONNECT_UPDATE_INTERVAL   1500
#endif // !AUTOCONNECT_UPDATE_INTERVAL

// Wait timer for rebooting after updated
#ifndef AUTOCONNECT_UPDATE_WAITFORREBOOT
#define AUTOCONNECT_UPDATE_WAITFORREBOOT  15000
#endif // !AUTOCONNECT_UPDATE_WAITFORREBOOT

// URIs of the behaviors owned by the update server
#ifndef AUTOCONNECT_UPDATE_CATALOG
#define AUTOCONNECT_UPDATE_CATALOG    "/_catalog"
#endif // !AUTOCONNECT_UPDATE_CATALOG
#ifndef AUTOCONNECT_UPDATE_DOWNLOAD
#define AUTOCONNECT_UPDATE_DOWNLOAD   "/"
#endif // !AUTOCONNECT_UPDATE_DOWNLOAD
#ifndef AUTOCONNECT_UPDATE_CATALOG_JSONBUFFER_SIZE
#define AUTOCONNECT_UPDATE_CATALOG_JSONBUFFER_SIZE  256
#endif // !AUTOCONNECT_UPDATE_CATALOG_JSONBUFFER_SIZE

// HTTP authentication default realm
#ifndef AUTOCONNECT_AUTH_REALM
#define AUTOCONNECT_AUTH_REALM        "AUTOCONNECT"
#endif // !AUTOCONNECT_AUTH_REALM

// Flename pattern that AutoConnectOTA considers to be firmware.
// The extension used as the criterion for uploading destination is
// fixed.
// AUTOCONNECT_UPLOAD_ASFIRMWARE_USE_REGEXP allows you to use regular
// expressions with the extension to determine the uploading destination.
#ifndef AUTOCONNECT_UPLOAD_ASFIRMWARE
#ifdef  AUTOCONNECT_UPLOAD_ASFIRMWARE_USE_REGEXP
#define AUTOCONNECT_UPLOAD_ASFIRMWARE "^.+\\.bin$"
#else
#define AUTOCONNECT_UPLOAD_ASFIRMWARE ".bin"
#endif
#endif

// File name where AutoConnectConfig is persisted on the file system.
#ifndef AUTOCONNECT_CONFIGAUX_FILE
#define AUTOCONNECT_CONFIGAUX_FILE    "acconfig.json"
#endif // !AUTOCONNECT_CONFIGAUX_NAME

// Explicitly avoiding unused warning with token handler of PageBuilder
#define AC_UNUSED(expr) do { (void)(expr); } while (0)

// Numeric to string deployment
#define AUTOCONNECT_STRING_DEPLOY(n)  _AUTOCONNECT_STRING_DEPLOY(n)
#define _AUTOCONNECT_STRING_DEPLOY(s) #s

// Generates a template that determines whether the class owns the
// specified member function.
// The purpose of this macro is to avoid the use of invalid member
// functions due to differences in the version of the library which
// AutoConnect depends on.
#define AC_HAS_FUNC(func)                                   \
template<typename T>                                        \
struct has_func_##func {                                    \
 private:                                                   \
  typedef char  one;                                        \
  typedef long  two;                                        \
  template<typename U> static one test(decltype(&U::func)); \
  template<typename U> static two test(...);                \
 public:                                                    \
  enum { value = sizeof(test<T>(0)) == sizeof(char) };      \
}

// Provides ESP-IDF logging interface
// This setting has no effect on the ESP8266 platform
#if defined(ARDUINO_ARCH_ESP32) && defined(AC_USE_ESPIDFLOG)
#define AC_ESP_LOG(t, l) do {esp_log_level_set(t, l);} while(0)
#else
#define AC_ESP_LOG(...) do {(void)0;} while(0)
#endif

#endif // _AUTOCONNECTDEFS_H_
