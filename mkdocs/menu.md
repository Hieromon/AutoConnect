!!! info "Luxbar"
    The AutoConnect menu is developed using the [LuxBar](https://github.com/balzss/luxbar) which is licensed under the MIT License. See the [License](license.md).

## <i class="fa fa-external-link"></i> Where the from

The following screen will appear as the AutoConnect menu when you access to **AutoConnect root URL** via `http://{localIP}/_ac`. (eg. `http://172.217.28.1/_ac`) It is a top page of AutoConnect which shows the current WiFi connection statistics. To invoke the AutoConnect menu, you can tap <i class="fa fa-bars"></i> at right on top.

<img src="images/ac.png" style="border-style:solid;border-width:1px;border-color:lightgrey;width:280px;" />

!!! note "AutoConnect root URL"
    It is assigned "**/_ac**" located on the *local IP address* of ESP8266/ESP32 module by default and can be changed with the Sketch. A local IP means Local IP at connection established or SoftAP's IP.

## <i class="fa fa-bars"></i> Right on top

Currently, AutoConnect supports six menus. Undermost menu as "HOME" returns to the home path of its sketch.

- **Configure new AP**: Configure SSID and Password for new access point.
- **Open SSIDs**: Opens the past SSID which has been established connection from the flash.
- **Disconnect**: Disconnects current connection.
- **Reset...**: Rest the ESP8266/ESP32 module.
- **Update**: OTA updates. (Optional)
- **HOME**: Return to user home page.

<img src="images/menu.png" style="width:280px;" />

## <i class="fa fa-bars"></i> Configure new AP

It scans all available access points in the vicinity and display it further the WiFi signal strength and security indicator as <i class="fa fa-lock"></i> of the detected AP. Below that, the number of discovered hidden APs will be displayed. 
Enter SSID and Passphrase and tap "**Apply**" to start a WiFi connection. 

<img src="images/newap.png" style="border-style:solid;border-width:1px;border-color:lightgrey;width:280px;" />

If you want to configure with static IP, uncheck "**Enable DHCP**". Once the WiFi connection is established, the entered static IP[^1] configuration will be stored to the credentials in the flash and restored to the station configuration via the [Open SSIDs](#open-ssids) menu.

[^1]: AutoConnect does not check the syntax and validity of the entered IP address. If the entered static IPs are incorrect, it cannot connect to the access point.

<img src="images/newap_static.png" style="border-style:solid;border-width:1px;border-color:lightgrey;width:280px;" />

## <i class="fa fa-bars"></i> Open SSIDs

After WiFi connected, AutoConnect will automatically save the established SSID and password to the flash on the ESP module. **Open SSIDs** menu reads the saved SSID credentials and lists them as below. Listed items are clickable buttons and can initiate a connection to its access point.

<img src="images/openssid.png" style="border-style:solid;border-width:1px;border-color:lightgrey;width:280px;" />

Also, this menu allows you to interactively delete the stored credentials. <i class="fa fa-trash-alt"></i> icon will appear next to each SSID in the Open SSIDs menu when the credential removal feature is enabled with [AutoConnectConfig::menuItems](apiconfig.md#menuitems). Clicking the <i class="fa fa-trash-alt"></i> on this screen will delete the SSID. This feature is disabled by default.

<img src="images/openssid_trash.png" style="border-style:solid;border-width:1px;border-color:lightgrey;width:280px;" />

!!! note "Saved credentials data structure has changed"
    A structure of AutoConnect saved credentials has changed in v1.1.0 and was lost backward compatibility. Credentials saved by AutoConnect v1.0.3 (or earlier) will not display properly with AutoConnect v1.1.0. You need to erase the flash of the ESP module using the esptool before the Sketch uploading.
    ```powershell
    esptool -c esp8266 (or esp32) -p [COM_PORT] erase_flash
    ```

## <i class="fa fa-bars"></i> Disconnect

It disconnects ESP8266/ESP32 from the current connection. Also, ESP8266/ESP32 can be automatically reset after WiFi cutting by instructing with the Sketch using the [AutoConnect API](api.md#autoreset).

After tapping the **Disconnect**, you will not be able to reach the AutoConnect menu. Once disconnected, you will need to set the SSID again for connecting to the WLAN. 

## <i class="fa fa-bars"></i> Reset...

Resetting the ESP8266/ESP32 module will initiate a reboot. When the module restarting, the *esp8266ap* or *esp32ap* access point will disappear from the WLAN and the ESP8266/ESP32 module will begin to reconnect a previous access point with WIFI_STA mode.

<img src="images/resetting.png" style="width:280px;" />

!!! warning "Not every ESP8266 module will be rebooted normally"
    The Reset menu is using the **ESP.reset()** function for ESP8266. This is an almost hardware reset. In order to resume the Sketch normally, the [state of GPIO0](https://github.com/esp8266/esp8266-wiki/wiki/Boot-Process#esp-boot-modes) is important. Since this depends on the circuit implementation for each module, not every module will be rebooted normally. See also [FAQ](faq.md#hang-up-after-reset).

## <i class="fa fa-bars"></i> Custom menu items

If the Sketch has custom Web pages, the AutoConnect menu lines them up with the AutoConnect's items. Details for [Custom Web pages in AutoConnect menu](acintro.md#custom-web-pages-in-autoconnect-menu).

## <i class="fa fa-bars"></i> Update

If you specify [AutoConnectConfig::ota](apiconfig.md#ota) to import the OTA update feature into Sketch, an item will appear in the menu list as **Update**.

!!! note "The Update menu item will appear only AutoConnectOTA enabled"
    The Update item is displayed automatically in the menu only when [AutoConnectConfig::ota](apiconfig.md#ota) is specified with **AC_OTA_BUILTIN** or [AutoConnectUpdate](otaserver.md#how-to-embed-autoconnectupdate-to-your-sketch) is attached.

## <i class="fa fa-bars"></i> HOME

A **HOME** item at the bottom of the menu list is a link to the home path, and the default URI is `/` which is defined by `AUTOCONNECT_HOMEURI` in **AutoConnectDefs.h** header file.

```cpp
#define AUTOCONNECT_HOMEURI     "/"
```

Also, you can change the HOME path using the AutoConnect API. The [**AutoConnect::home**](api.md#home) function sets the URI as a link of the HOME item in the AutoConnect menu.

## <i class="fa fa-bars"></i> Applying the active menu items

Each of the above menu items can be configured with a Sketch. [AutoConnectConfig::menuItems](apiconfig.md#menuitems) specifies the menu items that will be enabled at runtime. You can also adjust available menu items using [AutoConnect::enableMenu](api.md#enablemenu) and [AutoConnect::disableMenu](api.md#disablemenu) function. It is an alternative to [AutoConnectConfig::menuItems](apiconfig.md#menuitems) and provides a shortcut to avoid using AutoConnectConfig.  
For example, by disabling the [Configure new AP](#configure-new-ap) and [Disconnect](#disconnect) item, you can prevent the configuration for unknown access points.

```cpp
AutoConnect portal;
AutoConnectConfig config;

void setup() {
  config.menuItems = AC_MENUITEM_OPENSSIDS | AC_MENUITEM_RESET | AC_MENUITEM_HOME;
  portal.config(config);
}
```

Following code snippet is another way to achieve the same effect.

```cpp
AutoConnect portal;

void setup() {
  portal.disableMenu(AC_MENUITEM_CONFIGNEW | AC_MENUITEM_DISCONNECT);
  portal.config(config);
}
```

Here is the result of running the above sketch:

<img src="images/applymenu.png" style="border-style:solid;border-width:1px;border-color:lightgrey;width:280px;" />

[AutoConnectConfig::menuItems](apiconfig.md#menuitems) section has more details.

AutoConnect shows and hides menu items when [AutoConnect::begin](api.md#begin) is executed and when [AutoConnect::handleClient](api.md#handleclient) is executed in a `loop` function. You can dynamically change the available menu items during the *loop()* by setting the show/hide items before executing those functions with [AutoConnect::enableMenu](api.md#enablemenu) and [AutoConnect::disableMenu](api.md#disablemenu).

The current menu item settings held by AutoConnectConfig can be retrieved with the [AutoConnect::getConfig](api.md#getconfig) function, and the code snippet to reconfigure menu items based on the `getConfig` return value is as follows:

#### Enable OTA menu on demand using an external switch connected to a GPIO.

<img src="images/menu_ondemand.png" width="320px" />

```cpp
const int externalSwitch = 5;  // Assign the OTA activation switch to D1 (GPIO5).
AutoConnect portal;
AutoConnectConfig config;

void setup() {
  // The logic level of the external switch assumes active LOW.
  // Note: A said switch is an alternate and must retain its state.
  pinMode(externalSwitch, INPUT_PULLUP);

  // Set up OTA, but hide the Update item from the menu list until an external
  // switch is pressed.
  config.ota = AC_OTA_BUILTIN;
  portal.config(config);
  portal.disableMenu(AC_MENUITEM_UPDATE);
  portal.begin(); 
}

void loop() {
  // Use AutoConnect::getConfig to obtain the current AutoConnectConfig values
  // and indicate the display state of an Update item.
  bool  menuUpdate = portal.getConfig().menuItems & AC_MENUITEM_UPDATE;

  // Hides the Update item from the menu list depending on the state of the switch.
  if (digitalRead(externalSwitch) == LOW && !(menuUpdate)) {
    portal.enableMenu(AC_MENUITEM_UPDATE);
  }
  else {
    portal.disableMenu(AC_MENUITEM_UPDATE);
  }

  portal.handleClient();
}
```
!!! note "enableMenu/disableMenu has no effect for custom web page items"
    *AutoConnect::enableMenu* and *disableMenu* functions are not enabled to show/hide menu items for [custom web pages](acintro.md). They only work on AutoConnect's built-in pages[^2]. Use the [AutoConnectAux::menu](apiaux.md#menu) and [AutoConnectAux::isMenu](apiaux.md#ismenu) functions to show/hide menu items for custom web pages. For more information, see [Custom Web pages in AutoConnect menu](acintro.md#custom-web-pages-in-autoconnect-menu) section.

[^2]: AutoConnect built-in pages are predefined by the [AC_MENUITEM_t](api.md#enablemenu) enum value.

## <i class="fa fa-bars"></i> Attaching to AutoConnect menu

The AutoConnect menu can contain your sketch's web pages as extra items as a custom. It works for HTML pages implemented by the **ESP8266WebServer::on** handler or the **WebServer::on** handler for ESP32. That is, you can make them invoke the legacy web pages from the AutoConnect menu. The below screen-shot is the result of adding an example sketch for the ESP8266WebServer library known as [FSBrowser](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/examples/FSBrowser) to the AutoConnect menu item. It can add Edit and List items with little modification to the legacy sketch code.

<div style="float:left;width:auto;height:420px;"><img style="width:auto;height:420px;" src="images/fsbmenu.png"></div>
<img style="margin-left:70px;width:auto;height:420px;" src="images/fsbmenu_expand.png">

AutoConnect allows capturing the extra pages handled with ESP8266WebServer or WebServer's legacy into the AutoConnect menu. See Section [*Advanced Usage*](advancedusage.md#capture-the-legacy-web-pages-as-items-into-the-menu) for detailed instructions on how to add the extra pages into its menu.
