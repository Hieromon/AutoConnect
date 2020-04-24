/*
  EEPROM.ino, Example for the AutoConnect library.
  Copyright (c) 2020, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect
  This software is released under the MIT License.
  https://opensource.org/licenses/MIT

  The AutoConnectConfig::boundaryOffset setting allows AutoConnect to
  write its data to EEPROM while preserving custom configuration data.
  Similarly, when a Sketch writes its own data to EEPROM, it must
  preserve the data written by AutoConnect.
  This example demonstrates how to use the getEEPROMUsedSize() method to
  store custom configuration settings in EEPROM without conflicting with
  AutoConnect's use of that storage. (Note: this applies to the ESP8266
  only, not the ESP32.)
*/

#ifndef ARDUINO_ARCH_ESP8266
#error "EEPROM.ino sketch is available for ESP8266 only."
#endif

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <AutoConnect.h>

ESP8266WebServer  server;
AutoConnect       portal(server);
AutoConnectConfig config;

const char AUX_EEPROM_IO[] PROGMEM = R"(
[
  {
    "title": "EEPROM",
    "uri": "/",
    "menu": true,
    "element": [
      {
        "name": "data1",
        "type": "ACInput",
        "label": "DATA1",
        "global": true
      },
      {
        "name": "data2",
        "type": "ACInput",
        "label": "DATA2",
        "global": true
      },
      {
        "name": "data3",
        "type": "ACInput",
        "label": "DATA3",
        "global": true
      },
      {
        "name": "write",
        "type": "ACSubmit",
        "value": "WRITE",
        "uri": "/eeprom"
      }
    ]
  },
  {
    "title": "EEPROM",
    "uri": "/eeprom",
    "menu": false,
    "element": [
      {
        "name": "data1",
        "type": "ACText",
        "format": "DATA1: %s",
        "posterior": "br",
        "global": true
      },
      {
        "name": "data2",
        "type": "ACText",
        "format": "DATA2: %s",
        "posterior": "br",
        "global": true
      },
      {
        "name": "data3",
        "type": "ACText",
        "format": "DATA3: %s",
        "posterior": "br",
        "global": true
      }
    ]
  }
]
)";

// Defines the custom data should be stored in EEPROM.
typedef struct {
  char  data1[8];
  char  data2[8];
  char  data3[8];
} EEPROM_CONFIG_t;

String toString(char* c, uint8_t length) {
  String  r;
  while (length-- && *c) {
    r += (isAlphaNumeric(*c) ? String(*c) : String(*c, HEX));
    c++;
  }
  return r;
}

// Read from EEPROM
String onEEPROM(AutoConnectAux& page, PageArgument& args) {
  EEPROM_CONFIG_t eepromConfig;

  EEPROM.begin(sizeof(eepromConfig));
  EEPROM.get<EEPROM_CONFIG_t>(0, eepromConfig);
  EEPROM.end();

  page["data1"].value = toString(eepromConfig.data1, sizeof(EEPROM_CONFIG_t::data1));
  page["data2"].value = toString(eepromConfig.data2, sizeof(EEPROM_CONFIG_t::data2));
  page["data3"].value = toString(eepromConfig.data3, sizeof(EEPROM_CONFIG_t::data3));
  return String();
}

// Write to EEPROM
String onEEPROMWrite(AutoConnectAux& page, PageArgument& args) {
  EEPROM_CONFIG_t eepromConfig;

  strncpy(eepromConfig.data1, page["data1"].value.c_str(), sizeof(EEPROM_CONFIG_t::data1));
  strncpy(eepromConfig.data2, page["data2"].value.c_str(), sizeof(EEPROM_CONFIG_t::data2));
  strncpy(eepromConfig.data3, page["data3"].value.c_str(), sizeof(EEPROM_CONFIG_t::data3));

  // The actual area size of the EEPROM region to be given to
  // EEPROM.begin is the sum of the size of the own custom data and
  // the size of the currently stored AutoConnect credentials.
  // eg.
  //   EEPROM.begin(portal.getEEPROMUsedSize())
  EEPROM.begin(portal.getEEPROMUsedSize());
  EEPROM.put<EEPROM_CONFIG_t>(0, eepromConfig);
  EEPROM.commit();
  EEPROM.end();
  return String();
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  portal.load(FPSTR(AUX_EEPROM_IO));
  portal.on("/", onEEPROM);
  portal.on("/eeprom", onEEPROMWrite);

  // Specifies to shift the AutoConnect credentials with the custom data region size.
  config.boundaryOffset = sizeof(EEPROM_CONFIG_t);
  portal.config(config);

  portal.begin();
}

void loop() {
  portal.handleClient();
}
