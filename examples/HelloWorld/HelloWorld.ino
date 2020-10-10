/*
  HelloWorld.ino, Example for the AutoConnect library.
  Copyright (c) 2019, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect

  This software is released under the MIT License.
  https://opensource.org/licenses/MIT
*/
/*
  To experience this example, upload the JSON file which is style.json
  from the data folder. Its file contains the attributes for the Caption
  of AutoConnectText. You can change the elements for your realization.
*/

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
typedef ESP8266WebServer WEBServer;
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
typedef WebServer WEBServer;
#endif
#include <AutoConnect.h>

/*
  AC_USE_SPIFFS indicates SPIFFS or LittleFS as available file systems that
  will become the AUTOCONNECT_USE_SPIFFS identifier and is exported as showng
  the valid file system. After including AutoConnect.h, the Sketch can determine
  whether to use FS.h or LittleFS.h by AUTOCONNECT_USE_SPIFFS definition.
*/
#ifdef AUTOCONNECT_USE_SPIFFS
#include <FS.h>
#if defined(ARDUINO_ARCH_ESP8266)
FS& FlashFS = SPIFFS;
#elif defined(ARDUINO_ARCH_ESP32)
fs::SPIFFSFS& FlashFS = SPIFFS;
#endif
#include <LittleFS.h>
FS& FlashFS = LittleFS;
#endif

#define HELLO_URI   "/hello"
#define PARAM_STYLE "/style.json"

// Declare AutoConnectText with only a value.
// Qualify the Caption by reading style attributes from the style.json file.
ACText(Caption, "Hello, world");

//AutoConnectAux for the custom Web page.
AutoConnectAux helloPage(HELLO_URI, "Hello", true, { Caption });
AutoConnect portal;

// JSON document loading buffer
String ElementJson;

// Redirects from root to the hello page.
void onRoot() {
  WEBServer& webServer = portal.host();
  webServer.sendHeader("Location", String("http://") + webServer.client().localIP().toString() + String(HELLO_URI));
  webServer.send(302, "text/plain", "");
  webServer.client().flush();
  webServer.client().stop();
}

// Load the attribute of the element to modify at runtime from external.
String onHello(AutoConnectAux& aux, PageArgument& args) {
  aux.loadElement(ElementJson);
  return String();
}

// Load the element from specified file in the flash on board.
void loadParam(const char* fileName) {
  Flash.begin();
  File param = FlashFS.open(fileName, "r");
  if (param) {
    ElementJson = param.readString();
    param.close();
  }
  FlashFS.end();
}

void setup() {
  delay(1000);
  Serial.begin(115200);

  loadParam(PARAM_STYLE);     // Pre-load the element from JSON.
  helloPage.on(onHello);      // Register the attribute overwrite handler.
  portal.join(helloPage);     // Join the hello page.
  portal.begin();

  WEBServer& webServer = portal.host();
  webServer.on("/", onRoot);  // Register the root page redirector.
}

void loop() {
  portal.handleClient();
}
