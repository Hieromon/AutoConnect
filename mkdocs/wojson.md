## Suppress increase in memory consumption

Custom Web page processing consumes a lot of memory. AutoConnect will take a whole string of the JSON document for the custom Web pages into memory. The required buffer size for the JSON document of example sketch *mqttRSSI* reaches approximately 3000 bytes. And actually, it needs twice the heap area. Especially this constraint will be a problem with the ESP8266 which has a heap size poor.

AutoConnect can handle custom Web pages without using JSON. In that case, since the ArduinoJson library will not be bound, the sketch size will also be reduced.

## Writing the custom Web pages without JSON

To handle the custom Web pages without using JSON, follow the steps below.

1. Create or define [AutoConnectAux](apiaux.md) for each page.
2. Create or define [AutoConnectElement(s)](acelements.md).
3. Add [AutoConnectElement(s)](acelements.md) to AutoConnectAux.
4. Create more AutoConnectAux containing [AutoConnectElement(s)](acelements.md), if necessary.
5. Register the request handlers for the custom Web pages.
6. [Join](api.md#join) prepared AutoConnectAux(s) to AutoConnect.
7. Invoke [AutoConnect::begin()](api.md#begin).

In addition to the above procedure, to completely cut off for binding with the ArduinoJson library, turn off the ArduinoJson use indicator which is declared by the [AutoConnect definitions](api.md#defined-macros). Its declaration is in **AutoConnectDefs.h** file.[^1]

[^1]:Detaching the ArduinoJson library reduces the sketch size by approximately 10K bytes.

```cpp
// Comment out the AUTOCONNECT_USE_JSON macro to detach the ArduinoJson.
#define AUTOCONNECT_USE_JSON
```

!!! caution "JSON processing will be disabled"
    Commenting out the **AUTOCONNECT_USE_JSON** macro invalidates all functions related to JSON processing. If the sketch is using the JSON function, it will result in a compile error.

## Implementation example without ArduinoJson

The code excluding JSON processing from the mqttRSSI sketch attached to the library is as follows. <small>(It is a part of code. Refer to mqttRSSI_NA.ino for the whole sketch.)</small>

<i class="fa fa-code"></i> The JSON document for mqttRSSI

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
        "value": "<h2>MQTT broker settings</h2>",
        "style": "text-align:center;color:#2f4f4f;padding:10px;"
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
        "label": "Server",
        "pattern": "^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$",
        "placeholder": "MQTT broker server"
      },
      {
        "name": "channelid",
        "type": "ACInput",
        "label": "Channel ID",
        "pattern": "^[0-9]{6}$"
      },
      {
        "name": "userkey",
        "type": "ACInput",
        "label": "User Key"
      },
      {
        "name": "apikey",
        "type": "ACInput",
        "label": "API Key"
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
        "name": "period",
        "type": "ACRadio",
        "value": [
          "30 sec.",
          "60 sec.",
          "180 sec."
        ],
        "label": "Update period",
        "arrange": "vertical",
        "checked": 1
      },
      {
        "name": "newline",
        "type": "ACElement",
        "value": "<hr>"
      },
      {
        "name": "hostname",
        "type": "ACInput",
        "value": "",
        "label": "ESP host name",
        "pattern": "^([a-zA-Z0-9]([a-zA-Z0-9-])*[a-zA-Z0-9]){1,32}$"
      },
      {
        "name": "save",
        "type": "ACSubmit",
        "value": "Save&amp;Start",
        "uri": "/mqtt_save"
      },
      {
        "name": "discard",
        "type": "ACSubmit",
        "value": "Discard",
        "uri": "/"
      }
    ]
  },
  {
    "title": "MQTT Setting",
    "uri": "/mqtt_save",
    "menu": false,
    "element": [
      {
        "name": "caption",
        "type": "ACText",
        "value": "<h4>Parameters saved as:</h4>",
        "style": "text-align:center;color:#2f4f4f;padding:10px;"
      },
      {
        "name": "parameters",
        "type": "ACText"
      },
      {
        "name": "clear",
        "type": "ACSubmit",
        "value": "Clear channel",
        "uri": "/mqtt_clear"
      }
    ]
  }
]
```
<span style="margin-right:6px;margin-top:2px;"><img align="middle" width="32" height="32" src="images/arrow_down.png"></span><i class="fa fa-code"></i> Exclude the JSON and replace to the AutoConnectElements natively

```cpp
// In the declaration,
// Declare AutoConnectElements for the page asf /mqtt_setting
ACText(header, "<h2>MQTT broker settings</h2>", "text-align:center;color:#2f4f4f;padding:10px;");
ACText(caption, "Publishing the WiFi signal strength to MQTT channel. RSSI value of ESP8266 to the channel created on ThingSpeak", "font-family:serif;color:#4682b4;");
ACInput(mqttserver, "", "Server", "^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$", "MQTT broker server");
ACInput(channelid, "", "Channel ID", "^[0-9]{6}$");
ACInput(userkey, "", "User Key");
ACInput(apikey, "", "API Key");
ACElement(newline, "<hr>");
ACCheckbox(uniqueid, "unique", "Use APID unique");
ACRadio(period, { "30 sec.", "60 sec.", "180 sec." }, "Update period", AC_Vertical, 1);
ACSubmit(save, "Start", "mqtt_save");
ACSubmit(discard, "Discard", "/");

// Declare the custom Web page as /mqtt_setting and contains the AutoConnectElements
AutoConnectAux mqtt_setting("/mqtt_setting", "MQTT Setting", true, {
  header,
  caption,
  mqttserver,
  channelid,
  userkey,
  apikey,
  newline,
  uniqueid,
  period,
  newline,
  save,
  discard
});

// Declare AutoConnectElements for the page as /mqtt_save
ACText(caption2, "<h4>Parameters available as:</h4>", "text-align:center;color:#2f4f4f;padding:10px;");
ACText(parameters);
ACSubmit(clear, "Clear channel", "/mqtt_clear");

// Declare the custom Web page as /mqtt_save and contains the AutoConnectElements
AutoConnectAux mqtt_save("/mqtt_save", "MQTT Setting", false, {
  caption2,
  parameters,
  clear
});

// In the setup(),
// Join the custom Web pages and performs begin
  portal.join({ mqtt_setting, mqtt_save });
  portal.begin();
```