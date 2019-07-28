## OTA Updates with AutoConnect

AutoConnect provides **two types of the platform** for updating the binary sketch stored in the ESP8266 or ESP32 module via OTA. They correspond to the [Web Browser Update](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html) and [HTTP Server Update](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html#http-server) mentioned in the [ESP8266 Arduino Core documentation](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html#ota-updates).

[**The update behavior when using a web browser**](otabrowser.md) as an update client keeps with the scenario assumed by the ESP8266 arduino core. Therefore, the user sketch must meet the requirements described in the ESP8266 Arduino Core documentation, but it is not difficult to incorporate an implementation for that into a sketch using AutoConnect. All you need to do is bind the **ESP8266HTTPUpdateServer library class**[^1] of the ESP8266 Arduino core as an [AutoConnectAux](acintro.md) to the [AutoConnect menu](menu.md).

[^1]: The AutoConnect library provides an implementation of the **HTTPUpdateServer** class that ported from ESP8266HTTPUpdateServer class for ESP32 intention. It is contained the **WebUpdate** under the examples folder.

<img src="images/webupdatemodel.png" width="420" />

!!! caution "It is for the only the same network"
    This method can apply only if the client browser and the ESP module belong to the same network segment. It cannot work correctly across networks.

[**Another update method using an update server**](otaserver.md) can be applied more broadly than using a web browser. This method can also update the ESP module over the Internet if you can secure the correct route and transparency between the ESP module and the update server. To configure this platform, you need to have an [update server](otaserver.md#update-server-for-the-autoconnectupdate-class) along with using the [AutoConnectUpdate](apiupdate.md) class in your sketch.

<img src="images/updatemodel.png" width="540" />

!!! info "Security Disclaimer"
    The security of the OTA update platform provided by AutoConnect is a very weak level. No guarantees as to the level of security provided for your application by the AutoConnect OTA Update is implied.
