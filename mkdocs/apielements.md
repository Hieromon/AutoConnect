## AutoConnectButton

### <i class="fa fa-code"></i> Constructor

```cpp
AutoConnectButton(const char* name = "", const char* value = "", const String& action = String(), const ACPosterior_t post = AC_Tag_None)
```

<dl class="apidl">
    <dt><strong>Parameters</strong></dt>
    <dd><span class="apidef">name</span><span class="apidesc">The element name.</span></dd>
    <dd><span class="apidef">value</span><span class="apidesc">Value of the element.</span></dd>
    <dd><span class="apidef">action</span><span class="apidesc">Native code of the action script executed when the button is clicked.</span></dd>
    <dd><span class="apidef">post</span><span class="apidesc">Specifies the tag to be output afterward the element.</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### <i class="fa fa-caret-right"></i> action

HTML native code of the action script to be executed when the button is clicked. It is mostly used with a JavaScript to activate a script.[^1]<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

[^1]:JavaScript can be inserted into a custom Web page using AutoConnectElement.

#### <i class="fa fa-caret-right"></i> enable

Enable HTML tag generation for the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">AutoConnect will generate the element into HTML only if the enable attribute is true.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> global

The global attribute copies input values ​​between elements of the same name on different custom Web pages.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">An entered value will be copied to elements of the same name in other AutoConnectAuxes during page transition.<br>However, it will be copied only when the destination element has the true for a global attribute.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> name

The element name.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> post

Specifies a tag to add behind the HTML code generated from the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">ACPosterior_t</span><span class="apidesc">
        
- **`AC_Tag_None`** : No generate additional tags.
- **`AC_Tag_BR`** : Add a `<br>` tag to the end of the element.
- **`AC_Tag_P`** : Include the element in the `<p> ~ </p>` tag.
</span></dd></dl>

#### <i class="fa fa-caret-right"></i> value

Value of the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> typeOf

```cpp
ACElement_t typeOf(void)
```

Returns type of AutoConnectElement.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Button</dd></dl>

## AutoConnectCheckbox

### <i class="fa fa-code"></i> Constructor

```cpp
AutoConnectCheckbox(const char* name = "", const char* value = "", const char* label = "", const bool checked = false, const ACPosition_t labelPosition = AC_Behind, const ACPosterior_t post = AC_Tag_BR)
```

<dl class="apidl">
    <dt><strong>Parameters</strong></dt>
    <dd><span class="apidef">name</span><span class="apidesc">The element name.</span></dd>
    <dd><span class="apidef">value</span><span class="apidesc">Value of the element.</span></dd>
    <dd><span class="apidef">label</span><span class="apidesc">A label string prefixed to the checkbox.</span></dd>
    <dd><span class="apidef">check</span><span class="apidesc">Checked state of the checkbox.</span></dd>
    <dd><span class="apidef">labelPosition</span><span class="apidesc">Specifies the position of the label to generate.</span></dd>
    <dd><span class="apidef">post</span><span class="apidesc">Specifies the tag to be output afterward the element.</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### <i class="fa fa-caret-right"></i> checked

It indicates the checked status of the checkbox. The value of the checked checkbox element is packed in the query string and sent by submit.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc"></span></dl>

#### <i class="fa fa-caret-right"></i> enable

Enable HTML tag generation for the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">AutoConnect will generate the element into HTML only if the enable attribute is true.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> global

The global attribute copies input values ​​between elements of the same name on different custom Web pages.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">An entered value will be copied to elements of the same name in other AutoConnectAuxes during page transition.<br>However, it will be copied only when the destination element has the true for a global attribute.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> label

A label is an optional string. A label is always arranged on the right side of the checkbox. Specification of a label will generate an HTML `#!html <label>` tag with an `id` attribute. The checkbox and the label are connected by the id attribute.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> labelPosition

Specifies the position of the label to generate with ACPostion_t enumeration value.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">ACPosition_t</span><span class="apidesc">
        
- **`AC_Infront`** : Place a label in front of the check box.
- **`AC_Behind`** : Place a label behind the check box.
</span></dd></dl>

#### <i class="fa fa-caret-right"></i> name

The element name.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> post

Specifies a tag to add behind the HTML code generated from the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">ACPosterior_t</span><span class="apidesc">
        
- **`AC_Tag_None`** : No generate additional tags.
- **`AC_Tag_BR`** : Add a `<br>` tag to the end of the element.
- **`AC_Tag_P`** : Include the element in the `<p> ~ </p>` tag.
</span></dd></dl>

#### <i class="fa fa-caret-right"></i> value

Value of the element. It becomes a value attribute of an HTML `#!html <input type="checkbox">` tag.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> typeOf

```cpp
ACElement_t typeOf(void)
```

Returns type of AutoConnectElement.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Checkbox</dd></dl>

## AutoConnectElement

### <i class="fa fa-code"></i> Constructor

```cpp
AutoConnectElement(const char* name = "", const char* value = "", const ACPosterior_t post = AC_Tag_None)
```

<dl class="apidl">
    <dt><strong>Parameters</strong></dt>
    <dd><span class="apidef">name</span><span class="apidesc">The element name.</span></dd>
    <dd><span class="apidef">value</span><span class="apidesc">Value of the element.</span></dd>
    <dd><span class="apidef">post</span><span class="apidesc">Specifies the tag to be output afterward the element.</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### <i class="fa fa-caret-right"></i> enable

Enable HTML tag generation for the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">AutoConnect will generate the element into HTML only if the enable attribute is true.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> global

The global attribute copies input values ​​between elements of the same name on different custom Web pages.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">An entered value will be copied to elements of the same name in other AutoConnectAuxes during page transition.<br>However, it will be copied only when the destination element has the true for a global attribute.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> name

The element name.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> post

Specifies a tag to add behind the HTML code generated from the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">ACPosterior_t</span><span class="apidesc">
        
- **`AC_Tag_None`** : No generate additional tags.
- **`AC_Tag_BR`** : Add a `<br>` tag to the end of the element.
- **`AC_Tag_P`** : Include the element in the `<p> ~ </p>` tag.
</span></dd></dl>

#### <i class="fa fa-caret-right"></i> value

Value of the element. It is output as HTML as it is as a source for generating HTML code.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> typeOf

```cpp
ACElement_t typeOf(void)
```

Returns type of AutoConnectElement.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Element</dd></dl>

#### <i class="fa fa-caret-right"></i> as<T\>

```cpp
AutoConnectElement& as<T>(void)
```

Casts the reference to the AutoConnectElement the specified type.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">T</span><span class="apidesc">The element type. AutoConnectElements type such as [AutoConnectButton](apielements.md#autoconnectbutton), [AutoConnectCheckbox](apielements.md#autoconnectcheckbox), [AutoConnectFile](apielements.md#autoconnectfile), [AutoConnectInput](apielements.md#autoconnectinput), [AutoConnectRadio](apielements.md#autoconnectradio), [AutoConnectSelect](apielements.md#autoconnectselect), [AutoConnectStyle](apielements.md#autoconnectstyle), [AutoConnectSubmit](apielements.md#autoconnectsubmit), [AutoConnectText](apielements.md#autoconnecttext).</span></dd>
    <dt>**Return value**</dt>
    <dd>A reference to the AutoConnectElement with actual type.</dd></dl>

## AutoConnectFile

### <i class="fa fa-code"></i> Constructor

```cpp
AutoConnectFile(const char* name = "", const char* value = "", const char* label = "", const ACFile_t store = AC_File_FS, const ACPosterior_t post = AC_Tag_BR)
```

<dl class="apidl">
    <dt><strong>Parameters</strong></dt>
    <dd><span class="apidef">name</span><span class="apidesc">The element name.</span></dd>
    <dd><span class="apidef">value</span><span class="apidesc">File name to be upload.</span></dd>
    <dd><span class="apidef">label</span><span class="apidesc">Label string.</span></dd>
    <dd><span class="apidef">store</span><span class="apidesc">The **ACFile_t** enumerator that represents the media to save the uploaded file.</span></dd>
    <dd><span class="apidef">post</span><span class="apidesc">Specifies the tag to be output afterward the element.</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### <i class="fa fa-caret-right"></i> enable

Enable HTML tag generation for the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">AutoConnect will generate the element into HTML only if the enable attribute is true.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> global

The global attribute copies input values ​​between elements of the same name on different custom Web pages.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">An entered value will be copied to elements of the same name in other AutoConnectAuxes during page transition.<br>However, it will be copied only when the destination element has the true for a global attribute.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> label

A label is an optional string. A label is always arranged on the left side of the file input box. Specification of a label will generate an HTML `#!html <label>` tag with an id attribute. The file input box and the label are connected by the id attribute.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> mimeType

The mime type of the upload file which included as Media type in the http post request. Set by the client (usually the browser) that requested the upload. It is determined by the file type as `application/octet-stream`, `text` etc. which is described in [IANA Media Type](https://www.iana.org/assignments/media-types/media-types.xhtml).<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> name

The element name.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> post

Specifies a tag to add behind the HTML code generated from the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">ACPosterior_t</span><span class="apidesc">
        
- **`AC_Tag_None`** : No generate additional tags.
- **`AC_Tag_BR`** : Add a `<br>` tag to the end of the element.
- **`AC_Tag_P`** : Include the element in the `<p> ~ </p>` tag.
</span></dd></dl>

#### <i class="fa fa-caret-right"></i> size

Size of the uploading file.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">size_t</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> store

Specifies the save destination of the uploaded file. You can use the built-in uploader to save uploaded file to the flash of the ESP8266/ESP32 module or external SD media without writing a dedicated sketch code. It also supports saving to any destination using a custom uploader that inherits from the AutoConnectUploadHandler class.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">ACFile_t</span><span class="apidesc">
        
- **`AC_File_FS`** : Save the uploaded file to SPIFFS in the flash.
- **`AC_File_SD`** : Save the uploaded file to SD.
- **`AC_File_Extern`** : Save the file using your own upload handler.
    </span></dd></dl>

#### <i class="fa fa-caret-right"></i> value

File name to be upload. The value contains the value entered by the client browser to the `#!html <input type="file">` tag and is read-only.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> typeOf

```cpp
ACElement_t typeOf(void)
```

Returns type of AutoConnectFile.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_File</dd>
</dl>

## AutoConnectInput

### <i class="fa fa-code"></i> Constructor

```cpp
AutoConnectInput(const char* name = "", const char* value = "", const char* label = "", const char* pattern = "", const char* placeholder = "", const ACPosterior_t post = AC_Tag_BR, const ACInput_t apply = AC_Input_Text)
```

<dl class="apidl">
    <dt><strong>Parameters</strong></dt>
    <dd><span class="apidef">name</span><span class="apidesc">The element name.</span></dd>
    <dd><span class="apidef">value</span><span class="apidesc">Value of the element.</span></dd>
    <dd><span class="apidef">label</span><span class="apidesc">Label string.</span></dd>
    <dd><span class="apidef">pattern</span><span class="apidesc">Regular expression string for checking data format.</span></dd>
    <dd><span class="apidef">placeholder</span><span class="apidesc">A placeholder string.</span></dd>
    <dd><span class="apidef">post</span><span class="apidesc">Specifies the tag to be output afterward the element.</span></dd>
    <dd><span class="apidef">apply</span><span class="apidesc">Specifies the type of input that the text box accepts.</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### <i class="fa fa-caret-right"></i> enable

Enable HTML tag generation for the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">AutoConnect will generate the element into HTML only if the enable attribute is true.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> global

The global attribute copies input values ​​between elements of the same name on different custom Web pages.
<dl class="apidl">    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">An entered value will be copied to elements of the same name in other AutoConnectAuxes during page transition.<br>However, it will be copied only when the destination element has the true for a global attribute.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> label

A label is an optional string. A label is always arranged on the left side of the input box. Specification of a label will generate an HTML `#!html <label>` tag with an id attribute. The input box and the label are connected by the id attribute.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> name

The element name.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> pattern

A pattern specifies a regular expression that the input-box's value is checked against on form submission.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> placeholder

A placeholder is an option string. Specification of a placeholder will generate a `placeholder` attribute for the input tag.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> post

Specifies a tag to add behind the HTML code generated from the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">ACPosterior_t</span><span class="apidesc">
        
- **`AC_Tag_None`** : No generate additional tags.
- **`AC_Tag_BR`** : Add a `<br>` tag to the end of the element.
- **`AC_Tag_P`** : Include the element in the `<p> ~ </p>` tag.
</span></dd></dl>

#### <i class="fa fa-caret-right"></i> value

Value of the element. It becomes a value attribute of an HTML `#!html <input type="text">` tag. An entered text in the custom Web page will be sent with a query string of the form. The value set before accessing the page is displayed as the initial value.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> apply

Specifies the type of input that the text box accepts. AutoConnectInput will generate either a `#!html <input type="text">`, `#!html <input type="password">`, or `#!html <input type="number">` tag based on the `apply` specifying as input type. The input type can be specified the following values in the *ACInput_t* enumeration type. [^1]<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">ACInput_t</span><span class="apidesc">
        
- **`AC_Input_Text`** : `input type="text"`
- **`AC_Input_Password`** : `input type="password"`
- **`AC_Input_Number`** : `input type="number"`
</span></dd></dl>

[^1]: **ACInput_t** does not mind what kind of display effects on the browser. For example, **input type="number"** has a spin button in Chrome, but has no display effects in iOS Safari. You will see the numeric keypad at inputting the number field with giving the pattern as `\d*`.

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> isValid

```cpp
bool isValid(void)
```

Evaluate the pattern as a regexp and return whether value matches. Always return true if the pattern is undefined.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">The value matches a pattern.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">The value does not match a pattern.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> typeOf

```cpp
ACElement_t typeOf(void)
```

Returns type of AutoConnectElement.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Input</dd></dl>

## AutoConnectRadio

### <i class="fa fa-code"></i> Constructor

```cpp
AutoConnectRadio(const char* name = "", std::vector<String> const& values = {}, const char* label = "", const ACArrange_t order = AC_Vertical, const uint8_t checked = 0, const ACPosterior_t post = AC_Tag_BR)
```

<dl class="apidl">
    <dt><strong>Parameters</strong></dt>
    <dd><span class="apidef">name</span><span class="apidesc">The element name.</span></dd>
    <dd><span class="apidef">values</span><span class="apidesc">An array of values of the radio buttons. Specifies a <a href="https://en.cppreference.com/w/cpp/container/vector">std::vector</a> object.</span></dd>
    <dd><span class="apidef">label</span><span class="apidesc">Label string.</span></dd>
    <dd><span class="apidef">order</span><span class="apidesc">The direction to arrange the radio buttons.</span></dd>
    <dd><span class="apidef">checked</span><span class="apidesc">An index to be checked in the radio buttons.</span></dd>
    <dd><span class="apidef">post</span><span class="apidesc">Specifies the tag to be output afterward the element.</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### <i class="fa fa-caret-right"></i> checked

Specifies the index number (1-based) of the **values** to be checked. If this parameter is not specified neither item is checked.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">uint8_t</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> enable

Enable HTML tag generation for the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">AutoConnect will generate the element into HTML only if the enable attribute is true.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> global

The global attribute copies input values ​​between elements of the same name on different custom Web pages.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">An entered value will be copied to elements of the same name in other AutoConnectAuxes during page transition.<br>However, it will be copied only when the destination element has the true for a global attribute.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> label

A label is an optional string. A label will be arranged in the left or top of the radio buttons according to the [order](#order).<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> name

The element name.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> order

Specifies the direction to arrange the radio buttons. A label will place in the left or the top according to the **_order_**. It is a value of **ACArrange_t** type and accepts one of the following:<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">ACArrange_t</span><span class="apidesc">
        
- **`AC_Horizontal`** : Horizontal arrangement.
- **`AC_Vertical`** : Vertical arrangement.
    </span></dd></dl>

#### <i class="fa fa-caret-right"></i> post

Specifies a tag to add behind the HTML code generated from the element.
<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">ACPosterior_t</span><span class="apidesc">
        
- **`AC_Tag_None`** : No generate additional tags.
- **`AC_Tag_BR`** : Add a `<br>` tag to the end of the element.
- **`AC_Tag_P`** : Include the element in the `<p> ~ </p>` tag.
</span></dd></dl>

#### <i class="fa fa-caret-right"></i> values

An array of String type for the radio button options. It is an initialization list can be used. The `#!html <input type="radio">` tags will be generated from each entry in the values.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">std::vector&lt;String&gt;</span><span class="apidesc"></span></dd></dl>

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> add

```cpp
void add(const String& value)
```

Adds an option for the radio button.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">value</span><span class="apidesc">An option string to add to the radio button.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> check

```cpp
void check(const String& value)
```

Indicates the check of the specified option for the radio buttons. You can use the **check** function for checking dynamically with arbitrary of the radio button.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">value</span><span class="apidesc">An option string to be checked.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> empty

```cpp
void empty(const size_t reserve = 0)
```

Clear the array of option strings that AutoConnectRadio has in the values. When the **_reserve_** parameter is specified, a vector container of that size is reserved.

The empty function resets the checked value to zero. When the empty function is executed, any button will be turned off.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">reserve</span><span class="apidesc">Reserved size of a container for the radio button option strings.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> operator &#91;&nbsp;&#93;

```cpp
const String& operator[] (const std::size_t n)
```

Returns a value string of the index specified by **_n_**.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">n</span><span class="apidesc">Index of values array to return. Its base number is 0.</span></dd>
    <dt>**Return value**</dt>
    <dd>A reference of a value string indexed by the specified the **n**.</dd></dl>

#### <i class="fa fa-caret-right"></i> size

```cpp
size_t size(void)
```

Returns number of options which contained.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>Number of options which contained.</dd></dl>

#### <i class="fa fa-caret-right"></i> typeOf

```cpp
ACElement_t typeOf(void)
```

Returns type of AutoConnectElement.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Radio</dd></dl>

#### <i class="fa fa-caret-right"></i> value

```cpp
  const String& value(void) const
```

Returns current checked option of the radio buttons.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>A String of an option current checked. If there is no checked option, a null string returned.</dd></dl>

## AutoConnectSelect

### <i class="fa fa-code"></i> Constructor

```cpp
AutoConnectSelect(const char* name = "", std::vector<String> const& options = {}, const char* label = "", const uint8_t selected = 0, const ACPosterior_t post = AC_Tag_BR)
```

<dl class="apidl">
    <dt><strong>Parameters</strong></dt>
    <dd><span class="apidef">name</span><span class="apidesc">The element name.</span></dd>
    <dd><span class="apidef">options</span><span class="apidesc">An array of options of the select element. Specifies a <a href="https://en.cppreference.com/w/cpp/container/vector">std::vector</a> object.</span></dd>
    <dd><span class="apidef">label</span><span class="apidesc">Label string.</span></dd>
    <dd><span class="apidef">selected</span><span class="apidesc">An option should be pre-selected when the page loads.</span></dd>
    <dd><span class="apidef">post</span><span class="apidesc">Specifies the tag to be output afterward the element.</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### <i class="fa fa-caret-right"></i> enable

Enable HTML tag generation for the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">AutoConnect will generate the element into HTML only if the enable attribute is true.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> global

The global attribute copies input values ​​between elements of the same name on different custom Web pages.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">An entered value will be copied to elements of the same name in other AutoConnectAuxes during page transition.<br>However, it will be copied only when the destination element has the true for a global attribute.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> name

The element name.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> label

A label is an optional string. A label will be arranged in the top of the selection list.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> options

An array of String type for the selection options. It is an initialization list can be used. The `#!html <option value>` tags will be generated from each entry in the options.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">std::vector&lt;String&gt;</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> post

Specifies a tag to add behind the HTML code generated from the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">ACPosterior_t</span><span class="apidesc">
        
- **`AC_Tag_None`** : No generate additional tags.
- **`AC_Tag_BR`** : Add a `<br>` tag to the end of the element.
- **`AC_Tag_P`** : Include the element in the `<p> ~ </p>` tag.
</span></dd></dl>

#### <i class="fa fa-caret-right"></i> selected

A `selected` is an optional value. Specifies 1-based index value of an options array that an option should be pre-selected when the page loads.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">uint8_t</span><span class="apidesc"></span></dd></dl>

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> add

```cpp
void add(const String& option)
```

Adds a selectable option string for the selection list.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">option</span><span class="apidesc">A string of selectable item to be contained in the select element.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> empty

```cpp
void empty(const size_t reserve = 0)
```

Clear the array of options list that AutoConnectSelect has in the options. When the **_reserve_** parameter is specified, a vector container of that size is reserved.

The empty function resets the selected value to zero. When the empty function is executed, there are no selected options and the first item is placed at the beginning.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">reserve</span><span class="apidesc">Reserved size of a container for the options.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> operator &#91;&nbsp;&#93;

```cpp
const String& operator[] (const std::size_t n)
```

Returns an option string of the index specified by **_n_**.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">n</span><span class="apidesc">Index of options array to return. Its base number is 0.</span></dd>
    <dt>**Return value**</dt>
    <dd>A reference of a option string indexed by the specified the **n**.</dd></dl>

#### <i class="fa fa-caret-right"></i> select

```cpp
void  select(const String& value);
```

Selects an option with the value.<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">value</span><span class="apidesc">String value that option should be selected in an option array.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> size

```cpp
size_t size(void)
```

Returns number of options which contained.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>Number of options which contained.</dd></dl>

#### <i class="fa fa-caret-right"></i> typeOf

```cpp
ACElement_t typeOf(void)
```

Returns type of AutoConnectElement.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Select</dd></dl>

#### <i class="fa fa-caret-right"></i> value

```cpp
const String& value(void) const;
```

Returns current selected option of the select list.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>A String of an option current selected. If there is no select option, a null string returned.</dd></dl>

## AutoConnectStyle

### <i class="fa fa-code"></i> Constructor

```cpp
AutoConnectStyle(const char* name = "", const char* value = "")
```

<dl class="apidl">
    <dt><strong>Parameters</strong></dt>
    <dd><span class="apidef">name</span><span class="apidesc">The element name.</span></dd>
    <dd><span class="apidef">value</span><span class="apidesc">Raw CSS code to insert into a style block in a custom web page to generate.</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### <i class="fa fa-caret-right"></i> enable

Enable HTML tag generation for the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">AutoConnect will generate the element into HTML only if the enable attribute is true.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> name

The element name.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> value

Raw CSS code to insert into a style block in a custom web page to generate.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> typeOf

```cpp
ACElement_t typeOf(void)
```

Returns type of AutoConnectElement.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Style</dd></dl>

## AutoConnectSubmit

### <i class="fa fa-code"></i> Constructor

```cpp
AutoConnectSubmit(const char* name = "", const char* value ="", char* uri = "", const ACPosterior_t post = AC_Tag_None)
```

<dl class="apidl">
    <dt><strong>Parameters</strong></dt>
    <dd><span class="apidef">name</span><span class="apidesc">The element name.</span></dd>
    <dd><span class="apidef">value</span><span class="apidesc">The name of the submit button as an HTML `#!html <input type="button">` tag, it will also be the label of the button.</span></dd>
    <dd><span class="apidef">uri</span><span class="apidesc">Destination URI.</span></dd>
    <dd><span class="apidef">post</span><span class="apidesc">Specifies the tag to be output afterward the element.</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### <i class="fa fa-caret-right"></i> enable

Enable HTML tag generation for the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">AutoConnect will generate the element into HTML only if the enable attribute is true.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> global

The global attribute copies input values ​​between elements of the same name on different custom Web pages.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">An entered value will be copied to elements of the same name in other AutoConnectAuxes during page transition.<br>However, it will be copied only when the destination element has the true for a global attribute.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> name

The element name.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> post

Specifies a tag to add behind the HTML code generated from the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">ACPosterior_t</span><span class="apidesc">
        
- **`AC_Tag_None`** : No generate additional tags.
- **`AC_Tag_BR`** : Add a `<br>` tag to the end of the element.
- **`AC_Tag_P`** : Include the element in the `<p> ~ </p>` tag.
</span></dd></dl>

#### <i class="fa fa-caret-right"></i> uri

Destination URI.<dl class="apidl">    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> value

The name of the submit button. It will also be the label of the button.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> typeOf

```cpp
ACElement_t typeOf(void)
```

Returns type of AutoConnectElement.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Submit</dd></dl>

## AutoConnectText

### <i class="fa fa-code"></i> Constructor

```cpp
AutoConnectText(const char* name = "", const char* value = "", const char* style = "", const char* format = "", const ACPosterior_t post = AC_Tag_None)
```

<dl class="apidl">
    <dt><strong>Parameters</strong></dt>
    <dd><span class="apidef">name</span><span class="apidesc">The element name.</span></dd>
    <dd><span class="apidef">value</span><span class="apidesc">String of content for the text element.</span></dd>
    <dd><span class="apidef">style</span><span class="apidesc">A style code with CSS format that qualifiers the text.</span></dd>
    <dd><span class="apidef">format</span><span class="apidesc">A pointer to a null-terminated multibyte string specifying how to interpret the value. It specifies the conversion format when outputting values. The format string conforms to C-style printf library functions</span></dd>
    <dd><span class="apidef">post</span><span class="apidesc">Specifies the tag to be output afterward the element.</span></dd>
</dl>

### <i class="fa fa-code"></i> Public member variables

#### <i class="fa fa-caret-right"></i> enable

Enable HTML tag generation for the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">AutoConnect will generate the element into HTML only if the enable attribute is true.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> format

The conversion format when outputting values. The format string conforms to C-style printf library functions.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> global

The global attribute copies input values ​​between elements of the same name on different custom Web pages.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">bool</span><span class="apidesc">An entered value will be copied to elements of the same name in other AutoConnectAuxes during page transition.<br>However, it will be copied only when the destination element has the true for a global attribute.</span></dd></dl>

#### <i class="fa fa-caret-right"></i> name

The element name.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> post

Specifies a tag to add behind the HTML code generated from the element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">ACPosterior_t</span><span class="apidesc">
        
- **`AC_Tag_None`** : No generate additional tags.
- **`AC_Tag_BR`** : Add a `<br>` tag to the end of the element.
- **`AC_Tag_P`** : Include the element in the `<p> ~ </p>` tag.
</span></dd></dl>

#### <i class="fa fa-caret-right"></i> style

A style code with CSS format that qualifiers the text.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

#### <i class="fa fa-caret-right"></i> value

A content string of the text element.<dl class="apidl">
    <dt>**Type**</dt>
    <dd><span class="apidef">String</span><span class="apidesc"></span></dd></dl>

### <i class="fa fa-code"></i> Public member functions

#### <i class="fa fa-caret-right"></i> typeOf

```cpp
ACElement_t typeOf(void)
```

Returns type of AutoConnectElement.<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>AC_Text</dd></dl>
