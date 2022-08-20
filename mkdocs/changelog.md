### [1.3.7] Aug. 20, 2022

- Fixed an authentication failure in Captive Portal state.
- Fixed loss of current SSID.

---
### [1.3.6] Jul. 26, 2022

- Fixed OTA being incomplete.

---
### [1.3.5] Jun. 03, 2022

#### Fix

- Fixed Fixed OTA exit not being called.
- Fixed an ambiguous type call with IPAddress.
- Fixed loss of response due to OTA session reset occurrence.
- Made fit the mqttRSSI examples to ThingSpeak's updated channel authentication.

!!! info "For ESP-IDF 4.4 with Arduino ESP32 Core"
    AutoConnect 1.3.5 is the version compatible with both ESP-IDF 4.4 and ESP-IDF 3.3. It is recommended to use Arduino esp32 core 1.0.6 for ESP-IDF 3.3 based and Arduino esp32 core 2.0.3 or later for ESP-IDF 4.4 based.  
    If you are using PlatformIO as your development platform, you can select any of these two versions by specifying them in **platformio.ini** file.
    
    - For ESP-IDF 4.4 with Arduino ESP32 Core 2.0.3
    
    ```ini
    framework = arduino
    platform = espressif32@4.4.0
    ```
    
    - For ESO-IDF 3.3 with Arduino ESP32 Core 1.0.6
    
    ```ini
    framework = arduino
    platform = espressif32@3.5.0
    ```

---
### [1.3.4] Mar. 02, 2022

#### Enhancements

- Supports [LittleFS_esp32](https://github.com/lorol/LITTLEFS) legacy library with ESP32 Arduino core 1.0.6 or less.
- Added enablement of credentials removal function with Open SSIDs menu.
- Migrate the CI platform to GitHub actions.

#### Fix

- Fixed AutoConnectOTA crashing if there is no OTA partition.
- Fixed AutoConnectUpdate crashing if there is no OTA partition.

---
### [1.3.3] Jan. 25, 2022

#### Fix

- Fixed the missing initialization of MQTT parameter settings of mqttRSSI.ino example sketch.
- Reverted the MQTT API endpoint of Thingspeak.com in the mqttRSSI example sketches.
- Changed ESP32Cam XCLK to be attenuated to avoid interference with WiFi signals.

---
### [1.3.2] Jan. 1, 2022

#### Enhancements

- Supports an [AutoConnectRange](acelements.md#autoconnectrange) as a new AutoConnectElement.
- Adds the [`responsive`](achandling.md#an-http-response-from-the-custom-web-page-handler) parameter with AutoConnectAux.
- Adds an [`AutoConnectAux::redirect`](apiaux.md#redirect) function.
- Adds an example for using AutoConnect with the [ESP32 camera driver](https://github.com/espressif/esp32-camera) as WebCamServer.

#### Fix

- Fixed an issue where a password is lost when SoftAP is stopped.

---
### [1.3.1] Oct. 09, 2021

#### Fixes

- Fixed an issue that was incompatible with ArduinoJson version 5.
- Fixed LittleFS mount check not working with ESP32.
- Fixed autoReconnect not being able to restore a static IP setting.
- Fixed that static IP settings were not cleared when loading credential.

---
### [1.3.0] Sep. 25, 2021

#### Enhancements

- Supports ESP8266 3.0.0 Arduino core.
- Supports ESP32 Arduino core 2.0.0.
- Supports LittleFS with ESP32.
- Supports AutoConnectOTA status notifications.
- Supports AutoConnectConfigAux. (Preview)
- Supports to save credentials always.
- Adds a `style` attribute with AutoConnectInput.
- Adds the `div` tag generation with the AutoConnectElement.
- Adds `[]` operator with const char for AutoConnectAux.
- Adds `[]` operator with `__FlashStringHelper` for AutoConnectAux.
- Adds `AutoConnectAux::content` function to get a number of AutoConnectElements.
- Adds `AutoConnect::getConfig` function to get an actual instance of AutoConnectConfig.

#### Fixes

- Fixed CSS attribute missing of AutoConnectInput with the number type.
- Fixed garbage being mixed in a loaded credential.
- Fixed the output place of Posterior attribute for AutoConnectRadio.
- Improved the the calculation for the size of ArduinoJson document.
- Fixed Incomplete deletion with AutoConnectCredential.
- Fixed credentials not erased correctly.
- Fixed AutoConnectText posterior being unavailable.

---
### [1.2.3] Jan. 3, 2021

#### Enhancements

- Improved memory management.

!!! note "PageBuilder v1.5.0 is required"
    Since AutoConnect v1.2.3, [PageBuilder](https://github.com/Hieromon/PageBuilder) v1.5.0 or later is required. Please update PageBuilder to latest version.

---
### [1.2.2] Dec. 13, 2020

#### Fix

- Fixed an issue where OTA updates would crash on the ESP32 platform. (issue #284)  
  With this fix, `AUTOCONNECT_UPLOAD_ASFIRMWARE_USE_REGEXP` must be enabled for regular expressions to be enabled in `AUTOCONNECT_UPLOAD_ASFIRMWARE`.

---
### [1.2.1] Dec. 5, 2020

#### Fix

- Fixed that not declared error with `AUTOCONNECT_NOUSE_JSON`. (issue #282)

---
### [1.2.0] Dec. 3, 2020

#### New features

- Supports a [whileCaptivePortal](adcpcontrol.md#sketch-execution-during-the-captive-portal-loop) exit. (issue #149, issue #244)
- Supports an [onConnect](adconnection.md#detects-connection-establishment-to-ap) exit.
- Supports [HTTP authentication](adauthentication.md). (issue #171)

#### Enhancements

- Added `AUTOCONNECT_APKEY_SSID` definition to [seek access points by SSID](adconnection.md#match-with-known-access-points-by-ssid). (issue #251)
- Added [AutoConnect::append](api.md#append) and [AutoConnect::detach](api.md#detach) function that can be dynamically AutoConnectAux attaching and detaching. (issue #230)
- Added [AutoConnect::getEEPROMUsedSize](adcredential.md#move-the-saving-area-of-eeprom-for-the-credentials) that notifies the occupied size of the credential storage area. (issue #209)
- Added [AutoConnectConfig::beginTimeout](adconnection.md#timeout-settings-for-a-connection-attempt) setting. (issue #247)
- Added [AutoConnectConfig::preserveAPMode](adconnection.md#preserve-ap-mode) setting. (issue #210)
- Enable support for the [*LittleFS*](adothers.md#choice-of-the-filesystem-for-esp8266) as [filesystem](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#spiffs-and-littlefs) with ESP8266 platform.
- Enhanced [AutoConnectInput](acelements.md#autoconnectinput) to allow accepts [password](acelements.md#apply) and [number](acelements.md#apply) type. (issue #237, issue #255)
- Enhanced handleClient to [dynamically launch the captive portal](adcpcontrol.md#launch-the-captive-portal-on-demand-at-losing-wifi) when losing WiFi connection.
- Enhanced the ability to upload a regular file with [AutoConnectOTA](otabrowser.md#regular-file-uploading-using-autoconnectotaenhanced-wv120). (issue #236)
- Enhanced ticker to work even in handleClient loop.
- Improved [autoReconnect](adconnection.md#automatic-reconnect-background) to work even in handleClient loop. (issue #234, issue #251)

#### Fixes

- Avoids an empty-body warning when `AC_DEBUG` is disabled. (issue #218)
- Fixed a core panic in the regex with ESP32.
- Fixed an exception in the AutoConnect::end function.
- Fixed an invalid SPIFFS compile error with ESP32.
- Fixed deficiently forward references with HandleClient.ino example. (PR #275)
- Fixed incorrect connection wait time. (issue #216)
- Fixed not being able to specify channel ID with a mqttRSSI.ino example. (issue #262)
- Fixed posterior being disabled in AutoConnectElement.

---
### [1.1.7] Apr. 19, 2020

#### Fixes

- Fixed Apply button not work.

---
### [1.1.6] Apr. 17, 2020

#### Fixes

- Fixed OTA page translation not work.

---
### [1.1.5] Apr. 15, 2020

#### New features

- Supports AutoConnect [menu configuration](menu.md#applying-the-active-menu-items).
- Supports the built-in OTA feature as [AutoConnectOTA](otabrowser.md#updates-with-the-web-browserupdated-wv115).

#### Enhancements

- Enhanced allows the AutoConnect::begin to connect to the access point in order of signal strength. This option can specify the order of [connection attempting according to the WiFi signal strength](adconnection.md#connects-depending-on-the-wifi-signal-strength) indicated with RSSI.
- Changed the [bootUri behavior](advancedusage.md#assign-user-sketchs-home-path) to be an automatic pop-up at the captive portal.

---
### [1.1.4] Feb. 14, 2020

#### New features

- Supports for overriding text of the menu items with user-defined labels.

#### Fixes

- Fixed the compiler warning with experimental WiFi mode of ESP8266.

---
### [1.1.3] Jan. 1, 2020

#### Enhancements

- Improved Config New button behavior.
- Added `AUTOCONNECT_NOUSE_JSON` directive

#### Fixes

- Fixed relocate Config New menu URI inability.
- Removed compiler warning of unused.

---
### [1.1.2] Oct. 22, 2019

#### Fixes

- Fixed a crash when no SSID found.
- Fixed memory leak on destruction.

---
### [1.1.1] Oct. 17, 2019

#### Fixes

- Fixed crash with unique_ptr deleting reference content.
- Fixed disconnection request initialization missing.

---
### [1.1.0] Oct. 15, 2019

#### Enhancements

- Enhanced to be able to specify static IP in the [**Configure new AP**](menu.md#configure-new-ap) menu.

#### Fixes

- Fixed compilation error that no member named 'printTo' with ArduinoJson version 5.

---
### [1.0.3] Sept. 30, 2019

#### Fixes

- Fixed a return of AutoConnectCredential::entries().

---
### [1.0.2] Sept. 19, 2019

#### Fixes

- Fixed compilation error that no member named 'success' with ArduinoJson version 5.
- Fixed SSID non termination.
- Fixed compilation error that getBytesLength missing with ESP32.
- Added #include directive restriction for EEPROM and ESP8266httpUpdate to FAQ.

---
### [1.0.1] Sept. 13, 2019

#### Enhancements

- Added an example sketch for ESP32 boards that migrates credentials stored in EEPROM partition to the Preferences.

---
### [1.0.0] Sept. 7, 2019

#### New features

- Supports AutoConnectUpdate for the [OTA update](otaupdate.md).

#### Enhancements

- Supported Arduino core for ESP32 1.0.3.
- Added [AutoConnectAux::isValid](apiaux.md#isvalid) function.
- Added the [`global`](achandling.md#transfer-of-input-values-across-pages) attribute with all AutoConnectElements.
- Changed the credential storage area to Preferences with ESP32 core 1.0.3 and later. **In ESP32, the credentials stored past in EEPROM will lose**.

---
### [0.9.12] Aug. 18, 2019

#### Fixes

- Fixed missing captive portal notifications on the newer mobile OS client. As a result of this fix, the SoftAP default IP address and gateway have been changed to `172.217.28.1`.

---
### [0.9.11] July 13, 2019

#### New features

- Supports new element as AutoConnectSytle that can insert the custom CSS into AutoConnectAux page.

#### Enhancements

- Supports that `<br>` tags can now be added to each element.
- Supports that able to place the checkbox label forward or backward.
- Supports flicker signal output according to the status of WiFi_mode.
- Supports [AutoConnectAux::fetchElement](apiaux.md#fetchelement) function to retrieve inputted element values via a custom Web page.

#### Fixes

- Fixed bug in AutoConnectCredential when offset is >256.

---
### [0.9.10] June 12, 2019

#### Fixes

- Fixed the unable to get AutoConnectElemets values ​​in the sketch with ESP8266 arduino core 2.5.2.

---
### [0.9.9] May 25, 2019

#### Enhancements

- Supports ESP8266 Arduino core 2.5.2.
- Menu text/background color can be statically customized.
- Added the [enable](achandling.md#enable-autoconnectelements-during-the-sketch-execution)  attribute to the AutoConnectElements. This attribute gives dynamically change to the element activation during the sketch executing.
- Added ID attribute to HTML tag generated from AutoConnectText.

#### Fixes

- Fixed the input box layout collapsed.
- Fixed that the decoration of AutoConnectButton was disabled.
- Fixed that the value remains even after clearing the option with AutoConnectSelect.
- Fixed that an alignment violation exception occurred when loading AutoConnectAux described by JSON with PROGMEM attribute.

---
### [0.9.8] May 3, 2019

#### New features

- Supports new element type [AutoConnectFile](acelements.md#autoconnectfile) and built-in file uploader.

#### Enhancements

- Enhanced to support ArduinoJson 6.9.1 or later.
- Enhanced to use PSRAM on ESP32 module as the buffer allocation destination of JsonDocument with ArduinoJson 6.10.0 or later.
- Added an [operator`[]`](apiaux.md#operator) as a shortcut for AutoConnectAux::getElement function.
- Added an [AutoConnectElement::as<T\>](apielements.md#ast62) function to easily coding for conversion from an AutoConnectElement to an actual type.
- Added a [format](acelements.md#format) attribute with the AutoConnectText element.
- Added a [selected](acelements.md#selected) attribute with the AutoConnectSelect element.
- Enhanced [AutoConnectAux::loadElement](apiaux.md#loadelement) with multiple elements loading.
- Changed menu labels placement in source files structure.
- Changed API interface of [AutoConnect::where](api.md#where) function.

#### Fixes

- Fixed blank page responds with Configure new.
- Fixed loading elements value missing.
- Fixed losing elements in saveElement with ArduinoJson version 6.
- Fixed compile error with older than ESP8266 core 2.5.0.

---
### [0.9.7] Jan. 25, 2019

#### New features

- Supports AutoConnect menu extension by user sketch with [AutoConnectAux](acintro.md).
- Supports loading and saving of user-defined parameters with JSON format.

#### Enhancements

- Improved the WiFi connection sequence at the first WiFi.begin. Even if [AutoConnectConfig::autoReconnect](apiconfig.md#autoreconnect) is disabled when SSID and PSK are not specified, it will use the information of the last established access point. The autoReconnect option will achieve trying the connect after a previous connection failed.
- Added the [AutoConnectConfig::immediateStart](apiconfig.md#immediatestart) option and immediately starts the portal without first trying WiFi.begin. You can start the captive portal at any time in combination with the [AutoConnectConfig::autoRise](apiconfig.md#autorise) option.
- Improved boot uri after reset. [AutoConnectConfig::bootUri](apiconfig.md#booturi) can be specified either /_ac or HOME path as the uri to be accessed after invoking Reset from AutoConnect menu.
- Improved source code placement of predefined macros. Defined common macros have been moved to `AutoConnectDefs.h`.
- Added [AutoConnectConfig::hostName](apiconfig.md#hostname). It activates `WiFi.hostname()`/`WiFi.setHostName()`.
- Added the captive portal time-out. It can be controlled by [AutoConnectConfig::portalTimeout](apiconfig.md#portaltimeout) and [AutoConnectConfig::retainPortal](apiconfig.md#retainportal).

---
#### Fixes

- Fixed crash in some environments. Thank you @ageurtse

### [0.9.6] Sept.27, 2018.

#### Enhancements

- Improvement of RSSI detection for saved SSIDs.

#### Fixes

- Fixed disconnection SoftAP completely at the first connection phase of the [AutoConnect::begin](api.md#begin).

---
### [0.9.5] Aug.27, 2018.

#### Enhancements

- Supports ESP32.

#### Fixes

- Fixed that crash may occur if the number of stored credentials in the EEPROM is smaller than the number of found WiFi networks.

---
### [0.9.4] May 5, 2018.

#### New features

- Supports [AutoConnectConfig::autoReconnect](apiconfig.md#autoreconnect) option, it will scan the WLAN when it can not connect to the default SSID, apply the applicable credentials if it is saved, and try reconnecting.

#### Enhancements

- Automatically focus Passphrase after selecting SSID with Configure New AP.

---
### [0.9.3] March 23, 2018.

#### Enhancements

- Supports a static IP address assignment.

---
### [0.9.2] March 19, 2018.

#### Enhancements

- Improvement of string literal declaration with the examples, no library change.

---
### [0.9.1] March 13, 2018.

- A release of the stable.
