/*
  ConfigIP.ino, Example for the AutoConnect library.
  Copyright (c) 2019, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect
  This software is released under the MIT License.
  https://opensource.org/licenses/MIT

  This sketch implements an example of enabling a custom web page
  according to the state of an externally equipped switch. The custom
  web page configures and provides a static IP to the ESP module.
  This example needs to equip an external switch circuit that pulls up
  with a resistor (1K to 10K ohms) and then drops it to GND via a push
  switch to connect to any GPIO of the ESP module.

  An external switch circuit:
     3.3V
    --+--
      |
     +-+
     | | 1K ~ 10K
     +-+
      |
      +--> D2 (for ESP8266, ex: GPIO16 in case of ESP32)
      |
    | O
  --+
    | O
      |
    --+--
     GND
*/

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#define EXTERNAL_SWITCH_PIN 4
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#define EXTERNAL_SWITCH_PIN 16
#endif
#include <AutoConnect.h>
#include <EEPROM.h>

static const char AUX_CONFIGIP[] PROGMEM = R"(
{
  "title": "Config IP",
  "uri": "/configip",
  "menu": true,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "value": "<b>Module IP configuration</b>",
      "style": "color:steelblue"
    },
    {
      "name": "mac",
      "type": "ACText",
      "format": "MAC: %s"
    },
    {
      "name": "staip",
      "type": "ACInput",
      "label": "IP",
      "pattern": "^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$",
      "global": true
    },
    {
      "name": "gateway",
      "type": "ACInput",
      "label": "Gateway",
      "pattern": "^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$",
      "global": true
    },
    {
      "name": "netmask",
      "type": "ACInput",
      "label": "Netmask",
      "pattern": "^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$",
      "global": true
    },
    {
      "name": "dns1",
      "type": "ACInput",
      "label": "DNS",
      "pattern": "^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$",
      "global": true
    },
    {
      "name": "ok",
      "type": "ACSubmit",
      "value": "OK",
      "uri": "/restart"
    },
    {
      "name": "cancel",
      "type": "ACSubmit",
      "value": "Cancel",
      "uri": "/_ac"
    }
  ]
}
)";

static const char AUX_RESTART[] PROGMEM = R"(
{
  "title": "Config IP",
  "uri": "/restart",
  "menu": false,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "value": "Settings",
      "style": "font-family:Arial;font-weight:bold;text-align:center;margin-bottom:10px;color:steelblue"
    },
    {
      "name": "staip",
      "type": "ACText",
      "format": "IP: %s",
      "global": true
    },
    {
      "name": "gateway",
      "type": "ACText",
      "format": "Gateway: %s",
      "global": true
    },
    {
      "name": "netmask",
      "type": "ACText",
      "format": "Netmask: %s",
      "global": true
    },
    {
      "name": "dns1",
      "type": "ACText",
      "format": "DNS1: %s",
      "global": true
    },
    {
      "name": "result",
      "type": "ACText",
      "posterior": "par"
    }
  ]
}
)";

AutoConnect portal;
AutoConnectConfig config;
AutoConnectAux    auxIPConfig;
AutoConnectAux    auxRestart;

// Pin assignment for an external configuration switch
uint8_t ConfigPin = EXTERNAL_SWITCH_PIN;
uint8_t ActiveLevel = LOW;

// EEPROM saving structure
typedef union {
  struct {
    uint32_t  ip;
    uint32_t  gateway;
    uint32_t  netmask;
    uint32_t  dns1;
  } ipconfig;
  uint8_t  ipraw[sizeof(uint32_t) * 4];
} IPCONFIG;

// Load IP configuration from EEPROM
void loadConfig(IPCONFIG* ipconfig) {
  EEPROM.begin(sizeof(IPCONFIG));
  int dp = 0;
  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t c = 0; c < sizeof(uint32_t); c++)
      ipconfig->ipraw[c + i * sizeof(uint32_t)] = EEPROM.read(dp++);
  }
  EEPROM.end();

  // Unset value screening
  if (ipconfig->ipconfig.ip == 0xffffffffL)
    ipconfig->ipconfig.ip = 0U;
  if (ipconfig->ipconfig.gateway == 0xffffffffL)
    ipconfig->ipconfig.gateway = 0U;
  if (ipconfig->ipconfig.netmask == 0xffffffffL)
    ipconfig->ipconfig.netmask = 0U;
  if (ipconfig->ipconfig.dns1 == 0xffffffffL)
    ipconfig->ipconfig.dns1 = 0U;

  Serial.println("IP configuration loaded");
  Serial.printf("Sta IP :0x%08lx\n", ipconfig->ipconfig.ip);
  Serial.printf("Gateway:0x%08lx\n", ipconfig->ipconfig.gateway);
  Serial.printf("Netmask:0x%08lx\n", ipconfig->ipconfig.netmask);
  Serial.printf("DNS1   :0x%08lx\n", ipconfig->ipconfig.dns1);
}

// Save current IP configuration to EEPROM
void saveConfig(const IPCONFIG* ipconfig) {
  // EEPROM.begin will truncate the area to the size given by the argument.
  // The part overflowing from the specified size is filled with 0xff,
  // so if the argument value is too small, the credentials may be lost.
  EEPROM.begin(128);

  int dp = 0;
  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t d = 0; d < sizeof(uint32_t); d++)
      EEPROM.write(dp++, ipconfig->ipraw[d + i * sizeof(uint32_t)]);
  }
  EEPROM.end();
  delay(100);
}

// Custom web page handler to set current configuration to the page
String getConfig(AutoConnectAux& aux, PageArgument& args) {
  IPCONFIG  ipconfig;
  loadConfig(&ipconfig);

  // Fetch MAC address
  String  macAddress;
  uint8_t mac[6];
  WiFi.macAddress(mac);
  for (uint8_t i = 0; i < 6; i++) {
    char buf[3];
    sprintf(buf, "%02X", mac[i]);
    macAddress += buf;
    if (i < 5)
      macAddress += ':';
  }
  aux["mac"].value = macAddress;

  // Fetch each IP address configuration from EEPROM
  IPAddress staip = IPAddress(ipconfig.ipconfig.ip);
  IPAddress gateway = IPAddress(ipconfig.ipconfig.gateway);
  IPAddress netmask = IPAddress(ipconfig.ipconfig.netmask);
  IPAddress dns1 = IPAddress(ipconfig.ipconfig.dns1);

  // Echo back the IP settings
  aux["staip"].value = staip.toString();
  aux["gateway"].value = gateway.toString();
  aux["netmask"].value = netmask.toString();
  aux["dns1"].value = dns1.toString();

  return String();
}

// Convert IP address from AutoConnectInput string value
void getIPAddress(String ipString, uint32_t* ip) {
  IPAddress ipAddress;

  if (ipString.length())
    ipAddress.fromString(ipString);
  *ip = (uint32_t)ipAddress;
}

// Custom web page handler to save the configuration to AutoConnectConfig
String setConfig(AutoConnectAux& aux, PageArgument& args) {
  IPCONFIG  ipconfig;

  // Retrieve each IP address from AutoConnectInput field
  getIPAddress(aux["staip"].value, &ipconfig.ipconfig.ip);
  getIPAddress(aux["gateway"].value, &ipconfig.ipconfig.gateway);
  getIPAddress(aux["netmask"].value, &ipconfig.ipconfig.netmask);
  getIPAddress(aux["dns1"].value, &ipconfig.ipconfig.dns1);

  // Make a result message
  if (auxIPConfig.isValid()) {
    saveConfig(&ipconfig);
    aux["result"].value = "Reset by AutoConnect menu will restart with the above.";
  }
  else
    aux["result"].value = "Invalid IP address specified.";
  return String();
}

// Sense the external switch to enter the configuraton mode
bool senseSW(const uint8_t pin, const uint8_t activeLevel) {
  bool  sw = digitalRead(pin) == activeLevel;
  if (sw) {
    // Cut-off the chattering noise
    unsigned long tm = millis();
    while (digitalRead(pin) == activeLevel) {
      if (millis() - tm > 1000)
        break;
      delay(1);
    }
  }
  return sw;
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  // Shift the credentials storage to reserve saving IPCONFIG
  config.boundaryOffset = sizeof(IPCONFIG);

  // Load current IP configuration
  IPCONFIG  ipconfig;
  loadConfig(&ipconfig);

  // Configure pre-loaded static IPs
  config.staip = IPAddress(ipconfig.ipconfig.ip);
  config.staGateway = IPAddress(ipconfig.ipconfig.gateway);
  config.staNetmask = IPAddress(ipconfig.ipconfig.netmask);
  config.dns1 = IPAddress(ipconfig.ipconfig.dns1);
  portal.config(config);

  // Sense the configuration button (external switch)
  pinMode(ConfigPin, INPUT);
  if (senseSW(ConfigPin, ActiveLevel)) {
    Serial.println("IP configuration enable");
    auxIPConfig.load(AUX_CONFIGIP);
    auxIPConfig.on(getConfig);
    auxRestart.load(AUX_RESTART);
    auxRestart.on(setConfig);
    portal.join({ auxIPConfig, auxRestart });
  }

  portal.begin();
}

void loop() {
  // User sketch process is here.

  portal.handleClient();
}
