#ifndef _PARAM_H_
#define _PARAM_H_

// Identifier for persisting the parameter settings for mqttRSSI.
// For the ESP8266, it is the EEPROM area identifier, and for the ESP32, it is
// the Preference's key.
#define MQTT_PARAM_ID "mqttRSSI"

// MQTT broker connection paramaters
typedef struct {
  char  id[sizeof(MQTT_PARAM_ID)];
  char  mqttServer[33];
  char  apikey[17];
  char  channelId[9];
  char  writekey[17];
  char  clientId[25];
  char  username[25];
  char  password[25];
  char  hostname[25];
  unsigned long publishInterval;
} mqtt_param_t;

void getParams(mqtt_param_t& param);
void putParams(const mqtt_param_t& param);

extern const uint16_t  CREDENTIAL_OFFSET;

#endif  // !_PARAM_H_
