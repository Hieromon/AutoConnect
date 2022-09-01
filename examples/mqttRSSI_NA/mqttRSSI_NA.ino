/*
ESP8266/ESP32 publish the RSSI as the WiFi signal strength to ThingSpeak channel.
This example is for explaining how to use the AutoConnect library.

In order to execute this example, the ThingSpeak account is needed. Sing up
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

// ThingSpeak MQTT channel settings are persisted using the ESP module's flash
// memory. The area is EEPROM for ESP8266 and Preferences for ESP32.
// The `param.cpp` code stores the channel settings in a flash area adapted to
// the ESP module
#include "param.h"

// URLs assigned to the custom web page.
const char* PARAM_FILE       = "/param.json";
const char* URL_MQTT_HOME    = "/";
const char* URL_MQTT_SETTING = "/mqtt_setting";
const char* URL_MQTT_START   = "/mqtt_start";
const char* URL_MQTT_CLEAR   = "/mqtt_clear";
const char* URL_MQTT_STOP    = "/mqtt_stop";

// This example shows a sketch that realizes the equivalent operation
// of mqttRSSI without using JSON.
// By comparing this example with the example using JSON, mqttRSSI or
// mqttRSSI_FS, you will better understand AutoConnect custom Web page
// facility.

// Declare AutoConnectElements for the page asf `/mqtt_setting`
ACStyle(style, "label+input,label+select{position:sticky;left:140px;width:204px!important;box-sizing:border-box;}");
ACElement(header, "<h2 style='text-align:center;color:#2f4f4f;margin-top:10px;margin-bottom:10px'>MQTT Broker settings</h2>");
ACText(caption, "Publish WiFi signal strength via MQTT, publishing the RSSI value of the ESP module to the ThingSpeak public channel.", "font-family:serif;color:#053d76", "", AC_Tag_P);
ACInput(mqttserver, "", "Server", "^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$", "MQTT broker server");
ACInput(apikey, "", "Uer API Key");
ACInput(channelid, "", "Channel ID", "^[0-9]{6}$");
ACInput(writekey, "", "Write API Key");
ACElement(nl1, "<hr>");
ACText(credential, "MQTT Device Credentials", "font-weight:bold;color:#1e81b0", "", AC_Tag_DIV);
ACInput(clientid, "", "Client ID");
ACInput(username, "", "Username");
ACInput(password, "", "Password", "", "", AC_Tag_BR, AC_Input_Password);
ACElement(nl2, "<hr>");
ACRadio(period, { "30 sec.", "60 sec.", "180 sec." }, "Update period", AC_Vertical, 1);
ACInput(hostname, "", "ESP host name", "^([a-zA-Z0-9]([a-zA-Z0-9-])*[a-zA-Z0-9]){1,24}$");
ACSubmit(save, "Save&amp;Start", URL_MQTT_START);
ACSubmit(discard, "Discard", URL_MQTT_HOME);
ACSubmit(stop, "Stop publishing", URL_MQTT_STOP);

// Declare AutoConnectElements for the page asf `/mqtt_start`
ACText(c1, "<h4>MQTT publishing has started.</h4>", "text-align:center;color:#2f4f4f;padding:5px;");
ACText(c2, "<h4>Parameters saved as:</h4>", "text-align:center;color:#2f4f4f;padding:5px;");
ACText(mqttserver_p, "", "", "Server: %s", AC_Tag_BR);
ACText(apikey_p, "", "", "Uer API Key: %s", AC_Tag_BR);
ACText(channelid_p, "", "", "Channel ID: %s", AC_Tag_BR);
ACText(writekey_p, "", "", "Write API Key: %s", AC_Tag_BR);
ACText(clientid_p, "", "", "Client ID: %s", AC_Tag_BR);
ACText(username_p, "", "", "Username: %s", AC_Tag_BR);
ACText(password_p, "", "", "Password: %s", AC_Tag_BR);
ACText(hostname_p, "", "", "ESP host: %s", AC_Tag_BR);
ACText(period_p, "", "", "Update period: %s", AC_Tag_BR);
ACSubmit(clear, "Clear channel", URL_MQTT_CLEAR);

// Declare the custom Web page as `/mqtt_setting` and contains the AutoConnectElements
AutoConnectAux mqtt_setting(URL_MQTT_SETTING, "MQTT Setting", true, {
  style,
  header,
  caption,
  mqttserver,
  apikey,
  channelid,
  writekey,
  nl1,
  credential,
  clientid,
  username,
  password,
  nl2,
  period,
  hostname,
  save,
  discard,
  stop
});

// Declare the custom Web page as `/mqtt_start` and contains the AutoConnectElements
AutoConnectAux mqtt_start(URL_MQTT_START, "MQTT Setting", false, {
  c1,
  c2,
  mqttserver_p,
  apikey_p,
  channelid_p, 
  writekey_p,
  clientid_p,
  username_p,
  password_p,
  hostname_p,
  period_p,
  clear
});

// Declare the custom Web page as `/mqtt_stop`and contains the AutoConnectElements
// This declaration with `response=false` is required for the custom web page handler
// to respond to 302 redirects.
AutoConnectAux mqtt_stop(URL_MQTT_STOP, "MQTT Setting", false, {}, false);

// Declare the custom Web page as `/mqtt_clear` and contains the AutoConnectElements
// This declaration with `response=false` is required for the custom web page handler
// to respond to 302 redirects.
AutoConnectAux mqtt_clear(URL_MQTT_CLEAR, "MQTT Setting", false, {}, false);

WiFiWebServer server;
AutoConnect   portal(server);
AutoConnectConfig config;
WiFiClient    wifiClient;
PubSubClient  mqttClient(wifiClient);

// A variety of control variables to keep message publishing periodic.
mqtt_param_t  mqtt_param;

// Indicates the message publishing status. ESP module will continue to send
// messages periodically when enablePublish is true. `auxMQTTStop` handler
// can be false with a stop publish operation via the `/mqtt_stop` web page.
bool  enablePublish;
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
  memset(&mqtt_param, '\0', sizeof(mqtt_param_t));
  strncpy(mqtt_param.mqttServer, aux[F("mqttserver")].as<AutoConnectInput>().value.c_str(), sizeof(mqtt_param_t::mqttServer) - sizeof('\0'));
  strncpy(mqtt_param.apikey, aux[F("apikey")].as<AutoConnectInput>().value.c_str(), sizeof(mqtt_param_t::apikey) - sizeof('\0'));
  strncpy(mqtt_param.channelId, aux[F("channelid")].as<AutoConnectInput>().value.c_str(), sizeof(mqtt_param_t::channelId) - sizeof('\0'));
  strncpy(mqtt_param.writekey, aux[F("writekey")].as<AutoConnectInput>().value.c_str(), sizeof(mqtt_param_t::writekey) - sizeof('\0'));
  strncpy(mqtt_param.clientId, aux[F("clientid")].as<AutoConnectInput>().value.c_str(), sizeof(mqtt_param_t::clientId) - sizeof('\0'));
  strncpy(mqtt_param.username, aux[F("username")].as<AutoConnectInput>().value.c_str(), sizeof(mqtt_param_t::username) - sizeof('\0'));
  strncpy(mqtt_param.password, aux[F("password")].as<AutoConnectInput>().value.c_str(), sizeof(mqtt_param_t::password) - sizeof('\0'));
  strncpy(mqtt_param.hostname, aux[F("hostname")].as<AutoConnectInput>().value.c_str(), sizeof(mqtt_param_t::hostname) - sizeof('\0'));
  mqtt_param.publishInterval = aux[F("period")].as<AutoConnectRadio>().value().substring(0, 2).toInt() * 1000;
}

void loadParams(AutoConnectAux& aux) {
  aux[F("mqttserver")].as<AutoConnectInput>().value = mqtt_param.mqttServer;
  aux[F("apikey")].as<AutoConnectInput>().value = mqtt_param.apikey;
  aux[F("channelid")].as<AutoConnectInput>().value = mqtt_param.channelId;
  aux[F("writekey")].as<AutoConnectInput>().value = mqtt_param.writekey;
  aux[F("clientid")].as<AutoConnectInput>().value = mqtt_param.clientId;
  aux[F("username")].as<AutoConnectInput>().value = mqtt_param.username;
  aux[F("password")].as<AutoConnectInput>().value = mqtt_param.password;
  aux[F("hostname")].as<AutoConnectInput>().value = mqtt_param.hostname;
  aux[F("period")].as<AutoConnectRadio>().checked = mqtt_param.publishInterval / (30 * 1000);
  if (aux[F("period")].as<AutoConnectRadio>().checked > 3)
    aux[F("period")].as<AutoConnectRadio>().checked = 3;
}

// Bind the mDNS service to the http server launched by the ESP module.
void startMDNS(void) {
  if (!strlen(mqtt_param.hostname))
    strcpy(mqtt_param.hostname, WiFi.getHostname());
  else
    WiFi.setHostname(mqtt_param.hostname);

  Serial.printf("mDNS responder %s.local start\n", mqtt_param.hostname);
  if (MDNS.begin(mqtt_param.hostname)) {
    if (!MDNS.addService("http", "tcp", 80))
      Serial.println("mDNS service adding failed");
  }
  else
    Serial.println("mDNS failed");
}

// Start MQTT message publishing.
void startMQTT() {
  // Set MQTT broker endpoint to PubSubClient.
  Serial.printf("Starting MQTT, interval %lu, ", mqtt_param.publishInterval);
  if (mqtt_param.publishInterval > 0)
    enablePublish = true;
  else {
    enablePublish = false;
    Serial.println("not yet in operation");
  }
  nextPeriod = millis();
  retryCount = 0;
  mqttClient.setServer(mqtt_param.mqttServer, 1883);

  // Rebind mDNS service with `hostname`.
  if (strcmp(mqtt_param.hostname, WiFi.getHostname())) {
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

  if (strlen(mqtt_param.mqttServer)) {
    if (millis() > nextPeriod) {
      // Attempts to connect to the MQTT broker based on a valid server name.
      // mqttClient.setServer(mqtt.data.server.c_str(), 1883);
      if (!mqttClient.connected()) {
        Serial.printf("Attempting MQTT broker:%s\n", mqtt_param.mqttServer);
        if ((inPublish = mqttClient.connect(mqtt_param.clientId, mqtt_param.username, mqtt_param.password)))
          Serial.printf("Established:%s\n", mqtt_param.clientId);
        else
          Serial.print("Connection failed:" + String(mqttClient.state()));
      }

      if (inPublish) {
        String  topic = String("channels/") + String(mqtt_param.channelId) + String("/publish");
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
        nextPeriod = millis() + mqtt_param.publishInterval;
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
          nextPeriod = millis() + mqtt_param.publishInterval;
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
  loadParams(aux);
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

  // Save the value of each element entered by `/mqtt_setting`
  putParams(mqtt_param);

  // Transfer the contents of `mqtt_setting` to `mqtt_start`.
  aux[F("mqttserver_p")].as<AutoConnectText>().value = mqtt_param.mqttServer;
  aux[F("apikey_p")].as<AutoConnectText>().value = mqtt_param.apikey;
  aux[F("channelid_p")].as<AutoConnectText>().value = mqtt_param.channelId;
  aux[F("writekey_p")].as<AutoConnectText>().value = mqtt_param.writekey;
  aux[F("clientid_p")].as<AutoConnectText>().value = mqtt_param.clientId;
  aux[F("username_p")].as<AutoConnectText>().value = mqtt_param.username;
  aux[F("password_p")].as<AutoConnectText>().value = mqtt_param.password;
  aux[F("hostname_p")].as<AutoConnectText>().value = mqtt_param.hostname;
  aux[F("period_p")].as<AutoConnectText>().value = String(mqtt_param.publishInterval / 1000) + String(" sec.");

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

  String  endpoint(mqtt_param.mqttServer);
  endpoint.replace("mqtt3", "api");
  String  delUrl = "http://" + endpoint + "/channels/" + String(mqtt_param.channelId) + "/feeds.json?api_key=" + String(mqtt_param.apikey);

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

  content.replace("{{CHANNEL}}", String(mqtt_param.channelId));
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

  // When storing ThingSpeak channel settings in the ESP8266's EEPROM, its area
  // must be reserved to avoid conflicts with AutoConnect credentials.
  // `CREDENTIAL_OFFSET` has the size of the settings parameter area.
  config.boundaryOffset = CREDENTIAL_OFFSET;

  // Assign the captive portal popup screen to the URL as the root path.
  // Reconnect and continue publishing even if WiFi is disconnected.
  config.homeUri = URL_MQTT_HOME;
  config.bootUri = AC_ONBOOTURI_HOME;
  config.autoReconnect = true;
  config.reconnectInterval = 1;
  portal.config(config);

  // Join AutoConnectAux pages.
  portal.join({ mqtt_setting, mqtt_start, mqtt_clear, mqtt_stop });
  portal.on(URL_MQTT_SETTING, auxMQTTSetting);
  portal.on(URL_MQTT_START, auxMQTTStart);
  portal.on(URL_MQTT_CLEAR, auxMQTTClear);
  portal.on(URL_MQTT_STOP, auxMQTTStop);

  // Restore saved MQTT broker setting values.
  // This example stores all setting parameters as a set of AutoConnectElement,
  // so they can be restored in bulk using `AutoConnectAux::loadElement`.
  AutoConnectAux& settings_mqtt = *portal.aux(URL_MQTT_SETTING);
  getParams(mqtt_param);
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
