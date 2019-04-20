/*
  WebUpdate.ino, Example for the AutoConnect library.
  Copyright (c) 2018, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect
  This example is an implementation of a lightweight update feature
  that updates the ESP8266's firmware from your web browser. It embeds
  ESP8266HTTPUpdateServer into the AutoConnect menu and can invoke the
  firmware update UI via a Web browser.
  You need a compiled sketch binary file to the actual update and can
  retrieve it using Arduino-IDE menu: [Sketck] -> [Export compiled binary].
  Then you will find the .bin file in your sketch folder. Select the.bin
  file on the update UI page to update the firmware.

  Notes:
  1. This example is valid only for ESP8266. In order to apply this
  example to ESP32, it is necessary to quote WebUpdate.ino included
  in the ESP32 arduino core distribution and implement a class
  equivalent to ESP8266HTTPUpdateServer. But it is not included in this
  example.

  2. To experience this example, your client OS needs to be running a
  service that can respond to multicast DNS.
  For Mac OSX support is built in through Bonjour already.
  For Linux, install Avahi.
  For Windows10, available since Windows10 1803(April 2018 Update/RS4).

  3. If you receive an error as follows:
  Update error: ERROR[11]: Invalid bootstrapping state, reset ESP8266 before updating.
  You need reset the module before sketch running.
  Refer to https://hieromon.github.io/AutoConnect/faq.html#hang-up-after-reset for details.

  This software is released under the MIT License.
  https://opensource.org/licenses/MIT
*/
#ifdef ARDUINO_ARCH_ESP8266

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <AutoConnect.h>

// This page for an example only, you can prepare the other for your application.
static const char AUX_AppPage[] PROGMEM = R"(
{
  "title": "Hello world",
  "uri": "/",
  "menu": true,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "value": "<h2>Hello, world</h2>",
      "style": "text-align:center;color:#2f4f4f;padding:10px;"
    },
    {
      "name": "content",
      "type": "ACText",
      "value": "In this page, place the custom web page handled by the sketch application."
    }
  ]
}
)";

// Fix hostname for mDNS. It is a requirement for the lightweight update feature.
static const char* host = "esp8266-webupdate";
#define HTTP_PORT 80

// ESP8266WebServer instance will be shared both AutoConnect and UpdateServer.
ESP8266WebServer  httpServer(HTTP_PORT);

// Declare AutoConnectAux to bind the HTTPWebUpdateServer via /update url
// and call it from the menu.
// The custom web page is an empty page that does not contain AutoConnectElements.
// Its content will be emitted by ESP8266HTTPUpdateServer.
ESP8266HTTPUpdateServer httpUpdater;
AutoConnectAux  update("/update", "Update");

// Declare AutoConnect and the custom web pages for an application sketch.
AutoConnect     portal(httpServer);
AutoConnectAux  hello;

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println("\nBooting Sketch...");

  // Prepare the ESP8266HTTPUpdateServer
  // The /update handler will be registered during this function.
  httpUpdater.setup(&httpServer);

  // Load a custom web page for a sketch and a dummy page for the updater.
  hello.load(AUX_AppPage);
  portal.join({ hello, update });

  if (portal.begin()) {
    if (MDNS.begin(host)) {
        MDNS.addService("http", "tcp", HTTP_PORT);
        Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
    }
    else
      Serial.println("Error setting up MDNS responder");
  }
}

void loop() {
  // Sketch the application here.

  // Invokes mDNS::update and AutoConnect::handleClient() for the menu processing.
  MDNS.update();
  portal.handleClient();
}

#else
void setup() {}
void loop() {}
#endif
