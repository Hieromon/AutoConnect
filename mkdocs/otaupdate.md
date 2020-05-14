## OTA Updates with AutoConnect

AutoConnect provides **two type platforms** for updating the binary sketch in the ESP8266 or ESP32 module via OTA. They correspond to the [Web Browser Update](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html#web-browser) and [HTTP Server Update](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html#http-server) whiches mentioned in the [ESP8266 Arduino Core documentation](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html#ota-updates).

[**The update behavior using a web browser**](otabrowser.md) as the client that supplies the binary sketch file for update follows the scenario assumed by the ESP8266 Arduino core. Therefore, the user sketch must meet the requirements described in the ESP8266 Arduino Core documentation, but you can easily embed the OTA update feature that able to operate via the web browser by AutoConnect menu. All you need to do is that specify [AutoConnectConfig](apiconfig.md#ota).

<img src="images/webupdatemodel.png" width="420" />

!!! caution "It is for the only the same network"
    This method can apply only if the client browser and the ESP module belong to the same network segment. It cannot work correctly across networks.

[**Another update method using an update server**](otaserver.md) can be applied more broadly than using a web browser. This method can also update the ESP module over the Internet if you can secure the correct route and transparency between the ESP module and the update server. To configure this platform, you need to have an [update server](otaserver.md#update-server-for-the-autoconnectupdate-class) along with using the [AutoConnectUpdate](apiupdate.md) class in your sketch.

<img src="images/updatemodel.png" width="540" />

!!! info "Security Disclaimer"
    The security level of the OTA update platform provided by AutoConnect is very weak. No guarantees as to the level of security for your application by the AutoConnect OTA Update is implied.
