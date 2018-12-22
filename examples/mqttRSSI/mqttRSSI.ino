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
#include <SPIFFS.h>
#endif
#include <FS.h>
#include <PubSubClient.h>
#include <AutoConnect.h>

#define PARAM_FILE      "/param.json"
#define AUX_SETTING_URI "/mqtt_setting"
#define AUX_SAVE_URI    "/mqtt_save"

static const char AUX_mqtt_setting[] PROGMEM = R"raw(
[
  {
    "title": "MQTT Setting",
    "uri": "/mqtt_setting",
    "menu": true,
    "element": [
      {
        "name": "header",
        "type": "ACText",
        "value": "<h2>MQTT broker settings</h2>",
        "style": "text-align:center;color:#2f4f4f;padding:10px;"
      },
      {
        "name": "caption",
        "type": "ACText",
        "value": "Publishing the WiFi signal strength to MQTT channel. RSSI value of ESP8266 to the channel created on ThingSpeak",
        "style": "font-family:serif;color:#4682b4;"
      },
      {
        "name": "mqttserver",
        "type": "ACInput",
        "value": "mqtt.thingspeak.com",
        "placeholder": "MQTT broker server",
        "label": "Server"
      },
      {
        "name": "channelid",
        "type": "ACInput",
        "value": "454951",
        "label": "Channel ID"
      },
      {
        "name": "userkey",
        "type": "ACInput",
        "value": "NRTFYGJ6TJFGX4RC",
        "label": "User Key"
      },
      {
        "name": "apikey",
        "type": "ACInput",
        "value": "HBVQ2XV6VYBI4582",
        "label": "API Key"
      },
      {
        "name": "period",
        "type": "ACRadio",
        "label": "Update period",
        "value": [
          "30 sec.",
          "60 sec.",
          "180 sec."
        ],
        "arrange": "vertical",
        "checked": 1
      },
      {
        "name": "newline",
        "type": "ACElement",
        "value": "<hr>"
      },
      {
        "name": "uniqueid",
        "type": "ACCheckbox",
        "value": "unique",
        "label": "Use APID unique",
        "checked": true
      },
      {
        "name": "hostname",
        "type": "ACInput",
        "label": "ESP host name",
        "value": ""
      },
      {
        "name": "save",
        "type": "ACSubmit",
        "value": "Save&Start",
        "uri": "/mqtt_save"
      },
      {
        "name": "discard",
        "type": "ACSubmit",
        "value": "Discard",
        "uri": "/"
      }
    ]
  },
  {
    "title": "MQTT Setting",
    "uri": "/mqtt_save",
    "name": "mqttsave",
    "element": [
      {
        "name": "caption",
        "type": "ACText",
        "value": "<h4>Parameters saved as:</h4>",
        "style": "text-align:center;color:#2f4f4f;padding:10px;"
      }
    ]
  }
]
)raw";

AutoConnect  portal;
AutoConnectConfig config;
WiFiClient   wifiClient;
PubSubClient mqttClient(wifiClient);
String  serverName;
String  channelId;
String  userKey;
String  apiKey;
String  apid;
String  hostName;
unsigned int  updateInterval = 0;
unsigned long lastPub = 0;

#define MQTT_USER_ID  "anyone"

bool mqttConnect() {
  static const char alphanum[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";  // For random generation of client ID.
  char    clientId[9];

  uint8_t retry = 10;
  while (!mqttClient.connected()) {
    if (serverName.length() <= 0)
      break;

    mqttClient.setServer(serverName.c_str(), 1883);
    Serial.println(String("Attempting MQTT broker:") + serverName);

    for (uint8_t i = 0; i < 8; i++) {
      clientId[i] = alphanum[random(62)];
    }
    clientId[8] = '\0';

    if (mqttClient.connect(clientId, MQTT_USER_ID, userKey.c_str())) {
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
  String path = String("channels/") + channelId + String("/publish/") + apiKey;
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
  return points ? static_cast<int>(rssi / points) : 0;
}

String loadParams(AutoConnectAux& aux, PageArgument& args) {
  (void)(args);
  SPIFFS.begin();
  File param = SPIFFS.open(PARAM_FILE, "r");
  if (param) {
    aux.loadElement(param);
    param.close();
  }
  else
    Serial.println(PARAM_FILE " open failed");
  SPIFFS.end();
  return "";
}

String saveParams(AutoConnectAux& aux, PageArgument& args) {
  String  echo;

  serverName = args.arg("mqttserver");
  serverName.trim();
  echo = "mqttserver: " + serverName + "<br>";

  channelId = args.arg("channelid");
  channelId.trim();
  echo += "channelid: " + channelId + "<br>";
  
  userKey = args.arg("userkey");
  userKey.trim();
  echo += "userkey: " + userKey + "<br>";
  
  apiKey = args.arg("apikey");
  apiKey.trim();
  echo += "apikey: " + apiKey + "<br>";
  
  String upd = args.arg("period");
  updateInterval = upd.substring(0, 2).toInt() * 1000;
  echo += "period: " + String(updateInterval) + "<br>";

  String uniqueid = args.arg("uniqueid");
  echo += "uniqueid: " + uniqueid + "<br>";

  hostName = args.arg("hostname");
  hostName.trim();
  echo += "hostname: " + hostName + "<br>";
  
  SPIFFS.begin();
  File param = SPIFFS.open(PARAM_FILE, "w");
  portal.aux("/mqtt_setting")->saveElement(param, { "mqttserver", "channelid", "userkey", "apikey", "period", "uniqueid", "hostname" });
  param.close();
  SPIFFS.end();
  return echo;
}

void handleRoot() {
  String  content =
    "<html>"
    "<head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "</head>"
    "<body>"
    "<iframe width=\"450\" height=\"260\" style=\"transform:scale(0.79);-o-transform:scale(0.79);-webkit-transform:scale(0.79);-moz-transform:scale(0.79);-ms-transform:scale(0.79);transform-origin:0 0;-o-transform-origin:0 0;-webkit-transform-origin:0 0;-moz-transform-origin:0 0;-ms-transform-origin:0 0;border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/454951/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&type=line\"></iframe>"
    "<p style=\"padding-top:10px;text-align:center\">" AUTOCONNECT_LINK(COG_24) "</p>"
    "</body>"
    "</html>";

#if defined(ARDUINO_ARCH_ESP8266)
  ESP8266WebServer& server = portal.host();
#elif defined(ARDUINO_ARCH_ESP32)
  WebServer&  server = portal.host();
#endif
  server.send(200, "text/html", content);
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  if (portal.load(FPSTR(AUX_mqtt_setting))) {
    AutoConnectAux* setting = portal.aux(AUX_SETTING_URI);
    AutoConnectCheckbox&  uniqueidElm = setting->getElement<AutoConnectCheckbox>("uniqueid");
    AutoConnectInput&     hostnameElm = setting->getElement<AutoConnectInput>("hostname");
    if (uniqueidElm.checked) {
      config.apid = String("ESP") + "_" + String(ESP.getChipId(), HEX);
    }
    if (hostnameElm.value.length()) {
      config.hostName = hostnameElm.value;
    }

    portal.on(AUX_SETTING_URI, loadParams);
    portal.on(AUX_SAVE_URI, saveParams, AC_EXIT_LATER);
    portal.config(config);
  }
  else
    Serial.println("load error");

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

#if defined(ARDUINO_ARCH_ESP8266)
  ESP8266WebServer& server = portal.host();
#elif defined(ARDUINO_ARCH_ESP32)
  WebServer&  server = portal.host();
#endif
  server.on("/", handleRoot);
}

void loop() {
  if (updateInterval > 0) {
    if (millis() - lastPub > updateInterval) {
      if (!mqttClient.connected()) {
        mqttConnect();
      }
      String item = String("field1=") + String(getStrength(7));
      mqttPublish(item);
      mqttClient.loop();
      lastPub = millis();
    }
  }
  portal.handleClient();
}
