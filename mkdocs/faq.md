## <i class="fa fa-question-circle"></i> After connected, AutoConnect menu performs but no happens.

If you can access the **AutoConnect root path** as http://ESP8266IPADDRESS/_ac from browser, probably the sketch uses *ESP8266WebServer::handleClient()* without [*AutoConnect::handleRequest()*](api.md#handlerequest).  
For AutoConnect menus to work properly, call [*AutoConnect::handleRequest()*](api.md#handlerequest) after *ESP8266WebServer::handleClient()* invoked, or use [*AutoConnect::handleClient()*](api.md#handleclient). [*AutoConnect::handleClient()*](api.md#handleclient) is equivalent *ESP8266WebServer::handleClient* combined [*AutoConnect::handleRequest()*](api.md#handlerequest).

See also the explanation [here](basicusage.md#esp8266webserver-hosted-or-parasitic).

## <i class="fa fa-question-circle"></i> An esp8266ap as SoftAP was connected but Captive portal does not start.

Captive portal detection could not be trapped. It is necessary to disconnect and reset ESP8266 to clear memorized connection data in ESP8266. Also, It may be displayed on the smartphone if the connection information of esp8266ap is wrong. In that case, delete the connection information of esp8266ap memorized by the smartphone once.

## <i class="fa fa-question-circle"></i> Connection lost immediately after establishment with AP

A captive portal is disconnected immediately after the connection establishes with the new AP. This is a known problem of ESP32, and it may occur when the following conditions are satisfied at the same time.

- SoftAP channel on ESP32 and the connecting AP channel you specified are different. (The default channel of SoftAP is 1.)
- NVS had erased by erase_flash causes the connection data lost. The NVS partition has been moved. Never connected to the AP in the past.
- There are receivable multiple WiFi signals which are the same SSID with different channels using the WiFi repeater etc. (This condition is loose, it may occur even if there is no WiFi repeater.)
- Or the using channel of the AP which established a connection is congested with the radio signal on the same band. (If the channel crowd, connections to known APs may also fail.)

!!! info "Other possibilities"
    The above conditions are not absolute. It results from my investigation, and other conditions may exist.

To avoid this problem, try [changing the channel](#1-change-wifi-channel).

ESP32 hardware equips only one RF circuitry for WiFi signal. At the AP_STA mode, ESP32 as an AP attempts connect to another AP on another channel while keeping the connection with the station then the channel switching will occur causes the station may be disconnected. But it may not be just a matter of channel switching causes ESP8266 has the same constraints too. It may be a problem with AutoConnect or the arduino core or SDK issue. This problem will persist until a specific solution.

## <i class="fa fa-question-circle"></i> Does not appear esp8266ap in smartphone.

Maybe it is successfully connected at the [**first WiFi.begin**](lsbegin.md#autoconnectbegin-logic-sequence). ESP8266 remembers the last SSID successfully connected and will use at the next. It means SoftAP will only start up when the first *WiFi.begin()* fails.

The saved SSID would be cleared by  *WiFi.disconnect()* with WIFI_STA mode. If you do not want automatic reconnection, you can erase the memorized SSID with the following simple sketch.

```cpp
#include <ESP8266WiFi.h>

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  delay(100);
  WiFi.begin();
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    WiFi.disconnect();
    while (WiFi.status() == WL_CONNECTED)
      delay(100);
  }
  Serial.println("WiFi disconnected.");
}

void loop() {
  delay(1000);
}
```

??? hint "You can interactively check the WiFi state of ESP8266."
    Please try [**ESPShaker**](https://github.com/Hieromon/ESPShaker). It is ESP8266 interactive serial command processor.

    <img src="images/espshaker.gif" />

## <i class="fa fa-question-circle"></i> Does not response from /\_ac.

Probably **WiFi.begin** failed with the specified SSID. Activating the [debug printing](advancedusage.md#debug-print) will help you to track down the cause.

## <i class="fa fa-question-circle"></i> How change esp8266ap for SSID name in Captive portal?

You can change both by using [AutoConnectConfig::apid](apiconfig.md#apid) and [AutoConnectConfig::psk](apiconfig.md#psk). Refer to section [Change SSID and Password for SoftAP](advancedusage.md#change-ssid-and-password-for-softap) in [Advanced usage](advancedusage.md).

## <i class="fa fa-question-circle"></i> How change HTTP port?

HTTP port number is defined as a macro in [AutoConnectDefs.h](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h#L112) header file. You can change it directly with several editors and must re-compile.

```cpp
#define AUTOCONNECT_HTTPPORT    80
```

## <i class="fa fa-question-circle"></i> Hang up after Reset?

If ESP8266 hang up after reset by AutoConnect menu, perhaps manual reset is not yet. Especially if it is not manual reset yet after uploading the sketch, the boot mode will stay 'Uart Download'. There is some discussion about this on the Github's ESP8266 core: https://github.com/esp8266/Arduino/issues/1017

If you received the following message, the boot mode is still sketch uploaded. It needs to the manual reset once.

```
ets Jan  8 2013,rst cause:2, boot mode:(1,6) or (1,7)
ets Jan  8 2013,rst cause:4, boot mode:(1,6) or (1,7)
wdt reset
```

The correct boot mode for starting the sketch is **(3, x)**.

!!! info "ESP8266 Boot Messages"
    It is described by [ESP8266 Non-OS SDK API Reference](view-source:https://www.espressif.com/en/products/hardware/esp8266ex/resources), section A.5.

    | Messages | Description |
    |----------|-------------|
    | rst cause | 1: power on<br>2: external reset<br>4: hardware watchdog reset |
    | boot mode<br>(the first parameter) | 1: ESP8266 is in UART-down mode (and downloads firmware into flash).<br>3: ESP8266 is in Flash-boot mode (and boots up from flash). |

## <i class="fa fa-question-circle"></i> How erase the credentials saved in EEPROM?

Make some sketches for erasing the EEPROM area, or some erasing utility is needed. You can prepare the sketch to erase the saved credential with *AutoConnectCredential*. The *AutoConnectCrendential* class provides the access method to the saved credential in EEPROM and library source file is including it. Refer to '[Saved credential access](credit.md#saved-credential-in-eeprom)' on section [Appendix](credit.md) for details.

!!! hint
    With the [**ESPShaker**](https://github.com/Hieromon/ESPShaker), you can access EEPROM interactively from the serial monitor, and of course you can erase saved credentials.

## <i class="fa fa-question-circle"></i> How locate the link button to the  AutoConnect menu?

Link button to AutoConnect menu can be embedded into Sketch's web page. The root path of the menu is **/_ac** by default and embed the following ```<a></a>``` tag in the generating HTML.

```html
<a style="background-color:SteelBlue; display:inline-block; padding:7px 13px; text-decoration:none;" href="/_ac">MENU</a>
```

## <i class="fa fa-question-circle"></i> How much memory does AutoConnect consume?

### Sketch size

It increases about 53K bytes compared to the case without AutoConnect. A sketch size of the most simple example introduced in the Getting started is about 330K bytes. (270K byte without AutoConnect)

### Heap size

It consumes about 2K bytes in the static and about 12K bytes are consumed at the moment when menu executed.

## <i class="fa fa-question-circle"></i> I cannot complete to Wi-Fi login from smartphone.

Because AutoConnect does not send a login success response to the captive portal requests from the smartphone. The login success response varies iOS, Android and Windows. By analyzing the request URL of different login success inquiries for each OS, the correct behavior can be implemented, but not yet. Please resets ESP8266 from the AutoConnect menu.

## <i class="fa fa-question-circle"></i> I cannot see the custom Web page.

If the sketch is correct, a JSON syntax error may have occurred. In this case, activate the [AC_DEBUG](faq.md#3-turn-on-the-debug-log-options) and rerun. If you take the message of JSON syntax error, the [Json Assistant](https://arduinojson.org/v5/assistant/) helps syntax checking. This online tool is provided by the author of ArduinoJson and is most consistent for the AutoConnect. 

## <i class="fa fa-question-circle"></i> Submit element in a custom Web page does not react.

Is there the AutoConnectElements element named **SUBMIT** in the custom Web page? (case sensitive ignored) AutoConnect does not rely on the `input type=submit` element for the form submission and uses [HTML form element submit](https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement/submit) function instead. So, the submit function will fail if there is an element named 'submit' in the form. You can not use **SUBMIT** as the element name of AutoConnectElements in a custom Web page that declares the AutoConnectSubmit element.

## <i class="fa fa-question-circle"></i> Still, not stable with my sketch.

If AutoConnect behavior is not stable with your sketch, you can try the following measures.

### 1. Change WiFi channel

Both ESP8266 and ESP32 can only work on one channel at any given moment. This will cause your station to lose connectivity on the channel hosting the captive portal. If the channel of the AP which you want to connect is different from the SoftAP channel, the operation of the captive portal will not respond with the screen of the AutoConnect connection attempt remains displayed. In such a case, please try to configure the [channel](apiconfig.md#channel) with [AutoConnectConfig](apiconfig.md#autoconnectconfig) to match the access point.

```cpp
AutoConnect portal;
AutoConnectConfig config;

config.channel = 3;     // Specifies a channel number that matches the AP
portal.config(config);  // Apply channel configurration
portal.begin();         // Start the portal
```

!!! info "Channel selection guide"
    Espressif Systems has released a [channel selection guide](https://www.espressif.com/sites/default/files/esp8266_wi-fi_channel_selection_guidelines.pdf).

### 2. Change the arduino core version

I recommend change installed an arduino core version to the upstream when your sketch is not stable with AutoConnect on each board.

#### with ESP8266 arduino core

You can select the [lwIP](http://lwip.wikia.com/wiki/LwIP_Wiki) variant to contribute for the stable behavior. The **lwIP v2 Lower memory** option of Arduino IDE for core version 2.4.2 is based on the lwIP-v2. On the other hand, the core version 2.5.0 upstream is based on the lwIP-2.1.2 stable release.

You can select the option from Arduino IDE as **Tool** menu, if you are using ESP8266 core 2.5.0. It can be select `lwIP v2 Lower Memory` option. (not `lwIP v2 Lower Memory (no features)`) It is expected to improve response performance and stability.

#### with ESP32 arduino core

The [arduino-esp32](https://github.com/espressif/arduino-esp32) is still under development. It is necessary to judge whether the problem cause of the core or AutoConnect. Trace the log with the esp32 core and the AutoConnect debug option enabled for problem diagnosis and please you check the [issue of arduino-esp32](https://github.com/espressif/arduino-esp32/issues). The problem that your sketch possesses may already have been solved.

### 3. Turn on the debug log options

To fully enable for the AutoConnect debug logging options, change the following two files.

<i class="fas fa-angle-right"></i> AutoConnectDefs.h

```cpp
#define AC_DEBUG
```

<i class="fas fa-angle-right"></i> PageBuilder.h [^2]

```cpp
#define PB_DEBUG
```

[^2]: `PageBuilder.h` exists in the `libraries/PageBuilder/src` directory under your sketch folder.

### 4. Reports the issue to AutoConnect Github repository

If you can not solve AutoConnect problems please report to [Issues](https://github.com/Hieromon/AutoConnect/issues). And please make your question comprehensively, not a statement. Include all relevant information to start the problem diagnostics as follows:[^3]

* [ ] Hardware module
* [ ] Arduino core version Including the upstream commit ID if necessary
* [ ] Operating System which you use
* [ ] Your smartphone OS and version (Especially for Android)
* [ ] Your AP information (IP, channel) if related
* [ ] lwIP variant
* [ ] Problem description
* [ ] If you have a STACK DUMP decoded result with formatted by the code block tag
* [ ] The sketch code with formatted by the code block tag (Reduce to the reproducible minimum code for the problem)
* [ ] Debug messages output (Including arduino core)

I will make efforts to solve as quickly as possible. But I would like you to know that it is not always possible.

Thank you.

[^3]:Without this information, the reproducibility of the problem is reduced, making diagnosis and analysis difficult.
