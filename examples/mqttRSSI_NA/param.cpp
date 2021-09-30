/*
Persist the Thingspeak channel configuration that should be configured in
the mqttRSSI_NA.ino sketch. These functions adapt the MQTT broker
configuration settings persistence to either ESP8266 or ESP32 platforms.
These functions save the settings in the EEPROM for ESP8266 and in the
Preferences area for ESP32.

Copyright (c) 2021 Hieromon Ikasamo.
This software is released under the MIT License.
https://opensource.org/licenses/MIT
*/
#include <Arduino.h>
#include "param.h"

#if defined(ARDUINO_ARCH_ESP8266)
#include <EEPROM.h>
#include <AutoConnect.h>
extern AutoConnect   portal;

void getParams(mqtt_param_t& param) {
  // It is important to use getEEPROMUsedSize when the user sketch
  // allocates its own EEPROM area on the ESP8266. It avoids that area
  // colliding with the area where AutoConnect stores credentials.
  // Note that the return value of AutoConnect::getEEPROMUsedSize takes
  // effect after giving the boundaryOffset via AutoConnectConfig.
  EEPROM.begin(portal.getEEPROMUsedSize());

  EEPROM.get<mqtt_param_t>(0, param);
  EEPROM.end();

  // If the area read from the EEPROM is completely filled with 0xFF,
  // that area is before it is initialized with the sketch-owned area.
  if (*((uint8_t*)&param) == 0xff && !memcmp((uint8_t*)&param, (uint8_t*)&param + 1, sizeof(mqtt_param_t) - 1))
    memset(&param, 0x00, sizeof(mqtt_param_t));
}

void putParams(const mqtt_param_t& param) {
  EEPROM.begin(portal.getEEPROMUsedSize());
  EEPROM.put<mqtt_param_t>(0, param);
  EEPROM.end();
}

#elif defined(ARDUINO_ARCH_ESP32)
#include <Preferences.h>
// The Preferences allows your sketch-owned area to separate from the
// AutoConnect credentials storage area by namespace and key.
// If the sketch begins the Preferences before the namespace and key
// exist, you will get an unallocated error in the core, but you don't
// have to worry about that. The area is automatically allocated by
// Preferences::putBytes.
const char* PREFS_NAMESPACE = "MQTT";
const char* PREFS_KEY       = "MQTT";
Preferences prefs;

void getParams(mqtt_param_t &param) {
  memset(&param, 0x00, sizeof(mqtt_param_t));
  prefs.begin(PREFS_NAMESPACE, true);
  prefs.getBytes(PREFS_KEY, &param, sizeof(mqtt_param_t));
  prefs.end();
}

void putParams(const mqtt_param_t& param) {
  prefs.begin(PREFS_NAMESPACE);
  prefs.putBytes(PREFS_KEY, &param, sizeof(mqtt_param_t));
  prefs.end();
}

#endif
