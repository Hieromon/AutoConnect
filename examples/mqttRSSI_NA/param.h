#ifndef _PARAM_H_
#define _PARAM_H_

// Identifier for persisting the parameter settings for mqttRSSI.
// For the ESP8266, it is the EEPROM area identifier, and for the ESP32, it is
// the Preference's key.
#define MQTT_PARAM_ID "mqttRSSI"

// MQTT broker connection paramaters
typedef struct {
  char  id[sizeof(MQTT_PARAM_ID)];
  char  server[33];
  char  channelId[9];
  char  userAPIKey[17];
  char  writeAPIKey[17];
  char  clientID[24];
  char  username[24];
  char  password[24];
  unsigned long publishInterval;
} mqtt_param_t;

void getParams(mqtt_param_t& param);
void putParams(const mqtt_param_t& param);

#endif  // !_PARAM_H_
