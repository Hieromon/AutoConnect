/*
  FileUpload.ino, Example for the AutoConnect library.
  Copyright (c) 2019, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect

  This software is released under the MIT License.
  https://opensource.org/licenses/MIT

  FileUpload.ino writes the file uploaded from the HTTP client to SPIFFS
  with its file name. To run this example successfully, you need the
  SPIFFS area setting with Arduino IDE Tool menu which is larger than
  the size of the upload file.

  This example leverages the AutoConnectFile element to upload files to
  the flash on ​the ESP8266/ESP32 module. The necessary basic process
  for uploading and storing to flash is already embedded in the
  AutoConnectFile element. By the sketch, just place the AutoConnectFile
  element on a custom web page.
*/

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#endif
#include <FS.h>
#include <AutoConnect.h>

static const char PAGE_UPLOAD[] PROGMEM = R"(
{
  "uri": "/",
  "title": "Upload",
  "menu": true,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "value": "<h2>File uploading platform<h2>"
    },
    {
      "name": "filename",
      "type": "ACFile",
      "label": "Select file: ",
      "store": "fs"
    },
    {
      "name": "upload",
      "type": "ACSubmit",
      "value": "UPLOAD",
      "uri": "/upload"
    }
  ]
}
)";

static const char PAGE_BROWSE[] PROGMEM = R"(
{
  "uri": "/upload",
  "title": "Upload",
  "menu": false,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "value": "<h2>Uploading ended<h2>"
    },
    {
      "name": "filename",
      "type": "ACText"
    },
    {
      "name": "size",
      "type": "ACText"
    },
    {
      "name": "content_type",
      "type": "ACText",
      "value": "Content: "
    }
  ]
}
)";

#if defined(ARDUINO_ARCH_ESP8266)
typedef ESP8266WebServer  WiFiWebServer;
#elif defined(ARDUINO_ARCH_ESP32)
typedef WebServer WiFiWebServer;
#endif

WiFiWebServer server;
AutoConnect portal(server);
AutoConnectAux auxUpload;
AutoConnectAux auxBrowse;

/**
 * Post uploading, AutoConnectFile's built-in upload handler reads the
 * file saved in SPIFFS and displays the file contents on /upload custom
 * web page. However, only files with mime type uploaded as text are
 * displayed. A custom web page handler is called after upload.
 * @param  aux  AutoConnectAux(/upload)
 * @param  args PageArgument
 * @return Uploaded text content
 */
String postUpload(AutoConnectAux& aux, PageArgument& args) {
  String  content;
  String  filename = auxUpload.getElement<AutoConnectFile>("filename").value;
  aux.getElement<AutoConnectText>("filename").value = filename;
  aux.getElement<AutoConnectText>("size").value = String(auxUpload.getElement<AutoConnectFile>("filename").size) + String("bytes uploaded");
  String contentType = auxUpload.getElement<AutoConnectFile>("filename").mimeType;
  aux.getElement<AutoConnectText>("content_type").value = contentType;
  if (contentType.indexOf("text/") >= 0) {
    SPIFFS.begin();
    File uploadFile = SPIFFS.open(String("/" + filename).c_str(), "r");
    if (uploadFile) {
      while (uploadFile.available()) {
        char c = uploadFile.read();
        if (c == '\n')
          content += "<br>";
        else
          content += c;
      }
      uploadFile.close();
    }
    else
      content = "Not saved";
    SPIFFS.end();
  }
  return content;
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  auxUpload.load(PAGE_UPLOAD);
  auxBrowse.load(PAGE_BROWSE);
  portal.join({ auxUpload, auxBrowse });
  auxBrowse.on(postUpload);
  portal.begin();
}

void loop() {
  portal.handleClient();
}
