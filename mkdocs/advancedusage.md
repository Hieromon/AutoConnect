## Advanced usage

### <i class="fa fa-caret-right"></i> 404 handler

Registering the "not found" handler is a different way than ESP8266WebServer/WebServer. The *onNotFound* of ESP8266WebServer/WebServer does not work with AutoConnect. AutoConnect overrides *ESP8266WebServer::onNotFound*/*WebServer::onNotFound* to handle a captive portal. To register "not found" handler, use [*AutoConnect::onNotFound*](api.md#onnotfound).

### <i class="fa fa-caret-right"></i> Automatic reconnect

When the captive portal is started, SoftAP starts and the STA is disconnected. The current SSID setting memorized in ESP8266 will be lost but then the reconnect behavior of ESP32 is somewhat different from this.  
The [WiFiSTAClass::disconnect](https://github.com/espressif/arduino-esp32/blob/a0f0bd930cfd2d607bf3d3288f46e2d265dd2e11/libraries/WiFi/src/WiFiSTA.h#L46) function implemented in the arduino-esp32 has extended parameters than the ESP8266's arduino-core. The second parameter of WiFi.disconnect on the arduino-esp32 core that does not exist in the [ESP8266WiFiSTAClass](https://github.com/esp8266/Arduino/blob/7e1bdb225da8ab337373517e6a86a99432921a86/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.cpp#L296) has the effect of deleting the currently connected WiFi configuration and its default value is "false". On the ESP32 platform, even if WiFi.disconnect is executed, WiFi.begin() without the parameters in the next turn will try to connect to that AP. That is, automatic reconnection is implemented in arduino-esp32 already. Although this behavior appears seemingly competent, it is rather a disadvantage in scenes where you want to change the access point each time. When explicitly disconnecting WiFi from the Disconnect menu, AutoConnect will erase the AP connection settings saved by arduino-esp32 core. AutoConnect's automatic reconnection is a mechanism independent from the automatic reconnection of the arduino-esp32 core.  
If the [**autoReconnect**](api.md#autoreconnect) option of the [**AutoConnectConfig**](api.md#autoconnectconfig-api) class is enabled, it automatically attempts to reconnect to the disconnected past access point. When the autoReconnect option is specified, AutoConnect will not start SoftAP immediately if the first WiFi.begin fails. It will scan WiFi signal and the same connection information as the detected BSSID is stored in EEPROM as AutoConnect's credentials, explicitly apply it with WiFi.begin and rerun.

```arduino hl_lines="3"
AutoConnect       Portal;
AutoConnectConfig Config;
Config.autoReconnect = true;
Portal.config(Config);
Portal.begin();
```

An autoReconnect option is available to *AutoConnect::begin* without SSID and pass passphrase.

### <i class="fa fa-caret-right"></i> Auto save Credential

By default, AutoConnect saves the credentials of the established connection in EEPROM. You can disable it with the [**autoSave**](api.md#autosave) parameter specified by [**AutoConnectConfig**](api.md#autoconnectconfig).

```arduino hl_lines="3"
AutoConnect       Portal;
AutoConnectConfig Config;
Config.autoSave = AC_SAVECREDENTIAL_NEVER;
Portal.config(Config);
Portal.begin();
```

!!! note "In ESP32, the credentials for AutoConnect are not in NVS"
    The credentials used by AutoConnect are not saved in NVS on ESP32 module. ESP-IDF saves the WiFi connection configuration to NVS, but AutoConnect stores it on the eeprom partition. You can find the partition table for default as [default.csv](https://github.com/espressif/arduino-esp32/blob/master/tools/partitions/default.csv)

### <i class="fa fa-caret-right"></i> Captive portal start detection

The captive portal will only be activated if the first *WiFi::begin* fails. Sketch can detect with the [*AutoConnect::onDetect*](api.md#ondetect) function that the captive portal has started. For example, the sketch can be written like as follows that turns on the LED at the start captive portal.

```arduino hl_lines="3 13"
AutoConnect Portal;

bool startCP(IPAddress ip) {
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

### <i class="fa fa-caret-right"></i> Combination with mDNS

With [mDNS library](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS), you can access to ESP8266 by name instead of IP address after connection. The sketch can start the MDNS responder after [*AutoConnect::begin*](api.md#begin).

```arduino hl_lines="8 9"
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

### <i class="fa fa-caret-right"></i> Debug print

You can output AutoConnect monitor messages to the **Serial**. A monitor message activation switch is in an include header file [AutoConnect.h](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnect.h) of library source. Define [**AC_DEBUG**](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnect.h#L27) macro to output the monitor messages.

```cpp
#define AC_DEBUG
```

### <i class="fa fa-caret-right"></i> Disable the captive portal

It can also prevent the captive portal from starting even if the connection at the first *WiFi.begin* fails. In this case, [*AutoConnect::begin*](api.md#begin) behaves same as *WiFi.begin*.

For disabling the captive portal, [**autoRise**](api.md#autorise) sets to false with [**AutoConnectConfig**](api.md#autoconnectconfig-api).

```arduino hl_lines="4"
AutoConnect       portal;
AutoConnectConfig acConfig;

acConfig.autoRaise = false;
portal.config(acConfig);
portal.begin();
```

### <i class="fa fa-caret-right"></i> Move the saving area of EEPROM for the credentials

By default, the credentials saving area is occupied from the beginning of EEPROM area. [ESP8266 Arduino core document](http://arduino-esp8266.readthedocs.io/en/latest/filesystem.html?highlight=eeprom#flash-layout) says that:


> The following diagram illustrates flash layout used in Arduino environment:

<div class="highlight">
<pre><span></span>
  <span class="o">|--------------|-------|---------------|--|--|--|--|--|</span>
  <span class="o">^</span>              <span class="o">^</span>       <span class="o">^</span>               <span class="o">^</span>     <span class="o">^</span>
  <span class="n">Sketch</span>    <span class="n">OTA</span> <span class="n">update</span>   <span class="n">File</span> <span class="n">system</span>   <span class="n"><span class="highlighted">EEPROM</span></span>  <span class="n">WiFi</span> <span class="n">config</span> <span class="p">(</span><span class="n">SDK</span><span class="p">)</span>
</pre>
</div>

and

> EEPROM library uses one sector of flash located [just after the SPIFFS](http://arduino-esp8266.readthedocs.io/en/latest/libraries.html?highlight=SPIFFS#eeprom).

Also, the placement of the EEPROM area of ESP32 is described in the [partition table](https://github.com/espressif/arduino-esp32/blob/master/tools/partitions/default.csv). So in the default state, the credential storage area used by AutoConnect conflicts with data owned by the user sketch. It will be destroyed together saved data in EEPROM by user sketch and AutoConnect each other. But you can move the storage area to avoid this.

The [**boundaryOffset**](api.md#boundaryoffset) in [**AutoConnectConfig**](api.md#autoconnectconfig-api) specifies the start offset of the credentials storage area. The default value is 0.

### <i class="fa fa-caret-right"></i> Refers the hosted ESP8266WebServer/WebServer

Constructing an AutoConnect object variable without parameters then creates and starts an ESP8266WebServer/WebServer inside the AutoConnect. This object variable could be referred by [*AutoConnect::host()*](api.md#host) function to access ESP8266WebServer/WebServer instance as like below.

```arduino hl_lines="4"
AutoConnect Portal;

Portal.begin();
ESP8266WebServer& server = Portal.host();
server.send(200, "text/plain", "Hello, world");
```

!!! info "When host() is valid"
    The host() can be referred at after *AutoConnect::begin*.

### <i class="fa fa-caret-right"></i> Usage for automatically instantiated ESP8266WebServer/WebServer

The sketch can handle URL requests using ESP8266WebServer or WebServer that AutoConnect started internally. ESP8266WebServer/WebServer instantiated dynamically by AutoConnect can be referred to by [*AutoConnect::host*](api.md#host) function. The sketch can use the '**on**' function, '**send**' function, '**client**' function and others by ESP8266WebServer/WebServer reference of its return value.

```arduino hl_lines="8 9 13 14 20 21 27"
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
    The sketch cannot refer to an instance of ESP8266WebServer/WebServer until AutoConnect::begin completes successfully.

!!! warning "Do not use with ESP8266WebServer::begin or WebServer::begin"
    ESP8266WebServer/WebServer is already running inside the AutoConnect.

### <i class="fa fa-caret-right"></i> Use with the [PageBuilder](https://github.com/Hieromon/PageBuilder) library

In ordinary, the URL handler will respond the request by sending some HTML. [PageBuilder](https://github.com/Hieromon/PageBuilder) library is HTML assembly aid. it can handle predefined HTML as like a template and simplify an HTML string assemble logic, and also the generated HTML send automatically.

An example sketch used with the PageBuilder as follows and it explains how it aids for the HTML generating. Details for [Github repository](https://github.com/Hieromon/PageBuilder).

<img src="../images/PageBuilder.png" style="width:640px;"/>

## Configuration functions

### <i class="fa fa-caret-right"></i> Configuration for Soft AP

AutoConnect will activate SoftAP at failed the first *WiFi.begin*. It SoftAP settings are stored in [**AutoConnectConfig**](api.md#autoconnectconfig) as the following parameters. The sketch could be configured SoftAP using these parameters, refer the [AutoConnectConfig API](api.md#autoconnectconfig-api) for details.

- IP address of SoftAP activated.
- Gateway IP address.
- Subnet mask.
- SSID for SoftAP.
- Password for SoftAP.
- Channel.
- Hidden attribute.
- Auto save credential.
- Offset address of the credentials storage area in EEPROM.
- Length of start up time after reset.
- Automatic starting the captive portal.
- Auto reset after connection establishment.
- Home URL of the user sketch application.

!!! note "AutoConnect::config before AutoConnect::begin"
    *AutoConnect::config* must be executed before *AutoConnect::begin*.

### <i class="fa fa-caret-right"></i> Assign user sketch's home path

"**HOME**" for returning to the user's sketch homepage is displayed at the bottom of the AutoConnect menu. It could be set using the [*AutoConnect::home*](api.md#home) function.

<img src="../images/menu_home.png" />

### <i class="fa fa-caret-right"></i> Relocate the AutoConnect home path

A home path of AutoConnect is **/\_ac** by default. You can access from the browser with http://IPADDRESS/\_ac. You can change the home path by revising [**AUTOCONNECT_URI**](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnect.h#L58) macro in the include header file as [AutoConnect.h](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnect.h).

```cpp
#define AUTOCONNECT_URI         "/_ac"
```

### <i class="fa fa-caret-right"></i> Static IP assignment [^1]

It is also possible to assign static IP Address to ESP8266/ESP32 in STA mode. By default DHCP is enabled and it becomes the IP address assigned by the DHCP server with *WiFi.begin*.

To assign a static IP to ESP8266/ESP32 with WIFI\_MODE\_STA, the following parameters are required:

- IP address.
- Gateway address.
- Subnet mask.
- Primary DNS server.
- Secondary DNS server. (optional)

The above parameters must be executed before *AutoConnect::begin* as [arguments of AutoConnectConfig](api.md#staip).

```arduino
AutoConnect        portal;
AutoConnectConfig  Config;
Config.staip = IPAddress(192,168,1,10);
Config.staGateway = IPAddress(192,168,1,1);
Config.staNetmask = IPAddress(255,255,255,0);
Config.dns1 = IPAddress(192,168,1,1);
portal.config(Config);
portal.begin();
```

[^1]:Static IP address assignment is available from version 0.9.3.
