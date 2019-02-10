## What it is

<span style="margin-left:20px;margin-bottom:10px;float:right;width:280px;height:497px;"><img data-gifffer="./images/aux_ov.gif" data-gifffer-width="280" data-gifffer-height="497" /></span>
AutoConnect can handle custom Web pages prepared by user sketches individually. Custom Web pages are displayed in the [AutoConnect menu](menu.md) and can be used from there. It can also have input-output parameters and handle it with sketches. For example, you can program some sketches that publish messages by entering the URI or unique ID of the MQTT broker on a custom page. You do not need to code the processing to handle the web page, it retrieves the input parameters and passes to the MQTT broker connection API is only.

## How it works

Custom Web pages are dynamically created by AutoConnect. The Classes and APIs necessary for dynamic creation are provided. [**AutoConnectAux**](apiaux.md#autoconnectaux) is an object dependent on AutoConnect, which provides an easy way to incorporate custom Web pages like the one on the right figure into AutoConnect. The elements that make up a custom web page are provided as an [**AutoConnectElement**](acelements.md) class. Furthermore, an input box, a check box, a submit button, etc. are implemented by classes derived from AutoConnectElement.
<p align="center"><img align="center" width="560px" src="./images/ac_objects.svg"></p>
AutoConnectAux is a container for AutoConnectElements. To make a custom Web page, create elements that make up the page and put it in the AutoConnectAux object. Joining its AutoConnectAux object to AutoConnect will integrate the custom Web page into the AutoConnect menu.

<img src="./images/ac_declaration.svg">

The above figure shows a code sequence that declares AutoConnectElements and put in the AutoConnectAux container and integrates those into AutoConnect. It declares two text elements named &#39;*header*&#39;and &#39;*caption*&#39;, adds them to the AutoConnectAux object &#39;*aux*&#39;, binds to an AutoConnect object named &#39;*portal*&#39;. It's the basic procedure for configuring the custom Web page. Also, further explanation of AutoConnectElements is the [documentation](acelements.md).

## Custom Web pages in AutoConnect menu

<ul class="horizontal-list">
    <li><img style="margin:0 20px 0 -20px;float:left;width:300px;height:343px;" src="./images/ac_auxmenu.png">
    Custom Web pages as AutoConnectAux are integrated into the <a href="menu.html">AutoConnect menu</a>. AutoConnectAux object contains member variables which are URI and the title. It also has an indicator to show on the AutoConnect menu. You give the title and URI of the custom web page to the AutoConnectAux object with Sketch. Then the title of the custom Web page would be displayed at the bottom of the AutoConnect menu as the left figure.<sup id="fnref:1"><a class="footnote-ref" href="#fn:1" rel="footnote">1</a></sup> It is a hyperlink as URI you given to the custom Web page and taps it will display a page composed of AutoConnectElements. Also, a title will display in the upper left corner of the page, which includes a hyperlink to the AutoConnect statistics screen.</li>
</ul>

[^1]:There is no overlay in the actual menu.

## Multiple custom Web pages

You can create multiple custom Web pages and specify which pages you can invoke from the menu.<br>
In the sketch below, it shows the sequence of codes to integrate three custom Web pages into one and embed them into the menu.

<img align="center" width="520px" src="./images/ac_auxjoin_multi.svg">

<ul class="horizontal-list">
    <li><p style="margin:0 20px 0 -20px"><span style="margin-left:20px;float:right;width:280px;height:497px;"><img data-gifffer="./images/aux_menu.gif" data-gifffer-width="280" data-gifffer-height="497" /></span>In the above code, the third parameter of &#39;<i>aux2</i>&#39; is <b>false</b>. The third parameter of the AutoConnectAux constructor is an indicator of whether it's shown to the AutoConnect menu. Right animation is the execution result of the above code. You will see that the custom Web page&#39;s menu is displayed only in the last two lines. The sketch of this animation is written to transition to &#39;<i>aux2</i>&#39; by the utility of the <a href="acelements.html#autoconnectsubmit"><b>AutoConnectSubmit</b></a> element owned by &#39;<i>aux1</i>&#39;.<sup id="fnref:2"><a class="footnote-ref" href="#fn:2" rel="footnote">2</a></sup><br>An &#39;<i>aux2</i>&#39; page transitions only from the &#39;<i>aux1</i>&#39;. It is a page that saves the parameters you entered on the previous page as shown in mqttRSSI in the library example. It is to want to hide &#39;<i>aux2</i>&#39; from AutoConnect menu lines. The utility of the third parameter of the AutoConnectAux constructor is that.</p></li>
</ul>

[^2]:
    The sketch is actually this:
    ```cpp
    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>
    #include <AutoConnect.h>
    
    AutoConnect     portal;
    
    ACText(header, "MQTT broker settings");
    ACText(caption1, "Publishing the WiFi...");
    ACSubmit(save, "SAVE", "/mqtt_save");
    AutoConnectAux  aux1("/mqtt_setting", "MQTT Setting", { header, caption1, save });
    
    ACText(caption2, "Save parameters");
    ACSubmit(start, "START", "/mqtt_start"); 
    AutoConnectAux  aux2("/mqtt_save", "MQTT Setting", false, { caption2, start });
    
    AutoConnectAux  aux3("/mqtt_start", "MQTT Start");
    
    void setup() {
      portal.join({ aux1, aux2, aux3 });
      portal.begin();
    }
    
    void loop() {
      portal.handleClient();
    }
    ```

## Basic steps to use custom Web pages

So, the basic procedure for handling of the custom Web pages is as follows:

1. Create or define AutoConnectAux.
2. Create or define [AutoConnectElement(s)](acelements.md).
3. Add [AutoConnectElement(s)](acelements.md) to AutoConnectAux.
4. Create more AutoConnectAux containing [AutoConnectElement(s)](acelements.md), if necessary.
5. Register the request handlers for the custom Web pages
6. Join prepared AutoConnectAux(s) to [AutoConnect](api.md#join).
7. Invoke [AutoConnect::begin()](api.md#begin).

## Write the custom Web page with JSON

You can write the custom Web page in JSON without using sketch codes.[^3] It is possible to describe the entire page in JSON and can be described for each element also. The JSON description can be saved in SPIFFS or SD and read using AutoConnect's [**load**](api.md#load) function. If you take this approach, you can further reduce the steps of the above but this way consumes a lot of memory.
The following JSON code and sketch will execute the custom Web page as the example in the above figure. That is, the sketch of this code and footnote[^2] is equivalent.

**custom_page.json**
```json
[
  {
    "title": "MQTT Setting",
    "uri": "/mqtt_setting",
    "menu": true,
    "element": [
      {
        "name": "header",
        "type": "ACText",
        "value": "MQTT broker settings"
      },
      {
        "name": "caption1",
        "type": "ACText",
        "value": "Publishing the WiFi..."
      },
      {
        "name": "save",
        "type": "ACSubmit",
        "value": "SAVE",
        "uri": "/mqtt_save"
      }
    ]
  },
  {
    "title": "MQTT Setting",
    "uri": "/mqtt_save",
    "menu": false,
    "element": [
      {
        "name": "caption2",
        "type": "ACText",
        "value": "Save parameters"
      },
      {
        "name": "start",
        "type": "ACSubmit",
        "value": "START",
        "uri": "/mqtt_start"
      }
    ]
  },
  {
    "title": "MQTT Start",
    "uri": "/mqtt_start",
    "menu": true,
    "element": []
  }
]
```

**The sketch**
```cpp hl_lines="11 12"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <AutoConnect.h>

AutoConnect  portal;

void setup() {
  SPIFFS.begin();

  File page = SPIFFS.open("/custom_page.json", "r");
  portal.load(page);

  page.close();
  SPIFFS.end();

  portal.begin();
}

void loop() {
  portal.handleClient();
}
```

[^3]: Installation of the [ArduinoJson](https://github.com/bblanchon/ArduinoJson) as the latest release of version 5 series is required.

## Passing parameters with sketches and custom Web pages

A sketch can access variables of [AutoConnectElements](acelements.md) in the custom Web page. The value entered into the AutoConnectElements on the page is stored to the [member variables](acelements.md#form-and-autoconnectelements) of the element by AutoConnect whenever GET / POST transmission occurs. Your sketches can get these values with the GET / POST request handler. If you assign a value to an element before a request to the page occurs, its value will appear as the initial value when the page is displayed. Details are explained in the [Parameter handling](achandling.md#parameter-handling).

<script>
  window.onload = function() {
    Gifffer();
  };
</script>
