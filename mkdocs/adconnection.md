AutoConnect aims to connect the ESP module as a station to a WiFi access point and equips with various APIs to maintain a WiFi connection as possible while sketch running. The main APIs are [AutoConnect::begin](api.md#begin) and [AutoConnect::handleClient](api.md#handleclient). You can make sketches with flexible WiFi connection capability by properly using these two APIs and the settings by [AutoConnectConfig](apiconfig.md).

- [Automatic reconnect](#automatic-reconnect)
- [Automatic reconnect (Background)](#automatic-reconnect-background)
- [Configure WiFi channel](#configure-wifi-channel)
- [Connects depending on the WiFi signal strength](#connects-depending-on-the-wifi-signal-strength)
- [Detects connection establishment to AP](#detects-connection-establishment-to-ap)
- [Match with known access points by SSID](#match-with-known-access-points-by-ssid)
- [Preserve AP mode](#preserve-ap-mode)
- [Timeout settings for a connection attempt](#timeout-settings-for-a-connection-attempt)

## Automatic reconnect

AutoConnect will change the WiFi mode depending on the situation. The [AutoConnect::begin](lsbegin.md) function starts WiFi mode in **STA** and starts the webserver if the connection is successful by the [**1st-WiFi.begin**](lsbegin.md). But if it will fail to connect with the least recently established access point, it will switch the WiFi mode to **AP_STA** and starts the DNS server to be able to launch a captive portal.

When the captive portal starts, **SoftAP** starts and STA disconnected. At this point, the station configuration information (it is known as the SDK's [station_config](https://github.com/esp8266/Arduino/blob/db75d2c448bfccc6dc308bdeb9fbd3efca7927ff/tools/sdk/include/user_interface.h#L249) structure) that the ESP module has stored on its own is discarded.

AutoConnect can connect to an access point again that has disconnected once, and its control is allowed by [*AutoConnectConfig::autoReconnect*](apiconfig.md#autoreconnect) that option specifies to attempt to reconnect to the past established access point using the saved credentials. If the [**autoReconnect**](apiconfig.md#autoreconnect) is enabled, AutoConnect will not launch SoftAP immediately even if 1st-WiFi.begin fails. When AutoConnect fails to connect WiFi, it will scan the WiFi signal to find out which access points it had connected to in the past. Then if it finds the saved BSSID in the broadcasts, AutoConnect will attempt to connect again applying the matched credential explicitly while still in STA mode. (AutoReconnect works well even with hidden SSID access points)

```cpp hl_lines="3"
AutoConnect       Portal;
AutoConnectConfig Config;
Config.autoReconnect = true;
Portal.config(Config);
Portal.begin();
```

The [**autoReconnect**](apiconfig.md#autoreconnect) option is only available for [AutoConnect::begin](api.md#begin) without SSID and PASSWORD parameter. If you use [AutoConnect::begin](api.md#begin) with an SSID and PASSWORD, no reconnection attempt will be made if the 1st-WiFi.begin fails to connect to that SSID.

!!! note "The autoReconnect is not autoreconnect"
    The [*WiFiSTAClass::disconnect*](https://github.com/espressif/arduino-esp32/blob/a0f0bd930cfd2d607bf3d3288f46e2d265dd2e11/libraries/WiFi/src/WiFiSTA.h#L46) function implemented in the arduino-esp32 has extended parameters than the ESP8266's arduino-core. The second parameter of WiFi.disconnect on the arduino-esp32 core that does not exist in the [ESP8266WiFiSTAClass](https://github.com/esp8266/Arduino/blob/7e1bdb225da8ab337373517e6a86a99432921a86/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.cpp#L296) has the effect of deleting the currently connected WiFi configuration and its default value is "false". On the ESP32 platform, even if WiFi.disconnect is executed, WiFi.begin without the parameters in the next turn will try to connect to that AP. That is, automatic reconnection is implemented in arduino-esp32 already. Although this behavior appears seemingly competent, it is rather a disadvantage in scenes where you want to change the access point each time. When explicitly disconnecting WiFi from the Disconnect menu, AutoConnect will erase the AP connection settings saved by the arduino-esp32 core. AutoConnect's automatic reconnection is a mechanism independent from the automatic reconnection of the arduino-esp32 core.
    
## Automatic reconnect (Background)

Combining [**autoReconnect**](advancedusage.md#automatic-reconnect) with [*AutoConnectConfig::reconnectInterval*](apiconfig.md#reconnectinterval) allows you to periodically repeat connection attempts to known access points within [AutoConnect::handleClient](api.md#handleclient). This process is pseudo-asynchronous and does not block the Sketch process in the `loop()` function.

The reconnectInterval specifies the interval time to seek for known access points with saved credentials during the **handleClient** loop and attempt to connect to the AP.

```cpp hl_lines="5 6"
AutoConnect       Portal;
AutoConnectConfig Config;

void setup() {
  Config.autoReconnect = true;    // Attempt automatic reconnection.
  Config.reconnectInterval = 6;   // Seek interval time is 180[s].
  Portal.config(Config);
  Portal.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Here to do when WiFi is connected.
  }
  else {
    // Here to do when WiFi is not connected.
  }

  Portal.handleClient();
}
```

Above Sketch shows a configuration example that you want to keep connecting to known access points as long as possible. When the WiFi connection is lost, it will start seeking the WiFi network every 30 seconds during the handleClient loop.

!!! info "Limitation for automatic reconnection to a specific access point"
    An access point that ESP module to reconnect automatically depends on whether the SSID and password argument existence with [AutoConnect::begin](api.md#begin). If the Sketch calls [AutoConnect::begin](api.md#begin) without specifying an SSID or password, the [autoReconnect](apiconfig.md#autoreconnect) will connect to one of the detected access points and cannot be pre-determined.  
    The other one, the case of the Sketch specifies SSID and password with [AutoConnect::begin](api.md#begin), the [autoReconnect](apiconfig.md#autoreconnect) will try to reconnect to a specified access point periodically during the handleClient loop.

Also, you can combine the background automatic reconnect performing inside the loop function by [handleClient](api.md#handleclient) with [*AutoConnectConfig::retainPortal*](apiconfig.md#retainportal) and [*AutoConnectConfig::autoReset*](apiconfig.md#autoreset), to enable pop up the captive portal automatically on the client device each time the ESP module disconnects from the access point.

```cpp
AutoConnect       Portal;
AutoConnectConfig Config;

void setup() {
  Config.autoReset = false;     // Not reset the module even by intentional disconnection using AutoConnect menu.
  Config.autoReconnect = true;  // Reconnect to known access points.
  Config.reconnectInterval = 6; // Reconnection attempting interval is 3[min].
  Config.retainPortal = true;   // Keep the captive portal open.
  Portal.config(Config);
  Portal.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Here to do when WiFi is connected.
  }
  else {
    // Here to do when WiFi is not connected.
  }
}
```

!!! info "The effective range of the [reconnectInterval](apiconfig.md#reconnectinterval) depending on the setting value"
    The range of values that reconnectInterval can take is 0 to 255. (Actual seconds are from 0 to 255&#xd7;AUTOCONNECT_UNITTIME)<br>
    Reconnect behavior depends on the setting value. If it is 0, reconnection will work if the 1st-WiFi.begin in AutoConnect::begin fails and will suspend during the handleClient loop. If reconnectInterval is greater than 0, AutoConnect will attempt to reconnect both in AutoConnect::begin and during the handleClient loop.

## Configure WiFi channel

Appropriately specifying the WiFi channel to use for ESP8266 and ESP32 is essential for a stable connection with the access point. AutoConnect remembers the WiFi channel with a credential of the access point once connected and reuses it.

The default channel when a captive portal starts and AutoConnect itself becomes an access point is the [*AutoConnectConfig::channel*](apiconfig.md#channel) member. If this channel is different from the channel of the access point you will attempt to connect, WiFi.begin may fail. The cause is that the ESP module shares the same channel in AP mode and STA mode. If the connection attempt is not stable, specifying a proper channel using AutoConnectConfig::channel may result in a stable connection.

## Connects depending on the WiFi signal strength

When the ESP module found the multiple available access points (ie. AutoConnect has connected in the past), the default behavior AutoConnect will attempt to connect to the least recent one. However, If the ESP module can operate properly with any access point, it is advantageous to establish a connection with the best one of the reception sensitivity. 

The [*AutoConnectConfig::principle*](apiconfig.md#principle) parameter has the connection disposition, and specifying **AC_PRINCIPLE_RSSI** will attempt to connect to one of the highest RSSI value among multiple available access points. Also You can expect stable WiFi connection by specifying the lower limit of signal strength using [*AutoConnectConfig::minRSSI*](apiconfig.md#minrssi).  
Combining these two parameters allows you to filter the destination AP when multiple available access points are found.

[*AutoConnectConfig::principle*](apiconfig.md#principle) affects the behavior of both 1st-WiFi.begin and [**autoReconnect**](advancedusage.md#automatic-reconnect). If you specify **AC_PRINCIPLE_RECENT** for the [**principle**](apiconfig.md#principle), it will try according to the conventional connection rules, but if you specify **AC_PRINCIPLE_RSSI**, it will try to connect to the access point that is sending the strongest WiFi signal at that time instead of the last accessed AP. Also, the static IPs will be restored from a saved credential instead of AutoConnectConfig. (The values specified by AutoConnectConfig is ignored)

<table>
<tr>
    <th rowspan="2"></th>
    <th rowspan="2">SSID &amp;<br>Password</th>
    <th rowspan="2">AutoConnectConfig<br>::principle</th>
    <th rowspan="2">Which credentials would be selected</th>
    <th rowspan="2">Static IPs</th>
</tr>
<tr>
</tr>
<tr>
    <td rowspan="3">AutoConnect<br>::begin</td>
    <td rowspan="2">NULL specified</td>
    <td>AC_PRINCIPLE_RECENT</td>
    <td>Nothing, depends on SDK saves</td>
    <td>Use the specified value of AutoConnectConfig<br></td>
</tr>
<tr>
    <td>AC_PRINCIPLE_RSSI</td>
    <td>Auto-selected credentials with max RSSI</td>
    <td>Restoring static IPs suitable for the SSID from saved credentials</td>
</tr>
<tr>
    <td>Specified with the Sketch</td>
    <td>Not effective</td>
    <td>By AutoConnect::begin parameters</td>
    <td>Use the specified value of AutoConnectConfig</td>
</tr>
<tr>
    <td rowspan="2">AutoReconnect</td>
    <td rowspan="2">Load from<br>saved credential</td>
    <td>AC_PRINCIPLE_RECENT</td>
    <td>Recently saved SSID would be chosen</td>
    <td rowspan="2">Restoring static IPs suitable for the SSID from saved credentials</td>
</tr>
<tr>
    <td>AC_PRINCIPLE_RSSI</td>
    <td>Auto-selected credentials with max RSSI</td>
</tr>
</table>

## Detects connection establishment to AP

The Sketch can detect that the ESP module has established a WiFi connection as a station to the access point. The [AutoConnect::begin](api.md#begin) or [AutoConnect::handleClient](api.md#handleclient) will transit the control temporarily to the function in the Sketch registered by [AutoConnect::onConnect](api.md#onconnect) when the ESP module establish a WiFi connection.  
The **ConnectExit** function registered with [AutoConnect::onConnect](api.md#onconnect) should have the following types and arguments:

```cpp
void ConnectExit(IPAddress& ip)
```

The **ConnectExit** function is of type *void*. The argument *ip* is the IP address assigned to the ESP module by the connected AP. [AutoConnect::onConnect](api.md#onconnect) allows the Sketch registers a **ConnectExit** function to AutoConnect. Also, you can make the function using a [**lambda expression**](https://en.cppreference.com/w/cpp/language/lambda).

```cpp hl_lines="3 4 5 6 7 8 12"
AutoConnect Portal;

void onConnect(IPAddress& ipaddr) {
  Serial.print("WiFi connected with ");
  Serial.print(WiFi.SSID());
  Serial.print(", IP:");
  Serial.println(ipaddr.toString());
}

void setup() {
  Serial.begin(115200);
  Portal.onConnect(onConnect);  // Register the ConnectExit function
  Portal.begin();
}

void loop() {
  Portal.handleClient();
}
```

In addition, a sketch that shuts down SoftAP when the ESP module connects to the access point can be described using a lambda expression as follows:

```cpp hl_lines="5 6 7 8 9 10 11 12"
AutoConnect Portal;

void setup() {
  Serial.begin(115200);
  Portal.onConnect([](IPAddress& ipaddr){
    Serial.printf("WiiFi connected with %s, IP:%s\n", WiFi.SSID().c_str(), ipaddr.toString().c_str());
    if (WiFi.getMode() & WIFI_AP) {
      WiFi.softAPdisconnect(true);
      WiFi.enableAP(false);
      Serial.printf("SoftAP:%s shut down\n", WiFi.softAPSSID().c_str());
    }
  });
  Portal.begin();
}

void loop() {
  Portal.handleClient();
}
```

!!! note "It is not an event"
    AutoConnect::onConnect has the same effect on the Sketch as the [WiFi.onStationModeConnected](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/generic-class.html#onevent), but AutoConnect does not use the event. Sketch can use `WiFi.onEvent` independently of AutoConnect.

## Match with known access points by SSID

By default, AutoConnect uses the **BSSID** to search for known access points. (Usually, it's the MAC address of the device) By using BSSID as the key to finding the WiFi network, AutoConnect can find even if the access point is hidden. However BSSIDs can change on some mobile hotspots, the BSSID-keyed searches may not be able to find known access points.  
If you operate inconvenience in aiming at the access point by BSSID, you can change the collation key from BSSID to SSID by uncommenting `AUTOCONNECT_APKEY_SSID` macro definition in [`AutoConnectDefs.h`](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h#L59) library source code.

```cpp
#define AUTOCONNECT_APKEY_SSID
```

Allow you to use [PlatformIO](https://platformio.org/platformio-ide) as a build system and give the following description to the [`platformio.ini`](https://docs.platformio.org/en/latest/projectconf/section_env_build.html?highlight=build_flags#build-flags), you can enable **AUTOCONNECT_APKEY_SSID** each build without modifying the library source code:

```ini
build_flags=-DAUTOCONNECT_APKEY_SSID
```

!!! info "Can't be found hidden APs in SSID-keyed"
    The hidden access point's SSID will be blank on the broadcast. So if the seek key is an SSID, AutoConnect will not find it.

## Preserve AP mode

Sketch using AutoConnect can open a gateway to the Internet by connecting to a WiFi router even through use Espressif's peculiar WiFi protocol (eg. [ESP-MESH](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/mesh.html) or [ESP-NOW](https://www.espressif.com/en/products/software/esp-now)). These specific communication protocols require to keeps AP + STA as the WiFi mode. That is, to apply these protocols, it needs to launch SoftAP by a sketch itself and then call [AutoConnect::begin](api.md#begin). But the default behavior of [AutoConnect::begin](api.md#begin) will turn off SoftAP always then it will unable to open a connection.

[*AutoConnectConfig::preserveAPMode*](apiconfig.md#preserveAPMode) setting maintains WIFI_AP mode without disabling SoftAP inside [AutoConnect::begin](api.md#begin). The Sketch can utilize the WiFi connection via AutoConnect with ESP-MESH and ESP-NOW protocol by enabling this option.

The following diagram quoted from the [ESP-MESH documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/mesh.html#mesh-building-a-network) that illustrates the typical topology of the MESH network. The module located at the Root Node bridges between the mesh network and the router by an application that handles two protocols, TCP/IP and ESP-MESH. Its SoftAP communicates with the internal mesh network as an interface of the mesh layer. On the other hand, STA performs station communication with the WiFi router as an interface of the TCP/IP layer. AutoConnect allows assists the connection between the router and the STA of the Root Node using [*AutoConnectConfig::preserveAPMode*](apiconfig.md#preserveapmode) and starting the SoftAP via Sketch separately.

<img src="https://docs.espressif.com/projects/esp-idf/en/latest/esp32/_images/mesh-bidirectional-data-stream.png">

Also in general, the Sketch should set **false** to [*AutoConnectConfig::autoRise*](apiconfig.md#autorise), **true** to [*AutoConnectConfig::immediateStart*](apiconfig.md#immediatestart) when applying to those protocols.

## Timeout settings for a connection attempt

AutoConnect uses [*AutoConnectConfig::beginTimeout*](apiconfig.md#begintimeout) value to limit time to attempt when connecting the ESP module to the access point as a WiFi station. The default value is **AUTOCONNECT_TIMEOUT** defined in [`AutoConnectDefs.h`](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h#L132) and the initial value is 30 seconds. (actually specified in milliseconds)  
For example, the following sketch sets the connection timeout to 15 seconds:

```cpp hl_lines="5"
AutoConnect Portal;
AutoConnectConfig Config;

void setup() {
  Config.beginTimeout = 15000; // Timeout sets to 15[s]
  Portal.config(Config);
  Portal.begin();
}

void loop () {
  Portal.handleClient();
}
```

In addition, the limit of the waiting time for connection attempts can be specified by the [AutoConnect::begin](api.md#begin) parameter too. The *timeout* parameter specified in [AutoConnect::begin](api.md#begin) takes precedence over [*AutoConnectConfig::beginTimeout*](apiconfig.md#begintimeout).

!!! note "The beginTimeout has an effect on handleClient"
    The [**beginTimeout**](apiconfig.md#begintimeout) value will be applied with [**handleClient**](api.md#handleclient) when requesting a connection from the captive portal and when attempting to reconnect with [**autoReconnect**](apiconfig.md#autoreconnect).
