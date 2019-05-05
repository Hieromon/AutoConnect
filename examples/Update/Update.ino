/*
  Update.ino, Example for the AutoConnect library.
  Copyright (c) 2019, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect
  This software is released under the MIT License.
  https://opensource.org/licenses/MIT

  This example presents the simplest OTA Updates scheme.
*/

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
using WebServerClass = ESP8266WebServer;
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
using WebServerClass = WebServer;
#endif
#include <AutoConnect.h>

#define UPDATESERVER_URL  "192.168.1.9"
#define UPDATESERVER_PORT 8000
#define UPDATESERVER_PATH "bin"

WebServerClass    server;
AutoConnect       portal(server);
AutoConnectUpdate update(UPDATESERVER_URL, UPDATESERVER_PORT, UPDATESERVER_PATH);
bool  atFirst;

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  // Responder of root page handled directly from WebServer class.
  server.on("/", []() {
    String content = "Place the root page with the sketch application.&ensp;";
    content += AUTOCONNECT_LINK(COG_24);
    server.send(200, "text/html", content);
  });

  portal.begin();
  update.attach(portal);
}

void loop() {
  portal.handleClient();
}
