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
#include <FS.h>
#include <AutoConnect.h>

#define HELLO_URI   "/hello"
#define PARAM_STYLE "/style.json"

// Declare AutoConnectElements and AutoConnectAux for the custom Web page.
ACText(Caption, "Hello, world");
AutoConnectAux helloPage(HELLO_URI, "Hello", true, { Caption });
AutoConnect portal;

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
  File param = SPIFFS.open(PARAM_STYLE, "r");
  aux.loadElement(param);
  param.close();
  return String();
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  SPIFFS.begin();             // Prepare SPIFFS

  helloPage.on(onHello);      // Register the attribute overwrite handler.
  portal.join(helloPage);     // Join the hello page.
  portal.begin();

  WEBServer& webServer = portal.host();
  webServer.on("/", onRoot);  // Register the root page redirector.
}

void loop() {
  portal.handleClient();
}
