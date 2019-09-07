/**
 * Ported the ESP8266HTTPUpdateServer published by Arduino-core to
 * provide the web browser based OTA update on the ESP32 platform.
 * @file HTTPUpdateServer.cpp
 * @author hieromon@gmail.com
 * @version  0.9.10
 * @date 2019-06-06
 * @copyright  MIT license.
 */

#ifdef ARDUINO_ARCH_ESP32
// This class will available only EPS32 actually.

#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WebServer.h>
#include <WiFiUdp.h>
#include <Update.h>
#include "StreamString.h"
#include "HTTPUpdateServer.h"

static const char serverIndex[] PROGMEM = R"(
<html><body>
  <form method='POST' action='' enctype='multipart/form-data'>
    <input type='file' name='update'>
    <input type='submit' value='Update'>
  </form>
</body></html>
)";

static const char successResponse[] PROGMEM = 
"<meta http-equiv=\"refresh\" content=\"15;URL=/\">Update Success! Rebooting...\n";

/**
 * Setup for the web update. Register the authentication procedure and
 * binary file upload handler required to update the actual sketch binary by OTA.
 * @param  server    A pointer to the WebServer instance
 * @param  path      URI of the update handler
 * @param  username  Username for authentication
 * @arama  password  Password for authentication
 */
void HTTPUpdateServer::setup(WebServer* server, const String& path, const String& username, const String& password) {
  _server = server;
  _username = username;
  _password = password;

  // handler for the /update form page
  _server->on(path.c_str(), HTTP_GET, [&] () {
    if (_username != _emptyString && _password != _emptyString && !_server->authenticate(_username.c_str(), _password.c_str()))
      return _server->requestAuthentication();
    _server->send_P(200, PSTR("text/html"), serverIndex);
  });

  // handler for the /update form POST (once file upload finishes)
  _server->on(path.c_str(), HTTP_POST, [&] () {
    if(!_authenticated)
      return _server->requestAuthentication();
    if (Update.hasError()) {
      _server->send(200, F("text/html"), String(F("Update error: ")) + _updaterError);
    }
    else {
      _server->client().setNoDelay(true);
      _server->send_P(200, PSTR("text/html"), successResponse);
      delay(100);
      _server->client().stop();
      ESP.restart();
    }
  }, [&] () {
    // handler for the file upload, get's the sketch bytes, and writes
    // them through the Update object
    HTTPUpload& upload = _server->upload();

    if (upload.status == UPLOAD_FILE_START) {
      _updaterError = String();
      if (_serial_output)
        Serial.setDebugOutput(true);

      _authenticated = (_username == _emptyString || _password == _emptyString || _server->authenticate(_username.c_str(), _password.c_str()));
      if (!_authenticated) {
        if (_serial_output)
          Serial.println("Unauthenticated Update");
        return;
      }

      if (_serial_output)
        Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) {  //start with max available size
        _setUpdaterError();
      }
    }
    else if (_authenticated && upload.status == UPLOAD_FILE_WRITE && !_updaterError.length()) {
      if (_serial_output)
        Serial.print('.');
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
        _setUpdaterError();
    }
    else if (_authenticated && upload.status == UPLOAD_FILE_END && !_updaterError.length()) {
      if (Update.end(true)) { //true to set the size to the current progress
        if (_serial_output)
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      }
      else {
        _setUpdaterError();
      }
      if (_serial_output)
        Serial.setDebugOutput(false);
    }
    else if (_authenticated && upload.status == UPLOAD_FILE_ABORTED) {
      Update.end();
      if (_serial_output)
        Serial.println("Update was aborted");
    }
    delay(0);
  });
}

/**
 * Convert the update error code to string for notation.
 */
void HTTPUpdateServer::_setUpdaterError() {
  if (_serial_output)
    Update.printError(Serial);
  StreamString  str;
  Update.printError(str);
  _updaterError = str.c_str();
}

/**
 * Shared empty String instance
 */
const String HTTPUpdateServer::_emptyString;

#endif // !ARDUINO_ARCH_ESP32
