## Describe custom Web pages with JSON

You can embed custom Web pages written in JSON into AutoConnect without declaring AutoConnectAux &amp; AutoConnectElements in sketches. Custom Web page declaration in JSON can be included in the sketch in advance as the fixed string, or it can be stored in the external file such as SPIFFS for stream loading. You can also load and save AutoConnectElements objects individually.[^1]

<div class="codehilite" style="overflow:auto;width:400px;height:400px;"><pre>
{
  "title": "MQTT Setting",
  "uri": "/mqtt_setting",
  "menu": true,
  "element": [
    {
      "name": "header",
      "type": "ACText",
      "value": "<h2>MQTT broker settings</h2>",
      "style": "text-align:center;color:#2f4f4f;"
    },
    {
      "name": "caption",
      "type": "ACText",
      "value": "Publishing the WiFi signal strength to MQTT channel. RSSI value of ESP8266 to the channel created on ThingSpeak",
      "style": "font-family:serif;color:#4682b4;"
    },
    {
      "name": "mqttserver",
      "type": "ACInput",
      "value": "",
      "placeholder": "MQTT broker server",
      "label": "Server"
    },
    {
      "name": "channelid",
      "type": "ACInput",
      "value": "",
      "label": "Channel ID"
    },
    {
      "name": "userkey",
      "type": "ACInput",
      "value": "NRTFYGJ6TJFGX4RC",
      "label": "User Key"
    },
    {
      "name": "apikey",
      "type": "ACInput",
      "value": "",
      "label": "API Key"
    },
    {
      "name": "period",
      "type": "ACRadio",
      "label": "Update period",
      "value": [
        "30 sec.",
        "60 sec.",
        "180 sec."
      ],
      "arrange": "vertical",
      "checked": 1
    },
    {
      "name": "newline",
      "type": "ACElement",
      "value": "<hr>"
    },
    {
      "name": "uniqueid",
      "type": "ACCheckbox",
      "value": "unique",
      "label": "Use APID unique",
      "checked": false
    },
    {
      "name": "hostname",
      "type": "ACInput",
      "label": "ESP host name",
      "value": ""
    },
    {
      "name": "save",
      "type": "ACSubmit",
      "value": "Save&Start",
      "uri": "/mqtt_save"
    },
    {
      "name": "discard",
      "type": "ACSubmit",
      "value": "Discard",
      "uri": "/"
    }
  ]
}
```
</pre></div>

[^1]:Loading and saving AutoConnect parameters adopt this method.

!!! caution "Need ArduinoJson v5"
    To process the AutoConnectAux &amp; AutoConnectElements written in the JSON is you need to install the [ArduinoJson version 5](https://arduinojson.org/v5/doc/installation/) library.

The order in which AutoConnectElements are arranged on the custom Web page is the order described in JSON.