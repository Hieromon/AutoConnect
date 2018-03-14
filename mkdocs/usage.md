## Simple usage

### <i class="fa fa-edit"></i> Embed to the sketches

How embed the AutoConnect to the sketches you have. Most simple approach to applying AutoConnect for the existing sketches, follow the below steps.

<img src="../images/BeforeAfter.svg" />

<ul class="ulsty-edit" style="list-style:none;">
  <li>Insert <code class="codehilite"><span class="cp">#include</span> <span class="cpf">&lt;AutoConnect.h&gt;</span></code> to behind of <code class="codehilite"><span class="cp">#include</span> <span class="cpf">&lt;ESP8266WebServer.h&gt;</span></code>.</li>
  <li>Insert <code class="codehilite"><span class="na">AutoConnect</span> <em>PORTAL(WEBSERVER);</em></code> to behind of <code class="codehilite"><span class="na">ESP8266WebServer</span> <em>WEBSERVER;</em></code> declaration.<sup id="fnref:1"><a class="footnote-ref" href="#fn:1" rel="footnote">1</a></sup></li>
  <li>Remove <code class="codehilite">WiFi.<span class="na">begin</span>(<em>SSID</em>,<em>PSK</em>)</code> and the subsequent logic for the connection status check.</li>
  <li>Replace <code class="codehilite"><em>WEBSERVER</em>.<span class="na">begin</span><span class="p">()</span></code> to <code class="codehilite"><em>PORTAL</em>.<span class="na">begin</span><span class="p">()</span></code>.<sup id="fnref:2"><a class="footnote-ref" href="#fn:2" rel="footnote">2</a></sup></li>
  <li>Replace <code class="codehilite"><em>WEBSERVER</em>.<span class="na">handleClient</span><span class="p">()</span></code> to <code class="codehilite"><em>PORTAL</em>.<span class="na">handleClient</span><span class="p">()</span></code>.<sup id="fnref:3"><a class="footnote-ref" href="#fn:3" rel="footnote">3</a></sup></li>
  <li>If the connection checks logic is needed, you can check the return value according to <code class="codehilite"><em>PORTAL</em>.<span class="na">begin</span><span class="p">()</span></code> with <code class="codehilite">true</code> or <code class="codehilite">false</code>.</li>
</ul>

[^1]:
Each *VARIABLE* conforms to the actual declaration in the sketches.  

[^2]:
WiFi SSID and Password can be specified AutoConnect::begin() too.

[^3]:
Replacement the **handleClient** method is not indispensable. AutoConnect can still connect with the captive portal as it is ESP8266WebServer::handleClient. But it can **not valid AutoConnect menu**.

## Basic usage

### <i class="fa fa-caret-right"></i> Basic logic sequence for the user sketches

#### 1. A typical logic sequence

!!! note ""
    1. <strong>Include headers,</strong> `ESP8266WebServer.h` and `AutoConnect.h`  
    2. <strong>Declare ESP8266WebServer variable.</strong>  
    3. <strong>Declare AutoConnect variable.</strong>  
    4. <strong>Implements the URL handler *function()*.</strong>  
    5. <strong>setup()</strong>  
       5.1 <strong>Sets URL handler *function()* to ESP8266WebServer by</strong>`ESP8266WebServer::on`<strong>.</strong>  
       5.2 <strong>Starts </strong>`AutoConnect::begin()`<strong>.</strong>  
       5.3 <strong>Check connection status.</strong>  
    6. <strong>loop()</strong>  
       6.1 <strong>Invokes </strong>`AutoConnect::handleClient()`<strong>,<br>or invokes </strong>`ESP8266WebServer::handleClient()`<strong> then </strong>`AutoConnect::handleRequest()`<strong>.</strong>  
       6.2 <strong>Do the process for actual sketch.</strong>  

#### 2. Declare AutoConnect object

[Two options](#esp8266webserver-hosted-or-parasitic) are available for [AutoConnect constructor](api.md#constructors).

```arduino
AutoConnect VARIABLE(&ESP8266WebServer);
```
or

```arduino
AutoConnect VARIABLE;
```

- **Parameter with ESP8266WebServer variable:** An ESP8266WebServer object variable must be declared in the sketch. AutoConnect uses its variable for handling the [AutoConnect menu](menu.md).

- **With no parameter:** The sketch does not declare ESP8266WebServer object. In this case, AutoConnect allocates an instance of the ESP8266WebServer internally and the logic sequence of the sketch is somewhat different as the above. To register a URL handler function by *ESP8266WebServer::on* should be performed after [*AutoConnect::begin*](api.md#begin).

#### 3. No need WiFI.begin(...)

AutoConnect performs *WiFi.begin* for establishing a connection with WLAN internally. There is no need for a general process to establish a connection with *WiFi.begin* in a sketch.

#### 4. Alternate ESP8266WebServer::begin()

[*AutoConnect::begin*](api.md#begin) internally executes *ESP8266WebServer::begin* too and it starts DNS server to behave as a captive portal. So the sketch does not need to call *ESP8266WebServer::begin*.

!!! info "Why DNS Server Starts"
    AutoConnect traps the detection of captive portals and directs them to the AutoConnect menu to achieve a connection with the WLAN interactively. In order to trap, it temporarily responds SoftAP address to all DNS queries. When the connection with the WLAN is successfully established, the DNS server will stop.

#### 5. AutoConnect::begin with SSID and Password

SSID and Password can also specify by [*AutoConnect::begin*](api.me#begin). ESP8266 uses provided SSID and Password explicitly. If the connection false with specified SSID with Password then a captive portal is activated. SSID and Password are not present, ESP8266 SDK will attempt to connect using the still effectual SSID and password. Usually, it succeeds.

#### 6. Use ESP8266WebServer::on to handle URL

AutoConnect is designed to coexist with the process for handling the web pages by user sketches. The page processing function which will send an HTML to the client invoked by the "*on::ESP8266WebServer*" function is the same as when using ESP8266WebServer natively.

#### 7. Use either ESP8266WebServer::handleClient() or AutoConnect::handleClient()

Both classes member function name is the same: *handleClient*, but behavior is different. Using the AutoConnect embedded along with ESP8266WebServer::handleClient has limitations. Refer to the below section for details. 

### <i class="fa fa-caret-right"></i> ESP8266WebServer hosted or parasitic

The interoperable process with an ESP8266WebServer depends on the parameters of the [AutoConnect constructor](api.md#constructors).

Declaration parameter | Use ESP8266WebServer::handleClient | Use AutoConnect::handleClient
----|----|---
None | AutoConnect menu not available.<br>host() is needed. | AutoConnect menu available.<br>host() is needed.
Reference to ESP8266WebServer | AutoConnect menu not available.<br>host() not necessary. | AutoConnect menu available.<br>host() not necessary.

- **By declaration for the AutoConnect variable with no parameter**: The ESP8266WebServer instance is hosted by AutoConnect automatically then the sketches use [*AutoConnect::host*](api.md#host) as API to get it after [*AutoConnect::begin*](api.md#begin) performed.

- **By declaration for the AutoConnect variable with the reference of ESP8266WebServer**: AutoConnect will use it. The sketch can use it is too.

- **In use ESP8266WebServer::handleClient()**: AutoConnect menu can be dispatched but not works normally. It is necessary to call [*AutoConnect::handleRequest*](api.md#void-handlerequest) after *ESP8255WebServer::handleClient* invoking.

- **In use [AutoConnect::handleClient()](api.md#void-handleclient)**: The handleClient() process and the AutoConnect menu is available without calling *ESP8266WebServer::handleClient*.

!!! info "Why AutoConnect::handleRequest is needed when using ESP8266::handleClient"
    The AutoConnect menu function may affect WiFi connection state. It follows that the menu process must execute outside *ESP8266WebServer::handleClient*.  
    [*AutoConnect::handleClient*](api.md#void-handleclient) is equivalent *ESP8266WebServer::handleClient* included [*AutoConnect::handleRequest*](api.md#void-handlerequest).

## Advanced usage

### <i class="fa fa-caret-right"></i> 404 handler

Registering the "not found" handler is a different way than ESP8266. The *onNotFound* of ESP8266WebServer does not work with AutoConnect. AutoConnect overrides *ESP8266WebServer::onNotFound* to handle a captive portal. To register "not found" handler, use [*AutoConnect::onNotFound*](api.md#onnotfound).

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

The captive portal will only be activated if the first *WiFi::begin* fails. Sketch can detect with the [*AutoConnect::onDetect*](api.md#ondetect) funciton that the captive portal has started. For example, the sketch can be written like as follows that turns on the LED at the start captive portal.

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

!!! note "AutoConnect::config before AutoConnect::begin"
    *AutoConnect::config* must be executed before *AutoConnect::begin*.

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

By default, the credentials saving area is occupied from the beginning of EEPROM. [ESP8266 Arduino core document](http://arduino-esp8266.readthedocs.io/en/latest/filesystem.html?highlight=eeprom#flash-layout) says that:


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

So in the default state, the credential storage area used by AutoConnect conflicts with data area owned by the user sketch. It will be destroyed together saved data in EEPROM by user sketch and AutoConnect each other. But you can move the storage area to avoid this.

The [**saveOffset**](api.md#saveoffset) in [**AutoConnectConfig**](api.md#autoconnectconfig-api) specifies the start offset of the credentials storage area. The default value is 0.

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

### <i class="fa fa-caret-right"></i> Assign user sketch's home path

"**HOME**" for returning to the user's sketch homepage is displayed at the bottom of the AutoConnect menu. It could be set using the [*AutoConnect::home*](api.md#home) function.

<img src="../images/menu_home.png" />

### <i class="fa fa-caret-right"></i> Relocate the AutoConnect home path

A home path of AutoConnect is **\_ac** by default. You can access from the browser with http://IPADDRESS/\_ac. You can change the home path by revising [**AUTOCONNECT_URI**](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnect.h#L58) macro in the include header file as [AutoConnect.h](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnect.h).

```cpp
#define AUTOCONNECT_URI         "/_ac"
```