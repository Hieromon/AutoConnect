## Change the item's label text

You can change the text of AutoConnect menu items. The easiest way is to rewrite the header file directly in the library that defines the menu label. [Advanced Usage](advancedusage.md#change-the-menu-labels) section describes the detailed how to change the label text directly.

However, this way is less preferred as it modifies the library code and further affects the entire Arduino project you compile. So, here's how to change the label text for each Arduino project without directly modifying the library code. Using this method, you can also display the label text and fixed text on AutoConnect pages in your national language.

(e.g. in Japanese)
<img src="images/localize.png">

### Preparation

AutoConnect needs a definition file as c++ header (.h) to change the label text. It is used when your Arduino project is compiled, and there is no additional memory consumption due to changing the label text. This header file describes each fixed text of AutoConnect with the `#define` preprocessor directive.

The next thing you need is [PlatformIO](https://platformio.org/). [PlatformIO](https://docs.platformio.org/en/latest/what-is-platformio.html#overview) is a very powerful environment for embedded development with multi-platform and multi-architecture build systems. And you can easily set up a PlatformIO for the Arduino development system as follows on your host machine.

- [Microsoft Visual Studio Code](https://platformio.org/install/ide?install=vscode)
- [PlatformIO IDE](https://platformio.org/platformio-ide) (included PlatformIO core)

!!! info "Install PlatformIO and VSCode"
    Please refer to [the official documentation](https://docs.platformio.org/en/latest/ide/vscode.html#installation) for PlatformIO and VSCode installation.

The rest of this section assumes that you have a PlatformIO environment with VSCode as the front end that has installed on your host machine.

## How to change the label text

### Label text replacement header file

AutoConnect label texts are pre-assigned with a fixed string so that it can be determined at compile time. Their default definitions are in the [`AutoConnectLabels.h`](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectLabels.h) file that has all the replaceable label text defined by the `#define` directive.

<table>
	<tr><th>Label placed</th><th>Pre-defined text</th><th>ID (#define macro)</th></tr>
	<tr><td rowspan="7">Menu item</td><td>Configure new AP</td><td>AUTOCONNECT_MENULABEL_CONFIGNEW</td></tr>
	<tr><td>Open SSIDs</td><td>AUTOCONNECT_MENULABEL_OPENSSIDS</td></tr>
	<tr><td>Disconnect</td><td>AUTOCONNECT_MENULABEL_DISCONNECT</td></tr>
	<tr><td>Reset...</td><td>AUTOCONNECT_MENULABEL_RESET</td></tr>
	<tr><td>HOME</td><td>AUTOCONNECT_MENULABEL_HOME</td></tr>
	<tr><td>Update</td><td>AUTOCONNECT_MENULABEL_UPDATE</td></tr>
    <tr><td>Device info</td><td>AUTOCONNECT_MENULABEL_DEVINFO</td></tr>
	<tr><td rowspan="2">Button label</td><td>RESET</td><td>AUTOCONNECT_BUTTONLABEL_RESET</td></tr>
    <tr><td>UPDATE</td><td>AUTOCONNECT_BUTTONLABEL_UPDATE</td></tr>
	<tr><td rowspan="8">Page title</td><td>Page not found</td><td>AUTOCONNECT_PAGETITLE_NOTFOUND</td></tr>
    <tr><td>AutoConnect config</td><td>AUTOCONNECT_PAGETITLE_CONFIG</td></tr>
    <tr><td>AutoConnect connecting</td><td>AUTOCONNECT_PAGETITLE_CONNECTING</td></tr>
    <tr><td>AutoConnect connection failed</td><td>AUTOCONNECT_PAGETITLE_CONNECTIONFAILED</td></tr>
    <tr><td>AutoConnect credentials</td><td>AUTOCONNECT_PAGETITLE_CREDENTIALS</td></tr>
    <tr><td>AutoConnect disconnected</td><td>AUTOCONNECT_PAGETITLE_DISCONNECTED</td></tr>
    <tr><td>AutoConnect resetting</td><td>AUTOCONNECT_PAGETITLE_RESETTING</td></tr>
    <tr><td>AutoConnect statistics</td><td>AUTOCONNECT_PAGETITLE_STATISTICS</td></tr>
    <tr><td rowspan="14">Page:[statistics] row</td><td>Established connection</td><td>AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION</td></tr>
    <tr><td>Mode</td><td>AUTOCONNECT_PAGESTATS_MODE</td></tr>
    <tr><td>IP</td><td>AUTOCONNECT_PAGESTATS_IP</td></tr>
    <tr><td>GW</td><td>AUTOCONNECT_PAGESTATS_GATEWAY</td></tr>
    <tr><td>Subnet mask</td><td>AUTOCONNECT_PAGESTATS_SUBNETMASK</td></tr>
    <tr><td>SoftAP IP</td><td>AUTOCONNECT_PAGESTATS_SOFTAPIP</td></tr>
    <tr><td>AP MAC</td><td>AUTOCONNECT_PAGESTATS_APMAC</td></tr>
    <tr><td>STA MAC</td><td>AUTOCONNECT_PAGESTATS_STAMAC</td></tr>
    <tr><td>Channel</td><td>AUTOCONNECT_PAGESTATS_CHANNEL</td></tr>
    <tr><td>dBm</td><td>AUTOCONNECT_PAGESTATS_DBM</td></tr>
    <tr><td>Chip ID</td><td>AUTOCONNECT_PAGESTATS_CHIPID</td></tr>
    <tr><td>CPU Freq.</td><td>AUTOCONNECT_PAGESTATS_CPUFREQ</td></tr>
    <tr><td>Flash size</td><td>AUTOCONNECT_PAGESTATS_FLASHSIZE</td></tr>
    <tr><td>Free memory</td><td>AUTOCONNECT_PAGESTATS_FREEMEM</td></tr>
    <tr><td rowspan="6">Page:[config] text</td><td>Total:</td><td>AUTOCONNECT_PAGECONFIG_TOTAL</td></tr>
    <tr><td>Hidden:</td><td>AUTOCONNECT_PAGECONFIG_HIDDEN</td></tr>
    <tr><td>SSID</td><td>AUTOCONNECT_PAGECONFIG_SSID</td></tr>
    <tr><td>Passphrase</td><td>AUTOCONNECT_PAGECONFIG_PASSPHRASE</td></tr>
    <tr><td>Enable DHCP</td><td>AUTOCONNECT_PAGECONFIG_ENABLEDHCP</td></tr>
    <tr><td>Apply</td><td>AUTOCONNECT_PAGECONFIG_APPLY</td></tr>
    <tr><td rowspan="4">Page:[update] text</td><td>Updating firmware</td><td>AUTOCONNECT_TEXT_UPDATINGFIRMWARE</td></tr>
    <tr><td>Select firmware:</td><td>AUTOCONNECT_TEXT_SELECTFIRMWARE</td></tr>
    <tr><td>Successfully updated, rebooting...</td><td>AUTOCONNECT_TEXT_OTASUCCESS</td></tr>
    <tr><td>Failed to update:</td><td>AUTOCONNECT_TEXT_OTAFAILURE</td></tr>
    <tr><td>Page:[connection failed]</td><td>Connection Failed</td><td>AUTOCONNECT_PAGECONNECTIONFAILED_CONNECTIONFAILED</td></tr>
    <tr><td>Text</td><td>No saved credentials.</td><td>AUTOCONNECT_TEXT_NOSAVEDCREDENTIALS</td></tr>
    <tr><td rowspan="3">Menu Text</td><td>Connecting</td><td>AUTOCONNECT_MENUTEXT_CONNECTING</td></tr>
    <tr><td>Disconnect</td><td>AUTOCONNECT_MENUTEXT_DISCONNECT</td></tr>
    <tr><td>Failed</td><td>AUTOCONNECT_MENUTEXT_FAILED</td></tr>
</table>

The definition of label text must conform to a certain coding pattern. Undefine with `#undef` the `#define` directive corresponding to the above IDs, and then redefine the ID with the replacement text. And surround it with `#ifdef` ~ `#endif`.

```cpp
#ifdef AUTOCONNECT_MENULABEL_CONFIGNEW
#undef AUTOCONNECT_MENULABEL_CONFIGNEW
#define AUTOCONNECT_MENULABEL_CONFIGNEW   "NEW_STRING_YOU_WISH"
#endif
```

You may not need to rewrite all definitions. It depends on your wishes and is sufficient that the above the include file contains only the labels you need.

### Configuration of platformio.ini

You prepare its header file and place it in the `src` folder of the project folder. You can name the file whatever you like, but for the sake of explanation, let's say `mylabels.h`.

When you store `mylabels.h` containing the new label text definition in the `src` folder, your Arduino project folder structure should look like this:

```javascript
<Project folder>
|-- <pio>
|-- <.vscode>
|-- <include>
|-- <lib>
|-- <src>
|   |-- main.cpp
|   |-- mylabels.h  <-- Depends on the project
|-- <test>
|-- .gitignore
|-- .travis.yml
|-- platformio.ini
```

Then, open [`platformio.ini`](https://docs.platformio.org/en/latest/projectconf.html) file and add new [`build_flags`](https://docs.platformio.org/en/latest/projectconf/section_env_build.html#build-flags) for including `mylabels.h` to override the label text.

```ini
build_flags = -DAC_LABELS='"${PROJECT_SRC_DIR}/mylabels.h"'
```

!!! hint "Just change the `mylabels.h`"
    Keep `-DAC_LABELS='"${PROJECT_SRC_DIR}/YOUR_FILE_NAME"'` when changing the above `build_flags` item to match your labels header file name.

After placing the `mylabels.h` file and add the `build_flags`, build the project with the replaced label text. You will see the AutoConnect screen with the new text replaced by `mylabels.h`.

!!! caution "Need clean-up before re-build with updated `mylabels.h`"
    When you have updated `mylabels.h`, you need deleting compiled library object files before build. Use `Clean` of a PlatformIO task on VSCode status bar.
    <p><img src="images/vscode_clean.png"></p>
    <p><img src="images/vscode_statusbar.png"></p>
