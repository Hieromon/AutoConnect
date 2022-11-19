/*
ESP8266/ESP32 publish the RSSI as the WiFi signal strength to ThingSpeak channel.
This example is for explaining how to use the AutoConnect library.

In order to execute this example, the ThingSpeak account is needed. Sign up
for New User Account and create a New Channel via My Channels.
For details, please refer to the project page.
https://hieromon.github.io/AutoConnect/howtoembed.html#used-with-mqtt-as-a-client-application

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
#define mDNSUpdate()  do {MDNS.update();} while (0)
using WiFiWebServer = ESP8266WebServer;
const uint8_t LED_ACTIVE = LOW;
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#define FORMAT_ON_FAIL  true
#define mDNSUpdate() do {(void)0;} while(0)
using WiFiWebServer = WebServer;
const uint8_t LED_ACTIVE = HIGH;
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

#ifndef LED_BUILTIN
#pragma message("Warning, LED_BUILTIN is undefined. Assumes Pin #2.")
#define LED_BUILTIN 2
#endif

// URLs assigned to the custom web page.
const char* PARAM_FILE       = "/param.json";
const char* URL_MQTT_HOME    = "/";
const char* URL_MQTT_SETTING = "/mqtt_setting";
const char* URL_MQTT_START   = "/mqtt_start";
const char* URL_MQTT_CLEAR   = "/mqtt_clear";
const char* URL_MQTT_STOP    = "/mqtt_stop";

// JSON definition of AutoConnectAux.
// Multiple AutoConnectAux can be defined in the JSON array.
// In this example, JSON is hard-coded to make it easier to understand
// the AutoConnectAux API. In practice, it will be an external content
// which separated from the sketch, as the mqtt_RSSI_FS example shows.
static const char AUX_MQTT[] PROGMEM = R"*(
[
  {
    "title": "MQTT Setting",
    "uri": "/mqtt_setting",
    "menu": true,
    "element": [
      {
        "name": "style",
        "type": "ACStyle",
        "value": "label+input,label+select{position:sticky;left:140px;width:204px!important;box-sizing:border-box;}"
      },
      {
        "name": "header",
        "type": "ACElement",
        "value": "<h2 style='text-align:center;color:#2f4f4f;margin-top:10px;margin-bottom:10px'>MQTT Broker settings</h2>"
      },
      {
        "name": "caption",
        "type": "ACText",
        "value": "Publish WiFi signal strength via MQTT, publishing the RSSI value of the ESP module to the ThingSpeak public channel.",
        "style": "font-family:serif;color:#053d76",
        "posterior": "par"
      },
      {
        "name": "mqttserver",
        "type": "ACInput",
        "label": "Server",
        "pattern": "^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$",
        "placeholder": "MQTT broker server",
        "global": true
      },
      {
        "name": "apikey",
        "type": "ACInput",
        "label": "User API Key",
        "global": true
      },
      {
        "name": "channelid",
        "type": "ACInput",
        "label": "Channel ID",
        "pattern": "^[0-9]{6}$",
        "global": true
      },
      {
        "name": "writekey",
        "type": "ACInput",
        "label": "Write API Key",
        "global": true
      },
      {
        "name": "nl1",
        "type": "ACElement",
        "value": "<hr>"
      },
      {
        "name": "credential",
        "type": "ACText",
        "value": "MQTT Device Credentials",
        "style": "font-weight:bold;color:#1e81b0",
        "posterior": "div"
      },
      {
        "name": "clientid",
        "type": "ACInput",
        "label": "Client ID",
        "global": true
      },
      {
        "name": "username",
        "type": "ACInput",
        "label": "Username",
        "global": true
      },
      {
        "name": "password",
        "type": "ACInput",
        "label": "Password",
        "apply": "password",
        "global": true
      },
      {
        "name": "nl2",
        "type": "ACElement",
        "value": "<hr>"
      },
      {
        "name": "period",
        "type": "ACRadio",
        "value": [
          "30 sec.",
          "60 sec.",
          "180 sec."
        ],
        "label": "Update period",
        "arrange": "vertical",
        "global": true
      },
      {
        "name": "hostname",
        "type": "ACInput",
        "value": "",
        "label": "ESP host name",
        "pattern": "^([a-zA-Z0-9]([a-zA-Z0-9-])*[a-zA-Z0-9]){1,24}$",
        "global": true
      },
      {
        "name": "save",
        "type": "ACSubmit",
        "value": "Save&amp;Start",
        "uri": "/mqtt_start"
      },
      {
        "name": "discard",
        "type": "ACSubmit",
        "value": "Discard",
        "uri": "/"
      },
      {
        "name": "stop",
        "type": "ACSubmit",
        "value": "Stop publishing",
        "uri": "/mqtt_stop"
      }
    ]
  },
  {
    "title": "MQTT Setting",
    "uri": "/mqtt_start",
    "menu": false,
    "element": [
      {
        "name": "c1",
        "type": "ACText",
        "value": "<h4>MQTT publishing has started.</h4>",
        "style": "text-align:center;color:#2f4f4f;padding:5px;"
      },
      {
        "name": "c2",
        "type": "ACText",
        "value": "<h4>Parameters saved as:</h4>",
        "style": "text-align:center;color:#2f4f4f;padding:5px;"
      },
      {
        "name": "mqttserver",
        "type": "ACText",
        "format": "Server: %s",
        "posterior": "br",
        "global": true
      },
      {
        "name": "apikey",
        "type": "ACText",
        "format": "User API Key: %s",
        "posterior": "br",
        "global": true
      },
      {
        "name": "channelid",
        "type": "ACText",
        "format": "Channel ID: %s",
        "posterior": "br",
        "global": true
      },
      {
        "name": "writekey",
        "type": "ACText",
        "format": "Write API Key: %s",
        "posterior": "br",
        "global": true
      },
      {
        "name": "clientid",
        "type": "ACText",
        "format": "Client ID: %s",
        "posterior": "br",
        "global": true
      },
      {
        "name": "username",
        "type": "ACText",
        "format": "Username: %s",
        "posterior": "br",
        "global": true
      },
      {
        "name": "password",
        "type": "ACText",
        "format": "Password: %s",
        "posterior": "br",
        "global": true
      },
      {
        "name": "hostname",
        "type": "ACText",
        "format": "ESP host: %s",
        "posterior": "br",
        "global": true
      },
      {
        "name": "period",
        "type": "ACText",
        "format": "Update period: %s",
        "posterior": "br",
        "global": true
      },
      {
        "name": "clear",
        "type": "ACSubmit",
        "value": "Clear channel",
        "uri": "/mqtt_clear"
      }
    ]
  },
  {
    "title": "MQTT Setting",
    "uri": "/mqtt_clear",
    "menu": false,
    "response": false
  },
  {
    "title": "MQTT Setting",
    "uri": "/mqtt_stop",
    "menu": false,
    "response": false
  }
]
)*";

WiFiWebServer server;
AutoConnect   portal(server);
AutoConnectConfig config;
WiFiClient    wifiClient;
PubSubClient  mqttClient(wifiClient);

// ThingSpeak channel parameters for MQTT publishing.
// This example defines these parameters as input fields for a custom web page,
// allowing manipulation by the Web UI. For more information on the ThingSpeak
// channel and MQTT API, refer to the followings:
// https://www.mathworks.com/help/thingspeak/collect-data-in-a-new-channel.html
// https://www.mathworks.com/help/thingspeak/mqtt-api.html
String  mqttServer;
String  apikey;
String  channelId;
String  writekey;
String  clientId;
String  username;
String  password;
String  hostname;

// A variety of control variables to keep message publishing periodic.

// Indicates the message publishing status. ESP module will continue to send
// messages periodically when enablePublish is true. `auxMQTTStop` handler
// can be false with a stop publish operation via the `/mqtt_stop` web page.
bool  enablePublish = false;

// publishInterval holds the millisecond cycle for message publishing. The
// `/mqtt_setting` custom web page allows you to change this interval value via
// `auxMQTTSetting` handler.
unsigned long publishInterval;
unsigned long nextPeriod;

// This example allows communication retries between the ESP module and the
// ThingSpeak channel. The retry interval and a maximum number of attempts are
// delegated to `retryInterval` and `maxRetryCount` values.
const unsigned long retryInterval = 5000;
const int maxRetryCount = 3;
int   retryCount;

// On-board LED on the ESP module blinks during message publishing. ledBlinking
// measures the elapsed milliseconds of the ON/OFF cycle.
unsigned long ledBlinking;

// Measure WiFi signal strength
int getStrength(uint8_t points) {
  uint8_t sc = points;
  long    rssi = 0;

  while (sc--) {
    rssi += WiFi.RSSI();
    delay(20);
  }
  return points ? static_cast<int>(rssi / points) : 0;
}

// Reflects the loaded channel settings to global variables; the publishMQTT
// function uses those global variables to actuate ThingSpeak MQTT API.
void setParams(AutoConnectAux& aux) {
  mqttServer = aux[F("mqttserver")].as<AutoConnectInput>().value;
  apikey = aux[F("apikey")].as<AutoConnectInput>().value;
  channelId = aux[F("channelid")].as<AutoConnectInput>().value;
  writekey = aux[F("writekey")].as<AutoConnectInput>().value;
  clientId = aux[F("clientid")].as<AutoConnectInput>().value;
  username = aux[F("username")].as<AutoConnectInput>().value;
  password = aux[F("password")].as<AutoConnectInput>().value;
  AutoConnectRadio& period = aux[F("period")].as<AutoConnectRadio>();
  publishInterval = period.value().substring(0, 2).toInt() * 1000;
  hostname = aux[F("hostname")].as<AutoConnectInput>().value;
}

// Loads previously saved ThingSpeak channel parameters from the file system at
// the start of publishing.
bool loadParams(AutoConnectAux& aux) {
  bool  rc = false;

  Serial.print(PARAM_FILE);
  File param = FlashFS.open(PARAM_FILE, "r");
  if (param) {
    // These parameters are stored as JSON definitions in AutoConnectElements,
    // so the AutoConnectAux::loadElement function can be applied to restore.
    if (aux.loadElement(param)) {

      // Reflects the loaded channel settings to global variables; the publishMQTT
      // function uses those global variables to actuate ThingSpeak MQTT API.
      setParams(aux);
      Serial.println(" loaded");
      rc = true;
    }
    else
      Serial.println(" failed to load");
    param.close();
  }
  else
    Serial.println(" open failed");
  return rc;
}

// Bind the mDNS service to the http server launched by the ESP module.
void startMDNS(void) {
  if (!hostname.length())
    hostname = WiFi.getHostname();
  else
    WiFi.setHostname(hostname.c_str());

  Serial.printf("mDNS responder %s.local start\n", hostname.c_str());
  if (MDNS.begin(hostname.c_str())) {
    if (!MDNS.addService("http", "tcp", 80))
      Serial.println("mDNS service adding failed");
  }
  else
    Serial.println("mDNS failed");
}

// Start MQTT message publishing.
void startMQTT() {
  // Set MQTT broker endpoint to PubSubClient.
  Serial.printf("Starting MQTT, interval %lu, ", publishInterval);
  enablePublish = true;
  nextPeriod = millis();
  retryCount = 0;
  mqttClient.setServer(mqttServer.c_str(), 1883);

  // Rebind mDNS service with `hostname`.
  if (!hostname.equalsIgnoreCase(String(WiFi.getHostname()))) {
    MDNS.end();
    startMDNS();
  }

  // Prepare the GPIO port for LED blinking.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, !LED_ACTIVE);
  ledBlinking = millis();
}

// Publish a message with RSSI contained in the payload to the MQTT broker.
bool publishMQTT() {
  bool  inPublish = true;

  if (mqttServer.length()) {
    if (millis() > nextPeriod) {
      // Attempts to connect to the MQTT broker based on a valid server name.
      // mqttClient.setServer(mqtt.data.server.c_str(), 1883);
      if (!mqttClient.connected()) {
        Serial.println(String("Attempting MQTT broker:") + mqttServer);
        if ((inPublish = mqttClient.connect(clientId.c_str(), username.c_str(), password.c_str())))
          Serial.println("Established:" + clientId);
        else
          Serial.print("Connection failed:" + String(mqttClient.state()));
      }

      if (inPublish) {
        String  topic = String("channels/") + channelId + String("/publish");
        String  message = String("field1=") + String(getStrength(7));
        mqttClient.publish(topic.c_str(), message.c_str());
        inPublish = mqttClient.loop();
        if (!inPublish)
          Serial.print("MQTT publishing failed");
      }

      // If the message is successfully published, the connection to the MQTT
      // broker is disconnected and the interval is extended until the next
      // publishing turn.
      if (inPublish) {
        mqttClient.disconnect();
        nextPeriod = millis() + publishInterval;
        retryCount = 0;
      }
      else {
        // Error retry. By varying the interval until the next turn of the
        // process called, the processMQTT performs an error retry without an
        // internal loop.
        if (retryCount++ < maxRetryCount) {
          nextPeriod = millis() + retryInterval;
          Serial.printf("...retrying %d\n", retryCount);
        }
        else {
          nextPeriod = millis() + publishInterval;
          retryCount = 0;
          Serial.println(", retries exceeded, abandoned.");
        }
      }
    }
  }

  // The post-process is the LED blinking control. Stops LED flashing if message
  // publish fails.
  if (inPublish) {
    if (millis() - ledBlinking > 500) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      ledBlinking = millis();
    }
  }
  else
    digitalWrite(LED_BUILTIN, !LED_ACTIVE);

  return inPublish;
}

// Temporarily stops message publishing; transmission will be suspended until
// resumed by the startMQTT function.
void endMQTT() {
  mqttClient.disconnect();
  enablePublish = false;
  digitalWrite(LED_BUILTIN, !LED_ACTIVE);
  Serial.println("MQTT publishing stopped\n");
}

// The behavior of the auxMQTTSetting function below transfers the MQTT API
// parameters to the value of each AutoConnectInput element on the custom web
// page. (i.e., displayed as preset values)
String auxMQTTSetting(AutoConnectAux& aux, PageArgument& args) {
  aux[F("mqttserver")].as<AutoConnectInput>().value = mqttServer;
  aux[F("apikey")].as<AutoConnectInput>().value = apikey;
  aux[F("channelid")].as<AutoConnectInput>().value = channelId;
  aux[F("writekey")].as<AutoConnectInput>().value = writekey;
  aux[F("clientid")].as<AutoConnectInput>().value = clientId;
  aux[F("username")].as<AutoConnectInput>().value = username;
  aux[F("password")].as<AutoConnectInput>().value = password;
  aux[F("hostname")].as<AutoConnectInput>().value = hostname;
  aux[F("period")].as<AutoConnectRadio>().checked = publishInterval / (30 * 1000);
  if (aux[F("period")].as<AutoConnectRadio>().checked > 3)
    aux[F("period")].as<AutoConnectRadio>().checked = 3;
  return String();
}

// Get the connection settings entered in `/mqtt_setting` via `/mqtt_start` and
// save them to mqtt parameters.
String auxMQTTStart(AutoConnectAux& aux, PageArgument& args) {
  // The next step is to pass the transition source of `/mqtt_start` page to
  // match the argument interface of the setParams function.
  // `AutoConnectAux::referer` function can identify the AutoConnectAux that
  // caused the page transition to that AutoConnectAux.
  AutoConnectAux& mqtt_setting = aux.referer();
  setParams(mqtt_setting);

  // Save the value of each element entered by '/mqtt_setting' to the
  // parameter file. The saveParams as below is a callback function of
  // /mqtt_save. When invoking this handler, the input value of each
  // element is already stored in '/mqtt_setting'.
  // In the Sketch, you can output to stream its elements specified by name.
  File param = FlashFS.open(PARAM_FILE, "w");
  if (param) {
    mqtt_setting.saveElement(param, {"mqttserver", "apikey", "channelid", "writekey", "clientid", "username", "password", "period", "hostname"});
    param.close();
  }
  else
    Serial.printf("Save to %s open failed\n", PARAM_FILE);

  // Restart MQTT publishing.
  startMQTT();

  return String();
}

// Stops MQTT publishing.
// `mqtt_stop` does not respond to http content as a web page. It stops MQTT
// publishing and then navigates the client to redirect to the root page.
// To suppress page content response by AutoConnect, `/mqtt_stop` has a
// `response:false` attribute.
String auxMQTTStop(AutoConnectAux& aux, PageArgument& args) {
  endMQTT();
  aux.redirect(URL_MQTT_HOME);
  return String();
}

// Clear the channel data on ThingSpeak.
String auxMQTTClear(AutoConnectAux& aux, PageArgument& args) {
  HTTPClient  httpClient;

  String  endpoint = mqttServer;
  endpoint.replace("mqtt3", "api");
  String  delUrl = "http://" + endpoint + "/channels/" + channelId + "/feeds.json?api_key=" + apikey;

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

  // After clearing the channel data, it transitions to the home page.
  aux.redirect(URL_MQTT_HOME);
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

void wifiConnect(IPAddress& ip) {
  Serial.println("WiFi connected:" + WiFi.SSID());
  Serial.println("IP:" + WiFi.localIP().toString());
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  FlashFS.begin(FORMAT_ON_FAIL);

  // Assign the captive portal popup screen to the URL as the root path.
  // Reconnect and continue publishing even if WiFi is disconnected.
  config.homeUri = URL_MQTT_HOME;
  config.bootUri = AC_ONBOOTURI_HOME;
  config.autoReconnect = true;
  config.reconnectInterval = 1;
  portal.config(config);

  // Load AutoConnectAux pages described with JSON description.
  // Its custom web page definition has the following four pages combined into
  // one using a JSON array:
  portal.load(FPSTR(AUX_MQTT));
  portal.on(URL_MQTT_SETTING, auxMQTTSetting);
  portal.on(URL_MQTT_START, auxMQTTStart);
  portal.on(URL_MQTT_CLEAR, auxMQTTClear);
  portal.on(URL_MQTT_STOP, auxMQTTStop);

  // Restore saved MQTT broker setting values.
  // This example stores all setting parameters as a set of AutoConnectElement,
  // so they can be restored in bulk using `AutoConnectAux::loadElement`.
  AutoConnectAux& settings_mqtt = *portal.aux(URL_MQTT_SETTING);
  loadParams(settings_mqtt);

  // This home page is the response content by requestHandler with WebServer,
  // it does not go through AutoConnect. Such pages register requestHandler
  // directly using `WebServer::on`.
  server.on(URL_MQTT_HOME, handleRoot);

  portal.onConnect(wifiConnect);
  portal.begin();

  // Start services
  startMDNS();
  startMQTT();
}

void loop() {
  if (enablePublish)
    publishMQTT();

  portal.handleClient();
  mDNSUpdate();
}
