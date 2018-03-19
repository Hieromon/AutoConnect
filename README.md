# AutoConnect for ESP8266

An Arduino library for ESP8266 WLAN configuration at run time with web interface. [![Build Status](https://travis-ci.org/Hieromon/AutoConnect.svg?branch=master)](https://travis-ci.org/Hieromon/AutoConnect)

## Overview

To the dynamic configuration for joining to WLAN with SSID and PSK accordingly. It an Arduino library united with *ESP8266WebServer* class.
Easily implementing the Web interface constituting the WLAN for ESP8266 WiFi connection. With this library to make a sketch easily which connects from ESP8266 to the access point at runtime by the web interface without hard-coded SSID and password.

<div align="center"><img alt="Overview" width="460" src="docs/images/ov.png" />&emsp;&emsp;&emsp;<img alt="Captiveportal" width="182" src="docs/images/ov.gif" /></div>

### No need pre-coded SSID &amp; password

It is no needed hard-coding in advance the SSID and Password into the sketch to connect between ESP8266 and WLAN. You can input SSID &amp; Password from a smartphone via the web interface at runtime.

### Simple usage

AutoConnect control screen will be displayed automatically for establishing new connections. It aids by the <a href="https://en.wikipedia.org/wiki/Captive_portal">captive portal</a> when vested the connection cannot be detected.<br>By using the [AutoConnect menu](https://hieromon.github.io/AutoConnect/menu/index.html), to manage the connections convenient.

### Store the established connection

The connection authentication data as credentials are saved automatically in EEPROM of ESP8266 and You can select the past SSID from the [AutoConnect menu](https://hieromon.github.io/AutoConnect/menu/index.html).

### Easy to embed in

AutoConnect can be embedded easily into your sketch, just "**begin**" and "**handleClient**".

###  Lives with the your sketches

The sketches which provide the web page using ESP8266WebServer there is, AutoConnect will not disturb it. AutoConnect can use an already instantiated ESP8266WebServer object, or itself can assign it.

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
- [Getting started](https://hieromon.github.io/AutoConnect/gettingstarted/index.html) with the most simple sketch for using AutoConnect.
- The [Basic usage](https://hieromon.github.io/AutoConnect/basicusage/index.html) guides to using the library correctly.
- Details are explained in the [Advanced usage](https://hieromon.github.io/AutoConnect/advancedusage/index.html).
- The [API reference](https://hieromon.github.io/AutoConnect/api/index.html) describes the AutoConnect functions specification.
- [FAQ](https://hieromon.github.io/AutoConnect/faq/index.html).

## Change log

### [0.9.2] March 19, 2018.
- Improvement of string literal declaration with the examples, no library change.

### [0.9.1] March 13, 2018.
- A release of the stable.

## License

License under the [MIT license](LICENSE).