/*
  HandlePortalEX.ino, Example for the AutoConnect library.
  Copyright (c) 2018, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect

  This software is released under the MIT License.
  https://opensource.org/licenses/MIT
*/
/*
  This is an explicit declaration of ESP8266WebServer. AutoConnect uses
  its declaration. Also, by using PageBuilder for HTML assembly, you can
  display the web page without using the ESP8266WebServer::send() function.
  I recommend that you consider this example compared to HandlePortal.ino.
  https://github.com/Hieromon/AutoConnect/blob/master/examples/HandlePortal/HandlePortal.ino
  It will help you understand AutoConnect usage.
*/

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#endif
#include <PageBuilder.h>
#include <AutoConnect.h>

#ifndef BUILTIN_LED
#define BUILTIN_LED  2  // backward compatibility
#endif

#if defined(ARDUINO_ARCH_ESP8266)
ESP8266WebServer server;
#elif defined(ARDUINO_ARCH_ESP32)
WebServer server;
#endif
AutoConnect      portal(server);

void sendRedirect(String);

static const char PROGMEM mold_page[] = R"*lit(
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style type="text/css">
  body {
    -webkit-appearance:none;
    -moz-appearance:none;
    font-family:'Arial',sans-serif;
    text-align:center;
  }
  .menu > a:link {
    position: absolute;
    display: inline-block;
    right: 12px;
    padding: 0 6px;
    text-decoration: none;
  }
  .button {
    display:inline-block;
    border-radius:7px;
    background:#73ad21;
    margin:0 10px 0 10px;
    padding:10px 20px 10px 20px;
    text-decoration:none;
    color:#000000;
  }
  </style>
</head>
<body>
  <div class="menu">{{AUTOCONNECT_MENU}}</div>
  BUILT-IN LED<br>
  GPIO({{LED}}) : <span style="font-weight:bold;color:{{COLOR}}">{{GPIO}}</span>
  <p><a class="button" href="/io?v=low">low</a><a class="button" href="/io?v=high">high</a></p>
  </body>
</html>
)*lit";

static const char PROGMEM autoconnectMenu[] = { AUTOCONNECT_LINK(BAR_24) };

String getLEDPort(PageArgument& args) {
  return String(BUILTIN_LED);
}

String setColor(PageArgument& args) {
  return digitalRead(BUILTIN_LED) ? "Tomato" : "SlateBlue";
}

String readLEDPort(PageArgument& args) {
  return digitalRead(BUILTIN_LED) ? "HIGH" : "LOW";
}

PageElement elm_gpio(mold_page, {
  {"LED", getLEDPort},
  {"COLOR", setColor},
  {"GPIO", readLEDPort},
  {"AUTOCONNECT_MENU", [](PageArgument& args) {
    return String(FPSTR(autoconnectMenu));}}
});
PageBuilder root("/", { elm_gpio });


String gpio(PageArgument& args) {
  if (args.arg("v") == "low")
    digitalWrite(BUILTIN_LED, LOW);
  else if (args.arg("v") == "high")
    digitalWrite(BUILTIN_LED, HIGH);
  sendRedirect("/");
  return "";
}

PageElement elm_io("{{IO}}", { {"IO", gpio} });
PageBuilder io("/io", { elm_io });

// This function is for redirect only.
// The actual sending the HTML performs in PageBuilder.
void sendRedirect(String uri) {
  server.sendHeader("Location", uri, true);
  server.send(302, "text/plain", "");
  server.client().stop();
  io.cancel();
}

bool atDetect(IPAddress& softapIP) {
  Serial.println("Captive portal started, SoftAP IP:" + softapIP.toString());
  return true;
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  pinMode(BUILTIN_LED, OUTPUT);

  // Put the home location of the web site.
  // But in usually, setting the home uri is not needed cause default location is "/".
  //portal.home("/");   
  
  // Starts user web site included the AutoConnect portal.
  portal.onDetect(atDetect);
  if (portal.begin()) {
    // Register the page request handlers.
    root.insert(server);
    io.insert(server);
    Serial.println("Started, IP:" + WiFi.localIP().toString());
  }
  else {
    Serial.println("Connection failed.");
    while (true) { yield(); }
  }
}

void loop() {
  portal.handleClient();
  if (WiFi.status() == WL_IDLE_STATUS) {
#if defined(ARDUINO_ARCH_ESP8266)
    ESP.reset();
#elif defined(ARDUINO_ARCH_ESP32)
    ESP.restart();
#endif
    delay(1000);
  }
}
