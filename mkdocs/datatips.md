## Convert AutoConnectElements value to actual data type

The value of the AutoConnectElements field of the custom Web pages consists of String type for all. Sketches will need to convert them to the actual data type. And then if the data type required for processing in the sketch is not a String type, it is necessary to convert to String type when storing to the AutoConenctElements value.

AutoConnect library does not provide the data conversion utility, and its function depends on Arduino language functions or functions of the type class. However, commonly used data conversion methods are generally similar.

Here, represent examples the typical method for the data type conversion for the AutoConnectElements value of custom Web pages.

### <i class="fa fa-exchange"></i> Integer

Use [int()](https://www.arduino.cc/reference/en/language/variables/conversion/intcast/) or [toInt() of String](https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/toint/).

```cpp
AutoConnectInput& input = aux.getElement<AutoConnectInput>("INPUT");
int value = input.value.toInt();
```

### <i class="fa fa-exchange"></i> Float

Use [float()](https://www.arduino.cc/reference/en/language/variables/conversion/floatcast/) or [toFloat() of String](https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/tofloat/).

```cpp
AutoConnectInput& input = aux.getElement<AutoConnectInput>("INPUT");
float value = input.value.toFloat();
```

### <i class="fa fa-exchange"></i> Date &amp; Time

The easiest way is to use the [Arduino Time Library](https://www.pjrc.com/teensy/td_libs_Time.html). Sketches must accommodate differences in date and time formats depending on the time zone. You can absorb the difference in DateTime format by using `sscanf` function.[^1]

[^1]:It can not be used with the old Arduino core.

```cpp
#include <TimeLib.h>

time_t tm;
int Year, Month, Day, Hour, Minute, Second;

AutoConnectInput& input = aux.getElement<AutoConnectInput>("INPUT");
sscanf(input.value.c_str(), "%d-%d-%d %d:%d:%d", &Year, &Month, &Day, &Hour, &Minute, &Second);
tm.Year = CalendarYrToTm(Year);
tm.Month = Month;
tm.Day = Day;
tm.Hour = Hour;
tm.Minute = Minute;
tm.Second = Second;
```

### <i class="fa fa-exchange"></i> IP adderss

To convert a String to an IP address, use **IPAddress::fromString**. To stringize an instance of an IP address, use **IPAddress::toString**.

```cpp
IPAddress ip;
AutoConnectInput& input aux.getElement<AutoConnectInput>("INPUT");
ip.fromString(input.value);
input.value = ip.toString();
```

## Validation for the value

In order for data to be correctly converted from a string, the input data must be consistent with the format. How to implement strict validation in sketches depends on various perspectives and the power of tiny devices is not enough to implement a complete lexical analysis. But you can reduce the burden for data verification using the **pattern** of AutoConnectInput.

By giving a [**pattern**](achandling.md#check-data-against-on-submission) to [AutoConnectInput](apielements.md#pattern), you can find errors in data styles while typing in custom Web pages. The pattern is specified by [**regular expression**](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Regular_Expressions). If the value during input of AutoConnectInput does not match the regular expression specified in the pattern, its background color changes to pink. Refer to [Handling the custom Web pages](achandling.md#check-data-against-on-submission) section.

!!! caution "Regular Expressions for JavaScript"
    Regular expressions specified in the AutoConnectInput pattern conforms to the [JavaScript specification](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Regular_Expressions).

Here, represent examples the typical regular expression for the input validation.

### <i class="fa fa-check-square"></i> URL

```
^\w+([-+.]\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*$
```

### <i class="fa fa-check-square"></i> DNS hostname

```
^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$
```

### <i class="fa fa-check-square"></i> email address [^1]

```
^[a-zA-Z0-9.!#$%&'*+\/=?^_`{|}~-]+@[a-zA-Z0-9-]+(?:\.[a-zA-Z0-9-]+)*$
```

### <i class="fa fa-check-square"></i> IP Address

```
^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$
```

### <i class="fa fa-check-square"></i> Date as MM/DD/YYYY [^2]

```
^(0[1-9]|1[012])[- \/.](0[1-9]|[12][0-9]|3[01])[- \/.](19|20)\d\d$
```

!!! caution "Contain with backquote"
    If that regular expression contains a backquote it must be escaped by backquote duplication.

[^1]: This regular expression does not fully support the format of the e-mail address requested in [RFC5322](https://tools.ietf.org/html/rfc5322).
[^2]: This regular expression does not consider semantic constraints. It is not possible to detect errors that do not exist as actual dates.