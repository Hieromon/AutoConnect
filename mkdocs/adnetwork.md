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

## 404 handler

AutoConnect cannot allow the Sketch registers the **"Not-found"** handler (404-handler) to the ESP8266WebServer natively. AutoConnect traps Not-found handler of the ESP8266WebServer for its own page processing. If the Sketch overrides the Not-found handler, AutoConnect will miss the opportunity to control the HTTP session and becomes unresponsive to the menu.  
Registering the Not-found handler is a different method than for ESP8266WebServer, use [AutoConnect::onNotFound](api.md#onnotfound). This restriction applies to the WebServer for ESP32 as well.

## Assign user sketch's home path
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

## Change SSID and Password for SoftAP

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

## Combination with mDNS

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

## Make SSID of SoftAP unique

You can change SoftAP's SSID and password programmatically when the captive portal starts up. By using chip specific ID of esp8266/esp32 you can make SSID of SoftAP unique. SSID and password for SoftAP is [*AutoConnectConfig::apid*](apiconfig.md#apid) and [*AutoConnectConfig::psk*](apiconfig.md#psk).

```cpp
AutoConnect       portal;
AutoConnectConfig acConfig;

acConfig.apid = "ESP-" + String(ESP.getChipId(), HEX);
aConfig.psk = YOUR_PASSWORD;
portal.config(acConfig);
portal.begin();
```

!!! hint "Obtaining chip ID for ESP32"
    `acConfig.apid = "ESP-" + String((uint32_t)(ESP.getEfuseMac() >> 32), HEX);`

## Relocate the AutoConnect home path

A home path of AutoConnect is **/\_ac** by default. You can access from the browser with `http://IPADDRESS_OF_ESP_MODULE/_ac`. You can change the home path by revising [**AUTOCONNECT_URI**](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h#L69) macro in [`AutoConnectDefs.h`](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h) header file.

```cpp
#define AUTOCONNECT_URI         "/_ac"
```

## SoftAP access point IP settings

AutoConnect will activate [SoftAP](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/soft-access-point-class.html#soft-access-point-class) at failed the 1st-WiFi.begin. Its SoftAP settings are stored in AutoConnectConfig as the following parameters. The Sketch could be configured SoftAP using these parameters, refer the [AutoConnectConfig API](apiconfig.md) for details.

| AutoConnectConfig member | Settings for | Defined symbol | Initial value |
|--------------------------|--------------|----------------|---------------|
| [**apip**](apiconfig.md#apip) | SoftAP IP address | AUTOCONNECT_AP_IP | 172.217.28.1 |
|[**gateway**](apiconfig.md#gateway) | Gateway IP address | AUTOCONNECT_AP_GW | 172.217.28.1 |
| [**netmask**](apiconfig.md#netmask) | Subnet mask for the SoftAP | AUTOCONNECT_AP_NM | 255.255.255.0 |
| [**channel**](apiconfig.md#channel) | WiFi channel for the SoftAP | AUTOCONNECT_AP_CH | 1 |
| [**hidden**](apiconfig.md#hidden) | Hide the SoftAP | | false |

## Static IP assignment as a client

It is possible to assign a static IP Address to ESP8266/ESP32 in STA mode.[^3] By default DHCP is enabled and it becomes the IP address assigned by the DHCP server with **WiFi.begin**.

[^3]:Static IP address assignment is available from version 0.9.3.

These settings are made via [AutoConnectConfig](apiconfig.md) as in the case of SoftAP settings. To assign a static IP to ESP8266/ESP32 with WIFI\_STA, the following parameters are required:

|AutoConnectConfig member | Settings for | Initial value |
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

## Station hostname

[*AutoConnectConfig::hostName*](apiconfig.md#hostname) assigns a station DHCP hostname to the ESP module. The hostname must satisfy [RFC952](https://tools.ietf.org/html/rfc952) compliant and meet the following restrictions:

- Up to 24 characters
- Only the alphabet (a-z, A-Z), digits (0-9), minus sign (-)
- No '-' as last character
