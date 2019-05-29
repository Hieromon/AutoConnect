# AutoConnect for ESP8266/ESP32

[![arduino-library-badge](https://www.ardu-badge.com/badge/AutoConnect.svg?)](https://www.ardu-badge.com/AutoConnect)
[![GitHub release](https://img.shields.io/github/release/Hieromon/AutoConnect.svg)](https://github.com/Hieromon/AutoConnect/releases)
[![Build Status](https://travis-ci.org/Hieromon/AutoConnect.svg?branch=master)](https://travis-ci.org/Hieromon/AutoConnect)

An Arduino library for ESP8266/ESP32 WLAN configuration at run time with web interface. 

## Overview

To the dynamic configuration for joining to WLAN with SSID and PSK accordingly. It an Arduino library united with *ESP8266WebServer* class of ESP8266 or *WebServer* class of ESP32.
Easily implementing the Web interface constituting the WLAN for ESP8266/ESP32 WiFi connection. With this library to make a sketch easily which connects from ESP8266/ESP32 to the access point at runtime by the web interface without hard-coded SSID and password.

<div align="center"><img alt="Overview" width="460" src="docs/images/ov.png" />&emsp;&emsp;&emsp;<img alt="Captiveportal" width="182" src="docs/images/ov.gif" /></div>

### No need pre-coded SSID &amp; password

It is no needed hard-coding in advance the SSID and Password into the sketch to connect between ESP8266/ESP32 and WLAN. You can input SSID &amp; Password from a smartphone via the web interface at runtime.

### Simple usage

AutoConnect control screen will be displayed automatically for establishing new connections. It aids by the <a href="https://en.wikipedia.org/wiki/Captive_portal">captive portal</a> when vested the connection cannot be detected.<br>By using the [AutoConnect menu](https://hieromon.github.io/AutoConnect/menu.html), to manage the connections convenient.

### Store the established connection

The connection authentication data as credentials are saved automatically in EEPROM of ESP8266/ESP32 and You can select the past SSID from the [AutoConnect menu](https://hieromon.github.io/AutoConnect/menu.html).

### Easy to embed in

AutoConnect can be embedded easily into your sketch, just "**begin**" and "**handleClient**".

###  Lives with the your sketches

The sketches which provide the web page using ESP8266WebServer/WebServer there is, AutoConnect will not disturb it. AutoConnect can use an already instantiated ESP8266WebServer object(ESP8266) or WebServer object(ESP32), or itself can assign it.

### Easily add user-owned web screen and  parameters <sup><sub>ENHANCED w/ v0.9.7</sub></sup>

You can easily add your own web screen that can consist of representative HTML elements as the styled TEXT, INPUT, BUTTON, CHECKBOX, RADIO, SELECT, SUBMIT into the menu. It can be invoked from the AutoConnect menu and parameters can be passed.

### Just loading the JSON description <sup><sub>ENHANCED w/ v0.9.7</sub></sup>

These HTML elements that make up the user-owned screen can be easily loaded from the JSON description stored in PROGMEM, SPIFFS or SD.

<img width="40%" src="mkdocs/images/aux_json.png">&emsp;
<img width="32px" src="mkdocs/images/arrow_right.png" align="top">&emsp;
<img width="30%" height="400px" src="mkdocs/images/AutoConnectAux.gif">

## Supported hardware

Apply the [Arduino core](https://github.com/esp8266/Arduino) of the ESP8266 Community.

- Generic ESP8266 modules
- Adafruit HUZZAH ESP8266 (ESP-12)
- ESP-WROOM-02
- Heltec WiFi Kit 8
- NodeMCU 0.9 (ESP-12) / NodeMCU 1.0 (ESP-12E)
- Olimex MOD-WIFI-ESP8266
- SparkFun Thing
- SweetPea ESP-210

Alter the platform applying the [arduino-esp32](https://github.com/espressif/arduino-esp32) for the ESP32 modules.

- ESP32Dev Board
- SparkFun ESP32 Thing
- WEMOS LOLIN D32
- Ai-Thinker NodeMCU-32S
- Heltec WiFi Kit 32
- M5Stack
- And other ESP32 modules supported by the Additional Board Manager URLs of the Arduino-IDE.

## Simple usage

### The AutoConnect menu

<img src="mkdocs/images/menu_r.png" width="200">&emsp;<img src="mkdocs/images/statistics.png" width="200">&emsp;<img src="mkdocs/images/confignew.png"
 width="200">&emsp;<img src="mkdocs/images/openssid.png" width="200">

### How embed the AutoConnect to the sketches you have

Most simple approach to applying AutoConnect for the existing sketches, follow the below steps.

<img src="mkdocs/images/beforeafter.png">

## More usages and Documentation

Full documentation is available on https://Hieromon.github.io/AutoConnect, some quick links at the list:

- The [Installation](https://hieromon.github.io/AutoConnect/index.html#installation) is the installation procedure and requirements for the library.
- [Getting started](https://hieromon.github.io/AutoConnect/gettingstarted.html) with the most simple sketch for using AutoConnect.
- The [Basic usage](https://hieromon.github.io/AutoConnect/basicusage.html) guides to using the library correctly.
- Details are explained in the [Advanced usage](https://hieromon.github.io/AutoConnect/advancedusage.html).
- Details and usage of custom Web pages are explained in the [Custom Web pages](https://hieromon.github.io/AutoConnect/acintro.html).
- The [API reference](https://hieromon.github.io/AutoConnect/api.html) describes the AutoConnect functions specification.
- There are hints in [Examples](https://hieromon.github.io/AutoConnect/howtoembed.html) for making sketches with AutoConnect.
- [FAQ](https://hieromon.github.io/AutoConnect/faq.html).

## Change log

### [0.9.9] May 25, 2019
- Supports ESP8266 Arduino core 2.5.2.
- Menu text/background color can be statically customized. refer to the [Custom colorized](https://hieromon.github.io/AutoConnect/colorized.html) for the detailed specification for the menu colorizing.
- Added ID attribute to HTML tag generated from AutoConnectText.
- Added the enable attribute to the AutoConnectElements.
- Fixed the input box layout collapsed.
- Fixed that the decoration of AutoConnectButton was disabled.
- Fixed that the value remains even after clearing the option with AutoConnectSelect.
- Fixed that an alignment violation exception occurred when loading AutoConnectAux described by JSON with PROGMEM attribute.

### [0.9.8] May 3, 2019
- Supports ArduinoJson 6.9.1 or later.
- Supports allocating JsonDocument buffer to PSRAM on ESP32 with ArduinoJson 6.10.0 or later.
- Supports **operator`[]`** as a shortcut for AutoConnectAux::getElement function.
- Supports **AutoConnectElement::as<T\>** function to easily coding for conversion from an AutoConnectElement to an actual type.
- Supports new element type **AutoConnectFile** and built-in file uploader.
- Supports a **format** attribute with the AutoConnectText element.
- Supports a **selected** attribute with the AutoConnectSelect element.
- Supports multiple element loading with AutoConnectAux::loadElement.
- Changed menu labels placement in source files structure.
- Changed API interface of AutoConnect::where function.
- Fixed blank page responds with Configure new.
- Fixed loading elements value missing.
- Fixed losing elements in saveElement with ArduinoJson V6.
- Fixed compile error with older than ESP8266 core 2.5.0.

### [0.9.7] Feb. 25, 2019
- Fixed crash in some environments. Thank you @ageurtse
- Supports AutoConnect menu extension by user sketch with **AutoConnectAux**.
- Supports loading and saving of user-defined parameters with JSON format.
- Improved the WiFi connection sequence at the first WiFi.begin. Even if **AutoConnectConfig::autoReconnect** is disabled when SSID and PSK are not specified, it will use the information of the last established access point. The autoReconnect option will achieve trying the connect after a previous connection failed.
- Supports the **AutoConnectConfig::immediateStart** option and immediately starts the portal without first trying WiFi.begin. You can start the captive portal at any time in combination with the **AutoConnectConfig::autoRise** option.
- Improved boot uri after reset. **AutoConnectConfig::bootUri** can be specified either /_ac or HOME path as the uri to be accessed after invoking Reset from AutoConnect menu.
- Improved source code placement of predefined macros. Defined common macros have been moved to ```AutoConnectDefs.h```.
- Supports **AutoConnectConfig::hostName**. It activates ```WiFi.hostname()```.
- Supports the captive portal time-out. It can be controlled by **AutoConnectConfig::portalTimeout** and **AutoConnectConfig::retainPortal**.

### [0.9.6] Sept. 27, 2018
- Improvement of RSSI detection for saved SSIDs.
- Fixed disconnection SoftAP completely at the first connection phase of the AutoConnect::begin.

### [0.9.5] Aug. 27, 2018
- Supports the espressif arduino-esp32 core.
- Fixed that crash may occur if the number of stored credentials in the EEPROM is smaller than the number of found WiFi networks.

### [0.9.4] May 5, 2018.
- Automatically focus passphrase after selecting SSID with Configure New AP.
- Supports AutoConnectConfig::autoReconnect option, it will scan the WLAN when it can not connect to the default SSID, apply the applicable credentials if it is saved, and try reconnecting.

### [0.9.3] March 23, 2018.
- Supports a static IP address assignment.

### [0.9.2] March 19, 2018.
- Improvement of string literal declaration with the examples, no library change.

### [0.9.1] March 13, 2018.
- A release of the stable.

## License

License under the [MIT license](LICENSE).
