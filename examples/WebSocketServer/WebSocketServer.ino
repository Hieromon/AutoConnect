/*
  Graphical chart of ESP8266/ESP32 WiFi signal strength as RSSI on a browser.
  This example presents a typical application of WebSocket combined with AutoConnect.
  AutoConnect cannot host ESPAsyncWebServer as a WebServer, but can coexist with
  it for WebSocket communication.

  AutoConnect hosts a WebServer for the captive portal and custom web pages. If
  your custom web pages or native HTML implements websocket communication, you
  can take advantage of the WebSocket plugin with ESPAsyncWebServer that you
  instantiate in the sketch. This example displays a graphical chart of the
  signal strength of WiFi reception on a native HTML page stored in the ESP
  module's file system. The WebSocket implementation shown in this example shows
  how to let the WebServer hosted by AutoConnect handle HTTP sessions for native
  HTML content while communicating via WebSocket between the client browser and
  the ESP module.

  This sketch uses WebSockets for two purposes: Notify the client browser of the
  measured values, and to receive sampling rate settings from the client. The
  sampling rate is received by the `onEvent` handler of the AsyncWebSocket class,
  and the measured value is sent using the `textAll` function of AsyncWebSocket.
  Both message formats employ JSON, and this sketch shows the utility of the
  ArduinoJson library.

  To experience this example, the following libraries must be installed in the
  Arduino IDE environment beforehand:
    - ESPAsyncWebServer: https://github.com/me-no-dev/ESPAsyncWebServer
    - ESPAsyncTCP: https://github.com/me-no-dev/ESPAsyncTCP
    - AsyncTCP: https://github.com/me-no-dev/AsyncTCP
    - ArduinoJson: https://github.com/bblanchon/ArduinoJson
      
  It is also necessary to upload the `index.html` file in the `data` folder to
  the ESP module file system. Specify the destination file system as LittleFS
  for ESP8266 or SPIFFS for ESP32.

  Graph chart implementation contained in index.html relies on Chart.js. It is
  also enhanced with the Char.js plugin for streaming and zooming.
  For more information on Chart.js and plug-ins for it, refer to the following URLs:
    - Chart.js: https://www.chartjs.org/
    - Streaming plugin: https://nagix.github.io/chartjs-plugin-streaming/latest/
    - Zoom and pan plugin: https://www.chartjs.org/chartjs-plugin-zoom/latest/

  Copyright (c) 2022, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect

  This software is released under the MIT License.
  https://opensource.org/licenses/MIT
*/

#include <Arduino.h>
// The following include directive block absorbs the differences between the
// ESP8266 and ESP32 in the WebServer library and default file system.
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#define WEBSERVER_H
#define mDNSUpdate()  do {MDNS.update();} while (0)
#define FORMAT_ON_FAIL
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#define mDNSUpdate() do {(void)0;} while(0)
#define FORMAT_ON_FAIL  false
#endif

// Delegate WebSocket communications to be applied between clients to the
// ESPAsyncWebServer library. It can coexist with AutoConnect.
#include <ESPAsyncWebServer.h>
// This sketch does not have a custom web page, so it works only with the core
// components of AutoConnect. As an alternative to AutoConnect.h,
// AutoConnectCore.h can be included to reduce the built binary size.
#include <AutoConnectCore.h>
// The JSON format is applied to the message body of the WebSocket communication.
// The ArduinoJson library improves the convenience of sketch coding.
#include <ArduinoJson.h>
#if ARDUINOJSON_VERSION_MAJOR < 6
#error "Requires ArduinoJson version 6 or later"
#endif

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

#include "MeasureRSSI.h"

const int httpPort = 80;
const int wsPort = 3000;  // Assign port 3000 for WebSocket.

// The server instance for WebSocket uses ESPAsyncWebServer.
AsyncWebServer  wsServer(wsPort);
AsyncWebSocket  ws("/");

// This server instance is responsible for serving web pages. It is the host
// that AutoConnect will live on.
WebServer server(httpPort);

// Declare AutoConnect instance, as well known.
AutoConnect       portal(server);
AutoConnectConfig config;

// The MeasureRSSI class measures RSSI in realtime without delay. It measures
// RSSI a specified number of times at a specified measurement period in msec
// and detects the average of those RSSI values as the measurement value. It
// doesn't use the delay function to create a measurement cycle, so using it
// from within a loop function has no effect on ESPAsyncWebServer asynchronous
// processing.
MeasureRSSI rssi(3000);

// JSON object for messages to be applied to and from the client. It is
// managed by ArduinoJson.
StaticJsonDocument<32> measurements;

// The HTML for the graph chart is handled by index.html, which has an
// interactive UI for changing the RSSI measurement cycle and notifying the ESP
// module via WebSocket from the client. AsyncWebSocket will capture the
// message and pass it to the event handler as the updatePeriod function, which
// updates the measurement period of the ESP module side according to the value
// of the "period" contained in the WebSocket message.
void updatePeriod(const char* message) {
  measurements.clear();
  DeserializationError  err = deserializeJson(measurements, message, strlen(message));
  if (!err) {
    if (measurements.containsKey("period")) {
      // period = measurements["period"].as<unsigned long>();
      rssi.period = measurements["period"].as<unsigned long>();
    }
    else {
      Serial.println("[WS] No required key");
    }
  }
  else {
    Serial.printf("[WS] Message deserialization failed:%s\n", err.c_str());
  }
}

// Launch of mDNS service
void startMDNS(void) {
  MDNS.end();
  const char* hostname = WiFi.getHostname();
  Serial.printf("mDNS responder %s.local ", hostname);
  if (MDNS.begin(hostname)) {
    Serial.print("service ");
    if (MDNS.addService("http", "tcp", httpPort))
      Serial.println("started");
    else
      Serial.println("failed");
  }
  else
    Serial.println("could not start");
}

// The following event handlers for AsyncWebSocket correspond to message
// reception events.
void onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
  Serial.printf("[WS] Event(%u):%d\n", client->id(), type);

  // It applies the screening process to extract the message body with the event
  // type = WS_EVT_DATA.
  if (type == WS_EVT_DATA) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len) {
      Serial.printf("[WS] (%u):%s ", client->id(), server->url());
      if (info->opcode == WS_TEXT) {
        data[len] = '\0';
        Serial.println((const char*)data);

        // Update the measurement cycle.
        updatePeriod((const char*)data);
      }
      else {
        for (size_t i = 0; i < info->len; i++) {
          Serial.printf("%02x ", data[i]);
        }
        Serial.println();
      }
    }
  }
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  FlashFS.begin(FORMAT_ON_FAIL);

  // It is the role of the WebServer instance to serve the index.html page.
  // ESPAsyncWebServer does not interfere with this process.
  server.serveStatic("/", FlashFS, "/index.html");
  server.serveStatic("/index.html", FlashFS, "/index.html");
  server.onNotFound([]() {
    server.send(404, "text/plain", "File not found");
  });

  // Register an event handler for WebSocket and start ESPAsyncWebServer.
  ws.onEvent(onEvent);
  wsServer.addHandler(&ws);

  config.autoReconnect = true;
  config.reconnectInterval = 1;
  portal.config(config);

  // Start mDNS service and AsyncWebServer only when WiFi is connected.
  portal.onConnect([](IPAddress ip) {
    Serial.println(WiFi.SSID() + " connected:" + ip.toString());
    startMDNS();
    wsServer.begin();
  });

  portal.begin();
}

void loop() {
  if (rssi.measure()) {
    // RSSI measurement is not performed until the cycle is reached.
    // Do not use DELAY for this waiting process. It will cause unexpected results
    // that will compromise the asynchronous nature of ESPAsyncWebServer.
    if (ws.count()) {
      char  payload[32];

      // Send a measurement via WebSocket
      measurements.clear();
      measurements["rssi"] = rssi.rssi;
      serializeJson(measurements, payload);
      ws.textAll(payload);
      Serial.printf("[WS] send:%s\n", payload);
    }
  }

  portal.handleClient();
  mDNSUpdate();
}
