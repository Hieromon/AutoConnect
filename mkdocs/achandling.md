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

AutoConnect supports reading the custom Web page definitions written in JSON and also supports loading and saving of AutoConnectElements. In both cases, the target object is a [JSON document for AutoConnect](acjson.md). However, it can not save all AutoConnectElements contained in the page as a custom Web page. (ie. AutoConnectAux)

<img src="../images/ac_load_save.svg">

### <i class="fa fa-upload"></i> Loading AutoConnectAux &amp; AutoConnectElements with JSON

To load a JSON document as AutoConnectAux use the [**AutoConnect::load**](api.md#load) function and load the JSON document of each AutoConnectElement using the [**AutoConnectAux::loadElement**](apiaux.md#loadelement) function. Although the functions of both are similar, the structure of the target JSON document is different.

The [AutoConnect::load](apiaux.md#load) function loads the entire AutoConnectAux and creates both the AutoConnectAux instance and each AutoConnectElement instance. A single JSON document can contain multiple custom Web pages. If you write JSON of AutoConnectAux as an array, the load function generates all the pages contained in that array. Therefore, it is necessary to supply the JSON document of AutoConnectAux as an input of the load function and must contain the elements described [**here**](acjson.md#json-document-structure-for-autoconnectaux).

The [AutoConnectAux::loadElement](apiaux.md#loadelement) function loads the elements individually into an AutoConnectAux object. The structure of its supplying JSON document is not AutoConnectAux. It must be a [JSON structure for AutoConnectElement](acjson.md#json-object-for-autoconnectelements), but you can specify an array.

```cpp hl_lines="49 55"
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

### <i class="fa fa-desktop"></i> Where to pick up the values

The sketch receives the values of AutoConnectElements entered in a custom Web page after sending, but it is not the handler of the page where the values entered. It is necessary to be aware that can accept the entered values by the next page handler after the transition.

Usually, two ways to retrieve entered values we have. One is to use the [ESP8266WebServer::arg](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer#getting-information-about-request-arguments) (or WebServer::arg for ESP32) function in the [`on handler`](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer#client-request-handlers) attached by ESP8266WebServer (WebServer w/ESP32 also).

```cpp hl_lines="33"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

static const char addonJson[] PROGMEM = R"raw(
{
  "title": "Hello",
  "uri": "/hello",
  "menu": true,
  "element": [
    {
      "name": "feels",
      "type": "ACInput",
      "label": "What's up?"
    },
    {
      "name": "send",
      "type": "ACSubmit",
      "value": "Just it!",
      "uri": "/feels"
    }
  ]
}
)raw";

ESP8266WebServer webServer;
AutoConnect portal(webServer);

// Here, /feels handler
void feelsOn() {
  
  // Retrieve the value of a input-box named "feels"
  String feel = webServer.arg("feels");

  // Echo back the value
  String echo = "<html><p style=\"color:blue;font-family:verdana;font-size:300%;\">" + feel + String(" and a bold world!</p></html>");
  webServer.send(200, "text/html", echo);
}

void setup() {
  delay(1000);
  webServer.on("/feels", feelsOn);  // Register /feels handler
  portal.load(addonJson);           // Load a custom Web page
  portal.begin();
}

void loop() {
  portal.handleClient();
}
```

An above example is the most simple sketch of handling values entered into a custom Web page. This sketch obtains the string entered in the AutoConnectInput named `feels` with the `/feels` handler after page transition, and the AutoConnectInput is an `<input type="text" name="feels">` element wrapped in the form as the actual HTML code. 

!!! info "Should be accessed `/_ac` first"
    When you actually try the above sketch, there is no a root handler. So the URL that should be accessed first is `/_ac` concatenated with the local IP address of the esp8266 module.

Another method is effective when custom Web pages have complicated page transitions. It is a way to straight access the AutoConnectElements member value. You can get the AutoConnectElement with the specified name using the [getElement](#get-autoconnectelement-from-the-autoconnectaux) function. The following sketch executes the above example with AutoConnect only, without using the function of ESP8266WebServer.

```cpp hl_lines="47 50"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

static const char addonJson[] PROGMEM = R"raw(
[
  {
    "title": "Hello",
    "uri": "/hello",
    "menu": true,
    "element": [
      {
        "name": "feels",
        "type": "ACInput",
        "label": "What's up?"
      },
      {
        "name": "send",
        "type": "ACSubmit",
        "value": "Just it!",
        "uri": "/feels"
      }
    ]
  },
  {
    "title": "Hello",
    "uri": "/feels",
    "menu": false,
    "element": [
      {
        "name": "echo",
        "type": "ACText",
        "style": "color:blue;font-family:verdana;font-size:300%;"
      }
    ]
  }
]
)raw";

AutoConnect portal;

// Here, /feels handler
String feelsOn(AutoConnectAux& aux, PageArgument& args) {

  // Get the AutoConnectInput named "feels".
  // The where() function returns the AutoConnectAux of the page that triggered this handler.
  AutoConnectInput& feels = portal.where()->getElement<AutoConnectInput>("feels");
  
  // Get the AutoConnectText named "echo".
  AutoConnectText&  echo = aux.getElement<AutoConnectText>("echo");
  
  // Echo back from input-box to /feels page.
  echo.value = feels.value +  String(" and a bold world!");
  return String("");
}

void setup() {
  delay(1000);
  portal.load(addonJson);                       // Load custom Web pages
  portal.on("/feels", feelsOn, AC_EXIT_AHEAD);  // Register /feels handler
  portal.begin();
}

void loop() {
  portal.handleClient();
}
```

The above example handles in the handler for the values of a custom web page. An [AutoConnect::on](api.md#on) function registers a handler for the AutoConnectAux page of the specified uri and can also specify when the handler is called. The argument of the custom Web page handler is an AutoConnectAux of the page itself and the [PageArgument](https://github.com/Hieromon/PageBuilder#arguments-of-invoked-user-function) object.

To retrieve the values entered in a custom Web page you need to access the AutoConnectElement of the page that caused the request to this page and to do this, you use the [AutoConnect::where](api.md#where) function. The `AutoConnect::where` function returns a pointer to the AutoConnectAux object of the custom Web page that caused the HTTP request.

!!! note "The where() function is available for only AutoConnectAux."
    The `AutoConnect::where` function is available only for the AutoConnectAux object. It is invalid for HTTP requests from individual pages registered with the **on** handler of ESP8266WebServer/ESP32. In other words, the `AutoConnect::where` function only returns the last AutoConnecAux page called.

### <i class="fa fa-desktop"></i> When setting the initial values

The `AutoConnect::on` function has a parameter indicating the timing to call a custom Web page handler.

### <i class="fa fa-wrench"></i> How you can reach the values

AutoConnectSubmit uses the POST method to send HTTP requests. A value of AutoConnectInput sent to the ESP8266 or ESP32 with POST is stored in the request body of the HTTP request:
<pre>
POST /feels HTTP/1.1
Host: ESP8266_IP_ADDRESS
name1=value1&name2=value2&name3=value3
</pre>
The query string included in the HTTP request body is parsed by ESP8266WebServer class and retrieved it using the `ESP8266WebServer::arg` function in the handler with sketch side. Its argument specifies the name of the input element.

WebServer.args, PageArgument
Handling in 'on' handler

## Transitions of the custom Web pages

A handler registered with ESP8266Server::on can not coexist with AutoConnectAux::on.
