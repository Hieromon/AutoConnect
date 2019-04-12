## Saved credential in EEPROM

AutoConnect saves the established WiFi connection to the EEPROM area of the ESP8266/ESP32 module and equips a class for accessing the credentials. You can use the class to access the credentials individually.

Make some sketches for erasing the EEPROM area, or some erasing utility is needed. You can prepare the sketch to erase the saved credential with *AutoConnectCredential*. The *AutoConnectCrendential* class provides the access method to the saved credential in EEPROM and library source file is including it.

A class description of AutoConnectCredential is follows.

### <i class="fa fa-code"></i> Include header

```cpp
#include <AutoConnectCredential.h>
```

### <i class="fa fa-code"></i> Constructor

```cpp
AutoConnectCredential();
```

AutoConnectCredential default constructor. The default offset value is 0. If the offset value is 0, the credential storage area starts from the top of the EEPROM. AutoConnect sometimes overwrites data when using this area with user sketch.

```cpp
AutoConnectCredential(uint16_t offset);
```

Specify offset from the top of the EEPROM for the credential storage area together. The offset value is from 0 to the flash sector size.

### <i class="fa fa-code"></i> Public member functions

- uint8_t **entries()**  
   Returns number of entries as contained credentials.

- int8_t **load(const char\* _ssid_, struct station_config\* _config_)**  
   Load a credential entry specified *ssid* to *config*. Returns -1 as unsuccessfully loaded.

- bool **load(int8_t _entry_, struct station_config\* _config_)**  
   Load a credential entry to *config*. The *entry* parameter specify to index of the entry.

- bool **save(const struct station_config\* _config_)**  
   Save a credential entry stored in *config* to EEPROM. Returns the true as succeeded.

- bool **del(const char\* _ssid_)**  
   Delete a credential entry specified *ssid*. Returns the true as successfully deleted.

### <i class="fa fa-code"></i> Data structures

- station_config  
  A structure is included in the ESP8266 SDK. You can use it in the sketch like as follows.

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

- EEPROM data structure  
  A data structure of the credential saving area in EEPROM as the below. [^1]

[^1]:
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
