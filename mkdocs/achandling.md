## Handing AutoConnectElements with the sketches

AutoConnectElements (ie. they are the elements displayed on the custom Web page) must be contained in AutoConnectAux object. AutoConnectElements declared in sketch must be programmed to add to AutoConnectAux one after another. Elements are automatically included in AutoConnectAux by AutoConnect if you load it from the JSON description. In either method, it is common to use the function of AutoConnectAux to access an element with a sketch.

The AutoConnectAux class has several functions to manipulate AutoConnectElements. The functions can add, delete, retrieve elements, and get and set values.

### <i class="fa fa-edit"></i> Add AutoConnectElements to the AutoConnectAux object

To add AutoConnectElment(s) to an AutoConnectAux object, use the add function.

```cpp
void AutoConnectAux::add(AutoConenctElement& addon)
```
```cpp
void AutoConnectAux::add(AutoConenctElementVT addons)
```

The add function adds specified AutoConnectElement to the AutoConnectAux. If specified the collection of AutoConnectElements as a `std::vector` of the references to each element, these elements added in bulk.

The AutoConnectElements contained in the AutoConnectAux object are uniquely identified by the name. When adding an AutoConnectElement, if an element with the same name already exists in the AutoConnectAux, checking the type, and if it is the same, the value will be replaced. If another type of AutoConnectElement exists with the same name, that add operation will be invalid.[^1] In the following example, an AutoConnectButton as `button` addition is invalid.

[^1]: The valid scope of the name is within an AutoConnectAux.

```cpp hl_lines="3"
AutoConnectAux  aux;
AutoConnectText text("hello", "hello, world");
AutoConnectButton button("hello", "hello, world", "alert('Hello world!')");  // This is invalid.
aux.add({ text, button });
```

Similarly this, the uniqueness of the name is also necessary within the JSON document

```json hl_lines="12"
{
  "name" : "aux",
  "uri" : "/aux",
  "menu" : true,
  "element" : [
    {
      "name": "hello",
      "type": "ACText",
      "value": "hello, world"
    },
    {
      "name": "hello",
      "type": "ACButton",
      "value": "hello, world",
      "action": "alert('Hello world!')"
    }
  ]
}
```

!!! note "Load all elements from JSON document"
    If you load all AutoConnectElements from JSON document into AutoConnect, you do not need to sketch the population process of the AutoConnectElements. It is managed by the AutoConnect library automatically.

### <i class="fa fa-edit"></i> Get AutoConnectElement from the AutoConnectAux

To retrieve an element from AutoConnectAux, use the getElement or getElements function. Normally, the getElement is needed when accessing the value of AutoConnectElement in the sketch.

```cpp
AutoConnectElement* AutoConnectAux::getElement(const String& name);
```

```cpp
template<typename T> T& AutoConnectAux::getElement(const String& name);
```

```cpp
AutoConnectElementVT* AutoConnectAux::getElements(void);
```

The [**getElement**](apiaux.md#getelement) function returns an AutoConnectElement with the specified name as a key. When you use this function, you need to know the type of AutoConnectElement in advance. To retrieve an AutoConnectElement by specifying its type, use the following method.

```cpp
AutoConnectAux  aux;
aux.load("SOME_JSON_DOCUMENT");

// Retrieve the pointer of the AutoConnectText
AutoConnectText* text = reinterpret_cast<AutoConnectText*>(aux.getElement("TEXT_ELEMENT_NAME"));

// Retrieve the reference of the AutoConnectText
AutoConnectText& text = aux.getElement<AutoConnectText>("TEXT_ELEMENT_NAME");
```

The AutoConnectElement type behaves as a variant of other element types. Therefore use cast or template to convert to actual type as above. In the sketch, you access the real type of AutoConnectElement after casting it and storing into the variable.

```cpp
const String auxJson = String("{\"title\":\"Page 1 title\",\"uri\":\"/page1\",\"menu\":true,\"element\":[{\"name\":\"caption\",\"type\":\"ACText\",\"value\":\"hello, world\"}]}");
AutoConenct portal;
portal.load(auxJson);
AutoConnectAux* aux = portal.aux("/page1");  // Identify the AutoConnectAux instance with uri
AutoConenctText& text = aux->getElement<AutoConnectText>("caption");  // Cast to real type and access members
Serial.println(text.value);
```

To get all the AutoConnectElements of an AutoConnectAux object use the [**getElements**](apiaux.md#getelements) function. This function returns the vector of the reference wrapper as *AutoConnectElementVT* to all AutoConnectElements registered in the AutoConnectAux.

```cpp
AutoConnectElementVT& getElements(void)
```

*AutoConnectElementVT* is a predefined type for it and can use methods of [std::vector](https://en.cppreference.com/w/cpp/container/vector)<[std::reference_wrapper](https://en.cppreference.com/w/cpp/utility/functional/reference_wrapper)>.

## Loading &amp; saving AutoConnectElements with JSON

AutoConnect supports that loading the AutoConnectAux as a set of elements, and Loading and saving individual elements. It is not possible to save all AutoConnectElements at once as AutoConnectAux. In both cases the target object is a JSON document.

<img src="../images/ac_load_save.svg">

### <i class="fa fa-upload"></i> Loading AutoConnectAux &amp; AutoConnectElements with JSON

To load a JSON document as AutoConnectAux use the [**AutoConnect::load**](api.md#load) function and load the JSON document of each AutoConnectElement using the [**AutoConnectAux::loadElement**](apiaux.md#loadelement) function. Although the functions of both are similar, the structure of the target JSON document is different.

The [AutoConnect::load](apiaux.md#load) function loads the entire AutoConnectAux and creates both the AutoConnectAux instance and each AutoConnectElement instance. If the JSON document is described as an array of multiple custom Web pages, all pages contained in the array will be generated. Therefore, it is necessary to supply the JSON document of AutoConnectAux as an input of the load function and must contain the elements described [**here**](acjson.md#json-document-structure-for-autoconnectaux).

The [AutoConnectAux::loadElement](apiaux.md#loadelement) function loads the elements individually into an AutoConnectAux object. The structure of its supplying JSON document is not AutoConnectAux. It must be a [JSON structure for AutoConnectElement](acjson.md#json-object-for-autoconnectelements), but you can specify an array.

```cpp
// AutoConnectAux as a custom Web page.
const char page[] PROGMEM = R"raw(
{
  "title": "Settings",
  "uri": "/settings",
  "menu": true,
  "element": [
    {
      "name": "server",
      "type": "ACInput",
      "label": "Server"
    },
    {
      "name": "set",      
      "type": "ACSubmit",
      "value": "SET",
      "uri" : "/set"
    }
  ]
}
)raw";

// Additional AutoConnectElements.
const char addons[] PROGMEM = R"raw(
[
  {
    "name": "notes",
    "type": "ACText",
    "value": "An update period as the below optionally."
  },
  {
    "name": "period",
    "type": "ACRadio",
    "value": [
      "30 sec.",
      "60 sec.",
      "180 sec."
    ],
    "arrange": "vertical",
    "checked": 1
  }
]
)raw";

AutoConnect     portal;
AutoConnectAux* auxPage;

// Load a custom Web page.
portal.load(page);

// Get a '/settings' page
auxPage = portal.aux("/settings");

// Also, load only AutoConnectRadio named the period.
auxPage->loadElement(addons, "period");

// Retrieve a server name from an AutoConnectText value.
AutoConnectText& serverName = auxPage->getElement<AutoConnectText>("server");
Serial.println(serverName.value);
```

### <i class="fa fa-download"></i> Saving AutoConnectElements with JSON

To save the AutoConnectElement as a JSON document, use the [AutoConnectAux::saveElement](apiaux.md#saveelement) function. It serializes the contents of the object based on the type of the AutoConnectElement. You can persist a serialized AutoConnectElements as a JSON document to a stream.

```cpp
// Open a parameter file on the SPIFFS.
SPIFFS.begin();
FILE param = SPIFFS.open("/param", "w");

// Save elements as the parameters.
auxPage->saveElement(param, { "server", "period" });

// Close a parameter file.
param.close();
SPIFFS.end();
```

The example above saves `server` and `period` elements from the AutoConnectAux object as mentioned above to the `/param` file on SPIFFS. Its JSON document of AutoConnectElements saved by its code looks like this:

```json
[
  {
    "name": "server",
    "type": "ACInput",
    "value": "An inputted server name",
    "label": "Server",
    "placeholder": ""
  },
  {
    "name": "period",
    "type": "ACRadio",
    "value": [
      "30 sec.",
      "60 sec.",
      "180 sec."
    ],
    "arrange": "vertical",
    "checked": 2
  }
]
```

## Parameter handling

A sketch can access variables of AutoConnectElements in the custom Web page. The value entered into the AutoConnectElements on the page is stored in the member variable of each element by AutoConnect whenever GET/POST transmission occurs. 

The following diagram shows the flow of the input values of a custom Web page into a sketch and is the basis for actions to manipulate the values of custom Web pages using sketches.

<img src="./images/ac_param_flow.svg">

### <i class="fa fa-desktop"></i> When to pick up the values

### <i class="fa fa-desktop"></i> When setting the initial values

### <i class="fa fa-wrench"></i> How you can reach the valuess

WebServer.args, PageArgument
Handling in 'on' handler

## Transitions of the custom Web pages
