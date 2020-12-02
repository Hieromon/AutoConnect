/**
 *  Declaration of AutoConnectCredential class.
 *  @file AutoConnectCredential.h
 *  @author hieromon@gmail.com
 *  @version  1.2.0
 *  @date 2020-04-22
 *  @copyright  MIT license.
 */

#ifndef _AUTOCONNECTCREDENTIAL_H_
#define _AUTOCONNECTCREDENTIAL_H_

// The AUTOCONNECT_USE_PREFERENCES macro indicates which classes apply
// to the credentials storage structure either EEPROM or Preferences.
// It is a valid indicator only for ESP32.
// Undefine this macro, it maintains backward compatibility by applying
// EEPROM to the credentials storage class in the arduino-esp32 core
// v1.0.2 and earlier.
#define AUTOCONNECT_USE_PREFERENCES

#include <Arduino.h>
#include <memory>
#if defined(ARDUINO_ARCH_ESP8266)
#define AC_CREDENTIAL_PREFERENCES 0
extern "C" {
#include <user_interface.h>
}
#elif defined(ARDUINO_ARCH_ESP32)
#ifdef AUTOCONNECT_USE_PREFERENCES
#define AC_CREDENTIAL_PREFERENCES 1
#else
#define AC_CREDENTIAL_PREFERENCES 0
#endif
#include <esp_wifi.h>
#endif
#include "AutoConnectDefs.h"

/**
 * Credential storage area offset specifier in EEPROM.
 * By defining AC_IDENTIFIER_OFFSET macro in the user sketch, the credential
 * storage area can be shifted in EEPROM.
 */
#ifndef AC_IDENTIFIER_OFFSET
#define AC_IDENTIFIER_OFFSET  0
#endif

/**
 * Storage identifier for AutoConnect credentials. It is global constant
 * and reserved.
 */
#ifndef AC_IDENTIFIER
#define AC_IDENTIFIER "AC_CREDT"
#endif

typedef enum {
  STA_DHCP = 0,
  STA_STATIC
} station_config_dhcp;

typedef struct {
  uint8_t ssid[32];
  uint8_t password[64];
  uint8_t bssid[6];
  uint8_t dhcp;   /**< 0:DHCP, 1:Static IP */
  union _config {
    uint32_t  addr[5];
    struct _sta {
      uint32_t ip;
      uint32_t gateway;
      uint32_t netmask;
      uint32_t dns1;
      uint32_t dns2;
    } sta;
  } config;
} station_config_t;

class AutoConnectCredentialBase {
 public:
  explicit AutoConnectCredentialBase() : _entries(0), _containSize(0) {}
  virtual ~AutoConnectCredentialBase() {}
  virtual uint8_t entries(void) { return _entries; }
  virtual uint16_t dataSize(void) const { return sizeof(AC_IDENTIFIER) - 1 + sizeof(uint8_t) + sizeof(uint16_t) + _containSize; }
  virtual bool    del(const char* ssid) = 0;
  virtual int8_t  load(const char* ssid, station_config_t* config) = 0;
  virtual bool    load(int8_t entry, station_config_t* config) = 0;
  virtual bool    save(const station_config_t* config) = 0;

 protected:
  virtual void  _allocateEntry(void) = 0; /**< Initialize storage for credentials. */

  uint8_t   _entries;       /**< Count of the available entry */
  uint16_t  _containSize;   /**< Container size */
};

#if AC_CREDENTIAL_PREFERENCES == 0
// #pragma message "AutoConnectCredential applies the EEPROM"
#define NO_GLOBAL_EEPROM
#include <EEPROM.h>

/** AutoConnectCredential class using EEPROM for ESP8266 */
class AutoConnectCredential : public AutoConnectCredentialBase {
 public:
  AutoConnectCredential();
  explicit AutoConnectCredential(uint16_t offset);
  ~AutoConnectCredential();
  bool    del(const char* ssid) override;
  int8_t  load(const char* ssid, station_config_t* config) override;
  bool    load(int8_t entry, station_config_t* config) override;
  bool    save(const station_config_t* config) override;

 protected:
  void    _allocateEntry(void) override;  /**< Initialize storage for credentials. */

 private:
  void    _retrieveEntry(station_config_t* config);   /**< Read an available entry. */

  int       _dp;            /**< The current address in EEPROM */
  int       _ep;            /**< The current entry address in EEPROM */
  uint16_t  _offset;        /**< The offset for the saved area of credentials in EEPROM. */
  std::unique_ptr<EEPROMClass>  _eeprom;  /**< shared EEPROM class */
};

#else
// #pragma message "AutoConnectCredential applies the Preferences"
#include <type_traits>
#include <map>
#include <Preferences.h>
#include <nvs.h>

#define AC_CREDENTIAL_NVSNAME  AC_IDENTIFIER
#define AC_CREDENTIAL_NVSKEY   AC_CREDENTIAL_NVSNAME

/** Declare the member function existence determination */
namespace AutoConnectUtil {
AC_HAS_FUNC(getBytesLength);
}

/** AutoConnectCredential class using Preferences for ESP32 */
class AutoConnectCredential : public AutoConnectCredentialBase {
 public:
  AutoConnectCredential();
  explicit AutoConnectCredential(uint16_t offset);
  ~AutoConnectCredential();
  bool    del(const char* ssid) override;
  uint8_t entries(void) override;
  int8_t  load(const char* ssid, station_config_t* config) override;
  bool    load(int8_t entry, station_config_t* config) override;
  bool    save(const station_config_t* config) override;

 protected:
  void    _allocateEntry(void) override;  /**< Initialize storage for credentials. */

 private:
  typedef struct {
    String   password;
    uint8_t  bssid[6];
    uint8_t  dhcp;   /**< 1:DHCP, 2:Static IP */
    uint32_t ip[5];
  } AC_CREDTBODY_t;         /**< Credential entry */
  typedef std::map<String, AC_CREDTBODY_t>  AC_CREDT_t;

  bool    _add(const station_config_t* config); /**< Add an entry */
  size_t  _commit(void);    /**< Write back to the nvs */
  bool    _del(const char* ssid, const bool commit);  /**< Deletes an entry */
  uint8_t _import(void);    /**< Import from the nvs */
  void    _obtain(AC_CREDT_t::iterator const& it, station_config_t* config);  /**< Obtain an entry from iterator */
  template<typename T>
  typename std::enable_if<AutoConnectUtil::has_func_getBytesLength<T>::value, size_t>::type _getPrefBytesLength(T* pref, const char* key) {
    return pref->getBytesLength(key);
  }
  template<typename T>
  typename std::enable_if<!AutoConnectUtil::has_func_getBytesLength<T>::value, size_t>::type _getPrefBytesLength(T* pref, const char* key) {
    AC_UNUSED(pref);
    uint32_t  handle;
    size_t    len;
    esp_err_t err = nvs_open(AC_CREDENTIAL_NVSNAME, NVS_READONLY, &handle);
    if (err)
      len = 0;
    else {
      (void)nvs_get_blob(handle, key, NULL, &len);
      nvs_close(handle);
    }
    return len;
  }

  AC_CREDT_t  _credit;      /**< Dictionary to maintain the credentials */
  std::unique_ptr<Preferences>  _pref;  /**< Preferences class instance to access the nvs */
};

#endif

#endif  // _AUTOCONNECTCREDENTIAL_H_
