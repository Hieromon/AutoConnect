#### [0.9.5] Aug.27, 2018.

- Supports ESP32.
- Fixed that crash may occur if the number of stored credentials in the EEPROM is smaller than the number of found WiFi networks.

#### [0.9.4] May 5, 2018.

- Automatically focus passphrase after selecting SSID with Configure New AP.
- Supports AutoConnectConfig::autoReconnect option, it will scan the WLAN when it can not connect to the default SSID, apply the applicable credentials if it is saved, and try reconnecting.

#### [0.9.3] March 23, 2018.

- Supports a static IP address assignment.

#### [0.9.2] March 19, 2018.

- Improvement of string literal declaration with the examples, no library change.

#### [0.9.1] March 13, 2018.

- A release of the stable.