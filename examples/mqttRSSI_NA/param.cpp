/*
Persist the Thingspeak channel configuration that should be configured in
the mqttRSSI_NA.ino sketch. These functions adapt the MQTT broker
configuration settings persistence to either ESP8266 or ESP32 platforms.
These functions save the settings in the EEPROM for ESP8266 and in the
Preferences area for ESP32.

Copyright (c) 2022 Hieromon Ikasamo.
This software is released under the MIT License.
https://opensource.org/licenses/MIT
*/
#include <Arduino.h>
#include "param.h"

#if defined(ARDUINO_ARCH_ESP8266)
#include <EEPROM.h>
#include <AutoConnect.h>

extern AutoConnect   portal;
const char* _mqtt_param_id = MQTT_PARAM_ID;

void getParams(mqtt_param_t& param) {
  // It is important to use getEEPROMUsedSize when the user sketch
  // allocates its own EEPROM area on the ESP8266. It avoids that area
  // colliding with the area where AutoConnect stores credentials.
  // Note that the return value of AutoConnect::getEEPROMUsedSize takes
  // effect after giving the boundaryOffset via AutoConnectConfig.
  EEPROM.begin(portal.getEEPROMUsedSize());
  EEPROM.get<mqtt_param_t>(0, param);
  EEPROM.end();

  // If the area read from the EEPROM does not contain MQTT_PARAM_ID,
  // that area is before it is initialized with the sketch-owned area.
  if (strcmp(param.id, _mqtt_param_id) != 0)
    memset(&param, 0x00, sizeof(mqtt_param_t));
}

void putParams(const mqtt_param_t& param) {
  strcpy(const_cast<char*>(param.id), _mqtt_param_id);
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
const char* PREFS_NAMESPACE = MQTT_PARAM_ID;
const char* PREFS_KEY       = MQTT_PARAM_ID;
Preferences prefs;

void getParams(mqtt_param_t &param) {
  memset(&param, 0x00, sizeof(mqtt_param_t));
  if (prefs.begin(PREFS_NAMESPACE, true)) {
    prefs.getBytes(PREFS_KEY, &param, sizeof(mqtt_param_t));
    prefs.end();
  }
}

void putParams(const mqtt_param_t& param) {
  prefs.begin(PREFS_NAMESPACE);
  prefs.putBytes(PREFS_KEY, &param, sizeof(mqtt_param_t));
  prefs.end();
}

#endif
