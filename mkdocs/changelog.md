#### [0.9.8] Apr. 25, 2019
- Supports ArduinoJson 6.9.1 or later.
- Supports allocating JsonDocument buffer to PSRAM on ESP32 with ArduinoJson 6.10.0 or later.
- Supports [**operator`[]`**](apiaux.md#operator) as a shortcut for AutoConnectAux::getElement function.
- Supports [**AutoConnectElement::as<T\>**](apielements.md#ast62) function to easily coding for conversion from an AutoConnectElement to an actual type.
- Supports new element type [**AutoConnectFile**](acelements.md#autoconnectfile) and built-in file uploader.
- Supports a [**format attribute**](acelements.md#format) with the AutoConnectText element.
- Fixed blank page responds with Configure new.
- Changed menu labels placement in source files structure.

#### [0.9.7] Jan. 25, 2019

- Fixed crash in some environments. Thank you @ageurtse
- Supports AutoConnect menu extention by user sketch with [**AutoConnectAux**](acintro.md).
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
