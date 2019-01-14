## Describe custom Web pages with JSON

You can embed custom Web pages written in [**JSON**](https://www.json.org/index.html) into AutoConnect without declaring AutoConnectAux &amp; AutoConnectElements in sketches. Custom Web page declaration in JSON can be included in the sketch in advance as the fixed string, or it can be stored in the external file such as SPIFFS for stream loading. You can also load and save AutoConnectElements objects individually.[^1]

[^1]:Loading and saving AutoConnect parameters adopt this method.

By providing the following JSON document to AutoConnect, you can include the custom Web page as the bellow:

<div style="float:left;width:50%;height:470px;overflow:auto"><img src="./images/ac_json.png"></div>
<img style="margin-left:30px;width:40%;height:470px;" src="./images/ac_mqtt_setting.png">

A JSON document for AutoConnect can also contain declarations of multiple custom web pages. If you fit those multiple pages in one JSON document, sketch processing for loading AutoConnectAux will degenerate further.

!!! caution "Need ArduinoJson v5"
    To process the AutoConnectAux &amp; AutoConnectElements written in the JSON is you need to install the [ArduinoJson version 5](https://arduinojson.org/v5/doc/installation/) library.

## JSON objects &amp; elements for the custom Web page

### <i class="fa fa-caret-right"></i> JSON document structure for AutoConnectAux

An AutoConnectAux is described by a JSON object. The elements that make up an object are as follows:

<pre>
{
    "title" : <i>title</i>,
    "uri" : <i>uri</i>,
    "menu" : <b>true</b> | <b>false</b>,
    "element" : <i>element_array</i>
}
</pre>

#### <i class="fa fa-key"></i> **title**
: A title of the custome Web page. This is string value. String specified *title* will be displayed in the AutoConnection menu.

#### <i class="fa fa-key"></i> **uri**
: String of URI path that specifies where to place the custom web page. It needs to be a location from the root path including '**/**'.

#### <i class="fa fa-key"></i> **menu**
: This is a Boolean value indicating whether to include the custom web page in the AutoConnect menu. If the page only responds to another page and you want to prevent the direct use from the menu, you can exclude from the AutoConnect menu. If this key is false, it will not appear in the menu.

#### <i class="fa fa-key"></i> **element**
: Describe an array of JSON objects as *element_array*. It is a JSON object array of the AutoConnectElements that make up the custom Web page.

!!! note "Order of elements on a custom Web page"
    The order in which AutoConnectElements are placed on a custom web page is the order in the JSON document.

### <i class="fa fa-caret-right"></i> JSON object for AutoConnectElements

A JSON object of AutoConnectElements is described by the parameters of [each constructor](acelements.md#constructor).

<pre>
{
    "name" : <i>name</i>,
    "type" : <i>type</i>,
    <i>key_according_to_type</i> : <i>the_value</i> | <i>array_of_value</i>
}
</pre>

#### <i class="fa fa-key"></i> **name**
: A string of the name for the element.

#### <i class="fa fa-key"></i> **type**
: A string of the type for the element. For this type, specify the following string corresponding to each element.
: -  AutoConnectButton: **ACButton**
: -  AutoConnectCheckbox: **ACCheckbox** 
: -  AutoConnectElement: **ACElement**
: -  AutoConnectInput: **ACInput**
: -  AutoConnectRadio: **ACRadio**
: -  AutoConnectSelect: **ACSelect**
: -  AutoConnectSubmit: **ACSubmit**
: -  AutoConnectText: **ACText**

#### <i class="fa fa-key"></i> **<i>key_according_to_type</i>**

This is different for each AutoConnectElements type, and the key that can be specified by the type is determined.

<i class="fa fa-caret-right"></i> AutoConnectButton
: - **value** : 
: - **action** : 

<i class="fa fa-caret-right"></i> AutoConnectCheckbox
: - **value** :
: - **label** : 
: - **checked** :

<i class="fa fa-caret-right"></i> AutoConnectElement
: - **value** :

<i class="fa fa-caret-right"></i> AutoConnectInput
: - **value** :
: - **label** : 
: - **placeholder** :

<i class="fa fa-caret-right"></i> AutoConnectRadio
: - **value** :
: - **label** : 
: - **arrange** :
: - **checked** :

<i class="fa fa-caret-right"></i> AutoConnectSelect
: - **label** :
: - **option** : 

<i class="fa fa-caret-right"></i> AutoConnectSubmit
: - **value** :
: - **uri** : 

<i class="fa fa-caret-right"></i> AutoConnectText
: - **value** :
: - **style** : 

!!! caution "AutoConnect's JSON parsing process is not perfect"
    It is based on ArduinoJson, but the process is simplified to save memory. As a result, even if there is an unnecessary key, it will not be an error. It is ignored.

## Loading JSON document

AutoConnect supports loading of JSON document from the following instances.

- String
- PROGMEM
- Stream

## Saving JSON document

AutoConnect supports writing of JSON document to the stream.
