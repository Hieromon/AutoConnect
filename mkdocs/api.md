## <i class="fa fa-code"></i> Include headers

### AutoConnect.h

```cpp
#include <AutoConnect.h>
```

## <i class="fa fa-code"></i> Defined macros

They contain in ```AutoConnectDefs.h```.

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
#define AUTOCONNECT_CAPTIVEPORTAL_TIMEOUT  0    // Captive portal timeout value
#define AUTOCONNECT_USE_JSON                    // Allow AutoConnect elements to be handled by JSON format
```

!!! note "Macros placement moved"
    Source code placement of the above macros provided for user sketch changed from v0.9.7. The new code is in ```AutoConnectDefs.h```.

## <i class="fa fa-code"></i> Constructors

### AutoConnect

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

The [**handleClient**](api.md#handleclient) function of AutoConnect can include the response of the URI handler added by the user using the "*on*" function of ESP8266WebServer/WebServer. If ESP8266WebServer/WebServer is assigned internally by AutoConnect, the sketch can obtain that reference with the [**host**](api.md#host) function.
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">webServer</span><span class="apidesc">A reference of ESP8266WebServer or WebServer instance.</span></dd>
</dl>

## <i class="fa fa-code"></i> Public member functions

### <i class="fa fa-caret-right"></i> aux

```cpp
AutoConnectAux* aux(const String& uri) const
```
Returns a pointer to AutoConnectAux with the URI specified by *uri*. If AutoConnectAux with that URI is not bound, it returns **nullptr**.
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">uri</span><span class="apidesc">A string of the URI.</span></dd>
    <dt>**Return value**</dt>
    <dd>A Pointer of the AutoConnectAux instance.</dd>
</dl>

### <i class="fa fa-caret-right"></i> begin

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
    <dd><span class="apidef">ssid</span><span class="apidesc">SSID to be connected.</span></dd>
    <dd><span class="apidef">passphrase</span><span class="apidesc">Password for connection.</span></dd>
    <dd><span class="apidef">timeout</span><span class="apidesc">A time out value in milliseconds for waiting connection.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Connection established, AutoConnect service started with WIFI_STA mode.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">Could not connected, Captive portal started with WIFI_AP_STA mode.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> config

```cpp
bool config(AutoConnectConfig& config)
```  
```cpp
bool config(const char* ap, const char* password = nullptr)
```

Set SoftAP's WiFi configuration and static IP configuration. 
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">config</span><span class="apidesc">Reference to [**AutoConnectConfig**](apiconfig.md) containing SoftAP's parameters and static IP parameters.</span></dd>
    <dd><span class="apidef">ap</span><span class="apidesc">SSID for SoftAP. The default value is **esp8266ap** for ESP8266, **esp32ap** for ESP32.</span></dd>
    <dd><span class="apidef">password</span><span class="apidesc">Password for SodtAP. The default value is **12345678**.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Successfully configured.</span></dd>
    <dd><span class="apidef">false</span><span class="aidesc">Configuration parameter is invalid, some values out of range.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> end

```cpp
void end(void)
```

Stops AutoConnect captive portal service. Release ESP8266WebServer/WebServer and DNSServer. 

!!! warning "Attention to end"
    The end function releases the instance of ESP8266WebServer/WebServer and DNSServer. It can not process them after the end function.

### <i class="fa fa-caret-right"></i> handleClient

```cpp
void handleClient(void)
```

Process the AutoConnect menu interface. The handleClient() function of the ESP8266WebServer/WebServer hosted by AutoConnect is also called from within AutoConnect, and the client request handlers contained in the user sketch are also handled.

### <i class="fa fa-caret-right"></i> handleRequest

```cpp
void handleRequest(void)
```

Handling for the AutoConnect menu request.

!!! warning "About used in combination with handleClient"
    The handleRequest function is not supposed to use with AutoConnect::handleClient. It should be used with ESP8266WebServer::handleClient or WebServer::handleClient.

### <i class="fa fa-caret-right"></i> home

```cpp
void home(String& uri)
```

Put a user site's home URI. The URI specified by home is linked from "HOME" in the AutoConnect menu.
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">uri</span><span class="aidesc">A URI string of user site's home path.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> host

- For ESP8266

```cpp
ESP8266WebServer& host(void)
```

- For ESP32

```cpp
WebServer& host(void)
```

Returns the reference of the ESP8266WebServer/WebServer which is allocated in AutoConnect automatically.
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

### <i class="fa fa-caret-right"></i> join

```cpp
void join(AutoConnectAux& aux)
```
```cpp
void join(std::vector<std::reference_wrapper<AutoConnectAux>> aux)
```
Join the AutoConnectAux object to AutoConnect. AutoConnectAux objects can be joined one by one, or joined altogether. The AutoConnectAux object joined by the join function can be handled from the AutoConnect menu.
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">aux</span><span class="apidesc">Reference to AutoConnectAux. It can be std::vector of std::reference_wrapper of AutoConnectAux with [list initialization](https://en.cppreference.com/w/cpp/language/list_initialization).</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> load

```cpp
bool load(const String& aux)
```
```cpp
bool load(PGM_P aux)
```
```cpp
bool load(const __FlashStringHelper* aux)
```
```cpp
bool load(Stream& aux)
```
Load JSON document of AutoConnectAux which contains AutoConnectElements. If there is a syntax error in the JSON document, false is returned.
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">aux</span><span class="apidesc">The input string to be loaded.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">The JSON document as AutoConnectAux successfully loaded.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">Loading JSON document unsuccessful, probably syntax errors have occurred or insufficient memory. You can diagnose the cause of loading failure using the [ArduinoJson Assistant](https://arduinojson.org/v5/assistant/).</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> on

```cpp
bool on(const String& uri, const AuxHandlerFunctionT handler, AutoConnectExitOrder_t order = AC_EXIT_AHEAD)
```
Register the handler function of the AutoConnectAux.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">uri</span><span class="apidesc">A string of the URI assigned to the AutoConnectAux page.</span></dd>
    <dd><span class="apidef">handler</span><span class="apidesc">A function that behaves when a request to the AutoConnectAux page occurs. AuxHandlerFunctionT type is defined by the following declaration.<p class=""apidesc">`String handler(AutoConnectAux&, PageArgument&)`</p></span></dd>
    <dd><span class="apidef">order</span><span class="apidesc">Specifies when the handler is called with the following enumeration value.</span></dd>
: - **AC_EXIT_AHEAD** :
    Called before AutoConnect generates the HTML of the page. You set the value of AutoConnectElements in the handler then its value will be displayed on the page.
: - **AC_EXIT_LATER** :
    Called after AutoConnect generates the HTML of the page. You can append to HTML generated by AutoConnect.
: - **AC_EXIT_BOTH** :
    Called even before generating HTML and after generated.
</dl>

!!! caution "It is not ESP8266WebServer::on, not WebServer::on for ESP32."
    This function effects to AutoConnectAux only. However, it coexists with that of ESP8266WebServer::on or WebServer::on of ESP32. 

### <i class="fa fa-caret-right"></i> onDetect

```cpp
void onDetect(DetectExit_ft fn)
```
Register the function which will call from AutoConnect at the start of the captive portal.
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">fn</span><span class="apidesc">Function called at the captive portal start.</span></dd>

</dl>

An *fn* specifies the function called when the captive portal starts. Its prototype declaration is defined as "*DetectExit_ft*".

```cpp
typedef std::function<bool(IPAddress softapIP)>  DetectExit_ft
```
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">softapIP</span><span class="apidesc">An IP address of SoftAP for the captive portal.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Continues captive portal handling.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">Cancel the captive portal. AutoConnect::begin function will return with a false.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> onNotFound

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
    <dt>**Parameter**</dt>
    <dd><span class="apidef">fn</span><span class="apidesc">A function of the "not found" handler.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> where
```cpp
String where(void)
```
Returns an uri string of the AutoConnectAux uri object of the custom Web page that caused the request to the page.<br>
AutoConnect identifies the URI (ie. the referrer URI) that caused the request each time from the client occurs and will save the URI If the request source is a custom Web page of AutoConnectAux. The **where** function returns a pointer of AutoConnectAux which is a URI of a least recent request from the custom Web page.<br>
This function is provided to access the fields (ie. the AutoConnectElements) with a custom Web page handler of a page and is available only for request source that is the custom Web pages. It is invalid for HTTP requests from individual pages registered with the **on** handler of ESP8266WebServer/WebServer for ESP32. In other words, this function only returns the AutoConnecAux page which is a least recently displayed.
<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>An uri string of the AutoConnectAux that caused the request the page.</dd>
</dl>

The **where** function usage is described in the section [*Where to pick up the values*](achandling.md#where-to-pick-up-the-values).
