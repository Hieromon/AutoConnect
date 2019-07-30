## <i class="fa fa-code"></i> Constructor

### AutoConnectUpdate

```cpp
AutoConnectUpdate(const String& host, const uint16_t port, const String& uri, const int timeout, const uint8_t ledOn)
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">host</span><span class="apidesc">Update server address. Specifies IP address or FQDN.</span></dd>
    <dd><span class="apidef">port</span><span class="apidesc">Specifies HTTP port for the updating process. The default is defined as the  **AUTOCONNECT_UPDATE_PORT** macro in the `AutoConnectDefs.h` header file.</span></dd>
    <dd><span class="apidef">uri</span><span class="apidesc">Specifies a URI on the update server that has deployed available binary sketch files.</span></dd>
    <dd><span class="apidef">timeout</span><span class="apidesc">Specifies the maximum response time for the update server. The default is defined as the **AUTOCONNECT_UPDATE_TIMEOUT** macro in the `AutoConnectDefs.h` header file.</span></dt>
    <dd><span class="apidef">ledOn</span><span class="apidesc">Active signal to light the LED ticker during the update. Specifies **HIGH** or **LOW**</span></dt>
</dl>

## <i class="fa fa-code"></i> Public member functions

### <i class="fa fa-caret-right"></i> attach

```cpp
void AutoConnectUpdate::attach(AutoConnect& portal)
```
Attaches the [AutoConnectUpdate](apiupdate.md) to the AutoConnect which constitutes the bedrock of the update process. This function creates a [dialog page](otaserver.md#behavior-of-the-autoconnectupdate-class) for the update operation as an instance of [AutoConnectAux](apiaux.me) and participates in the [AutoConnect menu](menu.md).
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">portal</span><span class="apidesc">Specifies a reference to the AutoConnect instance to attach.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> disable

```cpp
void AutoConnectUpdate::disable(const bool activate)
```
Disable the Update item in [AutoConnect menu](menu.md). The [AutoConnect::disable](#disable) function only hides the Update item from the menu, and the [AutoConnectUpdate](apiupdate.md) class is still active with the parameter condition. You can use the [AutoConnectUpdate::enable](#enable) function to appear it again in the menu.
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">activate</span><span class="apidesc">If specified the **true** then the Update item will be displayed on the [AutoConnect menu](menu.md) and OTA update will be available during the WiFi status is WL_CONNECTED. For the **false**, the OTA update feature is disabled.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> enable

```cpp
void AutoConnectUpdate::enable(void)
```
Makes [AutoConnectUpdate](apiupdate.md) class available by incorporating the OTA update function into the [AutoConnect menu](menu.md). In ordinally, the [AutoConnectUpdate](apiupdate.md) class becomes available by just calling the [AutoConnectUpdate::attach](#attach) function.

### <i class="fa fa-caret-right"></i> handleUpdate

### <i class="fa fa-caret-right"></i> isEnabled

### <i class="fa fa-caret-right"></i> status

## <i class="fa fa-code"></i> Public member variables

### <i class="fa fa-caret-right"></i> host

### <i class="fa fa-caret-right"></i> port

### <i class="fa fa-caret-right"></i> uri
