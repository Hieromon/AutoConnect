/**
 * AutoConnectConfigAux class implementation.
 * @file AutoConnectConfigAux.cpp
 * @author hieromon@gmail.com
 * @version  1.3.0
 * @date 2021-09-13
 * @copyright  MIT license.
 */

#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>
#include "AutoConnectConfigAux.h"

#ifdef AUTOCONNECT_USE_CONFIGAUX

#include "AutoConnectFS.h"
#if defined(ARDUINO_ARCH_ESP32) && (AC_USE_FILESYSTEM == 1)
extern "C" {
#include <esp_spiffs.h>
}
#endif

// Name definitions of the AutoConnectElements placed on the AUX page
#define AUTOCONNECT_CONFIGAUX_ELM_APGATEWAY       "agw"
#define AUTOCONNECT_CONFIGAUX_ELM_APIP            "aip"
#define AUTOCONNECT_CONFIGAUX_ELM_APNETMASK       "anm"
#define AUTOCONNECT_CONFIGAUX_ELM_AUTH            "ath"
#define AUTOCONNECT_CONFIGAUX_ELM_AUTHSCOPE       "asc"
#define AUTOCONNECT_CONFIGAUX_ELM_AUTORECONNECT   "rcn"
#define AUTOCONNECT_CONFIGAUX_ELM_AUTORESTART     "rdc"
#define AUTOCONNECT_CONFIGAUX_ELM_AUTORISE        "ris"
#define AUTOCONNECT_CONFIGAUX_ELM_AUTOSAVE        "sav"
#define AUTOCONNECT_CONFIGAUX_ELM_BEGINTIMEOUT    "bto"
#define AUTOCONNECT_CONFIGAUX_ELM_BOOTURI         "rot"
#define AUTOCONNECT_CONFIGAUX_ELM_BOUNDARYOFFSET  "bdy"
#define AUTOCONNECT_CONFIGAUX_ELM_BUILTINOTA      "ota"
#define AUTOCONNECT_CONFIGAUX_ELM_CHANNEL         "chl"
#define AUTOCONNECT_CONFIGAUX_ELM_ENABLEDHCP      "dhc"
#define AUTOCONNECT_CONFIGAUX_ELM_HIDDEN          "hdn"
#define AUTOCONNECT_CONFIGAUX_ELM_HOMEURI         "uri"
#define AUTOCONNECT_CONFIGAUX_ELM_HOSTNAME        "hnm"
#define AUTOCONNECT_CONFIGAUX_ELM_IMMEDIATESTART  "ims"
#define AUTOCONNECT_CONFIGAUX_ELM_MENU            "mnu"
#define AUTOCONNECT_CONFIGAUX_ELM_MENUCONFIGNEW   "cfg"
#define AUTOCONNECT_CONFIGAUX_ELM_MENUDISCONNECT  "dcn"
#define AUTOCONNECT_CONFIGAUX_ELM_MENUHOME        "hom"
#define AUTOCONNECT_CONFIGAUX_ELM_MENUOPENSSIDS   "opn"
#define AUTOCONNECT_CONFIGAUX_ELM_MENURESET       "rst"
#define AUTOCONNECT_CONFIGAUX_ELM_MENUTITLE       "ttl"
#define AUTOCONNECT_CONFIGAUX_ELM_MENUUPDATE      "upd"
#define AUTOCONNECT_CONFIGAUX_ELM_MINRSSI         "min"
#define AUTOCONNECT_CONFIGAUX_ELM_PASSWORD        "pas"
#define AUTOCONNECT_CONFIGAUX_ELM_PORTALTIMEOUT   "cto"
#define AUTOCONNECT_CONFIGAUX_ELM_PRESERVEAPMODE  "pre"
#define AUTOCONNECT_CONFIGAUX_ELM_PRIMARYDNS      "ns1"
#define AUTOCONNECT_CONFIGAUX_ELM_PRINCIPLE       "pri"
#define AUTOCONNECT_CONFIGAUX_ELM_PSK             "psk"
#define AUTOCONNECT_CONFIGAUX_ELM_RECONNECTINT    "int"
#define AUTOCONNECT_CONFIGAUX_ELM_RETAINPORTAL    "rtn"
#define AUTOCONNECT_CONFIGAUX_ELM_SECONDARYDNS    "ns2"
#define AUTOCONNECT_CONFIGAUX_ELM_SSID            "sid"
#define AUTOCONNECT_CONFIGAUX_ELM_STAGATEWAY      "sgw"
#define AUTOCONNECT_CONFIGAUX_ELM_STAIP           "sip"
#define AUTOCONNECT_CONFIGAUX_ELM_STANETMASK      "snm"
#define AUTOCONNECT_CONFIGAUX_ELM_TICKER          "tik"
#define AUTOCONNECT_CONFIGAUX_ELM_TICKERON        "ton"
#define AUTOCONNECT_CONFIGAUX_ELM_TICKERPORT      "tio"
#define AUTOCONNECT_CONFIGAUX_ELM_USER            "usr"

const char AutoConnectConfigAux::_ELM_APGATEWAY[] PROGMEM       = AUTOCONNECT_CONFIGAUX_ELM_APGATEWAY;
const char AutoConnectConfigAux::_ELM_APIP[] PROGMEM            = AUTOCONNECT_CONFIGAUX_ELM_APIP;
const char AutoConnectConfigAux::_ELM_APNETMASK[] PROGMEM       = AUTOCONNECT_CONFIGAUX_ELM_APNETMASK;
const char AutoConnectConfigAux::_ELM_AUTH[] PROGMEM            = AUTOCONNECT_CONFIGAUX_ELM_AUTH;
const char AutoConnectConfigAux::_ELM_AUTHSCOPE[] PROGMEM       = AUTOCONNECT_CONFIGAUX_ELM_AUTHSCOPE;
const char AutoConnectConfigAux::_ELM_AUTORECONNECT[] PROGMEM   = AUTOCONNECT_CONFIGAUX_ELM_AUTORECONNECT;
const char AutoConnectConfigAux::_ELM_AUTORESTART[] PROGMEM     = AUTOCONNECT_CONFIGAUX_ELM_AUTORESTART;
const char AutoConnectConfigAux::_ELM_AUTORISE[] PROGMEM        = AUTOCONNECT_CONFIGAUX_ELM_AUTORISE;
const char AutoConnectConfigAux::_ELM_AUTOSAVE[] PROGMEM        = AUTOCONNECT_CONFIGAUX_ELM_AUTOSAVE;
const char AutoConnectConfigAux::_ELM_BEGINTIMEOUT[] PROGMEM    = AUTOCONNECT_CONFIGAUX_ELM_BEGINTIMEOUT;
const char AutoConnectConfigAux::_ELM_BOOTURI[] PROGMEM         = AUTOCONNECT_CONFIGAUX_ELM_BOOTURI;
const char AutoConnectConfigAux::_ELM_BOUNDARYOFFSET[] PROGMEM  = AUTOCONNECT_CONFIGAUX_ELM_BOUNDARYOFFSET;
const char AutoConnectConfigAux::_ELM_BUILTINOTA[] PROGMEM      = AUTOCONNECT_CONFIGAUX_ELM_BUILTINOTA;
const char AutoConnectConfigAux::_ELM_CHANNEL[] PROGMEM         = AUTOCONNECT_CONFIGAUX_ELM_CHANNEL;
const char AutoConnectConfigAux::_ELM_ENABLEDHCP[] PROGMEM      = AUTOCONNECT_CONFIGAUX_ELM_ENABLEDHCP;
const char AutoConnectConfigAux::_ELM_HIDDEN[] PROGMEM          = AUTOCONNECT_CONFIGAUX_ELM_HIDDEN;
const char AutoConnectConfigAux::_ELM_HOMEURI[] PROGMEM         = AUTOCONNECT_CONFIGAUX_ELM_HOMEURI;
const char AutoConnectConfigAux::_ELM_HOSTNAME[] PROGMEM        = AUTOCONNECT_CONFIGAUX_ELM_HOSTNAME;
const char AutoConnectConfigAux::_ELM_IMMEDIATESTART[] PROGMEM  = AUTOCONNECT_CONFIGAUX_ELM_IMMEDIATESTART;
const char AutoConnectConfigAux::_ELM_MENU[] PROGMEM            = AUTOCONNECT_CONFIGAUX_ELM_MENU;
const char AutoConnectConfigAux::_ELM_MENUCONFIGNEW[] PROGMEM   = AUTOCONNECT_CONFIGAUX_ELM_MENUCONFIGNEW;
const char AutoConnectConfigAux::_ELM_MENUDISCONNECT[] PROGMEM  = AUTOCONNECT_CONFIGAUX_ELM_MENUDISCONNECT;
const char AutoConnectConfigAux::_ELM_MENUHOME[] PROGMEM        = AUTOCONNECT_CONFIGAUX_ELM_MENUHOME;
const char AutoConnectConfigAux::_ELM_MENUOPENSSIDS[] PROGMEM   = AUTOCONNECT_CONFIGAUX_ELM_MENUOPENSSIDS;
const char AutoConnectConfigAux::_ELM_MENURESET[] PROGMEM       = AUTOCONNECT_CONFIGAUX_ELM_MENURESET;
const char AutoConnectConfigAux::_ELM_MENUTITLE[] PROGMEM       = AUTOCONNECT_CONFIGAUX_ELM_MENUTITLE;
const char AutoConnectConfigAux::_ELM_MENUUPDATE[] PROGMEM      = AUTOCONNECT_CONFIGAUX_ELM_MENUUPDATE;
const char AutoConnectConfigAux::_ELM_MINRSSI[] PROGMEM         = AUTOCONNECT_CONFIGAUX_ELM_MINRSSI;
const char AutoConnectConfigAux::_ELM_PASSWORD[] PROGMEM        = AUTOCONNECT_CONFIGAUX_ELM_PASSWORD;
const char AutoConnectConfigAux::_ELM_PORTALTIMEOUT[] PROGMEM   = AUTOCONNECT_CONFIGAUX_ELM_PORTALTIMEOUT;
const char AutoConnectConfigAux::_ELM_PRESERVEAPMODE[] PROGMEM  = AUTOCONNECT_CONFIGAUX_ELM_PRESERVEAPMODE;
const char AutoConnectConfigAux::_ELM_PRIMARYDNS[] PROGMEM      = AUTOCONNECT_CONFIGAUX_ELM_PRIMARYDNS;
const char AutoConnectConfigAux::_ELM_PRINCIPLE[] PROGMEM       = AUTOCONNECT_CONFIGAUX_ELM_PRINCIPLE;
const char AutoConnectConfigAux::_ELM_PSK[] PROGMEM             = AUTOCONNECT_CONFIGAUX_ELM_PSK;
const char AutoConnectConfigAux::_ELM_RECONNECTINT[] PROGMEM    = AUTOCONNECT_CONFIGAUX_ELM_RECONNECTINT;
const char AutoConnectConfigAux::_ELM_RETAINPORTAL[] PROGMEM    = AUTOCONNECT_CONFIGAUX_ELM_RETAINPORTAL;
const char AutoConnectConfigAux::_ELM_SECONDARYDNS[] PROGMEM    = AUTOCONNECT_CONFIGAUX_ELM_SECONDARYDNS;
const char AutoConnectConfigAux::_ELM_SSID[] PROGMEM            = AUTOCONNECT_CONFIGAUX_ELM_SSID;
const char AutoConnectConfigAux::_ELM_STAGATEWAY[] PROGMEM      = AUTOCONNECT_CONFIGAUX_ELM_STAGATEWAY;
const char AutoConnectConfigAux::_ELM_STAIP[] PROGMEM           = AUTOCONNECT_CONFIGAUX_ELM_STAIP;
const char AutoConnectConfigAux::_ELM_STANETMASK[] PROGMEM      = AUTOCONNECT_CONFIGAUX_ELM_STANETMASK;
const char AutoConnectConfigAux::_ELM_TICKER[] PROGMEM          = AUTOCONNECT_CONFIGAUX_ELM_TICKER;
const char AutoConnectConfigAux::_ELM_TICKERON[] PROGMEM        = AUTOCONNECT_CONFIGAUX_ELM_TICKERON;
const char AutoConnectConfigAux::_ELM_TICKERPORT[] PROGMEM      = AUTOCONNECT_CONFIGAUX_ELM_TICKERPORT;
const char AutoConnectConfigAux::_ELM_USER[] PROGMEM            = AUTOCONNECT_CONFIGAUX_ELM_USER;

const char* const AutoConnectConfigAux::_elmNames[] PROGMEM = {
  AutoConnectConfigAux::_ELM_APGATEWAY,
  AutoConnectConfigAux::_ELM_APIP,
  AutoConnectConfigAux::_ELM_APNETMASK,
  AutoConnectConfigAux::_ELM_AUTH,
  AutoConnectConfigAux::_ELM_AUTHSCOPE,
  AutoConnectConfigAux::_ELM_AUTORECONNECT,
  AutoConnectConfigAux::_ELM_AUTORESTART,
  AutoConnectConfigAux::_ELM_AUTORISE,
  AutoConnectConfigAux::_ELM_AUTOSAVE,
  AutoConnectConfigAux::_ELM_BEGINTIMEOUT,
  AutoConnectConfigAux::_ELM_BOOTURI,
  AutoConnectConfigAux::_ELM_BOUNDARYOFFSET,
  AutoConnectConfigAux::_ELM_BUILTINOTA,
  AutoConnectConfigAux::_ELM_CHANNEL,
  AutoConnectConfigAux::_ELM_ENABLEDHCP,
  AutoConnectConfigAux::_ELM_HIDDEN,
  AutoConnectConfigAux::_ELM_HOMEURI,
  AutoConnectConfigAux::_ELM_HOSTNAME,
  AutoConnectConfigAux::_ELM_IMMEDIATESTART,
  AutoConnectConfigAux::_ELM_MENU,
  AutoConnectConfigAux::_ELM_MENUCONFIGNEW,
  AutoConnectConfigAux::_ELM_MENUDISCONNECT,
  AutoConnectConfigAux::_ELM_MENUHOME,
  AutoConnectConfigAux::_ELM_MENUOPENSSIDS,
  AutoConnectConfigAux::_ELM_MENURESET,
  AutoConnectConfigAux::_ELM_MENUTITLE,
  AutoConnectConfigAux::_ELM_MENUUPDATE,
  AutoConnectConfigAux::_ELM_MINRSSI,
  AutoConnectConfigAux::_ELM_PASSWORD,
  AutoConnectConfigAux::_ELM_PORTALTIMEOUT,
  AutoConnectConfigAux::_ELM_PRESERVEAPMODE,
  AutoConnectConfigAux::_ELM_PRIMARYDNS,
  AutoConnectConfigAux::_ELM_PRINCIPLE,
  AutoConnectConfigAux::_ELM_PSK,
  AutoConnectConfigAux::_ELM_RECONNECTINT,
  AutoConnectConfigAux::_ELM_RETAINPORTAL,
  AutoConnectConfigAux::_ELM_SECONDARYDNS,
  AutoConnectConfigAux::_ELM_SSID,
  AutoConnectConfigAux::_ELM_STAGATEWAY,
  AutoConnectConfigAux::_ELM_STAIP,
  AutoConnectConfigAux::_ELM_STANETMASK,
  AutoConnectConfigAux::_ELM_TICKER,
  AutoConnectConfigAux::_ELM_TICKERON,
  AutoConnectConfigAux::_ELM_TICKERPORT,
  AutoConnectConfigAux::_ELM_USER
};

// AutoConnectConfigAux page definition
// The labels appearing in the definitions below are quoted from the
// literal definitions by AutoConnectLabels.h, which allows the sketch
// to change the display language on the page.
// Refer to https://hieromon.github.io/AutoConnect/changelabel.html for
// a detailed explanation of changing the label literals.
const char AutoConnectConfigAux::_ui[] PROGMEM = R"(
[
{
  "name": "cs",
  "type": "ACStyle",
  "value": ".noorder{width:100%}.shd{color:#494949;background:#cadffa;font-size:110%;font-weight:bold;margin:10px 0 7px 0;padding:7px 0 1px 7px;border-bottom:solid 2px #516ab6}.noorder label{display:inline-block;width:10.2em;cursor:pointer;padding:5px}.noorder input[type=\"text\"],.noorder input[type=\"number\"]{width:10em;margin:3px 0.3em 0 0;padding:3px}.tb input[type=\"checkbox\"]:checked~.tbc{display:none;}.tb input[type=\"checkbox\"]:not(:checked)~.tbc{display:inherit}.fll{float:left}.flr{list-style:none;display:flex;flex-wrap:wrap;position:relative}.flr input[type=\"radio\"]{margin-right:0}.flr label{width:auto}.flr input[type=\"radio\"]+label{margin-right:1em}.flr label+input[type=\"checkbox\"]{margin-right:1em;}"
},
{
  "name": "sep1",
  "type": "ACElement",
  "value": "<div class=\"shd\">)" AUTOCONNECT_PAGELABEL_SOFTAPSETTINGS R"(</div>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_SSID R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_SSID R"(",
  "placeholder": ")" AUTOCONNECT_APID R"("
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_PSK R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_PASSPHRASE R"(",
  "placeholder": ")" AUTOCONNECT_PSK R"("
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_CHANNEL R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGESTATS_CHANNEL R"(",
  "placeholder": ")" AUTOCONNECT_STRING_DEPLOY(AUTOCONNECT_AP_CH) R"(",
  "style": "width:3em",
  "apply": "number"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_HIDDEN R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_HIDDEN R"(",
  "label": ")" AUTOCONNECT_PAGECONFIG_HIDDENSSID R"(",
  "labelposition": "infront"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_APIP R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_IPADDRESS R"("
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_APGATEWAY R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_GATEWAY R"("
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_APNETMASK R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGESTATS_SUBNETMASK R"("
},
{
  "name": "sep2",
  "type": "ACElement",
  "value": "<div class=\"shd\">)" AUTOCONNECT_PAGELABEL_CPCONTROL R"(</div>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTORISE R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTORISE R"(",
  "label": ")" AUTOCONNECT_PAGECONFIG_AUTOPOPUP R"(",
  "labelposition": "infront"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_IMMEDIATESTART R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_IMMEDIATESTART R"(",
  "label": ")" AUTOCONNECT_PAGECONFIG_IMMEDIATESTART R"(",
  "labelposition": "infront"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_RETAINPORTAL R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_RETAINPORTAL R"(",
  "label": ")" AUTOCONNECT_PAGECONFIG_RETAINPORTAL R"(",
  "labelposition": "infront"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_PORTALTIMEOUT R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_CPTIMEOUT R"(",
  "placeholder": ")" AUTOCONNECT_STRING_DEPLOY(AUTOCONNECT_CAPTIVEPORTAL_TIMEOUT) R"(",
  "apply": "number",
  "style": "width:5em",
  "posterior": "none"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_PORTALTIMEOUT R"(C",
  "type": "ACText",
  "value": " [ms]",
  "posterior": "br"
},
{
  "name": "sep3",
  "type": "ACElement",
  "value": "<div class=\"shd\">)" AUTOCONNECT_PAGELABEL_CONNECTIONCONTROL R"(</div>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_PRINCIPLE R"(L",
  "type": "ACElement",
  "value": "<label class=\"fll\" for=\")" AUTOCONNECT_CONFIGAUX_ELM_PRINCIPLE R"(\">)" AUTOCONNECT_PAGECONFIG_PRINCIPLE R"(</label>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_PRINCIPLE R"(O",
  "type": "ACElement",
  "value": "<div class=\"flr\">"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_PRINCIPLE R"(",
  "type": "ACRadio",
  "value": [
    "RECENT",
    "RSSI"
  ],
  "posterior": "div",
  "arrange": "horizontal"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_PRINCIPLE R"(C",
  "type": "ACElement",
  "value": "</div>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_MINRSSI R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_MINRSSI R"(",
  "placeholder": ")" AUTOCONNECT_STRING_DEPLOY(AUTOCONNECT_MIN_RSSI) R"(",
  "apply": "number",
  "style": "width:5em"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_BEGINTIMEOUT R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_BEGINTIMEOUT R"(",
  "placeholder": ")" AUTOCONNECT_STRING_DEPLOY(AUTOCONNECT_TIMEOUT) R"(",
  "apply": "number",
  "style": "width:5em",
  "posterior": "none"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_BEGINTIMEOUT R"(C",
  "type": "ACText",
  "value": " [ms]",
  "posterior": "br"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTORECONNECT R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTORECONNECT R"(",
  "label": ")" AUTOCONNECT_PAGECONFIG_AUTORECONNECT R"(",
  "labelposition": "infront"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_RECONNECTINT R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_RECONNECTINTERVAL R"(",
  "apply": "number",
  "style": "width:3em",
  "posterior": "none"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_RECONNECTINT R"(C",
  "type": "ACText",
  "value": " x)" AUTOCONNECT_STRING_DEPLOY(AUTOCONNECT_UNITTIME) R"([s]",
  "posterior": "br"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTOSAVE R"(L",
  "type": "ACElement",
  "value": "<label class=\"fll\" for=\")" AUTOCONNECT_CONFIGAUX_ELM_AUTOSAVE R"(\">)" AUTOCONNECT_PAGECONFIG_SAVECREDENTIAL R"(</label>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTOSAVE R"(O",
  "type": "ACElement",
  "value": "<div class=\"flr\">"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTOSAVE R"(",
  "type": "ACRadio",
  "value": [
    "AUTO",
    "ALWAYS",
    "NEVER"
  ],
  "posterior": "div",
  "arrange": "horizontal"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTOSAVE R"(C",
  "type": "ACElement",
  "value": "</div>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTORESTART R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTORESTART R"(",
  "label": ")" AUTOCONNECT_PAGECONFIG_AUTORESTART R"(",
  "labelposition": "infront"
},
{
  "name": "sep4",
  "type": "ACElement",
  "value": "<div class=\"shd\">)" AUTOCONNECT_PAGELABEL_STATIONSETTINGS R"(</div>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_PRESERVEAPMODE R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_PRESERVEAPMODE R"(",
  "label": ")" AUTOCONNECT_PAGECONFIG_PRESERVEAPMODE R"(",
  "labelposition": "infront"
},
{
  "name": "tbO",
  "type": "ACElement",
  "value": "<div class=\"tb\">"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_ENABLEDHCP R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_ENABLEDHCP R"(",
  "label": ")" AUTOCONNECT_PAGECONFIG_ENABLEDHCP R"(",
  "labelposition": "infront",
  "posterior": "none"
},
{
  "name": "tbcO",
  "type": "ACElement",
  "value": "<div class=\"tbc\">"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_STAIP R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_IPADDRESS R"(",
  "placeholder": "0.0.0.0",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_STAGATEWAY R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_GATEWAY R"(",
  "placeholder": "0.0.0.0",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_STANETMASK R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGESTATS_SUBNETMASK R"(",
  "placeholder": "0.0.0.0",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_PRIMARYDNS R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_PRIMARYDNS R"(",
  "placeholder": "0.0.0.0",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_SECONDARYDNS R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_SECONDARYDNS R"(",
  "placeholder": "0.0.0.0",
  "posterior": "div"
},
{
  "name": "tbcC",
  "type": "ACElement",
  "value": "</div></div>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_HOSTNAME R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_HOSTNAME R"("
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_HOMEURI R"(",
  "type": "ACInput",
  "placeholder": ")" AUTOCONNECT_HOMEURI R"(",
  "label": ")" AUTOCONNECT_PAGECONFIG_HOMEURI R"("
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_BOOTURI R"(L",
  "type": "ACElement",
  "value": "<label class=\"fll\">)" AUTOCONNECT_PAGECONFIG_ONBOOTURI R"(</label>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_BOOTURI R"(O",
  "type": "ACElement",
  "value": "<div class=\"flr\">"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_BOOTURI R"(",
  "type": "ACRadio",
  "value": [
    "ROOT",
    "HOME"
  ],
  "arrange": "horizontal",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_BOOTURI R"(C",
  "type": "ACElement",
  "value": "</div>"
},
{
  "name": "sep5",
  "type": "ACElement",
  "value": "<div class=\"shd\">)" AUTOCONNECT_PAGELABEL_AUTHSETTINGS R"(</div>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTH R"(L",
  "type": "ACElement",
  "value": "<label class=\"fll\" for=\")" AUTOCONNECT_CONFIGAUX_ELM_AUTH R"(\">)" AUTOCONNECT_PAGECONFIG_AUTHENTICATION R"(</label>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTH R"(O",
  "type": "ACElement",
  "value": "<div class=\"flr\">"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTH R"(",
  "type": "ACRadio",
  "value": [
    "None",
    "BASIC",
    "DIGEST"
  ],
  "arrange": "horizontal",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTH R"(C",
  "type": "ACElement",
  "value": "</div>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTHSCOPE R"(L",
  "type": "ACElement",
  "value": "<label class=\"fll\" for=\")" AUTOCONNECT_CONFIGAUX_ELM_AUTHSCOPE R"(\">)" AUTOCONNECT_PAGECONFIG_AUTHSCOPE R"(</label>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTHSCOPE R"(O",
  "type": "ACElement",
  "value": "<div class=\"flr\">"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTHSCOPE R"(",
  "type": "ACRadio",
  "value": [
    "Partial",
    "AUX",
    "AC",
    "PORTAL"
  ],
  "arrange": "horizontal",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_AUTHSCOPE R"(C",
  "type": "ACElement",
  "value": "</div>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_USER R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_USERNAME R"(",
  "placeholder": ")" AUTOCONNECT_APID R"("
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_PASSWORD R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_PASSWORD R"(",
  "placeholder": ")" AUTOCONNECT_PSK R"("
},
{
  "name": "sep6",
  "type": "ACElement",
  "value": "<div class=\"shd\">)" AUTOCONNECT_PAGELABEL_MISCELLANEOUS R"(</div>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_MENUTITLE R"(",
  "type": "ACInput",
  "placeholder": ")" AUTOCONNECT_MENU_TITLE R"(",
  "label": ")" AUTOCONNECT_PAGECONFIG_MENUTITLE R"("
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_MENU R"(L",
  "type": "ACElement",
  "value": "<label class=\"fll\" for=\")" AUTOCONNECT_CONFIGAUX_ELM_MENU R"(\">)" AUTOCONNECT_PAGECONFIG_MENUSUSE R"(</label>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_MENU R"(O",
  "type": "ACElement",
  "value": "<div class=\"flr\">"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_MENUCONFIGNEW R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_MENUCONFIGNEW R"(",
  "label": ")" AUTOCONNECT_MENULABEL_CONFIGNEW R"(",
  "labelposition": "infront",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_MENUOPENSSIDS R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_MENUOPENSSIDS R"(",
  "label": ")" AUTOCONNECT_MENULABEL_OPENSSIDS R"(",
  "labelposition": "infront",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_MENUDISCONNECT R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_MENUDISCONNECT R"(",
  "label": ")" AUTOCONNECT_MENULABEL_DISCONNECT R"(",
  "labelposition": "infront",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_MENURESET R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_MENURESET R"(",
  "label": ")" AUTOCONNECT_MENULABEL_RESET R"(",
  "labelposition": "infront",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_MENUHOME R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_MENUHOME R"(",
  "label": ")" AUTOCONNECT_MENULABEL_HOME R"(",
  "labelposition": "infront",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_MENUUPDATE R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_MENUUPDATE R"(",
  "label": ")" AUTOCONNECT_MENULABEL_UPDATE R"(",
  "labelposition": "infront",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_MENU R"(C",
  "type": "ACElement",
  "value": "</div>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_TICKER R"(L",
  "type": "ACElement",
  "value": "<label class=\"fll\" for=\")" AUTOCONNECT_CONFIGAUX_ELM_TICKER R"(\">)" AUTOCONNECT_PAGECONFIG_TICKER R"(</label>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_TICKER R"(O",
  "type": "ACElement",
  "value": "<div class=\"flr\">"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_TICKER R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_TICKER R"(",
  "label": ")" AUTOCONNECT_PAGECONFIG_TICKERON R"(",
  "labelposition": "infront",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_TICKERON R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_TICKERON R"(",
  "label": ")" AUTOCONNECT_PAGECONFIG_TICKERACTIVE R"(",
  "labelposition": "infront",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_TICKERPORT R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_TICKERPORT R"(",
  "apply": "number",
  "style": "width:2.5em",
  "posterior": "div"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_TICKER R"(C",
  "type": "ACElement",
  "value": "</div>"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_BUILTINOTA R"(",
  "type": "ACCheckbox",
  "value": ")" AUTOCONNECT_CONFIGAUX_ELM_BUILTINOTA R"(",
  "label": ")" AUTOCONNECT_PAGECONFIG_BUILTINOTA R"(",
  "labelposition": "infront"
},
{
  "name": ")" AUTOCONNECT_CONFIGAUX_ELM_BOUNDARYOFFSET R"(",
  "type": "ACInput",
  "label": ")" AUTOCONNECT_PAGECONFIG_BOUNDARY R"(",
  "placeholder": ")" AUTOCONNECT_STRING_DEPLOY(AC_IDENTIFIER_OFFSET) R"(",
  "apply": "number",
  "style": "width:5em"
},
{
  "name": "apply",
  "type": "ACSubmit",
  "value": ")" AUTOCONNECT_PAGECONFIG_APPLY R"(",
  "uri": ")" AUTOCONNECT_URI_CONFIGAUX R"("
}
]
)";

const char AutoConnectConfigAux::_rdlg[] PROGMEM = R"(
<script type="text/javascript">window.onload=function(){window.location.replace('#rdlg');};</script>
)";

/**
 * Join AutoConnectConfigAux to the AutoConnect instance, and start
 * working as a custom web page.
 * This function inherits from AutoConnectAux::_join and loads the
 * persistent AutoConnectConfig settings from the file in addition to
 * the inherited JOIN behavior.
 * @param ac  AutoConnect instance
 */
void AutoConnectConfigAux::_join(AutoConnect& ac) {
  AutoConnectAux::_join(ac);
  this->on(std::bind(&AutoConnectConfigAux::_settings, this, std::placeholders::_1, std::placeholders::_2));
  _loadSettings();
}

/**
 * Loads the current settings from AutoConnectConfig to each element of
 * the AutoConnectConfigAux page.
 * Alternatively, the setting value entered on the AutoConnectConfigAux
 * page is reflected in the AutoConnectConfig.
 * This function is a page handler of AutoConnectAux with the Apply button
 * on the AutoConnectConfigAux page.
 * @param me  AutoConnectAux (That is, AutoConnectConfigAux itself)
 * @param arg PageArgument
 */
String AutoConnectConfigAux::_settings(AutoConnectAux& me, PageArgument& arg) {
  String  ps;

  if (arg.hasArg(AUTOCONNECT_AUXURI_PARAM)) {
    if (arg.arg(AUTOCONNECT_AUXURI_PARAM) == _indicateUri(arg)) {
      _saveSettings(me);
      ps = String(FPSTR(_rdlg));
    }
  }
  else
    _retrieveSettings(*this);
  return ps;
}

/**
 * Loads the persisted settings from AUTOCONNECT_CONFIGAUX_FILE into the
 * current AutoConnectConfig.
 */
void AutoConnectConfigAux::_loadSettings(void) {
  bool  bc;
  fs::File  cf;

  if (!loadElement(FPSTR(_ui), String(), 10400)) {
    AC_DBG("Failed to load AutoConnectConfigAux\n");
    return;
  }

  _retrieveSettings(*this);
#if defined(ARDUINO_ARCH_ESP8266)
  FSInfo  info;
  if (AUTOCONNECT_APPLIED_FILESYSTEM.info(info))
    bc = true;
  else
    bc = AUTOCONNECT_APPLIED_FILESYSTEM.begin();
#elif defined(ARDUINO_ARCH_ESP32)
  if (esp_spiffs_mounted(NULL))
    bc = true;
  else
    bc = AUTOCONNECT_APPLIED_FILESYSTEM.begin(true);
#endif
  if (!_fn.startsWith(String("/")))
    _fn = String("/") + _fn;
  AC_DBG("Settings %s ", _fn.c_str());
  if (bc) {
    bc = false;
    if (AUTOCONNECT_APPLIED_FILESYSTEM.exists(_fn)) {
      fs::File  cf = AUTOCONNECT_APPLIED_FILESYSTEM.open(_fn, "r");
      if (cf) {
        size_t fs = ((cf.size() + 256) / 256) * 256;
        AC_DBG_DUMB("Json buffer %u \n", fs);
        if (loadElement(cf, String(), fs)) {
          _restoreSettings(*this);
          bc = true;
        }
        else {
          AC_DBG("fails to load");
        }
        cf.close();
      }
      else {
        AC_DBG_DUMB("fails to open");
      }
    }
    else {
      AC_DBG_DUMB("not exists");
    }
  }
  if (!bc) {
    AC_DBG_DUMB(", assumes default assignments\n");
  }
}

/**
 * Persist by outputting the current AutoConnectConnect setting to
 * AUTOCONNECT_CONFIGAUX_FILE.
 * @param me  AutoConnectAux
 */
void AutoConnectConfigAux::_saveSettings(AutoConnectAux& me) {
  AC_DBG("Settings %s ", _fn.c_str());
  fs::File  cf = AUTOCONNECT_APPLIED_FILESYSTEM.open(_fn, "w");
  if (cf) {
    std::vector<String> elms;
    std::transform(std::begin(_elmNames), std::end(_elmNames), std::back_inserter(elms), [](PGM_P elmName){
      return String(FPSTR(elmName));
    });
    saveElement(cf, elms);
    cf.close();
    AC_DBG_DUMB("saved\n");
    _restoreSettings(reinterpret_cast<AutoConnectConfigAux&>(me));
  }
  else {
    AC_DBG_DUMB("fails to save\n");
  }
}

/**
 * Restore all settings from the elements of AutoConnectConfigAux to
 * the current AutoConnectConfig.
 * @param me  AutoConnectAux (That is, AutoConnectConfigAux itself)
 */
void AutoConnectConfigAux::_restoreSettings(AutoConnectConfigAux& me) {
  IPAddress univ;
  AutoConnectConfig&  acConfig = _ac->getConfig();
  acConfig.apid = me[AUTOCONNECT_CONFIGAUX_ELM_SSID].as<AutoConnectInput>().value;
  acConfig.psk = me[AUTOCONNECT_CONFIGAUX_ELM_PSK].as<AutoConnectInput>().value;
  acConfig.channel = me[AUTOCONNECT_CONFIGAUX_ELM_CHANNEL].as<AutoConnectInput>().value.toInt();
  acConfig.hidden = me[AUTOCONNECT_CONFIGAUX_ELM_HIDDEN].as<AutoConnectCheckbox>().checked ? 1 : 0;
  if (univ.fromString(me[AUTOCONNECT_CONFIGAUX_ELM_APIP].as<AutoConnectInput>().value))
    acConfig.apip = univ;
  if (univ.fromString(me[AUTOCONNECT_CONFIGAUX_ELM_APGATEWAY].as<AutoConnectInput>().value))
    acConfig.gateway = univ;
  if (univ.fromString(me[AUTOCONNECT_CONFIGAUX_ELM_APNETMASK].as<AutoConnectInput>().value))
    acConfig.netmask = univ;
  acConfig.autoRise = me[AUTOCONNECT_CONFIGAUX_ELM_AUTORISE].as<AutoConnectCheckbox>().checked;
  acConfig.immediateStart = me[AUTOCONNECT_CONFIGAUX_ELM_IMMEDIATESTART].as<AutoConnectCheckbox>().checked;
  acConfig.retainPortal = me[AUTOCONNECT_CONFIGAUX_ELM_RETAINPORTAL].as<AutoConnectCheckbox>().checked;
  acConfig.portalTimeout = me[AUTOCONNECT_CONFIGAUX_ELM_PORTALTIMEOUT].as<AutoConnectInput>().value.toInt();
  if (me[AUTOCONNECT_CONFIGAUX_ELM_PRINCIPLE].as<AutoConnectRadio>().value() == "RECENT")
    acConfig.principle = AC_PRINCIPLE_RECENT;
  else if (me[AUTOCONNECT_CONFIGAUX_ELM_PRINCIPLE].as<AutoConnectRadio>().value() == "RSSI")
    acConfig.principle = AC_PRINCIPLE_RSSI;
  acConfig.minRSSI = me[AUTOCONNECT_CONFIGAUX_ELM_MINRSSI].as<AutoConnectInput>().value.toInt();
  acConfig.beginTimeout = me[AUTOCONNECT_CONFIGAUX_ELM_BEGINTIMEOUT].as<AutoConnectInput>().value.toInt();
  acConfig.autoReconnect = me[AUTOCONNECT_CONFIGAUX_ELM_AUTORECONNECT].as<AutoConnectCheckbox>().checked;
  acConfig.reconnectInterval = me[AUTOCONNECT_CONFIGAUX_ELM_RECONNECTINT].as<AutoConnectInput>().value.toInt();
  if (me[AUTOCONNECT_CONFIGAUX_ELM_AUTOSAVE].as<AutoConnectRadio>().value() == "AUTO")
    acConfig.autoSave = AC_SAVECREDENTIAL_AUTO;
  else if (me[AUTOCONNECT_CONFIGAUX_ELM_AUTOSAVE].as<AutoConnectRadio>().value() == "ALWAYS")
    acConfig.autoSave = AC_SAVECREDENTIAL_ALWAYS;
  else if (me[AUTOCONNECT_CONFIGAUX_ELM_AUTOSAVE].as<AutoConnectRadio>().value() == "NEVER")
    acConfig.autoSave = AC_SAVECREDENTIAL_NEVER;
  acConfig.autoReset = me[AUTOCONNECT_CONFIGAUX_ELM_AUTORESTART].as<AutoConnectCheckbox>().checked;
  acConfig.preserveAPMode = me[AUTOCONNECT_CONFIGAUX_ELM_PRESERVEAPMODE].as<AutoConnectCheckbox>().checked;
  if (univ.fromString(me[AUTOCONNECT_CONFIGAUX_ELM_STAIP].as<AutoConnectInput>().value))
    acConfig.staip = univ;
  if (univ.fromString(me[AUTOCONNECT_CONFIGAUX_ELM_STAGATEWAY].as<AutoConnectInput>().value))
    acConfig.staGateway = univ;
  if (univ.fromString(me[AUTOCONNECT_CONFIGAUX_ELM_STANETMASK].as<AutoConnectInput>().value))
    acConfig.staNetmask = univ;
  if (univ.fromString(me[AUTOCONNECT_CONFIGAUX_ELM_PRIMARYDNS].as<AutoConnectInput>().value))
    acConfig.dns1 = univ;
  if (univ.fromString(me[AUTOCONNECT_CONFIGAUX_ELM_SECONDARYDNS].as<AutoConnectInput>().value))
    acConfig.dns2 = univ;
  acConfig.hostName = me[AUTOCONNECT_CONFIGAUX_ELM_HOSTNAME].as<AutoConnectInput>().value;
  acConfig.homeUri = me["uri"].as<AutoConnectInput>().value;
  if (me[AUTOCONNECT_CONFIGAUX_ELM_BOOTURI].as<AutoConnectRadio>().value() == "ROOT")
    acConfig.bootUri = AC_ONBOOTURI_ROOT;
  else if (me[AUTOCONNECT_CONFIGAUX_ELM_BOOTURI].as<AutoConnectRadio>().value() == "HOME")
    acConfig.bootUri = AC_ONBOOTURI_HOME;
  if (me[AUTOCONNECT_CONFIGAUX_ELM_AUTH].as<AutoConnectRadio>().value() == "None")
    acConfig.auth = AC_AUTH_NONE;
  else if (me[AUTOCONNECT_CONFIGAUX_ELM_AUTH].as<AutoConnectRadio>().value() == "BASIC")
    acConfig.auth = AC_AUTH_BASIC;
  else if (me[AUTOCONNECT_CONFIGAUX_ELM_AUTH].as<AutoConnectRadio>().value() == "DIGEST")
    acConfig.auth = AC_AUTH_DIGEST;
  if (me[AUTOCONNECT_CONFIGAUX_ELM_AUTHSCOPE].as<AutoConnectRadio>().value() == "Partial")
    acConfig.authScope = AC_AUTHSCOPE_PARTIAL;
  else if (me[AUTOCONNECT_CONFIGAUX_ELM_AUTHSCOPE].as<AutoConnectRadio>().value() == "AUX")
    acConfig.authScope = AC_AUTHSCOPE_AUX;
  else if (me[AUTOCONNECT_CONFIGAUX_ELM_AUTHSCOPE].as<AutoConnectRadio>().value() == "AC")
    acConfig.authScope = AC_AUTHSCOPE_AC;
  else if (me[AUTOCONNECT_CONFIGAUX_ELM_AUTHSCOPE].as<AutoConnectRadio>().value() == "PORTAL")
    acConfig.authScope = AC_AUTHSCOPE_PORTAL;
  acConfig.username = me[AUTOCONNECT_CONFIGAUX_ELM_USER].as<AutoConnectInput>().value;
  acConfig.password = me[AUTOCONNECT_CONFIGAUX_ELM_PASSWORD].as<AutoConnectInput>().value;
  acConfig.title = me[AUTOCONNECT_CONFIGAUX_ELM_MENUTITLE].as<AutoConnectInput>().value;
  acConfig.menuItems = 0;
  if (me[AUTOCONNECT_CONFIGAUX_ELM_MENUCONFIGNEW].as<AutoConnectCheckbox>().checked)
    acConfig.menuItems |=  AC_MENUITEM_CONFIGNEW;
  if (me[AUTOCONNECT_CONFIGAUX_ELM_MENUOPENSSIDS].as<AutoConnectCheckbox>().checked)
    acConfig.menuItems |= AC_MENUITEM_OPENSSIDS;
  if (me[AUTOCONNECT_CONFIGAUX_ELM_MENUDISCONNECT].as<AutoConnectCheckbox>().checked)
    acConfig.menuItems |= AC_MENUITEM_DISCONNECT;
  if (me[AUTOCONNECT_CONFIGAUX_ELM_MENURESET].as<AutoConnectCheckbox>().checked)
    acConfig.menuItems |= AC_MENUITEM_RESET;
  if (me[AUTOCONNECT_CONFIGAUX_ELM_MENUHOME].as<AutoConnectCheckbox>().checked)
    acConfig.menuItems |= AC_MENUITEM_HOME;
  if (me[AUTOCONNECT_CONFIGAUX_ELM_MENUUPDATE].as<AutoConnectCheckbox>().checked)
    acConfig.menuItems |= AC_MENUITEM_UPDATE;
  acConfig.ticker = me[AUTOCONNECT_CONFIGAUX_ELM_TICKER].as<AutoConnectCheckbox>().checked;
  acConfig.tickerOn = me[AUTOCONNECT_CONFIGAUX_ELM_TICKERON].as<AutoConnectCheckbox>().checked ? LOW : HIGH;
  acConfig.tickerPort = me[AUTOCONNECT_CONFIGAUX_ELM_TICKERPORT].as<AutoConnectInput>().value.toInt();
  acConfig.ota = me[AUTOCONNECT_CONFIGAUX_ELM_BUILTINOTA].as<AutoConnectCheckbox>().checked ? AC_OTA_BUILTIN : AC_OTA_EXTRA;
  acConfig.boundaryOffset = me[AUTOCONNECT_CONFIGAUX_ELM_BOUNDARYOFFSET].as<AutoConnectInput>().value.toInt();
  AC_DBG("_apConfig restored\n");
}

/**
 * Retrieve the current AutoConnectConfig settings to the
 * AutoConnectElements on the AUX page.
 * @param me  AutoConnectAux (That is, AutoConnectConfigAux itself)
 */
void AutoConnectConfigAux::_retrieveSettings(AutoConnectConfigAux& me) {
  AutoConnectConfig&  acConfig = _ac->getConfig();
  me[AUTOCONNECT_CONFIGAUX_ELM_SSID].as<AutoConnectInput>().value = acConfig.apid;
  me[AUTOCONNECT_CONFIGAUX_ELM_PSK].as<AutoConnectInput>().value = acConfig.psk;
  me[AUTOCONNECT_CONFIGAUX_ELM_CHANNEL].as<AutoConnectInput>().value = String(acConfig.channel);
  me[AUTOCONNECT_CONFIGAUX_ELM_HIDDEN].as<AutoConnectCheckbox>().checked = (bool)acConfig.hidden;
  me[AUTOCONNECT_CONFIGAUX_ELM_APIP].as<AutoConnectInput>().value = acConfig.apip.toString();
  me[AUTOCONNECT_CONFIGAUX_ELM_APGATEWAY].as<AutoConnectInput>().value = acConfig.gateway.toString();
  me[AUTOCONNECT_CONFIGAUX_ELM_APNETMASK].as<AutoConnectInput>().value = acConfig.netmask.toString();
  me[AUTOCONNECT_CONFIGAUX_ELM_AUTORISE].as<AutoConnectCheckbox>().checked = acConfig.autoRise;
  me[AUTOCONNECT_CONFIGAUX_ELM_IMMEDIATESTART].as<AutoConnectCheckbox>().checked = acConfig.immediateStart;
  me[AUTOCONNECT_CONFIGAUX_ELM_RETAINPORTAL].as<AutoConnectCheckbox>().checked = acConfig.retainPortal;
  me[AUTOCONNECT_CONFIGAUX_ELM_PORTALTIMEOUT].as<AutoConnectInput>().value = String(acConfig.portalTimeout);
  switch (acConfig.principle) {
  case AC_PRINCIPLE_RECENT:
    me[AUTOCONNECT_CONFIGAUX_ELM_PRINCIPLE].as<AutoConnectRadio>().check("RECENT");
    break;
  case AC_PRINCIPLE_RSSI:
    me[AUTOCONNECT_CONFIGAUX_ELM_PRINCIPLE].as<AutoConnectRadio>().check("RSSI");
    break;
  default:
    break;
  }
  me[AUTOCONNECT_CONFIGAUX_ELM_MINRSSI].as<AutoConnectInput>().value = String(acConfig.minRSSI);
  me[AUTOCONNECT_CONFIGAUX_ELM_BEGINTIMEOUT].as<AutoConnectInput>().value = String(acConfig.beginTimeout);
  me[AUTOCONNECT_CONFIGAUX_ELM_AUTORECONNECT].as<AutoConnectCheckbox>().checked = acConfig.autoReconnect;
  me[AUTOCONNECT_CONFIGAUX_ELM_RECONNECTINT].as<AutoConnectInput>().value = String(acConfig.reconnectInterval);
  switch (acConfig.autoSave) {
  case AC_SAVECREDENTIAL_AUTO:
    me[AUTOCONNECT_CONFIGAUX_ELM_AUTOSAVE].as<AutoConnectRadio>().check("AUTO");
    break;
  case AC_SAVECREDENTIAL_ALWAYS:
    me[AUTOCONNECT_CONFIGAUX_ELM_AUTOSAVE].as<AutoConnectRadio>().check("ALWAYS");
    break;
  case AC_SAVECREDENTIAL_NEVER:
    me[AUTOCONNECT_CONFIGAUX_ELM_AUTOSAVE].as<AutoConnectRadio>().check("NEVER");
    break;
  default:
    break;
  }
  me[AUTOCONNECT_CONFIGAUX_ELM_AUTORESTART].as<AutoConnectCheckbox>().checked = acConfig.autoReset;
  me[AUTOCONNECT_CONFIGAUX_ELM_PRESERVEAPMODE].as<AutoConnectCheckbox>().checked = acConfig.preserveAPMode;
  me[AUTOCONNECT_CONFIGAUX_ELM_ENABLEDHCP].as<AutoConnectCheckbox>().checked = !(acConfig.staip || acConfig.staGateway || acConfig.staNetmask || acConfig.dns1 || acConfig.dns2); 
  me[AUTOCONNECT_CONFIGAUX_ELM_STAIP].as<AutoConnectInput>().value = acConfig.staip.toString();
  me[AUTOCONNECT_CONFIGAUX_ELM_STAGATEWAY].as<AutoConnectInput>().value = acConfig.staGateway.toString();
  me[AUTOCONNECT_CONFIGAUX_ELM_STANETMASK].as<AutoConnectInput>().value = acConfig.staNetmask.toString();
  me[AUTOCONNECT_CONFIGAUX_ELM_PRIMARYDNS].as<AutoConnectInput>().value = acConfig.dns1.toString();
  me[AUTOCONNECT_CONFIGAUX_ELM_SECONDARYDNS].as<AutoConnectInput>().value = acConfig.dns2.toString();
  me[AUTOCONNECT_CONFIGAUX_ELM_HOSTNAME].as<AutoConnectInput>().value = acConfig.hostName;
  me["uri"].as<AutoConnectInput>().value = acConfig.homeUri;
  switch (acConfig.bootUri) {
  case AC_ONBOOTURI_ROOT:
    me[AUTOCONNECT_CONFIGAUX_ELM_BOOTURI].as<AutoConnectRadio>().check("ROOT");
    break;
  case AC_ONBOOTURI_HOME:
    me[AUTOCONNECT_CONFIGAUX_ELM_BOOTURI].as<AutoConnectRadio>().check("HOME");
    break;
  default:
    break;
  }
  switch (acConfig.auth) {
  case AC_AUTH_NONE:
    me[AUTOCONNECT_CONFIGAUX_ELM_AUTH].as<AutoConnectRadio>().check("None");
    break;
  case AC_AUTH_BASIC:
    me[AUTOCONNECT_CONFIGAUX_ELM_AUTH].as<AutoConnectRadio>().check("BASIC");
    break;
  case AC_AUTH_DIGEST:
    me[AUTOCONNECT_CONFIGAUX_ELM_AUTH].as<AutoConnectRadio>().check("DIGEST");
    break;
  default:
    break;
  }
  switch (acConfig.authScope) {
  case AC_AUTHSCOPE_PARTIAL:
    me[AUTOCONNECT_CONFIGAUX_ELM_AUTHSCOPE].as<AutoConnectRadio>().check("Partial");
    break;
  case AC_AUTHSCOPE_AUX:
    me[AUTOCONNECT_CONFIGAUX_ELM_AUTHSCOPE].as<AutoConnectRadio>().check("AUX");
    break;
  case AC_AUTHSCOPE_AC:
    me[AUTOCONNECT_CONFIGAUX_ELM_AUTHSCOPE].as<AutoConnectRadio>().check("AC");
    break;
  case AC_AUTHSCOPE_PORTAL:
    me[AUTOCONNECT_CONFIGAUX_ELM_AUTHSCOPE].as<AutoConnectRadio>().check("PORTAL");
    break;
  default:
    break;
  }
  me[AUTOCONNECT_CONFIGAUX_ELM_USER].as<AutoConnectInput>().value = acConfig.username;
  me[AUTOCONNECT_CONFIGAUX_ELM_PASSWORD].as<AutoConnectInput>().value = acConfig.password;
  me[AUTOCONNECT_CONFIGAUX_ELM_MENUTITLE].as<AutoConnectInput>().value = acConfig.title;
  me[AUTOCONNECT_CONFIGAUX_ELM_MENUCONFIGNEW].as<AutoConnectCheckbox>().checked = acConfig.menuItems & AC_MENUITEM_CONFIGNEW;
  me[AUTOCONNECT_CONFIGAUX_ELM_MENUOPENSSIDS].as<AutoConnectCheckbox>().checked = acConfig.menuItems & AC_MENUITEM_OPENSSIDS;
  me[AUTOCONNECT_CONFIGAUX_ELM_MENUDISCONNECT].as<AutoConnectCheckbox>().checked = acConfig.menuItems & AC_MENUITEM_DISCONNECT;
  me[AUTOCONNECT_CONFIGAUX_ELM_MENURESET].as<AutoConnectCheckbox>().checked = acConfig.menuItems & AC_MENUITEM_RESET;
  me[AUTOCONNECT_CONFIGAUX_ELM_MENUHOME].as<AutoConnectCheckbox>().checked = acConfig.menuItems & AC_MENUITEM_HOME;
  me[AUTOCONNECT_CONFIGAUX_ELM_MENUUPDATE].as<AutoConnectCheckbox>().checked = acConfig.menuItems & AC_MENUITEM_UPDATE;
  me[AUTOCONNECT_CONFIGAUX_ELM_TICKER].as<AutoConnectCheckbox>().checked = acConfig.ticker;
  me[AUTOCONNECT_CONFIGAUX_ELM_TICKERON].as<AutoConnectCheckbox>().checked = acConfig.tickerOn == LOW;
  me[AUTOCONNECT_CONFIGAUX_ELM_TICKERPORT].as<AutoConnectInput>().value = String(acConfig.tickerPort);
  me[AUTOCONNECT_CONFIGAUX_ELM_BUILTINOTA].as<AutoConnectCheckbox>().checked = acConfig.ota == AC_OTA_BUILTIN;
  me[AUTOCONNECT_CONFIGAUX_ELM_BOUNDARYOFFSET].as<AutoConnectInput>().value = String(acConfig.boundaryOffset);
  AC_DBG("_apConfig retrieved\n");
}

#endif // !AUTOCONNECT_USE_CONFIGAUX
