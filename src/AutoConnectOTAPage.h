/**
 * Define pages to operate updates using the AutoConnectUpdate class.
 * @file   AutoConnectOTAPage.h
 * @author hieromon@gmail.com
 * @version    1.1.5
 * @date   2020-04-09
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTOTAPAGE_H_
#define _AUTOCONNECTOTAPAGE_H_

const AutoConnectOTA::ACElementProp_t AutoConnectOTA::_elmUpdate[] PROGMEM = {
  { AC_Style, "s_rc", ".s_rc{display:none}", nullptr },
  { AC_Text, "cap", "<h3>" AUTOCONNECT_TEXT_UPDATINGFIRMWARE "<h3>", nullptr },
  { AC_File, "bin", nullptr, AUTOCONNECT_TEXT_SELECTFIRMWARE },
  { AC_Button, "update", AUTOCONNECT_BUTTONLABEL_UPDATE, "_upd(this,'bin','" AUTOCONNECT_URI_UPDATE_ACT "')" },
  { AC_Element, "js", "<script type=\"text/javascript\">function _upd(e,t,n){var r=document.getElementById(t);if(r.files.length>0){par=e.parentNode,par.removeChild(e),pb=document.createElement(\"progress\"),pb.setAttribute(\"id\",\"pb\"),pb.setAttribute(\"style\",\"margin-top:1.0em\"),pb.setAttribute(\"value\",\"0\"),pb.setAttribute(\"max\",r.files[0].size),par.appendChild(pb);var p=new FormData(_bu(n)),o=new XMLHttpRequest;o.upload.addEventListener(\"progress\",function(e){pb.setAttribute(\"value\",e.loaded)},!1),o.addEventListener(\"load\",function(){document.body.innerHTML=o.response.body.innerHTML,\"0\"==document.getElementById(\"rc\").innerText&&setTimeout(function(){location.href=\"" AUTOCONNECT_HOMEURI "\"}," AUTOCONNECT_STRING_DEPLOY(AUTOCONNECT_UPDATE_WAITFORREBOOT) ")},!1),o.open(\"POST\",n),o.responseType=\"document\",o.send(p)}}var par,pb;</script>", nullptr }
};

// The definition of the OTA update operation page, which will be located to AUTOCONNECT_URI_UPDATE.
const AutoConnectOTA::ACPage_t AutoConnectOTA::_pageUpdate PROGMEM = {
  AUTOCONNECT_URI_UPDATE, AUTOCONNECT_MENULABEL_UPDATE, true, AutoConnectOTA::_elmUpdate
};

const AutoConnectOTA::ACElementProp_t AutoConnectOTA::_elmResult[] PROGMEM = {
  { AC_Text, "bin", nullptr, "margin-bottom:0.5em;font-size:1.2em;font-weight:bold;color:#" },
  { AC_Text, "result", nullptr, nullptr },
  { AC_Element, "dvo", "<div class=\"s_rc\" id=\"rc\">", nullptr },
  { AC_Element, "rc", "", nullptr },
  { AC_Element, "dvc", "</div>", nullptr }
};

// The definition of the OTA update result display page.
// This page is assigned to AUTOCONNECT_URI_UPDATE_ACT, but the actual
// HTML document is dynamically rewritten on AUTOCONNECT_URI_UPDATE page
// by the JavaScript function included in the _pageUpdate AutoConnectAux.
const AutoConnectOTA::ACPage_t AutoConnectOTA::_pageResult PROGMEM = {
  AUTOCONNECT_URI_UPDATE_ACT, AUTOCONNECT_MENULABEL_UPDATE, false, AutoConnectOTA::_elmResult
};
#endif // !_AUTOCONNECTOTAPAGE_H_
