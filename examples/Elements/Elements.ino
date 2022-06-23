/*
  Elements.ino, Example for the AutoConnect library.
  Copyright (c) 2020, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect
  This software is released under the MIT License.
  https://opensource.org/licenses/MIT

  This example demonstrates the typical behavior of AutoConnectElement.
  It also represents a basic structural frame for saving and reusing
  values ​​entered in a custom web page into flash.
*/

// To properly include the suitable header files to the target platform.
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#define FORMAT_ON_FAIL
using WebServerClass = ESP8266WebServer;
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#define FORMAT_ON_FAIL  true
using WebServerClass = WebServer;
#endif

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

#define PARAM_FILE      "/elements.json"
#define USERNAME        "username_you_wish"   // For HTTP authentication
#define PASSWORD        "password_you_wish"   // For HTTP authentication

static const char PAGE_ELEMENTS[] PROGMEM = R"(
{
  "uri": "/elements",
  "title": "Elements",
  "menu": true,
  "element": [
    {
      "name": "tablecss",
      "type": "ACStyle",
      "value": "table{font-family:arial,sans-serif;border-collapse:collapse;width:100%;color:black;}td,th{border:1px solid #dddddd;text-align:center;padding:8px;}tr:nth-child(even){background-color:#dddddd;}"
    },
    {
      "name": "text",
      "type": "ACText",
      "value": "AutoConnect element behaviors collection",
      "style": "font-family:Arial;font-size:18px;font-weight:400;color:#191970",
      "posterior": "div"
    },
    {
      "name": "check",
      "type": "ACCheckbox",
      "value": "check",
      "label": "Check",
      "labelposition": "infront",
      "checked": true
    },
    {
      "name": "input",
      "type": "ACInput",
      "label": "Text input",
      "placeholder": "This area accepts hostname patterns",
      "pattern": "^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$"
    },
    {
      "name": "pass",
      "type": "ACInput",
      "label": "Password",
      "apply": "password"
    },
    {
      "name": "number",
      "type": "ACInput",
      "label": "Number",
      "value": "3",
      "apply": "number",
      "pattern": "\\d*"
    },
    {
      "name": "hr",
      "type": "ACElement",
      "value": "<hr style=\"height:1px;border-width:0;color:gray;background-color:#52a6ed\">",
      "posterior": "par"
    },
    {
      "name": "radio",
      "type": "ACRadio",
      "value": [
        "Button-1",
        "Button-2",
        "Button-3"
      ],
      "label": "Radio buttons",
      "arrange": "vertical",
      "checked": 1
    },
    {
      "name": "select",
      "type": "ACSelect",
      "option": [
        "Option-1",
        "Option-2",
        "Option-3"
      ],
      "label": "Select",
      "selected": 2
    },
    {
      "name": "table",
      "type": "ACElement",
      "value": "<table><tr><th>Board</th><th>Platform</th></tr><tr><td>NodeMCU</td><td>Espressif8266</td></tr><tr><td>ESP32-DevKitC</td><td>Espressif32</td></tr></table>",
      "posterior": "par"
    },
    {
      "name": "upload",
      "type": "ACFile",
      "label": "Upload:",
      "store": "fs"
    },
    {
      "name": "load",
      "type": "ACSubmit",
      "value": "Load",
      "uri": "/elements"
    },
    {
      "name": "save",
      "type": "ACSubmit",
      "value": "Save",
      "uri": "/save"
    },
    {
      "name": "adjust_width",
      "type": "ACElement",
      "value": "<script type=\"text/javascript\">window.onload=function(){var t=document.querySelectorAll(\"input[type='text']\");for(i=0;i<t.length;i++){var e=t[i].getAttribute(\"placeholder\");e&&t[i].setAttribute(\"size\",e.length*.8)}};</script>"
    }
  ]
}
)";

static const char PAGE_SAVE[] PROGMEM = R"(
{
  "uri": "/save",
  "title": "Elements",
  "menu": false,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "format": "Elements have been saved to %s",
      "style": "font-family:Arial;font-size:18px;font-weight:400;color:#191970"
    },
    {
      "name": "validated",
      "type": "ACText",
      "style": "color:red"
    },
    {
      "name": "echo",
      "type": "ACText",
      "style": "font-family:monospace;font-size:small;white-space:pre;"
    },
    {
      "name": "ok",
      "type": "ACSubmit",
      "value": "OK",
      "uri": "/elements"
    }
  ]
}
)";

WebServerClass  server;
AutoConnect portal(server);
AutoConnectConfig config;
AutoConnectAux  elementsAux;
AutoConnectAux  saveAux;

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  FlashFS.begin(FORMAT_ON_FAIL);

  // Responder of root page handled directly from WebServer class.
  server.on("/", []() {
    String content = "Place the root page with the sketch application.&ensp;";
    content += AUTOCONNECT_LINK(COG_24);
    server.send(200, "text/html", content);
  });

  // Load a custom web page described in JSON as PAGE_ELEMENT and
  // register a handler. This handler will be invoked from
  // AutoConnectSubmit named the Load defined on the same page.
  elementsAux.load(FPSTR(PAGE_ELEMENTS));
  elementsAux.on([] (AutoConnectAux& aux, PageArgument& arg) {
    if (portal.where() == "/elements") {
      // Use the AutoConnect::where function to identify the referer.
      // Since this handler only supports AutoConnectSubmit called the
      // Load, it uses the uri of the custom web page placed to
      // determine whether the Load was called me or not.
      File param = FlashFS.open(PARAM_FILE, "r");
      if (param) {
        aux.loadElement(param, { "text", "check", "input", "input", "pass", "number", "radio", "select" } );
        param.close();
      }
    }
    return String();
  });

  saveAux.load(FPSTR(PAGE_SAVE));
  saveAux.on([] (AutoConnectAux& aux, PageArgument& arg) {
    // You can validate input values ​​before saving with
    // AutoConnectInput::isValid function.
    // Verification is using performed regular expression set in the
    // pattern attribute in advance.
    AutoConnectInput& input = elementsAux["input"].as<AutoConnectInput>();
    aux["validated"].value = input.isValid() ? String() : String("Input data pattern missmatched.");

    // The following line sets only the value, but it is HTMLified as
    // formatted text using the format attribute.
    aux["caption"].value = PARAM_FILE;

    File param = FlashFS.open(PARAM_FILE, "w");
    if (param) {
      // Save as a loadable set for parameters.
      elementsAux.saveElement(param, { "text", "check", "input", "pass", "number", "radio", "select" });
      param.close();
      // Read the saved elements again to display.
      param = FlashFS.open(PARAM_FILE, "r");
      aux["echo"].value = param.readString();
      param.close();
    }
    else {
      aux["echo"].value = "Filesystem failed to open.";
    }
    return String();
  });

  portal.join({ elementsAux, saveAux });
  config.auth = AC_AUTH_BASIC;
  config.authScope = AC_AUTHSCOPE_AUX;
  config.username = USERNAME;
  config.password = PASSWORD;
  config.ticker = true;
  config.autoReconnect = true;
  config.reconnectInterval = 1;
  portal.config(config);
  portal.begin();
}

void loop() {
  portal.handleClient();
}
