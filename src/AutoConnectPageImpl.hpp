/**
 * AutoConnect portal site web page implementation.
 * @file AutoConnectPage.cpp
 * @author hieromon@gmail.com
 * @version 1.4.2
 * @date 2023-02-05
 * @copyright MIT license.
 */

#include <algorithm>
#include <type_traits>
#include "AutoConnectCore.hpp"

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

#define AC_STRINGIFY_DECL(s)    s,#s
template<typename T>
typename AutoConnectCore<T>::AC_PAGETOKENSTRING_t AutoConnectCore<T>::_tokenStringify[] PROGMEM = {
  { AC_STRINGIFY_DECL(AP_MAC) },
  { AC_STRINGIFY_DECL(BOOTURI) },
  { AC_STRINGIFY_DECL(CHANNEL) },
  { AC_STRINGIFY_DECL(CHIP_ID) },
  { AC_STRINGIFY_DECL(CONFIG_IP) },
  { AC_STRINGIFY_DECL(CPU_FREQ) },
  { AC_STRINGIFY_DECL(CSS_BASE) },
  { AC_STRINGIFY_DECL(CSS_ICON_LOCK) },
  { AC_STRINGIFY_DECL(CSS_ICON_TRASH) },
  { AC_STRINGIFY_DECL(CSS_INPUT_BUTTON) },
  { AC_STRINGIFY_DECL(CSS_INPUT_TEXT) },
  { AC_STRINGIFY_DECL(CSS_LUXBAR_ANI) },
  { AC_STRINGIFY_DECL(CSS_LUXBAR_BGR) },
  { AC_STRINGIFY_DECL(CSS_LUXBAR_BODY) },
  { AC_STRINGIFY_DECL(CSS_LUXBAR_HEADER) },
  { AC_STRINGIFY_DECL(CSS_LUXBAR_ITEM) },
  { AC_STRINGIFY_DECL(CSS_LUXBAR_MEDIA) },
  { AC_STRINGIFY_DECL(CSS_SPINNER) },
  { AC_STRINGIFY_DECL(CSS_TABLE) },
  { AC_STRINGIFY_DECL(CSS_UL) },
  { AC_STRINGIFY_DECL(CUR_SSID) },
  { AC_STRINGIFY_DECL(DBM) },
  { AC_STRINGIFY_DECL(DELREQ) },
  { AC_STRINGIFY_DECL(DISCONNECT) },
  { AC_STRINGIFY_DECL(ESTAB_SSID) },
  { AC_STRINGIFY_DECL(FLASH_SIZE) },
  { AC_STRINGIFY_DECL(FREE_HEAP) },
  { AC_STRINGIFY_DECL(GATEWAY) },
  { AC_STRINGIFY_DECL(HEAD) },
  { AC_STRINGIFY_DECL(HIDDEN_COUNT) },
  { AC_STRINGIFY_DECL(LIST_SSID) },
  { AC_STRINGIFY_DECL(LOCAL_IP) },
  { AC_STRINGIFY_DECL(MENU_AUX) },
  { AC_STRINGIFY_DECL(MENU_LIST) },
  { AC_STRINGIFY_DECL(MENU_POST) },
  { AC_STRINGIFY_DECL(MENU_PRE) },
  { AC_STRINGIFY_DECL(NETMASK) },
  { AC_STRINGIFY_DECL(OPEN_SSID) },
  { AC_STRINGIFY_DECL(REQ) },
  { AC_STRINGIFY_DECL(RESET) },
  { AC_STRINGIFY_DECL(SOFTAP_IP) },
  { AC_STRINGIFY_DECL(SSID_COUNT) },
  { AC_STRINGIFY_DECL(STA_MAC) },
  { AC_STRINGIFY_DECL(STATION_STATUS) },
  { AC_STRINGIFY_DECL(SYSTEM_UPTIME) },
  { AC_STRINGIFY_DECL(UPTIME) },
  { AC_STRINGIFY_DECL(WIFI_MODE) },
  { AC_STRINGIFY_DECL(WIFI_STATUS) }
};

template<typename T>
typename AutoConnectCore<T>::AC_PAGECONST_t AutoConnectCore<T>::_pageConst[] = {
  {AUTOCONNECT_URI, nullptr, AutoConnectCore<T>::_PAGE_STAT, AC_MENU_ALL, true, {
    HEAD, CSS_BASE, CSS_TABLE, CSS_LUXBAR_BODY, CSS_LUXBAR_HEADER, CSS_LUXBAR_BGR, CSS_LUXBAR_ANI, CSS_LUXBAR_MEDIA, CSS_LUXBAR_ITEM, MENU_PRE, MENU_LIST, MENU_AUX, MENU_POST, ESTAB_SSID, WIFI_MODE, WIFI_STATUS, LOCAL_IP, SOFTAP_IP, GATEWAY, NETMASK, AP_MAC, STA_MAC, CHANNEL, DBM, CPU_FREQ, FLASH_SIZE, CHIP_ID, FREE_HEAP, SYSTEM_UPTIME
  }},
  {AUTOCONNECT_URI_CONFIG, nullptr, AutoConnectCore<T>::_PAGE_CONFIGNEW, AC_MENUITEM_CONFIGNEW, true, {
    HEAD, CSS_BASE, CSS_UL, CSS_ICON_LOCK, CSS_INPUT_BUTTON, CSS_INPUT_TEXT, CSS_LUXBAR_BODY, CSS_LUXBAR_HEADER, CSS_LUXBAR_BGR, CSS_LUXBAR_ANI, CSS_LUXBAR_MEDIA, CSS_LUXBAR_ITEM, MENU_PRE, MENU_LIST, MENU_AUX, MENU_POST, LIST_SSID, SSID_COUNT, HIDDEN_COUNT, CONFIG_IP
  }},
  {AUTOCONNECT_URI_CONNECT, AUTOCONNECT_MENUTEXT_CONNECTING, AutoConnectCore<T>::_PAGE_CONNECTING, AC_MENUITEM_CONFIGNEW | AC_MENUITEM_OPENSSIDS, true, {
    REQ, HEAD, CSS_BASE, CSS_SPINNER, CSS_LUXBAR_BODY, CSS_LUXBAR_HEADER, CSS_LUXBAR_BGR, CSS_LUXBAR_ANI, CSS_LUXBAR_MEDIA, CSS_LUXBAR_ITEM, MENU_PRE, MENU_LIST, MENU_POST, CUR_SSID
  }},
  {AUTOCONNECT_URI_OPEN, nullptr, AutoConnectCore<T>::_PAGE_OPENCREDT, AC_MENUITEM_OPENSSIDS, true, {
    HEAD, CSS_BASE, CSS_ICON_LOCK, CSS_ICON_TRASH, CSS_INPUT_BUTTON, CSS_LUXBAR_BODY, CSS_LUXBAR_HEADER, CSS_LUXBAR_BGR, CSS_LUXBAR_ANI, CSS_LUXBAR_MEDIA, CSS_LUXBAR_ITEM, MENU_PRE, MENU_LIST, MENU_AUX, MENU_POST, OPEN_SSID
  }},
  {AUTOCONNECT_URI_DELETE, nullptr, AutoConnectCore<T>::_PAGE_DELREQ, AC_MENUITEM_OPENSSIDS | AC_MENUITEM_DELETESSID, true, {
    DELREQ
  }},
  {AUTOCONNECT_URI_DISCON, AUTOCONNECT_MENUTEXT_DISCONNECT, AutoConnectCore<T>::_PAGE_DISCONN, AC_MENUITEM_DISCONNECT, false, {
    DISCONNECT, HEAD, CSS_BASE, CSS_LUXBAR_BODY, CSS_LUXBAR_HEADER, CSS_LUXBAR_BGR, CSS_LUXBAR_ANI, CSS_LUXBAR_MEDIA, CSS_LUXBAR_ITEM, MENU_PRE, MENU_LIST, MENU_POST
  }},
  {AUTOCONNECT_URI_RESET, nullptr, AutoConnectCore<T>::_PAGE_RESETTING, AC_MENUITEM_RESET, true, {
    HEAD, BOOTURI, UPTIME, RESET
  }},
  {AUTOCONNECT_URI_RESULT, nullptr, AutoConnectCore<T>::_PAGE_RESULT, AC_MENU_ALL, false, {
    RESULT
  }},
  {AUTOCONNECT_URI_SUCCESS, nullptr, AutoConnectCore<T>::_PAGE_SUCCESS, AC_MENU_ALL, false, {
    HEAD, CSS_BASE, CSS_TABLE, CSS_LUXBAR_BODY, CSS_LUXBAR_HEADER, CSS_LUXBAR_BGR, CSS_LUXBAR_ANI, CSS_LUXBAR_MEDIA, CSS_LUXBAR_ITEM, MENU_PRE, MENU_LIST, MENU_AUX, MENU_POST, ESTAB_SSID, WIFI_MODE, WIFI_STATUS, LOCAL_IP, GATEWAY, NETMASK, CHANNEL, DBM
  }},
  {AUTOCONNECT_URI_FAIL, AUTOCONNECT_MENUTEXT_FAILED, AutoConnectCore<T>::_PAGE_FAIL, AC_MENU_ALL, false, {
    HEAD, CSS_BASE, CSS_TABLE, CSS_LUXBAR_BODY, CSS_LUXBAR_HEADER, CSS_LUXBAR_BGR, CSS_LUXBAR_ANI, CSS_LUXBAR_MEDIA, CSS_LUXBAR_ITEM, MENU_PRE, MENU_LIST, MENU_AUX, MENU_POST, STATION_STATUS
  }}
};

/**< Basic CSS common to all pages */
template<typename T>
const char AutoConnectCore<T>::_CSS_BASE[] PROGMEM = {
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
template<typename T>
const char AutoConnectCore<T>::_CSS_UL[] PROGMEM = {
  ".noorder,.exp{"
    "padding:0;"
    "list-style:none;"
    "display:table"
  "}"
  ".noorder li,.exp{"
    "display:table-row-group"
  "}"
  ".noorder li label, .exp li{"
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
    "cursor:pointer;"
    "-moz-appearance:radio;"
    "-webkit-appearance:radio"
  "}"
  ".noorder input[type=\"text\"],.noorder input[type=\"password\"],.noorder input[type=\"number\"],.noorder input[type=\"range\"]{"
    "width:auto"
  "}"
  ".noorder input[type=\"text\"]:invalid{"
    "background:#fce4d6"
  "}"
  ".noorder input[type=\"range\"]{"
    "height:7px;"
    "background:#dddddd;"
    "cursor:pointer"
  "}"
  ".magnify{"
    "display:inline-block"
  "}"
};

/**< Image icon for inline expansion, the lock mark. */
template<typename T>
const char AutoConnectCore<T>::_CSS_ICON_LOCK[] PROGMEM = {
  ".img-lock{"
    "width:22px;"
    "height:22px;"
    "margin-top:14px;"
    "float:right;"
    "background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAACXBIWXMAAAsTAAALEwEAmpwYAAAB1ElEQVRIibWVu0scURTGf3d2drBQFAWbbRQVCwuVLIZdi2gnWIiF/4GtKyuJGAJh8mgTcU0T8T8ICC6kiIVu44gvtFEQQWwsbExQJGHXmZtiZsOyzCN3Vz+4cDjfvec7j7l3QAF95onRZ54YKmdE1IbnS0c9mnAyAjkBxDy3LRHrjtRyu7OD52HntTAyvbw/HxP2hkCearrRb2WSCSuTTGi60S+QpzFhbwznDl/VVMHw0sF7hEjFbW2qkB38lfp8nNDipWcATil+uDM3cDWyeNRSijnfkHJnezb5Vkkgvbg3IOXD2e1ts93S+icnkZOAVaalZK3YQMa4L+pC6L1WduhYSeCf0PLBdxzOjZ93Lwvm6APAiLmlF1ubPiHotmaS41ExQjH0ZbfNM1NAFpgD0lVcICIrANqAVaAd+AFIYAy4BqaBG+Wsq5AH3vgk8xpYrzf4KLAZwhe8PYEIvQe4vc6H8Hnc2dQs0AFchvAXQGdEDF8s4A5TZS34BQqqQNaS1WMI3KD4WUbNoBJfce9CO7BSr4BfBe8A21vmUwh0VdjdTyHwscL+UK+AHxoD7FDoAX6/Cnpxn4ay/egCjcCL/w1chkqLakLQ/6ABhT57uAd+Vzv/Ara3iY6fK4WxAAAAAElFTkSuQmCC) no-repeat"
  "}"
};

/**< Image icon for inline expansion, the trash mark. */
template<typename T>
const char AutoConnectCore<T>::_CSS_ICON_TRASH[] PROGMEM = {
  ".img-trash{"
    "width:22px;"
    "height:22px;"
    "margin-top:14px;"
    "float:right;"
    "cursor:pointer;"
    "background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABYAAAAWCAYAAADEtGw7AAABhGlDQ1BJQ0MgcHJvZmlsZQAAKJF9kT1Iw0AcxV/TSkUqDnYQcQhSnSyIijpKFYtgobQVWnUwufQLmjQkKS6OgmvBwY/FqoOLs64OroIg+AHi6OSk6CIl/i8ptIjx4Lgf7+497t4BQqPCVDMwDqiaZaTiMTGbWxWDrxAQRAAzGJaYqSfSixl4jq97+Ph6F+VZ3uf+HL1K3mSATySeY7phEW8QT29aOud94jArSQrxOfGYQRckfuS67PIb56LDAs8MG5nUPHGYWCx2sNzBrGSoxFPEEUXVKF/Iuqxw3uKsVmqsdU/+wlBeW0lzneYQ4lhCAkmIkFFDGRVYiNKqkWIiRfsxD/+g40+SSyZXGYwcC6hCheT4wf/gd7dmYXLCTQrFgK4X2/4YAYK7QLNu29/Htt08AfzPwJXW9lcbwOwn6fW2FjkC+raBi+u2Ju8BlzvAwJMuGZIj+WkKhQLwfkbflAP6b4GeNbe31j5OH4AMdbV8AxwcAqNFyl73eHd3Z2//nmn19wONxHKyvZQ4mgAAAAZiS0dEAP8A/wD/oL2nkwAAAAlwSFlzAAAN1wAADdcBQiibeAAAALdJREFUOMvt1KEKAkEQBuBPMWkzWsRosWg2WEw2wTfQ6LNYfROLwSxWo81o02LQMsJxcJ4KguD9MOzMv8M/u8PsUqDAf2CMM25h5+CeopSzX8MBa6yCG2KAVhR5S3iDKuohsMcl9qpoR8FT8P20QCVDeIc5ljhm5DQwxeKdvnajn91ELOWnc146cbrIFr2Ik34myt8apUL4N4T3mMWa9J8ib9xG6OCKSXAPv/nJTUrxrG85tsn6Fu6nuijCLc4LEwAAAABJRU5ErkJggg==) no-repeat"
  "}"
};

/**< INPUT button and submit style */
template<typename T>
const char AutoConnectCore<T>::_CSS_INPUT_BUTTON[] PROGMEM = {
  "input[type=\"button\"],input[type=\"submit\"],button[type=\"submit\"],button[type=\"button\"]{"
    "padding:8px 0.5em;"
    "font-weight:bold;"
    "letter-spacing:0.8px;"
    "color:#fff;"
    "border:1px solid;"
    "border-radius:2px;"
    "margin-top:12px;"
    "cursor:pointer"
  "}"
  "input[type=\"button\"],button[type=\"button\"]{"
    "background-color:#1b5e20;"
    "border-color:#1b5e20;"
    "width:15em"
  "}"
  ".aux-page input[type=\"button\"],.aux-page button[type=\"button\"]{"
    "cursor:pointer;"
    "font-weight:normal;"
    "padding:8px 14px;"
    "margin:12px;"
    "width:auto"
  "}"
  "#sb[type=\"submit\"]{"
    "padding:8px 0;"
    "width:13em"
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
template<typename T>
const char AutoConnectCore<T>::_CSS_INPUT_TEXT[] PROGMEM = {
  "input[type=\"text\"],input[type=\"password\"],input[type=\"number\"],.aux-page select{"
    "background-color:#fff;"
    "border:1px solid #ccc;"
    "border-radius:2px;"
    "color:#444;"
    "margin:8px 0 8px 0;"
    "padding:10px"
  "}"
  "input[type=\"text\"],input[type=\"password\"],input[type=\"number\"]{"
    "font-weight:300;"
    "width:auto;"
    "-webkit-transition:all 0.20s ease-in;"
    "-moz-transition:all 0.20s ease-in;"
    "-o-transition:all 0.20s ease-in;"
    "-ms-transition:all 0.20s ease-in;"
    "transition:all 0.20s ease-in"
  "}"
  "input[type=\"text\"]:focus,input[type=\"password\"]:focus,input[type=\"number\"]:focus{"
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
    "cursor:pointer"
  "}"
};

/**< TABLE style */
template<typename T>
const char AutoConnectCore<T>::_CSS_TABLE[] PROGMEM = {
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
template<typename T>
const char AutoConnectCore<T>::_CSS_SPINNER[] PROGMEM = {
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
template<typename T>
const char AutoConnectCore<T>::_CSS_LUXBAR_BODY[] PROGMEM = {
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
};

template<typename T>
const char AutoConnectCore<T>::_CSS_LUXBAR_HEADER[] PROGMEM = {
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
    "padding:18px 3px 18px 24px"
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
};

template<typename T>
const char AutoConnectCore<T>::_CSS_LUXBAR_BGR[] PROGMEM = {
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
};

template<typename T>
const char AutoConnectCore<T>::_CSS_LUXBAR_ANI[] PROGMEM = {
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
};

template<typename T>
const char AutoConnectCore<T>::_CSS_LUXBAR_MEDIA[] PROGMEM = {
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
};

template<typename T>
const char AutoConnectCore<T>::_CSS_LUXBAR_ITEM[] PROGMEM = {
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
template<typename T>
const char AutoConnectCore<T>::_ELM_HTML_HEAD[] PROGMEM = {
  "<!DOCTYPE html>"
  "<html>"
  "<head>"
  "<meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
};

/**< LuxBar menu element. */
template<typename T>
const char  AutoConnectCore<T>::_ELM_MENU_PRE[] PROGMEM = {
  "<header id=\"lb\" class=\"lb-fixed\">"
    "<input type=\"checkbox\" class=\"lb-cb\" id=\"lb-cb\"/>"
    "<div class=\"lb-menu lb-menu-right lb-menu-material\">"
      "<ul class=\"lb-navigation\">"
        "<li class=\"lb-header\">"
          "<a href=\"BOOT_URI\" class=\"lb-brand\">MENU_TITLE</a>"
#ifdef AC_SHOW_PORTALIDENTIFIER
          "PORTAL"
#endif
          "<label class=\"lb-burger lb-burger-dblspin\" id=\"lb-burger\" for=\"lb-cb\"><span></span></label>"
        "</li>"
};

template<typename T>
const char  AutoConnectCore<T>::_ELM_MENU_POST[] PROGMEM = {
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
template<typename T>
const char  AutoConnectCore<T>::_PAGE_404[] PROGMEM = {
  "{{HEAD}}"
    "<title>" AUTOCONNECT_PAGETITLE_NOTFOUND "</title>"
  "</head>"
  "<body>"
    "404 Not found"
  "</body>"
  "</html>"
};

/**< The page that started the reset. */
template<typename T>
const char  AutoConnectCore<T>::_PAGE_RESETTING[] PROGMEM = {
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
template<typename T>
const char  AutoConnectCore<T>::_PAGE_STAT[] PROGMEM = {
  "{{HEAD}}"
    "<title>" AUTOCONNECT_PAGETITLE_STATISTICS "</title>"
    "<style type=\"text/css\">"
      "{{CSS_BASE}}"
      "{{CSS_TABLE}}"
      "{{CSS_LUXBAR_BODY}}"
      "{{CSS_LUXBAR_HEADER}}"
      "{{CSS_LUXBAR_BGR}}"
      "{{CSS_LUXBAR_ANI}}"
      "{{CSS_LUXBAR_MEDIA}}"
      "{{CSS_LUXBAR_ITEM}}"
    "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
    "<div class=\"container\">"
      "{{MENU_PRE}}"
      "{{MENU_LIST}}"
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
          "<tr>"
          "<td>" AUTOCONNECT_PAGESTATS_SYSTEM_UPTIME "</td>"
          "<td>{{SYSTEM_UPTIME}}</td>"
          "</tr>"
          "</tbody>"
        "</table>"
      "</div>"
    "</div>"
  "</body>"
  "</html>"
};

/**< A page that specifies the new configuration. */
template<typename T>
const char  AutoConnectCore<T>::_PAGE_CONFIGNEW[] PROGMEM = {
  "{{HEAD}}"
    "<title>" AUTOCONNECT_PAGETITLE_CONFIG "</title>"
    "<style type=\"text/css\">"
      "{{CSS_BASE}}"
      "{{CSS_ICON_LOCK}}"
      "{{CSS_UL}}"
      "{{CSS_INPUT_BUTTON}}"
      "{{CSS_INPUT_TEXT}}"
      "{{CSS_LUXBAR_BODY}}"
      "{{CSS_LUXBAR_HEADER}}"
      "{{CSS_LUXBAR_BGR}}"
      "{{CSS_LUXBAR_ANI}}"
      "{{CSS_LUXBAR_MEDIA}}"
      "{{CSS_LUXBAR_ITEM}}"
    "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
    "<div class=\"container\">"
      "{{MENU_PRE}}"
      "{{MENU_LIST}}"
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
template<typename T>
const char  AutoConnectCore<T>::_PAGE_OPENCREDT[] PROGMEM = {
  "{{HEAD}}"
    "<title>" AUTOCONNECT_PAGETITLE_CREDENTIALS "</title>"
    "<style type=\"text/css\">"
      "{{CSS_BASE}}"
      "{{CSS_ICON_LOCK}}"
      "{{CSS_ICON_TRASH}}"
      "{{CSS_INPUT_BUTTON}}"
      "{{CSS_LUXBAR_BODY}}"
      "{{CSS_LUXBAR_HEADER}}"
      "{{CSS_LUXBAR_BGR}}"
      "{{CSS_LUXBAR_ANI}}"
      "{{CSS_LUXBAR_MEDIA}}"
      "{{CSS_LUXBAR_ITEM}}"
    "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
    "<div class=\"container\">"
      "{{MENU_PRE}}"
      "{{MENU_LIST}}"
      "{{MENU_AUX}}"
      "{{MENU_POST}}"
      "<div class=\"base-panel\">"
        "<form id=\"_sid\" action=\"" AUTOCONNECT_URI_CONNECT "\" method=\"post\">"
          "{{OPEN_SSID}}"
        "</form>"
      "</div>"
    "</div>"
    "<script type=\"text/javascript\">"
      "function crdel(e){if(confirm(`${e} " AUTOCONNECT_TEXT_DELETECREDENTIAL "`)){var t=document.getElementById('_sid');t.setAttribute('action','" AUTOCONNECT_URI_DELETE "');var i=document.createElement('input');i.setAttribute('type','hidden'),i.setAttribute('name','del'),i.setAttribute('value',e),t.appendChild(i),t.submit()}}"
    "</script>"
  "</body>"
  "</html>"
};

/**< A page that informs during a connection attempting. */
template<typename T>
const char  AutoConnectCore<T>::_PAGE_CONNECTING[] PROGMEM = {
  "{{REQ}}"
  "{{HEAD}}"
    "<title>" AUTOCONNECT_PAGETITLE_CONNECTING "</title>"
    "<style type=\"text/css\">"
      "{{CSS_BASE}}"
      "{{CSS_SPINNER}}"
      "{{CSS_LUXBAR_BODY}}"
      "{{CSS_LUXBAR_HEADER}}"
      "{{CSS_LUXBAR_BGR}}"
      "{{CSS_LUXBAR_ANI}}"
      "{{CSS_LUXBAR_MEDIA}}"
      "{{CSS_LUXBAR_ITEM}}"
    "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
    "<div class=\"container\">"
      "{{MENU_PRE}}"
      "{{MENU_LIST}}"
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
template<typename T>
const char  AutoConnectCore<T>::_PAGE_SUCCESS[] PROGMEM = {
  "{{HEAD}}"
    "<title>" AUTOCONNECT_PAGETITLE_STATISTICS "</title>"
    "<style type=\"text/css\">"
      "{{CSS_BASE}}"
      "{{CSS_TABLE}}"
      "{{CSS_LUXBAR_BODY}}"
      "{{CSS_LUXBAR_HEADER}}"
      "{{CSS_LUXBAR_BGR}}"
      "{{CSS_LUXBAR_ANI}}"
      "{{CSS_LUXBAR_MEDIA}}"
      "{{CSS_LUXBAR_ITEM}}"
    "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
    "<div class=\"container\">"
      "{{MENU_PRE}}"
      "{{MENU_LIST}}"
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
template<typename T>
const char  AutoConnectCore<T>::_PAGE_FAIL[] PROGMEM = {
  "{{HEAD}}"
    "<title>" AUTOCONNECT_PAGETITLE_CONNECTIONFAILED "</title>"
    "<style type=\"text/css\">"
      "{{CSS_BASE}}"
      "{{CSS_TABLE}}"
      "{{CSS_LUXBAR_BODY}}"
      "{{CSS_LUXBAR_HEADER}}"
      "{{CSS_LUXBAR_BGR}}"
      "{{CSS_LUXBAR_ANI}}"
      "{{CSS_LUXBAR_MEDIA}}"
      "{{CSS_LUXBAR_ITEM}}"
    "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
    "<div class=\"container\">"
      "{{MENU_PRE}}"
      "{{MENU_LIST}}"
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
template<typename T>
const char  AutoConnectCore<T>::_PAGE_DISCONN[] PROGMEM = {
  "{{DISCONNECT}}"
  "{{HEAD}}"
    "<title>" AUTOCONNECT_PAGETITLE_DISCONNECTED "</title>"
    "<style type=\"text/css\">"
      "{{CSS_BASE}}"
      "{{CSS_LUXBAR_BODY}}"
      "{{CSS_LUXBAR_HEADER}}"
      "{{CSS_LUXBAR_BGR}}"
      "{{CSS_LUXBAR_ANI}}"
      "{{CSS_LUXBAR_MEDIA}}"
      "{{CSS_LUXBAR_ITEM}}"
    "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
    "<div class=\"container\">"
      "{{MENU_PRE}}"
      "{{MENU_LIST}}"
      "{{MENU_POST}}"
    "</div>"
  "</body>"
  "</html>"
};

/**< Interface page for accepting credential deletion requests. */
template<typename T>
const char  AutoConnectCore<T>::_PAGE_DELREQ[] PROGMEM = {
  "{{DELREQ}}"
};

/**< Interface page that redirects to the connection result. */
template<typename T>
const char  AutoConnectCore<T>::_PAGE_RESULT[] PROGMEM = {
  "{{RESULT}}"
};

template<typename T>
uint32_t AutoConnectCore<T>::_getChipId() {
#if defined(ARDUINO_ARCH_ESP8266)
  return ESP.getChipId();
#elif defined(ARDUINO_ARCH_ESP32)
  uint64_t  chipId;
  chipId = ESP.getEfuseMac();
  return (uint32_t)(chipId >> 32);
#endif
}

template<typename T>
uint32_t AutoConnectCore<T>::_getFlashChipRealSize() {
#if defined(ARDUINO_ARCH_ESP8266)
  return ESP.getFlashChipRealSize();
#elif defined(ARDUINO_ARCH_ESP32)
  return (uint32_t)spi_flash_get_chip_size();
#endif
}

template <typename T>
String AutoConnectCore<T>::_getSystemUptime() {
#if defined(ARDUINO_ARCH_ESP8266)
  unsigned long millisecs = millis();
#elif defined(ARDUINO_ARCH_ESP32)
  long  millisecs = esp_timer_get_time() / 1000;
#endif
  int systemUpTimeM = static_cast<int>((millisecs / (1000 * 60)) % 60);
  int systemUpTimeH = static_cast<int>((millisecs / (1000 * 60 * 60)) % 24);
  int systemUpTimeD = static_cast<int>(millisecs / (1000 * 60 * 60 * 24));
  String  uptime = String(systemUpTimeM) + "m ";
  if (systemUpTimeH > 0)
    uptime += String(systemUpTimeH) + "h ";
  if (systemUpTimeD > 0)
    uptime += String(systemUpTimeD) + "d ";
  return uptime;
}

template <typename T>
String AutoConnectCore<T>::_token_CSS_BASE(PageArgument &args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_BASE));
}

template<typename T>
String AutoConnectCore<T>::_token_CSS_ICON_LOCK(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_ICON_LOCK));
}

template<typename T>
String AutoConnectCore<T>::_token_CSS_ICON_TRASH(PageArgument& args) {
  AC_UNUSED(args);
  return (_apConfig.menuItems & AC_MENUITEM_DELETESSID) ? String(FPSTR(_CSS_ICON_TRASH)) : String("");
}

template<typename T>
String AutoConnectCore<T>::_token_CSS_INPUT_BUTTON(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_INPUT_BUTTON));
}

template<typename T>
String AutoConnectCore<T>::_token_CSS_INPUT_TEXT(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_INPUT_TEXT));
}

template<typename T>
String AutoConnectCore<T>::_token_CSS_LUXBAR_BODY(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_LUXBAR_BODY));
}

template<typename T>
String AutoConnectCore<T>::_token_CSS_LUXBAR_HEADER(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_LUXBAR_HEADER));
}

template<typename T>
String AutoConnectCore<T>::_token_CSS_LUXBAR_BGR(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_LUXBAR_BGR));
}

template<typename T>
String AutoConnectCore<T>::_token_CSS_LUXBAR_ANI(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_LUXBAR_ANI));
}

template<typename T>
String AutoConnectCore<T>::_token_CSS_LUXBAR_MEDIA(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_LUXBAR_MEDIA));
}

template<typename T>
String AutoConnectCore<T>::_token_CSS_LUXBAR_ITEM(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_LUXBAR_ITEM));
}

template<typename T>
String AutoConnectCore<T>::_token_CSS_SPINNER(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_SPINNER));
}

template<typename T>
String AutoConnectCore<T>::_token_CSS_TABLE(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_TABLE));
}

template<typename T>
String AutoConnectCore<T>::_token_CSS_UL(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_CSS_UL));
}

template<typename T>
String AutoConnectCore<T>::_token_MENU_AUX(PageArgument& args) {
  return _mold_MENU_AUX(args);
}

template<typename T>
String AutoConnectCore<T>::_token_MENU_PRE(PageArgument& args) {
  String  currentMenu = FPSTR(_ELM_MENU_PRE);
  currentMenu.replace(F("BOOT_URI"), _getBootUri());
  currentMenu.replace(F("MENU_TITLE"), _menuTitle);
#ifdef AC_SHOW_PORTALIDENTIFIER
  currentMenu.replace(F("PORTAL"), isPortalAvailable() ? String(F(AUTOCONNECT_PORTAL_LINK)) : String());
#endif
  return currentMenu;
}

template<typename T>
String AutoConnectCore<T>::_token_MENU_LIST(PageArgument& args) {
  String  menuItems = _attachMenuItem(AC_MENUITEM_CONFIGNEW) +
                      _attachMenuItem(AC_MENUITEM_OPENSSIDS) +
                      _attachMenuItem(AC_MENUITEM_DISCONNECT) +
                      _attachMenuItem(AC_MENUITEM_RESET);
  if (menuItems.indexOf(F("{{CUR_SSID}}")))
    menuItems.replace(F("{{CUR_SSID}}"), _token_ESTAB_SSID(args));
  return menuItems;
}

template<typename T>
String AutoConnectCore<T>::_token_MENU_POST(PageArgument& args) {
  AC_UNUSED(args);
  String  postMenu = FPSTR(_ELM_MENU_POST);
  postMenu.replace(F("MENU_HOME"), _attachMenuItem(AC_MENUITEM_HOME));
  postMenu.replace(F("HOME_URI"), _apConfig.homeUri);
  postMenu.replace(F("MENU_DEVINFO"), _attachMenuItem(AC_MENUITEM_DEVINFO));
  return postMenu;
}

template<typename T>
String AutoConnectCore<T>::_token_AP_MAC(PageArgument& args) {
  AC_UNUSED(args);
  uint8_t macAddress[6];
  WiFi.softAPmacAddress(macAddress);
  return AutoConnectCore<T>::_toMACAddressString(macAddress);
}

template<typename T>
String AutoConnectCore<T>::_token_BOOTURI(PageArgument& args) {
  AC_UNUSED(args);
  return _getBootUri();
}

template<typename T>
String AutoConnectCore<T>::_token_CHANNEL(PageArgument& args) {
  AC_UNUSED(args);
  return String(WiFi.channel());
}

template<typename T>
String AutoConnectCore<T>::_token_CHIP_ID(PageArgument& args) {
  AC_UNUSED(args);
  return String(_getChipId());
}

template<typename T>
String AutoConnectCore<T>::_token_CONFIG_STAIP(PageArgument& args) {
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
    { PSTR(AUTOCONNECT_PARAMID_STAIP), PSTR(AUTOCONNECT_PAGECONFIG_IPADDRESS) },
    { PSTR(AUTOCONNECT_PARAMID_GTWAY), PSTR(AUTOCONNECT_PAGECONFIG_GATEWAY) },
    { PSTR(AUTOCONNECT_PARAMID_NTMSK), PSTR(AUTOCONNECT_PAGECONFIG_NETMASK) },
    { PSTR(AUTOCONNECT_PARAMID_DNS1), PSTR(AUTOCONNECT_PAGECONFIG_DNS1) },
    { PSTR(AUTOCONNECT_PARAMID_DNS2), PSTR(AUTOCONNECT_PAGECONFIG_DNS2) }
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

template<typename T>
String AutoConnectCore<T>::_token_CPU_FREQ(PageArgument& args) {
  AC_UNUSED(args);
  return String(ESP.getCpuFreqMHz());
}

template<typename T>
String AutoConnectCore<T>::_token_CURRENT_SSID(PageArgument& args) {
  AC_UNUSED(args);
  char  ssid_c[sizeof(station_config_t::ssid) + 1];
  *ssid_c = '\0';
  strncat(ssid_c, reinterpret_cast<char*>(_credential.ssid), sizeof(ssid_c) - 1);
  String  ssid = String(ssid_c);
  return ssid;
}

template<typename T>
String AutoConnectCore<T>::_token_DBM(PageArgument& args) {
  AC_UNUSED(args);
  int32_t dBm = WiFi.RSSI();
  return (dBm == 31 ? String(F("N/A")) : String(dBm));
}

template<typename T>
String AutoConnectCore<T>::_token_ESTAB_SSID(PageArgument& args) {
  AC_UNUSED(args);
  return (WiFi.status() == WL_CONNECTED ? WiFi.SSID() : String(F("N/A")));
}

template<typename T>
String AutoConnectCore<T>::_token_FLASH_SIZE(PageArgument& args) {
  AC_UNUSED(args);
  return String(_getFlashChipRealSize());
}

template<typename T>
String AutoConnectCore<T>::_token_FREE_HEAP(PageArgument& args) {
  AC_UNUSED(args);
  return String(_freeHeapSize);
}

template <typename T>
String AutoConnectCore<T>::_token_GATEWAY(PageArgument &args) {
  AC_UNUSED(args);
  return WiFi.gatewayIP().toString();
}

template<typename T>
String AutoConnectCore<T>::_token_HEAD(PageArgument& args) {
  AC_UNUSED(args);
  return String(FPSTR(_ELM_HTML_HEAD));
}

template<typename T>
String AutoConnectCore<T>::_token_HIDDEN_COUNT(PageArgument& args) {
  AC_UNUSED(args);
  return String(_hiddenSSIDCount);
}

template<typename T>
String AutoConnectCore<T>::_token_LIST_SSID(PageArgument& args) {
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
          snprintf_P(slBuf, bufSize - (slBuf - ssidList), (PGM_P)_ssidList, ssid.c_str(), AutoConnectCore<T>::_toWiFiQuality((int32_t)WiFi.RSSI(i)), WiFi.channel(i), WiFi.encryptionType(i) != ENC_TYPE_NONE ? (PGM_P)_ssidEnc : "");
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
    snprintf_P(slBuf, bufSize - (slBuf - ssidList), (PGM_P)_ssidPage, page - 1, PSTR(AUTOCONNECT_PAGECONFIG_PREVIOUS));
    slBuf = ssidList + strlen(ssidList);
  }
  // Prepare next button
  if (validCount > (page + 1) * AUTOCONNECT_SSIDPAGEUNIT_LINES) {
    snprintf_P(slBuf, bufSize - (slBuf - ssidList), (PGM_P)_ssidPage, page + 1, PSTR(AUTOCONNECT_PAGECONFIG_NEXT));
  }
  String ssidListStr = String(ssidList);
  free(ssidList);
  return ssidListStr;
}

template<typename T>
String AutoConnectCore<T>::_token_LOCAL_IP(PageArgument& args) {
  AC_UNUSED(args);
  return WiFi.localIP().toString();
}

template<typename T>
String AutoConnectCore<T>::_token_NETMASK(PageArgument& args) {
  AC_UNUSED(args);
  return WiFi.subnetMask().toString();
}

template<typename T>
String AutoConnectCore<T>::_token_OPEN_SSID(PageArgument& args) {
  AC_UNUSED(args);
  static const char _ssidUndeleted[] PROGMEM = "<div style=\"color:red\">%s " AUTOCONNECT_TEXT_COULDNOTDELETED "</div>";
  static const char _ssidList[] PROGMEM = "<input id=\"sb\" type=\"submit\" name=\"%s\" value=\"%s\"><label class=\"slist\">%s</label>%s%s<br>";
  static const char _ssidRssi[] PROGMEM = "%d&#037;&ensp;Ch.%d";
  static const char _ssidNA[]   PROGMEM = "N/A";
  static const char _ssidTrsh[] PROGMEM = "<a onclick=\"crdel('%s')\" class=\"img-trash\"></a>";
  static const char _ssidLock[] PROGMEM = "<span class=\"img-lock\"></span>";
  static const char _ssidNull[] PROGMEM = "";
  String ssidList;
  station_config_t  entry;
  char  rssiCont[32];
  char  trash[80] = {'\0'};
  char  slCont[sizeof(_ssidList) + sizeof(AUTOCONNECT_PARAMID_CRED) + sizeof(station_config_t::ssid) + sizeof(rssiCont) + sizeof(trash) + sizeof(_ssidLock)];
  AutoConnectCredential credit(_apConfig.boundaryOffset);

  if (_indelibleSSID.length()) {
    char  indelible[82];

    snprintf_P(indelible, sizeof(indelible), _ssidUndeleted, _indelibleSSID.c_str());
    ssidList = String(indelible);
    _indelibleSSID.clear();
  }

  uint8_t creEntries = credit.entries();
  if (creEntries > 0)
    _scanCount = WiFi.scanNetworks(false, true);
  else
    ssidList += String(F("<p><b>" AUTOCONNECT_TEXT_NOSAVEDCREDENTIALS "</b></p>"));

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
        snprintf_P(rssiCont, sizeof(rssiCont), (PGM_P)_ssidRssi, AutoConnectCore<T>::_toWiFiQuality((int32_t)WiFi.RSSI(sc)), _connectCh);
        rssiSym = rssiCont;
        if (WiFi.encryptionType(sc) != ENC_TYPE_NONE)
          ssidLock = _ssidLock;
        break;
      }
    }
    if (_apConfig.menuItems & AC_MENUITEM_DELETESSID)
      snprintf_P(trash, sizeof(trash), (PGM_P)_ssidTrsh, reinterpret_cast<char*>(entry.ssid));

    snprintf_P(slCont, sizeof(slCont), (PGM_P)_ssidList, AUTOCONNECT_PARAMID_CRED, reinterpret_cast<char*>(entry.ssid), rssiSym, trash, ssidLock);
    ssidList += String(slCont);
  }
  return ssidList;
}

template<typename T>
String AutoConnectCore<T>::_token_SOFTAP_IP(PageArgument& args) {
  AC_UNUSED(args);
  return WiFi.softAPIP().toString();
}

template<typename T>
String AutoConnectCore<T>::_token_SSID_COUNT(PageArgument& args) {
  AC_UNUSED(args);
  return String(_scanCount);
}

template<typename T>
String AutoConnectCore<T>::_token_STA_MAC(PageArgument& args) {
  AC_UNUSED(args);
  uint8_t macAddress[6];
  WiFi.macAddress(macAddress);
  return AutoConnectCore<T>::_toMACAddressString(macAddress);
}

template<typename T>
String AutoConnectCore<T>::_token_STATION_STATUS(PageArgument& args) {
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

template <typename T>
String AutoConnectCore<T>::_token_SYSTEM_UPTIME(PageArgument &args) {
  AC_UNUSED(args);
  return _getSystemUptime();
}

template<typename T>
String AutoConnectCore<T>::_token_UPTIME(PageArgument& args) {
  AC_UNUSED(args);
  return String(_apConfig.uptime);
}

template<typename T>
String AutoConnectCore<T>::_token_WIFI_MODE(PageArgument& args) {
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

template<typename T>
String AutoConnectCore<T>::_token_WIFI_STATUS(PageArgument& args) {
  AC_UNUSED(args);
  return String(WiFi.status());
}

/**
 *  Generate AutoConnect menu item configured by AutoConnectConfig::attachMenu.
 *  @param  item  An enumerated value for the generating item configured in AutoConnectConfig.
 *  @return HTML string of a li tag with the menu item.
 */
template<typename T>
String AutoConnectCore<T>::_attachMenuItem(const AC_MENUITEM_t item) {
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
template<typename T>
PageElement* AutoConnectCore<T>::_setupPage(String& uri) {
  // PageElement *elm = new PageElement();
  PageElement *elm = nullptr;
  bool  reqAuth = false;

  // Restore menu title
  _menuTitle = _apConfig.title;

  // // Build the elements of current requested page.
  for (auto& pageConst : _pageConst) {
    if (!strcmp(pageConst.uri, uri.c_str())) {
      _freeHeapSize = ESP.getFreeHeap();
      if (_apConfig.menuItems & pageConst.appMenus) {
        if (pageConst.menuTitle)
          _menuTitle = pageConst.menuTitle;
        reqAuth = pageConst.authRequired;
        elm = new PageElement();
        elm->setMold(FPSTR(pageConst.mold));
        for (auto& token : pageConst.tokenId) {
          PGM_P embeddedToken;
          std::find_if(std::begin(_tokenStringify), std::end(_tokenStringify), [&](const AC_PAGETOKENSTRING_t& pred) {
            if (token == pred.tokenId) {
              embeddedToken = pred.token;
              return true;
            }
            return false;
          });
          HandleFuncT handler;
          std::find_if(_tokenRegistry.begin(), _tokenRegistry.end(), [&](const AC_PAGETOKENREGISTRY_t& pred) {
            if (token == pred.tokenId) {
              handler = pred.handler;
              return true;
            }
            return false;
          });
          elm->addToken(FPSTR(embeddedToken), handler);
        }
      }
    }
  }

  // Regiter authentication
  // Determine the necessity of authentication from the AutoConnectConfig settings
  if (elm) {
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
template<typename T>
void AutoConnectCore<T>::_authentication(bool allow) {
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
template<typename T>
void AutoConnectCore<T>::_authentication(bool allow, const HTTPAuthMethod method) {
  const char* user = nullptr;
  const char* password = nullptr;
  String  fails;

  // Enable authentication by setting of AC_AUTHSCOPE_DISCONNECTED even if WiFi is not connected.
  String  accUrl = _webServer->hostHeader();

  if (WiFi.status() != WL_CONNECTED)
    allow = _apConfig.authScope & AC_AUTHSCOPE_WITHCP;
  else if ((WiFi.getMode() & WIFI_AP) && ((accUrl == WiFi.softAPIP().toString()) || accUrl.endsWith(F(".local"))))
    allow = _apConfig.authScope & AC_AUTHSCOPE_WITHCP;

  if (allow) {
    // Regiter authentication method
    user = _apConfig.username.length() ? _apConfig.username.c_str() : _apConfig.apid.c_str();
    password = _apConfig.password.length() ? _apConfig.password.c_str() : _apConfig.psk.c_str();
    fails = String(FPSTR(AutoConnectCore<T>::_ELM_HTML_HEAD)) + String(F("</head><body>" AUTOCONNECT_TEXT_AUTHFAILED "</body></html>"));
    AC_DBG_DUMB(",%s+%s/%s", method == HTTPAuthMethod::BASIC_AUTH ? "BASIC" : "DIGEST", user, password);
  }
  _responsePage->authentication(user, password, method, AUTOCONNECT_AUTH_REALM, fails);
}
