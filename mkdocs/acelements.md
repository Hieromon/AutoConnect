## The elements for the custome Web pages

Representative HTML elements for making the custom Web page are provided as AutoConnectElements.

- [AutoConnectButton](#autoconnectbutton): Labeled action button
- [AutoConnectCheckbox](#autoconnectcheckbox): Labeled checkbox
- [AutoConnectElement](#autoconnectelement-a-basic-class-of-elements): General tag
- [AutoConnectInput](#autoconnectinput): Labeled text input box
- [AutoConnectRadio](#autoconnectradio): Labeled radio button
- [AutoConnectSelect](#autoconnectselect): Selection list
- [AutoConnectSubmit](#autoconnectsubmit): Submit button
- [AutoConnectText](#autoconnecttext): Style attributed text

Each element has a common attribute and its own attributes.

## Layout on custom Web page

You can specify the direction to arrange the radio buttons as [**AutoConnectRadio**](#autoconnectradio) vertically or horizontally. The checkbox as [**AutoConnectCheckbox**](#autoconnectcheckbox) is not divided in a line by each element. In order to arrange vertically, it is necessary to clearly indicate the **&lt;br&gt;** tag. Other elements are arranged vertically in the order of addition to AutoConnectAux. This basic layout depends on the CSS of the AutoConnect menu so it can not be changed drastically.

## AutoConnectElement - <small>A basic class of elements</small>

AutoConnectElement is a base class for other element classes and has common attributes for all elements. It can also be used as a [variant](#variant-for-autoconnectelements) of each element. The following items are attributes that AutoConnectElement has and are common to other elements.

### name

Every element has a name. **Name** is the String data type. To access its element in the sketches you can identify by the name. Its method is useful for loading elements in JSON. In the load function of AutoConnectElement(s) described [**later**](acjson.md), these objects are not created in advance by sketches. Therefore, access to the attribute of that element by name as follows:

```cpp hl_lines="4 10"
AutoConnectAux customPage("Custom page", "/custom_page");
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

### value

**Value** is the source of the HTML code generated with the element and its data type is String. Characteristics of Value vary depending on the element. The value of AutoConnectElement is native HTML code. A string of value is output as HTML as it is.

### type

**Type** indicates the type of the element and represented as the *ACElement_t* enumeration type in the sketch. Since AutoConnectElement also acts as a variant of other elements, it can be applied to handle elements collectively. At that time, the type can be referred to by the [**typeOf()**](apielements.md#typeof) function. The following example changes the font color of all [AutoConnectText](#autoconnecttext) elements of a custom web page to gray.

```cpp hl_lines="5"
AutoConnectAux  customPage;

AutoConnectElementVT& elements = customPage.getElements();
for (AutoConnectElement& elm : elememts) {
  if (elm.typeOf() == AC_Text) {
    AutoConnectText& text = reinterpret_cast<AutoConnectText&>(elm);
    text.style = "color:gray;";
  }
}
```

The enumerators for *ACElement_t* are as follows:

-  AutoConnetButton: **AC_Button**
-  AutoConnectCheckbox: **AC_Checkbox** 
-  AutoConnectElement: **AC_Element**
-  AutoConnectInput: **AC_Input**
-  AutoConnectRadio: **AC_Radio**
-  AutoConnectSelect: **AC_Select**
-  AutoConnectSubmit: **AC_Submit**
-  AutoConnectText: **AC_Text**
-  Uninitialized element: **AC_Unknown**

## AutoConnectButton

## AutoConnectCheckbox

## AutoConnectInput

## AutoConnectRadio

## AutoConnectSelect

## AutoConnectSubmit

## AutoConnectText

## How to coding for the elements

### Variant for AutoConnectElements



