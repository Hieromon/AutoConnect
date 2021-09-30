#ifndef _PARAM_H_
#define _PARAM_H_

// MQTT broker connection paramaters
typedef struct {
  char  server[33];
  char  channelid[9];
  char  userkey[17];
  char  apikey[17];
  unsigned long publishInterval;
} mqtt_param_t;

void getParams(mqtt_param_t& param);
void putParams(const mqtt_param_t& param);

#endif  // !_PARAM_H_
