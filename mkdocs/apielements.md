## AutoConnectButton

### <i class="fa fa-code"></i> Constructor

```cpp
AutoConnectButton(const char* name = "", const char* value = "", const String& action = String())
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">name</span>The element name.</dd>
    <dd><span class="apidef">value</span>Value of the element.</dd>
    <dd><span class="apidef">action</span>Native code of the action script executed when the button is clicked.</dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### <i class="fa fa-caret-right"></i> name

The element name.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

#### <i class="fa fa-caret-right"></i> value

Value of the element.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

#### <i class="fa fa-caret-right"></i> action

HTML native code of the action script to be executed when the button is clicked. It is mostly used with a JavaScript to activate a script.[^1]
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

[^1]:JavaScript can be inserted into a custom Web page using AutoConnectElement.

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> typeOf

```cpp
ACElement_t typeOf(void)
```
Returns type of AutoConnectElement.
<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Button</dd>
</dl>

## AutoConnectCheckbox

### <i class="fa fa-code"></i> Constructor

```cpp
  explicit AutoConnectCheckboxBasis(const char* name = "", const char* value = "", const char* label = "", const bool checked = false)
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">name</span>The element name.</dd>
    <dd><span class="apidef">value</span>Value of the element.</dd>
    <dd><span class="apidef">label</span>A label string prefixed to the checkbox.</dd>
    <dd><span class="apidef">check</span>Checked state of the checkbox.</dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### <i class="fa fa-caret-right"></i> name

The element name. 
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

#### <i class="fa fa-caret-right"></i> value

Value of the element. It becomes a value attribute of an HTML `#!html <input type="checkbox">` tag.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

#### <i class="fa fa-caret-right"></i> label

A label is an optional string. A label is always arranged on the right side of the checkbox. Specification of a label will generate an HTML `#!html <label>` tag with an `id` attribute. The checkbox and the label are connected by the id attribute.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

#### <i class="fa fa-caret-right"></i> checked

It indicates the checked status of the checkbox. The value of the checked checkbox element is packed in the query string and sent by submit.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">Boolean</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> typeOf

```cpp
ACElement_t typeOf(void)
```
Returns type of AutoConnectElement.
<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Checkbox</dd>
</dl>

## AutoConnectElement

### <i class="fa fa-code"></i> Constructor

```cpp
AutoConnectElement(const char* name = "", const char* value = "")
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">name</span>The element name.</dd>
    <dd><span class="apidef">value</span>Value of the element.</dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### <i class="fa fa-caret-right"></i> name

The element name.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

#### <i class="fa fa-caret-right"></i> value

Value of the element. It is output as HTML as it is as a source for generating HTML code.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> typeOf

```cpp
ACElement_t typeOf(void)
```
Returns type of AutoConnectElement.
<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Element</dd>
</dl>

## AutoConnectInput

### <i class="fa fa-code"></i> Constructor

```cpp
AutoConnectInput(const char* name = "", const char* value = "", const char* label = "", const char* pattern = "", const char* placeholder = "")
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">name</span>The element name.</dd>
    <dd><span class="apidef">value</span>Value of the element.</dd>
    <dd><span class="apidef">label</span>Label string.</dd>
    <dd><span class="apidef">pattern</span>Regular expression string for checking data format.</dd>
    <dd><span class="apidef">placeholder</span>A placeholder string.</dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### <i class="fa fa-caret-right"></i> name

The element name.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

#### <i class="fa fa-caret-right"></i> value

Value of the element. It becomes a value attribute of an HTML `#!html <input type="text">` tag. An entered text in the custom Web page will be sent with a query string of the form. The value set before accessing the page is displayed as the initial value.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

#### <i class="fa fa-caret-right"></i> label

A label is an optional string. A label is always arranged on the left side of the input box. Specification of a label will generate an HTML `#!html <label>` tag with an id attribute. The input box and the label are connected by the id attribute.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

#### <i class="fa fa-caret-right"></i> pattern

A pattern specifies a regular expression that the input-box's value is checked against on form submission.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

#### <i class="fa fa-caret-right"></i> placeholder

A placeholder is an option string. Specification of a placeholder will generate a `placeholder` attribute for the input tag.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> typeOf

```cpp
ACElement_t typeOf(void)
```
Returns type of AutoConnectElement.
<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Input</dd>
</dl>

## AutoConnectRadio

### <i class="fa fa-code"></i> Constructor

```cpp
AutoConnectRadio(const char* name = "", std::vector<String> const& values = {}, const char* label = "", const ACArrange_t order = AC_Vertical, const uint8_t checked = 0)
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">name</span>The element name.</dd>
    <dd><span class="apidef">values</span>An array of values of the radio buttons. Specifies an [std::vector](https://en.cppreference.com/w/cpp/container/vector) object.</dd>
    <dd><span class="apidef">label</span>Label string.</dd>
    <dd><span class="apidef">order</span>The direction to arrange the radio buttons.</dd>
    <dd><span class="apidef">checked</span>An index to be checked in the radio buttons.</dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### <i class="fa fa-caret-right"></i> name

The element name.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

#### <i class="fa fa-caret-right"></i> values

An array of String type for the radio button options. It is an initialization list can be used. The `#!html <input type="radio">` tags will be generated from each entry in the values.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">std::vector&lt;String&gt;</span></dd>
</dl>

#### <i class="fa fa-caret-right"></i> label

A label is an optional string. A label will be arranged in the left or top of the radio buttons according to the [order](#order).
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">String</span></dd>
</dl>

#### <i class="fa fa-caret-right"></i> order

Specifies the direction to arrange the radio buttons. A label will place in the left or the top according to the **_order_**. It is a value of **ACArrange_t** type and accepts one of the following:

- **`AC_Horizontal`** : Horizontal arrangement.
- **`AC_Vertical`** : Vertical arrangement.  
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">ACArrange_t</span></dd>
</dl>

#### <i class="fa fa-caret-right"></i> checked

Specifies the index number (1-based) of the **values** to be checked. If this parameter is not specified neither item is checked.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef" style="width:230px;">uint8_t</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> typeOf

```cpp
ACElement_t typeOf(void)
```
Returns type of AutoConnectElement.
<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Radio</dd>
</dl>

#### <i class="fa fa-caret-right"></i> add

```cpp
void add(const String& value)
```
Adds an option for the radio button.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">value</span>An option string to add to the radio button.</dd>
</dl>

#### <i class="fa fa-caret-right"></i> check

```cpp
void check(const String& value)
```
Indicates the check of the specified option for the radio buttons. You can use the **check** function for checking dynamically with arbitrary of the radio button.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">value</span>An option string to be checked.</dd>
</dl>

#### <i class="fa fa-caret-right"></i> empty

```cpp
void empty(const size_t reserve = 0)
```
Clear the array of option strings that AutoConnectRadio has in values. When a **_reserve_** parameter is specified, a vector container of that size is reserved.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">reserve</span>Reserved size of a container for option strings.</dd>
</dl>

#### <i class="fa fa-caret-right"></i> operator &#91;&nbsp;&#93;

```cpp
const String& operator[] (const std::size_t n)
```
Returns the option string of the index specified by **_n_**.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">n</span>Index of values array to return.</dd>
</dl>

#### <i class="fa fa-caret-right"></i> value

```cpp
  const String& value(void) const
```
Returns current checked option of the radio buttons.
<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>A String of an option current checked. If there is no checked option, a null string returned.</dd>
</dl>

## AutoConnectSelect

## AutoConnectSubmit

## AutoConnectText
