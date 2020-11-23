AutoConnect automatically saves the credentials of the established WiFi connection according to the [*AutoConnectConfig::autoSave*](apiconfig.md#autosave) settings. The save destination differs depending on the type of ESP module. In the case of ESP8266, it is the EEPROM, and in the case of ESP32, it is the NVS ([Non-volatile storage](https://demo-dijiudu.readthedocs.io/en/latest/api-reference/storage/nvs_flash.html?highlight=non%20volatile%20storage)) partition implemented by the [Preferences](https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences/src) class.  
Sketches can access their stored credentials through a class that is independent of AutoConnect.

- [Access to saved credentials](#access-to-saved-credentials)
- [Autosave Credential](#autosave-credential)
- [Move the saving area of EEPROM for the credentials](#move-the-saving-area-of-eeprom-for-the-credentials)

## Access to saved credentials

AutoConnect stores the credentials of the established WiFi connection in the flash of the ESP8266/ESP32 module and equips the class to access them from the Sketch. The Sketch can read, write, or erase the credentials using this class as the [AutoConnectCredential](credit.md#autoconnectcredential) individually. Refer to section [Saved credentials access](credit.md) for details.

!!! note "Where to store credentials in ESP32 with AutoConnect v1.0.0 or later"
    Since v1.0.0, credentials are stored in nvs of ESP32. AutoConnect v1.0.0 or later accesses the credentials area using the **Preferences** class with the arduino esp-32 core. So in ESP32, the credentials are not in the EEPROM, it is in the namespace **AC_CREDT** of the nvs. See [Saved credentials access](credit.md) for details.  
    In ESP8266, it is saved in EEPROM as is conventionally done.

## Autosave Credential

By default, AutoConnect saves the credentials of the established connection to the flash. You can disable it with the [**autoSave**](apiconfig.md#autosave) parameter specified by [AutoConnectConfig](apiconfig.md).  
See the [Saved credentials access](credit.md) chapter for details on accessing stored credentials.

```cpp hl_lines="3"
AutoConnect       Portal;
AutoConnectConfig Config;
Config.autoSave = AC_SAVECREDENTIAL_NEVER;
Portal.config(Config);
Portal.begin();
```

!!! note "Credentials storage location"
    The location where AutoConnect saves credentials depends on the module type and the AutoConnect library version, also arduino-esp32 core version.
    <table>
      <tr>
        <th rowspan="2" style="vertical-align:bottom">AutoConnect</th>
        <th rowspan="2" style="vertical-align:bottom">Arduino core<br>for ESP8266</th>
        <th colspan="2" style="text-align:center;vertical-align:bottom">Arduino core for ESP32</th>
      </tr>
      <tr>
        <th style="text-align:center;vertical-align:bottom">1.0.2 earlier</td>
        <th style="text-align:center;vertical-align:bottom">1.0.3 later</td>
      </tr>
      <tr>
        <td>v0.9.12 earlier</td>
        <td rowspan="2" style="text-align:center;vertical-align:middle">EEPROM</td>
        <td>EEPROM (partition)</td>
        <td>Not supported</td>
      </tr>
      <tr>
        <td>v1.0.0 later</td>
        <td>Preferences (nvs)<p>(Can be used EEPROM with turning off AUTOCONNECT_USE_PREFERENCES macro)</p></td>
        <td>Preferences (nvs)</td>
      </tr>
    </table>

## Move the saving area of EEPROM for the credentials

By default, the credentials saving area is occupied from the beginning of EEPROM area. [ESP8266 Arduino core document](http://arduino-esp8266.readthedocs.io/en/latest/filesystem.html?highlight=eeprom#flash-layout) says that:

> The following diagram illustrates flash layout used in Arduino environment:
>
> ```powershell
> |--------------|-------|---------------|--|--|--|--|--|
> ^              ^       ^               ^     ^
> Sketch    OTA update   File system   EEPROM  WiFi config (SDK)
> ```

and

> EEPROM library uses one sector of flash located [just after the SPIFFS](http://arduino-esp8266.readthedocs.io/en/latest/libraries.html?highlight=SPIFFS#eeprom).

Also, in ESP32 arduino core 1.0.2 earlier, the placement of the EEPROM area of ESP32 is described in the [partition table](https://github.com/espressif/arduino-esp32/blob/master/tools/partitions/default.csv). So in the default state, the credential storage area used by AutoConnect conflicts with data owned by the user sketch. It will be destroyed together saved data in EEPROM by user sketch and AutoConnect each other. But you can move the storage area to avoid this.

The [**boundaryOffset**](apiconfig.md#boundaryoffset) in [AutoConnectConfig](apiconfig.md) specifies the start offset of the credentials storage area. The default value is 0.

!!! info "The boundaryOffset ignored with AutoConnect v1.0.0 later on ESP32 arduino core 1.0.3 later"
    For ESP32 arduino core 1.0.3 and later, AutoConnect will store credentials to Preferences in the nvs. Since it is defined as the namespace dedicated to AutoConnect and separated from the area used for user sketches. Therefore, the [boundaryOffset](apiconfig.md#boundaryoffset) is ignored with the combination of AutoConnect v1.0.0 or later and the arduino-esp32 1.0.3 or later.

The [*AutoConnectConfig::boundaryOffset*](apiconfig.md#boundaryoffset) setting allows AutoConnect to write its data to EEPROM while preserving custom configuration data. Similarly, when a Sketch writes its own data to EEPROM, it must preserve the data written by AutoConnect.  
The EEPROM library for ESP8266 erases the entire flash sector when it writes to any part of the sector. Therefore, when writing data to EEPROM with a sketch that handles the custom data, it is necessary to call `EEPROM.begin` using a total amount of a custom data size and the saved credentials size.  
The following code shows how to use the [AutoConnect::getEEPROMUsedSize](api.md#geteepromusedsize) function to store custom configuration settings in EEPROM without conflicting with AutoConnect's use of that storage.

```cpp hl_lines="13 21"
AutoConnect       portal;
AutoConnectConfig config;

// Defines the custom data should be stored in EEPROM.
typedef struct {
  char  data1[8];
  char  data2[8];
  char  data3[8];
} EEPROM_CONFIG_t;
EEPROM_CONFIG_t eepromConfig;
...
// Declares to reserve the EEPROM_CONFIG_t area for a Sketch using.
config.boundaryOffset = sizeof(eepromConfig);
portal.config(config);
...
strcpy(eepromComfig.data1, "data1");
strcpy(eepromComfig.data2, "data2");
strcpy(eepromComfig.data3, "data3");

// Use getEEPROMUsedSize to access the EEPROM with the appropriate region size.
EEPROM.begin(portal.getEEPROMUsedSize());
EEPROM.put<EEPROM_CONFIG_t>(0, eepromConfig);
EEPROM.commit();
EEPROM.end();
...
```
