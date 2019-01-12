## Describe custom Web pages with JSON

You can embed custom Web pages written in [**JSON**](https://www.json.org/index.html) into AutoConnect without declaring AutoConnectAux &amp; AutoConnectElements in sketches. Custom Web page declaration in JSON can be included in the sketch in advance as the fixed string, or it can be stored in the external file such as SPIFFS for stream loading. You can also load and save AutoConnectElements objects individually.[^1]

[^1]:Loading and saving AutoConnect parameters adopt this method.

By providing the following JSON document to AutoConnect, you can include the custom Web page as the bellow:

<div style="float:left;width:50%;height:470px;overflow:auto"><img src="./images/ac_json.png"></div>
<img style="margin-left:30px;width:40%;height:470px;" src="./images/ac_mqtt_setting.png">

!!! note "Order of elements on a custom Web page"
    The order in which AutoConnectElements are arranged on the custom Web page is the order described in JSON.

!!! caution "Need ArduinoJson v5"
    To process the AutoConnectAux &amp; AutoConnectElements written in the JSON is you need to install the [ArduinoJson version 5](https://arduinojson.org/v5/doc/installation/) library.

## JSON objects &amp; elements for the custom Web page

### JSON document structure for AutoConnectAux


