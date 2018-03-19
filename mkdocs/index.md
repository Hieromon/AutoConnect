# AutoConnect <small>for ESP8266</small>

An Arduino library for ESP8266 WLAN configuration at run time with web interface.

## Overview

To the dynamic configuration for joining to WLAN with SSID and PSK accordingly. It an Arduino library united with *ESP8266WebServer* class.
Easy implementing the Web interface constituting the WLAN for ESP8266 WiFi connection. With this library to make a sketch easily which connects from ESP8266 to the access point at runtime by the web interface without hard-coded SSID and password.

<img style="display:inline-block;width:460px;margin-right:30px;" src="./images/ov.png" /><span style="display:inline-block;width:182px;height:322px;border:solid 1px lightgrey;"><img data-gifffer="./images/ov.gif" data-gifffer-width="180" style="width:180px;" /></span>

### <i class="fa fa-arrow-circle-right" aria-hidden="true"></i> No need pre-coded SSID &amp; password

<span class="lead">It is no needed hard-coding in advance the SSID and Password into the sketch to connect between ESP8266 and WLAN. You can input SSID &amp; Password from a smartphone via the web interface at runtime.</span>

### <i class="fa fa-arrow-circle-right" aria-hidden="true"></i> Simple usage

<span class="lead">AutoConnect control screen will be displayed automatically for establishing new connections. It aids by the <a href="https://en.wikipedia.org/wiki/Captive_portal">captive portal</a> when vested the connection cannot be detected.<br>By using the [AutoConnect menu](menu.md), to manage the connections convenient.</span>

### <i class="fa fa-arrow-circle-right" aria-hidden="true"></i> Store the established connection

<span class="lead">The connection authentication data as credentials are saved automatically in EEPROM of ESP8266 and You can select the past SSID from the [AutoConnect menu](menu.md).</span>

### <i class="fa fa-arrow-circle-right" aria-hidden="true"></i> Easy to embed in

<span class="lead">AutoConnect can be placed easily in your sketch. It's "**begin**" and "**handleClient**" only.</span>

### <i class="fa fa-arrow-circle-right" aria-hidden="true"></i> Lives with the your sketches

<span class="lead">The sketches which provide the web page using ESP8266WebServer there is, AutoConnect will not disturb it. AutoConnect can use an already instantiated ESP8266WebServer object, or itself can assign it.</span>

## Installation

### Requirements

#### Supported hardware

  * [X] Generic ESP8266 modules (applying the ESP8266 Community's Arduino core)
  * [X] Adafruit HUZZAH ESP8266 (ESP-12)
  * [X] ESP-WROOM-02
  * [X] Heltec WiFi Kit 8
  * [X] NodeMCU 0.9 (ESP-12) / NodeMCU 1.0 (ESP-12E)
  * [X] Olimex MOD-WIFI-ESP8266
  * [X] SparkFun Thing
  * [X] SweetPea ESP-210

!!! info "About flash size on the module"
    The AutoConnect sketch size is relatively large. Large flash capacity is necessary. 512Kbyte (4Mbits) flash inclusion module such as ESP-01 is not recommended.

#### Required libraries

AutoConnect requires the following environment and libraries.

<i class="fa fa-download"></i> <strong>Arduino IDE</strong>

The current upstream at the 1.8 level or later is needed. Please install from the [official Arduino IDE download page](https://www.arduino.cc/en/Main/Software). This step is not required if you already have a modern version.

<i class="fa fa-download"></i> <strong>ESP8266 Arduino core</strong>

AutoConnect targets sketches made on the assumption of [ESP8266 Community's Arduino core](https://github.com/esp8266/Arduino). The [latest release](https://github.com/esp8266/Arduino/releases/latest) is recommended.  
Install third-party platform using the *Boards Manager* of Arduino IDE. Package URL is http://arduino.esp8266.com/stable/package_esp8266com_index.json

<i class="fa fa-download"></i> <strong>Additional necessary library</strong>

The [PageBuilder](https://github.com/Hieromon/PageBuilder) library to build HTML for ESP8266WebServer is needed.  
To install the PageBuilder library into your Arduino IDE, you can use the *Library Manager*. Select the board of ESP8266 series in the Arduino IDE, open the library manager and search keyword '**PageBuilder**' with the topic '**Communication**', then you can see the *PageBuilder*. The latest version or 1.0.0 later is required.

<img src="./images/lm.png" width="640"/>

!!! warning "Library Manager of Arduino IDE"
    The version of PageBuilder may not have been updated yet. If the library manager shows only version 0.93.0, please install the latest PageBuilder version than 1.0.0 from the Github repository. [The PageBuilder release](https://github.com/Hieromon/PageBuilder/releases/latest) of the Github repository is the latest version. The [PlatformIO library](https://platformio.org/lib/show/2010/PageBuilder) has the latest version of 1.0.0.  
    In 0.93.0, the PageBuilder class has a compile error about without the 'cancel' method.

### Install the AutoConnect

Clone or download from the [AutoConnect GitHub repository](https://github.com/Hieromon/AutoConnect).

<img src="./images/gitrepo.png" width="640"/>

When you select Download, you can import it to Arduino IDE immediately. After downloaded, the AutoConnect-master.zip file will be saved in your download folder. Then in the Arduino IDE, navigate to *"Sketch > Include Library"*. At the top of the drop down list, select the option to *"Add .ZIP Library..."*. Details for [Arduino official page](https://www.arduino.cc/en/Guide/Libraries#toc4).

<img src="./images/ins_lib.png" />

!!! info "Supported by Library manager."
    AutoConnect was added to the Arduino IDE library manager as of March 17, 2018. It can be used with the PlatformIO library also.

<script>
  window.onload = function() {
    Gifffer();
  }
</script>
