!!! info "Luxbar"
    The AutoConnect menu is developed using the [LuxBar](https://github.com/balzss/luxbar) which is licensed under the MIT License. See the [License](license.md).

## <i class="fa fa-external-link"></i> Where the from

The AutoConnect menu appears when you access the **AutoConnect root path**. It is assigned "**/_ac**" located on the *local IP address* of ESP8266/ESP32 module by default. This location can be changed in the sketch. The following screen will appear at access to `http://{localIP}/_ac` as the root path. This is the statistics of the current WiFi connection. You can access the menu from the here, to invoke it tap <i class="fa fa-bars"></i> at right on top. (e.g. `http://192.168.244.1/_ac` for SoftAP mode.)

<img src="../images/ac.png" style="border-style:solid;border-width:1px;border-color:lightgrey;width:280px;" />

!!! note "What's the local IP?"
    A local IP means Local IP at connection established or SoftAP's IP.

## <i class="fa fa-bars"></i> Right on top

Currently, AutoConnect supports four menus. Undermost menu as "HOME" returns to the home path of its sketch.

- **Configure new AP**: Configure SSID and Password for new access point.
- **Open SSIDs**: Opens the past SSID which has been established connection from EEPROM.
- **Disconnect**: Disconnects current connection.
- **Reset...**: Rest the ESP8266/ESP32 module.
- **HOME**: Return to user home page.

<img src="../images/menu.png" style="width:280px;" />

## <i class="fa fa-bars"></i> Configure new AP

Scan all available access point in the vicinity and display it. Strength and security of the detected AP are marked. The <i class="fa fa-lock"></i> is indicated for the SSID that needs a security key. "**Hidden:**" means the number of hidden SSIDs discovered.  
Enter SSID and Passphrase and tap "**apply**" to starts WiFi connection. 

<img src="../images/newap.png" style="border-style:solid;border-width:1px;border-color:lightgrey;width:280px;" />

## <i class="fa fa-bars"></i> Open SSIDs

Once it was established WiFi connection, its SSID and password will be saved in EEPROM of ESP8266/ESP32 automatically. The **Open SSIDs** menu reads the saved SSID credentials from the EEPROM. The stored credential data are listed by the SSID as shown below. Its label is a clickable button. Tap the SSID button, starts WiFi connection it.

<img src="../images/open.png" style="border-style:solid;border-width:1px;border-color:lightgrey;width:280px;" />

## <i class="fa fa-bars"></i> Disconnect

Disconnect ESP8266/ESP32 from the current connection. It can also reset the ESP8266/ESP32 automatically after disconnection by instructing with using [API](api.md#autoreset) in the sketch.

After tapping "Disconnect", you will not be able to reach the AutoConnect menu. Once disconnected, you will need to set the SSID again for connecting the WLAN. 

## <i class="fa fa-bars"></i> Reset...

Reset the ESP8266/ESP32 module, it will start rebooting. After rebooting complete, the ESP8266/ESP32 module begins establishing the previous connection with WIFI_STA mode, and *esp8266ap* or *esp32ap* of an access point will disappear from WLAN.

<img src="../images/resetting.png" style="width:280px;" />

!!! warning "Not every ESP8266 module will be rebooted normally"
    The Reset menu is using the **ESP.reset()** function for ESP8266. This is an almost hardware reset. In order to resume the sketch normally, the [state of GPIO0](https://github.com/esp8266/esp8266-wiki/wiki/Boot-Process#esp-boot-modes) is important. Since this depends on the circuit implementation for each module, not every module will be rebooted normally. See also [FAQ](faq.md#hang-up-after-reset).