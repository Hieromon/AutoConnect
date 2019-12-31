#### [1.1.3] Jan. 1, 2020
- Added AUTOCONNECT_NOUSE_JSON directive
- Removed compiler warning of unused.
- Improved Config New button behavior.
- Fixed relocate Config New menu URI inability.

#### [1.1.2] Oct. 22, 2019
- Fixed crash when no SSID found.
- Fixed memory leak on destruction.

#### [1.1.1] Oct. 17, 2019
- Fixed crash with unique_ptr deleting reference content.
- Fixed disconnection request initialization missing.

#### [1.1.0] Oct. 15, 2019
- Supports static IPs with the [**Configure new AP**](menu.md#configure-new-ap) menu.
- Fixed compilation error that no member named 'printTo' with ArduinoJson 5.

#### [1.0.3] Sept. 30, 2019
- Fixed a return of AutoConnectCredential::entries().

#### [1.0.2] Sept. 19, 2019
- Fixed compilation error that no member named 'success' with ArduinoJson 5.
- Fixed SSID non termination.
- Fixed compilation error that getBytesLength missing with ESP32.
- Added #include directive restriction for EEPROM and ESP8266httpUpdate to FAQ.

#### [1.0.1] Sept. 13, 2019
- Added a sketch for ESP32 boards that migrates credentials stored in EEPROM partition to the Preferences.

#### [1.0.0] Sept. 7, 2019
- Supports Arduino core for ESP32 1.0.3.
- Supports AutoConnectUpdate for the [OTA update](otaupdate.md).
- Supports Preferences for saving credentials with ESP32 core 1.0.3 and later. **In ESP32, the credentials stored past in EEPROM will lose**.
- Supports [**AutoConnectAux::isValid**](apiaux.md#isvalid) function.
- Supports the [**global**](achandling.md#transfer-of-input-values-across-pages) attribute with all AutoConnectElements.

#### [0.9.12] Aug. 18, 2019
- Fixed missing captive portal notifications on the newer mobile OS client. As a result of this fix, the SoftAP default IP address and gateway have been changed to **172.217.28.1**.

#### [0.9.11] July 13, 2019
- Supports new element as AutoConnectSytle that can insert the custom CSS into AutoConnectAux page.
- Supports that `<br>` tags can now be added to each element.
- Supports that able to place the checkbox label forward or backward.
- Supports flicker signal output according to the status of WiFi_mode.
- Supports [**AutoConnectAux::fetchElement**](apiaux.md#fetchelement) function to retrieve inputted element values via a custom Web page.
- Fixed bug in AutoConnectCredential when offset is >256.

#### [0.9.10] June 12, 2019
- Fixed the unable to get AutoConnectElemets values ​​in the sketch with ESP8266 arduino core 2.5.2.

#### [0.9.9] May 25, 2019
- Supports ESP8266 Arduino core 2.5.2.
- Menu text/background color can be statically customized.
- Added the [enable](achandling.md#enable-autoconnectelements-during-the-sketch-execution)  attribute to the AutoConnectElements. This attribute gives dynamically change to the element activation during the sketch executing.
- Added ID attribute to HTML tag generated from AutoConnectText.
- Fixed the input box layout collapsed.
- Fixed that the decoration of AutoConnectButton was disabled.
- Fixed that the value remains even after clearing the option with AutoConnectSelect.
- Fixed that an alignment violation exception occurred when loading AutoConnectAux described by JSON with PROGMEM attribute.

#### [0.9.8] May 3, 2019
- Supports ArduinoJson 6.9.1 or later.
- Supports allocating JsonDocument buffer to PSRAM on ESP32 with ArduinoJson 6.10.0 or later.
- Supports [**operator`[]`**](apiaux.md#operator) as a shortcut for AutoConnectAux::getElement function.
- Supports [**AutoConnectElement::as<T\>**](apielements.md#ast62) function to easily coding for conversion from an AutoConnectElement to an actual type.
- Supports new element type [**AutoConnectFile**](acelements.md#autoconnectfile) and built-in file uploader.
- Supports a [**format**](acelements.md#format) attribute with the AutoConnectText element.
- Supports a [**selected**](acelements.md#selected) attribute with the AutoConnectSelect element.
- Supports multiple element loading with [AutoConnectAux::loadElement](apiaux.md#loadelement).
- Changed menu labels placement in source files structure.
- Changed API interface of [**AutoConnect::where](api.md#where) function.
- Fixed blank page responds with Configure new.
- Fixed loading elements value missing.
- Fixed losing elements in saveElement with ArduinoJson V6.
- Fixed compile error with older than ESP8266 core 2.5.0.

#### [0.9.7] Jan. 25, 2019

- Fixed crash in some environments. Thank you @ageurtse
- Supports AutoConnect menu extension by user sketch with [**AutoConnectAux**](acintro.md).
- Supports loading and saving of user-defined parameters with JSON format.
- Improved the WiFi connection sequence at the first WiFi.begin. Even if [**AutoConnectConfig::autoReconnect**](apiconfig.md#autoreconnect) is disabled when SSID and PSK are not specified, it will use the information of the last established access point. The autoReconnect option will achieve trying the connect after a previous connection failed.
- Supports the [**AutoConnectConfig::immediateStart**](apiconfig.md#immediatestart) option and immediately starts the portal without first trying WiFi.begin. You can start the captive portal at any time in combination with the [**AutoConnectConfig::autoRise**](apiconfig.md#autorise) option.
- Improved boot uri after reset. [**AutoConnectConfig::bootUri**](apiconfig.md#booturi) can be specified either /_ac or HOME path as the uri to be accessed after invoking Reset from AutoConnect menu.
- Improved source code placement of predefined macros. Defined common macros have been moved to ```AutoConnectDefs.h```.
- Supports [**AutoConnectConfig::hostName**](apiconfig.md#hostname). It activates ```WiFi.hostname()```/```WiFi.setHostName()```.
- Supports the captive portal time-out. It can be controlled by [**AutoConnectConfig::portalTimeout**](apiconfig.md#portaltimeout) and [**AutoConnectConfig::retainPortal**](apiconfig.md#retainportal).

#### [0.9.6] Sept.27, 2018.

- Improvement of RSSI detection for saved SSIDs.
- Fixed disconnection SoftAP completely at the first connection phase of the [**AutoConnect::begin**](api.md#begin).

#### [0.9.5] Aug.27, 2018.

- Supports ESP32.
- Fixed that crash may occur if the number of stored credentials in the EEPROM is smaller than the number of found WiFi networks.

#### [0.9.4] May 5, 2018.

- Automatically focus passphrase after selecting SSID with Configure New AP.
- Supports [**AutoConnectConfig::autoReconnect**](apiconfig.md#autoreconnect) option, it will scan the WLAN when it can not connect to the default SSID, apply the applicable credentials if it is saved, and try reconnecting.

#### [0.9.3] March 23, 2018.

- Supports a static IP address assignment.

#### [0.9.2] March 19, 2018.

- Improvement of string literal declaration with the examples, no library change.

#### [0.9.1] March 13, 2018.

- A release of the stable.
