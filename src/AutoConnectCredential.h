/**
 * Declaration of AutoConnectCredential class.
 * @file AutoConnectCredential.h
 * @author hieromon@gmail.com
 * @version  1.4.0
 * @date 2022-08-01
 * @copyright  MIT license.
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
#include "AutoConnectFS.h"

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
  explicit AutoConnectCredentialBase() : _entries(0), _containSize(0), _ensureFS(false) {}
  virtual ~AutoConnectCredentialBase() {}
  virtual uint8_t entries(void) { return _entries; }
  virtual uint16_t dataSize(void) const { return sizeof(AC_IDENTIFIER) - 1 + sizeof(uint8_t) + sizeof(uint16_t) + _containSize; }
  virtual bool    del(const char* ssid) = 0;
  virtual int8_t  load(const char* ssid, station_config_t* config) = 0;
  virtual bool    load(int8_t entry, station_config_t* config) = 0;
  virtual bool    save(const station_config_t* config) = 0;

 protected:
  virtual void  _allocateEntry(void) = 0; /**< Initialize storage for credentials. */

  // Start filesystem for backup and restore
  template<typename T>
  bool  _startFilesystem(T& fs, const bool forceMount) {
    _ensureFS = forceMount;
    if (_ensureFS)
      if (!fs.begin(AUTOCONNECT_FS_INITIALIZATION)) {
        AC_DBG("FS mount failed\n");
        return false;
      }
    return true;
  }

  // Start filesystem for backup and restore
  template<typename T>
  void  _endFilesystem(T& fs) {
    if (_ensureFS)
      fs.end();
  }

  uint8_t   _entries;       /**< Count of the available entry */
  uint16_t  _containSize;   /**< Container size */

 private:
  bool      _ensureFS;      /**< Filesystem starts with begin */
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
  template<typename T>
  bool  backup(const char* filename, T& fs, const bool ensureFS = true);
  template<typename T>
  bool  restore(const char* filename, T& fs, const bool ensureFS = true);

 protected:
  void    _allocateEntry(void) override;  /**< Initialize storage for credentials. */

 private:
  typedef struct __attribute__((__packed__)) {
    uint8_t ac_credt[sizeof(AC_IDENTIFIER) - sizeof('\0')];
    uint8_t e;
    uint16_t ss;
  } AC_CREDTHEADER_t;       /**< Header of AutConnectCredential */

  void    _retrieveEntry(station_config_t* config);   /**< Read an available entry. */

  int       _dp;            /**< The current address in EEPROM */
  int       _ep;            /**< The current entry address in EEPROM */
  uint16_t  _offset;        /**< The offset for the saved area of credentials in EEPROM. */
  std::unique_ptr<EEPROMClass>  _eeprom;  /**< shared EEPROM class */
};

/**
 * Save AutoConnectCredentials to the file system.
 * @param  filename Destination file name.
 * @param  fs       Destination file system.
 * @param  ensureFS Starts the file system on begin; omits starting the file system for the false.
 * @return true   All credentials successfully saved.
 * @return false  Could not save.
 */
template<typename T>
bool  AutoConnectCredential::backup(const char* filename, T& fs, const bool ensureFS) {
  if (!_startFilesystem<T>(fs, ensureFS))
    return false;

  bool  rc = false;
  uint16_t  stSize = _offset + dataSize();
  int dp = _offset;
  AC_CREDTHEADER_t  hdr;

  EEPROMClass eeprom;
  eeprom.begin(stSize);
  eeprom.get<AC_CREDTHEADER_t>(dp, hdr);
  if (!memcmp(hdr.ac_credt, AC_IDENTIFIER, sizeof(AC_CREDTHEADER_t::ac_credt))) {
    File  bf = fs.open(filename, "w");
    if (bf) {
      bf.write(reinterpret_cast<uint8_t*>(&hdr), sizeof(AC_CREDTHEADER_t));
      dp += sizeof(AC_CREDTHEADER_t);
      while ( dp < stSize)
        bf.write(eeprom.read(dp++));
      bf.close();
      AC_DBG("Credentials %" PRIu16 " bytes saved %s\n", stSize - _offset, filename);
      rc = true;
    }
    else {
      AC_DBG("Backup %s open failed\n", filename);
    }
  }
  else {
    AC_DBG("Cannot identify " AC_IDENTIFIER ", maybe boundaryOffset is wrong\n");
  }
  eeprom.end();
  _endFilesystem<T>(fs);
  return rc;
}

/**
 * Restore all credentials from specified file.
 * @param  filename Destination file name.
 * @param  fs       Destination file system.
 * @param  ensureFS Starts the file system on begin; omits starting the file system for the false.
 * @return true   Credentials successfully restored.
 * @return false  Could not restore.
 */
template<typename T>
bool  AutoConnectCredential::restore(const char* filename, T& fs, const bool ensureFS) {
  if (!_startFilesystem<T>(fs, ensureFS))
    return false;

  bool  rc = false;
  File  rf = fs.open(filename, "r");
  if (rf) {
    AC_CREDTHEADER_t  hdr;

    rf.readBytes(reinterpret_cast<char*>(&hdr), sizeof(AC_CREDTHEADER_t));
    if (!memcmp(hdr.ac_credt, AC_IDENTIFIER, sizeof(AC_CREDTHEADER_t::ac_credt))) {
      EEPROMClass eeprom;

      size_t  stSize = _offset + hdr.ss + (sizeof(AC_CREDTHEADER_t) - sizeof(AC_CREDTHEADER_t::ss));
      const char* sp = reinterpret_cast<const char*>(&hdr);
      int dp = _offset;
      AC_CREDTHEADER_t  eeprom_hdr;

      eeprom.begin(stSize);
      // Verify destination EEPROM as AutoConnectCredentail to be restored.
      eeprom.get<AC_CREDTHEADER_t>(dp, eeprom_hdr);
      if (memcmp(eeprom_hdr.ac_credt, AC_IDENTIFIER, sizeof(AC_CREDTHEADER_t::ac_credt))) {
        AC_DBG("Warning:EEPROM has no " AC_IDENTIFIER ", maybe boundaryOffset is wrong\n");
      }
      rc = true;
      // Restore the header
      while (dp < static_cast<int>(_offset + sizeof(AC_CREDTHEADER_t)))
        eeprom.write(dp++, *sp++);
      // Restore the body
      while (dp < static_cast<int>(stSize)) {
        int c = rf.read();
        if (c < 0) {
          AC_DBG("%s is unexpectedly EOF, broken\n", filename);
          rc = false;
          break;
        }
        eeprom.write(dp++, static_cast<uint8_t>(c));
      }
      eeprom.end();
    }
    else {
      AC_DBG("%s has no %s\n", filename, AC_IDENTIFIER);
    }
    rf.close();
  }
  else {
    AC_DBG("Restore %s open failed\n", filename);
  }
  _endFilesystem<T>(fs);

  if (rc) {
    AC_DBG("%s credentials restored\n", filename);
  }
  return rc;
}

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
  template<typename T>
  bool  backup(const char* filename, T& fs, const bool ensureFS = true);
  template<typename T>
  bool  restore(const char* filename, T& fs, const bool ensureFS = true);

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

/**
 * Save AutoConnectCredentials to the filesystem.
 * @param  filename Destination file name.
 * @param  fs       Destination file system.
 * @param  ensureFS Starts the file system on begin; omits starting the file system for the false.
 * @return true   All credentials successfully saved.
 * @return false  Could not save.
 */
template<typename T>
bool AutoConnectCredential::backup(const char* filename, T& fs, const bool ensureFS) {
  if (!_pref->begin(AC_CREDENTIAL_NVSNAME, true)) {
    AC_DBG(AC_CREDENTIAL_NVSNAME " no credentials storage\n");
    return false;
  }

  bool  rc = false;
  size_t  pfSize = _getPrefBytesLength<Preferences>(_pref.get(), AC_CREDENTIAL_NVSKEY);
  if (pfSize) {
    unsigned char*  pfPool = new unsigned char[pfSize];
    size_t  blSize = _pref->getBytes(AC_CREDENTIAL_NVSKEY, static_cast<void*>(pfPool), pfSize);

    if (blSize > 0) {
      if (_startFilesystem<T>(fs, ensureFS)) {
        File  bf = fs.open(filename, "w");
        if (bf) {
          bf.write(reinterpret_cast<const uint8_t*>(AC_IDENTIFIER), sizeof(AC_IDENTIFIER) - sizeof('\0'));
          for (unsigned char* dp = pfPool; blSize > 0; blSize--)
            bf.write(*dp++);
          bf.close();
          rc = true;
          AC_DBG("Credentials saved %s\n", filename);
        }
        else
          AC_DBG("Backup %s open failed\n", filename);
        _endFilesystem<T>(fs);
      }
    }
    else
      AC_DBG(AC_CREDENTIAL_NVSKEY " has no credentials\n");
    delete pfPool;
  }
  else
    AC_DBG("No " AC_CREDENTIAL_NVSKEY " storage\n");
  _pref->end();

  return rc;
}

/**
 * Restore all credentials from specified file.
 * @param  filename Destination file name.
 * @param  fs       Destination file system.
 * @param  ensureFS Starts the file system on begin; omits starting the file system for the false.
 * @return true   Credentials successfully restored.
 * @return false  Could not restore.
 */
template<typename T>
bool AutoConnectCredential::restore(const char* filename, T& fs, const bool ensureFS) {
  if (!_startFilesystem<T>(fs, ensureFS))
    return false;

  bool  rc = false;
  File  rf = fs.open(filename, "r");
  if (rf) {
    size_t  rfSize = rf.size();
    if (rfSize) {
      uint8_t*  rfPool = new unsigned char[rfSize];
      if (rfPool) {
        size_t  plSize = rf.read(rfPool, rfSize);
        size_t  idOffset = sizeof(AC_IDENTIFIER) - sizeof('\0');
        if (!memcmp(rfPool, AC_IDENTIFIER, idOffset)) {
          if (_pref->begin(AC_CREDENTIAL_NVSNAME, false)) {
            _pref->putBytes(AC_CREDENTIAL_NVSKEY, rfPool + idOffset, plSize - idOffset);
            _pref->end();
            rc = true;
            AC_DBG("%s credentials restored\n", filename);
          }
        }
        else
          AC_DBG("%s contains wrong %s\n", filename, AC_IDENTIFIER);
        delete rfPool;
      }
      else
        AC_DBG("Failed to allocate credential pool\n");
    }
    else
      AC_DBG("Restore %s open failed\n", filename);
    rf.close();
  }
  else
    AC_DBG("Credentails file %s open failed\n", filename);
  _endFilesystem<T>(fs);

  return rc;
}
#endif

#endif  // _AUTOCONNECTCREDENTIAL_H_
