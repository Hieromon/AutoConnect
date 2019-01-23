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

The add function adds specified AutoConnectElement to the AutoConnectAux. If speficied the collection of AutoConnectElements as a `std::vector` of the references to each element, these elements added in bulk.

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

## Loading &amp; saving AutoConnectElements

## Saving AutoConnectElements 

## Page transition

## Parameter handling

WebServer.args, PageArgument
Handling in 'on' handler

A sketch can access variables of AutoConnectElements in the custom Web page. The value entered into the AutoConnectElements on the page is stored in the member variable of the element by AutoConnect whenever GET/POST transmission occurs. 