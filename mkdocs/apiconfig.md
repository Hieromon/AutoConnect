## <i class="fa fa-code"></i> Constructor

### AutoConnectConfig

```cpp
AutoConnectConfig()
```
<p></p>
```cpp
AutoConnectConfig(const char* ap, const char* password)
```
<p></p>
```cpp
AutoConnectConfig(const char* ap, const char* password, const unsigned long timeout)
```
<p></p>
```cpp
AutoConnectConfig(const char* ap, const char* password, const unsigned long timeout, const uint8_t channel)
```

<dl class="apidl">
    <dt><strong>Parameters</strong></dt>
    <dd><span class="apidef">ap</span><span class="apidesc">SSID for SoftAP. The length should be up to 31. The default value is <strong>esp8266ap</strong> for ESP8266, <strong>esp32ap</strong> for ESP32.</span></dd>
    <dd><span class="apidef">password</span><span class="apidesc">Password for SoftAP. The length should be from 8 to up to 63. The default value is <strong>12345678</strong>.</span></dd>
    <dd><span class="apidef">timeout</span><span class="apidesc">The timeout value of the captive portal in [ms] units. The default value is 0.</span></dd>
    <dd><span class="apidef">channel</span><span class="apidesc">The channel number of WIFi when SoftAP starts. The default values is 1.</span></dd>
</dl>

## <i class="fa fa-code"></i> Public member variables

### <i class="fa fa-caret-right"></i> apid

SoftAP's SSID.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc">The default value is **esp8266ap** for ESP8266, **esp32ap** for ESP32.</span></dd></dl>

### <i class="fa fa-caret-right"></i> apip

Sets IP address for Soft AP in captive portal. When AutoConnect fails the initial WiFi.begin, it starts the captive portal with the IP address specified this.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">IPAddress</span><span class="apidesc">The default value is **172.217.28.1**</span></dd></dl>

### <i class="fa fa-caret-right"></i> auth

Apply HTTP authentication with the AutoConnect web page. This This setting allows the Sketch to authenticate with "BASIC" or "DIGEST" scheme. It is given as an enumeration value of **AC_AUTH_t** that indicates the authentication scheme.  
This setting determines the default scheme for HTTP authentication with AutoConnect. When the [**AutoConnectConfig::authScope**](#authscope) is **AC_AUTHSCOPE_PARTIAL**, each [AutoConnectAux authentication](apiaux.md#authentication) scheme has priority.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>AC_AUTH_t</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">AC_AUTH_NONE</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">No authentication. This is the default.</span></dd>
    <dd><span class="apidef">AC_AUTH_DIGEST</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Use the [digest scheme](https://tools.ietf.org/html/rfc2617#section-3).</span></dd>
    <dd><span class="apidef">AC_AUTH_BASIC</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Use the [basic scheme](https://tools.ietf.org/html/rfc2617#section-2).</span></dd></dl>

### <i class="fa fa-caret-right"></i> authScope

Specifies the authentication scope of AutoConnect Web pages. The Sketch will be able to expand or narrow the range of authentication by this setting, which can be either as **AC_AUTHSCOPE_t** enumeration value.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>AC_AUTHSCOPE_t</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">AC_AUTHSCOPE_AUX</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Require authentication to access for all custom Web pages, excepting AutoConnect's pages. This is the Default.</span></dd>
    <dd><span class="apidef">AC_AUTHSCOPE_PARTIAL</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Authenticate only specific custom Web pages which are specified by [AutoConnectAux::authentication](apiaux.md#authentication) function or JSON description.</span></dd>
    <dd><span class="apidef">AC_AUTHSCOPE_PORTAL</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Require authentication to access for all AutoConnect's pages, including custom Web pages.</span></dd>
</dl>

This setting is available only when [AutoConnectConfig::auth](#auth) is other than **AC_AUTH_NONE**. Ignored if it is AC_AUTH_NONE.

Also, the authScope setting has another bit that indicates to allow authentication in the captive portal state. Its enum value cannot be used alone and is always for qualifying the above three enum values.<dl class="apidl">
    <dt>**Value**</dt>
    <dd><span class="apidef">AC_AUTHSCOPE_WITHCP</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Allow authentication with the captive portal state. This value cannot be used alone to declare an authentication scope. It indicates to enable authentication in the captive portal by the logical OR operator with one of the AC_AUTHSCOPE_t values above.</span></dd></dl>

### <i class="fa fa-caret-right"></i> autoReconnect

Automatically will try to reconnect with the past established access point (BSSID) when the current configured SSID in ESP8266/ESP32 could not be connected. By enabling this option, *AutoConnect::begin()* function will attempt to reconnect to a known access point using credentials stored in the flash, even if the connection failed by current SSID.  
If the connection fails, starts the captive portal in SoftAP+STA mode.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>bool</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Reconnect automatically.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">Starts Captive Portal in SoftAP + STA mode without trying to reconnect. This is the default.</span></dd></dl>

When the autoReconnect option is enabled, an automatic connection will behave if the following conditions are satisfied.

- Invokes *AutoConnect::begin* without user name and password parameter as `begin()`.
- If one of the saved credentials matches the BSSID (or SSID) detected by the network scan.

!!! Info "Either BSSID or SSID to aim the access point"
    Whether or not it points to the target access point is determined by matching the **SSID** or **BSSID**. The default key to collate is **BSSID**.  
    The BSSID is usually fixed to the MAC address unique to its access point device, but when using some mobile hotspots, the BSSID may change even for the same access point. If you operate inconvenience in aiming at the access point by BSSID, you can change the collation key to SSID by uncomment the below line in `AutoConnectDefs.h`:

    ```cpp
    #define AUTOCONNECT_APKEY_SSID
    ```

    If `AUTOCONNECT_APKEY_SSID` macro is defined when the library is compiled, the access points are collated by the SSID.

### <i class="fa fa-caret-right"></i> autoReset

Reset ESP8266 module automatically after WLAN disconnected.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>bool</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Reset after WiFi disconnected automatically.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">No reset.</span></dd></dl>

### <i class="fa fa-caret-right"></i> autoRise

Captive portal activation switch. False for disabling the captive portal. It prevents starting the captive portal even if the connection at the 1st-WiFi.begin fails.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>bool</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Enable the captive portal. This is the default.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">Disable the captive portal.</span></dd></dl>

### <i class="fa fa-caret-right"></i> autoSave

The credential saved automatically at the connection establishment.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>AC_SAVECREDENTIAL_t</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">AC_SAVECREDENTIAL_AUTO</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">The credential saved automatically. This is the default.</span></dd>
    <dd><span class="apidef">AC_SAVECREDENTIAL_NEVER</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">The credential no saved.</span></dd></dl>

### <i class="fa fa-caret-right"></i> beginTimeout

Specify the limit time to attempt WiFi connection to the accesspoint. AutoConnect uses this value to abort the connection attempt at **WiFi.begin**. Its actual value specified in milliseconds unit.  
The default value is `AUTOCONNECT_TIMEOUT` defined in [`AutoConnectDefs.h`](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h#L132) and the initial value is 30 seconds.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>unsigned long</dd></dl>

### <i class="fa fa-caret-right"></i> bootUri

Specify the location to be redirected after module reset in the AutoConnect menu. It is given as an enumeration value of **AC_ONBOOTURI_t** indicating either the AutoConnect root path or the user screen home path.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>AC_ONBOOTURI_t</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">AC_ONBOOTURI_ROOT</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Resetting the module redirects it to the AutoConnect root path. The root path is assumed to be AUTOCONNECT_URI defined in [`AutoConnectDefs.h`](api.md#defined-macros).</span></dd>
    <dd><span class="apidef">AC_ONBOOTURI_HOME</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">It is redirected to the URI specified by [**AutoConnectConfig::homeUri**](apiconfig.md#homeuri).</span></dd></dl>

### <i class="fa fa-caret-right"></i> boundaryOffset

Sets the offset address of the credential storage area for EEPROM. This value must be between greater than 4 and less than flash sector size. (4096 by SDK)  
The default value is 0.  
This option is valid only for ESP8266 or ESP32 arduino core 1.0.2 earlier.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>uint16_t</dd></dl>

!!! warning "It will conflict with user data."
    If the Sketch leaves this offset at zero, it will conflict the storage area of credentials with the user sketch owned data. It needs to use the behind of credential area.

### <i class="fa fa-caret-right"></i> channel

The channel number of WIFi when SoftAP starts.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>uint8_t</dd>
    <dt>**Value**</dt>
    <dd>1 ~ 14. The default value is 1.</dd></dl>

!!! info "How do I choose Channel"
    Espressif Systems had announced the [application note](https://www.espressif.com/sites/default/files/esp8266_wi-fi_channel_selection_guidelines.pdf) about Wi-Fi channel selection.

### <i class="fa fa-caret-right"></i> dns1

Set primary DNS server address when using static IP address.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>IPAddress</dd></dl>

### <i class="fa fa-caret-right"></i> dns2

Set secondary DNS server address when using static IP address.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>IPAddress</dd></dl>

### <i class="fa fa-caret-right"></i> gateway

Sets gateway address for Soft AP in captive portal. When AutoConnect fails the initial WiFi.begin, it starts the captive portal with the IP address specified this.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">IPAddress</span><span class="apidesc">The default value is **172.217.28.1**</span></dd></dl>

### <i class="fa fa-caret-right"></i> hidden

Sets SoftAP to hidden SSID.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>uint8_t</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">0</span><span class="aidesc">SSID will be appeared. This is the default.</span></dd>
    <dd><span class="apidef">1</span><span class="apidesc">SSID will be hidden.</span></dd></dl>

### <i class="fa fa-caret-right"></i> homeUri

Sets the home path of user sketch. This path would be linked from 'HOME' in the AutoConnect menu. The default for homeUri is "/".<dl class="apidl">
    <dt>**Type**</dt>
    <dd>String</dd></dl>

### <i class="fa fa-caret-right"></i> hostName

Sets the station host name of ESP8266/ESP32.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>String</dd></dl>

### <i class="fa fa-caret-right"></i> immediateStart

Disable the 1st-WiFi.begin and start the captive portal. If this option is enabled, the module will be in AP_STA mode and the captive portal. The evaluation rank of this parameter is lower than the [**AutoConnectConfig::autoRise**](apiconfig.md#autorise). Even if immediateStart is true, the captive portal will not launch if autoRise is false.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>bool</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Start the captive portal with [**AutoConnect::begin**](api.md#begin).</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">Enable the 1st-WiFi.begin and it will start captive portal when connection failed. This is default.</span></dd></dl>

### <i class="fa fa-caret-right"></i> menuItems

Configure applying items of the [AutoConnect menu](menu.md). You can arbitrarily combine valid menus by coordinating the menuItems value.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">uint16_t</span><span class="apidesc">It provides the combined **AC_MENUITEM_t** value of the item to apply to the AutoConnect menu.<br>Specify the value calculated from the **logical OR** by the AC_MENUITEM_t value of each item applied as a menu.  It affects not only disappear the items from the menu also invalidates the URI they have. As a consequence, even if it accesses the URL directly will occur a 404 error.<br>The default value is logical OR of AC_MENUITEM_CONFIGNEW, AC_MENUITEM_OPENSSIDS, AC_MENUITEM_DISCONNECT, AC_MENUITEM_RESET, AC_MENUITEM_UPDATE and AC_MENUITEM_HOME.</span></dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">AC_MENUITEM_NONE</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">No assign items except for the AutoConnectAux page item.</span></dd>
    <dd><span class="apidef">AC_MENUITEM_CONFIGNEW</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Appends [Configure new AP](menu.md#config-new-ap) item.</span></dd>
    <dd><span class="apidef">AC_MENUITEM_OPENSSIDS</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Appends [Open SSIDs](menu.md#open-ssids) item.</span></dd>
    <dd><span class="apidef">AC_MENUITEM_DISCONNECT</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Appends [Disconnect](menu.md#disconnect) item.</span></dd>
    <dd><span class="apidef">AC_MENUITEM_RESET</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Appends [Reset...](menu.md#reset) item.</span></dd>
    <dd><span class="apidef">AC_MENUITEM_UPDATE</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Appends [Update](menu.md#update) item.</span></dd>
    <dd><span class="apidef">AC_MENUITEM_HOME</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Appends [HOME](menu.md#home) item.</span></dd>
    <dd><span class="apidef">AC_MENUITEM_DEVINFO</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Appends the **Device info** item which links to [AutoConnect statistics page](menu.md##where-the-from).</span></dd></dl>

!!! info "How to specify the value of the menu items"
    An menuItems accepts the logical OR of AC_MENUITEM_t type value. For example, to enable only Open SSIDs and HOME items, specify:
    ```cpp
    AutoConnectConfig config;
    config.menuItems = AC_MENUITEM_OPENSSIDS | AC_MENUITEM_HOME;
    ```
    However, even if you specify like the above, the AutoConnectAux page items still display on the menu. To remove the AutoConnectAux items, use the [AutoConnectAux::menu](apiaux.md#menu) function.

### <i class="fa fa-caret-right"></i> minRSSI

Specify the lower limit of the WiFi signal strength allowed to use as an access point. This value should be greater than -120 as RSSI. Generally, a data link will not be established unless it exceeds -90 dBm. Also, packet transmission is not reliable below -70 dBm to -80 dBm.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">int16_t</span><span class="apidesc">The default value is -120</span></dd></dl>

### <i class="fa fa-caret-right"></i> netmask

Sets subnet mask for Soft AP in captive portal. When AutoConnect fails the initial WiFi.begin, it starts the captive portal with the IP address specified this.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">IPAddress</span><span class="apidesc">The default value is **255.255.255.0**</span></dd></dl>

### <i class="fa fa-caret-right"></i> ota

Specifies to import the built-in OTA update class into the Sketch. When this option is enabled, an **Update** item will appear in the AutoConnect menu, and the OTA update via Web browser will be automatically embedded to the Sketch.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>AC_OTA_t</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">AC_OTA_EXTRA</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">AutoConnect does not import AutoConnectOTA. This is the default.</span></dd>
    <dd><span class="apidef">AC_OTA_BUILTIN</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Specifies to include AutoConnectOTA in the Sketch.</span></dd></dl>

### <i class="fa fa-caret-right"></i> password

Set the password for authentication.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"> The default value is same as [psk](#psk).</span></dd></dl>

### <i class="fa fa-caret-right"></i> portalTimeout

Specify the timeout value of the captive portal in [ms] units. It is valid when the station is not connected and does not time out if the station is connected to the ESP module in SoftAP mode (i.e. Attempting WiFi connection with the portal function). If 0, the captive portal will not be timed-out.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">unsigned long</span><span class="apidesc">Captive portal timeout value. The default value is 0.</span></dd></dl>

### <i class="fa fa-caret-right"></i> preserveAPMode

Specifies starting the STA while maintaining the state of the SoftAP mode in the [**AutoConnect::begin**](api.md#begin). This setting only applies when the [**AutoConnectConfig::autoRise**](apiconfig.md#autorise) is false.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>bool</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">AutoConnect::begin keeps AP mode.</span></dd>
    <dd><span class="apidef">false</span>AutoConnect::begin will stop SoftAP at the beginning of the process.<span class="apidesc"></span></dd></dl>

Note that this option is not for starting the SoftAP forcibly in [**AutoConnect::begin**](api.md#begin) and only keeps AP mode, SoftAP initiation is left to the Sketch.

### <i class="fa fa-caret-right"></i> principle

Specify the connection order will attempt to connect to one of the highest RSSI values among multiple available access points. It is given as an enumeration value of **AC_PRINCIPLE_t** indicating.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>AC_PRINCIPLE_t</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">AC_PRINCIPLE_RECENT</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Attempts to connect in the order of the saved credentials entries. The entry order is generally a time series connected in the past.</span></dd>
    <dd><span class="apidef">AC_PRINCIPLE_RSSI</span><span class="apidesc"></span><span class="apidef">&nbsp;</span><span class="apidesc">Attempts to connect to one of the highest RSSI values among multiple available access points.</span></dd></dl>

### <i class="fa fa-caret-right"></i> psk

Sets password for SoftAP. The length should be from 8 to up to 63. The default value is **12345678**.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>String</dd></dl>

### <i class="fa fa-caret-right"></i> reconnectInterval

Specifies the number of units for interval time to attempt automatic reconnection when [**AutoConnectConfig::autoReconnect**](#autoreconnect) is enabled. This value is specified by the number of unit times from 0 to 255, and one unit time is macro-defined as `AUTOCONNECT_UNITTIME` in `AutoConnectDefs.h` file of library source code, and its initial value is 30[s].<dl class="apidl">
    <dt>**Type**</dt>
    <dd>uint8_t</dd></dl>

WiFi connection retry is repeated inside [**AutoConnect::handleClient**](api.md#handleClient) after the number of seconds that the reconnectInterval value is multiplied by `AUTOCONNECT_UNITTIME` from the previous attempt. Then, when the connection with one of the saved credentials is established, the automatic reconnection will stop. And while [**AutoConnectConfig::autoReconnect**](#autoreconnect) is enabled, if the WiFi connection is lost, it will start to auto-reconnect again inside [**AutoConnect::handleClient**](api.md#handleclient).

If **0** is specified for the reconnectInterval, background reconnection attempt repeatedly will not be made, and only once at the 1st-WiFi.begin failure in [**AutoConnect::begin**](api.md#begin). (Only when [**AutoConnectConfig::autoReconnect**](#autoreconnect) is enabled) The default value is 0.

!!! Info "AUTOCONNECT_UNITTIME"
    **`AUTOCONNECT_UNITTIME`** as macro defined in `AutoConnectDefs.h` file of library source code as the below:
    ```cpp
    // Number of seconds in uint time [s]
    #ifndef AUTOCONNECT_UNITTIME
    #define AUTOCONNECT_UNITTIME    30
    #endif
    ```

### <i class="fa fa-caret-right"></i> retainPortal

Specify whether to continue the portal function even if the captive portal timed out. If the true, when a timeout occurs, the [**AutoConnect::begin**](api.md#begin) function is exited with returns false, but the portal facility remains alive. So SoftAP remains alive and you can invoke AutoConnect while continuing sketch execution. The default is false.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>bool</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Continue the portal function even if the captive portal times out. The STA + SoftAP mode of the ESP module continues and accepts the connection request to the AP.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">When the captive portal times out, STA + SoftAP mode of the ESP module is stopped. This is default.</span></dd></dl>

!!! hint "Connection request after timed-out"
    With the **retainPortal**, even if AutoConnect::begin in the setup() is timed out, you can execute the Sketch and the portal function as a WiFi connection attempt by calling AutoConnect::handleClient in the loop().

!!! info "All unresolved addresses redirects to /_ac"
    If you enable the **retainPortal** option, **all unresolved URIs will be redirected to `SoftAPIP/_ac`**. It happens frequently as client devices repeat captive portal probes in particular. To avoid this, you need to exit from the WiFi connection Apps on your device once.

### <i class="fa fa-caret-right"></i> staip

Set a static IP address. The IP will behave with STA mode.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>IPAddress</dd></dl>

### <i class="fa fa-caret-right"></i> staGateway

Set the gateway address when using static IP address.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>IPAddress</dd></dl>

### <i class="fa fa-caret-right"></i> staNetmask

Set the subnetmask when using static IP address.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>IPAddress</dd></dl>

### <i class="fa fa-caret-right"></i> ticker

Set flicker signal output according to WiFi connection status during AutoConnect::begin behavior.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>bool</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Output the flicker signal while [AutoConnect::begin](api.md#begin) operation. The `AUTOCONNECT_TICKER_PORT` macro in the `AutoConnectDefs.h` header file assigns pins for signal output. The default pin is arduino valiant's `LED_BUILTIN`. For boards without the LED_BUILTIN pin, assume pin #2.</span></dd>
    <dd><span class="apidef">false</span>No flicker signal output.<span class="apidesc"></span></dd></dl>

### <i class="fa fa-caret-right"></i> tickerPort

Specifies the GPIO port number to output the flicker signal of the ticker. The default assumes on the board dependent definition LED_BUILTIN macro redefined by **`AUTOCONNECT_TICKER_PORT`** in [`AutoConnectDefs.h`](api.md#defined-macros).<dl class="apidl">
    <dt>**Type**</dt>
    <dd>uint8_t</dd></dl>

### <i class="fa fa-caret-right"></i> tickerOn

Specifies the active logic level of the flicker signal. This value indicates the active signal level when driving the ticker.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>uint8_t</dd>
    <dt>**Value**</dt>
    <dd><span class="apidef">LOW</span>A flicker signal is an active-high.<span class="apidesc"></span></dd>
    <dd><span class="apidef">HIGH</span>A flicker signal is an active-low.<span class="apidesc"></span></dd></dl>

### <i class="fa fa-caret-right"></i> title

Set the menu title.<dl class="apidl">
    <dt>**Type**</dt>
    <dd>String</dd></dl>

### <i class="fa fa-caret-right"></i> uptime

Specifies the waiting time for the module to reboot.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">int</span><span class="apidesc">The default value is AUTOCONNECT_TIMEOUT/1000.</span></dd></dl>

### <i class="fa fa-caret-right"></i> username

Set the username for authentication.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc">The default value is same as [apid](#apid).</span></dd></dl>

## <i class="fa fa-code"></i> AutoConnectConfig Initial values

| Public member | Data type | Initial value definition | Defined symbol [^1] |
|---------------|------|----------------|--------------------------|
| [apid](#apid) | String | `esp8266ap`<br>`esp32ap` | AUTOCONNECT_APID |
| [apip](#apip) | IPAddress | 172.217.28.1 | AUTOCONNECT_AP_IP |
| [auth](#auth) | AC_AUTH_t | AC_AUTH_NONE | AC_AUTH_NONE<br>AC_AUTH_DIGEST<br>AC_AUTH_BASIC |
| [authScope](#authscope) | AC_AUTHSCOPE_t | AC_AUTHSCOPE_AUX | AC_AUTHSCOPE_PARTIAL<br>AC_AUTHSCOPE_AUX<br>AC_AUTHSCOPE_AC<br>AC_AUTHSCOPE_PORTAL<br>AC_AUTHSCOPE_WITHCP |
| [autoReconnect](#autoreconnect) | bool | false | |
| [autoReset](#autoreset) | bool | true | |
| [autoRise](#autorise) | bool | true | |
| [autoSave](#autosave) | AC_SAVECREDENTIAL_t | AC_SAVECREDENTIAL_AUTO | AC_SAVECREDENTIAL_NEVER<br>AC_SAVECREDENTIAL_AUTO |
| [bootUri](#booturi) | AC_ONBOOTURI_t | AC_ONBOOTURI_ROOT | AC_ONBOOTURI_ROOT<br>AC_ONBOOTURI_HOME |
| [boundaryOffset](#boundaryoffset) | uint16_t | 0 | AC_IDENTIFIER_OFFSET |
| [channel](#channel) | uint8_t | 1 | AUTOCONNECT_AP_CH |
| [dns1](#dns1) | IPAddress | 0U | |
| [dns2](#dns2) | IPAddress | 0U | |
| [gateway](#gateway) | IPAddress | 172.217.28.1 | AUTOCONNECT_AP_GW |
| [hidden](#hidden) | uint8_t | 0 | |
| [homeUri](#homeuri) | String | `/` | AUTOCONNECT_HOMEURI |
| [hostName](#hostname) | String | NULL | |
| [immediateStart](#immediatestart) | bool | false | |
| [menuItems](#menuIiems) | uint16_t | AC_MENUITEM_CONFIGNEW<br>+ AC_MENUITEM_OPENSSIDS<br>+ AC_MENUITEM_DISCONNECT<br>+ AC_MENUITEM_RESET<br>+ AC_MENUITEM_UPDATE<br>+ AC_MENUITEM_HOME | AC_MENUITEM_CONFIGNEW<br>AC_MENUITEM_OPENSSIDS<br>AC_MENUITEM_DISCONNECT<br>AC_MENUITEM_RESET<br>AC_MENUITEM_UPDATE<br>AC_MENUITEM_HOME |
| [minRSSI](#minrssi) | int16_t | -120 | AUTOCONNECT_MIN_RSSI |
| [netmask](#netmask) | IPAddress | 172.217.28.1 | AUTOCONNECT_AP_NM |
| [ota](#ota) | AC_OTA_t | AC_OTA_EXTRA | AC_OTA_EXTRA<br>AC_OTA_BUILTIN |
| [password](#password) | String | Follow [psk](#psk) | |
| [portalTimeout](#portaltimeout) | unsigned long | 0 | AUTOCONNECT_CAPTIVEPORTAL_TIMEOUT |
| [preserveAPMode](#preserveapmode) | bool | false | |
| [principle](#principle) | AC_PRINCIPLE_t | AC_PRINCIPLE_RECENT | AC_PRINCIPLE_RECENT<br>AC_PRINCIPLE_RSSI |
| [psk](#psk) | String | `12345678` | AUTOCONNECT_PSK |
| [reconnectInterval](#reconnectinterval) | uint8_t | 0 | |
| [retainPortal](#retainportal) | bool | false | |
| [staGateway](#stagateway) | IPAddress | 0U | |
| [staip](#staip) | IPAddress | 0U | |
| [staNetmask](#stanetmask) | IPAddress | 0U | |
| [ticker](#ticker) | bool | false | |
| [tickerOn](#tickeron) | uint8_t | LOW | AUTOCONNECT_UPDATE_LEDON |
| [tickerPort](#tickerport) | uint8_t | LED_BUILTIN | AUTOCONNECT_TICKER_PORT |
| [title](#title) | String | `AutoConnect` | AUTOCONNECT_MENU_TITLE |
| [uptime](#uptime) | int | AUTOCONNECT_TIMEOUT/1000 | AUTOCONNECT_STARTUPTIME |
| [username](#username) | String | Follow [apid](#apid) | |

[^1]:
Those symbols are defined in [`AutoConnectDefs.h`](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h).

## <i class="fa fa-code"></i> AutoConnectConfig example

```cpp
AutoConnect        Portal;
AutoConnectConfig  Config("", "passpass");    // SoftAp name is determined at runtime
Config.apid = ESP.hostname();                 // Retrieve host name to SotAp identification
Config.apip = IPAddress(192,168,10,101);      // Sets SoftAP IP address
Config.gateway = IPAddress(192,168,10,1);     // Sets WLAN router IP address
Config.netmask = IPAddress(255,255,255,0);    // Sets WLAN scope
Config.autoReconnect = true;                  // Enable auto-reconnect
Config.autoSave = AC_SAVECREDENTIAL_NEVER;    // No save credential
Config.boundaryOffset = 64;                   // Reserve 64 bytes for the user data in EEPROM.
Config.portalTimeout = 60000;                 // Sets timeout value for the captive portal
Config.retainPortal = true;                   // Retains the portal function after timed-out
Config.homeUri = "/index.html";               // Sets home path of Sketch application
Config.title ="My menu";                      // Customize the menu title
Config.staip = IPAddress(192,168,10,10);      // Sets static IP
Config.staGateway = IPAddress(192,168,10,1);  // Sets WiFi router address
Config.staNetmask = IPAddress(255,255,255,0); // Sets WLAN scope
Config.dns1 = IPAddress(192,168,10,1);        // Sets primary DNS address
Portal.config(Config);                        // Configure AutoConnect
Portal.begin();                               // Starts and behaves captive portal
```
