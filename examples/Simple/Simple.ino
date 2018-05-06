/*
  Simple.ino, Example for the AutoConnect library.
  Copyright (c) 2018, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect

  This software is released under the MIT License.
  https://opensource.org/licenses/MIT
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <time.h>
#include <AutoConnect.h>

ESP8266WebServer Server;
AutoConnect      Portal(Server);
AutoConnectConfig   Config;       // Enable autoReconnect supported on v0.9.4

#define TIMEZONE    (3600 * 9)    // Tokyo
#define NTPServer1  "ntp.nict.jp" // NICT japan.
#define NTPServer2  "time1.google.com"

void rootPage() {
  String  content = 
    "<html>"
    "<head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "</head>"
    "<body>"
    "<h2 align=\"center\" style=\"color:blue;margin:20px;\">Hello, world</h2>"
    "<h3 align=\"center\" style=\"color:gray;margin:10px;\">{{DateTime}}</h3>"
    "<p style=\"padding-top:10px;text-align:center\">" AUTOCONNECT_LINK(COG_32) "</p>"
    "</body>"
    "</html>";
  static const char *wd[7] = { "Sun","Mon","Tue","Wed","Thr","Fri","Sat" };
  struct tm *tm;
  time_t  t;
  char    dateTime[26];

  t = time(NULL);
  tm = localtime(&t);
  sprintf(dateTime, "%04d/%02d/%02d(%s) %02d:%02d:%02d.",
    tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
    wd[tm->tm_wday],
    tm->tm_hour, tm->tm_min, tm->tm_sec);
  content.replace("{{DateTime}}", String(dateTime));
  Server.send(200, "text/html", content);
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  // Behavior a root path of ESP8266WebServer.
  Server.on("/", rootPage);

  // Enable saved past credential by autoReconnect option,
  // even once it is disconnected.
  Config.autoReconnect = true;
  Portal.config(Config);

  // Establish a connection with an autoReconnect option.
  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
    configTime(TIMEZONE, 0, NTPServer1, NTPServer2);
  }
}

void loop() {
  Portal.handleClient();
}

