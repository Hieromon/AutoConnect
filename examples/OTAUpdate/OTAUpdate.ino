/*
  OTAUpdate.ino, Example for the AutoConnect library.
  Copyright (c) 2020, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect
  This example is an implementation of a lightweight update feature
  that updates the ESP8266's firmware from your web browser.
  You need a compiled sketch binary file to the actual update and can
  retrieve it using Arduino-IDE menu: [Sketch] -> [Export compiled binary].
  Then you will find the .bin file in your sketch folder. Select the.bin
  file on the update UI page to update the firmware.

  Notes:
  If you receive a following error, you need reset the module before sketch running.
  Update error: ERROR[11]: Invalid bootstrapping state, reset ESP8266 before updating.
  Refer to https://hieromon.github.io/AutoConnect/faq.html#hang-up-after-reset for details.

  This software is released under the MIT License.
  https://opensource.org/licenses/MIT
*/

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
typedef ESP8266WebServer  WiFiWebServer;
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
typedef WebServer WiFiWebServer;
#endif
#include <AutoConnect.h>

WiFiWebServer server;
AutoConnect portal(server);
AutoConnectConfig config;

void exitOTAStart() {
  Serial.println("OTA started");
}

void exitOTAProgress(unsigned int amount, unsigned int sz) {
  Serial.printf("OTA in progress: received %d bytes, total %d bytes\n", sz, amount);
}

void exitOTAEnd() {
  Serial.println("OTA ended");
}

void exitOTAError(uint8_t err) {
  Serial.printf("OTA error occurred %d\n", err);
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  // Responder of root page and apply page handled directly from WebServer class.
  server.on("/", []() {
    String content = R"(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8" name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
Place the root page with the sketch application.&ensp;
__AC_LINK__
</body>
</html>
    )";
    content.replace("__AC_LINK__", String(AUTOCONNECT_LINK(COG_16)));
    server.send(200, "text/html", content);
  });

  config.ota = AC_OTA_BUILTIN;
  portal.config(config);
  portal.onOTAStart(exitOTAStart);
  portal.onOTAEnd(exitOTAEnd);
  portal.onOTAProgress(exitOTAProgress);
  portal.onOTAError(exitOTAError);
  portal.begin();
}

void loop() {
  portal.handleClient();
}
