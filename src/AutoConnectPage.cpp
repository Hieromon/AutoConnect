/**
 *  AutoConnect portal site web page implementation.
 *  @file   AutoConnectPage.h
 *  @author hieromon@gmail.com
 *  @version    0.9.9
 *  @date   2019-05-25
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
    "-webkit-font-smoothing:antialiased"
  "}"
  "body{"
    "margin:0;"
    "padding:0"
  "}"
  ".base-panel{"
    "margin:0 22px 0 22px"
  "}"
  ".base-panel>*>label{"
    "display:inline-block;"
    "width:3.0em;"
    "text-align:right"
  "}"
  ".base-panel>*>label.slist{"
    "width:auto;"
    "font-size:0.9em;"
    "margin-left:10px;"
    "text-align:left"
  "}"
  "input{"
    "-moz-appearance:none;"
    "-webkit-appearance:none;"
    "font-size:0.9em;"
    "margin:8px 0 auto"
  "}"
  ".lap{"
    "visibility:collapse"
  "}"
  ".lap:target{"
    "visibility:visible"
  "}"
  ".lap:target .overlap{"
    "opacity:0.7;"
    "transition:0.3s"
  "}"
  ".lap:target .modal_button{"
    "opacity:1;"
    "transition:0.3s"
  "}"
  ".overlap{"
    "top:0;"
    "left:0;"
    "width:100%;"
    "height:100%;"
    "position:fixed;"
    "opacity:0;"
    "background:#000;"
    "z-index:1000"
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
    "z-index:1001"
  "}"
};

/**< non-marked list for UL */
const char AutoConnect::_CSS_UL[] PROGMEM = {
  "ul.noorder{"
    "padding:0;"
    "list-style:none;"
    "display:table"
  "}"
  "ul.noorder li{"
    "display:table-row"
  "}"
  "ul.noorder>*>label{"
    "display:table-cell;"
    "width:auto;"
    "margin-right:10px;"
    "text-align:right;"
    "padding:10px 0.5em"
  "}"
  "ul.noorder>input[type=\"checkbox\"]{"
    "-moz-appearance:checkbox;"
    "-webkit-appearance:checkbox"
  "}"
  "ul.noorder>input[type=\"radio\"]{"
    "margin-right:0.5em;"
    "-moz-appearance:radio;"
    "-webkit-appearance:radio"
  "}"
  "ul.noorder>input[type=\"text\"]{"
    "width:auto"
  "}"
  "ul.noorder>input[type=\"text\"]:invalid{"
    "background:#fce4d6"
  "}"
};

/**< Image icon for inline expansion, the lock mark. */
const char AutoConnect::_CSS_ICON_LOCK[] PROGMEM = {
  ".img-lock{"
    "display:inline-block;"
    "width:22px;"
    "height:22px;"
    "margin-top:14px;"
    "float:right;"
    "background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAACXBIWXMAAAsTAAALEwEAmpwYAAAB1ElEQVRIibWVu0scURTGf3d2drBQFAWbbRQVCwuVLIZdi2gnWIiF/4GtKyuJGAJh8mgTcU0T8T8ICC6kiIVu44gvtFEQQWwsbExQJGHXmZtiZsOyzCN3Vz+4cDjfvec7j7l3QAF95onRZ54YKmdE1IbnS0c9mnAyAjkBxDy3LRHrjtRyu7OD52HntTAyvbw/HxP2hkCearrRb2WSCSuTTGi60S+QpzFhbwznDl/VVMHw0sF7hEjFbW2qkB38lfp8nNDipWcATil+uDM3cDWyeNRSijnfkHJnezb5Vkkgvbg3IOXD2e1ts93S+icnkZOAVaalZK3YQMa4L+pC6L1WduhYSeCf0PLBdxzOjZ93Lwvm6APAiLmlF1ubPiHotmaS41ExQjH0ZbfNM1NAFpgD0lVcICIrANqAVaAd+AFIYAy4BqaBG+Wsq5AH3vgk8xpYrzf4KLAZwhe8PYEIvQe4vc6H8Hnc2dQs0AFchvAXQGdEDF8s4A5TZS34BQqqQNaS1WMI3KD4WUbNoBJfce9CO7BSr4BfBe8A21vmUwh0VdjdTyHwscL+UK+AHxoD7FDoAX6/Cnpxn4ay/egCjcCL/w1chkqLakLQ/6ABhT57uAd+Vzv/Ara3iY6fK4WxAAAAAElFTkSuQmCC) no-repeat"
  "}"
};

/**< INPUT button and submit style */
const char AutoConnect::_CSS_INPUT_BUTTON[] PROGMEM = {
  "input[type=\"button\"],input[type=\"submit\"],button[type=\"submit\"],button[type=\"button\"]{"
    "padding:8px 0.5em;"
    "font-weight:bold;"
    "letter-spacing:0.8px;"
    "color:#fff;"
    "border:1px solid;"
    "border-radius:2px;"
    "margin-top:12px"
  "}"
  "input[type=\"button\"],button[type=\"button\"]{"
    "background-color:#1b5e20;"
    "border-color:#1b5e20;"
    "width:15em"
  "}"
  ".aux-page input[type=\"button\"],button[type=\"button\"]{"
    "font-weight:normal;"
    "padding:8px 14px;"
    "margin:12px;"
    "width:auto"
  "}"
  "input#sb[type=\"submit\"]{"
    "width:15em"
  "}"
  "input[type=\"submit\"],button[type=\"submit\"]{"
    "padding:8px 30px;"
    "background-color:#006064;"
    "border-color:#006064"
  "}"
  "input[type=\"button\"],input[type=\"submit\"],button[type=\"submit\"]:focus,"
  "input[type=\"button\"],input[type=\"submit\"],button[type=\"submit\"]:active{"
    "outline:none;"
    "text-decoration:none"
  "}"
};

/**< INPUT text style */
const char AutoConnect::_CSS_INPUT_TEXT[] PROGMEM = {
  "input[type=\"text\"],input[type=\"password\"], .aux-page select{"
    "background-color:#fff;"
    "border:1px solid #ccc;"
    "border-radius:2px;"
    "color:#444;"
    "margin:8px 0 8px 0;"
    "padding:10px"
  "}"
  "input[type=\"text\"],input[type=\"password\"]{"
    "font-weight:300;"
    "width:auto;"
    "-webkit-transition:all 0.20s ease-in;"
    "-moz-transition:all 0.20s ease-in;"
    "-o-transition:all 0.20s ease-in;"
    "-ms-transition:all 0.20s ease-in;"
    "transition:all 0.20s ease-in"
  "}"
  "input[type=\"text\"]:focus,input[type=\"password\"]:focus{"
    "outline:none;"
    "border-color:#5C9DED;"
    "box-shadow:0 0 3px #4B8CDC"
  "}"
  "input.error, input.error:focus{"
    "border-color:#ED5564;"
    "color:#D9434E;"
    "box-shadow:0 0 3px #D9434E"
  "}"
  "input:disabled{"
    "opacity:0.6;"
    "background-color:#f7f7f7"
  "}"
  "input:disabled:hover{"
    "cursor:not-allowed"
  "}"
    "input.error::-webkit-input-placeholder{"
    "color:#D9434E"
  "}"
  "input.error:-moz-placeholder{"
    "color:#D9434E"
  "}"
  "input.error::-moz-placeholder{"
    "color:#D9434E"
  "}"
  "input.error:-ms-input-placeholder{"
    "color:#D9434E"
  "}"
  ".aux-page label{"
    "padding:10px 0.5em"
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
    "margin-bottom:20px"
  "}"
  "table.info,"
  "table.info>tfoot,"
  "table.info>thead{"
    "width:100%;"
    "border-color:#5C9DED"
  "}"
  "table.info>thead{"
    "background-color:#5C9DED"
  "}"
  "table.info>thead>tr>th{"
    "color:#fff"
  "}"
  "td,"
  "th{"
    "padding:10px 22px"
  "}"
  "thead{"
    "background-color:#f3f3f3;"
    "border-bottom:1px solid #ddd"
  "}"
  "thead>tr>th{"
    "font-weight:400;"
    "text-align:left"
  "}"
  "tfoot{"
    "border-top:1px solid #ddd"
  "}"
  "tbody,"
  "tbody>tr:nth-child(odd){"
    "background-color:#fff"
  "}"
  "tbody>tr>td,"
  "tfoot>tr>td{"
    "font-weight:300;"
    "font-size:.88em"
  "}"
  "tbody>tr:nth-child(even){"
    "background-color:#f7f7f7"
  "}"
    "table.info tbody>tr:nth-child(even){"
    "background-color:#EFF5FD"
  "}"
};

/**< SVG animation for spinner */
const char AutoConnect::_CSS_SPINNER[] PROGMEM = {
  ".spinner{"
    "width:40px;"
    "height:40px;"
    "position:relative;"
    "margin:100px auto"
  "}"
  ".double-bounce1, .double-bounce2{"
    "width:100%;"
    "height:100%;"
    "border-radius:50%;"
    "background-color:#a3cccc;"
    "opacity:0.6;"
    "position:absolute;"
    "top:0;"
    "left:0;"
    "-webkit-animation:sk-bounce 2.0s infinite ease-in-out;"
    "animation:sk-bounce 2.0s infinite ease-in-out"
  "}"
  ".double-bounce2{"
    "-webkit-animation-delay:-1.0s;"
    "animation-delay:-1.0s"
  "}"
  "@-webkit-keyframes sk-bounce{"
    "0%, 100%{-webkit-transform:scale(0.0)}"
    "50%{-webkit-transform:scale(1.0)}"
  "}"
  "@keyframes sk-bounce{"
    "0%,100%{"
      "transform:scale(0.0);"
      "-webkit-transform:scale(0.0);"
    "}50%{"
      "transform:scale(1.0);"
      "-webkit-transform:scale(1.0);"
    "}"
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
    "box-shadow:0 1px 3px rgba(0, 0, 0, 0.12), 0 1px 2px rgba(0, 0, 0, 0.24)"
  "}"
  ".luxbar-burger span,"
  ".luxbar-burger span::before,"
  ".luxbar-burger span::after{"
    "display:block;"
    "height:2px;"
    "width:26px;"
    "transition:0.6s ease"
  "}"
  ".luxbar-checkbox:checked~.luxbar-menu li .luxbar-burger span{"
    "background-color:transparent"
  "}"
  ".luxbar-checkbox:checked~.luxbar-menu li .luxbar-burger span::before,"
  ".luxbar-checkbox:checked~.luxbar-menu li .luxbar-burger span::after{"
    "margin-top:0"
  "}"
  ".luxbar-header{"
    "display:flex;"
    "flex-direction:row;"
    "justify-content:space-between;"
    "align-items:center;"
    "height:58px"
  "}"
  ".luxbar-menu-right .luxbar-burger{"
    "margin-left:auto"
  "}"
  ".luxbar-brand{"
    "font-size:1.6em;"
    "padding:18px 24px 18px 24px"
  "}"
  ".luxbar-menu{"
    "min-height:58px;"
    "transition:0.6s ease;"
    "width:100%"
  "}"
  ".luxbar-navigation{"
    "display:flex;"
    "flex-direction:column;"
    "list-style:none;"
    "padding-left:0;"
    "margin:0"
  "}"
  ".luxbar-menu a,"
  ".luxbar-item a{"
    "text-decoration:none;"
    "color:inherit;"
    "cursor:pointer"
  "}"
  ".luxbar-item{"
    "height:58px"
  "}"
  ".luxbar-item a{"
    "padding:18px 24px 18px 24px;"
    "display:block"
  "}"
  ".luxbar-burger{"
    "padding:18px 24px 18px 24px;"
    "position:relative;"
    "cursor:pointer"
  "}"
  ".luxbar-burger span::before,"
  ".luxbar-burger span::after{"
    "content:'';"
    "position:absolute"
  "}"
  ".luxbar-burger span::before{"
    "margin-top:-8px"
  "}"
  ".luxbar-burger span::after{"
    "margin-top:8px"
  "}"
  ".luxbar-checkbox{"
    "display:none"
  "}"
  ".luxbar-checkbox:not(:checked)~.luxbar-menu{"
    "overflow:hidden;"
    "height:58px"
  "}"
  ".luxbar-checkbox:checked~.luxbar-menu{"
    "transition:height 0.6s ease;"
    "height:100vh;"
    "overflow:auto"
  "}"
  ".dropdown{"
    "position:relative;"
    "height:auto;"
    "min-height:58px"
  "}"
  ".dropdown:hover>ul{"
    "position:relative;"
    "display:block;"
    "min-width:100%"
  "}"
  ".dropdown>a::after{"
    "position:absolute;"
    "content:'';"
    "right:10px;"
    "top:25px;"
    "border-width:5px 5px 0;"
    "border-color:transparent;"
    "border-style:solid"
  "}"
  ".dropdown>ul{"
    "display:block;"
    "overflow-x:hidden;"
    "list-style:none;"
    "padding:0"
  "}"
  ".dropdown>ul .luxbar-item{"
    "min-width:100%;"
    "height:29px;"
    "padding:5px 10px 5px 40px"
  "}"
  ".dropdown>ul .luxbar-item a{"
    "min-height:29px;"
    "line-height:29px;"
    "padding:0"
  "}"
  "@media screen and (min-width:768px){"
    ".luxbar-navigation{"
      "flex-flow:row;"
      "justify-content:flex-end;"
    "}"
    ".luxbar-burger{"
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
  ".luxbar-checkbox:checked+.luxbar-menu .luxbar-burger-doublespin span::before{"
    "transform:rotate(225deg)"
  "}"
  ".luxbar-checkbox:checked+.luxbar-menu .luxbar-burger-doublespin span::after{"
    "transform:rotate(-225deg)"
  "}"
  ".luxbar-menu-material,"
  ".luxbar-menu-material .dropdown ul{"
    "background-color:" AUTOCONNECT_MENUCOLOR_BACKGROUND ";"
    "color:" AUTOCONNECT_MENUCOLOR_TEXT
  "}"
  ".luxbar-menu-material .active,"
  ".luxbar-menu-material .luxbar-item:hover{"
    "background-color:" AUTOCONNECT_MENUCOLOR_ACTIVE
  "}"
  ".luxbar-menu-material .luxbar-burger span,"
  ".luxbar-menu-material .luxbar-burger span::before,"
  ".luxbar-menu-material .luxbar-burger span::after{"
    "background-color:" AUTOCONNECT_MENUCOLOR_TEXT
  "}"
};

/**< Common html document header. */
const char AutoConnect::_ELM_HTML_HEAD[] PROGMEM = {
  "<!DOCTYPE html>"
  "<html>"
  "<head>"
  "<meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
};

/**< LuxBar menu element. */
const char  AutoConnect::_ELM_MENU_PRE[] PROGMEM = {
  "<header id=\"luxbar\" class=\"luxbar-fixed\">"
    "<input type=\"checkbox\" class=\"luxbar-checkbox\" id=\"luxbar-checkbox\"/>"
    "<div class=\"luxbar-menu luxbar-menu-right luxbar-menu-material\">"
      "<ul class=\"luxbar-navigation\">"
        "<li class=\"luxbar-header\">"
          "<a href=\"" AUTOCONNECT_URI "\" class=\"luxbar-brand\">MENU_TITLE</a>"
          "<label class=\"luxbar-burger luxbar-burger-doublespin\" id=\"luxbar-burger\" for=\"luxbar-checkbox\"><span></span></label>"
        "</li>"
        "<li class=\"luxbar-item\"><a href=\"" AUTOCONNECT_URI_CONFIG "\">" AUTOCONNECT_MENULABEL_CONFIGNEW "</a></li>"
        "<li class=\"luxbar-item\"><a href=\"" AUTOCONNECT_URI_OPEN "\">" AUTOCONNECT_MENULABEL_OPENSSIDS "</a></li>"
        "<li class=\"luxbar-item\"><a href=\"" AUTOCONNECT_URI_DISCON "\">" AUTOCONNECT_MENULABEL_DISCONNECT "</a></li>"
        "<li class=\"luxbar-item\" id=\"reset\"><a href=\"#rdlg\">" AUTOCONNECT_MENULABEL_RESET "</a></li>"
};

const char  AutoConnect::_ELM_MENU_AUX[] PROGMEM = {
        "{{AUX_MENU}}"
};

const char  AutoConnect::_ELM_MENU_POST[] PROGMEM = {
        "<li class=\"luxbar-item\"><a href=\"HOME_URI\">" AUTOCONNECT_MENULABEL_HOME "</a></li>"
      "</ul>"
    "</div>"
    "<div class=\"lap\" id=\"rdlg\"><a href=\"#reset\" class=\"overlap\"></a>"
      "<div class=\"modal_button\"><h2><a href=\"" AUTOCONNECT_URI_RESET "\" class=\"modal_button\">" AUTOCONNECT_BUTTONLABEL_RESET "</a></h2></div>"
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
    "<meta http-equiv=\"refresh\" content=\"{{UPTIME}};url={{BOOTURI}}\">"
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
      "{{MENU_PRE}}"
      "{{MENU_AUX}}"
      "{{MENU_POST}}"
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
      "{{MENU_PRE}}"
      "{{MENU_AUX}}"
      "{{MENU_POST}}"
      "<div class=\"base-panel\">"
        "<form action=\"" AUTOCONNECT_URI_CONNECT "\" method=\"post\">"
          "{{LIST_SSID}}"
          "<div style=\"margin:16px 0 8px 0;border-bottom:solid 1px #263238;\">Total:{{SSID_COUNT}} Hidden:{{HIDDEN_COUNT}}</div>"
          "<ul class=\"noorder\">"
            "<li>"
              "<label for=\"ssid\">SSID</label>"
              "<input id=\"ssid\" type=\"text\" name=\"" AUTOCONNECT_PARAMID_SSID "\" placeholder=\"SSID\">"
            "</li>"
            "<li>"
              "<label for=\"passphrase\">Passphrase</label>"
              "<input id=\"passphrase\" type=\"password\" name=\"" AUTOCONNECT_PARAMID_PASS "\" placeholder=\"Passphrase\">"
            "</li>"
            "<br><li><input type=\"submit\" value=\"Apply\"></li>"
          "</ul>"
        "</form>"
      "</div>"
    "</div>"
  "</body>"
  "<script type=\"text/javascript\">"
    "function onFocus(value){"
      "document.getElementById('ssid').value=value;"
      "document.getElementById('passphrase').focus();"
    "}"
  "</script>"
  "</html>"
};

/**< A page that reads stored authentication information and starts connection. */
const char  AutoConnect::_PAGE_OPENCREDT[] PROGMEM = {
  "{{HEAD}}"
    "<title>AutoConnect credentials</title>"
    "<style type=\"text/css\">"
      "{{CSS_BASE}}"
      "{{CSS_ICON_LOCK}}"
      "{{CSS_INPUT_BUTTON}}"
      "{{CSS_LUXBAR}}"
    "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
    "<div class=\"container\">"
      "{{MENU_PRE}}"
      "{{MENU_AUX}}"
      "{{MENU_POST}}"
      "<div class=\"base-panel\">"
        "<form action=\"" AUTOCONNECT_URI_CONNECT "\" method=\"post\">"
          "{{OPEN_SSID}}"
        "</form>"
      "</div>"
    "</div>"
  "</body>"
  "</html>"
};

/**< A page that informs during a connection attempting. */
const char  AutoConnect::_PAGE_CONNECTING[] PROGMEM = {
  "{{REQ}}"
  "{{HEAD}}"
    "<title>AutoConnect connecting</title>"
    "<style type=\"text/css\">"
      "{{CSS_BASE}}"
      "{{CSS_SPINNER}}"
      "{{CSS_LUXBAR}}"
    "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
    "<div class=\"container\">"
      "{{MENU_PRE}}"
      "{{MENU_POST}}"
      "<div class=\"spinner\">"
        "<div class=\"double-bounce1\"></div>"
        "<div class=\"double-bounce2\"></div>"
        "<div style=\"position:absolute;left:-100%;right:-100%;text-align:center;margin:10px auto;font-weight:bold;color:#0b0b33;\">{{CUR_SSID}}</div>"
      "</div>"
    "</div>"
    "<script type=\"text/javascript\">"
      "setTimeout(\"link()\"," AUTOCONNECT_RESPONSE_WAITTIME ");"
      "function link(){location.href='" AUTOCONNECT_URI_RESULT "';}"
    "</script>"
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
      "{{MENU_PRE}}"
      "{{MENU_AUX}}"
      "{{MENU_POST}}"
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
      "{{MENU_PRE}}"
      "{{MENU_AUX}}"
      "{{MENU_POST}}"
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
      "{{MENU_PRE}}"
      "{{MENU_POST}}"
    "</div>"
  "</body>"
  "</html>"
};

// Each page of AutoConnect is http transferred by the content transfer
// mode of Page Builder. The default transfer mode is
// AUTOCONNECT_HTTP_TRANSFER defined in AutoConnectDefs.h. The page to
// which default transfer mode is not applied, specifies the enumeration
// value of PageBuilder::TransferEncoding_t. The content construction
// buffer can be reserved with the chunked transfer, and its size is
// macro defined by AUTOCONNECT_CONTENTBUFFER_SIZE.
const AutoConnect::PageTranserModeST AutoConnect::_pageBuildMode[] = {
  { AUTOCONNECT_URI,         AUTOCONNECT_HTTP_TRANSFER, 0 },
  { AUTOCONNECT_URI_CONFIG,  PB_Chunk, AUTOCONNECT_CONTENTBUFFER_SIZE },
  { AUTOCONNECT_URI_CONNECT, AUTOCONNECT_HTTP_TRANSFER, 0 },
  { AUTOCONNECT_URI_RESULT,  AUTOCONNECT_HTTP_TRANSFER, 0 },
  { AUTOCONNECT_URI_OPEN,    AUTOCONNECT_HTTP_TRANSFER, 0 },
  { AUTOCONNECT_URI_DISCON,  AUTOCONNECT_HTTP_TRANSFER, 0 },
  { AUTOCONNECT_URI_RESET,   AUTOCONNECT_HTTP_TRANSFER, 0 },
  { AUTOCONNECT_URI_SUCCESS, AUTOCONNECT_HTTP_TRANSFER, 0 },
  { AUTOCONNECT_URI_FAIL,    AUTOCONNECT_HTTP_TRANSFER, 0 }
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
  AC_UNUSED(args);
  return String(FPSTR(_CSS_BASE));
}

String AutoConnect::_token_CSS_UL(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_UL));
}

String AutoConnect::_token_CSS_ICON_LOCK(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_ICON_LOCK));
}

String AutoConnect::_token_CSS_INPUT_BUTTON(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_INPUT_BUTTON));
}

String AutoConnect::_token_CSS_INPUT_TEXT(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_INPUT_TEXT));
}

String AutoConnect::_token_CSS_TABLE(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_TABLE));
}

String AutoConnect::_token_CSS_SPINNER(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_SPINNER));
}

String AutoConnect::_token_HEAD(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_ELM_HTML_HEAD));
}

String AutoConnect::_token_MENU_PRE(PageArgument& args) {
  String  currentMenu = FPSTR(_ELM_MENU_PRE);
  currentMenu.replace(String(F("MENU_TITLE")), _menuTitle);
  currentMenu.replace(String(F("{{CUR_SSID}}")), _token_ESTAB_SSID(args));
  return currentMenu;
}

String AutoConnect::_token_MENU_AUX(PageArgument& args) {
  String  menuItem = String("");
  if (_aux)
    menuItem = _aux->_injectMenu(args);
  return menuItem;
}

String AutoConnect::_token_MENU_POST(PageArgument& args) {
  AC_UNUSED(args);
  String  postMenu = FPSTR(_ELM_MENU_POST);
  postMenu.replace(String(F("HOME_URI")), _apConfig.homeUri);
  return postMenu;
}

String AutoConnect::_token_CSS_LUXBAR(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_LUXBAR));
}

String AutoConnect::_token_ESTAB_SSID(PageArgument& args) {
  AC_UNUSED(args);
  return (WiFi.status() == WL_CONNECTED ? WiFi.SSID() : String(F("N/A")));
}

String AutoConnect::_token_WIFI_MODE(PageArgument& args) {
  AC_UNUSED(args);
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
  AC_UNUSED(args);
  return String(WiFi.status());
}

String AutoConnect::_token_STATION_STATUS(PageArgument& args) {
  AC_UNUSED(args);
  const char* wlStatusSymbol ="";
  static const char* wlStatusSymbols[] = {
#if defined(ARDUINO_ARCH_ESP8266)
    "IDLE",
    "CONNECTING",
    "WRONG_PASSWORD",
    "NO_AP_FOUND",
    "CONNECT_FAIL",
    "GOT_IP"
  };
  switch (wifi_station_get_connect_status()) {
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
#elif defined(ARDUINO_ARCH_ESP32)
    "IDLE",
    "NO_SSID_AVAIL",
    "SCAN_COMPLETED",
    "CONNECTED",
    "CONNECT_FAILED",
    "CONNECTION_LOST",
    "DISCONNECTED",
    "NO_SHIELD"
  };
  switch (_rsConnect) {
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
  case WL_NO_SHIELD:
    wlStatusSymbol = wlStatusSymbols[7];
    break;
#endif
  }
  return String("(") + String(_rsConnect) + String(") ") + String(wlStatusSymbol);
}

String AutoConnect::_token_LOCAL_IP(PageArgument& args) {
  AC_UNUSED(args);
  return WiFi.localIP().toString();
}

String AutoConnect::_token_SOFTAP_IP(PageArgument& args) {
  AC_UNUSED(args);
  return WiFi.softAPIP().toString();
}

String AutoConnect::_token_GATEWAY(PageArgument& args) {
  AC_UNUSED(args);
  return WiFi.gatewayIP().toString();
}

String AutoConnect::_token_NETMASK(PageArgument& args) {
  AC_UNUSED(args);
  return WiFi.subnetMask().toString();
}

String AutoConnect::_token_AP_MAC(PageArgument& args) {
  AC_UNUSED(args);
  uint8_t macAddress[6];
  WiFi.softAPmacAddress(macAddress);
  return AutoConnect::_toMACAddressString(macAddress);
}

String AutoConnect::_token_STA_MAC(PageArgument& args) {
  AC_UNUSED(args);
  uint8_t macAddress[6];
  WiFi.macAddress(macAddress);
  return AutoConnect::_toMACAddressString(macAddress);
}

String AutoConnect::_token_CHANNEL(PageArgument& args) {
  AC_UNUSED(args);
  return String(WiFi.channel());
}

String AutoConnect::_token_DBM(PageArgument& args) {
  AC_UNUSED(args);
  int32_t dBm = WiFi.RSSI();
  return (dBm == 31 ? String(F("N/A")) : String(dBm));
}

String AutoConnect::_token_CPU_FREQ(PageArgument& args) {
  AC_UNUSED(args);
  return String(ESP.getCpuFreqMHz());
}

String AutoConnect::_token_FLASH_SIZE(PageArgument& args) {
  AC_UNUSED(args);
  return String(_getFlashChipRealSize());
}

String AutoConnect::_token_CHIP_ID(PageArgument& args) {
  AC_UNUSED(args);
  return String(_getChipId());
}

String AutoConnect::_token_FREE_HEAP(PageArgument& args) {
  AC_UNUSED(args);
  return String(_freeHeapSize);
}

String AutoConnect::_token_LIST_SSID(PageArgument& args) {
  // Obtain the page number to display.
  // When the display request is the first page, it will be obtained
  // from the scan results of the WiFiScan class if it has already been
  // scanned.
  uint8_t page = 0;
  if (args.hasArg(String(F("page"))))
    page = args.arg("page").toInt();
  else {
    // Scan at a first time
    WiFi.scanDelete();
    _scanCount = WiFi.scanNetworks(false, true);
    AC_DBG("%d network(s) found\n", (int)_scanCount);
  }
  // Locate to the page and build SSD list content.
  String  ssidList = String("");
  _hiddenSSIDCount = 0;
  uint8_t validCount = 0;
  uint8_t dispCount = 0;
  for (uint8_t i = 0; i < _scanCount; i++) {
    String ssid = WiFi.SSID(i);
    if (ssid.length() > 0) {
      // An available SSID may be listed.
      // AUTOCONNECT_SSIDPAGEUNIT_LINES determines the number of lines
      // per page in the available SSID list.
      if (validCount >= page * AUTOCONNECT_SSIDPAGEUNIT_LINES && validCount <= (page + 1) * AUTOCONNECT_SSIDPAGEUNIT_LINES - 1) {
        if (++dispCount <= AUTOCONNECT_SSIDPAGEUNIT_LINES) {
          ssidList += String(F("<input type=\"button\" onClick=\"onFocus(this.getAttribute('value'))\" value=\"")) + ssid + String("\">");
          ssidList += String(F("<label class=\"slist\">")) + String(AutoConnect::_toWiFiQuality(WiFi.RSSI(i))) + String(F("&#037;&ensp;Ch.")) + String(WiFi.channel(i)) + String(F("</label>"));
          if (WiFi.encryptionType(i) != ENC_TYPE_NONE)
            ssidList += String(F("<span class=\"img-lock\"></span>"));
          ssidList += String(F("<br>"));
        }
      }
      // The validCount counts the found SSIDs that is not the Hidden
      // attribute to determines the next button should be displayed.
      validCount++;
    }
    else
      _hiddenSSIDCount++;
  }
  // Prepare perv. button
  if (page >= 1)
    ssidList += String(F("<button type=\"submit\" name=\"page\" value=\"")) + String(page - 1) + String(F("\" formaction=\"")) + String(F(AUTOCONNECT_URI_CONFIG)) + String(F("\">Prev.</button>&emsp;"));
  // Prepare next button
  if (validCount > (page + 1) * AUTOCONNECT_SSIDPAGEUNIT_LINES)
    ssidList += String(F("<button type=\"submit\" name=\"page\" value=\"")) + String(page + 1) + String(F("\" formaction=\"")) + String(F(AUTOCONNECT_URI_CONFIG)) + String(F("\">Next</button>&emsp;"));
  return ssidList;
}

String AutoConnect::_token_SSID_COUNT(PageArgument& args) {
  AC_UNUSED(args);
  return String(_scanCount);
}

String AutoConnect::_token_HIDDEN_COUNT(PageArgument& args) {
  AC_UNUSED(args);
  return String(_hiddenSSIDCount);
}

String AutoConnect::_token_OPEN_SSID(PageArgument& args) {
  AC_UNUSED(args);
  AutoConnectCredential credit(_apConfig.boundaryOffset);
  struct station_config entry;
  String ssidList;
  String rssiSym;

  uint8_t creEntries = credit.entries();
  if (creEntries > 0) {
    ssidList = String("");
    _scanCount = WiFi.scanNetworks(false, true);
  }
  else
    ssidList = String(F("<p><b>No saved credentials.</b></p>"));

  for (uint8_t i = 0; i < creEntries; i++) {
    credit.load(i, &entry);
    AC_DBG("A credential #%d loaded\n", (int)i);
    ssidList += String(F("<input id=\"sb\" type=\"submit\" name=\"" AUTOCONNECT_PARAMID_CRED "\" value=\"")) + String(reinterpret_cast<char*>(entry.ssid)) + String(F("\"><label class=\"slist\">"));
    rssiSym = String(F("N/A</label>"));
    for (int8_t sc = 0; sc < (int8_t)_scanCount; sc++) {
      if (!memcmp(entry.bssid, WiFi.BSSID(sc), sizeof(station_config::bssid))) {
        _connectCh = WiFi.channel(sc);
        rssiSym = String(AutoConnect::_toWiFiQuality(WiFi.RSSI(sc))) + String(F("&#037;&ensp;Ch.")) + String(_connectCh) + String(F("</label>"));
        if (WiFi.encryptionType(sc) != ENC_TYPE_NONE)
          rssiSym += String(F("<span class=\"img-lock\"></span>"));
        break;
      }
    }
    ssidList += rssiSym + String(F("<br>"));
  }
  return ssidList;
}

String AutoConnect::_token_UPTIME(PageArgument& args) {
  AC_UNUSED(args);
  return String(_apConfig.uptime);
}

String AutoConnect::_token_BOOTURI(PageArgument& args) {
  AC_UNUSED(args);
  if (_apConfig.bootUri == AC_ONBOOTURI_ROOT)
    return String(AUTOCONNECT_URI);
  else if (_apConfig.bootUri == AC_ONBOOTURI_HOME)
    return _apConfig.homeUri.length() > 0 ? _apConfig.homeUri : String("/");
  else
    return _emptyString;
}

String AutoConnect::_token_CURRENT_SSID(PageArgument& args) {
  AC_UNUSED(args);
  String  ssid = reinterpret_cast<char*>(_credential.ssid); 
  return ssid;
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

  // Restore menu title
  _menuTitle = _apConfig.title;

  // Build the elements of current requested page.
  if (uri == String(AUTOCONNECT_URI)) {

    // Setup /auto
    _freeHeapSize = ESP.getFreeHeap();
    elm->setMold(_PAGE_STAT);
    elm->addToken(String(FPSTR("HEAD")), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_BASE")), std::bind(&AutoConnect::_token_CSS_BASE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_TABLE")), std::bind(&AutoConnect::_token_CSS_TABLE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_LUXBAR")), std::bind(&AutoConnect::_token_CSS_LUXBAR, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_PRE")), std::bind(&AutoConnect::_token_MENU_PRE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_AUX")), std::bind(&AutoConnect::_token_MENU_AUX, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_POST")), std::bind(&AutoConnect::_token_MENU_POST, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("ESTAB_SSID")), std::bind(&AutoConnect::_token_ESTAB_SSID, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("WIFI_MODE")), std::bind(&AutoConnect::_token_WIFI_MODE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("WIFI_STATUS")), std::bind(&AutoConnect::_token_WIFI_STATUS, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("LOCAL_IP")), std::bind(&AutoConnect::_token_LOCAL_IP, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("SOFTAP_IP")), std::bind(&AutoConnect::_token_SOFTAP_IP, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("GATEWAY")), std::bind(&AutoConnect::_token_GATEWAY, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("NETMASK")), std::bind(&AutoConnect::_token_NETMASK, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("AP_MAC")), std::bind(&AutoConnect::_token_AP_MAC, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("STA_MAC")), std::bind(&AutoConnect::_token_STA_MAC, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CHANNEL")), std::bind(&AutoConnect::_token_CHANNEL, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("DBM")), std::bind(&AutoConnect::_token_DBM, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CPU_FREQ")), std::bind(&AutoConnect::_token_CPU_FREQ, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("FLASH_SIZE")), std::bind(&AutoConnect::_token_FLASH_SIZE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CHIP_ID")), std::bind(&AutoConnect::_token_CHIP_ID, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("FREE_HEAP")), std::bind(&AutoConnect::_token_FREE_HEAP, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_CONFIG)) {

    // Setup /auto/config
    elm->setMold(_PAGE_CONFIGNEW);
    elm->addToken(String(FPSTR("HEAD")), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_BASE")), std::bind(&AutoConnect::_token_CSS_BASE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_UL")), std::bind(&AutoConnect::_token_CSS_UL, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_ICON_LOCK")), std::bind(&AutoConnect::_token_CSS_ICON_LOCK, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_INPUT_BUTTON")), std::bind(&AutoConnect::_token_CSS_INPUT_BUTTON, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_INPUT_TEXT")), std::bind(&AutoConnect::_token_CSS_INPUT_TEXT, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_LUXBAR")), std::bind(&AutoConnect::_token_CSS_LUXBAR, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_PRE")), std::bind(&AutoConnect::_token_MENU_PRE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_AUX")), std::bind(&AutoConnect::_token_MENU_AUX, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_POST")), std::bind(&AutoConnect::_token_MENU_POST, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("LIST_SSID")), std::bind(&AutoConnect::_token_LIST_SSID, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("SSID_COUNT")), std::bind(&AutoConnect::_token_SSID_COUNT, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("HIDDEN_COUNT")), std::bind(&AutoConnect::_token_HIDDEN_COUNT, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_CONNECT)) {

    // Setup /auto/connect
    _menuTitle = FPSTR("Connecting");
    elm->setMold(_PAGE_CONNECTING);
    elm->addToken(String(FPSTR("REQ")), std::bind(&AutoConnect::_induceConnect, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("HEAD")), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_BASE")), std::bind(&AutoConnect::_token_CSS_BASE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_SPINNER")), std::bind(&AutoConnect::_token_CSS_SPINNER, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_LUXBAR")), std::bind(&AutoConnect::_token_CSS_LUXBAR, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_PRE")), std::bind(&AutoConnect::_token_MENU_PRE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_POST")), std::bind(&AutoConnect::_token_MENU_POST, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CUR_SSID")), std::bind(&AutoConnect::_token_CURRENT_SSID, this, std::placeholders::_1));
 }
  else if (uri == String(AUTOCONNECT_URI_OPEN)) {

    // Setup /auto/open
    elm->setMold(_PAGE_OPENCREDT);
    elm->addToken(String(FPSTR("HEAD")), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_BASE")), std::bind(&AutoConnect::_token_CSS_BASE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_ICON_LOCK")), std::bind(&AutoConnect::_token_CSS_ICON_LOCK, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_INPUT_BUTTON")), std::bind(&AutoConnect::_token_CSS_INPUT_BUTTON, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_LUXBAR")), std::bind(&AutoConnect::_token_CSS_LUXBAR, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_PRE")), std::bind(&AutoConnect::_token_MENU_PRE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_AUX")), std::bind(&AutoConnect::_token_MENU_AUX, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_POST")), std::bind(&AutoConnect::_token_MENU_POST, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("OPEN_SSID")), std::bind(&AutoConnect::_token_OPEN_SSID, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_DISCON)) {

    // Setup /auto/disc
    _menuTitle = FPSTR("Disconnect");
    elm->setMold(_PAGE_DISCONN);
    elm->addToken(String(FPSTR("DISCONNECT")), std::bind(&AutoConnect::_induceDisconnect, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("HEAD")), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_BASE")), std::bind(&AutoConnect::_token_CSS_BASE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_LUXBAR")), std::bind(&AutoConnect::_token_CSS_LUXBAR, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_PRE")), std::bind(&AutoConnect::_token_MENU_PRE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_POST")), std::bind(&AutoConnect::_token_MENU_POST, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_RESET)) {

    // Setup /auto/reset
    elm->setMold(_PAGE_RESETTING);
    elm->addToken(String(FPSTR("HEAD")), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("BOOTURI")), std::bind(&AutoConnect::_token_BOOTURI, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("UPTIME")), std::bind(&AutoConnect::_token_UPTIME, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("RESET")), std::bind(&AutoConnect::_induceReset, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_RESULT)) {

    // Setup /auto/result
    elm->setMold("{{RESULT}}");
    elm->addToken(String(FPSTR("RESULT")), std::bind(&AutoConnect::_invokeResult, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_SUCCESS)) {

    // Setup /auto/success
    elm->setMold(_PAGE_SUCCESS);
    elm->addToken(String(FPSTR("HEAD")), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_BASE")), std::bind(&AutoConnect::_token_CSS_BASE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_TABLE")), std::bind(&AutoConnect::_token_CSS_TABLE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_LUXBAR")), std::bind(&AutoConnect::_token_CSS_LUXBAR, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_PRE")), std::bind(&AutoConnect::_token_MENU_PRE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_AUX")), std::bind(&AutoConnect::_token_MENU_AUX, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_POST")), std::bind(&AutoConnect::_token_MENU_POST, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("ESTAB_SSID")), std::bind(&AutoConnect::_token_ESTAB_SSID, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("WIFI_MODE")), std::bind(&AutoConnect::_token_WIFI_MODE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("WIFI_STATUS")), std::bind(&AutoConnect::_token_WIFI_STATUS, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("LOCAL_IP")), std::bind(&AutoConnect::_token_LOCAL_IP, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("GATEWAY")), std::bind(&AutoConnect::_token_GATEWAY, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("NETMASK")), std::bind(&AutoConnect::_token_NETMASK, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CHANNEL")), std::bind(&AutoConnect::_token_CHANNEL, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("DBM")), std::bind(&AutoConnect::_token_DBM, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_FAIL)) {

    // Setup /auto/fail
    _menuTitle = FPSTR("Failed");
    elm->setMold(_PAGE_FAIL);
    elm->addToken(String(FPSTR("HEAD")), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_BASE")), std::bind(&AutoConnect::_token_CSS_BASE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_TABLE")), std::bind(&AutoConnect::_token_CSS_TABLE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_LUXBAR")), std::bind(&AutoConnect::_token_CSS_LUXBAR, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_PRE")), std::bind(&AutoConnect::_token_MENU_PRE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_AUX")), std::bind(&AutoConnect::_token_MENU_AUX, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_POST")), std::bind(&AutoConnect::_token_MENU_POST, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("STATION_STATUS")), std::bind(&AutoConnect::_token_STATION_STATUS, this, std::placeholders::_1));
  }
  else {
    delete elm;
    elm = nullptr;
  }

  // Restore the page transfer mode and the content build buffer
  // reserved size corresponding to each URI defined in structure
  // _pageBuildMode.
  if (elm) {
    for (uint8_t n = 0; n < sizeof(_pageBuildMode) / sizeof(PageTranserModeST); n++)
      if (!strcmp(_pageBuildMode[n].uri, uri.c_str())) {
        _responsePage->reserve(_pageBuildMode[n].rSize);
        _responsePage->chunked(_pageBuildMode[n].transMode);
        break;
      }
  }

  return elm;
}
