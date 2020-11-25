/*
  HelloWorld.ino, Example for the AutoConnect library.
  Copyright (c) 2020, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect

  This software is released under the MIT License.
  https://opensource.org/licenses/MIT
*/
/*
  To experience this example, upload the JSON files which are .json
  from the data folder. These files contain the attributes for the
  Caption as AutoConnectText.
  You can change the elements for your realization.
*/

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
typedef ESP8266WebServer WEBServer;
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
typedef WebServer WEBServer;
#endif
#include <AutoConnect.h>

/*
  AC_USE_SPIFFS indicates SPIFFS or LittleFS as available file systems
  that will become the AUTOCONNECT_USE_SPIFFS identifier and is exported
  as showng the valid file system. After including AutoConnect.h, the
  Sketch can determine whether to use FS.h or LittleFS.h by
  AUTOCONNECT_USE_SPIFFS definition.
*/
#include <FS.h>
#if defined(ARDUINO_ARCH_ESP8266)
#ifdef AUTOCONNECT_USE_SPIFFS
FS& FlashFS = SPIFFS;
#else
#include <LittleFS.h>
FS& FlashFS = LittleFS;
#endif
#elif defined(ARDUINO_ARCH_ESP32)
#include <SPIFFS.h>
fs::SPIFFSFS& FlashFS = SPIFFS;
#endif

#define HELLO_URI   "/hello"

// Declare AutoConnectText with only a value.
// Qualify the Caption by reading style attributes from the style.json file.
ACText(Caption, "Hello, world");
ACRadio(Styles, {}, "");
ACSubmit(Apply, "Apply", HELLO_URI);

//AutoConnectAux for the custom Web page.
AutoConnectAux helloPage(HELLO_URI, "Hello", true, { Caption, Styles, Apply });
AutoConnect portal;

// JSON document loading buffer
String ElementJson;

// Load the element from specified file in the flash on board.
void loadParam(const char* fileName) {
  Serial.printf("Style %s ", fileName);
  File param = FlashFS.open(fileName, "r");
  if (param) {
    ElementJson = param.readString();
    param.close();
    Serial.printf("loaded:\n%s", ElementJson.c_str());
  }
  else
    Serial.println("open failed");
}

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
  // Select the style parameter file and load it into the text element.
  AutoConnectRadio& styles = helloPage["Styles"].as<AutoConnectRadio>();
  String  styleName = styles.value();
  if (styleName.length())
    loadParam(styleName.c_str());

  // List parameter files stored on the flash.
  // Those files need to be uploaded to the filesystem in advance.
  styles.empty();
#ifdef AUTOCONNECT_USE_SPIFFS
  Dir dir = FlashFS.openDir("/");
  while (dir.next()) {
    styles.add(dir.fileName());
  }
#else
  File  dir = FlashFS.open("/", "r");
  if (dir) {
    File  parmFile = dir.openNextFile();
    while (parmFile) {
      if (!parmFile.isDirectory())
        styles.add(String(parmFile.name()));
      parmFile = dir.openNextFile();
    }
    parmFile.close();
  }
#endif
  // Apply picked style
  helloPage.loadElement(ElementJson);
  return String();
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
#if defined(ARDUINO_ARCH_ESP8266)
  FlashFS.begin();
#elif defined(ARDUINO_ARCH_ESP32)
  FlashFS.begin(true);
#endif

  helloPage.on(onHello);      // Register the attribute overwrite handler.
  portal.join(helloPage);     // Join the hello page.
  portal.begin();

  WEBServer& webServer = portal.host();
  webServer.on("/", onRoot);  // Register the root page redirector.
}

void loop() {
  portal.handleClient();
}
