/**
 *	AutoConnectCredential class dispatcher.
 *	@file	AutoConnectCredential.cpp
 *	@author	hieromon@gmail.com
 *	@version	1.1.0
 *	@date	2019-10-07
 *	@copyright	MIT license.
 */

#include "AutoConnectCredential.h"

#if AC_CREDENTIAL_PREFERENCES == 0

#define AC_HEADERSIZE ((int)(_offset + sizeof(AC_IDENTIFIER) - 1 + sizeof(uint8_t) + sizeof(uint16_t)))
/**
 *  AutoConnectCredential constructor takes the available count of saved
 *  entries.
 *  A stored credential data structure in EEPROM.
 *   0      7 8 9a b                (u)                  (u+16)            (t)
 *  +--------+-+--+-----------------+-+--+--+--+----+----+-----------------+--+
 *  |AC_CREDT|e|ss|ssid\0pass\0bssid|d|ip|gw|nm|dns1|dns2|ssid\0pass\0bssid|\0|
 *  +--------+-+--+-----------------+-+--+--+--+----+----+-----------------+--+
 *  AC_CREDT : Identifier. 8 characters.
 *  e  : Number of contained entries(uint8_t).
 *  ss : Container size, excluding ID and number of entries(uint16_t).
 *  ssid: SSID string with null termination.
 *  password : Password string with null termination.
 *  bssid : BSSID 6 bytes.
 *  d  : DHCP is in available. 0:DHCP 1:Static IP
 *  ip - dns2 : Optional fields for static IPs configuration, these fields are available when d=1.
 *  ip : Static IP (uint32_t)
 *  gw : Gateway address (uint32_t)
 *  nm : Netmask (uint32_t)
 *  dns1 : Primary DNS (uint32)
 *  dns2 : Secondary DNS (uint32_t)
 *  t  : The end of the container is a continuous '\0'.
 *  The AC_CREDT identifier is at the beginning of the area.
 *  SSID and PASSWORD are terminated by '\ 0'.
 *  Free area are filled with FF, which is reused as an area for insertion.
 */
AutoConnectCredential::AutoConnectCredential() {
  _offset = AC_IDENTIFIER_OFFSET;
  _allocateEntry();
}

AutoConnectCredential::AutoConnectCredential(uint16_t offset) {
  // Save offset for the credential area.
  _offset = offset;
  _allocateEntry();
}

void AutoConnectCredential::_allocateEntry(void) {
  char    id_c[sizeof(AC_IDENTIFIER) - 1];

  _eeprom.reset(new EEPROMClass);
  _eeprom->begin(AC_HEADERSIZE);

  // Validate the save area of the EEPROM.
  // If it is a valid area, retrieve the stored number of entries,
  // if the identifier is not saved, initialize the EEPROM area.
  _dp = _offset;
  for (uint8_t c = 0; c < sizeof(id_c); c++)
    id_c[c] = static_cast<char>(_eeprom->read(_dp++));
  if (!strncmp(id_c, AC_IDENTIFIER, sizeof(id_c))) {
    _entries = _eeprom->read(static_cast<int>(_dp++));
    _containSize = _eeprom->read(static_cast<int>(_dp++));
    _containSize += _eeprom->read(static_cast<int>(_dp)) << 8;
  }
  else {
    _entries = 0;
    _containSize = 0;
  }
  _eeprom->end();
}

/**
 *  The destructor ends EEPROM access.
 */
AutoConnectCredential::~AutoConnectCredential() {
  _eeprom->end();
  _eeprom.reset();
}

/**
 *  Delete the credential entry for the specified SSID in the EEPROM.
 *  @param  ssid    A SSID character string to be deleted.
 *  @retval true    The entry successfully delete.
 *          false   Could not deleted.
 */
bool AutoConnectCredential::del(const char* ssid) {
  station_config_t  entry;
  bool  rc = false;

  if (load(ssid, &entry) >= 0) {
    // Saved credential detected, _ep has the entry location.
    _eeprom->begin(AC_HEADERSIZE + _containSize);
    _dp = _ep;

    // Erase SSID
    while (_eeprom->read(_dp) != 0x00)
      _eeprom->write(_dp++, 0xff);

    // Erase Password
    _eeprom->write(_dp++, 0xff);
    while (_eeprom->read(_dp) != 0x00)
      _eeprom->write(_dp++, 0xff);

    // Erase BSSID
    _eeprom->write(_dp++, 0xff);
    for (uint8_t i = 0; i < sizeof(station_config_t::bssid); i++)
      _eeprom->write(_dp++, 0xff);

    // Erase ip configuration extention
    if (_eeprom->read(_dp) == STA_STATIC) {
      for (uint8_t i = 0; i < sizeof(station_config_t::_config); i++)
        _eeprom->write(_dp++, 0xff);
    }

    // End 0xff writing, update headers.
    _entries--;
    _eeprom->write(_offset + static_cast<int>(sizeof(AC_IDENTIFIER)) - 1, _entries);

    // commit it.
    rc = _eeprom->commit();
    delay(10);
    _eeprom->end();
  }
  return rc;
}

/**
 *  Load the credential entry for the specified SSID from the EEPROM.
 *  The credentials are stored to the station_config structure which specified
 *  by *config as the SSID and password.
 *  @param  ssid    A SSID character string to be loaded.
 *  @param  config  A station_config structure pointer.
 *  @retval The entry number of the SSID in EEPROM. If the number less than 0,
 *  the specified SSID was not found.
 */
int8_t AutoConnectCredential::load(const char* ssid, station_config_t* config) {
  int8_t  entry = -1;

  _dp = AC_HEADERSIZE;
  if (_entries) {
    _eeprom->begin(AC_HEADERSIZE + _containSize);
    for (uint8_t i = 0; i < _entries; i++) {
      _retrieveEntry(config);
      if (!strcmp(ssid, reinterpret_cast<const char*>(config->ssid))) {
        entry = i;
        break;
      }
    }
    _eeprom->end();
  }
  return entry;
}

/**
 *  Load the credential entry for the specified number from the EEPROM.
 *  The credentials are stored to the station_config structure which specified
 *  by *config as the SSID and password.
 *  @param  entry   A number of entry to be loaded.
 *  @param  config  A station_config structure pointer.
 *  @retval true    The entry number of the SSID in EEPROM.
 *          false   The number is not available.
 */
bool AutoConnectCredential::load(int8_t entry, station_config_t* config) {
  _dp = AC_HEADERSIZE;
  if (_entries && entry < _entries) {
    _eeprom->begin(AC_HEADERSIZE + _containSize);
    while (entry-- >= 0)
      _retrieveEntry(config);
    _eeprom->end();
    return true;
  }
  else {
    return false;
  }
}

/**
 *  Save SSID and password to EEPROM.
 *  When the same SSID already exists, it will be replaced. If the current
 *  entry size insufficient for the new entry, the entry will be appended
 *  and increase whole size. Its previous areas are freed with FF and reused.
 *  @param  config  A pointer to the station_config structure storing SSID and password.
 *  @retval true    Successfully saved.
 *  @retval false   EEPROM commit failed.
 */
bool AutoConnectCredential::save(const station_config_t* config) {
  static const char _id[] = AC_IDENTIFIER;
  station_config_t  stage;
  int8_t  entry;
  bool    rep = false;
  bool    rc;

  // Detect same entry for replacement.
  entry = load(reinterpret_cast<const char*>(config->ssid), &stage);

  // Saving start.
  _eeprom->begin(AC_HEADERSIZE + _containSize + sizeof(station_config_t));

  // Determine insertion or replacement.
  if (entry >= 0) {
    // An entry with the same SSID was found, release the area for replacement.
    _dp = _ep;
    for (uint8_t dm = 2; dm; _dp++) {
      if (_eeprom->read(_dp) == '\0')
        dm--;
      _eeprom->write(_dp, 0xff);    // Clear SSID, Passphrase
    }
    for (uint8_t i = 0; i < sizeof(station_config_t::bssid); i++) {
      _eeprom->write(_dp++, 0xff);  // Clear BSSID
    }
    uint8_t ss = _eeprom->read(_dp); // Read dhcp assignment flag
    _eeprom->write(_dp++, 0xff);    // Clear dhcp
    if (ss == (uint8_t)STA_STATIC) {
      for (uint8_t i = 0 ; i < sizeof(station_config_t::_config); i++)
      _eeprom->write(_dp++, 0xff);  // Clear static IPs
    }
  }
  else {
    // Same entry not found. increase the entry.
    _entries++;
    int i;
    for (i = 0; i < static_cast<int>(sizeof(_id)) - 1; i++)
      _eeprom->write(i + _offset, (uint8_t)_id[i]);
    _eeprom->write(i + _offset, _entries);
  }

  rc = _eeprom->commit();
  delay(10);

  // Seek insertion point, evaluate capacity to insert the new entry.
  uint16_t eSize = strlen(reinterpret_cast<const char*>(config->ssid)) + strlen(reinterpret_cast<const char*>(config->password)) + sizeof(station_config_t::bssid) + sizeof(station_config_t::dhcp);
  if (config->dhcp == (uint8_t)STA_STATIC)
    eSize += sizeof(station_config_t::_config);
  eSize += sizeof('\0') + sizeof('\0');

  for (_dp = AC_HEADERSIZE; _dp < _containSize + AC_HEADERSIZE; _dp++) {
    uint8_t c = _eeprom->read(_dp);
    if (c == 0xff) {
      uint16_t fp = _dp;
      while (_eeprom->read(++_dp) == 0xff) {}
      if (_dp - fp >= eSize) {
        _dp = fp;
        rep = true;
        break;
      }
      _dp--;
    }
  }

  // Save new entry
  uint8_t         c;
  const uint8_t*  dt;
  dt = config->ssid;
  do {  // Write SSID
    c = *dt++;
    _eeprom->write(_dp++, c);
  } while (c != '\0');
  dt = config->password;
  do {  // Write password
    c = *dt++;
    _eeprom->write(_dp++, c);
  } while (c != '\0');
  for (uint8_t i = 0; i < sizeof(station_config_t::bssid); i++)
    _eeprom->write(_dp++, config->bssid[i]);  // write BSSID
  _eeprom->write(_dp++, config->dhcp); // write dhcp flag
  if (config->dhcp == (uint8_t)STA_STATIC) {
    for (uint8_t e = 0; e < sizeof(station_config_t::_config::addr) / sizeof(uint32_t); e++) {
      uint32_t  ip = config->config.addr[e];
      for (uint8_t b = 1; b <= sizeof(ip); b++)
        _eeprom->write(_dp++, ((uint8_t*)&ip)[sizeof(ip) - b]);
    }
  }

  // Terminate container, mark to the end of credential area.
  // When the entry is replaced, not mark a terminator.
  if (!rep) {
    _eeprom->write(_dp, '\0');

    // Update container size
    _containSize = _dp - AC_HEADERSIZE;
    _eeprom->write(_offset + sizeof(AC_IDENTIFIER) - 1 + sizeof(uint8_t), (uint8_t)_containSize);
    _eeprom->write(_offset + sizeof(AC_IDENTIFIER) - 1 + sizeof(uint8_t) + 1, (uint8_t)(_containSize >> 8));
  }

  rc &= _eeprom->commit();
  delay(10);
  _eeprom->end();

  return rc;
}

/**
 *  Get the SSID and password from EEPROM indicated by _dp as the pointer
 *  of current read address. FF is skipped as unavailable area.
 *  @param  ssid      A SSID storing address.
 *  @param  password  A password storing address.
 */
void AutoConnectCredential::_retrieveEntry(station_config_t* config) {
  uint8_t ec;

  // Skip unavailable entry.
  while ((ec = _eeprom->read(_dp++)) == 0xff) {}
  _ep = _dp - 1;

  // Retrieve SSID
  uint8_t* bp = config->ssid;
  *bp++ = ec;
  do {
    ec = _eeprom->read(_dp++);
    *bp++ = ec;
  } while (ec != '\0');
  // Retrieve Password
  bp = config->password;
  do {
    ec = _eeprom->read(_dp++);
    *bp++ = ec;
  } while (ec != '\0');
  // Retrieve BSSID
  for (uint8_t i = 0; i < sizeof(station_config_t::bssid); i++)
    config->bssid[i] = _eeprom->read(_dp++);
  // Extended readout for static IP
  config->dhcp = _eeprom->read(_dp++);
  if (config->dhcp == (uint8_t)STA_STATIC) {
    for (uint8_t e = 0; e < sizeof(station_config_t::_config::addr) / sizeof(uint32_t); e++) {
      uint32_t* ip = &config->config.addr[e];
      *ip = 0;
      for (uint8_t b = 0; b < sizeof(uint32_t); b++) {
        *ip <<= 8;
        *ip += _eeprom->read(_dp++);
      }
    }
  }
}

#else

/**
 *  AutoConnectCredential constructor takes the available count of saved
 *  entries.
 *  The credential area in the flash used by AutoConnect was moved from
 *  EEPROM to NVS with v.1.0.0. A stored credential data structure of
 *  Preferences is as follows. It has no identifier as AC_CREDT.
 *   0 12 3                (u)                  (u+16)            (t)
 *  +-+--+-----------------+-+--+--+--+----+----+-----------------+--+
 *  |e|ss|ssid\0pass\0bssid|d|ip|gw|nm|dns1|dns2|ssid\0pass\0bssid|\0|
 *  +-+--+-----------------+-+--+--+--+----+----+-----------------+--+
 *  e  : Number of contained entries(uint8_t).
 *  ss : Container size, excluding ID and number of entries(uint16_t).
 *  ssid: SSID string with null termination.
 *  password : Password string with null termination.
 *  bssid : BSSID 6 bytes.
 *  d  : DHCP is in available. 0:DHCP 1:Static IP
 *  ip - dns2 : Optional fields for static IPs configuration, these fields are available when d=1.
 *  ip : Static IP (uint32_t)
 *  gw : Gateway address (uint32_t)
 *  nm : Netmask (uint32_t)
 *  dns1 : Primary DNS (uint32)
 *  dns2 : Secondary DNS (uint32_t)
 *  t  : The end of the container is a continuous '\0'.
 *  SSID and PASSWORD are terminated by '\ 0'.
 */
AutoConnectCredential::AutoConnectCredential() {
  _allocateEntry();
}

AutoConnectCredential::AutoConnectCredential(uint16_t offset) {
  // In ESP32, always use from the beginning of the Preferences area.
  // The offset parameter is invalid but preserved for backward compatibility.
  (void)(offset);
  _allocateEntry();
}

void AutoConnectCredential::_allocateEntry(void) {
  _pref.reset(new Preferences);
  _entries = _import();
}

AutoConnectCredential::~AutoConnectCredential() {
  _credit.clear();
  _pref.reset();
}

/**
 *  Delete the credential entry for the specified SSID in the EEPROM.
 *  @param  ssid    A SSID character string to be deleted.
 *  @retval true    The entry successfully delete.
 *          false   Could not deleted.
 */
inline bool AutoConnectCredential::del(const char* ssid) {
  return _del(ssid, true);
}

/**
 *  Refresh the number of stored credential entries.
 *  @retval A number of entries.
 */
inline uint8_t AutoConnectCredential::entries(void) {
  _entries = _credit.size();
  return _entries;
}

/**
 *  Load the credential entry for the specified SSID from the internal
 *  dictionary. The credentials are stored to the station_config
 *  structure which specified by *config as the SSID and password.
 *  @param  ssid    A SSID character string to be loaded.
 *  @param  config  A station_config structure pointer.
 *  @retval The entry number of the SSID. If the number less than 0,
 *  the specified SSID was not found.
 */
int8_t AutoConnectCredential::load(const char* ssid, station_config_t* config) {
  // Determine the number in entries
  int8_t  en = 0;
  _entries = _import(); // Reload the saved credentials
  for (decltype(_credit)::iterator it = _credit.begin(), e = _credit.end(); it != e; ++it) {
    String  key = it->first;
    if (!strcmp(ssid, key.c_str())) {
      _obtain(it, config);
      return en;
    }
    en++;
  }
  return -1;
}

/**
 *  Load the credential entry for the specified number from the internal
 *  dictionary. The credentials are stored to the station_config
 *  structure which specified *config as the SSID and password.
 *  @param  entry   A number of entry to be loaded.
 *  @param  config  A station_config structure pointer.
 *  @retval true    The entry number of the SSID.
 *          false   The number is not available.
 */
bool AutoConnectCredential::load(int8_t entry, station_config_t* config) {
  _entries = _import();
  for (decltype(_credit)::iterator it = _credit.begin(), e = _credit.end(); it != e; ++it) {
    if (!entry--) {
      _obtain(it, config);
      return true;
    }
  }
  return false;
}

/**
 *  Save SSID and password to Preferences.
 *  When the same SSID already exists, it will be replaced. If the current
 *  entry size insufficient for the new entry, the entry will be appended
 *  and increase whole size. Its previous areas are freed with FF and reused.
 *  @param  config  A pointer to the station_config structure storing SSID and password.
 *  @retval true    Successfully saved.
 *  @retval false   Preferences commit failed.
 */
bool AutoConnectCredential::save(const station_config_t* config) {
  if (_add(config)) {
    return _commit() > 0 ? true : false;
  }
  return false;
}

/**
 *  Add an entry to internal dictionary that is std::map structure.
 *  It adds an entry by the insert after will delete the same entry
 *  caused std:: map does not accept duplicate keys.
 *  @param  config  A pointer to the station_config structure storing SSID and password.
 *  @retval true    Successfully saved.
 *  @retval false   Preferences commit failed.
 */
bool AutoConnectCredential::_add(const station_config_t* config) {
  const String  ssid = String(reinterpret_cast<const char*>(config->ssid));
  if (ssid.length() > 0) {

    // Remove a same entry to insert a new one.
    _del(ssid.c_str(), false);

    // Insert
    AC_CREDTBODY_t  credtBody;
    credtBody.password = String(reinterpret_cast<const char*>(config->password));
    memcpy(credtBody.bssid, config->bssid, sizeof(AC_CREDTBODY_t::bssid));
    credtBody.dhcp = config->dhcp;
    for (uint8_t e = 0; e < sizeof(AC_CREDTBODY_t::ip) / sizeof(uint32_t); e++)
      credtBody.ip[e] = credtBody.dhcp == (uint8_t)STA_STATIC ? config->config.addr[e] : 0U;
    std::pair<AC_CREDT_t::iterator, bool> rc = _credit.insert(std::make_pair(ssid, credtBody));
    _entries = _credit.size();
    #ifdef AC_DBG
    if (!rc.second) {
      AC_DBG("Failed to save a credential %s\n", config->ssid);
    }
    #endif
    return rc.second;
  }
  return false;
}

/**
 *  Serialize the AutoConnectCredential instance and write it back to NVS.
 */
size_t AutoConnectCredential::_commit(void) {
  AC_CREDTBODY_t credtBody;
  String  ssid;

  // Calculate the serialization size for each entry and add the size of 'e' with the size of 'ss' to it.
  size_t  sz = 0;
  for (const auto& credt : _credit) {
    ssid = credt.first;
    credtBody = credt.second;
    sz += ssid.length() + sizeof('\0') + credtBody.password.length() + sizeof('\0') + sizeof(AC_CREDTBODY_t::bssid) + sizeof(AC_CREDTBODY_t::dhcp);
    if (credtBody.dhcp == (uint32_t)STA_STATIC) {
      for (uint8_t e = 0; e < sizeof(AC_CREDTBODY_t::ip) / sizeof(uint32_t); e++)
        sz += sizeof(uint32_t);
    }
  }
  // When the entry is not empty, the size of container terminator as '\0' must be added.
  _containSize = sz + (_entries ? sizeof('\0') : 0);
  // Calculate the nvs pool size for saving to NVS. Add size of 'e' and 'ss' field.
  size_t  psz = _containSize + sizeof(uint8_t) + sizeof(uint16_t);

  // Dump container to serialization pool and write it back to NVS.
  uint8_t* credtPool = (uint8_t*)malloc(psz);
  if (credtPool) {
    uint16_t dp = 0;
    credtPool[dp++] = _entries;  // 'e'
    credtPool[dp++] = (uint8_t)(psz & 0x00ff); // 'ss' low byte
    credtPool[dp++] = (uint8_t)(psz >> 8);     // 'ss' high byte
    // Starts dump of credential entries
    for (const auto& credt : _credit) {
      ssid = credt.first;       // Retrieve SSID
      credtBody = credt.second; // Retrieve an entry
      // SSID
      size_t  itemLen = ssid.length() + sizeof('\0');
      ssid.toCharArray(reinterpret_cast<char*>(&credtPool[dp]), itemLen);
      // Password
      dp += itemLen;
      itemLen = credtBody.password.length() + sizeof('\0');
      credtBody.password.toCharArray(reinterpret_cast<char*>(&credtPool[dp]), itemLen);
      // BSSID
      dp += itemLen;
      memcpy(&credtPool[dp], credtBody.bssid, sizeof(station_config_t::bssid));
      dp += sizeof(station_config_t::bssid);
      // DHCP/Static IP indicator
      credtPool[dp++] = (uint8_t)credtBody.dhcp;
      // Static IP configuration
      if (credtBody.dhcp == STA_STATIC) {
        for (uint8_t e = 0; e < sizeof(AC_CREDTBODY_t::ip) / sizeof(uint32_t); e++) {
          for (uint8_t b = 1; b <= sizeof(credtBody.ip[e]); b++)
            credtPool[dp++] = ((uint8_t*)&credtBody.ip[e])[sizeof(credtBody.ip[e]) - b];
        }
      }
    }
    if (_credit.size() > 0)
      credtPool[dp] = '\0'; // Terminates a container
    // Write back to the nvs
    if (_pref->begin(AC_CREDENTIAL_NVSNAME, false)) {
      sz = _pref->putBytes(AC_CREDENTIAL_NVSKEY, credtPool, psz);
      _pref->end();
    }
    #ifdef AC_DBG
    else {
      AC_DBG("Preferences begin failed to save " AC_CREDENTIAL_NVSKEY "\n");
    }
    #endif
    free(credtPool);
  }
  #ifdef AC_DBG
  else {
    AC_DBG("Preferences pool %d(B) allocation failed\n", psz);
  }
  #endif
  return sz;
}

/**
 *  Delete the credential entry Actually for the specified SSID from Preferences.
 *  @param  ssid    A SSID character string to be deleted.
 *  @param  commit  If false, delete only a credential entry without updating Preferences.
 *  @retval true    The entry successfully delete.
 *          false   Could not deleted.
 */
bool AutoConnectCredential::_del(const char* ssid, const bool commit) {
  decltype(_credit)::iterator it = _credit.find(String(ssid));
  if (it != _credit.end()) {
    _credit.erase(it);
    _entries = _credit.size();
    if (commit)
      _commit();
    return true;
  }
  return false;
}

/**
 *  Import the credentials bulk data as Preferences from NVS.
 *  In ESP32, AutoConnect stores credentials in NVS from v1.0.0.
 */
uint8_t AutoConnectCredential::_import(void) {
  uint8_t cn = 0;
  if (_pref->begin(AC_CREDENTIAL_NVSNAME, true)) {
    size_t  psz = _getPrefBytesLength<Preferences>(_pref.get(), AC_CREDENTIAL_NVSKEY);
    if (psz) {
      uint8_t* credtPool = (uint8_t*)malloc(psz);
      if (credtPool) {
        _pref->getBytes(AC_CREDENTIAL_NVSKEY, static_cast<void*>(credtPool), psz);
        _credit.clear();
        uint16_t  dp = 0;
        cn = credtPool[dp++];  // Retrieve 'e'
        _containSize = (uint16_t)credtPool[dp++];
        _containSize += (uint16_t)(credtPool[dp++] << 8); // Retrieve size of 'ss'
        // Starts import
        while (dp < psz - sizeof('\0')) {
          AC_CREDTBODY_t  credtBody;
          // SSID
          String  ssid = String(reinterpret_cast<const char*>(&credtPool[dp]));
          // Password
          dp += ssid.length() + sizeof('\0');
          credtBody.password = String(reinterpret_cast<const char*>(&credtPool[dp]));
          // BSSID
          dp += credtBody.password.length() + sizeof('\0');
          memcpy(credtBody.bssid, &credtPool[dp], sizeof(AC_CREDTBODY_t::bssid));
          dp += sizeof(AC_CREDTBODY_t::bssid);
          // DHCP/Static IP indicator
          credtBody.dhcp = credtPool[dp++];
          // Static IP configuration
          for (uint8_t e = 0; e < sizeof(AC_CREDTBODY_t::ip) / sizeof(uint32_t); e++) {
            uint32_t* ip = &credtBody.ip[e];
            *ip = 0U;
            if (credtBody.dhcp == (uint8_t)STA_STATIC) {
              for (uint8_t b = 0; b < sizeof(uint32_t); b++) {
                *ip <<= 8;
                *ip += credtPool[dp++];
              }
            }
          }
          // Make an entry
          _credit.insert(std::make_pair(ssid, credtBody));
        }
        free(credtPool);
      }
      #ifdef AC_DBG
      else {
        AC_DBG("Preferences pool %d(B) allocation failed\n", psz);
      }
      #endif
    }
    _pref->end();
  }
  #ifdef AC_DBG
  else {
    AC_DBG("Preferences begin failed to import " AC_CREDENTIAL_NVSKEY "\n");
  }
  #endif
  return cn;
}

/**
 *  Obtains an entry pointed to by the specified iterator from the
 *  dictionary as the std::map that maintains the credentials into the
 *  station_config structure.
 *  @param  it  An  iterator to an entry
 *  @param  config  the station_config structure storing SSID and password.
 */
void AutoConnectCredential::_obtain(AC_CREDT_t::iterator const& it, station_config_t* config) {
  String  ssid = it->first;
  AC_CREDTBODY_t&  credtBody = it->second;
  ssid.toCharArray(reinterpret_cast<char*>(config->ssid), sizeof(station_config_t::ssid));
  credtBody.password.toCharArray(reinterpret_cast<char*>(config->password), sizeof(station_config_t::password));
  memcpy(config->bssid, credtBody.bssid, sizeof(station_config_t::bssid));
  config->dhcp = credtBody.dhcp;
  for (uint8_t e = 0; e < sizeof(AC_CREDTBODY_t::ip) / sizeof(uint32_t); e++)
    config->config.addr[e] = credtBody.dhcp == (uint8_t)STA_STATIC ? credtBody.ip[e] : 0U;
}

#endif
