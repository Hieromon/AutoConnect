/**
 * Ported the ESP8266HTTPUpdateServer published by Arduino-core to
 * provide the web browser based OTA update on the ESP32 platform.
 * @file HTTPUpdateServer.h
 * @author hieromon@gmail.com
 * @version  0.9.10
 * @date 2019-06-06
 * @copyright  MIT license.
 */

#ifndef __HTTP_UPDATE_SERVER_H
#define __HTTP_UPDATE_SERVER_H

#ifdef ARDUINO_ARCH_ESP32
// This class will available only EPS32 actually.

class WebServer;

class HTTPUpdateServer {
 public:
  explicit HTTPUpdateServer(bool serial_debug = false) : _serial_output(serial_debug), _server(nullptr), _username(_emptyString), _password(_emptyString), _authenticated(false) {}
  ~HTTPUpdateServer() {}
  void  setup(WebServer* server) { setup(server, _emptyString, _emptyString); }
  void  setup(WebServer* server, const String& path) { setup(server, path, _emptyString, _emptyString); }
  void  setup(WebServer* server, const String& username, const String& password) { setup(server, "/update", username, password); }
  void  setup(WebServer* server, const String& path, const String& username, const String& password);
  void  updateCredentials(const String& username, const String& password) { _username = username; _password = password; }

 protected:
  void  _setUpdaterError();

 private:
  bool    _serial_output;
  WebServer*  _server;
  String  _username;
  String  _password;
  bool    _authenticated;
  String  _updaterError;
  static const String _emptyString;
};

#endif // !ARDUINO_ARCH_ESP32
#endif // !__HTTP_UPDATE_SERVER_H
