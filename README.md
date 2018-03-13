# AutoConnect <small>for ESP8266</small>

An Arduino library for ESP8266 WLAN configuration at run time with web interface. [![Build Status](https://travis-ci.org/Hieromon/AutoConnect.svg?branch=master)](https://travis-ci.org/Hieromon/AutoConnect)

## Overview

To the dynamic configuration for joining to WLAN with SSID and PSK accordingly. It an Arduino library united with *ESP8266WebServer* class.
Easily implementing the Web interface constituting the WLAN for ESP8266 WiFi connection. With this library to make a sketch easily which connects from ESP8266 to the access point at runtime by the web interface without hard-coded SSID and password.

<img style="display:inline-block;width:460px;margin-right:30px;" src="docs/images/ov.png" /><img style="display:inline-block;width:182px;height:322px;border:solid 1px lightgrey;" src="docs/images/ov.gif" />

### No need pre-coded SSID &amp; password

It is no needed hard-coding in advance the SSID and Password into the sketch to connect between ESP8266 and WLAN. You can input SSID &amp; Password from a smartphone via the web interface at runtime.

### Simple usage

AutoConnect control screen will be displayed automatically for establishing new connections. It aids by the <a href="https://en.wikipedia.org/wiki/Captive_portal">captive portal</a> when vested the connection cannot be detected.<br>By using the **AutoConnect menu**, to manage the connections convenient.

### <i class="fa fa-arrow-circle-right" aria-hidden="true"></i> Store the established connection

<span class="lead">The connection authentication data as credentials are saved automatically in EEPROM of ESP8266 and You can select the past SSID from the [AutoConnect menu](menu.md).</span>

### <i class="fa fa-arrow-circle-right" aria-hidden="true"></i> Easy to embed in

<span class="lead">AutoConnect can be embedded easily into your sketch, just "**begin**" and "**handleClient**".</span>

### <i class="fa fa-arrow-circle-right" aria-hidden="true"></i> Lives with the your sketches

<span class="lead">The sketches which provide the web page using ESP8266WebServer there is, AutoConnect will not disturb it. AutoConnect can use an already instantiated ESP8266WebServer object, or itself can assign it.</span>

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

<img src="docs/images/BeforeAfter.svg">

## More usages and Documentation

The documentation is available on https://Hieromon.github.io/AutoConnect, here are some shortcuts:

- Installation.
- Getting started.
- Usage the Library.
- API reference.
- FAQ.

## License

License under the [MIT license](LICENSE).