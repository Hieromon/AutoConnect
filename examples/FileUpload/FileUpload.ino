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
  the flash on the ESP8266/ESP32 module. The necessary basic process
  for uploading and storing to flash is already embedded in the
  AutoConnectFile element. By the sketch, just place the AutoConnectFile
  element on a custom web page.
*/

// To properly include the suitable header files to the target platform.
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#define FORMAT_ON_FAIL
using WiFiWebServer = ESP8266WebServer;
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#define FORMAT_ON_FAIL  true
using WiFiWebServer = WebServer;
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
      "value": "<h2>File uploading platform</h2>"
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
      "value": "<h2>Uploading ended</h2>"
    },
    {
      "name": "filename",
      "type": "ACText",
      "posterior": "br"
    },
    {
      "name": "size",
      "type": "ACText",
      "format": "%s bytes uploaded",
      "posterior": "br"
    },
    {
      "name": "content_type",
      "type": "ACText",
      "format": "Content: %s",
      "posterior": "br"
    },
    {
      "name": "object",
      "type": "ACElement"
    }
  ]
}
)";

WiFiWebServer server;
AutoConnect portal(server);
// Declare AutoConnectAux separately as a custom web page to access
// easily for each page in the post-upload handler.
AutoConnectAux auxUpload;
AutoConnectAux auxBrowse;

#ifdef ARDUINO_ARCH_ESP32
String getContentType(const String& filename) {
  if (filename.endsWith(".txt")) {
    return "text/plain";
  } else if (filename.endsWith(".htm")) {
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".json")) {
    return "application/json";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".jpeg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".svg")) {
    return "image/svg+xml";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "application/octet-stream";
}
#endif

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
  Serial.printf("uploaded file saved as %s\n", aux_filename.value.c_str());
  aux_size.value = String(upload.size);
  aux_contentType.value = upload.mimeType;

  // Include the uploaded content in the object tag to provide feedback
  // in case of success.
  String  uploadFileName = String("/") + aux_filename.value;
  if (FlashFS.exists(uploadFileName.c_str()))
    auxBrowse["object"].value = String("<object data=\"") + uploadFileName + String("\"></object>");
  else
    auxBrowse["object"].value = "Not saved";
  return String();
}

/**
 * Read the given file from the filesystem and stream it back to the client
 */
void handleFileRead(void) {
  const String& filePath =
#if defined(ARDUINO_ARCH_ESP8266)
    ESP8266WebServer::urlDecode(server.uri());
#elif defined(ARDUINO_ARCH_ESP32)
    server.uri();
#endif
  if (FlashFS.exists(filePath.c_str())) {
    File  uploadedFile = FlashFS.open(filePath, "r");
    String  mime = 
#if defined(ARDUINO_ARCH_ESP8266)
      mime::getContentType(filePath);
#elif defined(ARDUINO_ARCH_ESP32)
      getContentType(filePath);
#endif
    server.streamFile(uploadedFile, mime);
    uploadedFile.close();
  }
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  // Start the filesystem
  FlashFS.begin(FORMAT_ON_FAIL);

  // Attach the custom web pages
  auxUpload.load(PAGE_UPLOAD);
  auxBrowse.load(PAGE_BROWSE);
  auxBrowse.on(postUpload);  
  portal.join({ auxUpload, auxBrowse });

  // The handleFileRead callback function provides an echo back of the
  // uploaded file to the client. You can include the uploaded file in
  // the response by embedding the object HTML tag in your custom web page.
  // The client browser will request to get the content according to
  // the link of the object tag, and the request can be caught by onNotFound handler.
  portal.onNotFound(handleFileRead);
  portal.begin();
}

void loop() {
  portal.handleClient();
}
