/**
 *  AutoConnect class implementation.
 *  @file   AutoConnect.cpp
 *  @author hieromon@gmail.com
 *  @version    1.2.0
 *  @date   2020-11-15
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
AutoConnect::AutoConnect()
: _scanCount( 0 )
, _menuTitle( _apConfig.title )
{
  memset(&_credential, 0x00, sizeof(station_config_t));
}

/**
 *  Run the AutoConnect site using the externally ensured ESP 8266 WebServer.
 *  User's added URI handler response can be included in handleClient method.
 *  @param  webServer   A reference of ESP8266WebServer instance.
 */
AutoConnect::AutoConnect(WebServerClass& webServer)
: AutoConnect()
{
  _webServer = WebserverUP(&webServer, [](WebServerClass*){});
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
  return begin(nullptr, nullptr, _apConfig.beginTimeout);
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
  if (timeout == 0)
    timeout = _apConfig.beginTimeout;

  if (_apConfig.preserveAPMode && !_apConfig.autoRise) {
    // Captive portal will not be started on connection failure. Enable Station mode
    // without disabling any current soft AP.
    cs = WiFi.enableSTA(true);
    AC_DBG("WiFi mode %d maintained, STA %s\n", WiFi.getMode(), cs ? "enabled" : "unavailable");
  }
  else {
    // Start WiFi connection with station mode.
    WiFi.softAPdisconnect(true);
    if (!WiFi.mode(WIFI_STA))
      AC_DBG("Unable start WIFI_STA\n");
    delay(100);
  }

  // Set host name
  if (_apConfig.hostName.length())
    SET_HOSTNAME(_apConfig.hostName.c_str());

  // Start Ticker according to the WiFi condition with Ticker is available.
  if (_apConfig.ticker) {
    _ticker.reset(new AutoConnectTicker(_apConfig.tickerPort, _apConfig.tickerOn));
    if (WiFi.status() != WL_CONNECTED)
      _ticker->start(AUTOCONNECT_FLICKER_PERIODDC, (uint8_t)AUTOCONNECT_FLICKER_WIDTHDC);
  }

  // If the portal is requested promptly skip the first WiFi.begin and
  // immediately start the portal.
  if (_apConfig.immediateStart) {
    cs = false;
    AC_DBG("Start the portal immediately\n");
  }
  else {
    station_config_t  current;
    // Restore current STA configuration
    if (_getConfigSTA(&current))
      AC_DBG("Current:%.32s\n", current.ssid);

    // Prepare valid configuration according to the WiFi connection right order.
    cs = true;
    _rfAdHocBegin = ssid == nullptr ? false : (strlen(ssid) > 0);
    if (_rfAdHocBegin) {
      // Save for autoReconnect
      strcpy(reinterpret_cast<char*>(_credential.ssid), ssid);
      if (passphrase)
        strcpy(reinterpret_cast<char*>(_credential.password), passphrase);
    }
    else {
      // AC_PRINCIPLE_RSSI is available when SSID and password are not provided.
      if (_apConfig.principle == AC_PRINCIPLE_RSSI) {
        // Find the strongest signal from the broadcast among the saved credentials.
        if ((cs = _loadCurrentCredential(reinterpret_cast<char*>(current.ssid), reinterpret_cast<char*>(current.password), AC_PRINCIPLE_RSSI, false))) {
          ssid = reinterpret_cast<const char*>(current.ssid);
          passphrase = reinterpret_cast<const char*>(current.password);
          AC_DBG("Adopted:%.32s\n", ssid);
        }
      }
      _loadAvailCredential(reinterpret_cast<const char*>(current.ssid));
    }

    if (cs) {
      // Advance configuration for STA mode. Restore previous configuration of STA.
      // _loadAvailCredential(reinterpret_cast<const char*>(current.ssid));
      if (!_configSTA(_apConfig.staip, _apConfig.staGateway, _apConfig.staNetmask, _apConfig.dns1, _apConfig.dns2))
        return false;

      // Try to connect by STA immediately.
      if (!_rfAdHocBegin)
        cs = WiFi.begin() != WL_CONNECT_FAILED;
      else {
        _disconnectWiFi(false);
        cs = WiFi.begin(ssid, passphrase) != WL_CONNECT_FAILED;
      }
      AC_DBG("WiFi.begin(%s%s%s)", ssid == nullptr ? "" : ssid, passphrase == nullptr ? "" : ",", passphrase == nullptr ? "" : passphrase);
      if (cs)
        cs = _waitForConnect(timeout) == WL_CONNECTED;
      else {
        AC_DBG_DUMB(" failed\n");
      }
    }

    // Reconnect with a valid credential as the autoReconnect option is enabled.
    if (!cs && _apConfig.autoReconnect && !_rfAdHocBegin) {
      // Load a valid credential.
      char  ssid_c[sizeof(station_config_t::ssid) + sizeof('\0')];
      char  password_c[sizeof(station_config_t::password) + sizeof('\0')];
      AC_DBG("autoReconnect");
      if ((cs = _loadCurrentCredential(ssid_c, password_c, _apConfig.principle, strlen(reinterpret_cast<const char*>(current.ssid)) > 0))) {
        // Try to reconnect with a stored credential.
        AC_DBG_DUMB(", %s(%s) loaded\n", ssid_c, _apConfig.principle == AC_PRINCIPLE_RECENT ? "RECENT" : "RSSI");
        const char* psk = strlen(password_c) ? password_c : nullptr;
        _configSTA(IPAddress(_credential.config.sta.ip), IPAddress(_credential.config.sta.gateway), IPAddress(_credential.config.sta.netmask), IPAddress(_credential.config.sta.dns1), IPAddress(_credential.config.sta.dns2));
        cs = WiFi.begin(ssid_c, psk) != WL_CONNECT_FAILED;
        AC_DBG("WiFi.begin(%s%s%s)", ssid_c, psk == nullptr ? "" : ",", psk == nullptr ? "" : psk);
        if (cs)
          cs = _waitForConnect(timeout) == WL_CONNECTED;
      }
      if (!cs) {
        AC_DBG_DUMB(" failed\n");
      }
    }
  }
  _currentHostIP = WiFi.localIP();

  // End first begin process, the captive portal specific process starts here.
  if (cs) {
    // Activate AutoConnectUpdate if it is attached and incorporate it into the AutoConnect menu.
    if (_update)
      _update->enable();
  }
  // Rushing into the portal.
  else {
    // Connection unsuccessful, launch the captive portal.
    // The captive portal is effective at the autoRise is valid only.
    if (_apConfig.autoRise) {

      // Change WiFi working mode, Enable AP with STA
      WiFi.setAutoConnect(false);
      _disconnectWiFi(false);

      // Activate the AP mode with configured softAP and start the access point.
      _softAP();
      _currentHostIP = WiFi.softAPIP();

      // Fork to the exit routine that starts captive portal.
      cs = _onDetectExit ? _onDetectExit(_currentHostIP) : true;

      // Start Web server when TCP connection is enabled.
      _startWebServer();

      // Start captive portal without cancellation by DetectExit.
      if (cs) {
        // Prepare for redirecting captive portal detection.
        // Pass all URL requests to _captivePortal to disguise the captive portal.
        _startDNSServer();

        // The following two lines are the trick statements.
        // They have the effect of avoiding unintended automatic
        // reconnection by autoReconnect within handleClient.
        // Also retainPortal too.
        bool  actReconnect = _apConfig.autoReconnect;
        bool  actRetainPortal = _apConfig.retainPortal;
        _apConfig.autoReconnect = false;
        _apConfig.retainPortal = true;

        // Start the captive portal to make a new connection
        bool  hasTimeout = false;
        _portalAccessPeriod = millis();
        while (WiFi.status() != WL_CONNECTED && !_rfReset) {
          handleClient();
          // By an exit routine to escape from Captive portal
          if (_whileCaptivePortal) {
            if (!_whileCaptivePortal())
              break;
          }
          // Force execution of queued processes.
          yield();
          // Check timeout
          if ((hasTimeout = _hasTimeout(_apConfig.portalTimeout))) {
            AC_DBG("CP timeout exceeded:%ld\n", millis() - _portalAccessPeriod);
            break;
          }
        }
        cs = WiFi.status() == WL_CONNECTED;

        // Restore actual autoReconnect and retainPortal settings.
        _apConfig.autoReconnect = actReconnect;
        _apConfig.retainPortal = actRetainPortal;

        // Captive portal staying time exceeds timeout,
        // Close the portal if an option for keeping the portal is false.
        if (!cs && hasTimeout) {
          if (_apConfig.retainPortal) {
            _purgePages();
            AC_DBG("Maintain portal\n");
          }
          else
            _stopPortal();
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
  return true; //_config();
}

/**
 *  Configure AutoConnect portal access point.
 *  @param  Config  AutoConnectConfig class instance.
 */
bool AutoConnect::config(AutoConnectConfig& Config) {
  _apConfig = Config;
  return true; //_config();
}

/**
 *  Configure access point.
 *  Set up access point with internal AutoConnectConfig parameter corrected
 *  by Config method.
 */
bool AutoConnect::_configAP(void) {
  if (static_cast<uint32_t>(_apConfig.apip) == 0U || static_cast<uint32_t>(_apConfig.gateway) == 0U || static_cast<uint32_t>(_apConfig.netmask) == 0U) {
    AC_DBG("Warning: Contains invalid SoftAPIP address(es).\n");
  }
  bool  rc = WiFi.softAPConfig(_apConfig.apip, _apConfig.gateway, _apConfig.netmask);
  AC_DBG("SoftAP configure %s, %s, %s %s\n", _apConfig.apip.toString().c_str(), _apConfig.gateway.toString().c_str(), _apConfig.netmask.toString().c_str(), rc ? "" : "failed");
  return rc;
}

/**
 *  Advance configuration for STA mode.
 *  @param  ip       IP address
 *  @param  gateway  Gateway address
 *  @param  netmask  Netmask
 *  @param  dns1     Primary DNS address
 *  @param  dns2     Secondary DNS address
 *  @return true     Station successfully configured
 *  @return false    WiFi.config failed
 */
bool AutoConnect::_configSTA(const IPAddress& ip, const IPAddress& gateway, const IPAddress& netmask, const IPAddress& dns1, const IPAddress& dns2) {
  bool  rc;

  AC_DBG("WiFi.config(IP=%s, Gateway=%s, Subnetmask=%s, DNS1=%s, DNS2=%s)\n", ip.toString().c_str(), gateway.toString().c_str(), netmask.toString().c_str(), dns1.toString().c_str(), dns2.toString().c_str());
  if (!(rc = WiFi.config(ip, gateway, netmask, dns1, dns2))) {
    AC_DBG("failed, AC::begin abort\n");
  }
#ifdef ARDUINO_ARCH_ESP8266
  AC_DBG("DHCP client(%s)\n", wifi_station_dhcpc_status() == DHCP_STOPPED ? "STOPPED" : "STARTED");
#endif
  return rc;
}

/**
 *  Get URI to redirect at boot. It uses the URI according to the
 *  AutoConnectConfig::bootUti setting with the AutoConnectConfig::homeUri
 *  as the boot path.
 *  @return the boot uri.
 */
String AutoConnect::_getBootUri(void) {
  if (_apConfig.bootUri == AC_ONBOOTURI_ROOT)
    return String(AUTOCONNECT_URI);
  else if (_apConfig.bootUri == AC_ONBOOTURI_HOME)
    return _apConfig.homeUri.length() > 0 ? _apConfig.homeUri : String("/");
  else
    return _emptyString;
}

/**
 *  Obtains the currently established AP connection to determine if the
 *  station configuration needs to run before the first WiFi.begin.
 *  Get the SSID of the currently connected AP stored in the ESP module
 *  by using the SDK API natively.
 *  AutoConnect::begin retrieves the IP configuration from the stored
 *  credentials of AutoConnectCredential based on that SSID and executes
 *  WiFi.config before WiFi.begin.
 *  @param  config  Station configuration stored in the ESP module.
 *  @return true    The config parameter has obtained configuration.
 *  @return false   Station configuration does not exist.
 */
bool AutoConnect::_getConfigSTA(station_config_t* config) {
  bool  rc;
  uint8_t*  ssid;
  uint8_t*  bssid;

#if defined(ARDUINO_ARCH_ESP8266)
  struct station_config current;
  ssid = current.ssid;
  bssid = current.bssid;
  rc = wifi_station_get_config(&current);
#elif defined(ARDUINO_ARCH_ESP32)
  wifi_config_t current;
  ssid = current.sta.ssid;
  bssid = current.sta.bssid;
  rc = (esp_wifi_get_config(WIFI_IF_STA, &current) == ESP_OK);
#endif
  if (rc) {
    memcpy(config->ssid, ssid, sizeof(station_config_t::ssid));
    memcpy(config->bssid, bssid, sizeof(station_config_t::bssid));
  }
  return rc;
}

/**
 *  Stops AutoConnect captive portal service.
 */
void AutoConnect::end(void) {
  _currentPageElement.reset();
  _ticker.reset();
  _update.reset();
  _ota.reset();

  _stopPortal();
  _dnsServer.reset();
  _webServer.reset();
}

/**
 *  Get the total amount of memory required to hold the AutoConnect credentials
 *  and any custom configuration settings stored in EEPROM.
 *  This function is available only for ESP8266 use.
 *  @return  Combined size of AutoConnect credentials and custom settings.
 */
uint16_t AutoConnect::getEEPROMUsedSize(void) {
#if defined(ARDUINO_ARCH_ESP8266)
  AutoConnectCredential credentials(_apConfig.boundaryOffset);
  return _apConfig.boundaryOffset + credentials.dataSize();
#elif defined(ARDUINO_ARCH_ESP32)
  return 0;
#endif
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
  bool  skipPostTicker;

  // Controls reconnection and portal startup when WiFi is disconnected.
  if (WiFi.status() != WL_CONNECTED) {

    // Launch the captive portal when SoftAP is active and autoRise is
    // specified to be maintained.
    // Pass all URL requests to _captivePortal to disguise the captive portal.
    if (_apConfig.retainPortal && _apConfig.autoRise) {
      if (!(WiFi.getMode() & WIFI_AP)) {
        _softAP();
        _currentHostIP = WiFi.softAPIP();
      }
      if (!_dnsServer) {
        bool  cs = _onDetectExit ? _onDetectExit(_currentHostIP) : true;
        if (cs)
          _startDNSServer();
      }
    }

    // AutoConnectConfig::reconnectInterval allows a dynamic connection
    // to a known access point without blocking the execution of
    // Sketch's loop function.
    if (_apConfig.autoReconnect && _apConfig.reconnectInterval > 0) {
      int8_t  sc = WiFi.scanComplete();

      // Scan has not triggered then starts asynchrony scan and repeats at
      // intervals of time with AutoConnectConfig::reconnectInterval value
      // multiplied by AUTOCONNECT_UNITTIME.
      if (sc == WIFI_SCAN_FAILED) {
        if (millis() - _attemptPeriod > ((unsigned long)_apConfig.reconnectInterval * AUTOCONNECT_UNITTIME * 1000)) {
          int8_t  sn = WiFi.scanNetworks(true, true);
          AC_DBG("autoReconnect %s\n", sn == WIFI_SCAN_RUNNING ? "running" : "failed");
          _attemptPeriod = millis();
          (void)(sn);
        }
      }

      // After the background scan is complete, seek a connectable
      // access point. If it is found, it will generate a connection
      // request inside.
      else if (sc != WIFI_SCAN_RUNNING) {
        AC_DBG("%d network(s) found\n", (int)sc);
        if (sc > 0) {
          if (_seekCredential(_apConfig.principle, _rfAdHocBegin ? AC_SEEKMODE_CURRENT : AC_SEEKMODE_ANY))
            _rfConnect = true;
        }
        WiFi.scanDelete();
      }
    }
  }
  else
    _attemptPeriod = millis();

  // Handling processing requests to AutoConnect.
  if (_rfConnect) {
    // Leave from the AP currently.
    if (WiFi.status() == WL_CONNECTED)
      _disconnectWiFi(true);

    // Leave current AP, reconfigure station
    _configSTA(_apConfig.staip, _apConfig.staGateway, _apConfig.staNetmask, _apConfig.dns1, _apConfig.dns2);

    // Purge scan results to initialize the asynchronous network scan that
    // will be triggered by disconnection during handleRequests.
    WiFi.scanDelete();

    // An attempt to establish a new AP.
    int32_t ch = _connectCh == 0 ? _apConfig.channel : _connectCh;
    char ssid_c[sizeof(station_config_t::ssid) + 1];
    char password_c[sizeof(station_config_t::password) + 1];
    *ssid_c = '\0';
    strncat(ssid_c, reinterpret_cast<const char*>(_credential.ssid), sizeof(ssid_c) - 1);
    *password_c = '\0';
    strncat(password_c, reinterpret_cast<const char*>(_credential.password), sizeof(password_c) - 1);
    AC_DBG("WiFi.begin(%s%s%s) ch(%d)", ssid_c, strlen(password_c) ? "," : "", strlen(password_c) ? password_c : "", (int)ch);

    if (WiFi.begin(ssid_c, password_c, ch) != WL_CONNECT_FAILED) {
      // Wait for the connection attempt to complete and send a response
      // page to notify the connection result.
      // End the current session to complete a response page transmission.
      _rsConnect = _waitForConnect(_apConfig.beginTimeout);
      do {
        _webServer->handleClient();
      } while (_webServer->client());

      if (_rsConnect == WL_CONNECTED) {
        // WLAN successfully connected then release the DNS server.
        // Also, stop WIFI_AP if retainPortal not specified.
        _stopDNSServer();
        if (!_apConfig.retainPortal) {
          WiFi.softAPdisconnect(true);
          WiFi.enableAP(false);
        }
        else {
          AC_DBG("Maintain SoftAP\n");
        }

        // It will automatically save the credential which was able to
        // establish current connection.
        if (WiFi.BSSID() != NULL) {
          memcpy(_credential.bssid, WiFi.BSSID(), sizeof(station_config_t::bssid));
          _currentHostIP = WiFi.localIP();
          _redirectURI = String(F(AUTOCONNECT_URI_SUCCESS));

          // Save current credential
          if (_apConfig.autoSave == AC_SAVECREDENTIAL_AUTO) {
            AutoConnectCredential credit(_apConfig.boundaryOffset);
            if (credit.save(&_credential)) {
              AC_DBG("%.*s credential saved\n", sizeof(_credential.ssid), reinterpret_cast<const char*>(_credential.ssid));
            }
            else {
              AC_DBG("credential %.*s save failed\n", sizeof(_credential.ssid), reinterpret_cast<const char*>(_credential.ssid));
            }
          }

          // Ensures that keeps a connection with the current AP
          // while the portal behaves.
          _setReconnect(AC_RECONNECT_SET);
        }
        else {
          AC_DBG("%.*s has no BSSID, saving is unavailable\n", sizeof(_credential.ssid), reinterpret_cast<const char*>(_credential.ssid));
        }

        // Activate AutoConnectUpdate if it is attached and incorporate
        // it into the AutoConnect menu.
        if (_update)
          _update->enable();
      }
      else {
        _currentHostIP = WiFi.softAPIP();
        _redirectURI = String(F(AUTOCONNECT_URI_FAIL));
        _disconnectWiFi(false);
        wl_status_t wl = WiFi.status();
        unsigned long tm = millis();
        while (wl != WL_IDLE_STATUS && wl != WL_DISCONNECTED && wl != WL_NO_SSID_AVAIL) {
          if (millis() - tm > 3000)
            break;
          delay(1);
          yield();
          wl = WiFi.status();
        }
        AC_DBG("Quit connecting, status(%d)\n", wl);
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
    // Response for disconnection request is not completed while
    // the session exists.
    if (!_webServer->client()) {
      // Disconnect from the current AP.
      _disconnectWiFi(false);
      while (WiFi.status() == WL_CONNECTED) {
        delay(10);
        yield();
      }
      AC_DBG("Disconnected ");
      if ((WiFi.getMode() & WIFI_AP) && !_apConfig.retainPortal) {
        _stopPortal();
      }
      else {
        if (_dnsServer)
          AC_DBG_DUMB("- Portal maintained");
        AC_DBG_DUMB("\n");
      }
      // Reset disconnection request
      _rfDisconnect = false;

      if (_apConfig.autoReset) {
        delay(1000);
        SOFT_RESET();
        delay(1000);
      }
    }
  }

  // Indicate that not disturb the ticker cycle during OTA.
  // It will be set to true during OTA in progress due to
  // subsequent OTA handling.
  skipPostTicker = false;

  // Handle the update behaviors for attached AutoConnectUpdate.
  if (_update) {
    _update->handleUpdate();
    if (_update->status() == UPDATE_PROGRESS)
      skipPostTicker = true;
  }

  // Attach AutoConnectOTA if OTA is available.
  if (_apConfig.ota == AC_OTA_BUILTIN) {
    if (!_ota) {
      _ota.reset(new AutoConnectOTA());
      _ota->attach(*this);
      _ota->authentication(_apConfig.auth);
      _ota->setTicker(_apConfig.tickerPort, _apConfig.tickerOn);
    }
  }

  // Post-process for AutoConnectOTA
  if (_ota) {
    if (_ota->status() == AutoConnectOTA::OTA_RIP) {
      // Indicate the reboot at the next handleClient turn
      // with on completion of the update via OTA.
      if (_webServer->client().connected()) {
        _webServer->client().setNoDelay(true);
        _ota->reset();
      }

      if (_ota->dest() == AutoConnectOTA::OTA_DEST_FIRM)
        // OTA for firmware update requires module reset.
        _rfReset = true;
    }
    else if (_ota->status() == AutoConnectOTA::OTA_PROGRESS)
      skipPostTicker = true;
    // Reflect the menu display specifier from AutoConnectConfig to
    // AutoConnectOTA page
    _ota->menu(_apConfig.menuItems & AC_MENUITEM_UPDATE);
  }

  // Post-process for ticker
  // Adjust the ticker cycle to the latest WiFi connection state.
  if (_ticker && !skipPostTicker) {
    uint8_t   tWidth;
    uint32_t  tCycle = WiFi.status() == WL_CONNECTED ? 0 : AUTOCONNECT_FLICKER_PERIODDC;
    tWidth = AUTOCONNECT_FLICKER_WIDTHDC;
    if (WiFi.getMode() & WIFI_AP) {
      tCycle = AUTOCONNECT_FLICKER_PERIODAP;
      tWidth = AUTOCONNECT_FLICKER_WIDTHAP;
    }
    if (tCycle != _ticker->getCycle()) {
      _ticker->stop();
      if (tCycle)
        _ticker->start(tCycle, tWidth);
    }    
  }
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
  AutoConnectAux* aux_p = _aux;
  while (aux_p) {
    if (!strcmp(aux_p->uri(), uri.c_str()))
      break;
    aux_p = aux_p->_next;
  }
  return aux_p;
}

/**
 *  Creates an AutoConnectAux dynamically with the specified URI and
 *  integrates it into the menu. Returns false if a menu item with
 *  the same URI already exists.
 *  @param  uri   An uri of a new item to add
 *  @param  title Title of the menu item
 *  @return A pointer to an added AutoConnectAux
 */
AutoConnectAux* AutoConnect::append(const String& uri, const String& title) {
  AutoConnectAux* reg = aux(uri);
  if (!reg) {
    reg = new AutoConnectAux(uri, title);
    reg->_deletable = true;
    join(*reg);
    return reg;
  }
  AC_DBG("%s has already listed\n", uri.c_str());
  return nullptr;
}

/**
 *  Creates an AutoConnectAux dynamically with the specified URI and
 *  integrates it into the menu. It will register the request handler
 *  for the WebServer after the addMenuItem works. It has similar
 *  efficacy to calling addMenuItem and WebSever::on at once.
 *  @param  uri     An uri of a new item to add
 *  @param  title   Title of the menu item
 *  @param  handler Function of the request handler for WebServer class
 *  @return true    Added
 *  @return A pointer to an added AutoConnectAux
 */
AutoConnectAux* AutoConnect::append(const String& uri, const String& title, WebServerClass::THandlerFunction handler) {
  if (_webServer) {
    AutoConnectAux* reg = append(uri, title);
    if (reg)
      _webServer->on(uri, handler);
    return reg;
  }
  AC_DBG("No WebServer instance\n");
  return nullptr;
}

/**
 *  Detach a AutoConnectAux from the portal.
 *  @param  uri   An uri of the AutoConnectAux should be released
 *  @return true  Specified AUX has released
 *  @return false Specified AUX not registered
 */
bool AutoConnect::detach(const String &uri) {
  AutoConnectAux**  self = &_aux;
  while (*self) {
    if (!strcmp((*self)->uri(), uri.c_str())) {
      AC_DBG("%s released\n", (*self)->uri());
      AutoConnectAux* ref = *self;
      *self = (*self)->_next;
      if (ref->_deletable)
        delete ref;
      return true;
    }
    self = &((*self)->_next);
  }
  AC_DBG("%s not listed\n", uri.c_str());
  return false;
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
    _aux = &aux;
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
  AutoConnectAux* aux = _aux;
  while (aux) {
    if (!strcmp(uri.c_str(), aux->uri())) {
      aux->on(handler, order);
      return true;
    }
    aux = aux->_next;
  }
  return false;
}

/**
 *  Register the exit routine that is being called when WiFi connected.
 *  @param  fn  A function of the exit routine.
 */
void AutoConnect::onConnect(ConnectExit_ft fn) {
  _onConnectExit = fn;
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
 *  Register an exit routine to call during the captive portal.
 *  @param  fn  A function of the exit routine that calls while captive portal.
 */
void AutoConnect::whileCaptivePortal(WhileCaptivePortalExit_ft fn) {
  _whileCaptivePortal = fn;
}

/**
 *  Load current available credential
 *  @param  ssid      A pointer to the buffer that SSID should be stored.
 *  @param  password  A pointer to the buffer that password should be stored.
 *  @param  principle  WiFi connection principle.
 *  @param  excludeCurrent  Skip loading the current SSID.
 *  @return true  Current SSID and password returned.
 *  @return false There is no available SSID.
 */
bool AutoConnect::_loadCurrentCredential(char* ssid, char* password, const AC_PRINCIPLE_t principle, const bool excludeCurrent) {
  bool  rc;

  if ((rc = _loadAvailCredential(nullptr, principle, excludeCurrent))) {
    strcpy(ssid, reinterpret_cast<const char*>(_credential.ssid));
    strcpy(password, reinterpret_cast<const char*>(_credential.password));
  }
  return rc;
}

/**
 *  Load stored credentials that match nearby WLANs.
 *  @param  ssid  SSID which should be loaded. If nullptr is assigned, search SSID with WiFi.scan.
 *  @param  principle  WiFi connection principle.
 *  @param  excludeCurrent  Skip loading the current SSID.
 *  @return true  A matched credential of BSSID was loaded.
 */
bool AutoConnect::_loadAvailCredential(const char* ssid, const AC_PRINCIPLE_t principle, const bool excludeCurrent) {
  AutoConnectCredential credential(_apConfig.boundaryOffset);

  if (credential.entries() > 0) {
    // Scan the vicinity only when the saved credentials are existing.
    if (!ssid) {
      int8_t  nn = WiFi.scanNetworks(false, true);
      AC_DBG_DUMB(", %d network(s) found", (int)nn);
      if (nn > 0)
        return _seekCredential(principle, excludeCurrent ? AC_SEEKMODE_NEWONE : AC_SEEKMODE_ANY);
    }

    // The SSID to load was specified.
    // Set the IP configuration globally from the saved credential.
    else if (strlen(ssid))
      if (credential.load(ssid, &_credential) >= 0) {
        if (_credential.dhcp == STA_STATIC) {
          _apConfig.staip = static_cast<IPAddress>(_credential.config.sta.ip);
          _apConfig.staGateway = static_cast<IPAddress>(_credential.config.sta.gateway);
          _apConfig.staNetmask = static_cast<IPAddress>(_credential.config.sta.netmask);
          _apConfig.dns1 = static_cast<IPAddress>(_credential.config.sta.dns1);
          _apConfig.dns2 = static_cast<IPAddress>(_credential.config.sta.dns2);
        }
        return true;
      }
  }
  return false;
}

/**
 *  Aims a connectable access point by seeking with the WiFi scan results.
 *  The collation uses the saved credentials, and the connection priority
 *  follows AutoConnectConfig::principle.
 *  Either BSSID or SSID of the collation key is determined at compile
 *  time according to the AUTOCONNECT_APKEY_SSID definition.
 *  @param  principle  WiFi connection principle.
 *  @param  mode  Seek mode for whether to target a specific SSID.
 *  @return true  A matched credential of BSSID was loaded.
 */
bool AutoConnect::_seekCredential(const AC_PRINCIPLE_t principle, const AC_SEEKMODE_t mode) {
  AutoConnectCredential credential(_apConfig.boundaryOffset);
  station_config_t  validConfig;  // Temporary to find the strongest RSSI.
  int32_t minRSSI = -120;         // Min value to find the strongest RSSI.

  // Seek SSID
  const char* currentSSID = WiFi.SSID().c_str();
  for (uint8_t n = 0; n < WiFi.scanComplete(); n++) {
    if (mode == AC_SEEKMODE_CURRENT) {
      // It finds a specific access point that matches the SSID
      // specified by AutoConnect::begin.
      if (!strcmp(WiFi.SSID(n).c_str(), reinterpret_cast<const char*>(_credential.ssid)))
        return true;
    }
    else {
      // Seek valid configuration according to the WiFi connection principle.
      // Verify that an available SSIDs meet AC_PRINCIPLE_t requirements.
      for (uint8_t i = 0; i < credential.entries(); i++) {
        credential.load(i, &_credential);
        // The access point collation key is determined at compile time
        // according to the AUTOCONNECT_APKEY_SSID definition, which is
        // either BSSID or SSID.
        if (_isValidAP(_credential, n)) {
          if ((mode == AC_SEEKMODE_NEWONE) && (strlen(currentSSID) > 0))
            continue;
          if (WiFi.RSSI(n) < _apConfig.minRSSI) {
            // Excepts SSID that has weak RSSI under the lower limit.
            AC_DBG("%s:%" PRId32 "dBm, rejected\n", reinterpret_cast<const char*>(_credential.ssid), WiFi.RSSI(n));
            continue;
          }
          // Determine valid credential
          switch (principle) {
          case AC_PRINCIPLE_RECENT:
            // By BSSID, exit to keep the credential just loaded.
            return true;

          case AC_PRINCIPLE_RSSI:
            // Verify that most strong radio signal.
            // Continue seeking to find the strongest WIFI signal SSID.
            if (WiFi.RSSI(n) > minRSSI) {
              minRSSI = WiFi.RSSI(n);
              memcpy(&validConfig, &_credential, sizeof(station_config_t));
            }
            break;
          }
          break;
        }
      }
    }
  }

  // Increasing the minSSI will indicate the successfully sought for AC_PRINCIPLE_RSSI.
  // Restore the credential that has maximum RSSI.
  if (minRSSI > -120) {
    memcpy(&_credential, &validConfig, sizeof(station_config_t));
    return true;
  }
  return false;
}

/**
 *  Changes WiFi mode to enable SoftAP and configure IPs with current
 *  AutoConnectConfig settings then start SoftAP.
 */
void AutoConnect::_softAP(void) {
  WiFi.enableAP(true);
  while (!(WiFi.getMode() & WIFI_AP)) {
    delay(1);
    yield();
  }

#if defined(ARDUINO_ARCH_ESP8266)
  _configAP();
#endif

  WiFi.softAP(_apConfig.apid.c_str(), _apConfig.psk.c_str(), _apConfig.channel, _apConfig.hidden);
  do {
    delay(100);
    yield();
  } while (!WiFi.softAPIP());

#if defined(ARDUINO_ARCH_ESP32)
  _configAP();
#endif

  if (_apConfig.apip) {
    do {
      delay(100);
      yield();
    } while (WiFi.softAPIP() != _apConfig.apip);
  }
  AC_DBG("SoftAP %s/%s Ch(%d) IP:%s %s\n", _apConfig.apid.c_str(), _apConfig.psk.c_str(), _apConfig.channel, WiFi.softAPIP().toString().c_str(), _apConfig.hidden ? "hidden" : "");
}

/**
 *  Starts Web server for AutoConnect service.
 */
void AutoConnect::_startWebServer(void) {
  // Boot Web server
  if (!_webServer) {
    // Only when hosting WebServer internally
    _webServer =  WebserverUP(new WebServerClass(AUTOCONNECT_HTTPPORT), std::default_delete<WebServerClass>() );
    AC_DBG("WebServer allocated\n");
  }
  // Discard the original the not found handler to redirect captive portal detection.
  // It is supposed to evacuate but ESP8266WebServer::_notFoundHandler is not accessible.
  _webServer->onNotFound(std::bind(&AutoConnect::_handleNotFound, this));
  // here, Prepare PageBuilders for captive portal
  if (!_responsePage) {
    _responsePage.reset( new PageBuilder() );
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
 *  Stops DNS server.
 *  Free its instance to avoid multiple launches when the retainPortal enabled.
 */
void AutoConnect::_stopDNSServer(void) {
  if (_dnsServer) {
    _dnsServer->stop();
    _dnsServer.reset();
    AC_DBG("DNS server stopped\n");
  }
}

/**
 *  Disconnect from the AP and stop the AutoConnect portal.
 *  Stops DNS server and flush tcp sending.
 */
void AutoConnect::_stopPortal(void) {
  _stopDNSServer();

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
    String location = String(F("http://")) + _webServer->client().localIP().toString() + _getBootUri();
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
    credential.load(args.arg(String(F(AUTOCONNECT_PARAMID_CRED))).c_str(), &_credential);
#ifdef AC_DEBUG
    IPAddress staip = IPAddress(_credential.config.sta.ip);
    AC_DBG("Credential loaded:%.*s(%s)\n", sizeof(station_config_t::ssid), reinterpret_cast<const char*>(_credential.ssid), _credential.dhcp == STA_DHCP ? "DHCP" : staip.toString().c_str());
#endif
  }
  else {
    AC_DBG("Queried SSID:%s\n", args.arg(AUTOCONNECT_PARAMID_SSID).c_str());
    // Credential had by the post parameter.
    strncpy(reinterpret_cast<char*>(_credential.ssid), args.arg(String(F(AUTOCONNECT_PARAMID_SSID))).c_str(), sizeof(_credential.ssid));
    strncpy(reinterpret_cast<char*>(_credential.password), args.arg(String(F(AUTOCONNECT_PARAMID_PASS))).c_str(), sizeof(_credential.password));
    // Static IP detection
    if (args.hasArg(String(F(AUTOCONNECT_PARAMID_DHCP)))) {
      _credential.dhcp = STA_DHCP;
      _credential.config.sta.ip = _credential.config.sta.gateway = _credential.config.sta.netmask = _credential.config.sta.dns1 = _credential.config.sta.dns2 = 0U;
    }
    else {
      _credential.dhcp = STA_STATIC;
      const String paramId[] = {
        String(F(AUTOCONNECT_PARAMID_STAIP)),
        String(F(AUTOCONNECT_PARAMID_GTWAY)),
        String(F(AUTOCONNECT_PARAMID_NTMSK)),
        String(F(AUTOCONNECT_PARAMID_DNS1)),
        String(F(AUTOCONNECT_PARAMID_DNS2))
      };
      for (uint8_t i = 0; i < sizeof(station_config_t::_config::addr) / sizeof(uint32_t); i++) {
        if (args.hasArg(paramId[i])) {
          IPAddress ip;
          if (ip.fromString(args.arg(paramId[i])))
            _credential.config.addr[i] = static_cast<uint32_t>(ip);
        }
      }
    }
  }

  // Restore the configured IPs to STA configuration
  _apConfig.staip = static_cast<IPAddress>(_credential.config.sta.ip);
  _apConfig.staGateway = static_cast<IPAddress>(_credential.config.sta.gateway);
  _apConfig.staNetmask = static_cast<IPAddress>(_credential.config.sta.netmask);
  _apConfig.dns1 = static_cast<IPAddress>(_credential.config.sta.dns1);
  _apConfig.dns2 = static_cast<IPAddress>(_credential.config.sta.dns2);

  // Determine the connection channel based on the scan result.
  _connectCh = 0;
  for (uint8_t nn = 0; nn < _scanCount; nn++) {
    String  ssid = WiFi.SSID(nn);
    if (!strncmp(ssid.c_str(), reinterpret_cast<const char*>(_credential.ssid), sizeof(station_config_t::ssid))) {
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
// radio channel occurs. Although the Espressif view is true. However,
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
  AC_DBG("Redirect to %s\n", redirect.c_str());
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
  AC_DBG("Host:%s,%s", _webServer->hostHeader().c_str(), uri.c_str());

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
  _currentPageElement.reset( _setupPage(uri) );
  if (!_currentPageElement && _aux) {
    // Requested URL is not a normal page, exploring AUX pages
    _currentPageElement.reset(_aux->_setupPage(uri));
  }

  if (_currentPageElement) {
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
  AutoConnectAux* aux = _aux;
  while (aux) {
    if (aux->_uriStr == requestUri) {
      aux->upload(_prevUri, upload);
      break;
    }
    aux = aux->_next;
  }
}

/**
 *  Purge allocated pages. 
 */
void AutoConnect::_purgePages(void) {
  _responsePage->clearElement();
  if (_currentPageElement) {
    _currentPageElement.reset();
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
  if (WiFi.status() == WL_CONNECTED)
    if (_onConnectExit) {
      IPAddress localIP = WiFi.localIP();
      _onConnectExit(localIP);
    }
  _attemptPeriod = millis();  // Save to measure the interval between an autoReconnect.
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
      AC_DBG("STA connection %s\n", WiFi.reconnect() ? "restored" : "failed");
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
