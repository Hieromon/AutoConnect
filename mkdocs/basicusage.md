## Simple usage

### <i class="fa fa-edit"></i> Embed to the Sketches

How embed the AutoConnect to the Sketches you have. Most simple approach to applying AutoConnect for the existing Sketches, follow the below steps. The below Sketch is for ESP8266. For ESP32, replace ```ESP8266WebServer``` with ```WebServer``` and ```ESP8266WiFi.h``` with ```WiFi.h``` respectively.

<img src="images/BeforeAfter.svg" />

:fa-edit: Insert <code class="highlight"><span class="cp">#include</span> <span class="cpf">&lt;AutoConnect.h&gt;</span></code> to behind of <code class="highlight"><span class="cp">#include</span> <span class="cpf">&lt;ESP8266WebServer.h&gt;</span></code>.

:fa-edit: Insert <code class="highlight"><span class="na">AutoConnect</span> <em>PORTAL(WEBSERVER);</em></code> to behind of <code class="highlight"><span class="na">ESP8266WebServer</span> <em>WEBSERVER;</em></code> declaration.<sup id="fnref:1"><a class="footnote-ref" href="#fn:1" rel="footnote">1</a></sup>

:fa-edit: Remove <code class="highlight">WiFi.<span class="na">begin</span>(<em>SSID</em>,<em>PSK</em>)</code> and the subsequent logic for the connection status check.

:fa-edit: Replace <code class="highlight"><em>WEBSERVER</em>.<span class="na">begin</span><span class="p">()</span></code> to <code class="highlight"><em>PORTAL</em>.<span class="na">begin</span><span class="p">()</span></code>.<sup id="fnref:2"><a class="footnote-ref" href="#fn:2" rel="footnote">2</a></sup>

:fa-edit: Replace <code class="highlight"><em>WEBSERVER</em>.<span class="na">handleClient</span><span class="p">()</span></code> to <code class="highlight"><em>PORTAL</em>.<span class="na">handleClient</span><span class="p">()</span></code>.<sup id="fnref:3"><a class="footnote-ref" href="#fn:3" rel="footnote">3</a></sup>

:fa-edit: If the connection checks logic is needed, you can check the return value according to <code class="highlight"><em>PORTAL</em>.<span class="na">begin</span><span class="p">()</span></code> with <code class="highlight">true</code> or <code class="highlight">false</code>.

[^1]:
Each *VARIABLE* conforms to the actual declaration in the Sketches.  

[^2]:
WiFi SSID and Password can be specified AutoConnect::begin() too.

[^3]:
Replacement the **handleClient** method is not indispensable. AutoConnect can still connect with the captive portal as it is ESP8266WebServer::handleClient. But it can **not valid AutoConnect menu**.

## Basic usage

### <i class="fa fa-caret-right"></i> Basic logic sequence for the user Sketches

#### 1. A typical logic sequence

!!! note ""
    1. <strong>Include headers,</strong> `ESP8266WebServer.h`/`WebServer.h` and `AutoConnect.h`  
    2. <strong>Declare an ESP8266WebServer variable for ESP8266 or a WebServer variable for ESP32.</strong>  
    3. <strong>Declare an AutoConnect variable.</strong>  
    4. <strong>Implement the URL handlers provided for the </strong>`on`<strong> method of ESP8266WebServer/WebServer with the *function()*.</strong>  
    5. <strong>setup()</strong>  
       5.1 <strong>Sets URL handler the *function()* to ESP8266WebServer/WebServer by</strong>`ESP8266WebServer::on`<strong>/</strong>`WebServer::on`<strong>.</strong>  
       5.2 <strong>Starts </strong>`AutoConnect::begin()`<strong>.</strong>  
       5.3 <strong>Check WiFi connection status.</strong>  
    6. <strong>loop()</strong>  
       6.1 <strong>Do the process for actual Sketch.</strong>  
       6.2 <strong>Invokes </strong>`AutoConnect::handleClient()`<strong>, or invokes </strong>`ESP8266WebServer::handleClient()`<strong>/</strong>`WebServer::handleClient`<strong> then </strong>`AutoConnect::handleRequest()`<strong>.</strong>  

#### 2. Declare AutoConnect object

[Two options](#esp8266webserver-hosted-or-parasitic) are available for [AutoConnect constructor](api.md#constructors).

```cpp
AutoConnect VARIABLE(&ESP8266WebServer);  // For ESP8266
AutoConnect VARIABLE(&WebServer);         // For ESP32
```
or

```cpp
AutoConnect VARIABLE;
```

- **The parameter with an ESP8266WebServer/WebServer variable:** An ESP8266WebServer/WebServer object variable must be declared. AutoConnect uses its variable to handles the [AutoConnect menu](menu.md).

- **With no parameter:** the Sketch does not declare ESP8266WebServer/WebServer object. In this case, AutoConnect allocates an instance of the ESP8266WebServer/WebServer internally. The logic sequence of the Sketch is somewhat different as the above. To register a URL handler function by *ESP8266WebServer::on* or *WebServer::on* should be performed after [*AutoConnect::begin*](api.md#begin).

#### 3. No need WiFI.begin(...)

AutoConnect internally performs *WiFi.begin* to establish a WiFi connection. There is no need for a general process to establish a connection using *WiFi.begin* with a Sketch code.

#### 4. Alternate ESP8266WebServer::begin() and WebServer::begin()

[*AutoConnect::begin*](api.md#begin) executes *ESP8266WebServer::begin*/*WebServer::begin* internally too and it starts the DNS server to behave as a Captive portal. So it is not needed to call *ESP8266WebServer::begin*/*WebServer::begin* in the Sketch.

!!! info "Why DNS Server starts"
    AutoConnect traps the detection of the captive portal and achieves a connection with the WLAN interactively by the AutoConnect menu. It responds SoftAP address to all DNS queries temporarily to trap. Once a WiFi connection establishes, the DNS server contributed by AutoConnect stops.

#### 5. AutoConnect::begin with SSID and Password

SSID and Password can also specify by [*AutoConnect::begin*](api.md#begin). ESP8266/ESP32 uses provided SSID and Password explicitly. If the connection false with specified SSID with Password then a captive portal is activated. SSID and Password are not present, ESP8266 SDK will attempt to connect using the still effectual SSID and password. Usually, it succeeds.

#### 6. Use ESP8266WebServer::on and WebServer::on to handle URL

AutoConnect is designed to coexist with the process for handling the web pages by user Sketches. The page processing function which will send an HTML to the client invoked by the "*on::ESP8266WebServer*" or the "*on::WebServer*" function is the same as when using ESP8266WebServer/WebServer natively.

#### 7. Use either ESP8266WebServer::handleClient()/WebServer::handleClient() or AutoConnect::handleClient()

Both classes member function name is the same: *handleClient*, but the behavior is different. Using the AutoConnect embedded along with ESP8266WebServer::handleClient/WebServer::handleClient has limitations. Refer to the below section for details. 

### <i class="fa fa-caret-right"></i> ESP8266WebServer/WebServer hosted or parasitic

The interoperable process with an ESP8266WebServer/WebServer depends on the parameters of the [AutoConnect constructor](api.md#constructors).

Declaration parameter for the constructor | Use ESP8266WebServer::handleClient or WebServer::handleClient only | Use AutoConnect::handleClient
----|----|---
[None](api.md#constructors) | AutoConnect menu not available.<br>To use AutoConnect menu, need [AutoConnect::handleRequest()](api.md#handlerequest).<br>also to use ESP8266WebServer/WebServer natively, need [AutoConnect::host()](api.md#host). | AutoConnect menu available.<br>To use ESP8266WebServer/WebServer natively, need [AutoConnect::host()](api.md#host).
[Reference to ESP8266WebServer/WebServer](api.md#withparameter) | AutoConnect menu not available.<br>To use AutoConnect menu, need [AutoConnect::handleRequest()](api.md#handlerequest). | AutoConnect menu available.

- **By declaration for the AutoConnect variable with no parameter**: The ESP8266WebServer/WebServer instance is hosted by AutoConnect automatically then the Sketches use [*AutoConnect::host*](api.md#host) as API to get it after [*AutoConnect::begin*](api.md#begin) performed.

- **By declaration for the AutoConnect variable with the reference of ESP8266WebServer/WebServer**: AutoConnect will use it. the Sketch can use it is too.

- **In use ESP8266WebServer::handleClient()/WebServer::handleClient()**: AutoConnect menu can be dispatched but not works normally. It is necessary to call [*AutoConnect::handleRequest*](api.md#void-handlerequest) after *ESP8255WebServer::handleClient*/*WebServer::handleClient* invoking.

- **In use [AutoConnect::handleClient()](api.md#void-handleclient)**: The handleClient() process and the AutoConnect menu is available without calling *ESP8266WebServer::handleClient*.

!!! info "Why AutoConnect::handleRequest is needed when using ESP8266WebServer::handleClient/WebServer::handleClient"
    The AutoConnect menu function may affect WiFi connection state. It follows that the menu process must execute outside *ESP8266WebServer::handleClient* and *WebServer::handleClient*.  
    [*AutoConnect::handleClient*](api.md#void-handleclient) is equivalent *ESP8266WebServer::handleClient* and *WEbServer::handleClient* included [*AutoConnect::handleRequest*](api.md#void-handlerequest).

## Reducing Binary Size

Typically, AutoConnect components include [AutoConnectAux](acelements.md) for handling [Custom Web pages](acintro.md); AutoConnectAux plays a central role in responding to requests for Custom Web pages. It also incorporates several AutoConnectElements used in the sketch, which may exceed 1 MB in binary size after the build. To reduce the binary size, you can deactivate the component to handle these custom web pages, depending on the use case. If your sketch does not use Custom web pages, allows you to exclude the AutoConnectAux component to reduce the built binary size.

[**`AutoConnect.h`**](api.md#autoconnecth) header file enables all AutoConnect components. In a normal sketch, including this header enables all AutoConnect functionality.  
On the other hand, for sketches that don't use custom web pages, you can apply the [**`AutoConnectCore.h`**](api.md#autoconnectcoreh) header file.

[**`AutoConnectCore.h`**](api.md#autoconnectcoreh) provides an AutoConnect class that excludes AutoConnectAux and AutoConnectElements from AutoConnect. Therefore, it does not implement the APIs required for custom web page processing. Also, [AutoConnectOTA](otabrowser.md) and [AutoConnectUpdate](otaserver.md) cannot be used. (i.e., to use AutoConnect's equipped OTA Update feature, you must include the full AutoConnect component in your sketch) Instead, the binary size of the AutoConnectCore component is reduced by about 170 KB (1.3 KB for RAM) compared to the ESP32 AutoConnect full component. (60KB for ESP8266)

To switch between AutoConnect and AutoConnectCore, simply change the corresponding header file with `#include` header.

### <i class="fa fa-edit"></i> Using AutoConnect Full component

```cpp hl_lines="3 21"
#include <WiFi.h>
#include <WebServer.h>
#include <AutoConnect.h>

static const char PAGE_HELLO[] = R"(
{
  "uri": "/",
  "title": "Hello",
  "menu": false,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "value": "Hello, World"
    },
  ]
}
)";

WebServer Server;
AutoConnect Portal(Server);
AutoConnectConfig Config;

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  Config.ota = AC_OTA_BUILTIN;
  Portal.config(Config);

  portal.load(PAGE_HELLO);
  Portal.begin();
  Serial.println("Web Server started:" + WiFi.localIP().toString());
}

void loop() {
  Portal.handleClient();
}
```

### <i class="fa fa-edit"></i> Using AutoConnectCore without Custom Web pages and OTA Update facilities

Even in the sketch with `AutoConnectCore.h` applied, the class name remains `AutoConnect`.

```cpp hl_lines="3 6"
#include <WiFi.h>
#include <WebServer.h>
#include <AutoConnectCore.h>

WebServer Server;
AutoConnect Portal(Server);

void rootPage() {
  char content[] = "Hello, World";
  Server.send(200, "text/plain", content);
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  Server.on("/", rootPage);
  Portal.begin();
  Serial.println("Web Server started:" + WiFi.localIP().toString());
}

void loop() {
  Portal.handleClient();
}
```

!!! info "Either `AutoConnect.h` or `AutoConnectCore.h`"
    A sketch can include either `AutoConnect.h` or `AutoConnectCore.h`. These two header files are mutually exclusive and cannot be included together at the same time.  
    Also, If the sketch includes `AutoConnectCore.h`, some members involved in the custom web page facility are excluded from [AutoConnectConfig](apiconfig.md) class.
