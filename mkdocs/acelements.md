## The elements for the custom Web pages

Representative HTML elements for making the custom Web page are provided as AutoConnectElements.

- [AutoConnectButton](#autoconnectbutton): Labeled action button
- [AutoConnectCheckbox](#autoconnectcheckbox): Labeled checkbox
- [AutoConnectElement](#autoconnectelement-a-basic-class-of-elements): General tag
- [AutoConnectInput](#autoconnectinput): Labeled text input box
- [AutoConnectRadio](#autoconnectradio): Labeled radio button
- [AutoConnectSelect](#autoconnectselect): Selection list
- [AutoConnectSubmit](#autoconnectsubmit): Submit button
- [AutoConnectText](#autoconnecttext): Style attributed text

## Layout on custom Web page

You can specify the direction to arrange the radio buttons as [**AutoConnectRadio**](#autoconnectradio) vertically or horizontally. Other elements are arranged vertically in the order of addition to AutoConnectAux. This basic layout depends on the CSS of the AutoConnect menu so it can not be changed drastically.

## Form and AutoConnectElements

All AutoConnectElements placed on a custom Web page are included in one form. Its form is fixed and created by AutoConnect. The form's value (usually the text or checkbox you entered) is sent by [AutoConnectSubmit](#autoconnectsubmit) using the POST method.

**name** and **value** of each AutoConnectElement which own form-data are reflected in the query string when the custom Web page was sent. You can retrieve the value in the sketch as the parameter's value of the query string with [**ESP8266WebServer::arg**](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer#getting-information-about-request-arguments) function or **PageArgument** class of [**AutoConnect::on**](api.md#on) handler when the form is submitted.

## AutoConnectElement - <small>A basic class of elements</small>

AutoConnectElement is a base class for other element classes and has common attributes for all elements. It can also be used as a [variant](#variant-for-autoconnectelements) of each element. The following items are attributes that AutoConnectElement has and are common to other elements.

### <i class="fa fa-edit"></i> Constructor

```cpp
AutoConnectElement(const char* name, const char* value)
```

### <i class="fa fa-caret-right"></i> name

Each element has a name. **Name** is the String data type. To access its element in the sketches you can identify by the name. Its method is useful for loading elements in JSON. In the load function of AutoConnectElement(s) described [**later**](acjson.md), these objects are not created in advance by sketches. Therefore, access to the attribute of that element by the **name** as follows:

```cpp hl_lines="4 10"
AutoConnectAux customPage("/custom_page", "Custom page");
const char ELEMENT_TEXT[] PROGMEM = R"(
{
    "name": "text1",
    "type": "ACText",
    "value": "Hello, world"
}
)";
customPage.loadElement(ELEMENT_TEXT);
AutoConnectText& text1 = customPage.getElement<AutoConnectText>("text1");
text1.style = "text-align:center;color:#2f4f4f;"
```

The above example accesses the *text1* element which loaded from JSON using AutoConnectAux's [**getElement**](apiaux.md#getelement) function by name and sets the [**style**](#style) attribute.

### <i class="fa fa-caret-right"></i> value

**Value** is the source of the HTML code generated with the element and its data type is String. Characteristics of Value vary depending on the element. The value of AutoConnectElement is native HTML code. A string of value is output as HTML as it is.<br>

### <i class="fa fa-caret-right"></i> type

**Type** indicates the type of the element and represented as the *ACElement_t* enumeration type in the sketch. Since AutoConnectElement also acts as a variant of other elements, it can be applied to handle elements collectively. At that time, the type can be referred to by the [**typeOf()**](apielements.md#typeof) function. The following example changes the font color of all [AutoConnectText](#autoconnecttext) elements of a custom Web page to gray.

```cpp hl_lines="5"
AutoConnectAux  customPage;

AutoConnectElementVT& elements = customPage.getElements();
for (AutoConnectElement& elm : elements) {
  if (elm.typeOf() == AC_Text) {
    AutoConnectText& text = reinterpret_cast<AutoConnectText&>(elm);
    text.style = "color:gray;";
  }
}
```

The enumerators for *ACElement_t* are as follows:

-  AutoConnectButton: **AC_Button**
-  AutoConnectCheckbox: **AC_Checkbox** 
-  AutoConnectElement: **AC_Element**
-  AutoConnectInput: **AC_Input**
-  AutoConnectRadio: **AC_Radio**
-  AutoConnectSelect: **AC_Select**
-  AutoConnectSubmit: **AC_Submit**
-  AutoConnectText: **AC_Text**
-  Uninitialized element: **AC_Unknown**

Furthermore, to convert an entity that is not an AutoConnectElement to its native type, you must [re-interpret](https://en.cppreference.com/w/cpp/language/reinterpret_cast) that type with c++.

## AutoConnectButton

AutoConnectButton generates an HTML `button type="button"` tag and locates a clickable button to a custom Web page. Currently AutoConnectButton corresponds only to name, value, an onclick attribute of HTML button tag. An onclick attribute is generated from an `action` member variable of the AutoConnectButton, which is mostly used with a JavaScript to activate a script.

### <i class="fa fa-edit"></i> Constructor

```cpp
AutoConnectButton(const char* name, const char* value, const String& action)
```

### <i class="fa fa-caret-right"></i> name

It is the name of the AutoConnectButton element and matches the name attribute of the button tag. It also becomes the parameter name of the query string when submitted.

### <i class="fa fa-caret-right"></i> value

It becomes a value of the `value` attribute of an HTML button tag.

### <i class="fa fa-caret-right"></i> action

**action** is String data type and is an onclick attribute fire on a mouse click on the element. it is mostly used with a JavaScript to activate a script.[^1] For example, the following code defines a custom Web page that copies a content of `Text1` to `Text2` by clicking `Button`.

[^1]:JavaScript can be inserted into a custom Web page using AutoConnectElement.

```cpp hl_lines="3 4 5 10"
const char* scCopyText = R"(
<script>
function CopyText() {
  document.getElementById("Text2").value = document.getElementById("Text1").value;
}
</script>
)";
ACInput(Text1, "Text1");
ACInput(Text2, "Text2");
ACButton(Button, "COPY", "CopyText()");
ACElement(TextCopy, scCopyText);
```

## AutoConnectCheckbox

AutoConnectCheckbox generates an HTML `input type="checkbox"` tag and a `label` tag. It places horizontally on a custom Web page by default.

### <i class="fa fa-edit"></i> Constructor

```cpp
AutoConnectCheckbox(const char* name, const char* value, const char* label, const bool checked)
```

### <i class="fa fa-caret-right"></i> name

It is the name of the AutoConnectCheckbox element and matches the name attribute of the input tag. It also becomes the parameter name of the query string when submitted.

### <i class="fa fa-caret-right"></i> value

It becomes a value of the `value` attribute of an HTML input tag.

### <i class="fa fa-caret-right"></i> label

A label is an optional string. If you specify a label, an `id` attribute is attached to the HTML `checkbox` tag and an HTML `label` tag is generated. Label tag and checkbox will combine by its id attribute. Labels are always placed on the right side of the checkbox. If you do not specify a label, only &#9744; will be displayed.

### <i class="fa fa-caret-right"></i> checked

A checked is a Boolean value and indicates the checked status of the checkbox. The value of the checkbox element checked is packed in the query string and sent.

## AutoConnectInput

AutoConnectInput genarates an HTML `input type="text"` tag and a `label` tag. It can also have a placeholder. The value of the input box is passed to the destination in the query string and can be retrieved programmatically. You can also update from the sketches.

### <i class="fa fa-edit"></i> Constructor

```cpp
AutoConnectInput(const char* name, const char* value, const char* label, const char* placeholder)
```

### <i class="fa fa-caret-right"></i> name

It is the name of the AutoConnectInput element and matches the name attribute of the input tag. It also becomes the parameter name of the query string when submitted.

### <i class="fa fa-caret-right"></i> value



## AutoConnectRadio

## AutoConnectSelect

## AutoConnectSubmit

## AutoConnectText

## How to coding for the elements

### Definition in sketch

Each element can be defined by a macro. By using the macro, you can treat element names that are String types as variables in sketches.[^2]

[^2]: The square brackets in the syntax are optional parameters. The stroke is a selection parameter.

ACElement ( *name* <small>\[</small> , *value* <small>\]</small> )

ACButton ( *name* <small>\[</small> , *value* <small>\]</small> <small>\[</small> , *action* <small>\]</small> )
 
ACCheckbox ( *name* <small>\[</small> , *value* <small>\]</small> <small>\[</small> , *label* <small>\]</small> <small>\[</small> , **true** | **false** <small>\]</small> )

ACInput ( *name* <small>\[</small> , *value* <small>\]</small> <small>\[</small> , *label* <small>\]</small> <small>\[</small> , *placeholder* <small>\]</small> )

ACRadio ( *name* <small>\[</small> , *values* <small>\]</small> <small>\[</small> , *label* <small>\]</small> <small>\[</small> , **AC\_Horitontal** | **AC\_Vertical** <small>\]</small> <small>\[</small> , *checked* <small>\]</small> )

ACSelect ( *name* <small>\[</small> , *options* <small>\]</small> <small>\[</small> , *label* <small>\]</small> )

ACSubmit ( *name* <small>\[</small> , *value* <small>\]</small> <small>\[</small> , *uri* <small>\]</small> )

ACText ( *name* <small>\[</small> , *value* <small>\]</small> <small>\[</small> , *style* <small>\]</small> )

### Variant for AutoConnectElements



