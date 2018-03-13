# AutoConnect <small>for ESP8266</small>

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



### How embed the AutoConnect to the sketches you have

Most simple approach to applying AutoConnect for the existing sketches, follow the below steps.

<div align="center"><img src="docs/images/BeforeAfter.svg" width="800"></div>

## More usages and Documentation

The documentation is available on https://Hieromon.github.io/AutoConnect, here are some shortcuts:

- [Installation](https://hieromon.github.io/AutoConnect/index.html#installation).
- [Getting started](https://hieromon.github.io/AutoConnect/gettingstarted/index.html).
- [Usage the Library](https://hieromon.github.io/AutoConnect/usage/index.html).
- [API reference](https://hieromon.github.io/AutoConnect/api/index.html).
- [FAQ](https://hieromon.github.io/AutoConnect/faq/index.html).

## License

License under the [MIT license](LICENSE).