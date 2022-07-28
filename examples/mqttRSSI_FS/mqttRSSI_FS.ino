/*
  ESP8266/ESP32 publish the RSSI as the WiFi signal strength to ThingSpeak channel.
  This example is for explaining how to use the AutoConnect library.

  In order to execute this example, the ThingSpeak account is needed. Sign up
  for New User Account and create a New Channel via My Channels.
  For details, please refer to the project page.
  https://hieromon.github.io/AutoConnect/howtoembed.html#used-with-mqtt-as-a-client-application

  Also, this example uses AutoConnectAux menu customization which stored in SPIFFS.
  To evaluate this example, you upload the contents as mqtt_setting.json of
  the data folder with MkSPIFFS Tool ("ESP8266 Sketch Data Upload" in Tools menu
  in Arduino IDE).

  When building this sketch, you may receive a compilation error message
  similar to the following:
  - text section exceeds available space in board
  This cause is the small text block size of the predetermined partition
  table. You can avoid this error by selecting Partition Scheme: from
  Arduino IDE's Tool menu and applying Minimal SPIFFS.

  This example is based on the environment as of March 20, 2018.
  Copyright (c) 2020 Hieromon Ikasamo.
  This software is released under the MIT License.
  https://opensource.org/licenses/MIT
*/

// To properly include the suitable header files to the target platform.
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#define FORMAT_ON_FAIL
#define GET_CHIPID()    (ESP.getChipId())
#define GET_HOSTNAME()  (WiFi.hostname())
using WiFiWebServer = ESP8266WebServer;
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#define FORMAT_ON_FAIL  true
#define GET_CHIPID()    ((uint16_t)(ESP.getEfuseMac()>>32))
#define GET_HOSTNAME()  (WiFi.getHostname())
using WiFiWebServer = WebServer;
#endif

#include <PubSubClient.h>
#include <AutoConnect.h>

#ifdef AUTOCONNECT_USE_LITTLEFS
#include <LittleFS.h>
#if defined(ARDUINO_ARCH_ESP8266)
FS& FlashFS = LittleFS;
#elif defined(ARDUINO_ARCH_ESP32)
fs::LittleFSFS& FlashFS = LittleFS;
#endif
#else
#include <FS.h>
#include <SPIFFS.h>
fs::SPIFFSFS& FlashFS = SPIFFS;
#endif

const char* PARAM_FILE      = "/param.json";
const char* AUX_SETTING_URI = "/mqtt_setting";
const char* AUX_SAVE_URI    = "/mqtt_save";
const char* AUX_CLEAR_URI   = "/mqtt_clear";

WiFiWebServer server;
AutoConnect   portal(server);
AutoConnectConfig config;
WiFiClient    wifiClient;
PubSubClient  mqttClient(wifiClient);

String  apId;
String  hostName;

String  serverName;
String  userAPIKey;
String  channelId;
String  writeAPIKey;
String  clientID;
String  username;
String  password;
unsigned long publishInterval = 0;
bool  uniqueid;

unsigned long lastPub = 0;
unsigned long lastAttempt = 0;
const unsigned long attemptInterval = 3000;
bool  reconnect = false;
int   retry;

bool mqttConnect() {
  bool  rc;

  rc = serverName.length() > 0;
  if (rc) {
    // Attempts to connect to the MQTT broker based on a valid server name.
    mqttClient.setServer(serverName.c_str(), 1883);
    Serial.println(String("Attempting MQTT broker:") + serverName);

    rc = mqttClient.connect(clientID.c_str(), username.c_str(), password.c_str());
    if (rc)
      Serial.println("Established:" + clientID);
    else
      Serial.println("Connection failed:" + String(mqttClient.state()));
  }
  return rc;
}

bool mqttPublish(const String& endPoint, const String& payload) {
  // By checking the connection with the broker at the time of the publish
  // request, it can delegate the reconnection attempt to the loop function.
  // This strategy eliminates handling the delay that occurs during the
  // broker reconnection attempt loop and smoothes AutoConnect communication
  // with the client.
  reconnect = !mqttClient.connected();
  if (!reconnect)
    return mqttClient.publish(endPoint.c_str(), payload.c_str());
  else
    return false;
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

void getParams(AutoConnectAux& aux) {
  serverName = aux[F("mqttserver")].value;
  serverName.trim();
  userAPIKey = aux[F("apikey")].value;
  userAPIKey.trim();
  channelId = aux[F("channelid")].value;
  channelId.trim();
  writeAPIKey = aux[F("writekey")].value;
  writeAPIKey.trim();
  clientID = aux[F("clientid")].value;
  clientID.trim();
  username = aux[F("username")].value;
  username.trim();
  password = aux[F("password")].value;
  password.trim();
  AutoConnectRadio& period = aux[F("period")].as<AutoConnectRadio>();
  publishInterval = period.value().substring(0, 2).toInt() * 1000;
  uniqueid = aux[F("uniqueid")].as<AutoConnectCheckbox>().checked;
  hostName = aux[F("hostname")].value;
  hostName.trim();
}

// Load parameters saved with saveParams from SPIFFS into the
// elements defined in /mqtt_setting JSON.
String loadParams(AutoConnectAux& aux, PageArgument& args) {
  (void)(args);
  Serial.print(PARAM_FILE);
  File param = FlashFS.open(PARAM_FILE, "r");
  if (param) {
    if (aux.loadElement(param)) {
      getParams(aux);
      Serial.println(" loaded");
    }
    else
      Serial.println(" failed to load");
    param.close();
  }
  else
    Serial.println(" open failed");
  return String("");
}

// Save the value of each element entered by '/mqtt_setting' to the
// parameter file. The saveParams as below is a callback function of
// /mqtt_save. When invoking this handler, the input value of each
// element is already stored in '/mqtt_setting'.
// In the Sketch, you can output to stream its elements specified by name.
String saveParams(AutoConnectAux& aux, PageArgument& args) {
  // The 'where()' function returns the AutoConnectAux that caused
  // the transition to this page.
  AutoConnectAux&   mqtt_setting = *portal.aux(portal.where());
  getParams(mqtt_setting);

  // The entered value is owned by AutoConnectAux of /mqtt_setting.
  // To retrieve the elements of /mqtt_setting, it is necessary to get
  // the AutoConnectAux object of /mqtt_setting.
  File param = FlashFS.open(PARAM_FILE, "w");
  mqtt_setting.saveElement(param, {"mqttserver", "apikey", "channelid", "writekey", "clientid", "username", "password", "uniqueid", "period", "hostname"});
  param.close();

  // Echo back saved parameters to AutoConnectAux page.
  aux[F("period")].value = String(publishInterval / 1000);

  return String();
}

// The root of the URL to include the channel views that provide the Thingspeak.
void handleRoot() {
  String  content =
    "<html>"
    "<head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "</head>"
    "<body>"
    "<iframe width=\"450\" height=\"260\" style=\"transform:scale(0.79);-o-transform:scale(0.79);-webkit-transform:scale(0.79);-moz-transform:scale(0.79);-ms-transform:scale(0.79);transform-origin:0 0;-o-transform-origin:0 0;-webkit-transform-origin:0 0;-moz-transform-origin:0 0;-ms-transform-origin:0 0;border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/{{CHANNEL}}/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&type=line\"></iframe>"
    "<p style=\"padding-top:5px;text-align:center\">" AUTOCONNECT_LINK(COG_24) "</p>"
    "</body>"
    "</html>";

  content.replace("{{CHANNEL}}", channelId);
  server.send(200, "text/html", content);
}

// Clear channel using ThingSpeak's API.
void handleClearChannel() {
  HTTPClient  httpClient;

  String  endpoint = serverName;
  endpoint.replace("mqtt3", "api");
  String  delUrl = "http://" + endpoint + "/channels/" + channelId + "/feeds.json?api_key=" + userAPIKey;

  Serial.print("DELETE " + delUrl);
  if (httpClient.begin(wifiClient, delUrl)) {
    Serial.print(":");
    int resCode = httpClient.sendRequest("DELETE");
    const String& res = httpClient.getString();
    Serial.println(String(resCode) + String(",") + res);
    httpClient.end();
  }
  else
    Serial.println(" failed");

  // Returns the redirect response. The page is reloaded and its contents
  // are updated to the state after deletion.
  server.sendHeader("Location", String("http://") + server.client().localIP().toString() + String("/"));
  server.send(302, "text/plain", "");
  server.client().stop();
}

// Load AutoConnectAux JSON from the flash on the board.
bool loadAux(const String auxName) {
  bool  rc = false;
  String  fn = auxName + ".json";
  File fs = FlashFS.open(fn.c_str(), "r");
  if (fs) {
    rc = portal.load(fs);
    fs.close();
  }
  else
    Serial.println("Filesystem open failed: " + fn);
  return rc;
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  FlashFS.begin(FORMAT_ON_FAIL);

  if (!loadAux(AUX_SETTING_URI))
    Serial.printf("%s load error\n", AUX_SETTING_URI);
  if (!loadAux(AUX_SAVE_URI))
    Serial.printf("%s load error\n", AUX_SAVE_URI);

  AutoConnectAux* setting = portal.aux(AUX_SETTING_URI);
  if (setting) {
    PageArgument  args;
    AutoConnectAux& mqtt_setting = *setting;
    loadParams(mqtt_setting, args);
    AutoConnectCheckbox&  uniqueidElm = mqtt_setting["uniqueid"].as<AutoConnectCheckbox>();
    AutoConnectInput&     hostnameElm = mqtt_setting["hostname"].as<AutoConnectInput>();
    if (uniqueidElm.checked) {
      config.apid = String("ESP") + "-" + String(GET_CHIPID(), HEX);
      Serial.println("apid set to " + config.apid);
    }
    if (hostnameElm.value.length()) {
      config.hostName = hostnameElm.value;
      Serial.println("hostname set to " + config.hostName);
    }
    portal.on(AUX_SETTING_URI, loadParams);
    portal.on(AUX_SAVE_URI, saveParams);
  }

  // Assign the captive portal popup screen to the URL as the root path.
  server.on("/", handleRoot);
  server.on(AUX_CLEAR_URI, handleClearChannel);
  config.homeUri = "/";
  config.bootUri = AC_ONBOOTURI_HOME;

  // Reconnect and continue publishing even if WiFi is disconnected.
  config.autoReconnect = true;
  config.reconnectInterval = 1;
  portal.config(config);

  Serial.print("WiFi ");
  if (portal.begin()) {
    Serial.println("connected:" + WiFi.SSID());
    Serial.println("IP:" + WiFi.localIP().toString());
    Serial.print("mDNS responder ");
    if (MDNS.begin(GET_HOSTNAME())) {
      MDNS.addService("http", "tcp", 80);
      Serial.println("started");
    }
    else
      Serial.println("setting up failed");
  }
  else {
    Serial.println("connection failed:" + String(WiFi.status()));
    Serial.println("Needs WiFi connection to start publishing messages");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED && publishInterval > 0) {
    if (reconnect) {
      // Attempts to reconnect with the broker do not involve a delay.
      // The reconnect interval is realized as the measurement of elapsed
      // time using the millis function.
      if (millis() - lastAttempt > attemptInterval) {
        reconnect = !mqttConnect();
        lastAttempt = millis();
        if (++retry >= 3) {
          retry = 0;
          reconnect = false;
        }
      }
    }
    else {
      // It is not the delay function that produces the publish interval.
      // Using delay inside a loop function is deprecated for web server
      // sketches. It blocks HTTP request replies.
      // The publish interval is guaranteed by measuring the elapsed time.
      if (millis() - lastPub > publishInterval) {
        String  topic = String("channels/") + channelId + String("/publish");
        String  message = String("field1=") + String(getStrength(7));
        mqttPublish(topic, message);
        lastPub = millis();
      }
      mqttClient.loop();
    }
  }

#ifdef ARDUINO_ARCH_ESP8266
  MDNS.update();
#endif
  portal.handleClient();
}
