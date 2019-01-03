## AutoConnectButton

## <small><i class="fa fa-code"></i> Constructor</small>

```cpp
AutoConnectButton()
```
```cpp
AutoConnectButton(const char* name)
```
```cpp
AutoConnectButton(const char* name, const char* value)
```
```cpp
AutoConnectButton(const char* name, const char* value, const String& action)
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">name</span>The element name.</dd>
    <dd><span class="apidef">value</span>Value of the element.</dd>
    <dd><span class="apidef">action</span>Native code of the action script executed when the button is clicked.</dd>
</dl>

## <small><i class="fa fa-code"></i> Public member variables</small>

### name

The element name.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

### value

Value of the element.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

### action

Native code of the action script executed when the button is clicked.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

## <small><i class="fa fa-code"></i> Public member functions</small>

### typeOf

```cpp
ACElement_t typeOf(void)
```
Returns type of AutoConnectElement.
<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Button</dd>
</dl>

## AutoConnectElement

## <small><i class="fa fa-code"></i> Constructor</small>

```cpp
AutoConnectElement()
```
```cpp
AutoConnectElement(const char* name)
```
```cpp
AutoConnectElement(const char* name, const char* value)
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">name</span>The element name.</dd>
    <dd><span class="apidef">value</span>Value of the element.</dd>
</dl>

## <small><i class="fa fa-code"></i> Public member variables</small>

### name

The element name.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

### value

Value of the element.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

## <small><i class="fa fa-code"></i> Public member functions</small>

### typeOf

```cpp
ACElement_t typeOf(void)
```
Returns type of AutoConnectElement.
<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Element</dd>
</dl>
