## OTA Updates with AutoConnect

AutoConnect provides **two types of the platform** for updating the binary sketch stored in the ESP8266 or ESP32 module via OTA. They correspond to the [Web Browser Update](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html) and [HTTP Server Update](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html#http-server) mentioned in the [ESP8266 Arduino Core documentation](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html#ota-updates).

[**The update behavior when using a web browser**](#updates-with-the-web-browser) as an update client keeps with the scenario assumed by the ESP8266 arduino core. Therefore, the user sketch must meet the requirements described in the ESP8266 Arduino Core documentation, but it is not difficult to incorporate an implementation for that into a sketch using AutoConnect.

<img src="images/webupdate.png" width="480" />

!!! caution "It is for the only the same network"
    This method can apply only if the client browser and the ESP module belong to the same network segment. It cannot update across networks.

[**Another update method using an update server**](#updates-with-the-update-server) can be applied more broadly than using a web browser. This method can also update the ESP module over the Internet if you can make the correct transparency between the ESP module and the update server.

<img src="images/updateserver.png" width="380" />

!!! info "Security Disclaimer"
    The security of the OTA update platform provided by AutoConnect is a very weak level. No guarantees as to the level of security provided for your application by the AutoConnect OTA Update is implied.

## Updates with the Web Browser

You can implement the user sketch as described in the [ESP8266 Arduino Core documentation](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html#implementation-overview) to realize using the web browser as an update client. By incorporating the ESP8266HTTPUpdateServer class into AutoConnect, you can operate the page for selecting the update owned by ESP8266HTTPUpdateServer from the AutoConnect menu. Updates with a web browser are implemented using ESP8266HTTPUpdateServer class together with ESP8266WebServer and ESP8266mDNS classes. However, **ESP32 Arduino core does not provide a class equivalent to ESP8266HTTPUpdateServer**. Therefore, it is necessary to implement HTTPUpdateServer class for ESP32 to realize the update with a Web browser using the ESP32. The AutoConnect library includes an implementation of the HTTPUpdateServer class for ESP32 to make it easy for you to experience. [^1]

[^1]: You can find the implementation of the HTTPUpdateServer class in the WebUpdate folder included in the AutoConnect library examples.

### <i class="fa fa-edit"></i> How to embed ESP8266HTTPUpdateServer class with AutoConnect

To embed the ESP8266HTTPUpdate server with  AutoConnect into your sketch, basically follow these steps:

1. Include `ESP8266mDNS.h` and `ESP8266HTTPUpdateServer.h` and `WiFiClient.h` additionally, except the usual include directives as ESP8266WebServer and AutoConnect.
2. Declare an ESP8266WebServer object. (In ESP32, as WebServer)
3. Declare an ESP8266HTTPUpdateServer object.
4. Declare an AutoConnect object with an ESP8266WebServer object.
5. Declare an AutoConnectAux object for the update operation page.
6. Assign `/update` to the URI of the update operation page.
7. Assign an arbitrary title as the AutoConnect menu for the update operation page.
8. Declare additional AutoConnectAux pages for your application intention as needed.
9. Perform the following procedure steps in the `setup()` function:
    1. Invokes `ESP8288HTTPUpdateServer::setup` function, specifies the **USERNAME** and the **PASSWORD** as needed.
    2. Load the AutoConnectAux pages declared in step #8 for your application. (Except the update operation page)
    3. Join these pages to AutoConnect along with the update operation page declared in step #5.
    4. Invokes `AutoConnect::begin` function.
    5. Call the `MDNS.begin` and `MDNS.addServer` functions to start the multicast DNS service.
10. Perform the following procedure steps in the `loop()` function:
    1. Call the `MDNS.update` function to parse requests for mDNS. (No needed as ESP32)
    2. Invoke `AutoConnect::handleClient` function.

```cpp
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>    // Step #1
#include <ESP8266mDNS.h>                // Step #1
#include <WiFiClient.h>                 // Step #1
#include <AutoConnect.h>

static const char HELLO_PAGE[] PROGMEM = R"(
{ "title": "Hello world", "uri": "/", "menu": true, "element": [
    { "name": "caption", "type": "ACText", "value": "<h2>Hello, world</h2>",  "style": "text-align:center;color:#2f4f4f;padding:10px;" },
    { "name": "content", "type": "ACText", "value": "In this page, place the custom web page handled by the sketch application." } ]
}
)";

ESP8266WebServer httpServer;                // Step #2
ESP8266HTTPUpdateServer httpUpdate;         // Step #3
AutoConnect portal(httpServer);             // Step #4
AutoConnectAux update("/update", "UPDATE"); // Step #5, #6, #7
AutoConnectAux hello;                       // Step #8

void setup() {
  httpUpdate.setup(&httpServer, "USERNAME", "PASSWORD"); // Step #9.a
  hello.load(HELLO_PAGE);                                // Step #9.b
  portal.join({ hello, update });                        // Step #9.c
  if (portal.begin()) {                                  // Step #9.d
    if (MDNS.begin("esp-webupdate"))                     // Step #9.e
        MDNS.addService("http", "tcp", 80);              // Step #9.e
  }
}

void loop() {
  MDNS.update();            // Step #10.a
  portal.handleClient();    // Step #10.b
}
```

!!! hint "For ESP32"
    This method is equally applicable to ESP32. Just change ESP8266HTTPUpdaetServer to HTTPUpdateServer using an implementation provided from AutoConnect library example code.

!!! example "Share an ESP8266WebServer"
    AutoConnect shares the ESP8266WebServer instance with the ESP8266HTTPUpdateServer class. You can give the same instance as ESP8266WebServer instance given to AutoConnect to ESP8266HTTPUpdateServer class.
    ```cpp
    ESP8266WebServer httpServer;
    ESP8266HTTPUpdateServer updateServer;
    AutoConnect portal(httpServer);

    updateServer(&httpServer);
    ```
    This sharing specification is the same for ESP32.

The execution result of the above sketch should be as follows. [^2]

[^2]: The authentication dialog is displayed first.

<span style="display:block;margin-left:auto;margin-right:auto;width:282px;height:362px;border:1px solid lightgrey;"><img data-gifffer="images/webupdate.gif" data-gifffer-height="360" data-gifffer-width="280" /></span>

!!! faq "How LED ticking during an update"
    You **cannot** get the ticker with LED during an update by using this way. It is since the current implementation of the **ESP8266HTTPUpdateServer class provided Arduino core library does not supply** an LED pin to the ESP8266HTTPUpdate class.

### <i class="fa fa-wrench"></i> How to make the binary sketch

Binary sketch files for updating can be retrieved using the Arduino IDE. Open the **Sketch** menu and select the **Export compiled Binary**, then starts compilation.

<img src="images/export_binary.png" width="450" />

When the compilation is complete, a binary sketch will save with the extension `.bin` in the same folder as the sketch.

## Updates with the update server

Since the v1.0.0 release, AutoConnect provides new feature for updating sketch firmware of ESP8266 or ESP32 modules via OTA using the [AutoConnectUpdate](apiupdate.md#autoconnectupdate) class that is an implementation of the sketch binary update by the HTTP server mentioned in the [OTA update](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html#http-server) of the ESP8266 Arduino Core documentation, which inherits from the ESP8266HTTPUpdate class (as HTTPUpdate class in the case of ESP32). It acts as a client agent for a series of update operations.

If you choose this update method, you must prepare the server process that provides the binary sketch files for the update client agent as a variant of the HTTP server. This server requires to be able to handle the HTTP headers extended for updating the firmware generated by ESP8266HTTPUpdate class as described in the [ESP8266 Arduino Core documentation](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html#server-request-handling), and the AutoConnectUpdate class generates those headers for it.

There are various implementations of update servers that provide binary sketch files. The ESP8266 Arduino Core documentation publishes a php script for the [Advanced updater](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html#id5) which is a process that works fully well with client agents using the ESP8266HTTPUpdate class, as suggested in the implementation. That is, the update server for AutoConnect must work with the client agent, and its implementation should make the handshake well with the AutoConnectUpdate class.

### <i class="far fa-handshake"></i> HTTP contents and the sequence for the AutoConnectUpdate class

The handshake with the AutoConnectUpdate class has two requirements:

- The update server notifies the catalog list of updatable binary files which stored in the update server to the client agent. [^3]
- Send an updating binary file and MD5 hash to a client in response to URI request (HTTP GET). [^4]

[^3]: The **client agent** is an instance of the AutoConnectUpdate class.
[^4]: The client agent will send its URI request to the update server.

Above requirements will be implemented on along the HTTP protocol. The AutoConenctUpdate class requests an update server to notify the client for a catalog list of binary sketch files using an HTTP URL query string. The specifications of the HTTP query and the contents of the catalog list to be returned are as follows:

#### 1. HTTP URL query for the catalog list of the updatable

```
[address]/_catalog?op=list&path=[path]
```
<dl class="apidl">
  <dt></dt>
  <dd><span class="apidef">**address**</span><span class="apidesc">URL of the update server</span>
  <dd><span class="apidef">**/_catalog**</span><span class="apidesc">Request path, it is fixed.</span>
  <dd><span class="apidef">**op**</span><span class="apidesc">Operation command for the update server. Currently, only '**list**' occurs.</span>
  <dd><span class="apidef">**path**</span><span class="apidesc">Path containing the updatable binary files on the update server.</span>
</dl>

#### 2. The catalog list content

The response (that is, the catalog list) to the above query from the server is the following specification in JSON format.

```js
{
  "name" : FILE_NAME,
  "type" : FILE_TYPE,
  "date" : FILE_TIMESTAMP_DATED,
  "time" : FILE_TIMESTAMP_TIMED,
  "size" : FILE_SIZE
}
```
<dl class="apidl">
  <dt></dt>
  <dd><span class="apidef">**name**</span><span class="apidesc">Binary sketch file name for update (String)</span>
  <dd><span class="apidef">**type**</span><span class="apidesc">One of '**bin**', '**directory**' or '**file**'. AutoConnect Update recognizes only file types of '**bin**' as update targets. (String)</span>
  <dd><span class="apidef">**date**</span><span class="apidesc">File update date. AutoConnect v1.0.0 treats the file update date as an annotation and is not equip the version control feature yet. (String)</span>
  <dd><span class="apidef">**time**</span><span class="apidesc">File update time. AutoConnect v1.0.0 treats the file update date as an annotation and is not equip the version control feature yet. (String)</span>
  <dd><span class="apidef">**size**</span><span class="apidesc">File byte count (Numeric)</span>
</dl>

The above JSON object is one entry. The actual catalog list is an array of this entry since it  assumes that an update server will provide multiple update binary files in production. The update server should respond with the MIME type specified as `application/json` for the catalog list.[^5]

[^5]: It should be represented as `Content-Type: application/json` in the HTTP response header.

#### 3. The binary sketch file used for updating



### <i class="fas fa-server"></i> Update server for the AutoConnectUpdate class

### <i class="fas fa-microchip"></i> Behavior of the AutoConnectUpdate class

<script>
  window.onload = function() {
    Gifffer();
  };
</script>
