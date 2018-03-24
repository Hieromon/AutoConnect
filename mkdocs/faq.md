## <i class="fa fa-question-circle"></i> After connected, AutoConnect menu performs but no happens.

If you can access the **AutoConnect root path** as http://ESP8266IPADDRESS/_ac from browser, probably the sketch uses *ESP8266WebServer::handleClient()* without [*AutoConnect::handleRequest()*](api.md#handlerequest).  
For AutoConnect menus to work properly, call [*AutoConnect::handleRequest()*](api.md#handlerequest) after *ESP8266WebServer::handleClient()* invoked, or use [*AutoConnect::handleClient()*](api.md#handleclient). [*AutoConnect::handleClient()*](api.md#handleclient) is equivalent *ESP8266WebServer::handleClient* combined [*AutoConnect::handleRequest()*](api.md#handlerequest).

See also the explanation [here](basicusage.md#esp8266webserver-hosted-or-parasitic).

## <i class="fa fa-question-circle"></i> An esp8266ap as SoftAP was connected but Captive portal does not start.

Captive portal detection could not be trapped. It is necessary to disconnect and reset ESP8266 to clear memorized connection data in ESP8266. Also, It may be displayed on the smartphone if the connection information of esp8266ap is wrong. In that case, delete the connection information of esp8266ap memorized by the smartphone once.

## <i class="fa fa-question-circle"></i> Does not appear esp8266ap in smartphone.

Maybe it is successfully connected at the **first WiFi.begin**. ESP8266 remembers the last SSID successfully connected and will use at the next. It means SoftAP will only start up when the first *WiFi.begin()* fails.

The saved SSID would be cleared by  *WiFi.disconnect()* with WIFI_STA mode. If you do not want automatic reconnection, you can erase the memorized SSID with the following simple sketch.

```arduino
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

    <img src="../images/espshaker.gif" />

## <i class="fa fa-question-circle"></i> Does not response from /\_ac.

Probably **WiFi.begin** failed with the specified SSID. Activating the [debug printing](advancedusage.md#debug-print) will help you to track down the cause.

## <i class="fa fa-question-circle"></i> How change esp8266ap for SSID name in Captive portal?

An **esp8266ap** is default SSID name for SoftAP of captive portal and password is **12345678**. You can change both by using [AutoConnectConfig](api.md#autoconnectconfig-api).

## <i class="fa fa-question-circle"></i> How change HTTP port?

HTTP port number is defined as a macro in [AutoConnect.h](https://github.com/Hieromon/AutoConnect/blob/77d5cb5dc6d50651fd15f74b47859105c71fc317/src/AutoConnect.h#L89) header file. You can change it directly with several editors and must re-compile.

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

Make some sketches for erasing the EEPROM area, or some erasing utility is needed. You can prepare the sketch to erase the saved credential with *AutoConnectCredential*. The *AutoConnectCrendential* class provides the access method to the saved credential in EEPROM and library source file is including it.

A class description of AutoConnectCredential is follows.

### Include header

```cpp
#include <AutoConnectCredential.h>
```

### Constructor

```cpp
AutoConnectCredential();
```

AutoConnectCredential default constructor. The default offset value is 0. If the offset value is 0, the credential storage area starts from the top of the EEPROM. AutoConnect sometimes overwrites data when using this area with user sketch.

```cpp
AutoConnectCredential(uint16_t offset);
```

Specify offset from the top of the EEPROM for the credential storage area together. The offset value is from 0 to the flash sector size.

### Public member functions

- uint8_t **entries()**  
   Returns number of entries as contained credentials.

- int8_t **load(const char\* _ssid_, struct station_config\* _config_)**  
   Load a credential entry specified *ssid* to *config*. Returns -1 as unsuccessfully loaded.

- bool **load(int8_t _entry_, struct station_config\* _config_)**  
   Load a credential entry to *config*. The *entry* parameter specify to index of the entry.

- bool **save(const struct station_config\* _config_)**  
   Save a credential entry stored in *config* to EEPROM. Returns the true as succeeded.

- bool **del(const char\* _ssid_)**  
   Delete a credential entry specified *ssid*. Returns the true as successfully deleted.

### Data structures

- station_config  
  A structure is included in the ESP8266 SDK. You can use it in the sketch like as follows.

```cpp
extern "C" {
#include <user_interface.h>
}
```

```cpp
struct station_config {
    uint8 ssid[32];
    uint8 password[64];
    uint8 bssid_set;
    uint8 bssid[6];
};
```

- EEPROM data structure  
  A data structure of the credential saving area in EEPROM as the below. [^1]

[^1]:
There may be 0xff as an invalid data in the credential saving area. The 0xff area would be reused.

| Byte offset | Length   | Value                                                               |
|-------------|----------|---------------------------------------------------------------------|
| 0           | 8        | AC_CREDT                                                            |
| 8           | 1        | Number of contained entries (uint8_t)                               |
| 9           | 2        | Container size, excluding size of AC_CREDT and size of the number of entries(width for uint16_t type). |
| 11          | variable | SSID terminated by 0x00. Max length is 32 bytes.                    |
| variable    | variable | Password plain text terminated by 0x00. Max length is 64 bytes.     |
| variable    | 6        | BSSID                                                               |
| variable    |          | Contained the next entries. (Continuation SSID+Password+BSSID)      |
| variable    | 1        | 0x00. End of container.                                             |

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
