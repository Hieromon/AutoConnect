## <i class="fa fa-code"></i> Constructor

### AutoConnectUpdate

```cpp
AutoConnectUpdate(const String& host, const uint16_t port, const String& uri, const int timeout, const uint8_t ledOn)
```

<dl class="apidl">
    <dt><strong>Parameters</strong></dt>
    <dd><span class="apidef">host</span><span class="apidesc">Update server address. Specifies IP address or FQDN.</span></dd>
    <dd><span class="apidef">port</span><span class="apidesc">Specifies HTTP port for the updating process. The default assumes <code>AUTOCONNECT_UPDATE_PORT</code> defined in the <a href="api.html#defined-macros"><code>AutoConnectDefs.h</code></a> header file.</span></dd>
    <dd><span class="apidef">uri</span><span class="apidesc">Specifies a URI on the update server that has deployed available binary sketch files.</span></dd>
    <dd><span class="apidef">timeout</span><span class="apidesc">Specifies the maximum response time for the update server. The default assumes <code>AUTOCONNECT_UPDATE_TIMEOUT</code> in the <a href="api.html#defined-macros"><code>AutoConnectDefs.h</code></a> header file.</span></dt>
    <dd><span class="apidef">ledOn</span><span class="apidesc">Active signal to light the LED ticker during the update. Specifies <strong>HIGH</strong> or <strong>LOW</strong></span></dt>
</dl>

The AutoConnectUpdate class inherits from the **ESP8266HTTPUpdate** (**HTTPUpdate** for ESP32) class.

## <i class="fa fa-code"></i> Public member functions

### <i class="fa fa-caret-right"></i> attach

```cpp
void AutoConnectUpdate::attach(AutoConnect& portal)
```

Attaches the [AutoConnectUpdate](apiupdate.md) to the AutoConnect which constitutes the bedrock of the update process. This function creates a [dialog page](otaserver.md#behavior-of-the-autoconnectupdate-class) for the update operation as an instance of [AutoConnectAux](apiaux.md) and participates in the [AutoConnect menu](menu.md).<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">portal</span><span class="apidesc">Specifies a reference to the AutoConnect instance to attach.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> disable

```cpp
void AutoConnectUpdate::disable(const bool activate)
```

Disable the Update item in [AutoConnect menu](menu.md). The [AutoConnect::disable](#disable) function only hides the Update item from the menu, and the [AutoConnectUpdate](apiupdate.md) class is still active with the parameter condition. You can use the [AutoConnectUpdate::enable](#enable) function to appear it again in the menu.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">activate</span><span class="apidesc">If specified the **true** then the Update item will be displayed on the [AutoConnect menu](menu.md) and OTA update will be available during the WiFi status is WL_CONNECTED. For the **false**, the OTA update feature is disabled.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> enable

```cpp
void AutoConnectUpdate::enable(void)
```

Makes [AutoConnectUpdate](apiupdate.md) class available by incorporating the OTA update function into the [AutoConnect menu](menu.md). In ordinarily, the [AutoConnectUpdate](apiupdate.md) class becomes available by just calling the [AutoConnectUpdate::attach](#attach) function.

### <i class="fa fa-caret-right"></i> handleUpdate

```cpp
void AutoConnectUpdate::handleUpdate(void)
```

Performs the update process. This function is called by [AutoConnect::handleClient](api.md#handleClient) when AutoConnectUpdate is enabled. In many cases, sketches do not need to call this function on purpose.

### <i class="fa fa-caret-right"></i> isEnabled

```cpp
bool AutoConnectUpdate::isEnabled(void)
```

Returns whether AutoConnectUpdate is enabled.

### <i class="fa fa-caret-right"></i> rebootOnUpdate

```cpp
void AutoConnectUpdate::rebootOnUpdate(bool reboot)
```

Specifies whether or not to automatically restart the module as a result of the successful completion of the update process.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">reboot</span><span class="apidesc">If specified the **true** then the ESP module will reboot after the updating successfully completed. For the **false**, The module does not reboot automatically. The uploaded new firmware remains in the updating stage of the flash on the ESP module.  
    The boot process during the next start turn of the module by reset will copy the updated firmware to the actual program area and a new sketch program will start. The default value is true.</span></dd>
</dl>

This function inherits from the ESP8266HTTPUpdate (HTTPUpdate for ESP32) class.

### <i class="fa fa-caret-right"></i> onEnd

```cpp
void AutoConnectUpdate::onEnd(HTTPUpdateEndCB fn)
```
Register the on-end exit routine that is called only once when the update is finished. For the ESP32 platform, this function is only available in ESP32 Arduino core 2.0.0 or later.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">fn</span><span class="apidesc">A function called when the update has been finished.</span></dd>
</dl>

This function inherits from the ESP8266HTTPUpdate (HTTPUpdate for ESP32) class.

An *fn* specifies the function called when the update has been finished. Its prototype declaration is defined as *HTTPUpdateEndCB*.

```cpp
using HTTPUpdateEndCB = std::function<void()>;
```

### <i class="fa fa-caret-right"></i> onError

```cpp
void AutoConnectUpdate::onError(HTTPUpdateErrorCB fn)
```

Register the exit routine that is called when some error occurred. For the ESP32 platform, this function is only available in ESP32 Arduino core 2.0.0 or later.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">fn</span><span class="apidesc">A function called when some updating error occurs.</span></dd>
</dl>

This function inherits from the ESP8266HTTPUpdate (HTTPUpdate for ESP32) class.

An *fn* specifies the function called when the some error occurred. Its prototype declaration is defined as *HTTPUpdateErrorCB*.

```cpp
using HTTPUpdateErrorCB = std::function<void(int error)>;
```

<dl class="apidl">
    <dt><strong>Parameter</strong></dt>
    <dd><span class="apidef">error</span><span class="apidesc">Error code of the Update. It is defined in the <a href="https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h">ESP8266HTTPClient</a> class, <a href="https://github.com/esp8266/Arduino/blob/ee7ac2f79d4bbf5460bf1c60c58469ab5e3022b9/libraries/ESP8266httpUpdate/src/ESP8266httpUpdate.h">ESP8266HTTPUpdate</a> class or the <a href="https://github.com/espressif/arduino-esp32/blob/master/libraries/HTTPUpdate/src/HTTPUpdate.h">HTTPUpdate</a> class of the Arduino core for each platform.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> onProgress

```cpp
void AutoConnectUpdate::onProgress(HTTPUpdateProgressCB fn)
```

Register the exit routine that is called during the update progress. For the ESP32 platform, this function is only available in ESP32 Arduino core 2.0.0 or later.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">fn</span><span class="apidesc">A function called during the updating progress.</span></dd></dl>

This function inherits from the ESP8266HTTPUpdate (HTTPUpdate for ESP32) class.

An *fn* specifies the function called during the updating progress. Its prototype declaration is defined as *HTTPUpdateProgressCB*.

!!! note "Updating Progress bar will not available"
    AutoConnectUpdate uses the onProgress exit to update the [progress bar on a web page](otaserver.md#behavior-of-the-autoconnectupdate-class) during updating. If the user sketch registers its own exit routine with the onProgress function, AutoConnectUpdate's progress bar on the web page will not be updated.

```cpp
using HTTPUpdateProgressCB = std::function<void(int amount, int size)>;
```

<dl class="apidl">
    <dt><strong>Parameters</strong></dt>
    <dd><span class="apidef">amount</span><span class="apidesc">Total amount of bytes received.</span></dd>
    <dd><span class="apidef">size</span><span class="apidesc">Block size of current send.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> onStart

```cpp
void AutoConnectUpdate::onStart(HTTPUpdateStartCB fn)
```

Register the on-start exit routine that is called only once when the update has been started. For the ESP32 platform, this function is only available in ESP32 Arduino core 2.0.0 or later.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">fn</span><span class="apidesc">A function called at the update start.</span></dd></dl>

This function inherits from the ESP8266HTTPUpdate (HTTPUpdate for ESP32) class.

An *fn* specifies the function called when the OTA starts. Its prototype declaration is defined as *HTTPUpdateStartCB*.

```cpp
using HTTPUpdateStartCB = std::function<void()>;
```

### <i class="fa fa-caret-right"></i> setLedPin

```cpp
void AutoConnectUpdate::setLedPin(int ledPin, uint8_t ledOn)
```

Sets the port and the ON signal level of the externally connected LED that should act as a ticker during the update process.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">ledPin</span><span class="apidesc">Specifies the PIN connected external LED for the ticker. The default assumes `AUTOCONNECT_TICKER_PORT` defined in the [`AutoConnectDefs.h`](api.md#defined-macros) header file and it is derived from the board-specific **LED_BUILTIN**. By default, the AutoConnectUpdate class does not use the ticker for boards without the LED_BUILTIN definition. If you connect the ticker LED externally, you need to specify the PIN using the [setLedPin](#setledpin) function.</span></dd>
    <dd><span class="apidef">ledOn</span><span class="apidesc">Specifies the the ON signal level of the LED PIN port. It is **HIGH** or **LOW**.</span></dd>
</dl>

This function inherits from the ESP8266HTTPUpdate (HTTPUpdate for ESP32) class.

### <i class="fa fa-caret-right"></i> status

```cpp
AC_UPDATESTATUS_t AutoConnectUpdate::status(void)
```

Returns the update process status transition indicator as an enumerated value of the **AC_UPDATESTATUS_t** type that indicates the process status of the AutoConnectUpdate class.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>One of the enumerated values ​​indicating the status of the Update class as follows:</dd>
    <dd><span class="apidef"></span><span class="apidesc">

- UPDATE_RESET : Update process ended, need to reset.
- UPDATE_IDLE : Update process has not started.
- UPDATE_START : Update process has been started.
- UPDATE_PROGRESS : Update process has been started.
- UPDATE_SUCCESS : Update successfully completed.
- UPDATE_NOAVAIL : No available update.
- UPDATE_FAIL : Update failed.</span></dd>
</dl>

## <i class="fa fa-code"></i> Public member variables

### <i class="fa fa-caret-right"></i> host

Update server address. Specifies IP address or FQDN.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span></dd><dd><span class="apidesc"></span></dd>
</dl>

### <i class="fa fa-caret-right"></i> port

HTTP port for the updating process.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc">The default assumes `AUTOCONNECT_UPDATE_PORT` defined in the [`AutoConnectDefs.h`](api.md#defined-macros) header file.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> uri

URI on the update server that has deployed available binary sketch files.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span></dd><dd><span class="apidesc"></span></dd>
</dl>
