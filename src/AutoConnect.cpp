/**
 *  AutoConnect class implementation.
 *  @file   AutoConnect.cpp
 *  @author hieromon@gmail.com
 *  @version    0.9.5
 *  @date   2018-08-27
 *  @copyright  MIT license.
 */

#include "AutoConnect.h"
#ifdef ARDUINO_ARCH_ESP32
#include <esp_wifi.h>
#endif

/**
 *  An actual reset function dependent on the architecture
 */
#if defined(ARDUINO_ARCH_ESP8266)
#define SOFT_RESET()  ESP.reset()
#elif defined(ARDUINO_ARCH_ESP32)
#define SOFT_RESET()  ESP.restart()
#endif

/**
 *  AutoConnect default constructor. This entry activates WebServer
 *  internally and the web server is allocated internal.
 */
AutoConnect::AutoConnect() {
  _initialize();
  _webServer.reset(nullptr);
  _dnsServer.reset(nullptr);
  _webServerAlloc = AC_WEBSERVER_HOSTED;
}

/**
 *  Run the AutoConnect site using the externally ensured ESP 8266 WebServer.
 *  User's added URI handler response can be included in handleClient method.
 *  @param  webServer   A reference of ESP8266WebServer instance.
 */
AutoConnect::AutoConnect(WebServerClass& webServer) {
  _initialize();
  _webServer.reset(&webServer);
  _dnsServer.reset(nullptr);
  _webServerAlloc = AC_WEBSERVER_PARASITIC;
}

void AutoConnect::_initialize() {
  _rfConnect = false;
  _rfReset = false;
  _currentPageElement = nullptr;
  _menuTitle = String(AUTOCONNECT_MENU_TITLE);
  _portalTimeout = AUTOCONNECT_TIMEOUT;
  memset(&_credential, 0x00, sizeof(struct station_config));
}

/**
 *  A destructor. Free AutoConnect web pages and release Web server.
 *  When the server is hosted it will be purged.
 */
AutoConnect::~AutoConnect() {
  end();
}

/**
 *  Starts establishing WiFi connection without SSID and password.
 */
bool AutoConnect::begin() {
  return begin(nullptr, nullptr);
}

/**
 *  Starts establishing WiFi connection.
 *  Before establishing, start the Web server and DNS server for the captive
 *  portal. Then begins connection establishment in WIFI_STA mode. If
 *  connection can not established with the specified SSID and password,
 *  switch to WIFI_AP_STA mode and activate SoftAP.
 *  @param  ssid        SSID to be connected.
 *  @param  passphrase  Password for connection.
 *  @param  timeout     A time out value in milliseconds for waiting connection.
 *  @retval true        Connection established, AutoConnect service started with WIFI_STA mode.
 *  @retval false       Could not connected, Captive portal started with WIFI_AP_STA mode.
 */
bool AutoConnect::begin(const char* ssid, const char* passphrase, unsigned long timeout) {
  bool  cs;

  // Overwrite for the current timeout value.
  _portalTimeout = timeout;

  // Start WiFi connection.
  WiFi.mode(WIFI_STA);
  delay(100);

  // Advance configuration for STA mode.
#ifdef AC_DEBUG
  String staip_s = _apConfig.staip.toString();
  String staGateway_s = _apConfig.staGateway.toString();
  String staNetmask_s = _apConfig.staNetmask.toString();
  String dns1_s = _apConfig.dns1.toString();
  String dns2_s = _apConfig.dns2.toString();
  AC_DBG("WiFi.config(IP=%s, Gateway=%s, Subnetmask=%s, DNS1=%s, DNS2=%s) ", staip_s.c_str(), staGateway_s.c_str(), staNetmask_s.c_str(), dns1_s.c_str(), dns2_s.c_str());
#endif
  if (!WiFi.config(_apConfig.staip, _apConfig.staGateway, _apConfig.staNetmask, _apConfig.dns1, _apConfig.dns2)) {
    AC_DBG("failed\n");
    return false;
  }
#ifdef ARDUINO_ARCH_ESP8266
  AC_DBG("DHCP client(%s)\n", wifi_station_dhcpc_status() == DHCP_STOPPED ? "STOPPED" : "STARTED");
#endif

  // Try to connect by STA immediately.
  if (ssid == nullptr && passphrase == nullptr)
    WiFi.begin();
  else
    WiFi.begin(ssid, passphrase);
  AC_DBG("WiFi.begin(%s%s%s)\n", ssid == nullptr ? "" : ssid, passphrase == nullptr ? "" : ",", passphrase == nullptr ? "" : passphrase);
  cs = _waitForConnect(_portalTimeout) == WL_CONNECTED;

  // Reconnect with a valid credential as the autoReconnect option is enabled.
  if (!cs && _apConfig.autoReconnect && (ssid == nullptr && passphrase == nullptr)) {
    // Load a valid credential.
    if (_loadAvailCredential()) {
      // Try to reconnect with a stored credential.
      AC_DBG("Past SSID:%s loaded\n", reinterpret_cast<const char*>(_credential.ssid));
      const char* psk = strlen(reinterpret_cast<const char*>(_credential.password)) ? reinterpret_cast<const char*>(_credential.password) : nullptr;
      WiFi.begin(reinterpret_cast<const char*>(_credential.ssid), psk);
      AC_DBG("WiFi.begin(%s%s%s)\n", _credential.ssid, psk == nullptr ? "" : ",", psk == nullptr ? "" : psk);
      cs = _waitForConnect(_portalTimeout) == WL_CONNECTED;
    }
  }
  _currentHostIP = WiFi.localIP();

  // It doesn't matter the connection status for launching the Web server.
  _startWebServer();

  // Rushing into the portal.
  if (!cs) {

    // The captive portal is effective at the autoRise is valid only.
    if (_apConfig.autoRise) {

      // Change WiFi working mode, Enable AP with STA
      WiFi.setAutoConnect(false);
      _disconnectWiFi(true);
      WiFi.mode(WIFI_AP_STA);
      delay(100);

      // Connection unsuccessful, launch the captive portal.
      if (!(_apConfig.apip == IPAddress(0, 0, 0, 0) || _apConfig.gateway == IPAddress(0, 0, 0, 0) || _apConfig.netmask == IPAddress(0, 0, 0, 0))) {
        _config();
      }
      WiFi.softAP(_apConfig.apid.c_str(), _apConfig.psk.c_str(), _apConfig.channel, _apConfig.hidden);
      while (WiFi.softAPIP() == IPAddress(0, 0, 0, 0))
        yield();
      _currentHostIP = WiFi.softAPIP();
      AC_DBG("SoftAP %s/%s CH(%d) H(%d) IP:%s\n", _apConfig.apid.c_str(), _apConfig.psk.c_str(), _apConfig.channel, _apConfig.hidden, WiFi.softAPIP().toString().c_str());

      // Fork to the exit routine that starts captive portal.
      cs = _onDetectExit ? _onDetectExit(_currentHostIP) : true;

      // Start captive portal without cancellation by DetectExit.
      if (cs) {
        // Prepare for redirecting captive portal detection.
        // Pass all URL requests to _captivePortal to disguise the captive portal.
        _startDNSServer();

        // Start the captive portal to make a new connection
        while (WiFi.status() != WL_CONNECTED && !_rfReset) {
          handleClient();
          // Force execution of queued processes.
          yield();
        }
        cs = WiFi.status() == WL_CONNECTED;

        // If WLAN successfully connected, release DNS server.
        if (cs) {
          _dnsServer->stop();
          _dnsServer.reset();
        }
      }
    }
  }
  return cs;
}

/**
 *  Configure AutoConnect portal access point.
 *  @param  ap      SSID for access point.
 *  @param  psk     Password for access point.
 */
bool AutoConnect::config(const char* ap, const char* password) {
  _apConfig.apid = String(ap);
  _apConfig.psk = String(password);
  return _config();
}

/**
 *  Configure AutoConnect portal access point.
 *  @param  Config  AutoConnectConfig class instance.
 */
bool AutoConnect::config(AutoConnectConfig& Config) {
  _apConfig = Config;
  return _config();
}

/**
 *  Configure access point.
 *  Set up access point with internal AucoConnectConfig parameter corrected
 *  by Config method.
 */
bool AutoConnect::_config() {
  return WiFi.softAPConfig(_apConfig.apip, _apConfig.gateway, _apConfig.netmask);
}

/**
 *  Put a user site's home URI.
 *  The URI specified by home is linked from "HOME" in the AutoConnect
 *  portal menu.
 *  @param  uri   A URI string of user site's home.
 */
void AutoConnect::home(String uri) {
  _apConfig.homeUri = uri;
}

/**
 *  Stops AutoConnect captive portal service.
 */
void AutoConnect::end() {
  if (_responsePage != nullptr) {
    _responsePage->~PageBuilder();
    delete _responsePage;
  }
  if (_currentPageElement != nullptr)
    _currentPageElement->~PageElement();

  _stopPortal();
  if (_webServer) {
    switch (_webServerAlloc) {
    case AC_WEBSERVER_HOSTED:
      if (_dnsServer)
        _dnsServer.reset();
      _webServer.reset();
      break;
    case AC_WEBSERVER_PARASITIC:
      _webServer.release();
      break;
    }
  }
}

/**
 *  Returns the current hosted ESP8266WebServer.
 */
WebServerClass& AutoConnect::host() {
  return  *_webServer;
}

/**
 *  Starts Web server for AutoConnect service.
 */
void AutoConnect::_startWebServer() {
  // Boot Web server
  if (!_webServer) {
    // Only when hosting WebServer internally
    _webServer.reset(new WebServerClass(AUTOCONNECT_HTTPPORT));
    _webServerAlloc = AC_WEBSERVER_HOSTED;
    AC_DBG("WebServer allocated\n");
  }
  // Discard the original the not found handler to redirect captive portal detection.
  // It is supposed to evacuate but ESP8266WebServer::_notFoundHandler is not accessible.
  _webServer->onNotFound(std::bind(&AutoConnect::_handleNotFound, this));
  // here, Prepare PageBuilders for captive portal
  _responsePage = new PageBuilder();
  _responsePage->exitCanHandle(std::bind(&AutoConnect::_classifyHandle, this, std::placeholders::_1, std::placeholders::_2));
  _responsePage->insert(*_webServer);

  _webServer->begin();
  AC_DBG("http server started\n");
}

/**
 *  Starts DNS server for Captive portal.
 */
void AutoConnect::_startDNSServer() {
  // Boot DNS server, set up for captive portal redirection.
  if (!_dnsServer) {
    _dnsServer.reset(new DNSServer());
    _dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    _dnsServer->start(AUTOCONNECT_DNSPORT, "*", WiFi.softAPIP());
    AC_DBG("DNS server started\n");
  }
}

/**
 *  Handling for the AutoConnect web interface.
 *  Invoke the handleClient of parent web server to process client request of
 *  AutoConnect WEB interface.
 *  No effects when the web server is not available.
 */
void AutoConnect::handleClient() {
  // Is there DNS Server process next request?
  if (_dnsServer)
    _dnsServer->processNextRequest();
  // handleClient valid only at _webServer activated.
  if (_webServer)
    _webServer->handleClient();

  handleRequest();
}

/**
 *  Handling for the AutoConnect menu request.
 */
void AutoConnect::handleRequest() {
  // Handling processing requests to AutoConnect.
  if (_rfConnect) {
    // Leave from the AP currently.
    if (WiFi.status() == WL_CONNECTED)
      _disconnectWiFi(true);

    // An attempt to establish a new AP.
    AC_DBG("Request for %s\n", (const char*)_credential.ssid);
    WiFi.begin((const char*)_credential.ssid, (const char*)_credential.password);
    if (_waitForConnect(_portalTimeout) == WL_CONNECTED) {
      if (WiFi.BSSID() != NULL) {
        memcpy(_credential.bssid, WiFi.BSSID(), sizeof(station_config::bssid));
        _currentHostIP = WiFi.localIP();
        _redirectURI = String(AUTOCONNECT_URI_SUCCESS);

        // Save current credential
        if (_apConfig.autoSave == AC_SAVECREDENTIAL_AUTO) {
          AutoConnectCredential credit(_apConfig.boundaryOffset);
          credit.save(&_credential);
          AC_DBG("%s credential saved\n", _credential.ssid);
        }
      }
      else
        AC_DBG("%s has no BSSID, saving is unavailable\n", _credential.ssid);
    }
    else {
      _currentHostIP = WiFi.softAPIP();
      _redirectURI = String(AUTOCONNECT_URI_FAIL);
    }
    _rfConnect = false;
  }

  if (_rfReset) {
    // Reset or disconnect by portal operation result
    _stopPortal();
    AC_DBG("Reset\n");
    delay(1000);
    SOFT_RESET();
    delay(1000);
  }

  if (_rfDisconnect) {
    // Disconnect from the current AP.
    _stopPortal();
    _disconnectWiFi(true);
    AC_DBG("Disconnected\n");
    // Reset disconnection request, restore the menu title.
    _rfDisconnect = false;
    _menuTitle = String(AUTOCONNECT_MENU_TITLE);

    if (_apConfig.autoReset) {
      delay(1000);
      SOFT_RESET();
      delay(1000);
    }
  }
}

/**
 *  Register the exit routine for the starting captive portal.
 *  @param  fn  A function of the exit routine.
 */
void AutoConnect::onDetect(DetectExit_ft fn) {
  _onDetectExit = fn;
}

/**
 *  Register the handler function for undefined url request detected.
 *  @param  fn  A function of the not found handler.
 */
void AutoConnect::onNotFound(WebServerClass::THandlerFunction fn) {
  _notFoundHandler = fn;
}

/**
 *  Load stored credentials that match nearby WLANs.
 *  @retval true  A matched credential of BSSID was loaded.
 */
bool AutoConnect::_loadAvailCredential() {
  AutoConnectCredential credential(_apConfig.boundaryOffset);

  if (credential.entries() > 0) {
    // Scan the vicinity only when the saved credentials are existing.
    int8_t  nn = WiFi.scanNetworks(false, true);
    AC_DBG("%d network(s) found\n", (int)nn);
    if (nn > 0) {
      // Determine valid credentials by BSSID.
      for (uint8_t i = 0; i < credential.entries(); i++) {
        credential.load(i, &_credential);
        for (uint8_t n = 0; n < nn; n++) {
          if (!memcmp(_credential.bssid, WiFi.BSSID(n), sizeof(station_config::bssid)))
            return true;
        }
      }
    }
  }
  return false;
}

/**
 *  Disconnect from the AP and stop the AutoConnect portal.
 *  Stops DNS server and flush tcp sending.
 */
void AutoConnect::_stopPortal() {
  if (_dnsServer && _webServerAlloc == AC_WEBSERVER_HOSTED)
    _dnsServer->stop();

  if (_webServer) {
    _webServer->client().stop();
    delay(1000);
  }

  WiFi.softAPdisconnect(false);
  AC_DBG("SoftAP stopped\n");
}

/**
 *  Redirect to captive portal if we got a request for another domain.
 *  Return true in that case so the page handler do not try to handle the request again.
 */
bool AutoConnect::_captivePortal() {
  String  hostHeader = _webServer->hostHeader();
  if (!_isIP(hostHeader) && (hostHeader != WiFi.localIP().toString())) {
    String location = String("http://") + _webServer->client().localIP().toString() + String(AUTOCONNECT_URI);
    _webServer->sendHeader("Location", location, true);
    _webServer->send(302, "text/plain", "");
    _webServer->client().flush();
    _webServer->client().stop();
    return true;
  }
  return false;
}

/**
 *  A handler that redirects access to the captive portal to the connection
 *  configuration page.
 */
void AutoConnect::_handleNotFound() {
  if (!_captivePortal()) {
    if (_notFoundHandler) {
      _notFoundHandler();
    }
    else {
      PageElement page404(_PAGE_404, { { "HEAD", std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1) } });
      String html = page404.build();
      _webServer->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate", true);
      _webServer->sendHeader("Pragma", "no-cache");
      _webServer->sendHeader("Expires", "-1");
      _webServer->sendHeader("Content-Length", String(html.length()));
      _webServer->send(404, "text/html", html);
    }
  }
}

/**
 *  Reset the ESP8266 module.
 *  It is called from the PageBuilder of the disconnect page and indicates
 *  the request for disconnection. It will be into progress after handleClient.
 */
String AutoConnect::_induceReset(PageArgument& args) {
  _rfReset = true;
  return String("Reset in progress...");
}

/**
 *  Disconnect from AP.
 *  It is called from the PageBuilder of the disconnect page and indicates
 *  the request for disconnection. It will be into progress after handleClient.
 */
String AutoConnect::_induceDisconnect(PageArgument& args) {
  _rfDisconnect = true;
  _menuTitle = String("Disconnect");
  return "";
}

/**
 *  Indicates a connection establishment request and returns a redirect
 *  response to the waiting for connection page. This is called from
 *  handling of the current request by PageBuilder triggered by handleClient().
 *  If "Credential" exists in POST parameter, it reads from EEPROM.
 *  @param  args  http request arguments.
 *  @retval A redirect response including "Location:" header.
 */
String AutoConnect::_induceConnect(PageArgument& args) {
  // Retrieve credential from the post method content.
  if (args.hasArg(AUTOCONNECT_PARAMID_CRED)) {
    // Read from EEPROM
    AutoConnectCredential credential(_apConfig.boundaryOffset);
    struct station_config entry;
    AC_DBG("Load credential:%s\n", args.arg(AUTOCONNECT_PARAMID_CRED).c_str());
    credential.load(args.arg(AUTOCONNECT_PARAMID_CRED).c_str(), &entry);
    strncpy(reinterpret_cast<char*>(_credential.ssid), reinterpret_cast<const char*>(entry.ssid), sizeof(_credential.ssid));
    strncpy(reinterpret_cast<char*>(_credential.password), reinterpret_cast<const char*>(entry.password), sizeof(_credential.password));
  }
  else {
    // Credential had by the post parameter.
    strncpy(reinterpret_cast<char*>(_credential.ssid), args.arg(AUTOCONNECT_PARAMID_SSID).c_str(), sizeof(_credential.ssid));
    strncpy(reinterpret_cast<char*>(_credential.password), args.arg(AUTOCONNECT_PARAMID_PASS).c_str(), sizeof(_credential.password));
  }

  // Turn on the trigger to start WiFi.begin().
  _rfConnect = true;

  // Redirect to waiting URI while executing connection request.
  //_webServer->sendHeader("Location", String("http://") + _webServer->client().localIP().toString() + String(AUTOCONNECT_URI_RESULT), true);
  String url = String("http://") + _webServer->client().localIP().toString() + String(AUTOCONNECT_URI_RESULT);
  _webServer->sendHeader("Location", url, true);
  //_webServer->sendHeader("Connection", "keep-alive");
  _webServer->send(302, "text/plain", "");
  _webServer->client().flush();
  _webServer->client().stop();
  _responsePage->cancel();

  return "";
}

/**
 *  Responds response as redirect to the connection result page.
 *  A destination as _redirectURI is indicated by loop to establish connection.
 */
String AutoConnect::_invokeResult(PageArgument& args) {
  String redirect = String("http://") + _currentHostIP.toString() + _redirectURI;
  _webServer->sendHeader("Location", redirect, true);
  _webServer->send(302, "text/plain", "");
  _webServer->client().flush();
  _webServer->client().stop();
  _responsePage->cancel();
  return "";
}

/**
 *  Classify the requested URI to responsive page builder.
 *  There is always only one PageBuilder instance that can exist in
 *  AutoConnect for saving RAM. Invokes a subordinate function that
 *  dynamically generates a response page at handleRequest. This is
 *  a part of the handling of http request originated from handleClient.
 */
bool AutoConnect::_classifyHandle(HTTPMethod method, String uri) {
  AC_DBG("%s%s\n", _webServer->hostHeader().c_str(), uri.c_str());
  if (uri == _uri) {
    return true;  // The response page already exists.
  }

  // Dispose decrepit page
  _responsePage->clearElement();
  if (_currentPageElement != nullptr)
    delete _currentPageElement;
  _uri = String();

  // Create the page dynamically
  if ((_currentPageElement = _setupPage(uri)) != nullptr) {
    _uri = String(uri);
    _responsePage->addElement(*_currentPageElement);
  }
  _responsePage->setUri(_uri.c_str());
  AC_DBG("Page[%s] allocated\n", _responsePage->uri());

  return _currentPageElement != nullptr ? true : false;
}

/**
 *  It checks whether the specified character string is a valid IP address.
 *  @param  ipStr   IP string for validation.
 *  @return true    Valid.
 */
bool AutoConnect::_isIP(String ipStr) {
  for (uint8_t i = 0; i < ipStr.length(); i++) {
    char c = ipStr.charAt(i);
    if (c != '.' && (c < '0' || c > '9'))
      return false;
  }
  return true;
}

/**
 *  Convert MAC address in uint8_t array to Sting XX:XX:XX:XX:XX:XX format.
 *  @param  mac   Array of MAC address 6 bytes.
 *  @retval MAC address string in XX:XX:XX:XX:XX:XX format.
 */
String AutoConnect::_toMACAddressString(const uint8_t mac[]) {
  String  macAddr = "";
  for (uint8_t i = 0; i < 6; i++) {
    char buf[3];
    sprintf(buf, "%02X", mac[i]);
    macAddr += buf;
    if (i < 5)
      macAddr += ':';
  }
  return macAddr;
}

/**
 *  Convert dBm to the wifi signal quality.
 *  @param  rssi  dBm.
 *  @retval a signal quality percentage.
 */
unsigned int AutoConnect::_toWiFiQuality(int32_t rssi) {
  unsigned int  qu;
  if (rssi == 31)   // WiFi signal is weak and RSSI value is unreliable.
    qu = 0;
  else if (rssi <= -100)
    qu = 0;
  else if (rssi >= -50)
    qu = 100;
  else
    qu = 2 * (rssi + 100);
  return qu;
}

/**
 *  Wait for establishment of the connection until the specified time expires.
 *  @param  timeout Expiration time by millisecond unit.
 *  @retval wl_status_t
 */
wl_status_t AutoConnect::_waitForConnect(unsigned long timeout) {
  wl_status_t wifiStatus;

  AC_DBG("Connecting");
  unsigned long st = millis();
  while ((wifiStatus = WiFi.status()) != WL_CONNECTED) {
    if (timeout) {
      if (millis() - st > timeout)
        break;
    }
#ifdef AC_DEBUG
    AC_DEBUG_PORT.print('.');
#endif // AC_DEBUG
    delay(300);
  }
  AC_DBG("%s IP:%s\n", wifiStatus == WL_CONNECTED ? "established" : "time out", WiFi.localIP().toString().c_str());
  return wifiStatus;
}

/**
 *  Disconnects the station from an associated access point.
 *  @param  wifiOff The station mode turning switch.
 */
void AutoConnect::_disconnectWiFi(bool wifiOff) {
#if defined(ARDUINO_ARCH_ESP8266)
  WiFi.disconnect(wifiOff);
#elif defined(ARDUINO_ARCH_ESP32)
  WiFi.disconnect(wifiOff, true);
#endif
  while (WiFi.status() == WL_CONNECTED)
    delay(100);
}