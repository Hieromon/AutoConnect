## Convert AutoConnectElements value to actual data type

The values in the AutoConnectElements field of the custom Web page are all typed as String. A sketch needs to be converted to an actual data type if the data type required for sketch processing is not a String type.

The AutoConnect library does not provide the data conversion utility, and its function depends on Arduino language functions or functions of the type class. However, commonly used data conversion methods are generally similar.

Here, represent examples the typical method for the data type conversion for the AutoConnectElements value of custom Web pages.

### <i class="fa fa-exchange"></i> Integer

Use [int()](https://www.arduino.cc/reference/en/language/variables/conversion/intcast/) or [toInt() of String](https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/toint/).

```cpp
AutoConnectInput& input = aux.getElement<AutoConnectInput>("INPUT");
int value = input.value.toInt();
```
You can shorten it and write as like:
```cpp
int value = aux["INPUT"].value.toInt();
```

### <i class="fa fa-exchange"></i> Float

Use [float()](https://www.arduino.cc/reference/en/language/variables/conversion/floatcast/) or [toFloat() of String](https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/tofloat/).

```cpp
AutoConnectInput& input = aux.getElement<AutoConnectInput>("INPUT");
float value = input.value.toFloat();
```
You can shorten it and write as like:
```cpp
float value = aux["INPUT"].value.toFloat();
```

### <i class="fa fa-exchange"></i> Date &amp; Time

The easiest way is to use the [Arduino Time Library](https://www.pjrc.com/teensy/td_libs_Time.html). Sketches must accommodate differences in date and time formats depending on the time zone. You can absorb the difference in DateTime format by using `sscanf` function.[^1]

[^1]:The ssanf library function cannot be used with the old Arduino core.

```cpp
#include <TimeLib.h>

time_t tm;
int Year, Month, Day, Hour, Minute, Second;

AutoConnectInput& input = aux.["INPUT"].as<AutoConnectInput>();
sscanf(input.value.c_str(), "%d-%d-%d %d:%d:%d", &Year, &Month, &Day, &Hour, &Minute, &Second);
tm.Year = CalendarYrToTm(Year);
tm.Month = Month;
tm.Day = Day;
tm.Hour = Hour;
tm.Minute = Minute;
tm.Second = Second;
```

### <i class="fa fa-exchange"></i> IP address

To convert a String to an IP address, use **IPAddress::fromString**. To stringize an instance of an IP address, use **IPAddress::toString**.

```cpp
IPAddress ip;
AutoConnectInput& input aux["INPUT"].as<AutoConnectInput>();
ip.fromString(input.value);
input.value = ip.toString();
```

## Validation for the value

To convert input data correctly from the string, it must match its format. The validation implementation with sketches depends on various perspectives. Usually, the tiny devices have no enough power for the lexical analysis completely. But you can reduce the burden for data verification using the [**pattern**](achandling.md#check-data-against-on-submission) of AutoConnectInput.

By giving a [pattern](acelements.md#pattern) to [AutoConnectInput](apielements.md#pattern), you can find errors in data format while typing in custom Web pages. Specifying the input data rule as a [regular expression](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Regular_Expressions) will validate the type match during input. If there is an error in the format during input, the background color of the field will change to pink. Refer to section [*Handling the custom Web pages*](achandling.md#check-data-against-on-submission).

However, input data will be transmitted even if the value does not match the pattern. Sketches require the validation of the received data. You can use the [AutoConnectInput::isValid](apielements.md#isvalid) function to validate it. The isValid function validates whether the value member variable matches a pattern and returns true or false.

```cpp hl_lines="16 47"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

static const char input_page[] PROGMEM = R"raw(
[
  {
    "title": "IP Address",
    "uri": "/",
    "menu": true,
    "element": [
      {
        "name": "ipaddress",
        "type": "ACInput",
        "label": "IP Address",
        "pattern": "^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$"
      },
      {
        "name": "send",
        "type": "ACSubmit",
        "value": "SEND",
        "uri": "/check"
      }
    ]
  },
  {
    "title": "IP Address",
    "uri": "/check",
    "menu": false,
    "element": [
      {
        "name": "result",
        "type": "ACText"
      }
    ]
  }
]
)raw";

AutoConnect portal;

String checkIPAddress(AutoConnectAux& aux, PageArgument& args) {
  AutoConnectAux&   input_page = *portal.aux("/");
  AutoConnectInput& ipaddress = input_page["ipaddress"].as<AutoConnectInput>();
  AutoConnectText&  result = aux["result"].as<AutoConnectText>();

  if (ipaddress.isValid()) {
    result.value = "IP Address " + ipaddress.value + " is OK.";
    result.style = "";
  }
  else {
    result.value = "IP Address " + ipaddress.value + " error.";
    result.style = "color:red;";
  }
  return String("");
}

void setup() {
  portal.load(input_page);
  portal.on("/check", checkIPAddress);
  portal.begin();
}

void loop() {
  portal.handleClient();
}
```

!!! caution "Regular Expressions for JavaScript"
    Regular expressions specified in the AutoConnectInput pattern conforms to the [JavaScript specification](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Regular_Expressions).

Here, represent examples the typical regular expression for the input validation.

### <img src="images/regexp.png" align="top"> URL

```
^\w+([-+.]\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*$
```

### <img src="images/regexp.png" align="top"> DNS hostname

```
^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$
```

### <img src="images/regexp.png" align="top"> email address [^2]

```
^[a-zA-Z0-9.!#$%&'*+\/=?^_`{|}~-]+@[a-zA-Z0-9-]+(?:\.[a-zA-Z0-9-]+)*$
```

### <img src="images/regexp.png" align="top"> IP Address

```
^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$
```

### <img src="images/regexp.png" align="top"> Date as MM/DD/YYYY [^3]

```
^(0[1-9]|1[012])[- \/.](0[1-9]|[12][0-9]|3[01])[- \/.](19|20)\d\d$
```

!!! caution "Contain with backquote"
    If that regular expression contains a backquote it must be escaped by backquote duplication.

[^2]: This regular expression does not fully support the format of the e-mail address requested in [RFC5322](https://tools.ietf.org/html/rfc5322).
[^3]: This regular expression does not consider semantic constraints. It is not possible to detect errors that do not exist as actual dates.
