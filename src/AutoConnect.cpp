/**
 *  AutoConnect class implementation.
 *  @file   AutoConnect.cpp
 *  @author hieromon@gmail.com
 *  @version    0.9.9
 *  @date   2019-05-25
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
#define SET_HOSTNAME(x) do { WiFi.hostname(x); } while(0)
#elif defined(ARDUINO_ARCH_ESP32)
#define SOFT_RESET()  ESP.restart()
#define	SET_HOSTNAME(x)	do { WiFi.setHostname(x); } while(0)
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

void AutoConnect::_initialize(void) {
  _rfConnect = false;
  _rfReset = false;
  _responsePage = nullptr;
  _currentPageElement = nullptr;
  _menuTitle = _apConfig.title;
  _connectTimeout = AUTOCONNECT_TIMEOUT;
  _scanCount = 0;
  memset(&_credential, 0x00, sizeof(struct station_config));
#ifdef ARDUINO_ARCH_ESP32
  _disconnectEventId = -1;  // The member available for ESP32 only
#endif
  _aux.release();
  _auxUri = String("");
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
bool AutoConnect::begin(void) {
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
 *  @return true        Connection established, AutoConnect service started with WIFI_STA mode.
 *  @return false       Could not connected, Captive portal started with WIFI_AP_STA mode.
 */
bool AutoConnect::begin(const char* ssid, const char* passphrase, unsigned long timeout) {
  bool  cs;

  // Overwrite for the current timeout value.
  _connectTimeout = timeout;

  // Start WiFi connection with station mode.
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  delay(100);

  // Set host name
  if (_apConfig.hostName.length())
    SET_HOSTNAME(_apConfig.hostName.c_str());

  // Advance configuration for STA mode.
#ifdef AC_DEBUG
  String staip_s = _apConfig.staip.toString();
  String staGateway_s = _apConfig.staGateway.toString();
  String staNetmask_s = _apConfig.staNetmask.toString();
  String dns1_s = _apConfig.dns1.toString();
  String dns2_s = _apConfig.dns2.toString();
  AC_DBG("WiFi.config(IP=%s, Gateway=%s, Subnetmask=%s, DNS1=%s, DNS2=%s)\n", staip_s.c_str(), staGateway_s.c_str(), staNetmask_s.c_str(), dns1_s.c_str(), dns2_s.c_str());
#endif
  if (!WiFi.config(_apConfig.staip, _apConfig.staGateway, _apConfig.staNetmask, _apConfig.dns1, _apConfig.dns2)) {
    AC_DBG("failed\n");
    return false;
  }
#ifdef ARDUINO_ARCH_ESP8266
  AC_DBG("DHCP client(%s)\n", wifi_station_dhcpc_status() == DHCP_STOPPED ? "STOPPED" : "STARTED");
#endif

  // If the portal is requested promptly skip the first WiFi.begin and
  // immediately start the portal.
  if (_apConfig.immediateStart) {
    cs = false;
    _apConfig.autoReconnect = false;
    _apConfig.autoRise = true;
    AC_DBG("Start the portal immediately\n");
  }
  else {
    // Try to connect by STA immediately.
    if (ssid == nullptr && passphrase == nullptr)
      WiFi.begin();
    else {
      _disconnectWiFi(false);
      WiFi.begin(ssid, passphrase);
    }
    AC_DBG("WiFi.begin(%s%s%s)\n", ssid == nullptr ? "" : ssid, passphrase == nullptr ? "" : ",", passphrase == nullptr ? "" : passphrase);
    cs = _waitForConnect(_connectTimeout) == WL_CONNECTED;
  }

  // Reconnect with a valid credential as the autoReconnect option is enabled.
  if (!cs && _apConfig.autoReconnect && (ssid == nullptr && passphrase == nullptr)) {
    // Load a valid credential.
    if (_loadAvailCredential()) {
      // Try to reconnect with a stored credential.
      AC_DBG("autoReconnect loaded SSID:%s\n", reinterpret_cast<const char*>(_credential.ssid));
      const char* psk = strlen(reinterpret_cast<const char*>(_credential.password)) ? reinterpret_cast<const char*>(_credential.password) : nullptr;
      WiFi.begin(reinterpret_cast<const char*>(_credential.ssid), psk);
      AC_DBG("WiFi.begin(%s%s%s)\n", _credential.ssid, psk == nullptr ? "" : ",", psk == nullptr ? "" : psk);
      cs = _waitForConnect(_connectTimeout) == WL_CONNECTED;
    }
  }
  _currentHostIP = WiFi.localIP();

  // Rushing into the portal.
  if (!cs) {

    // The captive portal is effective at the autoRise is valid only.
    if (_apConfig.autoRise) {

      // Change WiFi working mode, Enable AP with STA
      WiFi.setAutoConnect(false);
      _disconnectWiFi(false);

      // Activate the AP mode with configured softAP and start the access point.
      WiFi.mode(WIFI_AP_STA);
      while (WiFi.getMode() != WIFI_AP_STA) {
        delay(1);
        yield();
      }

      // Connection unsuccessful, launch the captive portal.
#if defined(ARDUINO_ARCH_ESP8266)
      if (!(_apConfig.apip == IPAddress(0, 0, 0, 0) || _apConfig.gateway == IPAddress(0, 0, 0, 0) || _apConfig.netmask == IPAddress(0, 0, 0, 0))) {
        _config();
      }
#endif
      WiFi.softAP(_apConfig.apid.c_str(), _apConfig.psk.c_str(), _apConfig.channel, _apConfig.hidden);
      do {
        delay(100);
        yield();
      } while (WiFi.softAPIP() == IPAddress(0, 0, 0, 0));
#if defined(ARDUINO_ARCH_ESP32)
      if (!(_apConfig.apip == IPAddress(0, 0, 0, 0) || _apConfig.gateway == IPAddress(0, 0, 0, 0) || _apConfig.netmask == IPAddress(0, 0, 0, 0))) {
        _config();
      }
#endif
      if (_apConfig.apip != IPAddress(0, 0, 0, 0)) {
        do {
          delay(100);
          yield();
        } while (WiFi.softAPIP() != _apConfig.apip);
      }
      _currentHostIP = WiFi.softAPIP();
      AC_DBG("SoftAP %s/%s Ch(%d) IP:%s %s\n", _apConfig.apid.c_str(), _apConfig.psk.c_str(), _apConfig.channel, _currentHostIP.toString().c_str(), _apConfig.hidden ? "hidden" : "");

      // Fork to the exit routine that starts captive portal.
      cs = _onDetectExit ? _onDetectExit(_currentHostIP) : true;

      // Start Web server when TCP connection is enabled.
      _startWebServer();

      // Start captive portal without cancellation by DetectExit.
      if (cs) {
        // Prepare for redirecting captive portal detection.
        // Pass all URL requests to _captivePortal to disguise the captive portal.
        _startDNSServer();

        // Start the captive portal to make a new connection
        bool  hasTimeout = false;
        _portalAccessPeriod = millis();
        while (WiFi.status() != WL_CONNECTED && !_rfReset) {
          handleClient();
          // Force execution of queued processes.
          yield();
          // Check timeout
          if ((hasTimeout = _hasTimeout(_apConfig.portalTimeout))) {
            AC_DBG("CP timeout exceeded:%ld\n", millis() - _portalAccessPeriod);
            break;
          }
        }
        cs = WiFi.status() == WL_CONNECTED;

        // If WLAN successfully connected, release DNS server.
        if (cs) {
          _dnsServer->stop();
          _dnsServer.reset();
          AC_DBG("DNS server stopped\n");
        }
        // Captive portal staying time exceeds timeout,
        // Close the portal if an option for keeping the portal is false.
        else if (hasTimeout) {
          if (_apConfig.retainPortal) {
            _purgePages();
            AC_DBG("Maintain portal\n");
          }
          else {
            _stopPortal();
          }
        }
      }
    }
    else {
      AC_DBG("Suppress autoRise\n");
    }
  }

  // It doesn't matter the connection status for launching the Web server.
  if (!_responsePage)
    _startWebServer();

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
bool AutoConnect::_config(void) {
  bool  rc = WiFi.softAPConfig(_apConfig.apip, _apConfig.gateway, _apConfig.netmask);
  AC_DBG("SoftAP configure %s, %s, %s %s\n", _apConfig.apip.toString().c_str(), _apConfig.gateway.toString().c_str(), _apConfig.netmask.toString().c_str(), rc ? "" : "failed");
  return rc;
}

/**
 *  Put a user site's home URI.
 *  The URI specified by home is linked from "HOME" in the AutoConnect
 *  portal menu.
 *  @param  uri   A URI string of user site's home.
 */
void AutoConnect::home(const String& uri) {
  _apConfig.homeUri = uri;
}

/**
 *  Stops AutoConnect captive portal service.
 */
void AutoConnect::end(void) {
  if (_responsePage != nullptr) {
    _responsePage->~PageBuilder();
    delete _responsePage;
    _responsePage = nullptr;
  }
  if (_currentPageElement != nullptr) {
    _currentPageElement->~PageElement();
    _currentPageElement = nullptr;
  }

  _stopPortal();
  if (_webServer) {
    switch (_webServerAlloc) {
    case AC_WEBSERVER_HOSTED:
      if (_dnsServer) {
        _dnsServer->stop();
        _dnsServer.reset();
      }
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
WebServerClass& AutoConnect::host(void) {
  return  *_webServer;
}

/**
 *  Returns AutoConnectAux instance of specified.
 *  @param  uri  An uri string.
 *  @return A pointer of AutoConnectAux instance.
 */
AutoConnectAux* AutoConnect::aux(const String& uri) const {
  AutoConnectAux* aux_p = _aux.get();
  while (aux_p) {
    if (!strcmp(aux_p->uri(), uri.c_str()))
      break;
    aux_p = aux_p->_next.get();
  }
  return aux_p;
}

/**
 *  Append auxiliary pages made up with AutoConnectAux.
 *  @param  aux A reference to AutoConnectAux that made up
 *  the auxiliary page to be added.
 */
void AutoConnect::join(AutoConnectAux& aux) {
  if (_aux)
    _aux->_concat(aux);
  else
    _aux.reset(&aux);
  aux._join(*this);
  AC_DBG("%s on hands\n", aux.uri());
}

/**
 *  Append auxiliary pages made up with AutoConnectAux.
 *  @param  aux A vector of reference to AutoConnectAux that made up
 *  the auxiliary page to be added.
 */
void AutoConnect::join(AutoConnectAuxVT auxVector) {
  for (std::reference_wrapper<AutoConnectAux> aux : auxVector)
    join(aux.get());
}

/**
 *  Starts Web server for AutoConnect service.
 */
void AutoConnect::_startWebServer(void) {
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
  if (!_responsePage) {
    _responsePage = new PageBuilder();
    _responsePage->exitCanHandle(std::bind(&AutoConnect::_classifyHandle, this, std::placeholders::_1, std::placeholders::_2));
    _responsePage->onUpload(std::bind(&AutoConnect::_handleUpload, this, std::placeholders::_1, std::placeholders::_2));
    _responsePage->insert(*_webServer);

    _webServer->begin();
    AC_DBG("http server started\n");
  }
  else {
    AC_DBG("http server readied\n");
  }
}

/**
 *  Starts DNS server for Captive portal.
 */
void AutoConnect::_startDNSServer(void) {
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
void AutoConnect::handleClient(void) {
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
void AutoConnect::handleRequest(void) {
  // Handling processing requests to AutoConnect.
  if (_rfConnect) {
    // Leave from the AP currently.
    if (WiFi.status() == WL_CONNECTED)
      _disconnectWiFi(true);

    // An attempt to establish a new AP.
    int32_t ch = _connectCh == 0 ? _apConfig.channel : _connectCh;
    AC_DBG("Attempt:%s Ch(%d)\n", reinterpret_cast<const char*>(_credential.ssid), (int)ch);
    WiFi.begin(reinterpret_cast<const char*>(_credential.ssid), reinterpret_cast<const char*>(_credential.password), ch);
    if (_waitForConnect(_connectTimeout) == WL_CONNECTED) {
      if (WiFi.BSSID() != NULL) {
        memcpy(_credential.bssid, WiFi.BSSID(), sizeof(station_config::bssid));
        _currentHostIP = WiFi.localIP();
        _redirectURI = String(F(AUTOCONNECT_URI_SUCCESS));

        // Save current credential
        if (_apConfig.autoSave == AC_SAVECREDENTIAL_AUTO) {
          AutoConnectCredential credit(_apConfig.boundaryOffset);
          credit.save(&_credential);
          AC_DBG("%s credential saved\n", reinterpret_cast<const char*>(_credential.ssid));
        }

        // Ensures that keeps a connection with the current AP while the portal behaves.
        _setReconnect(AC_RECONNECT_SET);
      }
      else
        AC_DBG("%s has no BSSID, saving is unavailable\n", reinterpret_cast<const char*>(_credential.ssid));
    }
    else {
      _currentHostIP = WiFi.softAPIP();
      _redirectURI = String(F(AUTOCONNECT_URI_FAIL));
      _rsConnect = WiFi.status();
      _disconnectWiFi(false);
      while (WiFi.status() != WL_IDLE_STATUS && WiFi.status() != WL_DISCONNECTED) {
        delay(1);
        yield();
      }
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
//    _waitForEndTransmission();
    _stopPortal();
    _disconnectWiFi(false);
    while (WiFi.status() == WL_CONNECTED) {
      delay(10);
      yield();
    }
    AC_DBG("Disconnected\n");
    // Reset disconnection request
    _rfDisconnect = false;

    if (_apConfig.autoReset) {
      delay(1000);
      SOFT_RESET();
      delay(1000);
    }
  }
}

/**
 *  Register the exit routine for AutoConnectAux.
 *  @param  uri     Specify the URI of the AutoConnectAux page that
 *  registers the exit routine.
 *  @param  handler A handler function of the exit routine.
 *  @param  order   Specify an enumeration type of
 *  AutoConnectExitOrder_t for the call timing of the exit routine.
 *  @return true    An exit routine registered.
 *  @return false   AutoConnectAux page for the specified URI is not
 *  registered.
 */
bool AutoConnect::on(const String& uri, const AuxHandlerFunctionT handler, AutoConnectExitOrder_t order) {
  AutoConnectAux* aux = _aux.get();
  while (aux) {
    if (!strcmp(uri.c_str(), aux->uri())) {
      aux->on(handler, order);
      return true;
    }
    aux = aux->_next.get();
  }
  return false;
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
 *  @return true  A matched credential of BSSID was loaded.
 */
bool AutoConnect::_loadAvailCredential(void) {
  AutoConnectCredential credential(_apConfig.boundaryOffset);

  if (credential.entries() > 0) {
    // Scan the vicinity only when the saved credentials are existing.
    WiFi.scanDelete();
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
void AutoConnect::_stopPortal(void) {
  if (_dnsServer && _webServerAlloc == AC_WEBSERVER_HOSTED)
    _dnsServer->stop();

  if (_webServer) {
    _webServer->client().stop();
    delay(1000);
  }

  _setReconnect(AC_RECONNECT_RESET);
  WiFi.softAPdisconnect(false);
  AC_DBG("Portal stopped\n");
}

/**
 *  Redirect to captive portal if we got a request for another domain.
 *  Return true in that case so the page handler do not try to handle the request again.
 */
bool AutoConnect::_captivePortal(void) {
  String  hostHeader = _webServer->hostHeader();
  if (!_isIP(hostHeader) && (hostHeader != WiFi.localIP().toString()) && (!hostHeader.endsWith(F(".local")))) {
    AC_DBG("Detected application, %s, %s\n", hostHeader.c_str(), WiFi.localIP().toString().c_str());
    String location = String(F("http://")) + _webServer->client().localIP().toString() + String(AUTOCONNECT_URI);
    _webServer->sendHeader(String(F("Location")), location, true);
    _webServer->send(302, String(F("text/plain")), _emptyString);
    _webServer->client().flush();
    _webServer->client().stop();
    return true;
  }
  return false;
}

/**
 *  Check whether the stay-time in the captive portal has a timeout.
 *  If the station is connected, the time measurement will be reset.
 *  @param  timeout The time limit for keeping the captive portal.
 *  @return true    There is no connection from the station even the time limit exceeds.
 *  @return false   Connectionless duration has not exceeded yet.
 */
bool AutoConnect::_hasTimeout(unsigned long timeout) {
  uint8_t staNum;

  if (!_apConfig.portalTimeout)
    return false;

#if defined(ARDUINO_ARCH_ESP8266)
  staNum = 0;
  struct station_info* station = wifi_softap_get_station_info();
  while (station) {
    staNum++;
    station = STAILQ_NEXT(station, next);
  }
  wifi_softap_free_station_info();
#elif defined(ARDUINO_ARCH_ESP32)
  staNum = WiFi.softAPgetStationNum();
#endif
  if (staNum)
    _portalAccessPeriod = millis();

  return (millis() - _portalAccessPeriod > timeout) ? true : false;
}

/**
 *  A handler that redirects access to the captive portal to the connection
 *  configuration page.
 */
void AutoConnect::_handleNotFound(void) {
  if (!_captivePortal()) {
    if (_notFoundHandler) {
      _notFoundHandler();
    }
    else {
      PageElement page404(_PAGE_404, { { String(F("HEAD")), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1) } });
      String html = page404.build();
      _webServer->sendHeader(String(F("Cache-Control")), String(F("no-cache, no-store, must-revalidate")), true);
      _webServer->sendHeader(String(F("Pragma")), String(F("no-cache")));
      _webServer->sendHeader(String(F("Expires")), String("-1"));
      _webServer->sendHeader(String(F("Content-Length")), String(html.length()));
      _webServer->send(404, String(F("text/html")), html);
    }
  }
}

/**
 *  Reset the ESP8266 module.
 *  It is called from the PageBuilder of the disconnect page and indicates
 *  the request for disconnection. It will be into progress after handleClient.
 */
String AutoConnect::_induceReset(PageArgument& args) {
  AC_UNUSED(args);
  _rfReset = true;
  return String(F(AUTOCONNECT_BUTTONLABEL_RESET " in progress..."));
}

/**
 *  Disconnect from AP.
 *  It is called from the PageBuilder of the disconnect page and indicates
 *  the request for disconnection. It will be into progress after handleClient.
 */
String AutoConnect::_induceDisconnect(PageArgument& args) {
  AC_UNUSED(args);
  _rfDisconnect = true;
  return _emptyString;
}

/**
 *  Indicates a connection establishment request and returns a redirect
 *  response to the waiting for connection page. This is called from
 *  handling of the current request by PageBuilder triggered by handleClient().
 *  If "Credential" exists in POST parameter, it reads from EEPROM.
 *  @param  args  http request arguments.
 *  @return A redirect response including "Location:" header.
 */
String AutoConnect::_induceConnect(PageArgument& args) {
  // Retrieve credential from the post method content.
  if (args.hasArg(String(F(AUTOCONNECT_PARAMID_CRED)))) {
    // Read from EEPROM
    AutoConnectCredential credential(_apConfig.boundaryOffset);
    struct station_config entry;
    credential.load(args.arg(String(F(AUTOCONNECT_PARAMID_CRED))).c_str(), &entry);
    strncpy(reinterpret_cast<char*>(_credential.ssid), reinterpret_cast<const char*>(entry.ssid), sizeof(_credential.ssid));
    strncpy(reinterpret_cast<char*>(_credential.password), reinterpret_cast<const char*>(entry.password), sizeof(_credential.password));
    AC_DBG("Credential loaded:%s\n", _credential.ssid);
  }
  else {
    AC_DBG("Queried SSID:%s\n", args.arg(AUTOCONNECT_PARAMID_SSID).c_str());
    // Credential had by the post parameter.
    strncpy(reinterpret_cast<char*>(_credential.ssid), args.arg(String(F(AUTOCONNECT_PARAMID_SSID))).c_str(), sizeof(_credential.ssid));
    strncpy(reinterpret_cast<char*>(_credential.password), args.arg(String(F(AUTOCONNECT_PARAMID_PASS))).c_str(), sizeof(_credential.password));
  }

  // Determine the connection channel based on the scan result.
  _connectCh = 0;
  for (uint8_t nn = 0; nn < _scanCount; nn++) {
    String  ssid = WiFi.SSID(nn);
    if (!strcmp(ssid.c_str(), reinterpret_cast<const char*>(_credential.ssid))) {
      _connectCh = WiFi.channel(nn);
      break;
    }
  }

  // Turn on the trigger to start WiFi.begin().
  _rfConnect = true;

// Since v0.9.7, the redirect method changed from a 302 response to the
// meta tag with refresh attribute.
// This approach for ESP32 makes an inefficient choice. The waiting
// procedure for a connection attempt should be the server side. Also,
// the proper value of waiting time until refreshing is unknown. But
// AutoConnect cannot avoid the following error as affairs stand now
// that occurs at connection establishment.
// [WiFiClient.cpp:463] connected(): Disconnected: RES: 0, ERR: 128
// When connecting as a station, TCP reset caused by switching of the
// radio channel occurs. Although the Espressif's view is true. However,
// the actual TCP reset occurs not at the time of switching the channel.
// It occurs at the connection from the ESP32 to the AP is established
// and it is possible that TCP reset is occurring in other situations.
// So, it may not be the real cause. Client-origin redirects with HTML
// refresh depend on the behavior of the arduino-esp32 library. Thus,
// the implementations for redirects with HTML will continue until
// the arduino-esp32 core supports reconnection.

  // Redirect to waiting URI while executing connection request.
  // String url = String(F("http://")) + _webServer->client().localIP().toString() + String(AUTOCONNECT_URI_RESULT);
  // _webServer->sendHeader(F("Location"), url, true);
  // _webServer->send(302, F("text/plain"), "");
  // _webServer->client().flush();
  // _webServer->client().stop();
  // _responsePage->cancel();
  return _emptyString;
}

/**
 *  Responds response as redirect to the connection result page.
 *  A destination as _redirectURI is indicated by loop to establish connection.
 */
String AutoConnect::_invokeResult(PageArgument& args) {
  AC_UNUSED(args);
  String redirect = String(F("http://"));
  // The host address to which the connection result for ESP32 responds
  // changed from v0.9.7. This change is a measure according to the
  // implementation of the arduino-esp32 1.0.1.
#if defined(ARDUINO_ARCH_ESP32)
  // In ESP32, the station IP address just established could not be reached.
  redirect += _webServer->client().localIP().toString();
#elif defined(ARDUINO_ARCH_ESP8266)
  // In ESP8266, the host address that responds for the connection
  // successful is the IP address of ESP8266 as a station.
  // This is the specification as before.
  redirect += _currentHostIP.toString();
#endif
  redirect += _redirectURI;
  _webServer->sendHeader(String(F("Location")), redirect, true);
  _webServer->send(302, String(F("text/plain")), _emptyString);
  _webServer->client().flush();
  _webServer->client().stop();
  _waitForEndTransmission();  // Wait for response transmission complete
  _responsePage->cancel();
  return _emptyString;
}

/**
 *  Classify the requested URI to responsive page builder.
 *  There is always only one PageBuilder instance that can exist in
 *  AutoConnect for saving RAM. Invokes a subordinate function that
 *  dynamically generates a response page at handleRequest. This is
 *  a part of the handling of http request originated from handleClient.
 */
bool AutoConnect::_classifyHandle(HTTPMethod method, String uri) {
  AC_UNUSED(method);
  _portalAccessPeriod = millis();
  AC_DBG("Host:%s,URI:%s", _webServer->hostHeader().c_str(), uri.c_str());

  // Here, classify requested uri
  if (uri == _uri) {
    AC_DBG_DUMB(",already allocated\n");
    return true;  // The response page already exists.
  }

  // Dispose decrepit page
  if (_uri.length())
    _prevUri = _uri;   // Save current uri for the upload request
  _purgePages();

  // Create the page dynamically
  if ((_currentPageElement = _setupPage(uri)) == nullptr)
    if (_aux) {
      // Requested URL is not a normal page, exploring AUX pages
      _currentPageElement = _aux->_setupPage(uri);
    }
  if (_currentPageElement != nullptr) {
    AC_DBG_DUMB(",generated:%s", uri.c_str());
    _uri = uri;
    _responsePage->addElement(*_currentPageElement);
    _responsePage->setUri(_uri.c_str());
  }
  AC_DBG_DUMB(",%s\n", _currentPageElement != nullptr ? " allocated" : "ignored");
  return _currentPageElement != nullptr ? true : false;
}

/**
 *  A wrapper of the upload function for the WebServerClass. Invokes the
 *  upload function of the AutoConnectAux which has a destination URI.
 */
void AutoConnect::_handleUpload(const String& requestUri, const HTTPUpload& upload) {
  AutoConnectAux* aux = _aux.get();
  while (aux) {
    if (aux->_uriStr == requestUri) {
      aux->upload(_prevUri, upload);
      break;
    }
    aux = aux->_next.get();
  }
}

/**
 *  Purge allocated pages. 
 */
void AutoConnect::_purgePages(void) {
  _responsePage->clearElement();
  if (_currentPageElement != nullptr) {
    delete _currentPageElement;
    _currentPageElement = nullptr;
    _uri = String("");
  }
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
 *  @return MAC address string in XX:XX:XX:XX:XX:XX format.
 */
String AutoConnect::_toMACAddressString(const uint8_t mac[]) {
  String  macAddr = String("");
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
 *  @return A signal quality percentage.
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
 *  @return wl_status_t
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
    AC_DBG_DUMB("%c", '.');
    delay(300);
  }
  AC_DBG_DUMB("%s IP:%s\n", wifiStatus == WL_CONNECTED ? "established" : "time out", WiFi.localIP().toString().c_str());
  return wifiStatus;
}

/**
 *  Control the automatic reconnection behaves. Reconnection behavior
 *  to the AP connected during captive portal operation is activated
 *  by an order as the argument.
 *  @param  order  AC_RECONNECT_SET or AC_RECONNECT_RESET
 */
void AutoConnect::_setReconnect(const AC_STARECONNECT_t order) {
#if defined(ARDUINO_ARCH_ESP32)
  if (order == AC_RECONNECT_SET) {
    _disconnectEventId = WiFi.onEvent([](WiFiEvent_t e, WiFiEventInfo_t info) {
      AC_DBG("STA lost connection:%d\n", info.disconnected.reason);
      bool  rst = WiFi.reconnect();
      AC_DBG("STA connection %s\n", rst ? "restored" : "failed");
    }, WiFiEvent_t::SYSTEM_EVENT_AP_STADISCONNECTED);
    AC_DBG("Event<%d> handler registered\n", static_cast<int>(WiFiEvent_t::SYSTEM_EVENT_AP_STADISCONNECTED));
  }
  else if (order == AC_RECONNECT_RESET) {
    if (_disconnectEventId) {
      WiFi.removeEvent(_disconnectEventId);
      AC_DBG("Event<%d> handler released\n", static_cast<int>(WiFiEvent_t::SYSTEM_EVENT_AP_STADISCONNECTED));
    }
  }
#elif defined(ARDUINO_ARCH_ESP8266)
  bool  strc = order == AC_RECONNECT_SET ? true : false;
  WiFi.setAutoReconnect(strc);
  AC_DBG("STA reconnection:%s\n", strc ? "EN" : "DIS");
#endif
}

/**
 * Wait for the end of transmission of the http response by closed
 * from the http client. 
 */
void AutoConnect::_waitForEndTransmission(void) {
#ifdef AC_DEBUG
  AC_DBG("Leaves:");
  unsigned long lt = millis();
#endif
  while (_webServer->client().connected()) {
    delay(1);
    yield();
  }
#ifdef AC_DEBUG
  // Notifies of the time taken to end the session. If the http client 
  // times out, AC_DEBUG must be enabled and it is necessary to confirm
  // that the http response is being transmitted correctly.
  // To trace the correctness the close sequence of TCP connection,
  // enable the debug log of the Arduino core side. If the normal,
  // a message of closed the TCP connection will be logged between
  // "Leaves:" and "the time taken to end the session" of the log.
  AC_DBG_DUMB("%d[ms]\n", static_cast<int>(millis() - lt));
#endif
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
  while (WiFi.status() == WL_CONNECTED) {
    delay(1);
    yield();
  }
}

/**
 *  Initialize an empty string to allow returning const String& with nothing.
 */
const String AutoConnect::_emptyString = String("");
