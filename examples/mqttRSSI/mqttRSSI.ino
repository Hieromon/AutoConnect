/*
  ESP8266/ESP32 publish the RSSI as the WiFi signal strength to ThingSpeak channel.
  This example is for explaining how to use the AutoConnect library.

  In order to execute this example, the ThingSpeak account is needed. Sing up
  for New User Account and create a New Channel via My Channels.
  For details, please refer to the project page.
  https://hieromon.github.io/AutoConnect/examples/index.html#used-with-mqtt-as-a-client-application

  This example is based on the environment as of March 20, 2018.
  Copyright (c) 2018 Hieromon Ikasamo.
  This software is released under the MIT License.
  https://opensource.org/licenses/MIT
*/

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif
#include <PubSubClient.h>
#include <AutoConnect.h>

#define MQTT_USER_KEY        "****************"  // Replace to User API Key.
#define CHANNEL_ID           "******"            // Replace to Channel ID.
#define CHANNEL_API_KEY_WR   "****************"  // Replace to the write API Key.

#define MQTT_UPDATE_INTERVAL 40000
#define MQTT_TOPIC           "channels/" CHANNEL_ID "/publish/" CHANNEL_API_KEY_WR
#define MQTT_USER_ID         "anyone" 
#define MQTT_SERVER          "mqtt.thingspeak.com"

AutoConnect  portal;
WiFiClient   wifiClient;
PubSubClient mqttClient(wifiClient);

bool mqttConnect() {
  static const char alphanum[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";  // For random generation of client ID.
  char    clientId[9];

  uint8_t retry = 10;
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT broker:" MQTT_SERVER);

    for (uint8_t i = 0; i < 8; i++) {
      clientId[i] = alphanum[random(62)];
    }
    clientId[8] = '\0';

    if (mqttClient.connect(clientId, MQTT_USER_ID, MQTT_USER_KEY)) {
      Serial.println("Established:" + String(clientId));
      return true;
    } else {
      Serial.println("Connection failed:" + String(mqttClient.state()));
      if (!--retry)
        break;
    }
    delay(3000);
  }
  return false;
}

void mqttPublish(String msg) {
  String path = String(MQTT_TOPIC);
  int    tLen = path.length();
  char   topic[tLen];
  path.toCharArray(topic, tLen + 1);

  int    mLen = msg.length();
  char   payload[mLen];
  msg.toCharArray(payload, mLen + 1);

  mqttClient.publish(topic, payload);
}

int getStrength(uint8_t points) {
  uint8_t sc = points;
  long    rssi = 0;

  while (sc--) {
    rssi += WiFi.RSSI();
    delay(20);
  }
  return points ? (int)(rssi / points) : 0;
}

unsigned long   lastPub = 0;

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  Serial.print("WiFi ");
  if (portal.begin()) {
    Serial.println("connected:" + WiFi.SSID());
    Serial.println("IP:" + WiFi.localIP().toString());
  } else {
    Serial.println("connection failed:" + String(WiFi.status()));
    while (1) {
      delay(100);
      yield();
    }
  }
  mqttClient.setServer(MQTT_SERVER, 1883);
}

void loop() {
  if (millis() - lastPub > MQTT_UPDATE_INTERVAL) {
    if (!mqttClient.connected()) {
      mqttConnect();
    }
    String item = String("field1=") + String(getStrength(7));
    mqttPublish(item);
    mqttClient.loop();
    lastPub = millis();
  }
  portal.handleClient();
}
