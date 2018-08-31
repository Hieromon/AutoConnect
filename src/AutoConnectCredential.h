/**
 *  Declaration of AutoConnectCredential class.
 *  @file AutoConnectCredential.h
 *  @author hieromon@gmail.com
 *  @version  0.9.5
 *  @date 2018-02-17
 *  @copyright  MIT license.
 */

#ifndef _AUTOCONNECTCREDENTIAL_H_
#define _AUTOCONNECTCREDENTIAL_H_

#include <Arduino.h>
#if defined(ARDUINO_ARCH_ESP8266)
extern "C" {
#include <user_interface.h>
}
#elif defined(ARDUINO_ARCH_ESP32)
#include <esp_wifi.h>
struct station_config {
    uint8_t  ssid[32];
    uint8_t  password[64];
    uint8_t  bssid_set;
    uint8_t  bssid[6];
    wifi_fast_scan_threshold_t threshold;
};
#endif

/** Credential storage area offset specifier in EEPROM.
 *  By defining AC_IDENTIFIER_OFFSET macro in the user sketch, the credential
 *  storage area can be shifted in EEPROM.
 */
#ifndef AC_IDENTIFIER_OFFSET
#define AC_IDENTIFIER_OFFSET  0
#endif

/** AutoConnectCredential class. */
class AutoConnectCredential {
 public:
  AutoConnectCredential();
  AutoConnectCredential(uint16_t offset);
  ~AutoConnectCredential();
  uint8_t   entries() { return _entries; }
  bool      del(const char* ssid);
  int8_t    load(const char* ssid, struct station_config* config);
  bool      load(int8_t entry, struct station_config* config);
  bool      save(const struct station_config* config);

 private:
  void      _allocateEntry();   /**< Initialize storage for credentials. */
  void      _retrieveEntry(char* ssid, char* password, uint8_t* bssid);   /**< Read an available entry. */

  uint8_t   _entries;       /**< Count of the available entry */
  uint16_t  _containSize;   /**< Container size */
  int       _dp;            /**< The current address in EEPROM */
  int       _ep;            /**< The current entry address in EEPROM */
  uint16_t  _offset;        /**< The offset for the saved area of credentials in EEPROM. */
};

#endif  // _AUTOCONNECTCREDENTIAL_H_
