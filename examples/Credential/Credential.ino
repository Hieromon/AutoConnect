/**
 *  AutoConnect for ESP8266.
 *  https://github.com/Hieromon/AutoConnect
 *  Copyright 2018, Hieromon Ikasamo.
 *  Licensed under The MIT License.
 *  https://opensource.org/licenses/mit-license.php
 *  An example sketch for an Arduino library for ESP8266 WLAN configuration
 *  via the Web interface. This sketch provides a conservation measures
 *  utility for saved credentials in EEPROM.
 *  By accessing the root path, you can see the list of currently saved
 *  credentials via the browser. Enter an entry number of the credential,
 *  that entry will be deleted from EEPROM.
 *  This sketch uses PageBuilder to support handling of operation pages.
 */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>
#include <AutoConnectCredentail.h>
#include <PageBuilder.h>

ESP8266WebServer Server;
AutoConnect      Portal(Server);
String viewCredential(PageArgument&);
String delCredential(PageArgument&);

/**
 *  An HTML for the operation page.
 *  In PageBuilder, the token {{SSID}} contained in an HTML template below is
 *  replaced by the actual SSID due to the action of the token handler's
 * 'viewCredential' function.
 *  The number of the entry to be deleted is passed to the function in the
 *  POST method.
 */
static const char  html[] PROGMEM = {
"<!DOCTYPE html>"
"<html>"
"<head>"
  "<meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
  "<style>"
  "html {"
  "font-family:Helvetica,Arial,sans-serif;"
  "-ms-text-size-adjust:100%;"
  "-webkit-text-size-adjust:100%;"
  "}"
  "a:link, a:visited {"
    "background-color: #2f4f4f;"
    "border-radius: 4px;"
    "color: white;"
    "display: inline-block;"
    "padding: 6px 12px;"
    "text-align: center;"
    "text-decoration: none;"
  "}"
  "a:hover, a:active {"
    "background-color: #132020;"
   "}"
  "</style>"
"</head>"
"<body>"
"<form action=\"/del\" method=\"POST\">"
  "<ol>"
  "{{SSID}}"
  "</ol>"
  "<p>Enter deleting entry:</p>"
  "<input type=\"number\" min=\"1\" name=\"num\">"
  "<input type=\"submit\">"
"</form>"
"<p><a href=\"" AUTOCONNECT_URI "\">Menu</a></p>"
"</body>"
"</html>"
};

// URL path as '/'
PageElement elmList(html, {{ "SSID", viewCredential }});
PageBuilder rootPage("/", { elmList });

// URL path as '/del'
PageElement elmDel("{{DEL}}", {{ "DEL", delCredential }});
PageBuilder delPage("/del", { elmDel });

// Retrieve the credential entries from EEPROM, Build the SSID line
// with the <li> tag.
String viewCredential(PageArgument& args) {
  AutoConnectCredential  ac;
  struct station_config  entry;
  String content = "";
  uint8_t  count = ac.entries();          // Get number of entries.

  for (int8_t i = 0; i < count; i++) {    // Loads all entries.
    ac.load(i, &entry);
    // Build a SSID line of an HTML.
    content += String("<li>") + String((char *)entry.ssid) + String("</li>");
  }

  // Returns the '<li>SSID</li>' container.
  return content;
}

// Delete a credential entry, the entry to be deleted is passed in the
// request parameter 'num'.
String delCredential(PageArgument& args) {
  AutoConnectCredential  ac;
  if (args.hasArg("num")) {
    int8_t  e = args.arg("num").toInt();
    if (e > 0) {
      struct  station_config entry;

      // If the input number is valid, delete that entry.
      int8_t  de = ac.load(e, &entry);
      if (de > 0) {
        ac.del((char *)entry.ssid);

        // Returns the redirect response. The page is reloaded and its contents
        // are updated to the state after deletion. It returns 302 response
        // from inside this token handler.
        Server.sendHeader("Location", String("http://") + Server.client().localIP().toString() + String("/"));
        Server.send(302, "text/plain", "");
        Server.client().flush();
        Server.client().stop();

        // Cancel automatic submission by PageBuilder.
        delPage.cancel();
      }
    }
  }
  return "";
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  rootPage.insert(Server);    // Instead of Server.on("/", ...);
  delPage.insert(Server);     // Instead of Server.on("/del", ...);

  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }
}

void loop() {
    Portal.handleClient();
}
