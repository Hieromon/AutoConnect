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
uint8_t* retrievePartition(const char* name, size_t *size) {
  const esp_partition_t*  eeprom = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, name);

  if (!eeprom) {
    Serial.printf("%s partition not found\n", name);
    return nullptr;
  }
  uint8_t*  pBuf = (uint8_t*)malloc(eeprom->size);
  if (!pBuf) {
    Serial.printf("Insufficient memory to retrieve %s partition\n", name);
    return nullptr;
  }
  if (esp_partition_read(eeprom, 0, (void*)pBuf, eeprom->size) != ESP_OK) {
    Serial.printf("Unable to read %s partition\n", name);
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
void convert(const uint8_t* eeprom, const size_t size) {
  uint8_t*  ac_credt = (uint8_t*)strstr((const char*)eeprom, "AC_CREDT");
  if (!ac_credt)
    Serial.println("AC_CREDT identifier not found in the partition.");
  else {
    AutoConnectCredential credential;
    uint8_t*  bp = ac_credt + sizeof("AC_CREDT") - sizeof('\0');
    uint8_t*  dp = bp;
    uint8_t   entries = *dp++;
    size_t    dpSize = *dp++;
    dpSize += *dp++ << 8;
    Serial.printf("%d stored credential(s),size:%d\n", (int)entries, dpSize);

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
      station_config_t  config;
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
      config.dhcp = *dp++;
      if (config.dhcp == STA_STATIC) {
        for (uint8_t e = 0; e < sizeof(station_config_t::_config::addr) / sizeof(uint32_t); e++) {
          uint32_t* ip = &config.config.addr[e];
          *ip = 0;
          for (uint8_t b = 0; b < sizeof(uint32_t); b++) {
            *ip <<= 8;
            *ip += *dp++;
          }
        }
      }
      bool rc = credential.save(&config);
      Serial.println(rc ? " transferred" : " failed to save Preferences");
    }
  }
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  size_t    eepromSize;
  uint8_t*  eepromData = retrievePartition("eeprom", &eepromSize);
  if (eepromData) {
    Serial.println("Start migration to Preferences");
    convert(eepromData, eepromSize);
    Serial.println("Transfer ended");
    free(eepromData);
  }
}

void loop() {}
