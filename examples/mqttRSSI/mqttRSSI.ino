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
#include <ESP8266HTTPClient.h>
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
#define AUX_CLEAR_URI   "/mqtt_clear"

// JSON definition of AutoConnectAux.
// Multiple AutoConnectAux can be defined in the JSON array.
// In this example, JSON is hard-coded to make it easier to
// understand the AutoConnectAux API. In practice, it will be
// an external content which separated from the sketch,
// as the mqtt_RSSI_FS example shows.
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
      },
      {
        "name": "parameters",
        "type": "ACText"
      },
      {
        "name": "clear",
        "type": "ACSubmit",
        "value": "Clear channel",
        "uri": "/mqtt_clear"
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

// Load parameters saved with  saveParams from SPIFFS into
// the elements defined in /mqtt_setting JSON.
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

// Save the value of each element entered by /mqtt_setting to
// the parameter file. The saveParams as below is a callback
// function of /mqtt_save.
// When this callback is invoked, the input value of each element
// of /mqtt_setting is already stored in the AutoConnectAux object.
// In Sketch, you can output to stream its elements specified by name.
String saveParams(AutoConnectAux& aux, PageArgument& args) {
  // PageArgument is a copy set of the elements that AutoConnectAux has.
  serverName = args.arg("mqttserver");
  serverName.trim();

  channelId = args.arg("channelid");
  channelId.trim();
  
  userKey = args.arg("userkey");
  userKey.trim();
  
  apiKey = args.arg("apikey");
  apiKey.trim();
  
  String upd = args.arg("period");
  updateInterval = upd.substring(0, 2).toInt() * 1000;

  String uniqueid = args.arg("uniqueid");

  hostName = args.arg("hostname");
  hostName.trim();
  
  // The entered value is owned by AutoConnectAux of /mqtt_setting.
  // In order to retrieve the elements of /mqtt_setting,
  // it is necessary to get the AutoConnectAux object of /mqtt_setting.
  SPIFFS.begin();
  File param = SPIFFS.open(PARAM_FILE, "w");
  portal.aux("/mqtt_setting")->saveElement(param, { "mqttserver", "channelid", "userkey", "apikey", "period", "uniqueid", "hostname" });
  param.close();
  SPIFFS.end();

  // Echo back saved parameters to AutoConnectAux page.
  AutoConnectText&  echo = aux.getElement<AutoConnectText>("parameters");
  echo.value = "Server: " + serverName + "<br>";
  echo.value += "Channel ID: " + channelId + "<br>";
  echo.value += "User Key: " + userKey + "<br>";
  echo.value += "API Key: " + apiKey + "<br>";
  echo.value += "Update period: " + String(updateInterval / 1000) + " sec.<br>";
  echo.value += "Use APID unique: " + uniqueid + "<br>";
  echo.value += "ESP host name: " + hostName + "<br>";

  return "";
}

void handleRoot() {
  String  content =
    "<html>"
    "<head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "</head>"
    "<body>"
    "<iframe width=\"450\" height=\"260\" style=\"transform:scale(0.79);-o-transform:scale(0.79);-webkit-transform:scale(0.79);-moz-transform:scale(0.79);-ms-transform:scale(0.79);transform-origin:0 0;-o-transform-origin:0 0;-webkit-transform-origin:0 0;-moz-transform-origin:0 0;-ms-transform-origin:0 0;border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/454951/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&type=line\"></iframe>"
    "<p style=\"padding-top:5px;text-align:center\">" AUTOCONNECT_LINK(COG_24) "</p>"
    "</body>"
    "</html>";

#if defined(ARDUINO_ARCH_ESP8266)
  ESP8266WebServer& server = portal.host();
#elif defined(ARDUINO_ARCH_ESP32)
  WebServer&  server = portal.host();
#endif
  server.send(200, "text/html", content);
}

// Clear channel using Thingspeak's API.
void handleClearChannel() {
  HTTPClient  httpClient;
  String  endpoint = serverName;
  endpoint.replace("mqtt", "api");
  String  delUrl = "http://" + endpoint + "/channels/" + channelId + "/feeds.json?api_key=" + userKey;

  Serial.println(delUrl + ":" + String(httpClient.begin(delUrl)));
  Serial.println("res:" + String(httpClient.sendRequest("DELETE")));
  String  res = httpClient.getString();
  httpClient.end();

  // Returns the redirect response. The page is reloaded and its contents
  // are updated to the state after deletion.
#if defined(ARDUINO_ARCH_ESP8266)
  ESP8266WebServer& server = portal.host();
#elif defined(ARDUINO_ARCH_ESP32)
  WebServer&  server = portal.host();
#endif
  server.sendHeader("Location", String("http://") + server.client().localIP().toString() + String("/"));
  server.send(302, "text/plain", "");
  server.client().flush();
  server.client().stop();
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
    config.bootUri = AC_ONBOOTURI_HOME;
    config.homeUri = "/";
    portal.config(config);

    portal.on(AUX_SETTING_URI, loadParams);
    portal.on(AUX_SAVE_URI, saveParams);
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
  server.on(AUX_CLEAR_URI, handleClearChannel);
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
