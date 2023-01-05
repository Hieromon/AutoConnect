## Interaction with AutoConnectElements w/o page transition

The substance of the custom web page deployed by AutoConnectAux is just HTML content; AutoConnectAux is just a request handler conforming to the *RequestHandler* class of the ESP8266 and ESP32 Arduino core's WebServer library.

Therefore, the request-response form typically provided by AutoConnectAux is based on stateless HTTP page transitions. Its communication between custom Web pages and sketches involves page transitions in the client browser via the request-response form.

However, major Web browsers support HTTP asynchronous communication without page transitions. By embedding those [Web APIs](https://developer.mozilla.org/en-US/docs/Web/API) in your custom web pages, you can implement sketches that do not disrupt the user working flow with page transitions.

[AutoConnectAux](apiaux.md#autoconnectaux) allows the custom web page to use two types of Web APIs for asynchronous communication with the sketch. Both methods can be accomplished by having JavaScript inherent in the custom web page to communicate with the server sketch (i.e., the AutoConnectAux event handler). 

- **XMLHttpRequest**

    JavaScript embedded in a custom web page uses the [XMLHttpRequest](https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest) (XHR) objects to communicate with the request handler on the sketch side. A sketch typically embeds its JavaScript coded as a string value with [AutoConnectElement](apielements.md#autoconnectelement) into a custom web page JSON description.

    The request handler that is communication partner with the JavaScript should be implemented in the sketch as the [Client request handlers](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/README.rst#client-request-handlers) of the ESP8266WebServer (WebServer for ESP32) class.

    The procedure for implementing a sketch in this manner is covered in [Communicate with the Sketch using XHR](achandling.md#communicate-with-the-sketch-using-xhr) section.

- **Fetch API**

    The [Fetch API](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API/Using_Fetch) supported by AutoConnectAux is even easy to implement than [XHR](https://developer.mozilla.org/en-US/docs/Glossary/XHR_(XMLHttpRequest)). AutoConnectElements can execute Fetch API-driven JavaScript that can communicate with the server sketch. Its script will be triggered by [expected events](#register-event-handling-for-autoconnectelements) and automatically be embedded into the HTML source of your custom web page by AutoConnect.

    Also, the sketch process with which the Fetch API scripts communicates can access and update the values and properties of each AutoConnectElement. Updated AutoConnectElement contents are immediately reflected on the custom web page by sending a response.

    This section describes a Fetch API-driven approach based on AutoConnectElements event firing and the specific API for the sketch implementation.

    !!! info "No retries around Fetch API handling"
        The JavaScript containing the Fetch API that AutoConnect automatically embeds in custom web pages **does not include retry handling**. If the connection with the ESP module is unstable, the request will be reset by the client browser without completing the HTTP transmission/reception. However, the state may be difficult to understand at first glance, and the user may not be able to immediately determine what has happened.

        When applying the Fetch API on the AutoConnect custom web page, it is recommended to keep the amount of communication as low as possible.

## Interact with sketches by AutoConnectElements event

Interaction between AutoConnectElements and sketch without page transitions is very smooth. It allows you to complete data exchange with ESP modules on the same page without interrupting your work.

The figure below illustrates what a custom web page without page transitions looks like in action. It's a screen capture of a custom web page behavior that controls a simple LED blink (like `"hello, world\n"` for *The C Programming Language*), and contains only a caption as [AutoConnectText](acelements.md#autoconnecttext) and a button as [AutoConnectButton](acelements.md#autoconnectbutton) to turn on or off LED. 

<img data-gifffer="images/fetch_led.gif"/>

The LED ON/OFF button as [AutoConnectButton](acelements.md#autoconnectbutton) used in [this sketch](achandling.md#led-control-sketch) handles the HTML element [click event](https://developer.mozilla.org/en-US/docs/Web/API/Element/click_event) to send the current [value](acelements.md#value_1) to the ESP module. The receiver sketch changes the LED signal level according to the received value and responds to the client with the [button's display text](acelements.md#value_1) and [caption color](acelements.md#style_1). These send-and-receive exchanges are attribute names and values for [AutoConnectButton](acelements.md#autoconnectbutton) and [AutoConnectText](acelements.md#autoconnecttext). This sketch handles these actions with the standard HTTP POST method without causing a page transition.

The following sections describe APIs and programming methods for interacting with custom web pages with no page transitions, following the sketch structure that implements the above figure.

### <i class="fas fa-bolt"></i> Associate events with AutoConnectElements

Custom web pages derived by AutoConnectAux are regular HTML, so it can contain JavaScript that responds to [DOM events](https://dom.spec.whatwg.org/#events). The reason why the sketch above figure does not cause a page transition is that the [click event](https://developer.mozilla.org/en-US/docs/Web/API/Element/click_event) with the AutoConnectButton is trigged to send an HTTP POST to the sketch running in the ESP module using the [Fetch API](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API).

AutoConnect **automatically inserts JavaScript** that communicates via the Fetch API in response to events if AutoConnectElements with registered an event handler is present on the custom web page. The following code is the JSON description of the custom web page shown above. It is no different from the page description with no event handling.

```cpp
const char LED_ONOFF[] PROGMEM = R"(
{
  "uri": "/led",
  "title": "LED",
  "menu": true,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "value": "BUILT-IN LED",
      "style": "font-weight:bold;font-size:25px;text-align:center;",
      "posterior": "div"
    },
    {
      "name": "onoff",
      "type": "ACButton",
      "value": "ON"
    }
  ]
}
)";
```

All you need to do is write your custom web page in JSON and write event handlers for AutoConnectElements that apply to user interactions.

### <i class="fas fa-bolt"></i> Allow AutoConnectElements to have event processing

AutoConnect will automatically embed the JavaScript into the HTML that communicates with a server-side sketch using the Fetch API if the custom web page has AutoConnectElements with an event handler. Use the `on` function to allow the event-handling capability to elements used for the interaction with the user.

To give your custom web page the ability to handle events using Fetch API, roughly follow these steps:

1. Declare AutoConnectElements for user interaction. It can be declared directly using the constructor of each element or embedded in the JSON description. This procedure is no different from the definition in custom web pages.

2. Identify elements that allow events after loading a custom web page. Usually, you use the [AutoConnectAux::getElement](apiaux.md#getelement) function (or override [operator `[]`](apiaux.md#operator)) accompanied by the [AutoConnect::aux](api.md#aux) or [AutoConnect::locate](api.md#locate) for this. This function takes the element name as an argument and returns a reference to an instance of AutoConnectElements.

    ```cpp
    AutoConnect portal;

    portal.load(FPSTR(LED_ONOFF));
    AutoConnectAux& led = portal.locate("/led");
    AutoConnectButton& onOff = led["onoff"].as<AutoConnectButton>();
    ```

3. Register the event handler with the [`AutoConnectElement::on`](apielements.md#on) function.

    ```cpp
    onOff.on(ledOnOff);
    ```

4. Write an event handler with the sketch. The event handler will be passed a reference to the instance of AutoConnectElements where the event occurred and a reference to AutoConnectAux. The event handler can use these parameters to receive the element's value of the event occurrence and perform processing according to that value. You can also give attributes such as new values and styles of other elements as return values. Use the [AutoConnectElement::response](apielements.md#response) function to set the return values.

    ```cpp
    // Event handler that attaches to an AutoConnectButton named `led`.
    // This event handler receives a reference to AutoConnectButton as `led`
    // and a reference to the AutoConnectAux of the page rendered in the client
    // browser.
    void ledOnOff(AutoConnectButton& me, AutoConnectAux& ledOnOff) {
      if (me.value == "ON") {
        // Since "ON" has been passed from the AutoConnectButton as `led`. Let the
        // LED turns on.
        digitalWrite(LED_BUILTIN, HIGH);
        // Direct assignment to AutoConnectElement values is not reflected on the
        // web page; use the `response` function to update the value of the element
        // on the web page.
        me.response("OFF");
        // The `on` event handler attached to AutoConnectElements can override the
        // value and attributes of other elements placed on that AutoConnectAux page.
        // For example, a following statement changes the font color of the `caption`
        // element along with a LED blinking.
        ledOnOff["caption"].response("style", "{\"color\":\"red\"}");
      }
      if (me.value == "OFF") {
        // Since a value "OFF" has been passed from the AutoConnectButton as `led`.
        // Let the LED turns off.
        digitalWrite(LED_BUILTIN, LOW);
        me.response("ON");
        ledOnOff["caption"].response("style", "{\"color\":\"black\"}");
      }
    }
    ```

### <i class="fa fa-edit"></i> Register Event handling for AutoConnectElements

The `on` function catches different events for each AutoConnectElements[^1]. There are also types of AutoConnectElements for which event handling cannot be registered. The syntax of the `on` function is as follows:

[^1]: *AutoConnectElements* is a generic term for elements handled by custom web pages. They are actually replaced by types such as `AutoConnectInput` or `AutoConnectText` etc.

void *AutoConnectElements*::on(*eventHandler*)

*AutoConnectElements* that can catch events and the types of events are as follows:

|Available elements for Event|Event type|The moment the event occurs|
|----------------------------|----------|---------------------------|
|[AutoConnectButton](apielements.md#on)|[click](https://developer.mozilla.org/en-US/docs/Web/API/Element/click_event)|Mouse click (only primary button)|
|[AutoConnectCheckbox](apielements.md#on_1)|[change](https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/change_event)|When an element is checked or unchecked|
|[AutoConnectInput](apielements.md#on_2)|[change](https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/change_event)|When the element loses focus after its value was changed|
|[AutoConnectRadio](apielements.md#on_3)|[change](https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/change_event)|When an element is checked (but not when unchecked)|
|[AutoConnectSelect](apielements.md#on_4)|[change](https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/change_event)|When the user commits the change explicitly|

The *eventHandler* parameter specifies the function that handles the event.

### <i class="fa fa-edit"></i> Event handling for AutoConnectElements

#### Event handler function

void *eventHandler*(*AutoConnectElements*& me, AutoConnectAux& aux)

*AutoConnectElements*[^1] is actually one of the types listed in the table above for which the event is available. A reference to the element itself and to the AutoConnectAux to which it belongs is passed to the event handler. For example, to receive an event when an [AutoConnectRadio](acelements.md#autoconnectradio) named `radio` changes its checked state by mouse clicking, declare a function like `onChangeRadio` as follows:

```cpp
void onChangeRadio(AutoConnectRadio& me, AutoConnectAux& aux)
```

Then use the [AutoConnectRadio::on](apielements.md#on_3) function to register the *onChangeRadio* handler function with the `radio` instance of [AutoConnectRadio](apielements.md#autoconnectradio).

```cpp
AutoConnectRadio radio("radio", {"Huey", "Dewey", "Louie"}, "Select one");
...
radio.on(onChangeRadio);
```

*AutoConnectElements*[^1] will handle events if an event handler is registered with the `on` function. AutoConnect automatically inserts a script containing the Fetch API in the HTML generation of your custom web page when an element with an event handler function is registered.

!!! note "A type of event source and type of event handler argument is always the same"
    The first argument of the event handler will contain a reference to an instance of an actual AutoConnectElements that is source of the event. So, the type of AutoConnectElements that registers the event handler and the type passed to the event handler are always the same.  
    For example, the type of the first argument of the handler that receives the change event of AutoConnectText is the type of AutoConnectText itself.

#### Data sent with the event

When the custom web page catches the event, the script inserted by AutoConnect uses the [Fetch API](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API) to send the value of each AutoConnectElements on the page to the ESP module. This script sends the value of each element as [form data](https://developer.mozilla.org/en-US/docs/Learn/Forms/Sending_and_retrieving_form_data) via [HTTP POST](https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/POST).

The data sent in this behavior is the same as the data transmission form with page transitions with [AutoConnectSubmit](apielements.md#autoconnectsubmit). Then AutoConnect will store the transmitted data in the actual instance of AutoConnectElements before control is passed to the user sketch. Details of this behavior are covered in the section [Custom field data handling](achandling.md#custom-field-data-handling).

So in your sketch, you can unconsciously access the value of each AutoConnectElements of the custom web page where the event occurred in the event handler function. A reference to the element where the event originated is passed as the first argument, and a reference to the custom web page where the event triggered as the second argument.

#### Make responses

Responses to an HTTP POST request triggered by the event will be returned by AutoConnect at the end of the event handler function. The event handler continues processing until it exits, depending on the situation. For example, the event handler that controls the LED on/off described above (i.e., `ledOnOff` function) responds to the requested HTTP POST by setting the signal level to the `LED_BUILTIN` port to conform to the sent value of the AutoConnectButton by the event. It then rewrites the button's label with the new value.

Use the [`response`](apielements.md#response) function for this. The [`response`](apielements.md#response) function has the effect of communicating the contents of AutoConnectElements updated by the event handler to the client browser. In the actual event handler, you should call the `response` as a member function of AutoConnectElements and specifies the instance of the element that will return the response. For example, to update the value of an [AutoConnectText](acelements.md#autoconnecttext) named `caption` on a page according to the LED ON/OFF control above, the code would look like this:

```cpp hl_lines="4 15 20"
...

ACButton(onoff, "ON");
ACText(caption, "LED OFF");

...

void eventHandler(AutoConnectButton& me, AutoConnectAux& aux) {

...

  if (me.value == "OFF") {
    digitalWrite(LED_BUILTIN, LOW);
    me.response("ON");
    caption.response("LED OFF");
  }
  if (me.value == "ON") {
    digitalWrite(LED_BUILTIN, HIGH);
    me.response("OFF");
    caption.response("LED ON");
  }

...

}
```

The [AutoConnectButton::response](apielements.md#response) function rewrites the [value attribute](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/button#attr-value) and [innerHTML property](https://developer.mozilla.org/en-US/docs/Web/API/Element/innerHTML) of the `onoff` element (i.e. `button type="button id="onoff"` node) on the page, while the [AutoConnectText::response](apielements.md#response_3) function rewrites only the [innerHTML property](https://developer.mozilla.org/en-US/docs/Web/API/Element/innerHTML) of the `div` or `span` DOM node on the page. In this way, the value returned by response differs depending on the type of AutoConnectElements that generated the event. The following table shows which attributes of elements on the page the `response(const char*)` function affects.

|Available AutoConnectElement::response functions|Attributes to rewrite|
|------------------------------------------------|---------------------|
|[`AutoConnectButton::response`](apielements.md#response)|[value](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/button#attr-value) for the [HTMLButtonElement](https://developer.mozilla.org/en-US/docs/Web/API/HTMLButtonElement)<br>[innerHTML](https://developer.mozilla.org/en-US/docs/Web/API/Element/innerHTML) for the [HTMLButtonElement](https://developer.mozilla.org/en-US/docs/Web/API/HTMLButtonElement) node|
|[`AutoConnectCheckbox::response`](apielements.md#response_1)|[checked](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input/checkbox#attr-checked) for [`<input type="checkbox">`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input/checkbox)|
|[`AutoConnectInput::response`](apielements.md#response_2)|[value](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input/text#value) for the [HTMLInputElement](https://developer.mozilla.org/en-US/docs/Web/API/HTMLInputElement)|
|[`AutoConnectText::response`](apielements.md#response_3)|[innerHTML](https://developer.mozilla.org/en-US/docs/Web/API/Element/innerHTML) for the `<div>` or `<span>` node|

!!! note "When the `response` function updates the value of AutoConnectElements"
    The `response` function also updates the value of the element's instance. For example, [AutoConnectText::response](apielements.md#response_3) function, in addition to sending the text to be updated to the client browser, also updates the [value member](apielements.md#value_10) of the sketch's AutoConnectText variable. However, that update process is done by AutoConnect at the exit of the event handler function.

    So, in the event handler function, even if you execute the response function, the value of AutoConnectElements will be kept as it was before the event occurred.

Each AutoConnectElements has another response function that takes two arguments. The response(const char*) function updates the value of that element, while the `response(const char*, const char*)` function with two arguments updates the specified attribute or property and is used to change attributes of an element other than its value. For example, citing the LED ON/OFF example above, you can change the button background color according to the LED lighting state using the `response` function that takes two arguments.

To change the button background color via an event handler when an event fires, specifies a String of a response form that allows direct access to the inline [styles property](https://developer.mozilla.org/ja/docs/Web/API/HTMLElement/style) of the button element using the [CSSStyleDeclaration](https://developer.mozilla.org/en-US/docs/Web/API/CSSStyleDeclaration) object.

In this case, specify the property name of the HTML element to be changed in the first argument of the response function and the value to be changed in the second argument. This specification format conforms to [object literals](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Grammar_and_types#object_literals) in JavaScript and can be expressed in JSON as follows:

```js
{ style: { backgroundColor: 'red' }}
```

So, specify `style` as the property part of the above syntax to the first argument of the response function and the nested `{backGroundColor:"red"}` part to the second argument.

```cpp
response("style", "{backGroundColor:\"red\"}");
```

If the property to be changed is not a nested object property like an inline style, the second argument can be the property value as it is. For example:

```cpp
response("placeholder", "Enter name");
```

Also, even if you give a boolean value, specify it as a string in the argument to the `response` function. AutoConnect automatically converts "true"/"false" as strings to booleans. For example:

```cpp
response("hidden", "true");
```

### <i class="far fa-image"></i> Overall the LED control sketch

Based on the explanation so far, the following sketch is the implementation of the custom web page shown in the opening figure.

```cpp hl_lines="13 14 20 21 35 45 51 57 58 84 85"
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <AutoConnect.h>

const char LED_ONOFF[] PROGMEM = R"(
{
  "uri": "/led",
  "title": "LED",
  "menu": true,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "value": "BUILT-IN LED",
      "style": "font-weight:bold;font-size:25px;text-align:center;",
      "posterior": "div"
    },
    {
      "name": "onoff",
      "type": "ACButton",
      "value": "ON"
    }
  ]
}
)";

AutoConnect portal;
AutoConnectConfig config;

// Event handler that attaches to an AutoConnectButton named `led`.
// This event handler receives a reference to AutoConnectButton as `led`
// and a reference to the AutoConnectAux of the page rendered in the client
// browser.
void ledOnOff(AutoConnectButton& me, AutoConnectAux& ledOnOff) {
  if (me.value == "ON") {

    // Since "ON" has been passed from the AutoConnectButton as `led`. Let the
    // LED turns on.
    digitalWrite(LED_BUILTIN, HIGH);

    // Direct assignment to AutoConnectElement values is not reflected on the
    // web page; use the `response` function to update the value of the element
    // on the web page.
    me.response("OFF");

    // The `on` event handler attached to AutoConnectElements can override the
    // value and attributes of other elements placed on that AutoConnectAux page.
    // For example, a following statement changes the font color of the `caption`
    // element along with a LED blinking.
    ledOnOff["caption"].response("style", "{\"color\":\"red\", \"font-weight\":\"bold\"}");
  }
  if (me.value == "OFF") {
    // Since a value "OFF" has been passed from the AutoConnectButton as `led`.
    // Let the LED turns off.
    digitalWrite(LED_BUILTIN, LOW);
    me.response("ON");
    ledOnOff["caption"].response("style", "{\"color\":\"black\", \"font-weight\":\"normal\"}");
  }
}

void setup() {
  delay(500);
  Serial.begin(115200);
  Serial.println();

  // Built-in LED port setting up
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Configure AutoConnect connection behavior.
  // Various configurations depending on the demands of your situation.
  config.autoReconnect = true;
  portal.config(config);

  // Load the AutoConnectAux page with the LED ON/OFF button into AutoConnect.
  // The sketch can get its instance using the AutoConnect::locate function
  // after AutoConnectAux is loaded.
  portal.load(FPSTR(LED_ONOFF));
  AutoConnectAux& led = portal.locate("/led");

  // The AutoConnectElement::on function allows the sketch to register an event
  // handler that interacts with the element individually.
  AutoConnectButton& onOff = led["onoff"].as<AutoConnectButton>();
  onOff.on(ledOnOff);

  // Start a portal
  portal.begin();
}

void loop() {
  portal.handleClient();
}
```

Above custom web page features two major AutoConnectElements:

- **`caption`**: AutoConnectText

    Shows the current LED lighting state. The font color also reflects this status. To change the font color, use the [`response`](apielements.md#response_3) function for the `caption` element according to the value of the AutoConnectButton named `onoff` in the event handler.

    ```cpp
    ledOnOff["caption"].response("style", "{\"color\":\"red\", \"font-weight\":\"bold\"}");
    ```

- **`onoff`**: AutoConnectButton

    LED lighting switch placed on the custom web page. Interact with an event handler of the server sketch by catching click events on the browser. The event handler function name is `ledOnOff`. Register an event handler with the [AutoConnectButton::on](apielements.md#on) function to catch a [click event](https://developer.mozilla.org/en-US/docs/Web/API/Element/click_event) on the `onoff` element as an AutoConnectButton.

    ```cpp
    AutoConnectButton& onOff = led["onoff"].as<AutoConnectButton>();
    onOff.on(ledOnOff);
    ```

    In the event handler, you can get the current value of the `onoff` through the reference to AutoConnectButton, which is the first argument as `me`.

    ```cpp
    if (me.value == "ON") {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    ```

    The label on the `onoff` button indicates the instruction to turn the LED signal. So the content will be the opposite of the LED lighting state. Use the [AutoConnectButton::response](apielements.md#response) function to rewrite the label.

    ```cpp
    me.response("OFF");
    ```

<script>
  window.onload = function() {
    Gifffer();
  };
</script>
