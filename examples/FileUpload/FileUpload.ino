/*
  FileUpload.ino, Example for the AutoConnect library.
  Copyright (c) 2020, Hieromon Ikasamo
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
#include <AutoConnect.h>

/*
  AC_USE_SPIFFS indicates SPIFFS or LittleFS as available file systems that
  will become the AUTOCONNECT_USE_SPIFFS identifier and is exported as showng
  the valid file system. After including AutoConnect.h, the Sketch can determine
  whether to use FS.h or LittleFS.h by AUTOCONNECT_USE_SPIFFS definition.
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

// Upload request custom Web page
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
      "name": "upload_file",
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

// Upload result display
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
      "type": "ACText",
      "format": "%s bytes uploaded"
    },
    {
      "name": "content_type",
      "type": "ACText",
      "format": "Content: %s"
    }
  ]
}
)";

#if defined(ARDUINO_ARCH_ESP8266)
#define FILE_MODE_R "r"
typedef ESP8266WebServer  WiFiWebServer;
#elif defined(ARDUINO_ARCH_ESP32)
#define FILE_MODE_R FILE_READ
typedef WebServer WiFiWebServer;
#endif

WiFiWebServer server;
AutoConnect portal(server);
// Declare AutoConnectAux separately as a custom web page to access
// easily for each page in the post-upload handler.
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
  // Explicitly cast to the desired element to correctly extract
  // the element using the operator [].
  AutoConnectFile&  upload = auxUpload["upload_file"].as<AutoConnectFile>();
  AutoConnectText&  aux_filename = aux["filename"].as<AutoConnectText>();
  AutoConnectText&  aux_size = aux["size"].as<AutoConnectText>();
  AutoConnectText&  aux_contentType = aux["content_type"].as<AutoConnectText>();
  // Assignment operator can be used for the element attribute.
  aux_filename.value = upload.value;
  aux_size.value = String(upload.size);
  aux_contentType.value = upload.mimeType;
  // The file saved by the AutoConnect upload handler is read from
  // the EEPROM and echoed to a custom web page.
  if (upload.mimeType.indexOf("text/") >= 0) {
    FlashFS.begin();
    File uploadFile = FlashFS.open(String("/" + upload.value).c_str(), FILE_MODE_R);
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
    FlashFS.end();
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
