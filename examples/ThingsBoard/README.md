# main.cpp

Sketch example to show how to enable ThingsBoard MQTT.

### Build the sketch and start project

Run the build in the PlatformIO IDE. PlatformIO IDE will compile the sketch successfully and uploads it to the ESP module. You can open the serial monitor in advance to monitor the execution status of the sketch. 

On PlatformIO create a new project esp32dev and add the librairies for:
hieromon/AutoConnect
thingsboard/ThingsBoard
knolleary/PubSubClient
rpolitex/ArduinoNvs

The first time you execute main, the captive portal will launch due to cannot connect to the Internet, and you will see the WiFi Access Point as `ESP-XXXXXX` or `esp8266ap`, `esp32ap` via your cellphone. Connect to it and use [Configure new AP](https://hieromon.github.io/AutoConnect/menu.html#configure-new-ap) from the [AutoConnect menu](https://hieromon.github.io/AutoConnect/menu.html) will allow the ESP module to connect to WiFi Access Point to reach the Internet.

Please create a new device on the thingsboard server [Configure MQTT](https://thingsboard.io/docs/getting-started-guides/helloworld/) and then add the settings to AutoConnect MQTT settings tab, once saved the device should be connected to thingsboard and should send temperature values, which in this case are random integers. 

