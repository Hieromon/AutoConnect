## Include headers

### <i class="fa fa-code"></i> AutoConnect.h

```cpp
#include <AutoConnect.h>
```

#### Defined macros

```cpp
#define AC_DEBUG                                // Monitor message output activation
#define AC_DEBUG_PORT           Serial          // Default message output device
#define AUTOCONNECT_AP_IP       0x01F4A8C0      // Default SoftAP IP
#define AUTOCONNECT_AP_GW       0x01F4A8C0      // Default SoftAP Gateway IP
#define AUTOCONNECT_AP_NM       0x00FFFFFF      // Default subnet mask
#define AUTOCONNECT_DNSPORT     53              // Default DNS port at captive portal
#define AUTOCONNECT_HTTPPORT    80              // Default HTTP
#define AUTOCONNECT_MENU_TITLE  "AutoConnect"   // Default AutoConnect menu title
#define AUTOCONNECT_STARTUPTIME 10              // Default waiting time[s] for after reset
#define AUTOCONNECT_URI         "/_ac"          // Default AutoConnect root path
#define AUTOCONNECT_TIMEOUT     30000           // Default connection timeout[ms]
```

## AutoConnect API

### <i class="fa fa-code"></i> Constructors

#### AutoConnect

```cpp
AutoConnect()
```

<a id="withparameter"></a>

AutoConnect default constructor. This entry internally allocates the ESP8266WebServer for ESP8266 or WebServer for ESP32 and is activated internally.

- For ESP8266

```cpp
AutoConnect(ESP8266WebServer& webServer)
```

- For ESP32

```cpp
AutoConnect(WebServer& webServer)
```

Run the AutoConnect site using the externally ensured ESP8266WebServer for ESP8266 or WebServer for ESP32.

The [**handleClient**](api.md#handleclient) function of AutoConnect can include the response of the URI handler added by the user using the "*on*" function of ESP8266WebServer/WebServer. If ESP8266WebServer/WebServer is assigned internally by AutoConnect, the sketch can obtain that reference with the [**host**](api.me#host) function.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">webServer</span>A reference of ESP8266WebServer or WebServer instance.</dd>
</dl>

### <i class="fa fa-code"></i> Public member functions

#### begin

```cpp
bool begin()
```  
```cpp
bool begin(const char* ssid, const char* passphrase)
```  
```cpp
bool begin(const char* ssid, const char* passphrase, unsigned long timeout)
```

Starts establishing the WiFi connection. The WiFi mode at this time is WIFI_STA.  
AutoConnect first invokes *WiFi.begin*. If the *ssid* and the *passphrase* are missing, its *WiFi.begin* has no SSID and Password. Regardless of the result, ESP8266WebServer/WebServer will start immediately after the first *WiFi.begin*.  
The captive portal will not be started if the connection has been established with first *WiFi.begin*. If the connection cannot establish, switch to WIFI_AP_STA mode and activate SoftAP. Then DNS server starts.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">ssid</span>SSID to be connected.</dd>
    <dd><span class="apidef">passphrase</span>Password for connection.</dd>
    <dd><span class="apidef">timeout</span>A time out value in milliseconds for waiting connection.</dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span>Connection established, AutoConnect service started with WIFI_STA mode.</dd>
    <dd><span class="apidef">false</span>Could not connected, Captive portal started with WIFI_AP_STA mode.</dd>
</dl>

#### config

```cpp
bool config(AutoConnectConfig& config)
```  
```cpp
bool config(const char* ap, const char* password = nullptr)
```

Set SoftAP's WiFi configuration and static IP configuration. 
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">config</span>Reference to [**AutoConnectConfig**](api.md#autoconnectconfig) containing SoftAP's parameters and static IP parameters.</dd>
    <dd><span class="apidef">ap</span>SSID for SoftAP. The default value is **esp8266ap** for ESP8266, **esp32ap** for ESP32.</dd>
    <dd><span class="apidef">password</span>Password for SodtAP. The default value is **12345678**.</dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span>Successfully configured.</dd>
    <dd><span class="apidef">false</span>Configuration parameter is invalid, some values out of range.</dd>
</dl>

#### end

```cpp
void end()
```

Stops AutoConnect captive portal service. Release ESP8266WebServer/WebServer and DNSServer. 

!!! warning "Attention to end"
    The end function releases the instance of ESP8266WebServer/WebServer and DNSServer. It can not process them after the end function.

#### handleClient

```cpp
void handleClient()
```

Process the AutoConnect menu interface. The handleClient() function of the ESP8266WebServer/WebServer hosted by AutoConnect is also called from within AutoConnect, and the client request handlers contained in the user sketch are also handled.

#### handleRequest

```cpp
void handleRequest()
```

Handling for the AutoConnect menu request.

!!! warning "About used in combination with handleClient"
    The handleRequest function is not supposed to use with AutoConnect::handleClient. It should be used with ESP8266WebServer::handleClient or WebServer::handleClient.

#### home

```cpp
void home(String uri)
```

Put a user site's home URI. The URI specified by home is linked from "HOME" in the AutoConnect menu.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">uri</span> A URI string of user site's home path.</dd>
</dl>

#### host

Returns the reference of the ESP8266WebServer/WebServer which is allocated in AutoConnect automatically.

- For ESP8266

```cpp
ESP8266WebServer& host()
```

- For ESP32

```cpp
WebServer& host()
```
<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>A reference of the ESP8266WebServer/WebServer.</dd>
</dl>

!!! note "&reference is not a pointer"
    A reference cannot be re-assigned, and must be assigned at initialization. It's like as bind as alias.      
    ```cpp
    ESP8266WebServer& server = portal.host();
    server.handleClient();
    ```
    or  
    ```cpp
    portal.host().handleClient();
    ```

#### onDetect

```cpp
void onDetect(DetectExit_ft fn)
```
Register the function which will call from AutoConnect at the start of the captive portal.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">fn</span>Function called at the captive portal start.</dd>

</dl>

An *fn* specifies the function called when the captive portal starts. Its prototype declaration is defined as "*DetectExit_ft*".

```cpp
typedef std::function<bool(IPAddress softapIP)>  DetectExit_ft
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">softapIP</span>An IP address of SoftAP for the captive portal.</dd>
    <dt>**Retuen value**</dt>
    <dd><span class="apidef">true</span>Continues captive portal handling.</dd>
    <dd><span class="apidef">false</span>Cancel the captive portal. AutoConnect::begin function will return with a false.</dd>
</dl>

#### onNotFound

- For ESP8266

```cpp
void onNotFound(ESP8266WebServer::THandlerFunction fn)
```

- For ESP32

```cpp
void onNotFound(WebServer::THandlerFunction fn)
```
Register the handler function for undefined URL request detected.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">fn</span>A function of the "not found" handler.</dd>
</dl>

## AutoConnectConfig API

### <i class="fa fa-code"></i> Constructor

#### AutoConnectConfig

```cpp
AutoConnectConfig();
```  
```cpp
AutoConnectConfig(const char* ap, const char* password);
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">ap</span>SSID for SoftAP. The length should be up to 31. The default value is **esp8266ap** for ESP8266, **esp32ap** for ESP32.</dd>
    <dd><span class="apidef">password</span>Password for SodtAP. The length should be from 8 to up to 63. The default value is **12345678**.</dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### apid
SoftAP's SSID.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd>String</dd>
</dl>

#### apip

Sets IP address for Soft AP in captive portal. When AutoConnect fails the initial WiFi.begin, it starts the captive portal with the IP address specified this.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">IPAddress</span>The default value is **192.168.244.1**</dd>
</dl>

#### autoReconnect

Automatically will try to reconnect with the past established access point (BSSID) when the current configured SSID in ESP8266/ESP32 could not be connected. By enabling this option, *AutoConnect::begin()* function will attempt to reconnect to a known access point using credentials stored in the EEPROM, even if the connection failed by current SSID.  
If the connection fails, starts the captive portal in SoftAP + STA mode.  
<dl class="apidl">
    <dt>**Type**</dt>
    <dd>bool</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef" style="width:230px;">true</span>Reconnect automatically.</dd>
    <dd><span class="apidef" style="width:230px;">false</span>Starts Captive Portal in SoftAP + STA mode without trying to reconnect. This is the default.</dd>
</dl>

When the autoReconnect option is enabled, an automatic connection will behave if the following conditions are satisfied.

- Invokes *AutoConnect::begin* without user name and password parameter as ```begin()```.
- If one of the saved BSSIDs (not the SSID) of the credentials matches the BSSID detected by the network scan.

#### autoReset

Reset ESP8266 module automatically after WLAN disconnected.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd>bool</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef" style="width:230px;">true</span>Reset after WiFi disconnected automatically.</dd>
    <dd><span class="apidef" style="width:230px;">false</span>No reset.</dd>
</dl>

#### autoRise

Captive portal activation switch. False for disabling the captive portal. It prevents starting the captive portal even if the connection at the first *WiFi.begin* fails.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd>bool</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef" style="width:230px;">true</span>Enable the captive portal. This is the default.</dd>
    <dd><span class="apidef" style="width:230px;">false</span>Disable the captive portal.</dd>
</dl>

#### autoSave

The credential saved automatically at the connection establishment.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd>AC_SAVECREDENTIAL_t</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef" style="width:230px;">AC_SAVECREDENTIAL_AUTO</span>The credential saved automatically. This is the default.</dd>
    <dd><span class="apidef" style="width:230px;">AC_SAVECREDENTIAL_NEVER</span>The credential no saved.</dd>
</dl>

#### boundaryOffset

Sets the offset address of the credential storage area for EEPROM. This value must be between greater than 4 and less than flash sector size. (4096 by SDK)  
The default value is 0.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd>uint16_t</dd>
</dl>

!!! warning "It will conflict with user data."
    If the sketch leaves this offset at zero, it will conflict the storage area of credentials with the user sketch owned data. It needs to use the behind of credential area.

#### channel

The channel number of WIFi when SoftAP starts.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd>uint8_t</dd>
    <dt>**Value**</dt>
    <dd>1 ~ 14. The default value is 1.</dd>
</dl>

!!! info "How do I choose Channel"
    Espressif Systems had announced the [application note](https://www.espressif.com/sites/default/files/esp8266_wi-fi_channel_selection_guidelines.pdf) about Wi-Fi channel selection.

#### dns1

Set primary DNS server address when using static IP address.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd>IPAddress</dd>
</dl>

#### dns2

Set secondary DNS server address when using static IP address.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd>IPAddress</dd>
</dl>

#### gateway

Sets gateway address for Soft AP in captive portal. When AutoConnect fails the initial WiFi.begin, it starts the captive portal with the IP address specified this.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">IPAddress</span>The default value is **192.168.244.1**</dd>
</dl>

#### hidden

Sets SoftAP to hidden SSID.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd>uint8_t</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef" style="width:230px;">0</span>SSID will be appeared. This is the default.</dd>
    <dd><span class="apidef" style="width:230px;">1</span>SSID will be hidden.</dd>
</dl>

#### homeUri

Sets the home path of user sketch. This path would be linked from 'HOME' in the AutoConnect menu.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd>String</dd>
</dl>

#### netmask

Sets subnet mask for Soft AP in captive portal. When AutoConnect fails the initial WiFi.begin, it starts the captive portal with the IP address specified this.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">IPAddress</span>The default value is **255.255.255.0**</dd>
</dl>

#### psk

Sets password for SoftAP. The length should be from 8 to up to 63. The default value is **12345678**.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd>String</dd>
</dl>

#### staip

Set a static IP address. The IP will behave with STA mode.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd>IPAddress</dd>
</dl>

#### staGateway

Set the gateway address when using static IP address.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd>IPAddress</dd>
</dl>

#### staNetmask

Set the subnetmask when using static IP address.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd>IPAddress</dd>
</dl>

### <i class="fa fa-code"></i> AutoConnectConfig example

```arduino
AutoConenct        Portal;
AutoConenctConfig  Config("", "passpass");    // SoftAp name is determined at runtime
Config.apid = ESP.hostname();                 // Retrieve host name to SotAp identification
Config.apip = IPAddress(192,168,10,101);      // Sets SoftAP IP address
Config.gateway = IPAddress(192,168,10,1);     // Sets WLAN router IP address
Config.netmask = IPAddress(255,255,255,0);    // Sets WLAN scope
Config.autoReconnect = true;                  // Enable auto-reconnect
Config.autoSave = AC_SAVECREDENTIAL_NEVER;    // No save credential
COnfig.boundaryOffet = 64;                    // Reserve 64 bytes for the user data in EEPROM. 
Config.homeUri = "/index.html"				  // Sets home path of the sketch application
Config.staip = IPAddress(192,168,10,10);      // Sets static IP
Config.staGateway = IPAddress(192,168,10,1);  // Sets WiFi router address
Config.staNetmask = IPAddress(255,255,255,0); // Sets WLAN scope
Config.dns1 = IPAddress(192,168,10,1);        // Sets primary DNS address
Portal.config(Config);                        // Configure AutoConnect
Portal.begin();                               // Starts and behaves captive portal
```

## <i class="fa fa-gift"></i> Something extra

The library presents two PNG icons which can be used to embed a hyperlink to the AutoConnect menu.

- Bar type <img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAApklEQVRYR2NkGGDAOMD2M4w6YDQEkEMgEJggZwCxGI0T5mug+alAvBFkD7IDXtLBcpjfXgEZ4ugOeAETpHEIgIwHeVYC3QH+0CgAS9AQgCwHRcFmdAfQ0E7cRo9mw0EVAqPlAKhwEKVTVsBZDsyiQ2k4Wg6gxPKgyoZ0Sn+o1iCHQBBQaiYQi9DYJTjbAyAJWluOtz0wWg7QOOqxGz+aDUdDYMBDAACA0x4hs/MPrwAAAABJRU5ErkJggg==" title="AutoConnect menu" alt="AutoConnect menu" />
- Cog type <img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAC2klEQVRIS61VvWsUQRSfmU2pon9BUIkQUaKFaCBKgooSb2d3NSSFKbQR/KrEIiIKBiGF2CgRxEpjQNHs7mwOUcghwUQ7g58IsbGxEBWsb2f8zR177s3t3S2cA8ftzPu993vzvoaSnMu2vRKlaqgKp74Q/tE8qjQPyHGcrUrRjwlWShmDbFMURd/a6TcQwNiYUmpFCPElUebcuQ2vz6aNATMVReHEPwzfSSntDcNwNo2rI+DcvQzhpAbA40VKyV0p1Q9snzBG1qYVcYufXV1sREraDcxpyHdXgkfpRBj6Uwm2RsC5dxxmZ9pdOY9cKTISRcHTCmGiUCh4fYyplTwG2mAUbtMTBMHXOgK9QfyXEZr+TkgQ1oUwDA40hEgfIAfj+HuQRaBzAs9eKyUZ5Htx+T3ZODKG8DzOJMANhmGomJVMXPll+hx9UUAlzZrJJ4QNCDG3VEfguu7mcpmcB/gkBOtShhQhchAlu5jlLUgc9ENgyP5gf9+y6LTv+58p5zySkgwzLNOIGc8sEoT1Lc53NMlbCQQuvMxeCME1NNPVVkmH/i3IzzXDtCSA0qQQwZWOCJDY50jsQRjJmkslEOxvTcDRO6zPxOh5xZglKkYLhWM9jMVnkIsTyMT6NBj7IbOCEjm6HxNVVTo2WXqEWJZ1T8rytB6GxizyDkPhWVpBqfiXUtbo/HywYJSpA9kMamNNPZ71R9Hcm+TMHHZNGw3EuraXEUldbfvw25UdOjqOt+JhMwJd7+jSTpZaEiIcaCDwPK83jtWnTkwnunFMtxeL/ge9r4XItt1RNNaj/0GAcV2bR3U5sG3nEh6M61US+Qrfd9Bs31GGulI2GOS/8dgcQZV1w+ApjIxB7TDwF9GcNzJzoA+rD0/8HvPnXQJCt2qFCwbBTfRI7UyXumWVt+HJ9NO4XI++bdsb0YyrqXmlh+AWOLHaLqS5CLQR5EggR3YlcVS9gKeH2hnX8r8Kmi1CAsl36QAAAABJRU5ErkJggg==" title="AutoConnect menu" alt="AutoConnect menu" />

To reference the icon, use the **AUTOCONNECT_LINK** macro in the sketch. It expands into the string literal as an HTML ```<a></a>``` tag with PNG embedded of the AutoConnect menu hyperlinks. Icon type is specified by the parameter of the macro.

<dl class="apidl">
    <dd><span class="apidef">BAR_24</span>Bars icon, 24x24.</dd>
    <dd><span class="apidef">BAR_32</span>Bars icon, 32x32.</dd>
    <dd><span class="apidef">BAR_48</span>Bars icon, 48x48.</dd>
    <dd><span class="apidef">COG_24</span>Cog icon, 24x24.</dd>
    <dd><span class="apidef">COG_32</span>Cog icon, 32x32.</dd>
</dl>

!!! note "Usage"
    ```arduino
    String html = "<html>";
    html += AUTOCONNECT_LINK(BAR_32);
    html += "</html>";
    server.send(200, "text/html", html);
    ```

