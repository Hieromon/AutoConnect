/**
 * Define dialogs to operate sketch binary updates operated by the
 * AutoConnectUpdate class.
 * @file   AutoConnectUpdatePage.h
 * @author hieromon@gmail.com
 * @version    1.3.6
 * @date   2022-07-25
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTUPDATEPAGE_H_
#define _AUTOCONNECTUPDATEPAGE_H_

/**
 * Definitions of notification commands to synchronize update processing
 * with the Web client.
 */
#define AUTOCONNECT_UPDATE_NOTIFY_START     "#s"
#define AUTOCONNECT_UPDATE_NOTIFY_PROGRESS  "#p"
#define AUTOCONNECT_UPDATE_NOTIFY_END       "#e"
#define AUTOCONNECT_UPDATE_NOTIFY_REBOOT    "#r"

// Define the AUTOCONNECT_URI_UPDATE page to select the sketch binary
// for update and order update execution.
const AutoConnectAux::ACElementProp_t AutoConnectUpdateAct::_elmCatalog[] PROGMEM = {
  { AC_Style, "bins", ".bins{display:grid;font-size:14px;grid-gap:10px 0;grid-template-columns:1em repeat(4,max-content);overflow-x:auto}.bins input[type=radio]{-moz-appearance:radio;-webkit-appearance:radio;margin:0;vertical-align:middle}.noorder .bins label,.bins span{margin:0 .5em 0 .5em;padding:0;text-align:left}", nullptr },
  { AC_Text, "caption", nullptr, nullptr },
  { AC_Element, "c1", "<div class=\"bins\">", nullptr },
  { AC_Radio, "firmwares", nullptr, nullptr },
  { AC_Element, "c2", "</div>", nullptr },
  { AC_Submit, "update", AUTOCONNECT_BUTTONLABEL_UPDATE, AUTOCONNECT_URI_UPDATE_ACT }
};
const AutoConnectAux::ACPage_t AutoConnectUpdateAct::_pageCatalog PROGMEM = {
  AUTOCONNECT_URI_UPDATE, AUTOCONNECT_MENULABEL_UPDATE, false, AutoConnectUpdateAct::_elmCatalog
};

// Define the AUTOCONNECT_URI_UPDATE_ACT page to display during the
// update process.
const AutoConnectAux::ACElementProp_t AutoConnectUpdateAct::_elmProgress[] PROGMEM = {
  { AC_Style, "loader", ".loader{border:2px solid #f3f3f3;border-radius:50%;border-top:2px solid #555;width:12px;height:12px;-webkit-animation:spin 1s linear infinite;animation:spin 1s linear infinite}@-webkit-keyframes spin{0%{-webkit-transform:rotate(0)}100%{-webkit-transform:rotate(360deg)}}@keyframes spin{0%{transform:rotate(0)}100%{transform:rotate(360deg)}}", nullptr },
  { AC_Element, "c1", "<div style=\"display:inline-block\">", nullptr },
  { AC_Element, "binname", nullptr, nullptr },
  { AC_Element, "c2", "&ensp;from&ensp;", nullptr },
  { AC_Element, "url", "</dv>", nullptr },
  { AC_Element, "c3", "<div id=\"progress\">Updating...<span style=\"display:inline-block;vertical-align:middle;margin-left:7px\">", nullptr },
  { AC_Element, "progress_meter", "<meter min=\"0\" />", nullptr },
  { AC_Element, "progress_loader", "<div id=\"ld\" />", nullptr },
  { AC_Element, "c4", "</span></div>", nullptr },
  { AC_Text, "status", nullptr, nullptr },
  { AC_Element, "c5", "<script type=\"text/javascript\">var lap,cls,rt;function rd(){clearInterval(lap),location.href=\"" AUTOCONNECT_URI_UPDATE_RESULT "\"}function bar(){rt=0;var t=new FormData;t.append(\"op\",\"" AUTOCONNECT_UPDATE_NOTIFY_START "\");var e=new XMLHttpRequest;e.timeout=" AUTOCONNECT_STRING_DEPLOY(AUTOCONNECT_UPDATE_TIMEOUT) ",e.open(\"POST\",\"" AUTOCONNECT_URI_UPDATE_PROGRESS "\",!0),e.onreadystatechange=function(){4==e.readyState&&(200==e.status?(cls=!1,lap=setInterval(upd," AUTOCONNECT_STRING_DEPLOY(AUTOCONNECT_UPDATE_INTERVAL) ")):document.getElementById(\"status\").textContent=\"Could not start (\"+e.status+\"): \"+e.responseText)},e.send(t)}function upd(){if(!cls){var t=new XMLHttpRequest;t.onload=function(){var t=this.responseText.split(\",\");\"" AUTOCONNECT_UPDATE_NOTIFY_START "\"==t[0]?(window.setTimeout(rd()," AUTOCONNECT_STRING_DEPLOY(AUTOCONNECT_UPDATE_DURATION) ")", nullptr },
  { AC_Element, "enable_loader", ",document.getElementById(\"ld\").className=\"loader\"", nullptr },
  { AC_Element, "c6", "):\"" AUTOCONNECT_UPDATE_NOTIFY_END "\"==t[0]?(cls=!0,rd()):\"" AUTOCONNECT_UPDATE_NOTIFY_PROGRESS "\"==t[0]&&incr(t[1])},t.onerror=function(){rt=rt++,!0==(0!=t.status||0==t.status&&rt>10)&&(document.getElementById(\"status\").textContent=\"" AUTOCONNECT_TEXT_OTAFAILURE "\"+t.status,console.log(\"http err:%d %s\",t.status,t.responseText),clearInterval(lap))},t.open(\"GET\",\"" AUTOCONNECT_URI_UPDATE_PROGRESS "\",!0),t.send()}}function incr(t){", nullptr },
  { AC_Element, "inprogress_meter", "var e=t.split(\":\"),n=document.getElementById(\"progress\").getElementsByTagName(\"meter\");n[0].setAttribute(\"value\",e[0]),n[0].setAttribute(\"max\",e[1])", nullptr },
  { AC_Element, "c7", "}window.onload=bar;</script>", nullptr }
};
const AutoConnectAux::ACPage_t AutoConnectUpdateAct::_pageProgress PROGMEM = {
  AUTOCONNECT_URI_UPDATE_ACT, AUTOCONNECT_MENULABEL_UPDATE, false, AutoConnectUpdateAct::_elmProgress
};

// Definition of the AUTOCONNECT_URI_UPDATE_RESULT page to notify update results
const AutoConnectAux::ACElementProp_t AutoConnectUpdateAct::_elmResult[] PROGMEM = {
  { AC_Text, "status", nullptr, nullptr },
  { AC_Element, "restart", "<script type=\"text/javascript\">window.onload=function(){var e=new FormData;e.append(\"op\",\"" AUTOCONNECT_UPDATE_NOTIFY_REBOOT "\");var o=new XMLHttpRequest;o.timeout=" AUTOCONNECT_STRING_DEPLOY(AUTOCONNECT_UPDATE_TIMEOUT) ",o.onloadend=function(){setTimeout(\"location.href='" AUTOCONNECT_HOMEURI "'\"," AUTOCONNECT_STRING_DEPLOY(AUTOCONNECT_UPDATE_WAITFORREBOOT) ")},o.open(\"POST\",\"" AUTOCONNECT_URI_UPDATE_PROGRESS "\",!0),o.send(e)};</script>", nullptr }
};
const AutoConnectAux::ACPage_t AutoConnectUpdateAct::_pageResult PROGMEM = {
  AUTOCONNECT_URI_UPDATE_RESULT, AUTOCONNECT_MENULABEL_UPDATE, false, AutoConnectUpdateAct::_elmResult
};

#endif // _AUTOCONNECTUPDATEPAGE_H
