/*
  Update.ino, Example for the AutoConnect library.
  Copyright (c) 2019, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect
  This software is released under the MIT License.
  https://opensource.org/licenses/MIT

  This example presents the simplest OTA Updates scheme.
  When building this sketch, you may receive a compilation error message
  similar to the following:
  - text section exceeds available space in board
  This cause is the small text block size of the predetermined partition
  table. You can avoid this error by selecting Partition Scheme: from
  Arduino IDE's Tool menu and applying Minimal SPIFFS.
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

// Update server setting page
static const char SETUP_PAGE[] PROGMEM = R"(
{
  "title": "Update setup",
  "uri": "/setup",
  "menu": true,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "value": "OTA update setup",
      "style": ""
    },
    {
      "name": "isvalid",
      "type": "ACText",
      "style": "color:red"
    },
    {
      "name": "server",
      "type": "ACInput",
      "label": "Update server",
      "pattern": "^((([a-zA-Z]|[a-zA-Z][a-zA-Z0-9-]*[a-zA-Z0-9]).)*([A-Za-z]|[A-Za-z][A-Za-z0-9-]*[A-Za-z0-9]))|((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)){3})$",
      "placeholder": "Your update server address"
    },
    {
      "name": "port",
      "type": "ACInput",
      "label": "port",
      "pattern": "[0-9]{1,4}"
    },
    {
      "name": "path",
      "type": "ACInput",
      "label": "path"
    },
    {
      "name": "apply",
      "type": "ACSubmit",
      "value": "Apply",
      "uri": "/apply"
    },
    {
      "name": "cancel",
      "type": "ACSubmit",
      "value": "Discard",
      "uri": "/"
    }
  ]
}
)";

// The /apply handler validates the update server settings entered on
// the setup page. APPLY_PAGE exists to enable the /apply handler,
// and its role is a page redirector. If the /apply handler detects some
// errors, the page will redirect to the /setup page with error message.
static const char APPLY_PAGE[] PROGMEM = R"(
{
  "title": "Update setup",
  "uri": "/apply",
  "menu": false,
  "element": [
    {
      "name": "redirect",
      "type": "ACElement",
      "value": "<script type=\"text/javascript\">location.href='__REDIRECT__';</script>"
    }
  ]
}
)";

WebServerClass    server;
AutoConnect       portal(server);
AutoConnectAux    setupPage;
AutoConnectAux    applyPage;
AutoConnectUpdate update;

#define UPDATESERVER_URL  ""    // Define to suit your environment 
#define UPDATESERVER_PORT 8000
#define UPDATESERVER_PATH "bin"

void loadAux() {
  setupPage.load(SETUP_PAGE);
  setupPage["server"].value = UPDATESERVER_URL;
  setupPage["port"].value = String(UPDATESERVER_PORT);
  setupPage["path"].value = UPDATESERVER_PATH;
  applyPage.load(APPLY_PAGE);
}

// The onSetup handler clears the error message field of the /setup page.
// Its field will be cleared after the /setup page generating by the
// effect of the AC_EXIT_LATER option.
String onSetup(AutoConnectAux& aux, PageArgument& arg) {
  setupPage["isvalid"].value = String();
  return String();
}

// The onApply handler validates the update server configuration.
// It does not do any semantic analysis but only verifies that the
// settings match the pattern defined in each field.
// The AutoConnectInput isValid function checks if the current value
// matches the pattern.
String onApply(AutoConnectAux& aux, PageArgument& arg) {
  String  returnUri;

  AutoConnectInput& host = setupPage["server"].as<AutoConnectInput>();
  AutoConnectInput& port = setupPage["port"].as<AutoConnectInput>();
  AutoConnectInput& path = setupPage["path"].as<AutoConnectInput>();

  Serial.printf("host: %s\n", host.value.c_str());
  Serial.printf("port: %s\n", port.value.c_str());
  Serial.printf("uri: %s\n", path.value.c_str());

  if (host.isValid() & port.isValid()) {
    update.host = host.value;
    update.port = port.value.toInt();
    update.uri = path.value;
    setupPage["isvalid"].value = String();
    returnUri = "/";
  }
  else {
    setupPage["isvalid"].value = String("Incorrect value specified.");
    returnUri = "/setup";
  }
  applyPage["redirect"].value.replace("__REDIRECT__", returnUri);
  return String();
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  // Responder of root page and apply page  handled directly from WebServer class.
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

  // AUX page loading
  loadAux();  
  setupPage.on(onSetup, AC_EXIT_LATER);
  applyPage.on(onApply);
  portal.join({ setupPage, applyPage });
  portal.begin();
  update.attach(portal);
}

void loop() {
  portal.handleClient();
}
