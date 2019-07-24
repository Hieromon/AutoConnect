## A simple update server for AutoConnect update feature

AutoConnect since v1.0.0 release provides a new function for updating the sketch firmware of ESP8266 or ESP32 module via OTA as AutoConnectUpdate class. The AutoConnectUpdate class is an implementation of the sketch binary update by the HTTP server mentioned in the OTA update of the ESP8266 document, which inherits from the ESP8266 HTTPUpdate class (HTTPUpdate class in the case of ESP32). It acts as a client agent for a series of update operations.

<img src="../../mkdocs/images/updateserver.png" width="380" />

updateserver.py

Details for the [AutoConnect documentation](https://hieromon.github.io/AutoConnect/otaupdate.html#updates-with-the-update-server).
