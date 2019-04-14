## Saved credential in EEPROM

AutoConnect stores the established WiFi connection in the EEPROM of the ESP8266/ESP32 module and equips the class to access it from the sketch. You can read, write or erase the credentials using this class individually. It's [**AutoConnectCredential**](#autoconnectcredential) class which provides the access method to the saved credentials in EEPROM.[^1]

[^1]:An example using AutoConnectCredential is provided as [an example](https://github.com/Hieromon/AutoConnect/blob/master/examples/Credential/Credential.ino) of a library sketch to delete saved credentials.

## AutoConnectCredential

### <i class="fa fa-code"></i> Include header

```cpp
#include <AutoConnectCredential.h>
```

### <i class="fa fa-code"></i> Constructors

```cpp
AutoConnectCredential();
```

AutoConnectCredential default constructor. The default offset value is 0. If the offset value is 0, the credential area starts from the top of the EEPROM. AutoConnect sometimes overwrites data when using this area with user sketch.

```cpp
AutoConnectCredential(uint16_t offset);
```
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">offset</span><span class="apidesc">Species offset from the top of the EEPROM for the credential area together. The offset value is from 0 to the flash sector size.</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> entries

```cpp
uint8_t entries(void)
```

Returns number of entries as contained credentials.
<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>Number of entries as contained credentials.</dd>
</dl>

#### <i class="fa fa-caret-right"></i> load

```cpp
int8_t load(const char* ssid, struct station_config* config)
```

Load a credential entry and store to **config**.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">ssid</span><span class="apidesc">SSID to be loaded.</span></dd>
    <dd><span class="apidef">config</span><span class="apidesc">station_config</span></dd>
    <dt>**Return value**</dt>
    <dd>Save the specified SSID's credential entry to station_config pointed to by the parameter as **config**. -1 is returned if the SSID is not saved. </dd>
</dl>

#### <i class="fa fa-caret-right"></i> load

```cpp
bool load(int8_t entry, struct station_config* config)
```

Load a credential entry and store to **config**.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">entry</span><span class="apidesc">Specifies the index number based 0 to be loaded.</span></dd>
    <dd><span class="apidef">config</span><span class="apidesc">station_config</span></dd>
    <dt>**Return value**</dt>
    <dd>Save the specified credential entry to station_config pointed to by the parameter as **config**. -1 is returned if specified number is not saved. </dd>
</dl>

#### <i class="fa fa-caret-right"></i> save

```cpp
bool save(const struct station_config* config)
```

Save a credentail entry.
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">config</span><span class="apidesc">station_config to be saved.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Successfully saved.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">Failed to save.</span></dd>
</dl>

#### <i class="fa fa-caret-right"></i> del

```cpp
bool del(const char* ssid)
```

Delete a credential the specified SSID.
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">ssid</span><span class="apidesc">SSID to be deleted.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Successfully deleted.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">Failed to delete.</span></dd>
</dl>

## The data structures

### <i class="fa fa-code"></i>  station_config

A structure is included in the ESP8266 SDK. You can use it in the sketch like as follows:

```cpp
extern "C" {
#include <user_interface.h>
}
```

```cpp
struct station_config {
  uint8 ssid[32];
  uint8 password[64];
  uint8 bssid_set;
  uint8 bssid[6];
};
```

### <i class="fa fa-code"></i>  The credential entry

A data structure of the credential saving area in EEPROM as the below. [^2]

[^2]:
There may be 0xff as an invalid data in the credential saving area. The 0xff area would be reused.

| Byte offset | Length   | Value                                                               |
|-------------|----------|---------------------------------------------------------------------|
| 0           | 8        | AC_CREDT                                                            |
| 8           | 1        | Number of contained entries (uint8_t)                               |
| 9           | 2        | Container size, excluding size of AC_CREDT and size of the number of entries(width for uint16_t type). |
| 11          | variable | SSID terminated by 0x00. Max length is 32 bytes.                    |
| variable    | variable | Password plain text terminated by 0x00. Max length is 64 bytes.     |
| variable    | 6        | BSSID                                                               |
| variable    |          | Contained the next entries. (Continuation SSID+Password+BSSID)      |
| variable    | 1        | 0x00. End of container.                                             |
