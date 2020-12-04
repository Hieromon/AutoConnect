## Saved credentials in the flash

AutoConnect stores the credentials of the established WiFi connection in the flash memory of the ESP8266/ESP32 module and equips the class to access the credentials from the sketch. You can read, write, or erase the credentials using this class individually. It's the [**AutoConnectCredential**](#autoconnectcredential), which provides the way of access to the credentials stored in flash.[^1]

[^1]:An example using AutoConnectCredential is provided as [an example](https://github.com/Hieromon/AutoConnect/blob/master/examples/Credential/Credential.ino) of a library sketch to delete saved credentials.

## Credentials storage location

The location where AutoConnect saves credentials depends on the module type and the AutoConnect library version, also arduino-esp32 core version. In either case, the location is flash memory, but EEPROM and Preferences (in the nvs[^2]) are used depending on the library versions.

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

However, sketches do not need to know where to store credentials using the commonly accessible [AutoConnectCredential](#AutoConnectCredential) API.

If you are using an Arduino core for ESP32 1.0.2 earlier and need to use credentials in EEPROM for backward compatibility, turns off the **`AUTOCONNECT_USE_PREFERENCES`**[^3] macro definition in `AutoConnectCredentials.h` file. AutoConnect behaves assuming that credentials are stored in EEPROM if `AUTOCONNECT_USE_PREFERENCES` is not defined.

[^2]:The namespace for Preferences used by AutoConnect is **AC_CREDT**.
[^3]:Available only for AutoConnect v1.0.0 and later.

## AutoConnectCredential

### <i class="fa fa-code"></i> Include header

```cpp
#include <AutoConnectCredential.h>
```

### <i class="fa fa-code"></i> Constructors

```cpp
AutoConnectCredential();
```

AutoConnectCredential default constructor. The default offset value is 0. In ESP8266 or ESP32 with arduino core 1.0.2 earlier, if the offset value is 0, the credential area starts from the top of the EEPROM. If you use this area in a user sketch, AutoConnect may overwrite that data.

```cpp
AutoConnectCredential(uint16_t offset);
```

<dl class="apidl">
    <dt><strong>Parameter</strong></dt>
    <dd><span class="apidef">offset</span><span class="apidesc">Species offset from the top of the EEPROM for the credential area together. The offset value is from 0 to the flash sector size. This parameter is ignored for AutoConnect v1.0.0 or later with arduino-esp32 core 1.0.3 or later.</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> entries

```cpp
uint8_t entries(void)
```

Returns number of entries as contained credentials.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>Number of entries as contained credentials.</dd></dl>

#### <i class="fa fa-caret-right"></i> load

```cpp
int8_t load(const char* ssid, station_config_t* config)
```

Load a credential entry and store to **config**.<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">ssid</span><span class="apidesc">SSID to be loaded.</span></dd>
    <dd><span class="apidef">config</span><span class="apidesc">station_config_t</span></dd>
    <dt>**Return value**</dt>
    <dd>Save the specified SSID's credential entry to `station_config_t` pointed to by the parameter as **config**. -1 is returned if the SSID is not saved.</dd></dl>

#### <i class="fa fa-caret-right"></i> load

```cpp
bool load(int8_t entry, station_config_t* config)
```

Load a credential entry and store to **config**.<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">entry</span><span class="apidesc">Specifies the index number based 0 to be loaded.</span></dd>
    <dd><span class="apidef">config</span><span class="apidesc">station_config_t</span></dd>
    <dt>**Return value**</dt>
    <dd>Save the specified credential entry to `station_config_t` pointed to by the parameter as **config**. -1 is returned if specified number is not saved.</dd></dl>

#### <i class="fa fa-caret-right"></i> save

```cpp
bool save(const station_config_t* config)
```

Save a credential entry.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">config</span><span class="apidesc">station_config_t to be saved.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Successfully saved.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">Failed to save.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> del

```cpp
bool del(const char* ssid)
```

Delete a credential the specified SSID.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">ssid</span><span class="apidesc">SSID to be deleted.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Successfully deleted.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">Failed to delete.</span></dd></dl>

!!! example "Clear saved credentials"
    There is no particular API for batch clearing of all credential data stored by AutoConnect. It is necessary to prepare a sketch function that combines several AutoConnectCredential APIs  to erase all saved credentials.
    The following function is an implementation example, and you can use it to achieve batch clearing.
    
    ```cpp
    void deleteAllCredentials(void) {
      AutoConnectCredential credential;
      station_config_t config;
      uint8_t ent = credential.entries();

      while (ent--) {
        credential.load(0, &config);
        credential.del((const char*)&config.ssid[0]);
      }
    }
    ```

## The data structures

### <i class="fas fa-database"></i>  station_config_t

The saved credential structure is defined as `station_config_t` in the AcutoConnectCredential header file.

```cpp
typedef struct {
  uint8_t ssid[32];
  uint8_t password[64];
  uint8_t bssid[6];
  uint8_t dhcp;   /**< 0:DHCP, 1:Static IP */
  union _config {
    uint32_t  addr[5];
    struct _sta {
      uint32_t ip;
      uint32_t gateway;
      uint32_t netmask;
      uint32_t dns1;
      uint32_t dns2;
    } sta;
  } config;
} station_config_t;
```

!!! note "The byte size of `station_config_t` in program memory and stored credentials is different"
    There is a gap byte for boundary alignment between the `dhcp` member and the static IP members of the above `station_config_t`. Its gap byte will be removed with saved credentials on the flash.

### <i class="fas fa-database"></i>  The credential entry

A data structure of the credential saving area in EEPROM as the below. [^4]

[^4]:
There may be 0xff as an invalid data in the credential saving area. The 0xff area would be reused.

| byte offset  | Length | Value |
|------------- |--------|-------|
| 0            | 8        | AC_CREDT |
| 8            | 1        | Number of contained entries (uint8_t) |
| 9            | 2        | Container size, excluding size of AC_CREDT and size of the number of entries(width for uint16_t type). |
| 11           | variable | SSID terminated by 0x00. Max length is 32 bytes. |
| variable     | variable | Password plain text terminated by 0x00. Max length is 64 bytes. |
| variable     | 6        | BSSID |
| variable     | 1        | Flag for DHCP or Static IP (0:DHCP, 1:Static IP) |
| <td colspan=3>The following IP address entries are stored only for static IPs.
| variable(1)  | 4        | Station IP address (uint32_t) |
| variable(5)  | 4        | Gateway address (uint32_t) |
| variable(9)  | 4        | Netmask (uint32_t) |
| variable(13) | 4        | Primary DNS address (uint32_t) |
| variable(17) | 4        | Secondary DNS address (uint32_t) |
| variable     | variable | Contained the next entries. (Continuation SSID+Password+BSSID+DHCP flag+Static IPs(if exists)) |
| variable     | 1        | 0x00. End of container. |

!!! note "AutoConnectCredential has changed"
    It was lost AutoConnectCredential backward compatibility. Credentials saved by AutoConnect v1.0.3 (or earlier) will not work properly with AutoConnect v1.1.0. You need to erase the flash of the ESP module using the esptool before the sketch uploading.
    ```powershell
    esptool -c esp8266 (or esp32) -p [COM_PORT] erase_flash
    ```
