In order for your sketches combined with AutoConnect to work as you intended, make them based on the right understanding of configuration settings and their effectiveness. AutoConnect includes the configuration settings using [AutoConnectConfig](apiconfig.md) class that adjusts controlling the behaviors for WiFi connection and the captive portal.  
For advanced usages, the configuration settings and the Sketch examples are followings:

- [AutoConnect WiFi connection control](#autoconnect-wifi-connection-control)
- [Captive portal control](#captive-portal-control)
- [Authentication settings](#authentication-settings)
- [Credential accesses](#credential-accesses)
- [Settings for customizing the AutoConnect page exterior](#settings-for-customizing-the-autoconnect-page-exterior)
- [Settings and controls involved for network and WiFi](#settings-and-controls-involved-for-network-and-wifi)
- [Other operation settings and controls](#other-operation-settings-and-controls)

## AutoConnect WiFi connection control

AutoConnect aims to connect the ESP module as a station to a WiFi access point. It is equipped with various APIs to keep a WiFi connection as possible while sketch running. The main APIs for maintaining a WiFi connection are [AutoConnect::begin](api.md#begin) and [AutoConnect::handleClient](api.md#handleclient). You can make sketches with flexible WiFi connection capability by properly using these two APIs and the settings by [AutoConnectConfig](apiconfig.md).

- [Automatic reconnect](#automatic-reconnect)
- [Automatic reconnect (Background)](#automatic-reconnect-background)
- [Configure WiFi channel](#configure-wifi-channel)
- [Connects depending on the WiFi signal strength](#connects-depending-on-the-wifi-signal-strength)
- [Detection of connection establishment to AP](#detection-of-connection-establishment-to-ap)
- [Match with known access points by SSID](#match-with-known-access-points-by-ssid)
- [Preserve WIFI_AP mode](#preserve-wifi_ap-mode)
- [Timeout settings for a connection attempt](#timeout-settings-for-a-connection-attempt)

### <i class="fa fa-caret-right"></i> Automatic reconnect

AutoConnect will change the WiFi mode depending on the situation. The [AutoConnect::begin](lsbegin.md) function starts WiFi mode in **STA** and starts the webserver if the connection is successful by the 1st-WiFi.begin. But if it will fail to connect with the least recently established access point, it will switch the WiFi mode to **AP_STA** and starts the DNS server to be able to launch a captive portal.

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
    
### <i class="fa fa-caret-right"></i> Automatic reconnect (Background)

Combining [**autoReconnect**](advancedusage.md#automatic-reconnect) with [*AutoConnectConfig::reconnectInterval*](apiconfig.md#reconnectinterval) allows you to periodically repeat connection attempts to known access points within [AutoConnect::handleClient](api.md#handleclient). This process is pseudo-asynchronous and does not block the Sketch process in the `loop()` function.

The reconnectInterval specifies the interval time to seek for known access points with saved credentials during the **handleClient** loop and attempt to connect to the AP.

```cpp hl_lines="7 8"
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
    AutoConnect 1.2.0 has not allowed attempts to automatically reconnect to a particular access point yet. It is recognized as a future enhancement. With the current automatic reconnection, one of the known access points is always selected.

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

!!! info "The effective range of the [reconnectInterval](apiconfig.md#reconnectinterval) depending on several values"
    The range of values that reconnectInterval can take is 0 to 255. (Actual seconds are from 0 to 255&#xd7;AUTOCONNECT_UNITTIME)<br>
    Reconnect behavior depends on the setting value. If it is 0, reconnection will work if the 1st-WiFi.begin in AutoConnect::begin fails and will suspend during the handleClient loop. If reconnectInterval is greater than 0, AutoConnect will attempt to reconnect both in AutoConnect::begin and during the handleClient loop.

### <i class="fa fa-caret-right"></i> Configure WiFi channel

Appropriately specifying the WiFi channel to use for ESP8266 and ESP32 is essential for a stable connection with the access point. AutoConnect remembers the WiFi channel with a credential of the access point once connected and reuses it.

The default channel when a captive portal starts and AutoConnect itself becomes an access point is the [*AutoConnectConfig::channel*](apiconfig.md#channel) member. If this channel is different from the channel of the access point you will attempt to connect, WiFi.begin may fail. The cause is that the ESP module shares the same channel in AP mode and STA mode. If the connection attempt is not stable, specifying a proper channel using AutoConnectConfig::channel may result in a stable connection.

### <i class="fa fa-caret-right"></i> Connects depending on the WiFi signal strength

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

### <i class="fa fa-caret-right"></i> Detection of connection establishment to AP

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
      Serial.printf("SoftAP:%s shutted down\n", WiFi.softAPSSID().c_str());
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

### <i class="fa fa-caret-right"></i> Match with known access points by SSID

By default, AutoConnect uses the **BSSID** to search for known access points. (Usually, it's the MAC address of the device) By using BSSID as the key to finding the WiFi network, AutoConnect can find even if the access point is hidden. However BSSIDs can change on some mobile hotspots, the BSSID-keyed seacrches may not be able to find known access points.  
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

### <i class="fa fa-caret-right"></i> Preserve WIFI_AP mode

Sketch using AutoConnect can open a gateway to the Internet by connecting to a WiFi router even through use Espressif's peculiar WiFi protocol (eg. [ESP-MESH](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/mesh.html) or [ESP-NOW](https://www.espressif.com/en/products/software/esp-now)). These specific communication protocols require to keeps AP + STA as the WiFi mode. That is, to apply these protocols, it needs to launch SoftAP by a sketch itself and then call [AutoConnect::begin](api.md#begin). But the default behavior of [AutoConnect::begin](api.md#begin) will turn off SoftAP always then it will unable to open a connection.

[*AutoConnectConfig::preserveAPMode*](apiconfig.md#preserveAPMode) setting maintains WIFI_AP mode without disabling SoftAP inside [AutoConnect::begin](api.md#begin). The Sketch can utilize the WiFi connection via AutoConnect with ESP-MESH and ESP-NOW protocol by enabling this option.

The following diagram quoted from the [ESP-MESH documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/mesh.html#mesh-building-a-network) that illustrates the typical topology of the MESH network. The module located at the Root Node bridges between the mesh network and the router by an application that handles two protocols, TCP/IP and ESP-MESH. Its SoftAP communicates with the internal mesh network as an interface of the mesh layer. On the other hand, STA performs station communication with the WiFi router as an interface of the TCP/IP layer. AutoConnect allows assists the connection between the router and the STA of the Root Node using [*AutoConnectConfig::preserveAPMode*](apiconfig.md#preserveapmode) and starting the SoftAP via Sketch separately.

<img src="https://docs.espressif.com/projects/esp-idf/en/latest/esp32/_images/mesh-bidirectional-data-stream.png">

Also in general, the Sketch should set **false** to [*AutoConnectConfig::autoRise*](apiconfig.md#autorise), **true** to [*AutoConnectConfig::immediateStart*](apiconfig.md#immediatestart) when applying to those protocols.

### <i class="fa fa-caret-right"></i> Timeout settings for a connection attempt

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

!!! note "The beginTime has an effect on handleClient"
    The [**beginTimeout**](apiconfig.md#begintimeout) value will be applied with [**handleClient**](api.md#handleclient) when requesting a connection from the captive portal and when attempting to reconnect with [**autoReconnect**](apiconfig.md#autoreconnect).

## Captive portal control

The default behavior of AutoConnect is to launch the captive portal if 1st-WiFi.begin attempting inside [AutoConnect::begin](api.md#begin) fails. You can change this default behavior through the [AutoConnectConfig](apiconfig.md) configuration settings mixture some processes of the Sketch.

- [Captive portal start control](#captive-portal-start-control)
- [Captive portal start detection](#captive-portal-start-detection)
- [Captive portal timeout control](#captive-portal-timeout-control)
- [Disable the captive portal](#disable-the-captive-portal)
- [Launch the captive portal on demand by external trigger](#launch-the-captive-portal-on-demand-by-external-trigger)
- [Launch the captive portal as demanded on WiFi disconnect](#launch-the-captive-portal-as-demanded-on-wifi-disconnect)
- [Shutting down the captive portal](#shutting-down-the-captive-portal)
- [Sketch execution during the captive portal loop](#sketch-execution-during-the-captive-portal-loop)

### <i class="fa fa-caret-right"></i> Captive portal start control

AutoConnect will launch the captive portal based on the [AutoConnectConfig](apiconfig.md) settings when the WiFi connection status will become to certain conditions. The settings involved in launching the captive portal that can be configured with [AutoConnectConfig](apiconfig.md) are [*AutoConnectConfig::autoRise*](apiconfig.md#autorise) and [*AutoConnectConfig::immediateStart*](apiconfig.md#immediatestart). Also, the [*AutoConnectConfig::retainPortal*](apiconfig.md#retainportal) controls the continuity of the captive portal, while it may also dynamically launch the captive portal from inside a [**handleClient**](api.md#handleclient) loop.

The [**autoRise**](apiconfig.md#autorise) allows or disallows the launch of the captive portal. AutoConnect launches the captive portal only if the autoRise is **true**. If the autoRise is **false**, the captive portal will not start even if the WiFi connection is lost.

Basically, the captive portal launch is subject to the result of 1st-WiFi.begin, but Sketch can control it. The Sketch can direct the following four actions by configuring AutoConnect with two parameters, the [**immediateStart**](apiconfig.md#immediatestart) and the [**autoRise**](apiconfig.md#autorise).

<table>
  <tr>
    <th rowspan="2" align="center">AutoConnectConfig<br>::immediateStart</th>
    <th colspan="2" align="center">AutoConnectConfig::autoRise</th>
  </tr>
  <tr>
    <td>true</td>
    <td>false</td>
  </tr>
  <tr>
    <td>true</td>
    <td>Skip 1st-WiFi.begin<br>ESP module becomes SoftAP and the captive portal starts immediately.<br></td>
    <td>Not attempt WiFi connection.<br>Only WebServer will start in STA mode.</td>
  </tr>
  <tr>
    <td>false</td>
    <td>Attempts WiFi connection in STA mode.<br>In some cases, the autoReconnect may restore the connection even if 1st-WiFiBeing fails.<br>If the connection is completely lost, the captive portal will be launched.<br><b>This is the default.</b></td>
    <td>Attempts WiFi connection in STA mode.<br>In some cases, the autoReconnect may restore the connection even if 1st-WiFiBeing fails.<br>ESP module stays in STA mode and WebServer will start.</td>
  </tr>
</table>

The [**retainPortal**](apiconfig.md#retainportal) specifies the continuity of the captive portal after [AutoConnect::begin](api.md#begin) and allows the captive portal would be launched after the Sketch execution has transitioned into the **loop** function. When AutoConnect establishes a WiFi connection while the captive portal with [AutoConnect::begin](api.md#begin), it stops the DNS server to close the captive portal leaving SoftAP is running. In this situation, if the Sketch loses the established WiFi connection while executing the loop function, it can reopen the captive portal.

However, this behavior can be redundant depending on the Sketch characteristics. In such a case, you can prevent to start captive portal during the `loop()` by setting [**autoRise**](apiconfig.md#autorise) to **false**.

### <i class="fa fa-caret-right"></i> Captive portal start detection

The captive portal will only be activated if 1st-WiFi::begin fails. Sketch can detect with the [AutoConnect::onDetect](api.md#ondetect) function that the captive portal has started. For example, the Sketch can be written like as follows that turns on the LED at the start captive portal.

```cpp hl_lines="3 13"
AutoConnect Portal;

bool startCP(IPAddress& ip) {
  digitalWrite(BUILTIN_LED, HIGH);
  Serial.println("C.P. started, IP:" + WiFi.localIP().toString());
  return true;
}

void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  Portal.onDetect(startCP);
  if (Portal.begin()) {
    digitalWrite(BUILTIN_LED, LOW);
  }
}

void loop() {
  Portal.handleClient();
}
```

### <i class="fa fa-caret-right"></i> Captive portal timeout control

Once AutoConnect has entered the captive portal state due to the above conditions, it will not exit until a WiFi connection can be established. (But that is the default behavior)

The Sketch can abort the [AutoConnect::begin](api.md#begin) by setting the captive portal timeout and returns control to Sketch. AutoConnect has two parameters for timeout control. One is a timeout value used when trying to connect to the specified AP. It behaves the same as general timeout control in connection attempt by WiFi.begin. This control is specified by the third parameter of [AutoConnect::begin](api.md#begin). The default value is macro defined by [**AUTOCONNECT_TIMEOUT**](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h#L103) in [`AutoConnectDefs.h`](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h) header file.

The other timeout control is for the captive portal itself. It is useful when you want to continue sketch execution with offline even if the WiFi connection is not possible. You can also combine it with the [**immediateStart**](#launch-the-captive-portal-on-demand-by-external-trigger) option to create sketches with high mobility.

The timeout of the captive portal is specified together with [*AutoConnectConfig::portalTimeout*](apiconfig.md#portaltimeout) as follows.

```cpp hl_lines="9"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

AutoConnect  portal;
AutoConnectConfig  config;

void setup() {
  config.portalTimeout = 60000;  // It will time out in 60 seconds
  portal.config(config);
  portal.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Some sketch code for the connected scene is here.
  }
  else {
    // Some sketch code for not connected scene is here.
  }
  portal.handleClient();
}
```
Also, if you want to stop AutoConnect completely when the captive portal is timed out, you need to call the [AutoConnect::end](api.md#end) function. It looks like the following code:

```cpp
bool acEnable;

void setup() {
  config.portalTimeout = 60000;  // It will time out in 60 seconds
  portal.config(config);
  acEnable = portal.begin();
  if (!acEnable) {
    portal.end();
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Some sketch code for the connected scene is here.
  }
  else {
    // Some sketch code for not connected scene is here.
  }
  if (acEnable) {
    portal.handleClient();
  }
}
```

There is another option related to timeout in AutoConnectConfig. It can make use of the captive portal function even after a timeout. The [*AutoConnectConfig::retainPortal*](apiconfig.md#retainportal) option will not stop the SoftAP when the captive portal is timed out. If you enable the ratainPortal option, you can try to connect to the AP at any time while continuing to sketch execution with offline even after the captive portal timed-out. Compared to the above code specified no option with the following example code, the captive portal will remain available even after a timeout without changing the logic of the Sketch.

```cpp hl_lines="10"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

AutoConnect  portal;
AutoConnectConfig  config;

void setup() {
  config.portalTimeout = 60000;  // It will time out in 60 seconds
  config.retainPortal = true;
  portal.config(config);
  portal.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Some sketch code for the connected scene is here.
  }
  else {
    // Some sketch code for not connected scene is here.
  }
  portal.handleClient();
}
```

### <i class="fa fa-caret-right"></i> Disable the captive portal

It can also prevent the captive portal from starting even if the connection at the 1st-WiFi.begin fails. In this case, [AutoConnect::begin](api.md#begin) behaves same as *WiFi.begin*.

For disabling the captive portal, [**autoRise**](apiconfig.md#autorise) sets to false with [AutoConnectConfig](apiconfig.md).

```cpp hl_lines="4"
AutoConnect       portal;
AutoConnectConfig acConfig;

acConfig.autoRise = false;
portal.config(acConfig);
portal.begin();
```

### <i class="fa fa-caret-right"></i> Launch the captive portal on demand by external trigger

The [default behavior](lsbegin.md) of [AutoConnect::begin](api.md#begin) gives priority to connect to the least recently established access point. In general, We expect this behavior in most situations, but will intentionally launch the captive portal on some occasion.

Here section describes how to launch on demand the captive portal, and suggests two templates that you can use to implement it.

1. Offline for usual operation, connect to WiFi with an external switch

    You can use this template if the ESP module does not connect to WiFi at an ordinal situation and need to establish by a manual trigger. In this case, it is desirable that AutoConnect not start until an external switch fires. This behavior is similar to the [WiFiManager's startConfigPortal](https://github.com/tzapu/WiFiManager#on-demand-configuration-portal) function.  
    [*AutoConnectConfig::immediateStart*](apiconfig.md#immediatestart) is an option to launch the portal by the SoftAP immediately without attempting 1st-WiFi.begin. Also, by setting the [*AutoConnectConfig::autoRise*](apiconfig.md#autorise) option to false, it is possible to suppress unintended automatic pop-ups of the portal screen when connecting to an ESP module SSID.  
    To implement this, execute AutoConnect::config within the **setup()** function as usual, and handle AutoConnect::begin inside the **loop()** function.

    ```cpp hl_lines="9"
    #define TRIGGER_PIN 5     // Trigger switch should be LOW active.
    #define HOLD_TIMER  3000

    AutoConnect       Portal;
    AutoConnectConfig Config;

    void setup() {
      pinMode(5, INPUT_PULLUP);
      Config.immediateStart = true;
      // Config.autoRise = false;   // If you don't need to automatically pop-up the portal when connected to the ESP module's SSID.
      Portal.config(Config);
    }

    void loop() {
      if (digitalRead(TRIGGER_PIN) == LOW) {
        unsigned long tm = millis();
        while (digitalRead(TRIGGER_PIN) == LOW) {
          yield();
        }
        // Hold the switch while HOLD_TIMER time to start connect.
        if (millis() - tm > HOLD_TIMER)
          Portal.begin();
      }

      if (WiFi.status() == WL_CONNECTED) {
        // Here, what to do if the module has connected to a WiFi access point
      }

      // Main process of your sketch

      Portal.handleClient();  // If WiFi is not connected, handleClient will do nothing.
    }
    ```

    !!! info "It will not be automatic reconnect"
        The above example does not connect to WiFi until TRIGGER\_PIN goes LOW. When TRIGGER\_PIN goes LOW, the captive portal starts and you can connect to WiFi. Even if you reset the module, it will not automatically reconnect.

2. Register new access points on demand

    The following template is useful for controlling the registration of unknown access points. In this case, the ESP module establishes a WiFi connection using WiFi.begin natively without relying on AutoConnect.  
    Known access point credentials are saved by AutoConnect, to the ESP module can use the saved credentials to handle WiFi.begin natively. This means that you can explicitly register available access points when needed, and the ESP module will not use unknown access points under normal situations.

    ```cpp
    AutoConnect* portal = nullptr;

    bool detectSwitch() {
      /*
      Returns true if an external switch to configure is active.
      */
    }

    bool connectWiFi(const char* ssid, const char* password, unsigned long timeout) {
      WiFi.mode(WiFi_STA);
      delay(100);
      WiFi.begin(ssid, password);
      unsigned long tm = millis();
      while (WiFi.status() != WL_CONNECTED) {
        if (millis() - tm > timeout)
          return false;
      }
      return true;
    }

    void setup() {
      AutoConnectCredential credt;
      station_config_t  config;
      for (int8_t e = 0; e < credt.entries(); e++) {
        credt.load(e, &config);
        if (connectWiFi(config.ssid, config.password, 30000))
          break;
      }
      if (WiFi.status() != WL_CONNECTED) {
        // Here, do something when WiFi cannot reach.
      }
    }

    void loop() {
      if (detectSwitch()) {
        AutoConnectConfig config;
        config.immediateStart= true;
        if (!portal) {
          portal = new AutoConnect;
        }
        portal->config(config);
        if (portal->begin()) {
          portal->end();
          delete portal;
          portal = nullptr;
        }
      }
      // Here, ordinary sketch logic.
    }
    ```

### <i class="fa fa-caret-right"></i> Launch the captive portal as demanded on WiFi disconnect

If the ESP module lost established WiFi connection during the loop of [**handleClient**](api.md#handleclient), you can prevent the ESP module from becoming an absolute standalone by starting the captive portal on demand.

When [**retainPortal**](apiconfig.md#retainportal) and [**autoRise**](apiconfig.md#autorise) settings are enabled, AutoConnect will launch SoftAP and start DNS when it detects a WiFi disconnect with the router during a handleClient loop. This behavior will occur caused by a WiFi disconnect detection even if the WiFi mode is STA.

**Since AutoConnect v1.2.0**, An improved [**retainPortal**](apiconfig.md#retainportal) option allows the captive portal to be restarted during a handleClient loop even if it is closed once in [AutoConnect::begin](api.md#begin). In this case, the Sketch execution stage has already transitioned into the loop function, so the Sketch process seems running concurrently with the captive portal loop. Its captive portal launched from inside handleClient does not block the execution of the Sketch, unlike that launched from [AutoConnect::begin](api.md#begin).

```cpp hl_lines="5 6"
AutoConnect       Portal;
AutoConnectConfig Config;

void setup() {
  Config.autoRise = true; // It's the default, no setting is needed explicitly.
  Config.retainPortal = true;
  Portal.config(Config);
  Portal.begin();
}

void loop() {
  Portal.handleClient();
}
```

!!! note "Need autoRise enabled"
    Need [*AutoConnectConfig::autoRise*](apiconfig.md#autorise) setting enabled to start the captive portal on demand during a handleClient loop.

Although the Sketch above specifies the [**retainPortal**](apiconfig.md#retainportal), it does not instruct starts the captive portal always. AutoConnect will try WiFi.begin once in [AutoConnect::begin](api.md#begin) unless the [**immediateStart**](apiconfig.md#immediatestart) option is specified. If AutoConnect fails the 1st-WiFi.begin, the captive portal will be launched at that point and the Sketch execution will stay within the [AutoConnect::begin](api.md#begin) function.

There is also a way to avoid starting the captive portal inside [AutoConnect::begin](api.md#begin) and start the captive portal according to the WiFi connection status after the Sketch execution transitions to a **handleClient loop**. Adjusting the timing of [**autoRise**](apiconfig.md#autorise) activation will allow the captive portal to start only from inside [AutoConnect::handleClient](api.md#handleclient) function.

```cpp hl_lines="5 6 9"
AutoConnect       Portal;
AutoConnectConfig Config;

void setup() {
  Config.retainPortal = true;
  Config.autoRise = false;  // Suppresses the launch of the captive portal from AutoConnect::begin.
  Portal.config(Config);    // Don't forget it.
  Portal.begin();
  Config.autoRise = true;   // Enable the launch of the captive portal.
  Portal.config(Config);    // Don't forget it.
}

void loop() {
  Portal.handleClient();
}
```

!!! note "Don't forget [**AutoConnect::config**](api.md#config)"
    The configuration cannot be reflected by only changing the member variables of AutoConnectConfig settings. Don't forget to run the [AutoConnect::config](api.md#config) function after changing the AutoConnectConfig member variables.

The [*retainPortal*](apiconfig.md#retainportal) option will keep SoftAP even if WiFi has established a connection as a client with the router.  Since it has the effect of stopping the DNS server, the phenomenon that the portal screen will not pop up automatically even though SoftAP is in action occur.<br>
This is a legacy behavior to ensure backward compatibility with up to AutoConnect v1.1.7. To stop SoftAP on escape from the on-demand captive portal, you need to explicitly call **[`WiFi.softAPdisconnect(true)`](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/soft-access-point-class.html#softapdisconnect)** and **[`WiFi.enableAP(false)`](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/generic-class.html?highlight=enableAP#other-function-calls)** in the Sketch.

```cpp hl_lines="25 26 27 28 29 30"
AutoConnect       Portal;
AutoConnectConfig Config;

bool  Connected;
unsigned long Elapsed;

void onConnect(IPAddress& clientIP) {
  Connected = true;
  Elapsed = millis();
}

void setup() {
  Config.retainPortal = true;
  Portal.config(Config);
  Portal.onConnect(onConnect);
  Portal.begin();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connected = false;
    Elapsed = millis();
  }

  if ((WiFi.getMode() & WIFI_AP) && Connected) {
    if (millis() - Elapsed > 30000) {
      WiFi.softAPdisconnect(true);
      WiFi.enableAP(false);
    }
  }

  // Actual sketch process is here.

  Portal.handleClient();
}
```

The above sketch will shutdown the SoftAP after elapsed time exceeds 30 seconds since the connection was re-established. Its logic is a bit tricky and does not stop SoftAP immediately after the connection established, which has several seconds delay. Doing it ensures that AutoConnect can send the HTML response.

!!!info "Stopped SoftAP is still displayed"
    After SoftAP stopped, there is a time lag before it disappears from the detected access points list on the client device.

### <i class="fa fa-caret-right"></i> Shutting down the captive portal

There is some complexity in the conditions under which AutoConnect shuts down the captive portal. Making a sketch that activates SoftAP only when needed can seem tedious.  
But there is a reason why. Even if AutoConnect could establish a connection using a captive portal, your cell phone as a client device would still have to keep connected to the ESP module-generated SoftAP in order to send the page for notifying the connection successful to a user. At that point, your client device that opened the captive portal still needs a connection with SoftAP.

What happens, after all, is as follows:

1. You made a connection to the access point such as WiFi router using the captive portal and took a successful page.
2. Your sketch will rush into the loop function and starts to works well, hooray!
3. Oops. Don't celebrate yet. I can see SoftAP ID on my cell phone. But the AutoConnect page never pops up automatically. Why?

Because, for the above reasons, we can not promptly shut down the SoftAP. (However, DNS will stop)

So, If you want to stop SoftAP after connecting to the access point using the captive portal, you need to implement the shutdown process with Sketch explicitly. A template of the basic sketch that can stop the SoftAP after the connection is the following:

```cpp hl_lines="5 6 7 8"
AutoConnect Portal;

void setup() {
  if (Portal.begin()) {
    if (WiFi.getMode() & WIFI_AP) {
      WiFi.softAPdisconnect(true);
      WiFi.enableAP(false);
    }
  }
  Portal.begin();
}

void loop() {
  Portal.handleClient();
}
```

### <i class="fa fa-caret-right"></i> Sketch execution during the captive portal loop

With [AutoConnect::begin](api.md#begin), once the captive portal is started without being able to connect to a known WiFi access point, control will not return to sketch until the WiFi connection is established or times out. This behavior helps to pin the ESP module's network participation as a WiFi client (that is, AutoConnect::begin is an alternative to WiFi.begin) but it can not rush into the loop function of the Sketch. Therefore, while the ESP module is in the captive portal state and waiting for a connection operation to the access point, the behavior of the Sketch will be restrained by the [escape conditions from AutoConnect::begin](lsbegin.md).

The **whileCaptivePortal exit** allows the Sketch to continue the process temporarily while the ESP module remains standalone and the captive portal is open. [AutConnect::whileCaptivePortal](api.md#whilecaptiveportal) function registers the user's sketch function to be called by [AutoConnect::begin](api.md#begin) or [AutoConnect::handleClient](api.md#handleclient) during the execution of the captive portal session loop.

The whileCaptivePortal exit can be registered by following: 

```cpp hl_lines="13"
AutoConnect portal;

bool whileCP(void) {
  bool  rc;
  // Here, something to process while the captive portal is open.
  // To escape from the captive portal loop, this exit function returns false.
  // rc = true;, or rc = false;
  return rc;
}

void setup() {
  ...
  portal.whileCaptivePortal(whileCP);
  portal.begin();
  ...
}
```

AutoConnect will open the captive portal in the [AutoConnect::begin](api.md#bdgin) and [AutoConnect::handleClient](api.md#handleclient) scenes, but the whileCaptive portal exit will be called repeatedly from [AutoConnect::begin](api.md#begin) until exits from it. The whileCaptivePortal exit will be called repeatedly while the captive portal is open until WiFi connected or times out occurs. In the Sketch, returning a **FALSE** value from the whileCaptivePortal function allows the control to escape from the captive portal loop even before the session elapsed time exceeds the limits.

## Authentication settings

The Sketch may use authentication to protect custom Web pages and prevent unauthorized access. AutoConnect has implemented HTTP authentication feature that can be applied to multiple scopes using the [authentication methods](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer#authentication) provided by the platform's WebServer library for ESP8266 or ESP32.[^1]

[^1]:ESP32 Arduino core has the authenticate method provided by the WebServer library, similar to that of the ESP8266. 

- [Applying HTTP authentication](#applying-http-authentication)
- [Applying HTTP authentication for Built-in OTA](#applying-http-authentication-for-built-in-ota)
- [Authentication with the captive portal state](#authentication-with-the-captive-portal-state)

### <i class="fa fa-caret-right"></i> Applying HTTP authentication

[*AutoConnectConfig::auth*](apiconfig.md#auth) setting allows the Sketch to HTTP authenticate with "[**BASIC**](https://tools.ietf.org/html/rfc2617#section-2)" or "[**DIGEST**](https://tools.ietf.org/html/rfc2617#section-3)" scheme. [*AutoConnectConfig::authScope*](apiconfig.md#authscope) specifies the scope covered by authentication which is the whole range for all pages of the Sketch, custom web pages, or AutoConnect pages. [*AutoConnectConfig::username*](apiconfig.md#username) and [*AutoConnectConfig::password*](apiconfig.md#password) specifies credential as user-id/password pairs.

The Sketch enables HTTP authentication with the [*AutoConnectConfig::auth*](apiconfig.md#auth) setting, also specifies the authentication scheme:

- **AC_AUTH_NONE**  
    AutoConnect pages and custom Web pages do not require authentication. Not protected from all HTTP access. This is the default.
- **AC_AUTH_DIGEST**  
    Protect AutoConnect pages and custom Web pages with DIGEST authentication.
- **AC_AUTH_BASIC**  
    Protect AutoConnect pages and custom Web pages with BASIC authentication.

Note that the authentication scope specified in [*AutoConnectConfig::authScope*](apiconfig.md#authscope) is different from the protection space shown by [**Realm**](https://tools.ietf.org/html/rfc7235#section-2.2) for HTTP authentication. AutoConnect assumes only one Realm and defines it as [**AUTOCONNECT_AUTH_REALM**](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h#L268) in [`AutoConnectDefs.h`](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h) header file. Instead, the Sketch will be able to expand or narrow the range of authentication by [*AutoConnectConfig::authScope*](apiconfig.md#authscope) setting, which can be either as follows:

- **AC_AUTHSCOPE_PORTAL**  
    Require authentication to access for all AutoConnect pages, including custom Web pages.
- **AC_AUTHSCOPE_AUX**  
    Require authentication to access for all custom Web pages, excepting AutoConnect pages. This is the Default.
- **AC_AUTHSCOPE_PARTIAL**  
    Authenticate only specific custom Web pages which are specified by [*AutoConnectAux::authentication*](apiaux.md#authentication) function or [JSON](acjson.md#auth).

Also, a credential used for authentication is specified in the Sketch using the [*AutoConnectConfig::username*](apiconfig.md#username) and [*AutoConnectConfig::password*](apiconfig.md#password) settings.[^2]

[^2]:The default user name and password for authentication inherits the setting of [AutoConnectConfig::apid](apiconfig.md#apid) and [AutoConnectConfig::psk](apiconfig.md#psk).

Here's a minimal Sketch with HTTP authentication for the custom Web page:

```cpp hl_lines="26 27 28 29"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

static const char PAGE_AUTH[] PROGMEM = R"(
{
  "uri": "/auth",
  "title": "Auth",
  "menu": true,
  "element": [
    {
      "name": "text",
      "type": "ACText",
      "value": "AutoConnect has authorized",
      "style": "font-family:Arial;font-size:18px;font-weight:400;color:#191970"
    }
  ]
}
)";

WebServerClass    server;
AutoConnect       portal(server);
AutoConnectConfig config;

void setup() {
  config.auth = AC_AUTH_DIGEST;
  config.authScope = AC_AUTHSCOPE_AUX;
  config.username = "user";
  config.password = "password";
  portal.config(config);
  portal.load(FPSTR(PAGE_AUTH));
  portal.begin();
}

void loop() {
  portal.handleClient();
}
```

If you want to authenticate only specific pages in a Sketch that handles multiple custom Web pages, set **AC_AUTHSCOPE_PARTIAL** to [*AutoConnectConfig::authScope*](apiconfig.md#authscope). Then, it specifies the authentication scheme with the [**auth**](acjson.md#auth) key in the JSON description of the page should be authenticated.  

AutoConnect determines which authentication method to use for custom Web pages (such as AutoConnectAux) based on its association with [*AutoConnectConfig::authScope*](apiconfig.md#authscope) setting. The table below shows which authentication scheme will be finally adopted. As shown in this table, the final authentication scheme depends on the [*AutoConnectConfig::authScope*](apiconfig.md#authscope) setting, and if **AC_AUTHSCOPE_PARTIAL** is specified it, [AutoConnectAux's authentication](apiaux.md#authentication) setting takes precedence over the [*AutoConnectConfig::auth*](apiconfig.md#auth) setting.

| AutoConnectConfig::authScope | Authentication scheme for the custom Web page |
|---|---|
| AC_AUTHSCOPE_PORTAL | Specified by AutoConnectConfig::auth |
| AC_AUTHSCOPE_AUX | Specified by AutoConnectConfig::auth |
| AC_AUTHSCOPE_PARTIAL | Specified by [AutoConnectAux::authentication](apiaux.md#authentication), The default values is **AC_AUTH_NONE**. |

Authentication designation for AutoConnectAux can also be specified by giving the following value to the **auth** key by the JSON description:

- "auth" : "basic"
- "auth" : "digest"
- "auth" : "none"

The following example Sketch has two custom Web pages, `Hello` and `Auth`. It applies authentication only to the `Auth` page by setting AC_AUTHSCOPE_PARTIAL to AutoConnectConfig::authScope.

```cpp hl_lines="26 45"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

static const char PAGE_HELLO[] PROGMEM = R"(
{
  "uri": "/hello",
  "title": "Hello",
  "menu": true,
  "element": [
    {
      "name": "text",
      "type": "ACText",
      "value": "Hello, word",
      "style": "font-family:Arial;font-size:18px;font-weight:400;color:#191970"
    }
  ]
}
)";

static const char PAGE_AUTH[] PROGMEM = R"(
{
  "uri": "/auth",
  "title": "Auth",
  "menu": true,
  "auth": "digest",
  "element": [
    {
      "name": "text",
      "type": "ACText",
      "value": "AutoConnect has authorized",
      "style": "font-family:Arial;font-size:18px;font-weight:400;color:#191970"
    }
  ]
}
)";

WebServerClass    server;
AutoConnect       portal(server);
AutoConnectConfig config;

void setup() {
  // It's a default value but has no meaning in the AC_AUTHSCOPE_PARTIAL setting.
  // config.auth = AC_AUTH_NONE;
  config.authScope = AC_AUTHSCOPE_PARTIAL;
  config.username = "user";
  config.password = "password";
  portal.config(config);
  portal.load(FPSTR(PAGE_HELLO));
  portal.load(FPSTR(PAGE_AUTH));
  portal.begin();
}

void loop() {
  portal.handleClient();
}
```

!!! info "PageBuilder v1.4.0 or later needed"
    [PageBuilder](https://github.com/Hieromon/PageBuilder) v1.4.0 or later is required to use HTTP authentication with AutoConnect. Also, v1.4.2 or later is required to eliminate SPIFFS, which is deprecated as a file system for ESP8266 module.

!!! warning "Can not use ESP32 arduino core 1.0.4 stable release"
    For ESP32, Arduino core 1.0.4 stable has a bug for HTTP authentication. The upstream of the master is recommended. (or use 1.0.5 later)

### <i class="fa fa-caret-right"></i> Applying HTTP authentication for Built-in OTA

[*AutoConnectConfig::auth*](apiconfig.md#auth) setting also affects the [built-in OTA](otaupdate.md) feature. **AC_AUTH_BASIC** or **AC_AUTH_DIGEST** setting allows Built-in OTA to authenticate with the [UPDATE](otabrowser.md#updates-with-the-web-browserupdated-wv115) page. This setting is valid even if [*AutoConnectConfig::authScope*](apiconfig.md#authscope) is **AC_AUTHSCOPE_PARTIAL**. That is if the AutoConnectConfig::auth setting is BASIC or DIGEST, authentication will be required for Built-in OTA.

### <i class="fa fa-caret-right"></i> Authentication with the captive portal state

When accessing the ESP module from an iOS or Android device in the captive portal state, the HTTP authentication framework is disabled in the OS of the client device. Even if the ESP module responds with a `401 unauthorized` with `WWW-Authenticate`, those client device OSs under the captive portal do not display the login dialog and deprive the user of the opportunity to enter their credentials. There will always be an unauthorized error.

AutoConnect's authentication operation based on HTTP (not HTTPS) depends on the OS of the client device, so in the captive portal state, most devices will unconditionally result in an authentication error. Therefore, the default authentication behavior of AutoConnect does not apply authentication in the captive portal state. (It will be ignored even if the AutoConnect setting is not AC_AUTH_NONE)

However, if you want to deny unauthorized access to the protected page even in the captive portal state, you can use the extension bit of [*AutoConnectConfig::authScope*](apiconfig.md#authScope). The **AC_AUTHSCOPE_WITHCP** flag allows AutoConnect to authentication in the captive portal state. It is set using a logical OR operator for the [*AutoConnectConfig::authScope*](apiconfig.md#authScope) setting and AutoConnect will enable authentication at the captive portal if the **AC_AUTHSCOPE_WITHCP** is ON.

```cpp hl_lines="4"
AutoConnectConfig config;
...
config.auth = AC_AUTH_DIGEST;
config.authScope = AC_AUTHSCOPE_AUX | AC_AUTHSCOPE_WITHCP;
...
```

## Credential accesses

AutoConnect automatically saves the credentials of the established WiFi connection according to the [*AutoConnectConfig::autoSave*](apiconfig.md#autosave) settings. The save destination differs depending on the type of ESP module. In the case of ESP8266, it is the EEPROM, and in the case of ESP32, it is the NVS ([Non-volatile storage](https://demo-dijiudu.readthedocs.io/en/latest/api-reference/storage/nvs_flash.html?highlight=non%20volatile%20storage)) partition implemented by the [Preferences](https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences/src) class.  
Sketches can access their stored credentials through a class that is independent of AutoConnect.

- [Access to saved credentials](#access-to-saved-credentials)
- [Autosave Credential](#autosave-credential)
- [Move the saving area of EEPROM for the credentials](#move-the-saving-area-of-eeprom-for-the-credentials)

### <i class="fa fa-caret-right"></i> Access to saved credentials

AutoConnect stores the credentials of the established WiFi connection in the flash of the ESP8266/ESP32 module and equips the class to access them from the Sketch. The Sketch can read, write, or erase the credentials using this class as the [AutoConnectCredential](credit.md#autoconnectcredential) individually. Refer to section [Saved credentials access](credit.md) for details.

!!! note "Where to store credentials in ESP32 with AutoConnect v1.0.0 or later"
    Since v1.0.0, credentials are stored in nvs of ESP32. AutoConnect v1.0.0 or later accesses the credentials area using the **Preferences** class with the arduino esp-32 core. So in ESP32, the credentials are not in the EEPROM, it is in the namespace **AC_CREDT** of the nvs. See [Saved credentials access](credit.md) for details.  
    In ESP8266, it is saved in EEPROM as is conventionally done.

### <i class="fa fa-caret-right"></i> Autosave Credential

By default, AutoConnect saves the credentials of the established connection to the flash. You can disable it with the [**autoSave**](apiconfig.md#autosave) parameter specified by [AutoConnectConfig](apiconfig.md).  
See the [Saved credentials access](credit.md) chapter for details on accessing stored credentials.

```cpp hl_lines="3"
AutoConnect       Portal;
AutoConnectConfig Config;
Config.autoSave = AC_SAVECREDENTIAL_NEVER;
Portal.config(Config);
Portal.begin();
```

!!! note "Credentials storage location"
    The location where AutoConnect saves credentials depends on the module type and the AutoConnect library version, also arduino-esp32 core version.
    <table>
      <tr>
        <th rowspan="2" style="vertical-align:bottom">AutoConnect</th>
        <th rowspan="2" style="vertical-align:bottom">Arduino core<br>for ESP8266</th>
        <th colspan="2" style="text-align:center;vertical-align:bottom">Arduino core for ESP32</th>
      </tr>
      <tr>
        <th style="text-align:center;vertical-align:bottom">1.0.2 earlier</td>
        <th style="text-align:center;vertical-align:bottom">1.0.3 later</td>
      </tr>
      <tr>
        <td>v0.9.12 earlier</td>
        <td rowspan="2" style="text-align:center;vertical-align:middle">EEPROM</td>
        <td>EEPROM (partition)</td>
        <td>Not supported</td>
      </tr>
      <tr>
        <td>v1.0.0 later</td>
        <td>Preferences (nvs)<p>(Can be used EEPROM with turning off AUTOCONNECT_USE_PREFERENCES macro)</p></td>
        <td>Preferences (nvs)</td>
      </tr>
    </table>

### <i class="fa fa-caret-right"></i> Move the saving area of EEPROM for the credentials

By default, the credentials saving area is occupied from the beginning of EEPROM area. [ESP8266 Arduino core document](http://arduino-esp8266.readthedocs.io/en/latest/filesystem.html?highlight=eeprom#flash-layout) says that:

> The following diagram illustrates flash layout used in Arduino environment:
>
> ```powershell
> |--------------|-------|---------------|--|--|--|--|--|
> ^              ^       ^               ^     ^
> Sketch    OTA update   File system   EEPROM  WiFi config (SDK)
> ```

and

> EEPROM library uses one sector of flash located [just after the SPIFFS](http://arduino-esp8266.readthedocs.io/en/latest/libraries.html?highlight=SPIFFS#eeprom).

Also, in ESP32 arduino core 1.0.2 earlier, the placement of the EEPROM area of ESP32 is described in the [partition table](https://github.com/espressif/arduino-esp32/blob/master/tools/partitions/default.csv). So in the default state, the credential storage area used by AutoConnect conflicts with data owned by the user sketch. It will be destroyed together saved data in EEPROM by user sketch and AutoConnect each other. But you can move the storage area to avoid this.

The [**boundaryOffset**](apiconfig.md#boundaryoffset) in [AutoConnectConfig](apiconfig.md) specifies the start offset of the credentials storage area. The default value is 0.

!!! info "The boundaryOffset ignored with AutoConnect v1.0.0 later on ESP32 arduino core 1.0.3 later"
    For ESP32 arduino core 1.0.3 and later, AutoConnect will store credentials to Preferences in the nvs. Since it is defined as the namespace dedicated to AutoConnect and separated from the area used for user sketches. Therefore, the [boundaryOffset](apiconfig.md#boundaryoffset) is ignored with the combination of AutoConnect v1.0.0 or later and the arduino-esp32 1.0.3 or later.

The [*AutoConnectConfig::boundaryOffset*](apiconfig.md#boundaryoffset) setting allows AutoConnect to write its data to EEPROM while preserving custom configuration data. Similarly, when a Sketch writes its own data to EEPROM, it must preserve the data written by AutoConnect.  
The EEPROM library for ESP8266 erases the entire flash sector when it writes to any part of the sector. Therefore, when writing data to EEPROM with a sketch that handles the custom data, it is necessary to call `EEPROM.begin` using a total amount of a custom data size and the saved credentials size.  
The following code shows how to use the [*AutoConnect::getEEPROMUsedSize*](api.md#geteepromusedsize) function to store custom configuration settings in EEPROM without conflicting with AutoConnect's use of that storage.

```cpp hl_lines="13 21"
AutoConnect       portal;
AutoConnectConfig config;

// Defines the custom data should be stored in EEPROM.
typedef struct {
  char  data1[8];
  char  data2[8];
  char  data3[8];
} EEPROM_CONFIG_t;
EEPROM_CONFIG_t eepromConfig;
...
// Declares to reserve the EEPROM_CONFIG_t area for a Sketch using.
config.boundaryOffset = sizeof(eepromConfig);
portal.config(config);
...
strcpy(eepromComfig.data1, "data1");
strcpy(eepromComfig.data2, "data2");
strcpy(eepromComfig.data3, "data3");

// Use getEEPROMUsedSize to access the EEPROM with the appropriate region size.
EEPROM.begin(portal.getEEPROMUsedSize());
EEPROM.put<EEPROM_CONFIG_t>(0, eepromConfig);
EEPROM.commit();
EEPROM.end();
...
```

## Settings for customizing the AutoConnect page exterior

The design of various AutoConnect web pages is basically inflexible. Its appearance and layout do not have many customizable visual aspects.<br>
Although, you can customize the following look of AutoConnect web pages:

- [AutoConnect menu colorize](colorized.md) (See [Appendix](colorized.md))
- [Capture the legacy web pages as items into the menu](#capture-the-legacy-web-pages-as-items-into-the-menu)
- [Change menu labels](#change-menu-labels)
- [Change menu title](#change-menu-title)

### <i class="fa fa-caret-right"></i> Capture the legacy web pages as items into the menu

You can embed the ordinary page processed by the ESP8266WebServer request handler as an item into the AutoConnect menu. AutoConnect can capture the legacy web pages for ESP8266WebServer or WebServer of ESP32 and extends the menu containing these items.  
In ordinary, the Sketch registers the request handler for the page depending on URI using the [ESP8266WebServer::on](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer#client-request-handlers) function. AutoConnect allows Sketch to bundle the registered legacy page into a menu. the Sketch is able to include its URI to a menu item using [AutoConnect::append](api.md#append) function that creates internally an [**AutoConnectAux**](acintro.md) depended on its URI and integrates into the menu.

The following code has a mixture of both AutoConnectAux and the legacy web page. An AutoConnectAux page is menued automatically with the [AutoConnect::join](api.md#join) or [AutoConnect::load](api.md#load) function. Similarly, a legacy page is integrated by the [AutoConnect::append](api.md#append) function.

```cpp hl_lines="26 35"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

ESP8266WebServer server;
AutoConnect      portal(server);

// Definitions of AutoConnectAux page
static const char PAGE[] PROGMEM = R"(
{
  "title": "PAGE",
  "uri": "/page",
  "menu": true,
  "element": [
    {
      "name": "cap",
      "type": "ACText",
      "value": "This is a custom web page."
    }
  ]
}
)";

void setup() {
  // The Web page handler located to /hello
  server.on("/hello", [](){
    server.send(200, "text/html", String(F(
"<html>"
"<head><meta name='viewport' content='width=device-width,initial-scale=1.0'></head>"
"<body><h2>Hello, world</h2></body>"
"</html>"
    )));
  });

  portal.append("/hello", "HELLO");  // Adds an item as HELLO into the menu
  portal.load(FPSTR(PAGE));               // Load AutoConnectAux custom web page
  portal.begin();
}

void loop() {
  portal.handleClient();
}
  
```

<span style="display:block;margin-left:auto;margin-right:auto;width:284px;height:462px;border:1px solid lightgrey;"><img data-gifffer="images/addmenu.gif" data-gifffer-height="460" data-gifffer-width="282" /></span>

The [AutoConnect::append](api.md#append) function also has the third parameter that directly specifies the request handler. It has similar efficacy to calling the append and `ESP8266WebSever::on` at once. [^4]

[^4]: However, the pages registered this way remain legacy. Therefore, the AutoConnect menu bar is not appeared. 

```cpp
portal.append("/hello", "HELLO", [](){
  server.send(200, "text/html", String(F(
"<html>"
"<head><meta name='viewport' content='width=device-width,initial-scale=1.0'></head>"
"<body><h2>Hello, world</h2></body>"
"</html>"
  )));
});
```

For more details, see section [Attach the menus](menuize.md) of Examples page.

!!! note "An instance of ESP8266WebServer/WebServer is needed"
    When calling the append function with request handler parameters, an instance of the WebServer as the registration destination must exist.  
    AutoConnect can instantiate and host a WebServer internally, but in that case, the point in time to call the [AutoConnect::append](api.md#append) function with a request handler parameter must be after [AutoConnect::begin](api.md#begin).

### <i class="fa fa-caret-right"></i> Change the menu labels

You can change the label text for each menu item but cannot change them at run time. There are two ways to change the label text, both of which require coding the label literal.

1. Overwrite the label literal of library source code directly.
  
    You can change the label of the AutoConnect menu item by rewriting the default label literal in [AutoConnectLabels.h](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectLabels.h) macros. However, changing menu items literal influences all the Sketch's build scenes.
   
    ```cpp
    #define AUTOCONNECT_MENULABEL_CONFIGNEW   "Configure new AP"
    #define AUTOCONNECT_MENULABEL_OPENSSIDS   "Open SSIDs"
    #define AUTOCONNECT_MENULABEL_DISCONNECT  "Disconnect"
    #define AUTOCONNECT_MENULABEL_RESET       "Reset..."
    #define AUTOCONNECT_MENULABEL_UPDATE      "Update"
    #define AUTOCONNECT_MENULABEL_HOME        "HOME"
    #define AUTOCONNECT_MENULABEL_DEVINFO     "Device info"
    #define AUTOCONNECT_BUTTONLABEL_RESET     "RESET"
    #define AUTOCONNECT_BUTTONLABEL_UPDATE    "UPDATE"
    ```

    !!! note "**build_flags** with PlatformIO will no effect"
        The mistake that many people make is to use PlatformIO's build_flags to try to redefine any literal at compile time.<br>
        The AutoConnect library statically contains the label literals which are embedded as binary data when compiling the library code. The label literals will not change without compiling the library source.<br>
        And PlatformIO is a build system. Library sources will not be compiled unless AutoConnectLabels.h is updated.

2. Change the label literals for each Arduino project

    Another way to change the label literal is to provide a header file that defines the label literals, as mentioned in [Appendix](changelabel.md#change-the-items-label-text). You can also use this method to display label text and fixed text in the local language on the AutoConnect page. See [Change the item's label text](changelabel.md#change-the-items-label-text) section for details.

### <i class="fa fa-caret-right"></i> Change menu title

Although the default menu title is **AutoConnect**, you can change the title by setting [*AutoConnectConfig::title*](apiconfig.md#title). To set the menu title properly, you must set before calling [AutoConnect::begin](api.md#begin).

```cpp hl_lines="6 7"
AutoConnect       Portal;
AutoConnectConfig Config;

void setup() {
  // Set menu title
  Config.title = "FSBrowser";
  Portal.config(Config);
  Portal.begin();
}
```

Executing the above sketch will rewrite the menu title for the **FSBrowser** as the below.

<div style="float:left;width:40%;height:470px;overflow:hidden;"><img src="images/fsbmenu.png"></div>
<img style="margin-left:70px;width:40%;height:470px;" src="images/fsbmenu_expand.png">

## Settings and controls involved for network and WiFi

AutoConnect allows you to make the static configuration of SoftAP at runtime. Its configuration includes the identification information on the network such as the IP address and the access path of the Web page handled by AutoConnect etc. In addition, the mDNS service allows SoftAP to be accessed by hostname on the local network.  
The configuration settings for the network that can be set by AutoConnect is as follows:

- [404 handler](#404-handler)
- [Assign user sketch's home path](#assign-user-sketchs-home-path)
- [Change SSID and Password for SoftAP](#change-ssid-and-password-for-softap)
- [Combination with mDNS](#combination-with-mdns)
- [Make SSID of SoftAP unique](#make-ssid-of-softap-unique)
- [Relocate the AutoConnect home path](#relocate-the-autoconnect-home-path)
- [SoftAP access point IP settings](#softap-access-point-ip-settings)
- [Static IP assignment as a client](#static-ip-assignment-as-a-client)
- [Station hostname](#station-hostname)

### <i class="fa fa-caret-right"></i> 404 handler

AutoConnect cannot allow the Sketch registers the **"Not-found"** handler (404-handler) to the ESP8266WebServer natively. AutoConnect traps Not-found handler of the ESP8266WebServer for its own page processing. If the Sketch overrides the Not-found handler, AutoConnect will miss the opportunity to control the HTTP session and becomes unresponsive to the menu.  
Registering the Not-found handler is a different method than for ESP8266WebServer, use [AutoConnect::onNotFound](api.md#onnotfound). This restriction applies to the WebServer for ESP32 as well.

### <i class="fa fa-caret-right"></i> Assign user sketch's home path

**HOME** for returning to the user's sketch homepage will display at the bottom of the AutoConnect menu. It could be set using the [AutoConnect::home](api.md#home) function.

<img src="images/menu_home.png" />

The Sketch HOME path is closely related to the [bootUri](apiconfig.md#booturi) that specifies the access path on module restart. AutoConnect has the following three parameters concerning control the URIs:

- **AUTOCONNECT_URI**  
    The **ROOT** URI of AutoConnect. It is defined in [`AutoConnectDefs.h`](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h#L93) file and is assigned to [AutoConnect statistics screen](menu.md#where-the-from) by default.
- [**AutoConnectConfig::homeUri**](apiconfig.md#homeuri)  
    It is the hyperlink of listed on the AutoConnect menu as **HOME**.
- [**AutoConnectConfig::bootUri**](apiconfig.md#booturi)  
    Which page appears at the captive portal, AUTOCONNECT_URI, or the homeUri. Its page will pop up automatically when you visit the captive portal.

| The definition of **HOME** | Behavior | Specified by | Default value | Possible value |
|---|---|---|---|---|
| **ROOT** of AutoConnect | Default for AC_ONBOOTURI_ROOT | `#define AUTOCONNECT_URI` in `AutoConnectDefs.h` |  `/_ac` | URI string |
| **HOME** for Application-specific | Listed on the **menu list** as **HOME**<br>Also, It may be linked from the **menu title** and is **redundant** with the HOME menu item.<br>eg. Case of bootURI = AC_ONBOOTURI_HOME | AutoConnectConfig::homeURI | `/` | URI string |
| Which page **loads** at the boot time, ROOT or HOME | Appears after module reboot by **RESET** button with AutoConnect menu | AutoConnectConfig::bootURI | AC_ONBOOTURI_ROOT | AC_ONBOOTURI_HOME |
| Which page **appears** at the captive portal, ROOT or HOME | Auto pop-up | AutoConnectConfig::bootURI | AC_ONBOOTURI_ROOT | AC_ONBOOTURI_HOME |

### <i class="fa fa-caret-right"></i> Change SSID and Password for SoftAP

An **esp8266ap** is default SSID name for SoftAP of captive portal and password is **12345678** for ESP8266. Similarly, **esp32ap** and **12345678** for ESP32. You can change both by setting [apid](apiconfig.md#apid) and [psk](apiconfig.md#psk).

```cpp hl_lines="5 6"
AutoConnect portal;
AutoConnectConfig config;

void setup() {
  config.apid = "ap_portal";
  config.psk  = "new_password";
  portal.config(config);
  portal.begin();
}
```

Also, you can specify the SSID, password for SoftAP with the constructor of the AutoConnectConfig as below.

```cpp hl_lines="2"
AutoConnect portal;
AutoConnectConfig config("ap_portal", "new_password");

void setup() {
  portal.config(config);
  portal.begin();
}
```

You can also assign no password to SoftAP launched as a captive portal. Assigning a null string as `String("")` to [*AutoConnectConfig::psk*](apiconfig.md#psk) does not require a password when connecting to SoftAP.  
But this method is not recommended. The broadcast radio of SSID emitted from SoftAP will leak and reach several tens of meters.

### <i class="fa fa-caret-right"></i> Combination with mDNS

With [mDNS library](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS), you can access to ESP8266 by name instead of IP address after connection. The Sketch can start the MDNS responder after [AutoConnect::begin](api.md#begin).

```cpp hl_lines="8 9"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
AutoConnect Portal;

void setup() {
  if (Portal.begin()) {
    if (MDNS.begin("esp8266")) {
      MDNS.addService("http", "tcp", 80);
    }
  }
}

void loop() {
  Portal.handleClient();
}
```

### <i class="fa fa-caret-right"></i> Make SSID of SoftAP unique

You can change SoftAP's SSID and password programmatically when the captive portal starts up. By using chip specific ID of esp8266/esp32 you can make SSID of SoftAP unique. SSID and password for SoftAP is [*AutoConnectConfig::apid*](apiconfig.md#apid) and [*AutoConnectConfig::psk*](apiconfig.md#psk).

```cpp
AutoConnect       portal;
AutoConnectConfig acConfig;

acConfig.apid = "ESP-" + String(ESP.getChipId(), HEX);
acConfig.psk = YOUR_PASSWORD;
portal.config(acConfig);
portal.begin();
```

!!! hint "Obtaining chip ID for ESP32"
    `acConfig.apid = "ESP-" + String((uint32_t)(ESP.getEfuseMac() >> 32), HEX);`

### <i class="fa fa-caret-right"></i> Relocate the AutoConnect home path

A home path of AutoConnect is **/\_ac** by default. You can access from the browser with http://IPADDRESS/\_ac. You can change the home path by revising [**AUTOCONNECT_URI**](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h#L69) macro in [`AutoConnectDefs.h`](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h) header file.

```cpp
#define AUTOCONNECT_URI         "/_ac"
```

### <i class="fa fa-caret-right"></i> SoftAP access point IP settings

AutoConnect will activate [SoftAP](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/soft-access-point-class.html#soft-access-point-class) at failed the 1st-WiFi.begin. Its SoftAP settings are stored in AutoConnectConfig as the following parameters. The Sketch could be configured SoftAP using these parameters, refer the [AutoConnectConfig API](apiconfig.md) for details.

| AutoConnectConfig member | Settings for | Defined symbol | Initial value |
|--------------------------|--------------|----------------|---------------|
| [**apip**](apiconfig.md#apip) | SoftAP IP address | AUTOCONNECT_AP_IP | 172.217.28.1 |
| [**gateway**](apiconfig.md#gateway) | Gateway IP address | AUTOCONNECT_AP_GW | 172.217.28.1 |
| [**netmask**](apiconfig.md#netmask) | Subnet mask for the SoftAP | AUTOCONNECT_AP_NM | 255.255.255.0 |
| [**channel**](apiconfig.md#channel) | WiFi channel for the SoftAP | AUTOCONNECT_AP_CH | 1 |
| [**hidden**](apiconfig.md#hidden) | Hide the SoftAP | | false |

### <i class="fa fa-caret-right"></i> Static IP assignment as a client

It is possible to assign a static IP Address to ESP8266/ESP32 in STA mode.[^3] By default DHCP is enabled and it becomes the IP address assigned by the DHCP server with **WiFi.begin**.

[^3]:Static IP address assignment is available from version 0.9.3.

These settings are made via [AutoConnectConfig](apiconfig.md) as in the case of SoftAP settings. To assign a static IP to ESP8266/ESP32 with WIFI\_STA, the following parameters are required:

| AutoConnectConfig member | Settings for | Initial value |
|--------------------------|--------------|---------------|
| [**staip**](apiconfig.md#staip) | Station IP address | 0.0.0.0 |
| [**staGateway**](apiconfig.md#stagateway) | Gateway address for the station | 0.0.0.0 |
| [**staNetmask**](apiconfig.md#stanetmask) | Subnet mask for the the station | 0.0.0.0 |
| [**dns1**](apiconfig.md#dns1) | Primary DNS server IP address | 0.0.0.0 |
| [**dns2**](apiconfig.md#dns2) | Secondary DNS server IP address | 0.0.0.0 |

The above parameters must be set using [AutoConnect::config](apiconfig.md) prior to [AutoConnect::begin](apiconfig.md#begin) call as following:

```cpp
AutoConnect        portal;
AutoConnectConfig  Config;
Config.staip = IPAddress(192,168,1,10);
Config.staGateway = IPAddress(192,168,1,1);
Config.staNetmask = IPAddress(255,255,255,0);
Config.dns1 = IPAddress(192,168,1,1);
portal.config(Config);
portal.begin();
```

### <i class="fa fa-caret-right"></i> Station hostname

[*AutoConnectConfig::hostName*](apiconfig.md#hostname) assigns a station DHCP hostname to the ESP module. The hostname must satisfy [RFC952](https://tools.ietf.org/html/rfc952) compliant and meet the following restrictions:

- Up to 24 characters
- Only the alphabet (a-z, A-Z), digits (0-9), minus sign (-)
- No '-' as last character

## Other operation settings and controls

You can adjust the AutoConnect behave at run-time using [AutoConnectConfig](apiconfig.md). AutoConnectConfig is a class that has only AutoConnect configuration settings. You define the behavior of AutoConnect using AutoConnectConfig member variables and give it to AutoConnect via the [AutoConnect::config](api.md#config) function. This allows Sketch controls the following:

- [Built-in OTA update](#built-in-ota-update-feature)
- [Debug print](#debug-print)
- [Ticker for WiFi status](#ticker-for-wifi-status)
- [Refers the hosted ESP8266WebServer/WebServer](#refers-the-hosted-esp8266webserverwebserver)
- [Usage for automatically instantiated ESP8266WebServer/WebServer](#usage-for-automatically-instantiated-esp8266webserverwebserver)
- [Use with the [PageBuilder](https://github.com/Hieromon/PageBuilder) library](#use-with-the-pagebuilder-library)

!!! note "AutoConnect::config before AutoConnect::begin"
    [AutoConnect::config](api.md#config) must be executed before [AutoConnect::begin](api.md#begin).

### <i class="fa fa-caret-right"></i> Built-in OTA update feature

AutoConnect features a built-in OTA function to update ESP module firmware. You can easily make the Sketch that equips OTA and able to operate with the AutoConnect menu.

<span style="display:block;margin-left:auto;margin-right:auto;width:284px;height:462px;border:1px solid lightgrey;"><img data-gifffer="images/webupdate.gif" data-gifffer-height="460" data-gifffer-width="282" /></span>

[*AutoConnectConfig::ota*](apiconfig.md#ota) specifies to import the [built-in OTA update class](otabrowser.md) into the Sketch.  
See the [Updates with the Web Browser](otabrowser.md) chapter for details.

### <i class="fa fa-caret-right"></i> Debug print

You can output AutoConnect monitor messages to the **Serial**. A monitor message activation switch is in an include header file [`AutoConnectDefs.h`](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h) of library source. Define [**AC_DEBUG**](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h#L14) macro to output the monitor messages.[^5]

[^5]:The source code placement of common macros for AutoConnect since v0.9.7 has changed.

```cpp
#define AC_DEBUG
```

### <i class="fa fa-caret-right"></i> Refers the hosted ESP8266WebServer/WebServer

Constructing an AutoConnect object variable without parameters then creates and starts an ESP8266WebServer/WebServer inside the AutoConnect. This object variable could be referred by [AutoConnect::host](api.md#host) function to access ESP8266WebServer/WebServer instance as like below.

```cpp hl_lines="4"
AutoConnect Portal;

Portal.begin();
ESP8266WebServer& server = Portal.host();
server.send(200, "text/plain", "Hello, world");
```

!!! info "When host() is valid"
    The host() can be referred at after [AutoConnect::begin](api.md#begin).

### <i class="fa fa-caret-right"></i> Ticker for WiFi status

Flicker signal can be output from the ESP8266/ESP32 module according to WiFi connection status. If you connect the LED to the signal output pin, you can know the WiFi connection status during behavior inside AutoConnect::begin through the LED blink.

[*AutoConnectConfig::ticker*](apiconfig.md#ticker) option specifies flicker signal output. The following sketch is an example of flashing the active-high LED connected to pin #16 according to WiFi connection during the AutoConnect::begin.

```cpp
AutoConnect        portal;
AutoConnectConfig  Config;
Config.ticker = true;
config.tickerPort = 16;
Config.tickerOn = HIGH;
portal.config(Config);
portal.begin();
```

The AutoConnect ticker indicates the WiFi connection status in the following three flicker patterns:

- Short blink: The ESP module stays in AP_STA mode.
- Short-on and long-off: No STA connection state. (i.e. WiFi.status != WL_CONNECTED)
- No blink: WiFi connection with access point established and data link enabled. (i.e. WiFi.status = WL_CONNECTED)

The flicker cycle length is defined by some macros in [`AutoConnectDefs.h`](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h#L180) header file.

```cpp
#define AUTOCONNECT_FLICKER_PERIODAP  1000 // [ms]
#define AUTOCONNECT_FLICKER_PERIODDC  (AUTOCONNECT_FLICKER_PERIODAP << 1) // [ms]
#define AUTOCONNECT_FLICKER_WIDTHAP   96  // (8 bit resolution)
#define AUTOCONNECT_FLICKER_WIDTHDC   16  // (8 bit resolution)
```

- **AUTOCONNECT_FLICKER_PERIODAP**:  
  Assigns a flicker period when the ESP module stays in AP_STA mode.
- **AUTOCONNECT_FLICKER_PERIODDC**:  
  Assigns a flicker period when WiFi is disconnected.
- **AUTOCONNECT_FLICKER_WIDTHAP** and **AUTOCONNECT_FLICKER_WIDTHDC**:  
  Specify the duty rate for each period [ms] in 8-bit resolution.

!!! note "Ticker during OTA"
    The LED blinking will always be a short blinking during the update via OTA, regardless of the definition of the flicker cycle.

[*AutoConnectConfig::tickerPort*](apiconfig.md#tickerport) specifies a port that outputs the flicker signal. If you are using an LED-equipped ESP module board, you can assign a LED pin to the tick-port for the WiFi connection monitoring without the external LED. The default pin is arduino valiant's **LED\_BUILTIN**. You can refer to the Arduino IDE's variant information to find out which pin actually on the module assign to **LED\_BUILTIN**.[^6]

[^6]: It's defined in the `pins_arduino.h` file, located in the sub-folder named **variants** wherein Arduino IDE installed folder.

[*AutoConnectConfig::tickerOn*](apiconfig.md#tickeron) specifies the active logic level of the flicker signal. This value indicates the active signal level when driving the ticker. For example, if the LED connected to tickPort lights by LOW, the tickerOn is **LOW**. The logic level of LED_BUILTIN for popular modules are as follows:

module | Logic level | LED_BUILTIN Pin | Arduino alias
----|----|:---:|----
NodeMCU V1.0 | Active-low | 16 | D0
WEMOS D1 mini | Active-low | 2 | D4
SparkFun ESP8266 Thing | Active-high | 5 |
Adafruit Feather HUZZAH ESP8266 | Active-low | 0 |
NodeMCU 32s | Active-high | 2 | T2
LOLIN32 Pro | Active-low | 5 | SS
SparkFun ESP32 Thing | Active-high | 5
Adafruit Feather HUZZAH32 | Active-high | 13 | A12

### <i class="fa fa-caret-right"></i> Usage for automatically instantiated ESP8266WebServer/WebServer

The Sketch can handle URL requests using ESP8266WebServer or WebServer that AutoConnect started internally. ESP8266WebServer/WebServer instantiated dynamically by AutoConnect can be referred to by [AutoConnect::host](api.md#host) function. The Sketch can use the '**on**' function, '**send**' function, '**client**' function and others by ESP8266WebServer/WebServer reference of its return value.

```cpp hl_lines="8 9 13 14 20 21 27"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

AutoConnect       Portal;

void handleRoot() {
  ESP8266WebServer& IntServer = Portal.host();
  IntServer.send(200, "text/html", "Hello, world");
}

void handleNotFound() {
  ESP8266WebServer& IntServer = Portal.host();
  IntServer.send(404, "text/html", "Unknown.");
}

void setup() {
  bool r = Portal.begin();
  if (r) {
    ESP8266WebServer& IntServer = Portal.host();
    IntServer.on("/", handleRoot);
    Portal.onNotFound(handleNotFound);    // For only onNotFound.
  }
}

void loop() {
  Portal.host().handleClient();
  Portal.handleRequest();
  /* or following one line code is equ.
  Portal.handleClient();
  */
}
```

!!! note "ESP8266WebServer/WebServer function should be called after AutoConnect::begin"
    The Sketch cannot refer to an instance of ESP8266WebServer/WebServer until AutoConnect::begin completes successfully.

!!! warning "Do not use with ESP8266WebServer::begin or WebServer::begin"
    ESP8266WebServer/WebServer is already running inside the AutoConnect.

### <i class="fa fa-caret-right"></i> Use with the [PageBuilder](https://github.com/Hieromon/PageBuilder) library

In ordinary, the URL handler will respond to the request from the client by sending some HTML. It will dynamically generate the HTML to respond to based on the sensing data etc. for the changing scene, but it contains elements of variable values in the middle of the HTML fixed string. Therefore, sketches tend to be in a tangled that repeats the logic for data handling and string splicing in turn, which tends to be less readable and maintainable.

[PageBuilder](https://github.com/Hieromon/PageBuilder) library is an HTML assembly aid. it can handle predefined HTML like the template and simplify an HTML string assemble logic, and also the generated HTML send automatically.

An example sketch used with the PageBuilder as follows and it explains how it aids for the HTML generating. Details for [Github repository](https://github.com/Hieromon/PageBuilder).

<img src="images/PageBuilder.png" style="width:640px;"/>

<script>
  window.onload = function() {
    Gifffer();
  };
</script>
