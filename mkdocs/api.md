## <i class="fa fa-code"></i> Include headers

### AutoConnect.h

```cpp
#include <AutoConnect.h>
```

## <i class="fa fa-code"></i> Defined macros

They contain in ```AutoConnectDefs.h```.

```cpp
#define AC_USE_SPIFFS                           // Use SPIFFS for the file system on the onboard flash, assumes LittleFS if not defined.
#define AC_DEBUG                                // Monitor message output activation
#define AC_DEBUG_PORT           Serial          // Default message output device
#define AUTOCONNECT_AP_IP       0x011CD9AC      // Default SoftAP IP
#define AUTOCONNECT_AP_GW       0x011CD9AC      // Default SoftAP Gateway IP
#define AUTOCONNECT_AP_NM       0x00FFFFFF      // Default subnet mask
#define AUTOCONNECT_DNSPORT     53              // Default DNS port at captive portal
#define AUTOCONNECT_HTTPPORT    80              // Default HTTP
#define AUTOCONNECT_MENU_TITLE  "AutoConnect"   // Default AutoConnect menu title
#define AUTOCONNECT_URI         "/_ac"          // Default AutoConnect root path
#define AUTOCONNECT_TIMEOUT     30000           // Default connection timeout[ms]
#define AUTOCONNECT_CAPTIVEPORTAL_TIMEOUT  0    // Captive portal timeout value
#define AUTOCONNECT_STARTUPTIME 30              // Default waiting time[s] for after reset
#define AUTOCONNECT_USE_JSON                    // Allow AutoConnect elements to be handled by JSON format
#define AUTOCONNECT_USE_UPDATE                  // Indicator of whether to use the AutoConnectUpdate feature.
#define AUTOCONNECT_UPDATE_PORT 8000            // Available HTTP port number for the update
#define AUTOCONNECT_UPDATE_TIMEOUT  8000        // HTTP client timeout limitation for the update [ms]
#define AUTOCONNECT_TICKER_PORT LED_BUILTIN     // Ticker port
#endif
```

!!! note "Macros placement moved"
    Source code placement of the above macros provided for user sketch changed from v0.9.7. The new code is in ```AutoConnectDefs.h```.

## <i class="fa fa-code"></i> Constructors

### AutoConnect

```cpp
AutoConnect()
```

AutoConnect default constructor. This entry internally allocates the ESP8266WebServer for ESP8266 or WebServer for ESP32 and is activated internally.

AutoConnect will call the user added handler to respond to the HTTP request using the [**ESP8266WebServer::on**](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer#client-request-handlers) (WebServer::on for ESP32) funtion. This call will be made from during the [**handleClient**](api.md#handleclient) of AutoConnect function.  
Therefore, in the use case of assigning AutoConnect in this constructor, it is necessary to know the instance of ESP8266WebServer in order to register the request handler. Sketch can use [**host**](api.md#host) functions to obtain a reference to an ESP8266WebServer instance that is internally hosted by AutoConnect.

- For ESP8266

```cpp
AutoConnect(ESP8266WebServer& webServer)
```

- For ESP32

```cpp
AutoConnect(WebServer& webServer)
```

Run the AutoConnect site using the externally ensured ESP8266WebServer for ESP8266 or WebServer for ESP32.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">webServer</span><span class="apidesc">A reference of ESP8266WebServer or WebServer instance.</span></dd></dl>

## <i class="fa fa-code"></i> Public member functions

### <i class="fa fa-caret-right"></i> append

- ESP8266/ESP32 Common

```cpp
AutoConnectAux* append(const String& uri, const String& title)
```

- For ESP8266

```cpp
AutoConnectAux* append(const String& uri, const String& title, ESP8266WebServer::THandlerFunction handler)
```

- For ESP32

```cpp
AutoConnectAux* append(const String& uri, const String& title, WebServer::THandlerFunction handler)
```

Creates an AutoConnectAux dynamically with the specified URI and integrates it into the menu. Calls with a request handler parameter can use this function as menu registration for a legacy page of ESP8266WebServer/WebServer. If the **handler** parameter specified, also it will register the request handler for the ESP8266WebServer/WebServer.  
AutoConnect manages the menu items using a sequence list, and this function always adds the item to the end of the list. Therefore, the order of the menu items is the additional order.  
Returns the pointer to created AutoConnectAux instance, the `nullptr` if an AutoConnectAux with the same URI already exists.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">uri</span><span class="apidesc">A string of the URI.</span></dd>
    <dd><span class="apidef">title</span><span class="apidesc">Title for menu item.</span></dd>
    <dd><span class="apidef">handler</span><span class="apidesc">Request handler function as type of **ESP8266WebServer::THandlerFunction**/**WebServer::THandlerFunction**.</span></dd>
    <dt>**Return value**</dt>
    <dd>A Pointer to a created AutoConnectAux instance.</dd></dl>

!!! note "An instance of ESP8266WebServer/WebServer is needed"
    The WebServer must have instantiated for calling with a request handler parameter. AutoConnect can instantiate and host a WebServer internally, but in that case, the point in time to call the append function with a request handler parameter must be after AutoConnect::begin.

### <i class="fa fa-caret-right"></i> aux

```cpp
AutoConnectAux* aux(const String& uri) const
```

Returns a pointer to AutoConnectAux with the URI specified by *uri*. If AutoConnectAux with that URI is not bound, it returns **nullptr**.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">uri</span><span class="apidesc">A string of the URI.</span></dd>
    <dt>**Return value**</dt>
    <dd>A Pointer of the AutoConnectAux instance.</dd>
</dl>

### <i class="fa fa-caret-right"></i> begin

```cpp
bool begin()
```

<p></p>

```cpp
bool begin(const char* ssid, const char* passphrase)
```

<p></p>

```cpp
bool begin(const char* ssid, const char* passphrase, unsigned long timeout)
```

Starts establishing the WiFi connection. The WiFi mode at this time is WIFI_STA.  
AutoConnect first invokes *WiFi.begin*. If the *ssid* and the *passphrase* are missing, its *WiFi.begin* has no SSID and Password. Regardless of the result, ESP8266WebServer/WebServer will start immediately after the first *WiFi.begin*.  
The captive portal will not be started if the connection has been established with first *WiFi.begin*. If the connection cannot establish, switch to WIFI_AP_STA mode and activate SoftAP. Then DNS server starts.<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">ssid</span><span class="apidesc">SSID to be connected.</span></dd>
    <dd><span class="apidef">passphrase</span><span class="apidesc">Password for connection.</span></dd>
    <dd><span class="apidef">timeout</span><span class="apidesc">A time out value in milliseconds for waiting connection.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Connection established, AutoConnect service started with WIFI_STA mode.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">Could not connected, Captive portal started with WIFI_AP_STA mode.</span></dd></dl>

### <i class="fa fa-caret-right"></i> config

```cpp
bool config(AutoConnectConfig& config)
```

<p></p>

```cpp
bool config(const char* ap, const char* password = nullptr)
```

Set AutoConnect configuration settings.<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">config</span><span class="apidesc">Reference to [**AutoConnectConfig**](apiconfig.md) containing SoftAP's parameters and static IP parameters.</span></dd>
    <dd><span class="apidef">ap</span><span class="apidesc">SSID for SoftAP. The default value is **esp8266ap** for ESP8266, **esp32ap** for ESP32.</span></dd>
    <dd><span class="apidef">password</span><span class="apidesc">Password for SodtAP. The default value is **12345678**.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Successfully configured.</span></dd>
    <dd><span class="apidef">false</span><span class="aidesc">Configuration parameter is invalid, some values out of range.</span></dd></dl>

### <i class="fa fa-caret-right"></i> detach

```cpp
bool detach(const String& uri)
```

Detach the AutoConnectAux with the specified URI from the management of AutoConnect. An unmanaged AutoConnectAux will no longer appear in menu items, and its page handler will no longer respond even if the URI is accessed directly.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">uri</span><span class="apidesc">URI of AutoConnectAux to be detached.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Successfully detached.</span></dd>
    <dd><span class="apidef">false</span><span class="aidesc">An AutoConnectAux with the specified URI does not exist.</span></dd></dl>

If the request handler registered in the detaching AutoConnectAux is for a legacy page of the ESP8266WebServer/WebServer, the URI is still valid after detaching. AutoConnect does not delete the request handler registered to ESP8266WebServer/WebServer with the `on` function. (It cannot be removed)

!!! hint "Deleting the AutoConnectAux"
    If the AutoConnectAux to detach was added by [AutoConnect::append](api.md#append), it will be automatically removed and freed from memory.

### <i class="fa fa-caret-right"></i> disableMenu

```cpp
void disableMenu(const uint16_t items)
```

Disable the [AutoConnect menu](menu.md) items specified by the items parameter with logical OR value using **AC_MENUITEM_t** constant.  
This function only works for AutoConnect primary menu items. It has no effect on disable for AutoConnectAux items. To disable the items by AutoConnectAux, use the [AutoConnectAux::menu](apiaux.md#menu) function.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">items</span><span class="apidesc">Specify the combined value of **AC_MENUITEM_t** of the items deleting from the AutoConnect menu. It provides the value calculated from the **logical OR** by the AC_MENUITEM_t value of each item. Refer to the [enableMenu](#enablemenu) about AC_MENUITEM_t.</span></dd></dl>

### <i class="fa fa-caret-right"></i> enableMenu

```cpp
void enableMenu(const uint16_t items)
```

Enable the [AutoConnect menu](menu.md) items specified by the items parameter with logical OR value using **AC_MENUITEM_t** constant.  
This function only works for AutoConnect primary menu items. It has no effect on enable for AutoConnectAux items. To enable the items by AutoConnectAux, use the [AutoConnectAux::menu](apiaux.md#menu) function.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">items</span><span class="apidesc">Specify the combined value of **AC_MENUITEM_t** of the items applying to the AutoConnect menu. It provides the value calculated from the **logical OR** by the AC_MENUITEM_t value of each item applied as a menu. AC_MENUITEM_t is enumeration type to identify each menu item and it has the below values.</span></dd>
: - **AC_MENUITEM_CONFIGNEW** :
    [Configure new AP](menu.md#configure-new-ap)
: - **AC_MENUITEM_OPENSSIDS** :
    [Open SSIDs](menu.md#open-ssids)
: - **AC_MENUITEM_DISCONNECT** :
    [Disconnect](menu.md#disconnect)
: - **AC_MENUITEM_RESET** :
    [Reset...](menu.md#reset)
: - **AC_MENUITEM_HOME** :
    [HOME](menu.md#home)
: - **AC_MENUITEM_DEVINFO** :
    [Device statistics](menu.md#where-the-from) as AutoConnect root page
: - **AC_MENUITEM_DELETESSID** :
    Enable to delete credentials on [Open SSIDs](http://localhost:8000/AutoConnect/menu.html#open-ssids).</dl>

!!! note "It is added, not replaced."
    The initial configuration of the AutoConnect menu items:  
    `AC_MENUITEM_CONFIGNEW | AC_MENUITEM_OPENSSIDS | AC_MENUITEM_DISCONNECT | AC_MENUITEM_RESET | AC_MENUITEM_HOME`  
    The enableMenu function adds an indication of the specified items to the current. Therefore, use the [disableMenu](#disableMenu) to remove the specified item from the initial menu.

### <i class="fa fa-caret-right"></i> end

```cpp
void end(void)
```

Stops AutoConnect captive portal service. Release ESP8266WebServer/WebServer and DNSServer. 

!!! warning "Attention to end"
    The end function releases the instance of ESP8266WebServer/WebServer and DNSServer. It can not process them after the end function.

### <i class="fa fa-caret-right"></i> getEEPROMUsedSize

```cpp
uint16_t getEEPROMUsedSize(void)
```

Returns the total amount of memory required to hold the AutoConnect credentials and any custom configuration settings stored in EEPROM. The Sketch that writes its own custom data to the EEPROM must call `EEPROM.begin` with this value.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>Total amount size of saved AutoConnect credentials and custom data.</dd></dl>

!!! note "The getEEPROMUsedSize is available for only ESP8266 use"
    It is available for only ESP8266 use and will return 0 when used with ESP32.
    
### <i class="fa fa-caret-right"></i> handleClient

```cpp
void handleClient(void)
```

Process the AutoConnect menu interface. The [ESP8266WebServer::handleClient](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer#handling-incoming-client-requests)[^1] function hosted by AutoConnect is also called from within AutoConnect to handle the request handlers contained in Sketch.

!!! info "Enhanced AutoConnect::handleClient"
    The handleClient function enhanced since AutoConnect 1.2.0 can start the captive portal according to the WiFi connection status.  
    By properly specifying [AutoConnectConfig::**retainPortal**](apiconfig.md#retainportal) and [AutoConnectConfig::**autoRise**](apiconfig.md#autorise), when handleClient detects WiFi disconnection, it shifts WiFi mode to **WIFI_AP_STA** and starts the DNS server together with **SoftAP** dynamically. Then trapping for incoming HTTP requests from client devices will be started by AutoConnect. Thus it will open the captive portal behind the execution of the sketch `loop()` function. The captive portal launched by enhanced handleClient does not interfere with sketch execution except waiting for the result of WiFi.begin.  
    Also, [AutoConnectConfig::**autoReconnect**](apiconfig.md#autoreconnect) has improved. The Sketch can specify the [AutoConnectConfig::**reconnectInterval**](apiconfig.md#reconnectinterval) to continue retrying the reconnection with enhanced handleClient.

[^1]:Equivalent to the **WebServer::handleClient** function on the **ESP32** platform.

### <i class="fa fa-caret-right"></i> handleRequest

```cpp
void handleRequest(void)
```

Handling for the AutoConnect menu request.

!!! warning "About used in combination with handleClient"
    The handleRequest function is not supposed to use with AutoConnect::handleClient. It should be used following ESP8266WebServer::handleClient or WebServer::handleClient.

### <i class="fa fa-caret-right"></i> home

```cpp
void home(String& uri)
```

Put a user site's home URI. The URI specified by home is linked from "HOME" in the AutoConnect menu.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">uri</span><span class="aidesc">A URI string of user site's home path.</span></dd></dl>

### <i class="fa fa-caret-right"></i> host

- For ESP8266

```cpp
ESP8266WebServer& host(void)
```

- For ESP32

```cpp
WebServer& host(void)
```

Returns the reference of the ESP8266WebServer/WebServer which is allocated in AutoConnect automatically.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>A reference of the ESP8266WebServer/WebServer.</dd></dl>

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

<p></p>

```cpp
void join(std::vector<std::reference_wrapper<AutoConnectAux>> aux)
```

Join the AutoConnectAux object to AutoConnect. AutoConnectAux objects can be joined one by one, or joined altogether. The AutoConnectAux object joined by the join function can be handled from the AutoConnect menu.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">aux</span><span class="apidesc">Reference to AutoConnectAux. It can be std::vector of std::reference_wrapper of AutoConnectAux with [list initialization](https://en.cppreference.com/w/cpp/language/list_initialization).</span></dd></dl>

### <i class="fa fa-caret-right"></i> load

```cpp
bool load(const String& aux)
```

<p></p>

```cpp
bool load(PGM_P aux)
```

<p></p>

```cpp
bool load(const __FlashStringHelper* aux)
```

<p></p>

```cpp
bool load(Stream& aux)
```

Load JSON document of AutoConnectAux which contains AutoConnectElements. If there is a syntax error in the JSON document, false is returned.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">aux</span><span class="apidesc">The input string to be loaded.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">The JSON document as AutoConnectAux successfully loaded.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">Loading JSON document unsuccessful, probably syntax errors have occurred or insufficient memory. You can diagnose the cause of loading failure using the [ArduinoJson Assistant](https://arduinojson.org/v5/assistant/).</span></dd></dl>

### <i class="fa fa-caret-right"></i> on

```cpp
bool on(const String& uri, const AuxHandlerFunctionT handler, AutoConnectExitOrder_t order = AC_EXIT_AHEAD)
```

Register the handler function of the AutoConnectAux.<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">uri</span><span class="apidesc">A string of the URI assigned to the AutoConnectAux page.</span></dd>
    <dd><span class="apidef">handler</span><span class="apidesc">A function that behaves when a request to the AutoConnectAux page occurs. AuxHandlerFunctionT type is defined by the following declaration.<p class=""apidesc">`String handler(AutoConnectAux&, PageArgument&)`</p></span></dd>
    <dd><span class="apidef">order</span><span class="apidesc">Specifies when the handler is called with the following enumeration value.</span></dd>
: - **AC_EXIT_AHEAD** :
    Called before AutoConnect generates the HTML of the page. You set the value of AutoConnectElements in the handler then its value will be displayed on the page.
: - **AC_EXIT_LATER** :
    Called after AutoConnect generates the HTML of the page. You can append to HTML generated by AutoConnect.
: - **AC_EXIT_BOTH** :
    Called even before generating HTML and after generated.</dl>

!!! caution "It is not ESP8266WebServer::on, not WebServer::on for ESP32."
    This function effects to AutoConnectAux only. However, it coexists with that of ESP8266WebServer::on or WebServer::on of ESP32. 

### <i class="fa fa-caret-right"></i> onConnect

```cpp
void onConnect(ConnectExit_ft fn)
```

Register the function which will call from AutoConnect at the WiFi connection established.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">fn</span><span class="apidesc">A function called at the WiFi connected.</span></dd>
</dl>

An *fn* specifies the function called when the WiFi connected. Its prototype declaration is defined as *ConnectExit_ft*.

```cpp
typedef std::function<void(IPaddress& localIP)> ConnectExit_ft
```

<dl class="apidl">
    <dt><strong>Parameter</strong></dt>
    <dd><span class="apidef">localIP</span><span class="apidesc">An IP address of the ESP module as STA.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> onDetect

```cpp
void onDetect(DetectExit_ft fn)
```

Register the function which will call from AutoConnect at the start of the captive portal.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">fn</span><span class="apidesc">A function called at the captive portal start.</span></dd>
</dl>

An *fn* specifies the function called when the captive portal starts. Its prototype declaration is defined as *DetectExit_ft*.

```cpp
typedef std::function<bool(IPAddress& softapIP)>  DetectExit_ft
```

<dl class="apidl">
    <dt><strong>Parameter</strong></dt>
    <dd><span class="apidef">softapIP</span><span class="apidesc">An IP address of SoftAP for the captive portal.</span></dd>
    <dt><strong>Return value</strong></dt>
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

Register the handler function for undefined URL request detected.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">fn</span><span class="apidesc">A function of the "not found" handler.</span></dd></dl>

### <i class="fa fa-caret-right"></i> onOTAEnd

```cpp
void onOTAEnd(OTAEndExit_ft fn)
```

Register the on-end exit routine that is called only once when the OTA is finished.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">fn</span><span class="apidesc">A function called when the OTA has been finished.</span></dd>
</dl>

An *fn* specifies the function called when the OTA has been finished. Its prototype declaration is defined as *OTAEndExit_ft*.

```cpp
typedef std::function<void(void)> OTAEndExit_ft
```

### <i class="fa fa-caret-right"></i> onOTAError

```cpp
void onOTAError(OTAErrorExit_ft fn)
```

Register the exit routine that is called when some error occurred.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">fn</span><span class="apidesc">A function called when some OTA error occurs.</span></dd>
</dl>

An *fn* specifies the function called when the some error occurred. Its prototype declaration is defined as *OTAErrorExit_ft*.

```cpp
typedef std::function<void(uint8_t error)> OTAErrorExit_ft
```

<dl class="apidl">
    <dt><strong>Parameter</strong></dt>
    <dd><span class="apidef">error</span><span class="apidesc">Error code of OTA. It is defined in the <a href="https://github.com/esp8266/Arduino/blob/master/cores/esp8266/Updater.h">Updater</a> class or the <a href="https://github.com/espressif/arduino-esp32/blob/master/libraries/Update/src/Update.h">Update</a> class of the Arduino core for each platform.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> onOTAProgress

```cpp
void onOTAProgress(OTAProgressExit_ft fn)
```

Register the exit routine that is called during the OTA progress.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">fn</span><span class="apidesc">A function called during the OTA progress.</span></dd></dl>

An *fn* specifies the function called during the OTA progress. Its prototype declaration is defined as *OTAProgressExit_ft*.

```cpp
typedef std::function<void(unsigned int amount, unsigned int size)> OTAProgressExit_ft
```

<dl class="apidl">
    <dt><strong>Parameters</strong></dt>
    <dd><span class="apidef">amount</span><span class="apidesc">Total amount of bytes received.</span></dd>
    <dd><span class="apidef">size</span><span class="apidesc">Block size of current send.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> onOTAStart

```cpp
void onOTAStart(OTAStartExit_ft fn)
```

Register the on-start exit routine that is called only once when the OTA has been started.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">fn</span><span class="apidesc">A function called at the OTA start.</span></dd></dl>

An *fn* specifies the function called when the OTA starts. Its prototype declaration is defined as *OTAStartExit_ft*.

```cpp
typedef std::function<void(void)> OTAStartExit_ft
```

### <i class="fa fa-caret-right"></i> where

```cpp
String where(void)
```

Returns an uri string of the AutoConnectAux uri object of the custom Web page that caused the request to the page.<br>
AutoConnect identifies the URI (ie. the referrer URI) that caused the request each time from the client occurs and will save the URI If the request source is a custom Web page of AutoConnectAux. The **where** function returns a pointer of AutoConnectAux which is a URI of a least recent request from the custom Web page.<br>
This function is provided to access the fields (ie. the AutoConnectElements) with a custom Web page handler of a page and is available only for request source that is the custom Web pages. It is invalid for HTTP requests from individual pages registered with the **on** handler of ESP8266WebServer/WebServer for ESP32. In other words, this function only returns the AutoConnecAux page which is a least recently displayed.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>An uri string of the AutoConnectAux that caused the request the page.</dd></dl>

The **where** function usage is described in the section [*Where to pick up the values*](achandling.md#where-to-pick-up-the-values).

### <i class="fa fa-caret-right"></i> whileCaptivePortal

```cpp
void whileCaptivePortal(WhileCaptivePortalExit_ft fn)
```

Register the function which will call from AutoConnect during a stay in the captive portal.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">fn</span><span class="apidesc">Function called at the captive portal start.</span></dd></dl>

An *fn* specifies the function called while staying in the captive portal. Its prototype declaration is defined as *WhileCaptivePortalExit_ft*.

```cpp
typedef std::function<bool(void)>   WhileCaptivePortalExit_ft
```

<dl class="apidl">
    <dt><strong>Return value</strong></dt>
    <dd><span class="apidef">true</span><span class="apidesc">Continues captive portal handling.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">Cancel the captive portal. AutoConnect::begin function will return with a false.</span></dd>
</dl>
