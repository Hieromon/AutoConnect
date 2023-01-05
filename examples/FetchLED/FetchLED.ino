/*
  FetchLED.ino, Example for the AutoConnect library.
  Copyright (c) 2023, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect
  This software is released under the MIT License.
  https://opensource.org/licenses/MIT

  This sketch demonstrates an example that allows user interaction without
  page transitions by responding to events on AutoConnectElements.
  Details are covered in the AutoConnect documentation.
  https://hieromon.github.io/AutoConnect/acinteract.html
*/

#include <Arduino.h>
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
using WebServer = ESP8266WebServer;
#define LED_ACTIVELEVEL LOW
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#define LED_ACTIVELEVEL HIGH
#endif
#include <AutoConnect.h>

// Adapt to the actual board you use if not declared LED_BUILTIN.
#ifndef LED_BUILTIN
#define LED_BUILTIN   25
#endif

// Define a path to the custom web page for the LED blink control.
#define LED_ENDPOINT  "/led"

const char LED_ONOFF[] PROGMEM = R"(
{
  "uri": ")" LED_ENDPOINT R"(",
  "title": "LED",
  "menu": true,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "value": "BUILT-IN LED",
      "style": "font-size:25px;text-align:center;",
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
// AutoConnectConfig config("MySSIDname", "123456789");

// Event handler that attaches to an AutoConnectButton named `led`.
// This event handler receives a reference to AutoConnectButton as `led`
// and a reference to the AutoConnectAux of the page rendered in the client
// browser.
void ledOnOff(AutoConnectButton& me, AutoConnectAux& ledOnOff) {
  if (me.value == "ON") {

    // Since "ON" has been passed from the AutoConnectButton as `led`. Let the
    // LED turns on.
    digitalWrite(LED_BUILTIN, LED_ACTIVELEVEL);

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
    digitalWrite(LED_BUILTIN, ~LED_ACTIVELEVEL & 0x1);
    me.response("ON");
    ledOnOff["caption"].response("style", "{\"color\":\"black\", \"font-weight\":\"normal\"}");
  }
}

void setup() {
  delay(500);
  Serial.begin(115200);
  Serial.println();

  // Built-in LED port setting up
  Serial.printf("LED port assignment GPIO #%d\n", LED_BUILTIN);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ~LED_ACTIVELEVEL & 0x1);

  // Configure AutoConnect connection behavior.
  // Various configurations depending on the demands of your situation.
  config.autoReconnect = true;
  portal.config(config);

  // Load the AutoConnectAux page with the LED ON/OFF button into AutoConnect.
  // The sketch can get its instance using the AutoConnect::locate function
  // after AutoConnectAux is loaded.
  portal.load(FPSTR(LED_ONOFF));
  AutoConnectAux& led = portal.locate(LED_ENDPOINT);

  // The AutoConnectElement::on function allows the sketch to register an event
  // handler that interacts with the element individually.
  AutoConnectButton& onOff = led["onoff"].as<AutoConnectButton>();
  onOff.on(ledOnOff);

  // Start a portal
  if (portal.begin()) {
    Serial.println("AutoConnect established a WiFi connection at " + WiFi.localIP().toString());
    Serial.println("Access to blink the LED http://" + WiFi.localIP().toString() + LED_ENDPOINT);
  }
}

void loop() {
  portal.handleClient();
}
