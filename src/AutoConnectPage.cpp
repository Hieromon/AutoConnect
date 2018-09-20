/**
 *  AutoConnect portal site web page implementation.
 *  @file   AutoConnectPage.h
 *  @author hieromon@gmail.com
 *  @version    0.9.5
 *  @date   2018-08-27
 *  @copyright  MIT license.
 */

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
extern "C" {
#include <user_interface.h>
}
#elif defined(ARDUINO_ARCH_ESP32)
#include <esp_spi_flash.h>
#include <WiFi.h>
#define ENC_TYPE_NONE WIFI_AUTH_OPEN
#endif
#include "AutoConnect.h"
#include "AutoConnectPage.h"
#include "AutoConnectCredential.h"

/**< Basic CSS common to all pages */
const char AutoConnect::_CSS_BASE[] PROGMEM = {
  "html{"
  "font-family:Helvetica,Arial,sans-serif;"
  "font-size:16px;"
  "-ms-text-size-adjust:100%;"
  "-webkit-text-size-adjust:100%;"
  "-moz-osx-font-smoothing:grayscale;"
  "-webkit-font-smoothing:antialiased;"
  "}"
  "body{"
  "margin:0;"
  "padding:0;"
  "}"
  ".base-panel{"
  "margin:0 22px 0 22px;"
  "}"
  ".base-panel>*>label{"
  "display:inline-block;"
  "width:3.0em;"
  "text-align:right;"
  "}"
  "input{"
  "-moz-appearance:none;"
  "-webkit-appearance:none;"
  "font-size:0.9em;"
  "margin:8px 0 auto;"
  "}"
  ".lap{"
  "visibility:collapse;"
  "}"
  ".lap:target{"
  "visibility:visible;"
  "}"
  ".lap:target .overlap{"
  "opacity:0.7;"
  "transition:0.3s;"
  "}"
  ".lap:target .modal_button{"
  "opacity:1;"
  "transition:0.3s;"
  "}"
  ".overlap{"
  "top:0;"
  "left:0;"
  "width:100%;"
  "height:100%;"
  "position:fixed;"
  "opacity:0;"
  "background:#000;"
  "z-index:1000;"
  "}"
  ".modal_button{"
  "border-radius:13px;"
  "background:#660033;"
  "color:#ffffcc;"
  "padding:20px 30px;"
  "text-align:center;"
  "text-decoration:none;"
  "letter-spacing:1px;"
  "font-weight:bold;"
  "display:inline-block;"
  "top:40%;"
  "left:40%;"
  "width:20%;"
  "position:fixed;"
  "opacity:0;"
  "z-index:1001;"
  "}"
};

/**< non-marked list for UL */
const char AutoConnect::_CSS_UL[] PROGMEM = {
  "ul.noorder{"
  "padding:0;"
  "list-style:none;"
  "}"
  "ul.noorder>*>label{"
  "display:inline-block;"
  "width:86px;"
  "margin-right:10px;"
  "text-align:right;"
  "}"
};

/**< Image icon for inline expansion, the lock mark. */
const char AutoConnect::_CSS_ICON_LOCK[] PROGMEM = {
  ".img-lock{"
  "display:inline-block;"
  "width:24px;"
  "height:24px;"
  "margin-left:12px;"
  "vertical-align:middle;"
  "background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAACXBIWXMAAAsTAAALEwEAmpwYAAAB1ElEQVRIibWVu0scURTGf3d2drBQFAWbbRQVCwuVLIZdi2gnWIiF/4GtKyuJGAJh8mgTcU0T8T8ICC6kiIVu44gvtFEQQWwsbExQJGHXmZtiZsOyzCN3Vz+4cDjfvec7j7l3QAF95onRZ54YKmdE1IbnS0c9mnAyAjkBxDy3LRHrjtRyu7OD52HntTAyvbw/HxP2hkCearrRb2WSCSuTTGi60S+QpzFhbwznDl/VVMHw0sF7hEjFbW2qkB38lfp8nNDipWcATil+uDM3cDWyeNRSijnfkHJnezb5Vkkgvbg3IOXD2e1ts93S+icnkZOAVaalZK3YQMa4L+pC6L1WduhYSeCf0PLBdxzOjZ93Lwvm6APAiLmlF1ubPiHotmaS41ExQjH0ZbfNM1NAFpgD0lVcICIrANqAVaAd+AFIYAy4BqaBG+Wsq5AH3vgk8xpYrzf4KLAZwhe8PYEIvQe4vc6H8Hnc2dQs0AFchvAXQGdEDF8s4A5TZS34BQqqQNaS1WMI3KD4WUbNoBJfce9CO7BSr4BfBe8A21vmUwh0VdjdTyHwscL+UK+AHxoD7FDoAX6/Cnpxn4ay/egCjcCL/w1chkqLakLQ/6ABhT57uAd+Vzv/Ara3iY6fK4WxAAAAAElFTkSuQmCC) no-repeat;"
  "}"
};

/**< INPUT button and submit style */
const char AutoConnect::_CSS_INPUT_BUTTON[] PROGMEM = {
  "input[type=\"button\"],input[type=\"submit\"]{"
  "padding:8px 30px;"
  "font-weight:bold;"
  "letter-spacing:0.8px;"
  "color:#fff;"
  "border:1px solid;"
  "border-radius:2px;"
  "margin-top:12px;"
  "}"
  "input[type=\"button\"]{"
  "background-color:#1b5e20;"
  "border-color:#1b5e20;"
  "width:16em;"
  "}"
  "input#sb[type=\"submit\"]{"
  "width:16em;"
  "}"
  "input[type=\"submit\"]{"
  "background-color:#006064;"
  "border-color:#006064;"
  "}"
  "input[type=\"button\"], input[type=\"submit\"]:focus,"
  "input[type=\"button\"], input[type=\"submit\"]:active {"
  "outline:none;"
  "text-decoration:none;"
  "}"
};

/**< INPUT text style */
const char AutoConnect::_CSS_INPUT_TEXT[] PROGMEM = {
  "input[type=\"text\"], input[type=\"password\"]{"
  "background-color:#fff;"
  "padding:10px;"
  "border:1px solid #ccc;"
  "margin:8px 0 8px 0;"
  "width:calc(100% - 124px);"
  "border-radius:2px;"
  "font-weight:300;"
  "color:#444;"
  "-webkit-transition:all 0.20s ease-in;"
  "-moz-transition:all 0.20s ease-in;"
  "-o-transition:all 0.20s ease-in;"
  "-ms-transition:all 0.20s ease-in;"
  "transition:all 0.20s ease-in;"
  "}"
  "input[type=\"text\"]:focus,input[type=\"password\"]:focus{"
  "outline:none;"
  "border-color:#5C9DED;"
  "box-shadow:0 0 3px #4B8CDC;"
  "}"
  "input.error, input.error:focus{"
  "border-color:#ED5564;"
  "color:#D9434E;"
  "box-shadow:0 0 3px #D9434E;"
  "}"
  "input:disabled{"
  "opacity:0.6;"
  "background-color:#f7f7f7;"
  "}"
  "input:disabled:hover{"
  "cursor:not-allowed;"
  "}"
  "input.error::-webkit-input-placeholder{"
  "color:#D9434E;"
  "}"
  "input.error:-moz-placeholder{"
  "color:#D9434E;"
  "}"
  "input.error::-moz-placeholder{"
  "color:#D9434E;"
  "}"
  "input.error:-ms-input-placeholder{"
  "color:#D9434E;"
  "}"
};

/**< TABLE style */
const char AutoConnect::_CSS_TABLE[] PROGMEM = {
  "table{"
  "border-collapse:collapse;"
  "border-spacing:0;"
  "border:1px solid #ddd;"
  "color:#444;"
  "background-color:#fff;"
  "margin-bottom:20px;"
  "}"
  "table.info,"
  "table.info>tfoot,"
  "table.info>thead{"
  "width:100%;"
  "border-color:#5C9DED;"
  "}"
  "table.info>thead{"
  "background-color:#5C9DED;"
  "}"
  "table.info>thead>tr>th{"
  "color:#fff;"
  "}"
  "td,"
  "th{"
  "padding:10px 22px;"
  "}"
  "thead{"
  "background-color:#f3f3f3;"
  "border-bottom:1px solid #ddd;"
  "}"
  "thead>tr>th{"
  "font-weight:400;"
  "text-align:left;"
  "}"
  "tfoot{"
  "border-top:1px solid #ddd;"
  "}"
  "tbody,"
  "tbody>tr:nth-child(odd){"
  "background-color:#fff;"
  "}"
  "tbody>tr>td,"
  "tfoot>tr>td{"
  "font-weight:300;"
  "font-size:.88em;"
  "}"
  "tbody>tr:nth-child(even){"
  "background-color:#f7f7f7;"
  "}"
  "table.info tbody>tr:nth-child(even){"
  "background-color:#EFF5FD;"
  "}"
};

/**< Common menu bar. This style quotes LuxBar. */
/**< balzss/luxbar is licensed under the MIT License https://github.com/balzss/luxbar */
const char AutoConnect::_CSS_LUXBAR[] PROGMEM = {
  ".luxbar-fixed{"
    "width:100%;"
    "position:fixed;"
    "top:0;"
    "left:0;"
    "z-index:1000;"
    "box-shadow:0 1px 3px rgba(0, 0, 0, 0.12), 0 1px 2px rgba(0, 0, 0, 0.24);"
  "}"
  ".luxbar-hamburger span,"
  ".luxbar-hamburger span::before,"
  ".luxbar-hamburger span::after{"
    "display:block;"
    "height:2px;"
    "width:26px;"
    "transition:0.6s ease;"
  "}"
  ".luxbar-checkbox:checked~.luxbar-menu li .luxbar-hamburger span{"
    "background-color:transparent;"
  "}"
  ".luxbar-checkbox:checked~.luxbar-menu li .luxbar-hamburger span::before,"
  ".luxbar-checkbox:checked~.luxbar-menu li .luxbar-hamburger span::after{"
    "margin-top:0;"
  "}"
  ".luxbar-header{"
    "display:flex;"
    "flex-direction:row;"
    "justify-content:space-between;"
    "align-items:center;"
    "height:58px;"
  "}"
  ".luxbar-menu-right .luxbar-hamburger{"
    "margin-left:auto;"
  "}"
  ".luxbar-brand{"
    "font-size:1.6em;"
    "padding:18px 24px 18px 24px;"
  "}"
  ".luxbar-menu{"
    "min-height:58px;"
    "transition:0.6s ease;"
    "width:100%;"
  "}"
  ".luxbar-navigation{"
    "display:flex;"
    "flex-direction:column;"
    "list-style:none;"
    "padding-left:0;"
    "margin:0;"
  "}"
  ".luxbar-menu a,"
  ".luxbar-item a{"
    "text-decoration:none;"
    "color:inherit;"
    "cursor:pointer;"
  "}"
  ".luxbar-item{"
    "height:58px;"
  "}"
  ".luxbar-item a{"
    "padding:18px 24px 18px 24px;"
    "display:block;"
  "}"
  ".luxbar-hamburger{"
    "padding:18px 24px 18px 24px;"
    "position:relative;"
    "cursor:pointer;"
  "}"
  ".luxbar-hamburger span::before,"
  ".luxbar-hamburger span::after{"
    "content:'';"
    "position:absolute;"
  "}"
  ".luxbar-hamburger span::before{"
    "margin-top:-8px;"
  "}"
  ".luxbar-hamburger span::after{"
    "margin-top:8px;"
  "}"
  ".luxbar-checkbox{"
    "display:none;"
  "}"
  ".luxbar-checkbox:not(:checked)~.luxbar-menu{"
    "overflow:hidden;"
    "height:58px;"
  "}"
  ".luxbar-checkbox:checked~.luxbar-menu{"
    "transition:height 0.6s ease;"
    "height:100vh;"
    "overflow:auto;"
  "}"
  ".dropdown{"
    "position:relative;"
    "height:auto;"
    "min-height:58px;"
  "}"
  ".dropdown:hover>ul{"
    "position:relative;"
    "display:block;"
    "min-width:100%;"
  "}"
  ".dropdown>a::after{"
    "position:absolute;"
    "content:'';"
    "right:10px;"
    "top:25px;"
    "border-width:5px 5px 0;"
    "border-color:transparent;"
    "border-style:solid;"
  "}"
  ".dropdown>ul{"
    "display:block;"
    "overflow-x:hidden;"
    "list-style:none;"
    "padding:0;"
  "}"
  ".dropdown>ul .luxbar-item{"
    "min-width:100%;"
    "height:29px;"
    "padding:5px 10px 5px 40px;"
  "}"
  ".dropdown>ul .luxbar-item a{"
    "min-height:29px;"
    "line-height:29px;"
    "padding:0;"
  "}"
  "@media screen and (min-width:768px){"
    ".luxbar-navigation{"
      "flex-flow:row;"
      "justify-content:flex-end;"
    "}"
    ".luxbar-hamburger{"
      "display:none;"
    "}"
    ".luxbar-checkbox:not(:checked)~.luxbar-menu{"
      "overflow:visible;"
    "}"
    ".luxbar-checkbox:checked~.luxbar-menu{"
      "height:58px;"
    "}"
    ".luxbar-menu .luxbar-item{"
      "border-top:0;"
    "}"
    ".luxbar-menu-right .luxbar-header{"
      "margin-right:auto;"
    "}"
    ".dropdown{"
      "height:58px;"
    "}"
    ".dropdown:hover>ul{"
      "position:absolute;"
      "left:0;"
      "top:58px;"
      "padding:0;"
    "}"
    ".dropdown>ul{"
      "display:none;"
    "}"
    ".dropdown>ul .luxbar-item{"
      "padding:5px 10px;"
    "}"
    ".dropdown>ul .luxbar-item a{"
      "white-space:nowrap;"
    "}"
  "}"
  ".luxbar-checkbox:checked+.luxbar-menu .luxbar-hamburger-doublespin span::before{"
    "transform:rotate(225deg);"
  "}"
  ".luxbar-checkbox:checked+.luxbar-menu .luxbar-hamburger-doublespin span::after{"
    "transform:rotate(-225deg);"
  "}"
  ".luxbar-menu-material-bluegrey,"
  ".luxbar-menu-material-bluegrey .dropdown ul{"
    "background-color:#263238;"
    "color:#fff;"
  "}"
  ".luxbar-menu-material-bluegrey .active,"
  ".luxbar-menu-material-bluegrey .luxbar-item:hover{"
    "background-color:#37474f;"
  "}"
  ".luxbar-menu-material-bluegrey .luxbar-hamburger span,"
  ".luxbar-menu-material-bluegrey .luxbar-hamburger span::before,"
  ".luxbar-menu-material-bluegrey .luxbar-hamburger span::after{"
    "background-color:#fff;"
  "}"
};

/**< Common html document header. */
const char AutoConnect::_ELM_HTML_HEAD[] PROGMEM = {
  "<!DOCTYPE html>"
  "<html>"
  "<head>"
  "<meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
};

/**< LuxBar menu element. */
const char  AutoConnect::_ELM_MENU[]  PROGMEM = {
  "<header id=\"luxbar\" class=\"luxbar-fixed\">"
  "<input type=\"checkbox\" class=\"luxbar-checkbox\" id=\"luxbar-checkbox\"/>"
  "<div class=\"luxbar-menu luxbar-menu-right luxbar-menu-material-bluegrey\">"
  "<ul class=\"luxbar-navigation\">"
  "<li class=\"luxbar-header\">"
  "<a href=\"" AUTOCONNECT_URI "\" class=\"luxbar-brand\">MENU_TITLE</a>"
  "<label class=\"luxbar-hamburger luxbar-hamburger-doublespin\" id=\"luxbar-hamburger\" for=\"luxbar-checkbox\"><span></span></label>"
  "</li>"
  "<li class=\"luxbar-item\"><a href=\"" AUTOCONNECT_URI_CONFIG "\">Configure new AP</a></li>"
  "<li class=\"luxbar-item\"><a href=\"" AUTOCONNECT_URI_OPEN "\">Open SSIDs</a></li>"
  "<li class=\"luxbar-item\"><a href=\"" AUTOCONNECT_URI_DISCON "\">Disconnect</a></li>"
  "<li class=\"luxbar-item\" id=\"reset\"><a href=\"#rdlg\">Reset...</a></li>"
  "<li class=\"luxbar-item\"><a href=\"HOME_URI\">HOME</a></li>"
  "</ul>"
  "</div>"
  "<div class=\"lap\" id=\"rdlg\"><a href=\"#reset\" class=\"overlap\"></a>"
  "<div class=\"modal_button\"><h2><a href=\"" AUTOCONNECT_URI_RESET "\" class=\"modal_button\">RESET</a></h2></div>"
  "</div>"
  "</header>"
};

/**< The 404 page content. */
const char  AutoConnect::_PAGE_404[] PROGMEM = {
  "{{HEAD}}"
  "<title>Page not found</title>"
  "</head>"
  "<body>"
  "404 Not found"
  "</body>"
  "</html>"
};

/**< The page that started the reset. */
const char  AutoConnect::_PAGE_RESETTING[] PROGMEM = {
  "{{HEAD}}"
  "<meta http-equiv=\"refresh\" content=\"{{UPTIME}};URL=" AUTOCONNECT_URI "\">"
  "<title>AutoConnect resetting</title>"
  "</head>"
  "<body>"
  "<h2>{{RESET}}</h2>"
  "</body>"
  "</html>"
};

/**< AutoConnect portal page. */
const char  AutoConnect::_PAGE_STAT[] PROGMEM = {
  "{{HEAD}}"
  "<title>AutoConnect statistics</title>"
  "<style type=\"text/css\">"
  "{{CSS_BASE}}"
  "{{CSS_TABLE}}"
  "{{CSS_LUXBAR}}"
  "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
  "<div class=\"container\">"
  "{{MENU}}"
  "<div>"
  "<table class=\"info\" style=\"border:none;\">"
  "<tbody>"
  "<tr>"
  "<td>Established connection</td>"
  "<td>{{ESTAB_SSID}}</td>"
  "</tr>"
  "<tr>"
  "<td>Mode</td>"
  "<td>{{WIFI_MODE}}({{WIFI_STATUS}})</td>"
  "</tr>"
  "<tr>"
  "<td>IP</td>"
  "<td>{{LOCAL_IP}}</td>"
  "</tr>"
  "<tr>"
  "<td>GW</td>"
  "<td>{{GATEWAY}}</td>"
  "</tr>"
  "<tr>"
  "<td>Subnet mask</td>"
  "<td>{{NETMASK}}</td>"
  "</tr>"
  "<tr>"
  "<td>SoftAP IP</td>"
  "<td>{{SOFTAP_IP}}</td>"
  "</tr>"
  "<tr>"
  "<td>AP MAC</td>"
  "<td>{{AP_MAC}}</td>"
  "</tr>"
  "<tr>"
  "<td>STA MAC</td>"
  "<td>{{STA_MAC}}</td>"
  "</tr>"
  "<tr>"
  "<td>Channel</td>"
  "<td>{{CHANNEL}}</td>"
  "</tr>"
  "<tr>"
  "<td>dBm</td>"
  "<td>{{DBM}}</td>"
  "</tr>"
  "<tr>"
  "<td>Chip ID</td>"
  "<td>{{CHIP_ID}}</td>"
  "</tr>"
  "<tr>"
  "<td>CPU Freq.</td>"
  "<td>{{CPU_FREQ}}MHz</td>"
  "</tr>"
  "<tr>"
  "<td>Flash size</td>"
  "<td>{{FLASH_SIZE}}</td>"
  "</tr>"
  "<tr>"
  "<td>Free memory</td>"
  "<td>{{FREE_HEAP}}</td>"
  "</tr>"
  "</tbody>"
  "</table>"
  "</div>"
  "</div>"
  "</body>"
  "</html>"
};

/**< A page that specifies the new configuration. */
const char  AutoConnect::_PAGE_CONFIGNEW[] PROGMEM = {
  "{{HEAD}}"
  "<title>AutoConnect config</title>"
  "<style type=\"text/css\">"
  "{{CSS_BASE}}"
  "{{CSS_ICON_LOCK}}"
  "{{CSS_UL}}"
  "{{CSS_INPUT_BUTTON}}"
  "{{CSS_INPUT_TEXT}}"
  "{{CSS_LUXBAR}}"
  "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
  "<div class=\"container\">"
  "{{MENU}}"
  "<div class=\"base-panel\">"
  "<form action=\"" AUTOCONNECT_URI_CONNECT "\" method=\"post\">"
  "{{LIST_SSID}}"
  "<div style=\"margin:16px 0 8px 0;border-bottom:solid 1px #263238;\">Hidden:{{HIDDEN_COUNT}}</div>"
  "<ul class=\"noorder\">"
  "<li>"
  "<label for=\"ssid\">SSID</label>"
  "<input id=\"ssid\" type=\"text\" name=\"" AUTOCONNECT_PARAMID_SSID "\" placeholder=\"SSID\">"
  "</li>"
  "<li>"
  "<label for=\"passphrase\">Passphrase</label>"
  "<input id=\"passphrase\" type=\"password\" name=\"" AUTOCONNECT_PARAMID_PASS "\" placeholder=\"Passphrase\">"
  "</li>"
  "<br><li><input type=\"submit\" value=\"apply\"></li>"
  "</ul>"
  "</form>"
  "</div>"
  "</div>"
  "</body>"
  "</html>"
};

/**< A page that reads stored authentication information and starts connection. */
const char  AutoConnect::_PAGE_OPENCREDT[] PROGMEM = {
  "{{HEAD}}"
  "<title>AutoConnect credentials</title>"
  "<style type=\"text/css\">"
  "{{CSS_BASE}}"
  "{{CSS_INPUT_BUTTON}}"
  "{{CSS_LUXBAR}}"
  "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
  "<div class=\"container\">"
  "{{MENU}}"
  "<div class=\"base-panel\">"
  "<form action=\"" AUTOCONNECT_URI_CONNECT "\" method=\"post\">"
  "{{OPEN_SSID}}"
  "</form>"
  "</div>"
  "</div>"
  "</body>"
  "</html>"
};

/**< A page announcing that a connection has been established. */
const char  AutoConnect::_PAGE_SUCCESS[] PROGMEM = {
  "{{HEAD}}"
  "<title>AutoConnect statistics</title>"
  "<style type=\"text/css\">"
  "{{CSS_BASE}}"
  "{{CSS_TABLE}}"
  "{{CSS_LUXBAR}}"
  "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
  "<div class=\"container\">"
  "{{MENU}}"
  "<div>"
  "<table class=\"info\" style=\"border:none;\">"
  "<tbody>"
  "<tr>"
  "<td>Established connection</td>"
  "<td>{{ESTAB_SSID}}</td>"
  "</tr>"
  "<tr>"
  "<td>Mode</td>"
  "<td>{{WIFI_MODE}}({{WIFI_STATUS}})</td>"
  "</tr>"
  "<tr>"
  "<td>IP</td>"
  "<td>{{LOCAL_IP}}</td>"
  "</tr>"
  "<td>GW</td>"
  "<td>{{GATEWAY}}</td>"
  "</tr>"
  "<tr>"
  "<td>Subnet mask</td>"
  "<td>{{NETMASK}}</td>"
  "</tr>"
  "<tr>"
  "<td>Channel</td>"
  "<td>{{CHANNEL}}</td>"
  "</tr>"
  "<tr>"
  "<td>dBm</td>"
  "<td>{{DBM}}</td>"
  "</tr>"
  "</tbody>"
  "</table>"
  "</div>"
  "</div>"
  "</body>"
  "</html>"
};
 /**< A response page for connection failed. */
const char  AutoConnect::_PAGE_FAIL[] PROGMEM = {
  "{{HEAD}}"
  "<title>AutoConnect statistics</title>"
  "<style type=\"text/css\">"
  "{{CSS_BASE}}"
  "{{CSS_TABLE}}"
  "{{CSS_LUXBAR}}"
  "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
  "<div class=\"container\">"
  "{{MENU}}"
  "<div>"
  "<table class=\"info\" style=\"border:none;\">"
  "<tbody>"
  "<tr>"
  "<td>Connection Failed</td>"
  "<td>{{STATION_STATUS}}</td>"
  "</tr>"
  "</tbody>"
  "</table>"
  "</div>"
  "</div>"
  "</body>"
  "</html>"
};

/**< A response page for disconnected from the AP. */
const char  AutoConnect::_PAGE_DISCONN[] PROGMEM = {
  "{{DISCONNECT}}"
  "{{HEAD}}"
  "<title>AutoConnect disconnected</title>"
  "<style type=\"text/css\">"
  "{{CSS_BASE}}"
  "{{CSS_LUXBAR}}"
  "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
  "<div class=\"container\">"
  "{{MENU}}"
  "</body>"
  "</html>"
};

uint32_t AutoConnect::_getChipId() {
#if defined(ARDUINO_ARCH_ESP8266)
  return ESP.getChipId();
#elif defined(ARDUINO_ARCH_ESP32)
  uint64_t  chipId;
  chipId = ESP.getEfuseMac();
  return (uint32_t)(chipId >> 32);
#endif
}

uint32_t AutoConnect::_getFlashChipRealSize() {
#if defined(ARDUINO_ARCH_ESP8266)
  return ESP.getFlashChipRealSize();
#elif defined(ARDUINO_ARCH_ESP32)
  return (uint32_t)spi_flash_get_chip_size();
#endif
}

String AutoConnect::_token_CSS_BASE(PageArgument& args) {
  return String(_CSS_BASE);
}

String AutoConnect::_token_CSS_UL(PageArgument& args) {
  return String(_CSS_UL);
}

String AutoConnect::_token_CSS_ICON_LOCK(PageArgument& args) {
  return String(_CSS_ICON_LOCK);
}
String AutoConnect::_token_CSS_INPUT_BUTTON(PageArgument& args) {
  return String(_CSS_INPUT_BUTTON);
}

String AutoConnect::_token_CSS_INPUT_TEXT(PageArgument& args) {
  return String(_CSS_INPUT_TEXT);
}

String AutoConnect::_token_CSS_TABLE(PageArgument& args) {
  return String(_CSS_TABLE);
}

String AutoConnect::_token_HEAD(PageArgument& args) {
  return String(_ELM_HTML_HEAD);
}

String AutoConnect::_token_MENU(PageArgument& args) {
  String  currentMenu = String(_ELM_MENU);
  currentMenu.replace(String("MENU_TITLE"), _menuTitle);
  currentMenu.replace(String("HOME_URI"), _apConfig.homeUri);
  return currentMenu;
}

String AutoConnect::_token_CSS_LUXBAR(PageArgument& args) {
  return String(_CSS_LUXBAR);
}

String AutoConnect::_token_ESTAB_SSID(PageArgument& args) {
  return (WiFi.status() == WL_CONNECTED ? WiFi.SSID() : String("N/A"));
}

String AutoConnect::_token_WIFI_MODE(PageArgument& args) {
  const char* wifiMode = "";
  switch (WiFi.getMode()) {
  case WIFI_OFF:
    wifiMode = "OFF";
    break;
  case WIFI_STA:
    wifiMode = "STA";
    break;
  case WIFI_AP:
    wifiMode = "AP";
    break;
  case WIFI_AP_STA:
    wifiMode = "AP_STA";
    break;
#ifdef ARDUINO_ARCH_ESP32
  case WIFI_MODE_MAX:
    wifiMode = "MAX";
    break;
#endif
  }
  return String(wifiMode);
}

String AutoConnect::_token_WIFI_STATUS(PageArgument& args) {
  return String(WiFi.status());
}

String AutoConnect::_token_STATION_STATUS(PageArgument& args) {
  uint8_t     st;
  const char* wlStatusSymbol;

#if defined(ARDUINO_ARCH_ESP8266)
  static const char *wlStatusSymbols[] = {
    "IDLE",
    "CONNECTING",
    "WRONG_PASSWORD",
    "NO_AP_FOUND",
    "CONNECT_FAIL",
    "GOT_IP"
  };
  st = wifi_station_get_connect_status();
  switch (st) {
  case STATION_IDLE:
    wlStatusSymbol = wlStatusSymbols[0];
    break;
  case STATION_CONNECTING:
    wlStatusSymbol = wlStatusSymbols[1];
    break;
  case STATION_WRONG_PASSWORD:
    wlStatusSymbol = wlStatusSymbols[2];
    break;
  case STATION_NO_AP_FOUND:
    wlStatusSymbol = wlStatusSymbols[3];
    break;
  case STATION_CONNECT_FAIL:
    wlStatusSymbol = wlStatusSymbols[4];
    break;
  case STATION_GOT_IP:
    wlStatusSymbol = wlStatusSymbols[5];
    break;
  }

#elif defined(ARDUINO_ARCH_ESP32)
  static const char *wlStatusSymbols[] = {
    "IDLE",
    "NO_SSID_AVAIL",
    "SCAN_COMPLETED",
    "CONNECTED",
    "CONNECT_FAILED",
    "CONNECTION_LOST",
    "DISCONNECTED"
  };
  st = WiFi.status();
  switch (st) {
  case WL_IDLE_STATUS:
    wlStatusSymbol = wlStatusSymbols[0];
    break;
  case WL_NO_SSID_AVAIL:
    wlStatusSymbol = wlStatusSymbols[1];
    break;
  case WL_SCAN_COMPLETED:
    wlStatusSymbol = wlStatusSymbols[2];
    break;
  case WL_CONNECTED:
    wlStatusSymbol = wlStatusSymbols[3];
    break;
  case WL_CONNECT_FAILED:
    wlStatusSymbol = wlStatusSymbols[4];
    break;
  case WL_CONNECTION_LOST:
    wlStatusSymbol = wlStatusSymbols[5];
    break;
  case WL_DISCONNECTED:
    wlStatusSymbol = wlStatusSymbols[6];
    break;
  }
#endif

  return "(" + String(st) + ") " + String(wlStatusSymbol);
}

String AutoConnect::_token_LOCAL_IP(PageArgument& args) {
  return WiFi.localIP().toString();
}

String AutoConnect::_token_SOFTAP_IP(PageArgument& args) {
  return WiFi.softAPIP().toString();
}

String AutoConnect::_token_GATEWAY(PageArgument& args) {
  return WiFi.gatewayIP().toString();
}

String AutoConnect::_token_NETMASK(PageArgument& args) {
  return WiFi.subnetMask().toString();
}

String AutoConnect::_token_AP_MAC(PageArgument& args) {
  uint8_t macAddress[6];
  WiFi.softAPmacAddress(macAddress);
  return AutoConnect::_toMACAddressString(macAddress);
}

String AutoConnect::_token_STA_MAC(PageArgument& args) {
  uint8_t macAddress[6];
  WiFi.macAddress(macAddress);
  return AutoConnect::_toMACAddressString(macAddress);
}

String AutoConnect::_token_CHANNEL(PageArgument& args) {
  return String(WiFi.channel());
}

String AutoConnect::_token_DBM(PageArgument& args) {
  int32_t dBm = WiFi.RSSI();
  return (dBm == 31 ? String("N/A") : String(dBm));
}

String AutoConnect::_token_CPU_FREQ(PageArgument& args) {
  return String(ESP.getCpuFreqMHz());
}

String AutoConnect::_token_FLASH_SIZE(PageArgument& args) {
  return String(_getFlashChipRealSize());
}

String AutoConnect::_token_CHIP_ID(PageArgument& args) {
  return String(_getChipId());
}

String AutoConnect::_token_FREE_HEAP(PageArgument& args) {
  return String(ESP.getFreeHeap());
}

String AutoConnect::_token_LIST_SSID(PageArgument& args) {
  String ssidList = "";
  _hiddenSSIDCount = 0;
  int8_t nn = WiFi.scanNetworks(false, true);
  for (uint8_t i = 0; i < nn; i++) {
    String ssid = WiFi.SSID(i);
    if (ssid.length() > 0) {
      ssidList += String(F("<input type=\"button\" onClick=\"document.getElementById('ssid').value=this.getAttribute('value');document.getElementById('passphrase').focus()\" value=\"")) + ssid + String(F("\">"));
      ssidList += String(F("<label>")) + String(AutoConnect::_toWiFiQuality(WiFi.RSSI(i))) + String(F("%</label>"));
      if (WiFi.encryptionType(i) != ENC_TYPE_NONE)
        ssidList += String(F("<span class=\"img-lock\"></span>"));
      ssidList += String(F("<br>"));
    }
    else
      _hiddenSSIDCount++;
  }
  return ssidList;
}

String AutoConnect::_token_HIDDEN_COUNT(PageArgument& args) {
  return String(_hiddenSSIDCount);
}

String AutoConnect::_token_OPEN_SSID(PageArgument& args) {
  AutoConnectCredential credit(_apConfig.boundaryOffset);
  struct station_config entry;
  String ssidList = "";
  
  uint8_t* bssid = WiFi.BSSID();
  for (uint8_t i = 0; i < credit.entries(); i++) {
    credit.load(i, &entry);
    AC_DBG("A credential #%d loaded\n", (int)i);
    ssidList += String(F("<input id=\"sb\" type=\"submit\" name=\"" AUTOCONNECT_PARAMID_CRED "\" value=\"")) + String((char*)entry.ssid) + String(F("\">"));
    ssidList += String(F("<label>"));
    if (bssid != NULL && memcmp(bssid, entry.bssid, sizeof(station_config::bssid)) == 0)
      ssidList += String(AutoConnect::_toWiFiQuality(WiFi.RSSI())) + String("%");
    else
      ssidList += String("N/A");
    ssidList += String(F("</label><br>"));
  }

  if (ssidList.length() == 0) {
    ssidList = String(PSTR("<p><b>No saved credentials.</b></p>"));
  }
  return ssidList;
}

String AutoConnect::_token_UPTIME(PageArgument& args) {
  return String(_apConfig.uptime);
}

/**
 *  This function dynamically build up the response pages that conform to
 *  the requested URI. A PageBuilder instance is stored in _rensponsePage
 *  as the response page.
 *  @param  Requested URI.
 *  @retval true  A response page generated.
 *  @retval false Requested uri is not defined.
 */
PageElement* AutoConnect::_setupPage(String uri) {
  PageElement *elm = new PageElement();

  // Build the elements of current requested page.
  if (uri == String(AUTOCONNECT_URI)) {

    // Setup /auto
    elm->setMold(_PAGE_STAT);
    elm->addToken(PSTR("HEAD"), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_BASE"), std::bind(&AutoConnect::_token_CSS_BASE, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_TABLE"), std::bind(&AutoConnect::_token_CSS_TABLE, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_LUXBAR"), std::bind(&AutoConnect::_token_CSS_LUXBAR, this, std::placeholders::_1));
    elm->addToken(PSTR("MENU"), std::bind(&AutoConnect::_token_MENU, this, std::placeholders::_1));
    elm->addToken(PSTR("ESTAB_SSID"), std::bind(&AutoConnect::_token_ESTAB_SSID, this, std::placeholders::_1));
    elm->addToken(PSTR("WIFI_MODE"), std::bind(&AutoConnect::_token_WIFI_MODE, this, std::placeholders::_1));
    elm->addToken(PSTR("WIFI_STATUS"), std::bind(&AutoConnect::_token_WIFI_STATUS, this, std::placeholders::_1));
    elm->addToken(PSTR("LOCAL_IP"), std::bind(&AutoConnect::_token_LOCAL_IP, this, std::placeholders::_1));
    elm->addToken(PSTR("SOFTAP_IP"), std::bind(&AutoConnect::_token_SOFTAP_IP, this, std::placeholders::_1));
    elm->addToken(PSTR("GATEWAY"), std::bind(&AutoConnect::_token_GATEWAY, this, std::placeholders::_1));
    elm->addToken(PSTR("NETMASK"), std::bind(&AutoConnect::_token_NETMASK, this, std::placeholders::_1));
    elm->addToken(PSTR("AP_MAC"), std::bind(&AutoConnect::_token_AP_MAC, this, std::placeholders::_1));
    elm->addToken(PSTR("STA_MAC"), std::bind(&AutoConnect::_token_STA_MAC, this, std::placeholders::_1));
    elm->addToken(PSTR("CHANNEL"), std::bind(&AutoConnect::_token_CHANNEL, this, std::placeholders::_1));
    elm->addToken(PSTR("DBM"), std::bind(&AutoConnect::_token_DBM, this, std::placeholders::_1));
    elm->addToken(PSTR("CPU_FREQ"), std::bind(&AutoConnect::_token_CPU_FREQ, this, std::placeholders::_1));
    elm->addToken(PSTR("FLASH_SIZE"), std::bind(&AutoConnect::_token_FLASH_SIZE, this, std::placeholders::_1));
    elm->addToken(PSTR("CHIP_ID"), std::bind(&AutoConnect::_token_CHIP_ID, this, std::placeholders::_1));
    elm->addToken(PSTR("FREE_HEAP"), std::bind(&AutoConnect::_token_FREE_HEAP, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_CONFIG)) {

    // Setup /auto/config
    elm->setMold(_PAGE_CONFIGNEW);
    elm->addToken(PSTR("HEAD"), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_BASE"), std::bind(&AutoConnect::_token_CSS_BASE, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_UL"), std::bind(&AutoConnect::_token_CSS_UL, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_ICON_LOCK"), std::bind(&AutoConnect::_token_CSS_ICON_LOCK, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_INPUT_BUTTON"), std::bind(&AutoConnect::_token_CSS_INPUT_BUTTON, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_INPUT_TEXT"), std::bind(&AutoConnect::_token_CSS_INPUT_TEXT, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_LUXBAR"), std::bind(&AutoConnect::_token_CSS_LUXBAR, this, std::placeholders::_1));
    elm->addToken(PSTR("MENU"), std::bind(&AutoConnect::_token_MENU, this, std::placeholders::_1));
    elm->addToken(PSTR("LIST_SSID"), std::bind(&AutoConnect::_token_LIST_SSID, this, std::placeholders::_1));
    elm->addToken(PSTR("HIDDEN_COUNT"), std::bind(&AutoConnect::_token_HIDDEN_COUNT, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_CONNECT)) {

    // Setup /auto/connect
    elm->setMold("{{REQ}}");
    elm->addToken(PSTR("REQ"), std::bind(&AutoConnect::_induceConnect, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_OPEN)) {

    // Setup /auto/open
    elm->setMold(_PAGE_OPENCREDT);
    elm->addToken(PSTR("HEAD"), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_BASE"), std::bind(&AutoConnect::_token_CSS_BASE, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_INPUT_BUTTON"), std::bind(&AutoConnect::_token_CSS_INPUT_BUTTON, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_LUXBAR"), std::bind(&AutoConnect::_token_CSS_LUXBAR, this, std::placeholders::_1));
    elm->addToken(PSTR("MENU"), std::bind(&AutoConnect::_token_MENU, this, std::placeholders::_1));
    elm->addToken(PSTR("OPEN_SSID"), std::bind(&AutoConnect::_token_OPEN_SSID, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_DISCON)) {

    // Setup /auto/disc
    elm->setMold(_PAGE_DISCONN);
    elm->addToken(PSTR("DISCONNECT"), std::bind(&AutoConnect::_induceDisconnect, this, std::placeholders::_1));
    elm->addToken(PSTR("HEAD"), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_BASE"), std::bind(&AutoConnect::_token_CSS_BASE, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_LUXBAR"), std::bind(&AutoConnect::_token_CSS_LUXBAR, this, std::placeholders::_1));
    elm->addToken(PSTR("MENU"), std::bind(&AutoConnect::_token_MENU, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_RESET)) {

    // Setup /auto/reset
    elm->setMold(_PAGE_RESETTING);
    elm->addToken(PSTR("HEAD"), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(PSTR("UPTIME"), std::bind(&AutoConnect::_token_UPTIME, this, std::placeholders::_1));
    elm->addToken(PSTR("RESET"), std::bind(&AutoConnect::_induceReset, this, std::placeholders::_1));

  }
  else if (uri == String(AUTOCONNECT_URI_RESULT)) {

    // Setup /auto/result
    elm->setMold("{{RESULT}}");
    elm->addToken(PSTR("RESULT"), std::bind(&AutoConnect::_invokeResult, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_SUCCESS)) {

    // Setup /auto/success
    elm->setMold(_PAGE_SUCCESS);
    elm->addToken(PSTR("HEAD"), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_BASE"), std::bind(&AutoConnect::_token_CSS_BASE, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_TABLE"), std::bind(&AutoConnect::_token_CSS_TABLE, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_LUXBAR"), std::bind(&AutoConnect::_token_CSS_LUXBAR, this, std::placeholders::_1));
    elm->addToken(PSTR("MENU"), std::bind(&AutoConnect::_token_MENU, this, std::placeholders::_1));
    elm->addToken(PSTR("ESTAB_SSID"), std::bind(&AutoConnect::_token_ESTAB_SSID, this, std::placeholders::_1));
    elm->addToken(PSTR("WIFI_MODE"), std::bind(&AutoConnect::_token_WIFI_MODE, this, std::placeholders::_1));
    elm->addToken(PSTR("WIFI_STATUS"), std::bind(&AutoConnect::_token_WIFI_STATUS, this, std::placeholders::_1));
    elm->addToken(PSTR("LOCAL_IP"), std::bind(&AutoConnect::_token_LOCAL_IP, this, std::placeholders::_1));
    elm->addToken(PSTR("GATEWAY"), std::bind(&AutoConnect::_token_GATEWAY, this, std::placeholders::_1));
    elm->addToken(PSTR("NETMASK"), std::bind(&AutoConnect::_token_NETMASK, this, std::placeholders::_1));
    elm->addToken(PSTR("CHANNEL"), std::bind(&AutoConnect::_token_CHANNEL, this, std::placeholders::_1));
    elm->addToken(PSTR("DBM"), std::bind(&AutoConnect::_token_DBM, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_FAIL)) {

    // Setup /auto/fail
    elm->setMold(_PAGE_FAIL);
    elm->addToken(PSTR("HEAD"), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_BASE"), std::bind(&AutoConnect::_token_CSS_BASE, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_TABLE"), std::bind(&AutoConnect::_token_CSS_TABLE, this, std::placeholders::_1));
    elm->addToken(PSTR("CSS_LUXBAR"), std::bind(&AutoConnect::_token_CSS_LUXBAR, this, std::placeholders::_1));
    elm->addToken(PSTR("MENU"), std::bind(&AutoConnect::_token_MENU, this, std::placeholders::_1));
    elm->addToken(PSTR("STATION_STATUS"), std::bind(&AutoConnect::_token_STATION_STATUS, this, std::placeholders::_1));
  }
  else {
    delete elm;
    elm = nullptr;
  }

  return elm;
}
