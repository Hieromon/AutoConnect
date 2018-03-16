## Advanced usage

### <i class="fa fa-caret-right"></i> 404 handler

Registering the "not found" handler is a different way than ESP8266WebServer. The *onNotFound* of ESP8266WebServer does not work with AutoConnect. AutoConnect overrides *ESP8266WebServer::onNotFound* to handle a captive portal. To register "not found" handler, use [*AutoConnect::onNotFound*](api.md#onnotfound).

### <i class="fa fa-caret-right"></i> Auto save Credential

By default, AutoConnect saves the credentials of the established connection in EEPROM. You can disable it with the [**autoSave**](api.md#autosave) parameter specified by [**AutoConnectConfig**](api.md#autoconnectconfig).

```arduino hl_lines="3"
AutoConnect       Portal;
AutoConnectConfig Config;
Config.autoSave = AC_SAVECREDENTIAL_NEVER;
Portal.config(Config);
Portal.begin();
```

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
  pinMode(BUILTIN_LED, output);
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

You can output AutoConnect monitor messages to the **Serial**. A monitor message activation switch is in an include header file [AutoConnect.h](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnect.h) of library source. Define [**AC_DEBUG**](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnect.h#L27) macro to output monitor messages.

```cpp
#define AC_DEBUG
```

### <i class="fa fa-caret-right"></i> Disable the captive portal

It can also prevent the captive portal from starting even if the connection at the first *WiFi.begin* fails. In this case, the behavior of [*AutoConnect::begin*](api.md#begin) is the same as *WiFi.begin*.

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

So in the default state, the credential storage area used by AutoConnect conflicts with data owned by the user sketch. It will be destroyed together saved data in EEPROM by user sketch and AutoConnect each other. But you can move the storage area to avoid this.

The [**boundaryOffset**](api.md#boundaryoffset) in [**AutoConnectConfig**](api.md#autoconnectconfig-api) specifies the start offset of the credentials storage area. The default value is 0.

### <i class="fa fa-caret-right"></i> Refers the hosted ESP8266WebServer

Constructing an AutoConnect object variable without parameters then creates and starts an ESP8266WebServer inside the AutoConnect. This object variable could be referred by [*AutoConnect::host()*](api.md#host) function to access ESP8266WebServer instance as like below.

```arduino hl_lines="4"
AutoConnect Portal;

Portal.begin();
ESP8266WebServer& server = Portal.host();
server.send(200, "text/plain", "Hello, world");
```

!!! info "When host() is valid"
    The host() can be referred at after *AutoConnect::begin*.

### <i class="fa fa-caret-right"></i> Usage for automatically instantiated ESP8266WebServer

The sketch can handle URL requests using ESP8266WebServer that AutoConnect started internally. ESP8266WebServer instantiated dynamically by AutoConnect can be referred to by [*AutoConnect::host*](api.md#host) function. The sketch can use the '**on**' function, '**send**' function, '**client**' function and others by ESP8266WebServer reference of its return value.

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

!!! note "ESP8266WebServer function should be called after AutoConnect::begin"
    The sketch cannot refer to an instance of ESP8266WebServer until AutoConnect::begin completes successfully.

!!! warning "Do not use with ESP8266WebServer::begin"
    ESP8266WebServer is already running inside the AutoConnect.

### <i class="fa fa-caret-right"></i> Use with the [PageBuilder](https://github.com/Hieromon/PageBuilder) library

In ordinary, the URL handler will respond the request by sending some HTML. [PageBuilder](https://github.com/Hieromon/PageBuilder) library is HTML assembly aid. it can handle predefined HTML as like a template and simplify an HTML string assemble logic, and also the generated HTML send automatically.

An example sketch used with the PageBuilder as follows and it explains how it aids for the HTML generating. Details for [Github repository](https://github.com/Hieromon/PageBuilder).

<img src="../images/PageBuilder.png" style="width:640px;"/>

## Configuration functions

### <i class="fa fa-caret-right"></i> Configuration for Soft AP

AutoConnect will activate SoftAP at failed initial WiFi.Begin. It SoftAP settings are stored in [**AutoConnectConfig**](api.md#autoconnectconfig) as the following parameters. The sketch could be configured SoftAP using these parameters, refer the [AutoConnectConfig API](api.md#autoconnectconfig-api) for details.

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