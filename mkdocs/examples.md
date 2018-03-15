## How to embed the AutoConnect

Here holds two case examples. Both examples perform the same function. Only how to incorporate the **AutoConnect** into the sketch differs. Also included in the sample folder, HandlePortal.ino also shows how to use the [PageBuilder](https://github.com/Hieromon/PageBuilder) library for HTML assemblies.

## What does this example do?

Uses the web interface to light the LED connected to the **[NodeMCU](https://github.com/nodemcu/nodemcu-devkit-v1.0)** module D0 port (which could be referred to as *BUILTIN_LED*), the following animation is it.

Access to the ESP8266 module connected WiFi from the browser then the page contains the current value of the D0 port would be displayed. The page has the buttons to switch the port value. The LED blinks according to the value of the button that was clicked. This example is a typical sketch of manipulating ESP8266's GPIO via WLAN.

<img data-gifffer="../images/ac2.gif" />

Embed AutoConnect library into this sketch. There are few places to be changed. And you can use AutoConnect's captive portal function to establish a connection freely to other WiFi spots.

## Embed AutoConnect

### <i class="fa fa-code" aria-hidden="true"></i> Pattern A.

Bind to ESP8266WebServer, performs handleClient with handleRequest.

<img src="../images/handleClient.svg" />

!!! hint "In what situations should the handleRequest be used."
    It is something needs to be done immediately after the handle client. It is better to call only AutoConnect::handleClient whenever possible.

### <i class="fa fa-code" aria-hidden="true"></i> Pattern B.

Declare only AutoConnect, performs handleClient.

<img src="../images/handlePortal.svg" />

<script>
  window.onload = function() {
    Gifffer();
  }
</script>
