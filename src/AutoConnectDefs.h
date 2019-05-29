/**
 * Predefined AutoConnect configuration parameters.
 * @file AutoConnectDefs.h
 * @author hieromon@gmail.com
 * @version  0.9.9
 * @date 2019-05-25
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTDEFS_H_
#define _AUTOCONNECTDEFS_H_

// Uncomment the following AC_DEBUG to enable debug output.
#define AC_DEBUG

// Debug output destination can be defined externally with AC_DEBUG_PORT
#ifndef AC_DEBUG_PORT
#define AC_DEBUG_PORT Serial
#endif // !AC_DEBUG_PORT
#ifdef AC_DEBUG
#define AC_DBG_DUMB(...) do {AC_DEBUG_PORT.printf( __VA_ARGS__ );} while (0)
#define AC_DBG(...) do {AC_DEBUG_PORT.print("[AC] "); AC_DEBUG_PORT.printf( __VA_ARGS__ );} while (0)
#else
#define AC_DBG(...)
#define AC_DBG_DUMB(...)
#endif // !AC_DEBUG

// Indicator to specify that AutoConnectAux handles elements with JSON.
// Comment out the AUTOCONNECT_USE_JSON macro to detach the ArduinoJson.
#define AUTOCONNECT_USE_JSON

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
#define AUTOCONNECT_AP_IP 0x01F4A8C0  //*< 192.168.244.1 */
#endif // !AUTOCONNECT_AP_IP
#ifndef AUTOCONNECT_AP_GW
#define AUTOCONNECT_AP_GW 0x01F4A8C0  //*< 192.168.244.1 */
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
#define AUTOCONNECT_URI_DISCON  AUTOCONNECT_URI "/disc"
#define AUTOCONNECT_URI_RESET   AUTOCONNECT_URI "/reset"
#define AUTOCONNECT_URI_SUCCESS AUTOCONNECT_URI "/success"
#define AUTOCONNECT_URI_FAIL    AUTOCONNECT_URI "/fail"

// Time-out limitation when AutoConnect::begin [ms]
#ifndef AUTOCONNECT_TIMEOUT
#define AUTOCONNECT_TIMEOUT     30000
#endif // !AUTOCONNECT_TIMEOUT

// Captive portal timeout value [ms]
#ifndef AUTOCONNECT_CAPTIVEPORTAL_TIMEOUT
#define AUTOCONNECT_CAPTIVEPORTAL_TIMEOUT 0
#endif // !AUTOCONNECT_CAPTIVEPORTAL_TIMEOUT

// Advance wait time [s]
#ifndef AUTOCONNECT_STARTUPTIME
#define AUTOCONNECT_STARTUPTIME (AUTOCONNECT_TIMEOUT/1000)
#endif // !AUTOCONNECT_STARTUPTIME

// Response wait time until requesting a result of connection attempt, uint:[s] as String
#ifndef AUTOCONNECT_RESPONSE_WAITTIME
#define AUTOCONNECT_RESPONSE_WAITTIME "2"
#endif // !AUTOCONNECT_RESPONSE_WAITTIME

// Default HTTP port
#ifndef AUTOCONNECT_HTTPPORT
#define AUTOCONNECT_HTTPPORT    80
#endif // !AUTOCONNECT_HTTPPORT

// DNS port
#ifndef AUTOCONNECT_DNSPORT
#define AUTOCONNECT_DNSPORT     53
#endif // !AUTOCONNECT_DNSPORT

// http response transfer method
#ifndef AUTOCONNECT_HTTP_TRANSFER
#define AUTOCONNECT_HTTP_TRANSFER PB_ByteStream
#endif // !AUTOCONNECT_HTTP_TRANSFER

// Reserved buffer size to build content
#ifndef AUTOCONNECT_CONTENTBUFFER_SIZE
#define AUTOCONNECT_CONTENTBUFFER_SIZE  (13 * 1024)
#endif // !AUTOCONNECT_CONTENTBUFFER_SIZE

// Number of unit lines in the page that lists available SSIDs
#ifndef AUTOCONNECT_SSIDPAGEUNIT_LINES
#define AUTOCONNECT_SSIDPAGEUNIT_LINES  5
#endif // !AUTOCONNECT_SSIDPAGEUNIT_LINES

// SPI transfer speed for SD
#ifndef AUTOCONNECT_SD_SPEED
#define AUTOCONNECT_SD_SPEED    4000000
#endif // !AUTOCONNECT_SD_SPEED

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

// Explicitly avoiding unused warning with token handler of PageBuilder
#define AC_UNUSED(expr) do { (void)(expr); } while (0)

#endif // _AUTOCONNECTDEFS_H_
