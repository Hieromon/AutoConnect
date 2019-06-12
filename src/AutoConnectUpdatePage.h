/**
 * Define dialogs to operate sketch binary updates operated by the
 * AutoConnectUpdate class.
 * @file   AutoConnectUpdatePage.h
 * @author hieromon@gmail.com
 * @version    0.9.11
 * @date   2019-05-14
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTUPDATEPAGE_H
#define _AUTOCONNECTUPDATEPAGE_H

// Define the AUTOCONNECT_URI_UPDATE page to select the sketch binary
// for update and order update execution.
const AutoConnectUpdateAct::ACElementProp_t AutoConnectUpdateAct::_elmCatalog[] PROGMEM = {
  { AC_Element, "binSty", "<style type\"text/css\">.bins{display:grid;font-size:14px;grid-gap:10px 0;grid-template-columns:1em repeat(4,max-content);overflow-x:auto}.bins input[type=radio]{-moz-appearance:radio;-webkit-appearance:radio;margin:0;vertical-align:middle}.noorder .bins label,span{margin:0 .5em 0 .5em;padding:0;text-align:left}</style>", nullptr },
  { AC_Text, "caption", nullptr, nullptr },
  { AC_Element, "c1", "<div class=\"bins\">", nullptr },
  { AC_Radio, "firmwares", nullptr, nullptr },
  { AC_Element, "c1", "</div>", nullptr },
  { AC_Submit, "update", "UPDATE", AUTOCONNECT_URI_UPDATE_ACT }
};
const AutoConnectUpdateAct::ACPage_t AutoConnectUpdateAct::_auxCatalog PROGMEM = {
  AUTOCONNECT_URI_UPDATE, "Update", false, AutoConnectUpdateAct::_elmCatalog
};

// Define the AUTOCONNECT_URI_UPDATE_ACT page to display during the
// update process.
const AutoConnectUpdateAct::ACElementProp_t AutoConnectUpdateAct::_elmProgress[] PROGMEM = {
  { AC_Element, "loader", "<style>.loader{border:2px solid #f3f3f3;border-radius:50%;border-top:2px solid #555;width:12px;height:12px;-webkit-animation:spin 1s linear infinite;animation:spin 1s linear infinite}@-webkit-keyframes spin{0%{-webkit-transform:rotate(0)}100%{-webkit-transform:rotate(360deg)}}@keyframes spin{0%{transform:rotate(0)}100%{transform:rotate(360deg)}}</style>", nullptr },
  { AC_Element, "c1", "<div style=\"display:inline-block\">", nullptr },
  { AC_Element, "binname", nullptr, nullptr },
  { AC_Element, "c2", "&ensp;from&ensp;", nullptr },
  { AC_Element, "url", "</dv>", nullptr },
  { AC_Element, "c3", "<div id=\"progress\">Updating...<span style=\"display:inline-block;vertical-align:middle;margin-left:7px\">", nullptr },
  { AC_Element, "progress_meter", "<meter min=\"0\" />", nullptr },
  { AC_Element, "progress_loader", "<div id=\"ld\" />", nullptr },
  { AC_Element, "c4", "</span></div></div>", nullptr },
  { AC_Text, "status", nullptr, nullptr },
  { AC_Element, "c5", "<script type=\"text/javascript\">function rd(){location.href=\"" AUTOCONNECT_URI_UPDATE_RESULT "\"}var ws;window.onload=function(){(ws=new WebSocket(\"", nullptr },
  { AC_Element, "wsurl", nullptr, nullptr },
  { AC_Element, "c6", "\")).onopen=function(){ws.onmessage=function(e){var o=e.data.split(\",\");\"#s\"==o[0]?", nullptr },
  { AC_Element, "enable_loader", "(document.getElementById(\"ld\").className=\"loader\",", nullptr },
  { AC_Element, "c7", "window.setTimeout(rd()," AUTOCONNECT_STRING_DEPLOY(AUTOCONNECT_UPDATE_DURATION) ")", nullptr },
  { AC_Element, "enable_loader_post", ")", nullptr },
  { AC_Element, "c8", ":\"#e\"==o[0]?ws.close():\"#p\"==o[0]&&incr(o[1])}},ws.onclose=function(e){rd()},ws.onerror=function(e){console.log(\"WS err(\"+e.code+\")\"+e.reason),1==ws.readyState&&(document.getElementById(\"status\").textContent=\"WebSocket \"+e.type)}};", nullptr },
  { AC_Element, "inprogress_meter", "function incr(e){var t=e.split(\":\"),r=document.getElementById(\"progress\").getElementsByTagName(\"meter\");r[0].setAttribute(\"value\",t[0]),r[0].setAttribute(\"max\",t[1])}", nullptr },
  { AC_Element, "inprogress_loader", "function incr(pv){}", nullptr },
  { AC_Element, "c9", "</script>", nullptr }
};
const AutoConnectUpdateAct::ACPage_t AutoConnectUpdateAct::_auxProgress PROGMEM = {
  AUTOCONNECT_URI_UPDATE_ACT, "Update", false, AutoConnectUpdateAct::_elmProgress
};

// Definition of the AUTOCONNECT_URI_UPDATE_RESULT page to notify update results
const AutoConnectUpdateAct::ACElementProp_t AutoConnectUpdateAct::_elmResult[] PROGMEM = {
  { AC_Text, "status", nullptr, nullptr },
  { AC_Element, "restart", "<script type=\"text/javascript\">setTimeout(\"location.href='" AUTOCONNECT_HOMEURI "'\",1000*10);</script>", nullptr }
};
const AutoConnectUpdateAct::ACPage_t AutoConnectUpdateAct::_auxResult PROGMEM = {
  AUTOCONNECT_URI_UPDATE_RESULT, "Update", false, AutoConnectUpdateAct::_elmResult
};

#endif // _AUTOCONNECTUPDATEPAGE_H
