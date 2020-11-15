You can adjust the AutoConnect behave at run-time using [AutoConnectConfig](apiconfig.md). AutoConnectConfig is a class that has only AutoConnect configuration settings. You define the behavior of AutoConnect using AutoConnectConfig member variables and give it to AutoConnect via the [AutoConnect::config](api.md#config) function. This allows Sketch controls the following:

- [Built-in OTA update](#built-in-ota-update-feature)
- [Debug print](#debug-print)
- [File uploading via built-in OTA feature](#file-uploading-via-built-in-ota-feature)
- [Ticker for WiFi status](#ticker-for-wifi-status)
- [Refers the hosted ESP8266WebServer/WebServer](#refers-the-hosted-esp8266webserverwebserver)
- [Usage for automatically instantiated ESP8266WebServer/WebServer](#usage-for-automatically-instantiated-esp8266webserverwebserver)
- [Use with the [PageBuilder](https://github.com/Hieromon/PageBuilder) library](#use-with-the-pagebuilder-library)

!!! note "AutoConnect::config before AutoConnect::begin"
    [AutoConnect::config](api.md#config) must be executed before [AutoConnect::begin](api.md#begin).

## Built-in OTA update feature

AutoConnect features a built-in OTA function to update ESP module firmware. You can easily make the Sketch that equips OTA and able to operate with the AutoConnect menu.

<span style="display:block;margin-left:auto;margin-right:auto;width:284px;height:462px;border:1px solid lightgrey;"><img data-gifffer="images/webupdate.gif" data-gifffer-height="460" data-gifffer-width="282" /></span>

[*AutoConnectConfig::ota*](apiconfig.md#ota) specifies to import the [built-in OTA update class](otabrowser.md) into the Sketch.  
See the [Updates with the Web Browser](otabrowser.md) chapter for details.

## Debug print

You can output AutoConnect monitor messages to the **Serial**. A monitor message activation switch is in an include header file [`AutoConnectDefs.h`](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h) of library source. Define [**AC_DEBUG**](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h#L14) macro to output the monitor messages.[^5]

[^5]:The source code placement of common macros for AutoConnect since v0.9.7 has changed.

```cpp
#define AC_DEBUG
```

## File uploading via built-in OTA feature

The [built-in OTA update feature](otabrowser.md) can update the firmware as well as upload regular files placed in the file system on the ESP module. It allows a regular file is uploaded via OTA using the [**Update**](menu.md#update) of AutoConnect menu without adding a particular custom Web page that contains AutoConnectFile. This utilization is useful for the operation of transferring the JSON document of the custom web page definition, the external parameter file of your sketch, and so on into the target ESP module via OTA.

The built-in OTA update feature determines where to save the uploaded file according to the filename pattern. By default, files with names with a **.bin** extension are subject to firmware updates. A file that has the other patterns with extension will be saved to SPIFFS in the flash.

The filename pattern that should be treated as the firmware is defined as the **`AUTOCONNECT_UPLOAD_ASFIRMWARE`** macro in [AutoConnectDefs.h](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h#L274) head file of the library source code and can be specified with the **regular expression**.

```cpp
#define AUTOCONNECT_UPLOAD_ASFIRMWARE "^.*\\.[bB][iI][nN]$"
```

!!! note "Specify with the PlatformIO"
    `AUTOCONNECT_UPLOAD_ASFIRMWARE` pattern will be embedded into the binary sketch is determined at compile time. The [**PlatformIO**](https://platformio.org/platformio-ide) build system allows you to change the pattern expression for each project without modifying the library source code.
    
    ```ini
    build_flags=-DAUTOCONNECT_UPLOAD_ASFIRMWARE='"^.*\\.[bB][iI][nN]$"'
    ```

## Refers the hosted ESP8266WebServer/WebServer

Constructing an AutoConnect object variable without parameters then creates and starts an ESP8266WebServer/WebServer inside the AutoConnect. This object variable could be referred by [AutoConnect::host](api.md#host) function to access ESP8266WebServer/WebServer instance as like below.

```cpp hl_lines="4"
AutoConnect Portal;

Portal.begin();
ESP8266WebServer& server = Portal.host();
server.send(200, "text/plain", "Hello, world");
```

!!! info "When host() is valid"
    The host() can be referred at after [AutoConnect::begin](api.md#begin).

## Ticker for WiFi status

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

## Usage for automatically instantiated ESP8266WebServer/WebServer

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

## Use with the [PageBuilder](https://github.com/Hieromon/PageBuilder) library

In ordinary, the URL handler will respond to the request from the client by sending some HTML. It will dynamically generate the HTML to respond to based on the sensing data etc. for the changing scene, but it contains elements of variable values in the middle of the HTML fixed string. Therefore, sketches tend to be in a tangled that repeats the logic for data handling and string splicing in turn, which tends to be less readable and maintainable.

[PageBuilder](https://github.com/Hieromon/PageBuilder) library is an HTML assembly aid. it can handle predefined HTML like the template and simplify an HTML string assemble logic, and also the generated HTML send automatically.

An example sketch used with the PageBuilder as follows and it explains how it aids for the HTML generating. Details for [Github repository](https://github.com/Hieromon/PageBuilder).

<img src="images/PageBuilder.png" style="width:640px;"/>

<script>
  window.onload = function() {
    Gifffer();
  };
</script>
