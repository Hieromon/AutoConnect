/*
  CreditMigrate.ino
  Copyright (c) 2019, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect
  This software is released under the MIT License.
  https://opensource.org/licenses/MIT
  This sketch migrates the credentials past saved to EEPROM in ESP32 to
  Preferences.
 */

#ifndef ARDUINO_ARCH_ESP32
#error This sketch should be compiled with the board of ESP32.
#endif

#include <Arduino.h>
#include <EEPROM.h>
#include <esp_partition.h>
#include <AutoConnectCredential.h>
#include <string.h>

/**
 * Retrieve saved credentials from eeprom partition.
 * @param  size  Returns a size of the eeprom partition
 * @return Retrieved data buffered pointer
 */
uint8_t* retrievePartition(size_t *size) {
  const esp_partition_t*  eeprom = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "eeprom");

  if (!eeprom) {
    Serial.println("EEPROM partition not found");
    return nullptr;
  }
  uint8_t*  pBuf = (uint8_t*)malloc(eeprom->size);
  if (!pBuf) {
    Serial.println("Insufficient memory to retrieve EEPROM partition");
    return nullptr;
  }
  if (esp_partition_read(eeprom, 0, (void*)pBuf, eeprom->size) != ESP_OK) {
    Serial.println("Unable to read EEPROM partition");
    free(pBuf);
    return nullptr;
  }
  *size = eeprom->size;
  return pBuf;
}

/**
 * Write credentials in retrieved buffer to Preferences.
 * @param  eeprom  Retrieved data buffered pointer
 * @param  size    Retrieved data size
 */
void convert(uint8_t* eeprom, size_t size) {
  uint8_t*  ac_credt = (uint8_t*)strstr((const char*)eeprom, "AC_CREDT");
  if (!ac_credt)
    Serial.println("AC_CREDT identifier not found in EEPROM partition.");
  else {
    AutoConnectCredential credential;
    uint8_t*  bp = ac_credt + sizeof("AC_CREDT") - 1;
    uint8_t*  dp = bp;
    uint8_t   entries = *dp++;
    size_t    dpSize = *dp++;
    dpSize += *dp++ << 8;
    Serial.printf("%d stored credential(s), size:%d\n", (int)entries, dpSize);

    // Start EEPROM to Preferences migration
    uint8_t*  ep = dp + dpSize - 1;
    for (int ec = 1; dp <= ep; ec++) {

      // Skip erased entry
      while (*dp == 0xff) {
        if (++dp > ep)
          break;
      }
      if (dp > ep)  // It reached at the end of the credential region.
        break;

      // Obtain each entry and store to Preferences
      struct station_config config;
      Serial.printf("[%d] ", ec);
      uint8_t ei = 0;
      do {
        config.ssid[ei++] = *dp;  
      } while (*dp++);
      Serial.print((char*)config.ssid);
      ei = 0;
      do {
        config.password[ei++] = *dp;
      } while (*dp++);
      Serial.printf("(%s)", config.password);
      for (ei = 0; ei < sizeof(config.bssid); ei++) {
        config.bssid[ei] = *dp++;
        Serial.printf(":%02x", config.bssid[ei]);
      }
      bool rc = credential.save(&config);
      Serial.println(rc ? " saved" : " failed to save");
    }
  }
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  size_t    eepromSize;
  uint8_t*  eepromData = retrievePartition(&eepromSize);
  if (eepromData) {
    Serial.println("Start EEPROM migration to Preferences");
    convert(eepromData, eepromSize);
    Serial.println("Conversion ended");
    free(eepromData);
  }
}

void loop() {}
