/**
 *  AutoConnect portal site web page implementation.
 *  @file   AutoConnectPage.cpp
 *  @author hieromon@gmail.com
 *  @version    1.2.0
 *  @date   2020-04-19
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
  ".base-panel * label :not(.bins){"
    "display:inline-block;"
    "width:3.0em;"
    "text-align:right"
  "}"
  ".base-panel * .slist{"
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
  ".noorder,.exp{"
    "padding:0;"
    "list-style:none;"
    "display:table"
  "}"
  ".noorder li,.exp{"
    "display:table-row-group"
  "}"
  ".noorder li label, .exp li *{"
    "display:table-cell;"
    "width:auto;"
    "text-align:right;"
    "padding:10px 0.5em"
  "}"
  ".noorder input[type=\"checkbox\"]{"
    "-moz-appearance:checkbox;"
    "-webkit-appearance:checkbox"
  "}"
  ".noorder input[type=\"radio\"]{"
    "margin-right:0.5em;"
    "-moz-appearance:radio;"
    "-webkit-appearance:radio"
  "}"
  ".noorder input[type=\"text\"]{"
    "width:auto"
  "}"
  ".noorder input[type=\"text\"]:invalid{"
    "background:#fce4d6"
  "}"
};

/**< Image icon for inline expansion, the lock mark. */
const char AutoConnect::_CSS_ICON_LOCK[] PROGMEM = {
  ".img-lock{"
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
  "input.error,input.error:focus{"
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
  "input.error::-webkit-input-placeholder,"
  "input.error::-moz-placeholder,"
  "input.error::-ms-input-placeholder{"
    "color:#D9434E"
  "}"
  ".aux-page label{"
    "display:inline;"
    "padding:10px 0.5em;"
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
  ".dbl-bounce1, .dbl-bounce2{"
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
  ".dbl-bounce2{"
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
  ".lb-fixed{"
    "width:100%;"
    "position:fixed;"
    "top:0;"
    "left:0;"
    "z-index:1000;"
    "box-shadow:0 1px 3px rgba(0, 0, 0, 0.12), 0 1px 2px rgba(0, 0, 0, 0.24)"
  "}"
  ".lb-burger span,"
  ".lb-burger span::before,"
  ".lb-burger span::after{"
    "display:block;"
    "height:2px;"
    "width:26px;"
    "transition:0.6s ease"
  "}"
  ".lb-cb:checked~.lb-menu li .lb-burger span{"
    "background-color:transparent"
  "}"
  ".lb-cb:checked~.lb-menu li .lb-burger span::before,"
  ".lb-cb:checked~.lb-menu li .lb-burger span::after{"
    "margin-top:0"
  "}"
  ".lb-header{"
    "display:flex;"
    "flex-direction:row;"
    "justify-content:space-between;"
    "align-items:center;"
    "height:58px"
  "}"
  ".lb-menu-right .lb-burger{"
    "margin-left:auto"
  "}"
  ".lb-brand{"
    "font-size:1.6em;"
    "padding:18px 24px 18px 24px"
  "}"
  ".lb-menu{"
    "min-height:58px;"
    "transition:0.6s ease;"
    "width:100%"
  "}"
  ".lb-navigation{"
    "display:flex;"
    "flex-direction:column;"
    "list-style:none;"
    "padding-left:0;"
    "margin:0"
  "}"
  ".lb-menu a,"
  ".lb-item a{"
    "text-decoration:none;"
    "color:inherit;"
    "cursor:pointer"
  "}"
  ".lb-item{"
    "height:58px"
  "}"
  ".lb-item a{"
    "padding:18px 24px 18px 24px;"
    "display:block"
  "}"
  ".lb-burger{"
    "padding:18px 24px 18px 24px;"
    "position:relative;"
    "cursor:pointer"
  "}"
  ".lb-burger span::before,"
  ".lb-burger span::after{"
    "content:'';"
    "position:absolute"
  "}"
  ".lb-burger span::before{"
    "margin-top:-8px"
  "}"
  ".lb-burger span::after{"
    "margin-top:8px"
  "}"
  ".lb-cb{"
    "display:none"
  "}"
  ".lb-cb:not(:checked)~.lb-menu{"
    "overflow:hidden;"
    "height:58px"
  "}"
  ".lb-cb:checked~.lb-menu{"
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
  ".dropdown>ul .lb-item{"
    "min-width:100%;"
    "height:29px;"
    "padding:5px 10px 5px 40px"
  "}"
  ".dropdown>ul .lb-item a{"
    "min-height:29px;"
    "line-height:29px;"
    "padding:0"
  "}"
  "@media screen and (min-width:768px){"
    ".lb-navigation{"
      "flex-flow:row;"
      "justify-content:flex-end;"
    "}"
    ".lb-burger{"
      "display:none;"
    "}"
    ".lb-cb:not(:checked)~.lb-menu{"
      "overflow:visible;"
    "}"
    ".lb-cb:checked~.lb-menu{"
      "height:58px;"
    "}"
    ".lb-menu .lb-item{"
      "border-top:0;"
    "}"
    ".lb-menu-right .lb-header{"
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
    ".dropdown>ul .lb-item{"
      "padding:5px 10px;"
    "}"
    ".dropdown>ul .lb-item a{"
      "white-space:nowrap;"
    "}"
  "}"
  ".lb-cb:checked+.lb-menu .lb-burger-dblspin span::before{"
    "transform:rotate(225deg)"
  "}"
  ".lb-cb:checked+.lb-menu .lb-burger-dblspin span::after{"
    "transform:rotate(-225deg)"
  "}"
  ".lb-menu-material,"
  ".lb-menu-material .dropdown ul{"
    "background-color:" AUTOCONNECT_MENUCOLOR_BACKGROUND ";"
    "color:" AUTOCONNECT_MENUCOLOR_TEXT
  "}"
  ".lb-menu-material .active,"
  ".lb-menu-material .lb-item:hover{"
    "background-color:" AUTOCONNECT_MENUCOLOR_ACTIVE
  "}"
  ".lb-menu-material .lb-burger span,"
  ".lb-menu-material .lb-burger span::before,"
  ".lb-menu-material .lb-burger span::after{"
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
  "<header id=\"lb\" class=\"lb-fixed\">"
    "<input type=\"checkbox\" class=\"lb-cb\" id=\"lb-cb\"/>"
    "<div class=\"lb-menu lb-menu-right lb-menu-material\">"
      "<ul class=\"lb-navigation\">"
        "<li class=\"lb-header\">"
          "<a href=\"BOOT_URI\" class=\"lb-brand\">MENU_TITLE</a>"
          "<label class=\"lb-burger lb-burger-dblspin\" id=\"lb-burger\" for=\"lb-cb\"><span></span></label>"
        "</li>"
        "MENU_LIST"
};

const char  AutoConnect::_ELM_MENU_AUX[] PROGMEM = {
        "{{AUX_MENU}}"
};

const char  AutoConnect::_ELM_MENU_POST[] PROGMEM = {
        "MENU_HOME"
        "MENU_DEVINFO"
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
    "<title>" AUTOCONNECT_PAGETITLE_NOTFOUND "</title>"
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
    "<title>" AUTOCONNECT_PAGETITLE_RESETTING "</title>"
  "</head>"
  "<body>"
    "<h3><div style=\"display:inline-block\"><span>{{RESET}}</span><span id=\"cd\"></span></div></h3>"
    "<script type=\"text/javascript\">"
      "window.onload=function(){"
        "var t={{UPTIME}},elm=document.getElementById(\"cd\"),ct=setInterval(function(){--t?elm.innerHTML=String(t)+\"&nbsp;sec.\":(elm.innerHTML=\"expiry\",clearInterval(ct))},1e3);"
      "};"
    "</script>"
  "</body>"
  "</html>"
};

/**< AutoConnect portal page. */
const char  AutoConnect::_PAGE_STAT[] PROGMEM = {
  "{{HEAD}}"
    "<title>" AUTOCONNECT_PAGETITLE_STATISTICS "</title>"
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
            "<td>" AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION "</td>"
            "<td>{{ESTAB_SSID}}</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_MODE "</td>"
            "<td>{{WIFI_MODE}}({{WIFI_STATUS}})</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_IP "</td>"
            "<td>{{LOCAL_IP}}</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_GATEWAY "</td>"
            "<td>{{GATEWAY}}</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_SUBNETMASK "</td>"
            "<td>{{NETMASK}}</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_SOFTAPIP "</td>"
            "<td>{{SOFTAP_IP}}</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_APMAC "</td>"
            "<td>{{AP_MAC}}</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_STAMAC "</td>"
            "<td>{{STA_MAC}}</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_CHANNEL "</td>"
            "<td>{{CHANNEL}}</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_DBM "</td>"
            "<td>{{DBM}}</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_CHIPID "</td>"
            "<td>{{CHIP_ID}}</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_CPUFREQ "</td>"
            "<td>{{CPU_FREQ}}MHz</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_FLASHSIZE "</td>"
            "<td>{{FLASH_SIZE}}</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_FREEMEM "</td>"
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
    "<title>" AUTOCONNECT_PAGETITLE_CONFIG "</title>"
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
          "<button style=\"width:0;height:0;padding:0;border:0;margin:0\" aria-hidden=\"true\" tabindex=\"-1\" type=\"submit\" name=\"apply\" value=\"apply\"></button>"
          "{{LIST_SSID}}"
          "<div style=\"margin:16px 0 8px 0;border-bottom:solid 1px #263238;\">" AUTOCONNECT_PAGECONFIG_TOTAL "{{SSID_COUNT}} " AUTOCONNECT_PAGECONFIG_HIDDEN "{{HIDDEN_COUNT}}</div>"
          "<ul class=\"noorder\">"
            "<li>"
              "<label for=\"ssid\">" AUTOCONNECT_PAGECONFIG_SSID "</label>"
              "<input id=\"ssid\" type=\"text\" name=\"" AUTOCONNECT_PARAMID_SSID "\" placeholder=\"" AUTOCONNECT_PAGECONFIG_SSID "\">"
            "</li>"
            "<li>"
              "<label for=\"passphrase\">" AUTOCONNECT_PAGECONFIG_PASSPHRASE "</label>"
              "<input id=\"passphrase\" type=\"password\" name=\"" AUTOCONNECT_PARAMID_PASS "\" placeholder=\"" AUTOCONNECT_PAGECONFIG_PASSPHRASE "\">"
            "</li>"
            "<li>"
              "<label for=\"dhcp\">" AUTOCONNECT_PAGECONFIG_ENABLEDHCP "</label>"
              "<input id=\"dhcp\" type=\"checkbox\" name=\"dhcp\" value=\"en\" checked onclick=\"vsw(this.checked);\">"
            "</li>"
            "{{CONFIG_IP}}"
            "<li><input type=\"submit\" name=\"apply\" value=\"" AUTOCONNECT_PAGECONFIG_APPLY "\"></li>"
          "</ul>"
        "</form>"
      "</div>"
    "</div>"
  "<script type=\"text/javascript\">"
    "window.onload=function(){"
      "['" AUTOCONNECT_PARAMID_STAIP "','" AUTOCONNECT_PARAMID_GTWAY "','" AUTOCONNECT_PARAMID_NTMSK "','" AUTOCONNECT_PARAMID_DNS1 "','" AUTOCONNECT_PARAMID_DNS2 "'].forEach(function(n,o,t){"
        "io=document.getElementById(n),io.placeholder='0.0.0.0',io.pattern='^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$'});"
      "vsw(true)};"
    "function onFocus(e){"
      "document.getElementById('ssid').value=e,document.getElementById('passphrase').focus()"
    "}"
    "function vsw(e){"
      "var t;t=e?'none':'table-row';for(const n of document.getElementsByClassName('exp'))n.style.display=t,n.getElementsByTagName('input')[0].disabled=e;e||document.getElementById('sip').focus()"
    "}"
  "</script>"
  "</body>"
  "</html>"
};

/**< A page that reads stored authentication information and starts connection. */
const char  AutoConnect::_PAGE_OPENCREDT[] PROGMEM = {
  "{{HEAD}}"
    "<title>" AUTOCONNECT_PAGETITLE_CREDENTIALS "</title>"
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
    "<title>" AUTOCONNECT_PAGETITLE_CONNECTING "</title>"
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
        "<div class=\"dbl-bounce1\"></div>"
        "<div class=\"dbl-bounce2\"></div>"
        "<div style=\"position:absolute;left:-100%;right:-100%;text-align:center;margin:10px auto;font-weight:bold;color:#0b0b33;\">{{CUR_SSID}}</div>"
      "</div>"
    "</div>"
    "<script type=\"text/javascript\">"
      "setTimeout(\"link()\"," AUTOCONNECT_STRING_DEPLOY(AUTOCONNECT_RESPONSE_WAITTIME) ");"
      "function link(){location.href='" AUTOCONNECT_URI_RESULT "';}"
    "</script>"
  "</body>"
  "</html>"
};

/**< A page announcing that a connection has been established. */
const char  AutoConnect::_PAGE_SUCCESS[] PROGMEM = {
  "{{HEAD}}"
    "<title>" AUTOCONNECT_PAGETITLE_STATISTICS "</title>"
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
            "<td>" AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION "</td>"
            "<td>{{ESTAB_SSID}}</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_MODE "</td>"
            "<td>{{WIFI_MODE}}({{WIFI_STATUS}})</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_IP "</td>"
            "<td>{{LOCAL_IP}}</td>"
          "</tr>"
            "<td>" AUTOCONNECT_PAGESTATS_GATEWAY "</td>"
            "<td>{{GATEWAY}}</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_SUBNETMASK "</td>"
            "<td>{{NETMASK}}</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_CHANNEL "</td>"
            "<td>{{CHANNEL}}</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_DBM "</td>"
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
    "<title>" AUTOCONNECT_PAGETITLE_CONNECTIONFAILED "</title>"
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
            "<td>" AUTOCONNECT_PAGECONNECTIONFAILED_CONNECTIONFAILED "</td>"
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
    "<title>" AUTOCONNECT_PAGETITLE_DISCONNECTED "</title>"
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

String AutoConnect::_token_CSS_LUXBAR(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_LUXBAR));
}

String AutoConnect::_token_CSS_SPINNER(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_SPINNER));
}

String AutoConnect::_token_CSS_TABLE(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_TABLE));
}

String AutoConnect::_token_CSS_UL(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_UL));
}

String AutoConnect::_token_MENU_AUX(PageArgument& args) {
  String  menuItem = String("");
  if (_aux)
    menuItem = _aux->_injectMenu(args);
  return menuItem;
}

String AutoConnect::_token_MENU_PRE(PageArgument& args) {
  String  currentMenu = FPSTR(_ELM_MENU_PRE);
  String  menuItem = _attachMenuItem(AC_MENUITEM_CONFIGNEW) +
                     _attachMenuItem(AC_MENUITEM_OPENSSIDS) +
                     _attachMenuItem(AC_MENUITEM_DISCONNECT) +
                     _attachMenuItem(AC_MENUITEM_RESET);
  currentMenu.replace(String(F("MENU_LIST")), menuItem);
  currentMenu.replace(String(F("BOOT_URI")), _getBootUri());
  currentMenu.replace(String(F("MENU_TITLE")), _menuTitle);
  currentMenu.replace(String(F("{{CUR_SSID}}")), _token_ESTAB_SSID(args));
  return currentMenu;
}

String AutoConnect::_token_MENU_POST(PageArgument& args) {
  AC_UNUSED(args);
  String  postMenu = FPSTR(_ELM_MENU_POST);
  postMenu.replace(String(F("MENU_HOME")), _attachMenuItem(AC_MENUITEM_HOME));
  postMenu.replace(String(F("HOME_URI")), _apConfig.homeUri);
  postMenu.replace(String(F("MENU_DEVINFO")), _attachMenuItem(AC_MENUITEM_DEVINFO));
  return postMenu;
}

String AutoConnect::_token_AP_MAC(PageArgument& args) {
  AC_UNUSED(args);
  uint8_t macAddress[6];
  WiFi.softAPmacAddress(macAddress);
  return AutoConnect::_toMACAddressString(macAddress);
}

String AutoConnect::_token_BOOTURI(PageArgument& args) {
  AC_UNUSED(args);
  return _getBootUri();
}

String AutoConnect::_token_CHANNEL(PageArgument& args) {
  AC_UNUSED(args);
  return String(WiFi.channel());
}

String AutoConnect::_token_CHIP_ID(PageArgument& args) {
  AC_UNUSED(args);
  return String(_getChipId());
}

String AutoConnect::_token_CONFIG_STAIP(PageArgument& args) {
  AC_UNUSED(args);
  static const char _configIPList[] PROGMEM =
    "<li class=\"exp\">"
    "<label for=\"%s\">%s</label>"
    "<input id=\"%s\" type=\"text\" name=\"%s\" value=\"%s\">"
    "</li>";
  struct _reps {
    PGM_P lid;
    PGM_P lbl;
  } static const reps[]  = {
    { PSTR(AUTOCONNECT_PARAMID_STAIP), PSTR("IP Address") },
    { PSTR(AUTOCONNECT_PARAMID_GTWAY), PSTR("Gateway") },
    { PSTR(AUTOCONNECT_PARAMID_NTMSK), PSTR("Netmask") },
    { PSTR(AUTOCONNECT_PARAMID_DNS1), PSTR("DNS1") },
    { PSTR(AUTOCONNECT_PARAMID_DNS2), PSTR("DNS2") }
  };
  char  liCont[600];
  char* liBuf = liCont;

  for (uint8_t i = 0; i < 5; i++) {
    IPAddress*  ip = nullptr;
    if (i == 0)
      ip = &_apConfig.staip;
    else if (i == 1)
      ip = &_apConfig.staGateway;
    else if (i == 2)
      ip = &_apConfig.staNetmask;
    else if (i == 3)
      ip = &_apConfig.dns1;
    else if (i == 4)
      ip = &_apConfig.dns2;
    String  ipStr = ip != nullptr ? ip->toString() : String(F("0.0.0.0"));
    snprintf_P(liBuf, sizeof(liCont) - (liBuf - liCont), (PGM_P)_configIPList, reps[i].lid, reps[i].lbl, reps[i].lid, reps[i].lid, ipStr.c_str());
    liBuf += strlen(liBuf);
  }
  return String(liCont);
}

String AutoConnect::_token_CPU_FREQ(PageArgument& args) {
  AC_UNUSED(args);
  return String(ESP.getCpuFreqMHz());
}

String AutoConnect::_token_CURRENT_SSID(PageArgument& args) {
  AC_UNUSED(args);
  char  ssid_c[sizeof(station_config_t::ssid) + 1];
  *ssid_c = '\0';
  strncat(ssid_c, reinterpret_cast<char*>(_credential.ssid), sizeof(ssid_c) - 1);
  String  ssid = String(ssid_c);
  return ssid;
}

String AutoConnect::_token_DBM(PageArgument& args) {
  AC_UNUSED(args);
  int32_t dBm = WiFi.RSSI();
  return (dBm == 31 ? String(F("N/A")) : String(dBm));
}

String AutoConnect::_token_ESTAB_SSID(PageArgument& args) {
  AC_UNUSED(args);
  return (WiFi.status() == WL_CONNECTED ? WiFi.SSID() : String(F("N/A")));
}

String AutoConnect::_token_FLASH_SIZE(PageArgument& args) {
  AC_UNUSED(args);
  return String(_getFlashChipRealSize());
}

String AutoConnect::_token_FREE_HEAP(PageArgument& args) {
  AC_UNUSED(args);
  return String(_freeHeapSize);
}

String AutoConnect::_token_GATEWAY(PageArgument& args) {
  AC_UNUSED(args);
  return WiFi.gatewayIP().toString();
}

String AutoConnect::_token_HEAD(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_ELM_HTML_HEAD));
}

String AutoConnect::_token_HIDDEN_COUNT(PageArgument& args) {
  AC_UNUSED(args);
  return String(_hiddenSSIDCount);
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
    _scanCount = WiFi.scanNetworks(false, true);
    AC_DBG("%d network(s) found, ", (int)_scanCount);
  }
  // Prepare SSID list content building buffer
  size_t  bufSize = sizeof('\0') + 192 * (_scanCount > AUTOCONNECT_SSIDPAGEUNIT_LINES ? AUTOCONNECT_SSIDPAGEUNIT_LINES : _scanCount);
  bufSize += 88 * (_scanCount > AUTOCONNECT_SSIDPAGEUNIT_LINES ? (_scanCount > (AUTOCONNECT_SSIDPAGEUNIT_LINES * 2) ? 2 : 1) : 0);
  AC_DBG_DUMB("%d buf", bufSize);
  char* ssidList = (char*)malloc(bufSize);
  if (!ssidList) {
    AC_DBG_DUMB(" alloc. failed\n");
    WiFi.scanDelete();
    return _emptyString;
  }
  AC_DBG_DUMB("\n");
  // Locate to the page and build SSD list content.
  static const char _ssidList[] PROGMEM =
    "<input type=\"button\" onClick=\"onFocus(this.getAttribute('value'))\" value=\"%s\">"
    "<label class=\"slist\">%d&#037;&ensp;Ch.%d</label>%s<br>";
  static const char _ssidEnc[] PROGMEM =
    "<span class=\"img-lock\"></span>";
  static const char _ssidPage[] PROGMEM =
    "<button type=\"submit\" name=\"page\" value=\"%d\" formaction=\"" AUTOCONNECT_URI_CONFIG "\">%s</button>&emsp;";
  _hiddenSSIDCount = 0;
  uint8_t validCount = 0;
  uint8_t dispCount = 0;
  char* slBuf = ssidList;
  *slBuf = '\0';
  for (uint8_t i = 0; i < _scanCount; i++) {
    String ssid = WiFi.SSID(i);
    if (ssid.length() > 0) {
      // An available SSID may be listed.
      // AUTOCONNECT_SSIDPAGEUNIT_LINES determines the number of lines
      // per page in the available SSID list.
      if (validCount >= page * AUTOCONNECT_SSIDPAGEUNIT_LINES && validCount <= (page + 1) * AUTOCONNECT_SSIDPAGEUNIT_LINES - 1) {
        if (++dispCount <= AUTOCONNECT_SSIDPAGEUNIT_LINES) {
          snprintf_P(slBuf, bufSize - (slBuf - ssidList), (PGM_P)_ssidList, ssid.c_str(), AutoConnect::_toWiFiQuality(WiFi.RSSI(i)), WiFi.channel(i), WiFi.encryptionType(i) != ENC_TYPE_NONE ? (PGM_P)_ssidEnc : "");
          slBuf += strlen(slBuf);
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
  if (page >= 1) {
    snprintf_P(slBuf, bufSize - (slBuf - ssidList), (PGM_P)_ssidPage, page - 1, PSTR("Prev."));
    slBuf = ssidList + strlen(ssidList);
  }
  // Prepare next button
  if (validCount > (page + 1) * AUTOCONNECT_SSIDPAGEUNIT_LINES) {
    snprintf_P(slBuf, bufSize - (slBuf - ssidList), (PGM_P)_ssidPage, page + 1, PSTR("Next"));
  }
  String ssidListStr = String(ssidList);
  free(ssidList);
  return ssidListStr;
}

String AutoConnect::_token_LOCAL_IP(PageArgument& args) {
  AC_UNUSED(args);
  return WiFi.localIP().toString();
}

String AutoConnect::_token_NETMASK(PageArgument& args) {
  AC_UNUSED(args);
  return WiFi.subnetMask().toString();
}

String AutoConnect::_token_OPEN_SSID(PageArgument& args) {
  AC_UNUSED(args);
  static const char _ssidList[] PROGMEM = "<input id=\"sb\" type=\"submit\" name=\"%s\" value=\"%s\"><label class=\"slist\">%s</label>%s<br>";
  static const char _ssidRssi[] PROGMEM = "%d&#037;&ensp;Ch.%d";
  static const char _ssidNA[]   PROGMEM = "N/A";
  static const char _ssidLock[] PROGMEM = "<span class=\"img-lock\"></span>";
  static const char _ssidNull[] PROGMEM = "";
  String ssidList;
  station_config_t  entry;
  char  slCont[176];
  char  rssiCont[32];
  AutoConnectCredential credit(_apConfig.boundaryOffset);

  uint8_t creEntries = credit.entries();
  if (creEntries > 0) {
    ssidList = String("");
    _scanCount = WiFi.scanNetworks(false, true);
  }
  else
    ssidList = String(F("<p><b>" AUTOCONNECT_TEXT_NOSAVEDCREDENTIALS "</b></p>"));

  for (uint8_t i = 0; i < creEntries; i++) {
    rssiCont[0] = '\0';
    PGM_P rssiSym = _ssidNA;
    PGM_P ssidLock = _ssidNull;
    credit.load(i, &entry);
    AC_DBG("Credential #%d loaded\n", (int)i);
    for (int8_t sc = 0; sc < (int8_t)_scanCount; sc++) {
      if (_isValidAP(entry, sc)) {
        // The access point collation key is determined at compile time
        // according to the AUTOCONNECT_APKEY_SSID definition, which is
        // either BSSID or SSID.
        _connectCh = WiFi.channel(sc);
        snprintf_P(rssiCont, sizeof(rssiCont), (PGM_P)_ssidRssi, AutoConnect::_toWiFiQuality(WiFi.RSSI(sc)), _connectCh);
        rssiSym = rssiCont;
        if (WiFi.encryptionType(sc) != ENC_TYPE_NONE)
          ssidLock = _ssidLock;
        break;
      }
    }
    snprintf_P(slCont, sizeof(slCont), (PGM_P)_ssidList, AUTOCONNECT_PARAMID_CRED, reinterpret_cast<char*>(entry.ssid), rssiSym, ssidLock);
    ssidList += String(slCont);
  }
  return ssidList;
}

String AutoConnect::_token_SOFTAP_IP(PageArgument& args) {
  AC_UNUSED(args);
  return WiFi.softAPIP().toString();
}

String AutoConnect::_token_SSID_COUNT(PageArgument& args) {
  AC_UNUSED(args);
  return String(_scanCount);
}

String AutoConnect::_token_STA_MAC(PageArgument& args) {
  AC_UNUSED(args);
  uint8_t macAddress[6];
  WiFi.macAddress(macAddress);
  return AutoConnect::_toMACAddressString(macAddress);
}

String AutoConnect::_token_STATION_STATUS(PageArgument& args) {
  AC_UNUSED(args);
  PGM_P wlStatusSymbol = PSTR("");
  PGM_P wlStatusSymbols[] = {
#if defined(ARDUINO_ARCH_ESP8266)
    PSTR("IDLE"),
    PSTR("CONNECTING"),
    PSTR("WRONG_PASSWORD"),
    PSTR("NO_AP_FOUND"),
    PSTR("CONNECT_FAIL"),
    PSTR("GOT_IP")
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
    PSTR("IDLE"),
    PSTR("NO_SSID_AVAIL"),
    PSTR("SCAN_COMPLETED"),
    PSTR("CONNECTED"),
    PSTR("CONNECT_FAILED"),
    PSTR("CONNECTION_LOST"),
    PSTR("DISCONNECTED"),
    PSTR("NO_SHIELD")
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
  return String("(") + String(_rsConnect) + String(") ") + String(FPSTR(wlStatusSymbol));
}

String AutoConnect::_token_UPTIME(PageArgument& args) {
  AC_UNUSED(args);
  return String(_apConfig.uptime);
}

String AutoConnect::_token_WIFI_MODE(PageArgument& args) {
  AC_UNUSED(args);
  PGM_P wifiMode;
  switch (WiFi.getMode()) {
  case WIFI_OFF:
    wifiMode = PSTR("OFF");
    break;
  case WIFI_STA:
    wifiMode = PSTR("STA");
    break;
  case WIFI_AP:
    wifiMode = PSTR("AP");
    break;
  case WIFI_AP_STA:
    wifiMode = PSTR("AP_STA");
    break;
#ifdef ARDUINO_ARCH_ESP32
  case WIFI_MODE_MAX:
    wifiMode = PSTR("MAX");
    break;
#endif
  default:
    wifiMode = PSTR("experimental");
  }
  return String(FPSTR(wifiMode));
}

String AutoConnect::_token_WIFI_STATUS(PageArgument& args) {
  AC_UNUSED(args);
  return String(WiFi.status());
}

/**
 *  Generate AutoConnect menu item configured by AutoConnectConfig::attachMenu.
 *  @param  item  An enumerated value for the generating item configured in AutoConnectConfig.
 *  @return HTML string of a li tag with the menu item.
 */
String AutoConnect::_attachMenuItem(const AC_MENUITEM_t item) {
  static const char _liTempl[]  PROGMEM = "<li class=\"lb-item\"%s><a href=\"%s\">%s</a></li>";
  PGM_P id = PSTR("");
  PGM_P link;
  PGM_P label;

  switch (static_cast<AC_MENUITEM_t>(_apConfig.menuItems & static_cast<uint16_t>(item))) {
  case AC_MENUITEM_CONFIGNEW:
    link = PSTR(AUTOCONNECT_URI_CONFIG);
    label = PSTR(AUTOCONNECT_MENULABEL_CONFIGNEW);
    break;
  case AC_MENUITEM_OPENSSIDS:
    link = PSTR(AUTOCONNECT_URI_OPEN);
    label = PSTR(AUTOCONNECT_MENULABEL_OPENSSIDS);
    break;
  case AC_MENUITEM_DISCONNECT:
    link = PSTR(AUTOCONNECT_URI_DISCON);
    label = PSTR(AUTOCONNECT_MENULABEL_DISCONNECT);
    break;
  case AC_MENUITEM_RESET:
    id = PSTR(" id=\"reset\"");
    link = PSTR("#rdlg");
    label = PSTR(AUTOCONNECT_MENULABEL_RESET);
    break;
  case AC_MENUITEM_HOME:
    link = PSTR("HOME_URI");
    label = PSTR(AUTOCONNECT_MENULABEL_HOME);
    break;
  case AC_MENUITEM_DEVINFO:
    link = PSTR(AUTOCONNECT_URI);
    label = PSTR(AUTOCONNECT_MENULABEL_DEVINFO);
    break;
  default:
    id = nullptr;
    link = nullptr;
    label = nullptr;
    break;
  }
  char  li[128] = { '\0' };
  if (!!id && !!link && !!label)
    snprintf(li, sizeof(li), (PGM_P)_liTempl, id, link, label);
  return String(li);
}

/**
 *  This function dynamically build up the response pages that conform to
 *  the requested URI. A PageBuilder instance is stored in _responsePage
 *  as the response page.
 *  @param  Requested URI.
 *  @retval true  A response page generated.
 *  @retval false Requested uri is not defined.
 */
PageElement* AutoConnect::_setupPage(String& uri) {
  PageElement *elm = new PageElement();
  bool  reqAuth = false;

  // Restore menu title
  _menuTitle = _apConfig.title;

  // Build the elements of current requested page.
  if (uri == String(AUTOCONNECT_URI)) {

    // Setup /_ac
    reqAuth = true;
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
  else if (uri == String(AUTOCONNECT_URI_CONFIG) && (_apConfig.menuItems & AC_MENUITEM_CONFIGNEW)) {

    // Setup /_ac/config
    reqAuth = true;
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
    elm->addToken(String(FPSTR("CONFIG_IP")), std::bind(&AutoConnect::_token_CONFIG_STAIP, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_CONNECT) && (_apConfig.menuItems & AC_MENUITEM_CONFIGNEW || _apConfig.menuItems & AC_MENUITEM_OPENSSIDS)) {

    // Setup /_ac/connect
    reqAuth = true;
    _menuTitle = FPSTR(AUTOCONNECT_MENUTEXT_CONNECTING);
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
  else if (uri == String(AUTOCONNECT_URI_OPEN) && (_apConfig.menuItems & AC_MENUITEM_OPENSSIDS)) {

    // Setup /_ac/open
    reqAuth = true;
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
  else if (uri == String(AUTOCONNECT_URI_DISCON) && (_apConfig.menuItems & AC_MENUITEM_DISCONNECT)) {

    // Setup /_ac/disc
    _menuTitle = FPSTR(AUTOCONNECT_MENUTEXT_DISCONNECT);
    elm->setMold(_PAGE_DISCONN);
    elm->addToken(String(FPSTR("DISCONNECT")), std::bind(&AutoConnect::_induceDisconnect, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("HEAD")), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_BASE")), std::bind(&AutoConnect::_token_CSS_BASE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("CSS_LUXBAR")), std::bind(&AutoConnect::_token_CSS_LUXBAR, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_PRE")), std::bind(&AutoConnect::_token_MENU_PRE, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("MENU_POST")), std::bind(&AutoConnect::_token_MENU_POST, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_RESET) && (_apConfig.menuItems & AC_MENUITEM_RESET)) {

    // Setup /_ac/reset
    elm->setMold(_PAGE_RESETTING);
    elm->addToken(String(FPSTR("HEAD")), std::bind(&AutoConnect::_token_HEAD, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("BOOTURI")), std::bind(&AutoConnect::_token_BOOTURI, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("UPTIME")), std::bind(&AutoConnect::_token_UPTIME, this, std::placeholders::_1));
    elm->addToken(String(FPSTR("RESET")), std::bind(&AutoConnect::_induceReset, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_RESULT)) {

    // Setup /_ac/result
    elm->setMold("{{RESULT}}");
    elm->addToken(String(FPSTR("RESULT")), std::bind(&AutoConnect::_invokeResult, this, std::placeholders::_1));
  }
  else if (uri == String(AUTOCONNECT_URI_SUCCESS)) {

    // Setup /_ac/success
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

    // Setup /_ac/fail
    _menuTitle = FPSTR(AUTOCONNECT_MENUTEXT_FAILED);
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

    // Regiter authentication
    // Determine the necessity of authentication from the AutoConnectConfig settings
    bool auth = (_apConfig.auth != AC_AUTH_NONE) &&
                (_apConfig.authScope & AC_AUTHSCOPE_AC) &&
                reqAuth;
    _authentication(auth);
  }

  return elm;
}

/**
 *  Allow the page set upped to authenticate.
 *  The argument parameter indicates that authentication is allowed with
 *  the condition of the AutoConnect.authScope setting.
 *  It determines to admit authentication in the captive portal state
 *  when the AP_AUTHSCOPE_WITHCP is enabled.
 *  @param allow  Indication of whether to authenticate with the page.
 */ 
void AutoConnect::_authentication(bool allow) {
  HTTPAuthMethod  method = _apConfig.auth == AC_AUTH_BASIC ? HTTPAuthMethod::BASIC_AUTH : HTTPAuthMethod::DIGEST_AUTH;
  _authentication(allow, method);
}

/**
 *  Allow the page set upped to authenticate.
 *  The argument parameter indicates that authentication is allowed with
 *  the condition of the AutoConnect.authScope setting.
 *  It determines to admit authentication in the captive portal state
 *  when the AP_AUTHSCOPE_WITHCP is enabled.
 *  @param allow  Indication of whether to authenticate with the page.
 */ 
void AutoConnect::_authentication(bool allow, const HTTPAuthMethod method) {
  const char* user = nullptr;
  const char* password = nullptr;
  String  fails;

  // Enable authentication by setting of AC_AUTHSCOPE_DISCONNECTED even if WiFi is not connected.
  if (WiFi.status() != WL_CONNECTED && (WiFi.getMode() & WIFI_AP)) {
    String  accUrl = _webServer->hostHeader();
    if ((accUrl != WiFi.softAPIP().toString()) && !accUrl.endsWith(F(".local"))) {
      if (!(_apConfig.authScope & AC_AUTHSCOPE_WITHCP))
        allow = false;
    }
  }

  if (allow) {
    // Regiter authentication method
    user = _apConfig.username.length() ? _apConfig.username.c_str() : _apConfig.apid.c_str();
    password = _apConfig.password.length() ? _apConfig.password.c_str() : _apConfig.psk.c_str();
    fails = String(FPSTR(AutoConnect::_ELM_HTML_HEAD)) + String(F("</head><body>" AUTOCONNECT_TEXT_AUTHFAILED "</body></html>"));
    AC_DBG_DUMB(",%s+%s/%s", method == HTTPAuthMethod::BASIC_AUTH ? "BASIC" : "DIGEST", user, password);
  }
  _responsePage->authentication(user, password, method, AUTOCONNECT_AUTH_REALM, fails);
}
