/**
 *	AutoConnectCredential class implementation.
 *	@file	AutoConnectCredential.cpp
 *	@author	hieromon@gmail.com
 *	@version	0.9.5
 *	@date	2018-02-17
 *	@copyright	MIT license.
 */

#include <EEPROM.h>
#include "AutoConnectCredential.h"

#define AC_IDENTIFIER "AC_CREDT"
#define AC_HEADERSIZE ((int)(_offset + sizeof(AC_IDENTIFIER) - 1 + sizeof(uint8_t) + sizeof(uint16_t)))

/**
 *  AutoConnectCredential constructor takes the available count of saved
 *  entries.
 *  A stored credential data structure in EEPROM.
 *   0      7 8 9a b                                  (t)
 *  +--------+-+--+-----------------+-----------------+--+
 *  |AC_CREDT|e|ss|ssid\0pass\0bssid|ssid\0pass\0bssid|\0|
 *  +--------+-+--+-----------------+-----------------+--+
 *  AC_CREDT : Identifier. 8 characters.
 *  e  : Number of contained entries(uint8_t).
 *  ss : Container size, excluding ID and number of entries(uint16_t).
 *  ssid: SSID string with null termination.
 *  password : Password string with null termination.
 *  bssid : BSSID 6 bytes.
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
  uint8_t c;

  EEPROM.begin(AC_HEADERSIZE);

  // Validate the save area of the EEPROM.
  // If it is a valid area, retrieve the stored number of entries,
  // if the identifier is not saved, initialize the EEPROM area.
  _dp = _offset;
  for (c = 0; c < sizeof(id_c); c++) {
    id_c[c] = static_cast<char>(EEPROM.read(_dp++));
  }
  if (!strncmp(id_c, AC_IDENTIFIER, sizeof(id_c))) {
    _entries = EEPROM.read(static_cast<int>(_dp++));
    _containSize = EEPROM.read(static_cast<int>(_dp++));
    _containSize += EEPROM.read(static_cast<int>(_dp)) << 8;
  }
  else {
    _entries = 0;
    _containSize = 0;
  }

  EEPROM.end();
}

/**
 *  The destructor ends EEPROM access.
 */
AutoConnectCredential::~AutoConnectCredential() {
  EEPROM.end();
}

/**
 *  Delete the credential entry for the specified SSID in the EEPROM.
 *  @param  ssid    A SSID character string to be deleted.
 *  @retval true    The entry successfully delete.
 *          false   Could not deleted.
 */
bool AutoConnectCredential::del(const char* ssid) {
  struct station_config entry;
  bool  rc = false;

  if (load(ssid, &entry) >= 0) {
    // Saved credential detected, _ep has the entry location.
    EEPROM.begin(AC_HEADERSIZE + _containSize);
    _dp = _ep;

    // Erase SSID
    while (EEPROM.read(_dp) != 0x00)
      EEPROM.write(_dp++, 0xff);

    // Erase Password
    EEPROM.write(_dp++, 0xff);
    while (EEPROM.read(_dp) != 0x00)
      EEPROM.write(_dp++, 0xff);

    // Erase BSSID
    EEPROM.write(_dp++, 0xff);
    for (uint8_t i = 0; i < sizeof(station_config::bssid); i++)
      EEPROM.write(_dp++, 0xff);

    // End 0xff writing, update headers.
    _entries--;
    EEPROM.write(_offset + static_cast<int>(sizeof(AC_IDENTIFIER)) - 1, _entries);

    // commit it.
    rc = EEPROM.commit();
    delay(10);
    EEPROM.end();
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
int8_t AutoConnectCredential::load(const char* ssid, struct station_config* config) {
  int8_t  entry = -1;

  _dp = AC_HEADERSIZE;
  if (_entries) {
    EEPROM.begin(AC_HEADERSIZE + _containSize);
    for (uint8_t i = 0; i < _entries; i++) {
      _retrieveEntry(reinterpret_cast<char*>(config->ssid), reinterpret_cast<char*>(config->password), config->bssid);
      if (!strcmp(ssid, (const char*)config->ssid)) {
        entry = i;
        break;
      }
    }
    EEPROM.end();
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
bool AutoConnectCredential::load(int8_t entry, struct station_config* config) {
  _dp = AC_HEADERSIZE;
  if (_entries && entry < _entries) {
    EEPROM.begin(AC_HEADERSIZE + _containSize);
    while (entry-- >= 0)
      _retrieveEntry(reinterpret_cast<char*>(config->ssid), reinterpret_cast<char*>(config->password), config->bssid);
    EEPROM.end();
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
bool AutoConnectCredential::save(const struct station_config* config) {
  static const char _id[] = AC_IDENTIFIER;
  struct station_config  stage;
  int8_t  entry;
  bool    rep = false;

  // Detect same entry for replacement.
  entry = load((const char*)(config->ssid), &stage);

  // Saving start.
  EEPROM.begin(AC_HEADERSIZE + _containSize + sizeof(struct station_config));

  // Determine insertion or replacement.
  if (entry >= 0) {
    // An entry with the same SSID was found, release the area for replacement.
    _dp = _ep;
    for (uint8_t dm = 2; dm; _dp++) {
      if (EEPROM.read(_dp) == '\0')
        dm--;
      EEPROM.write(_dp, 0xff);    // Clear SSID, Passphrase
    }
    for (uint8_t i = 0; i < sizeof(station_config::bssid); i++)
      EEPROM.write(_dp++, 0xff);  // Clear BSSID
  }
  else {
    // Same entry not found. increase the entry.
    _entries++;
    int i;
    for (i = 0; i < static_cast<int>(sizeof(_id)) - 1; i++)
      EEPROM.write(i + _offset, (uint8_t)_id[i]);
    EEPROM.write(i + _offset, _entries);
  }

  // Seek insertion point, evaluate capacity to insert the new entry.
  uint8_t eSize = strlen((const char*)config->ssid) + strlen((const char*)config->password) + sizeof(station_config::bssid) + 2;
  for (_dp = AC_HEADERSIZE; _dp < _containSize + AC_HEADERSIZE; _dp++) {
    if (EEPROM.read(_dp) == 0xff) {
      uint8_t fp = _dp;
      while (EEPROM.read(++_dp) == 0xff) {}
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
    EEPROM.write(_dp++, c);
  } while (c != '\0');
  dt = config->password;
  do {  // Write password
    c = *dt++;
    EEPROM.write(_dp++, c);
  } while (c != '\0');
  for (uint8_t i = 0; i < sizeof(station_config::bssid); i++)
    EEPROM.write(_dp++, config->bssid[i]);  // write BSSID
  // Terminate container, mark to the end of credential area.
  // When the entry is replaced, not mark a terminator.
  if (!rep) {
    EEPROM.write(_dp, '\0');

    // Update container size
    _containSize = _dp - AC_HEADERSIZE;
    EEPROM.write(_offset + sizeof(AC_IDENTIFIER) - 1 + sizeof(uint8_t), (uint8_t)_containSize);
    EEPROM.write(_offset + sizeof(AC_IDENTIFIER) - 1 + sizeof(uint8_t) + 1, (uint8_t)(_containSize >> 8));
  }

  bool rc = EEPROM.commit();
  delay(10);
  EEPROM.end();

  return rc;
}

/**
 *  Get the SSID and password from EEPROM indicated by _dp as the pointer
 *  of current read address. FF is skipped as unavailable area.
 *  @param  ssid      A SSID storing address.
 *  @param  password  A password storing address.
 */
void AutoConnectCredential::_retrieveEntry(char* ssid, char* password, uint8_t* bssid) {
  uint8_t ec;

  // Skip unavailable entry.
  while ((ec = EEPROM.read(_dp++)) == 0xff) {}

  // Retrieve SSID
  _ep = _dp - 1;
  *ssid++ = ec;
  do {
    ec = EEPROM.read(_dp++);
    *ssid++ = ec;
  } while (ec != '\0');
  // Retrieve Password
  do {
    ec = EEPROM.read(_dp++);
    *password++ = ec;
  } while (ec != '\0');
  // Retrieve BSSID
  for (uint8_t i = 0; i < sizeof(station_config::bssid); i++) {
    bssid[i] = EEPROM.read(_dp++);
  }
}
