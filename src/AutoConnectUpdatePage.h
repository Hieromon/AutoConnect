/**
 * Define dialogs to operate sketch binary updates operated by the
 * AutoConnectUpdate class.
 * @file   AutoConnectUpdatePage.h
 * @author hieromon@gmail.com
 * @version    0.9.9
 * @date   2019-05-04
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTUPDATEPAGE_H
#define _AUTOCONNECTUPDATEPAGE_H

// Define the AUTOCONNECT_URI_UPDATE page to select the sketch binary
// for update and order update execution.
const AutoConnectUpdate::ACElementProp_t AutoConnectUpdate::_elmCatalog[] PROGMEM = {
  { AC_Text, "caption", nullptr, nullptr },
  { AC_Radio, "firmwares", nullptr, nullptr },
  { AC_Submit, "update", "UPDATE", AUTOCONNECT_URI_UPDATE_ACT }
};
const AutoConnectUpdate::ACPage_t AutoConnectUpdate::_auxCatalog PROGMEM = {
  AUTOCONNECT_URI_UPDATE, "Update", false, AutoConnectUpdate::_elmCatalog
};

// Define the AUTOCONNECT_URI_UPDATE_ACT page to display during the
// update process.
const AutoConnectUpdate::ACElementProp_t AutoConnectUpdate::_elmProgress[] PROGMEM = {
  { AC_Element, "caption", "<div style=\"display:inline-block\"", nullptr },
  { AC_Text, "binname", nullptr, "%s&ensp;from&ensp;" },
  { AC_Text, "url", nullptr, "%s</div>" },
  { AC_Element, "progress", "<div id=\"progress\">Updating...&ensp;<meter min=\"0\"></meter></div>", nullptr },
  { AC_Text, "status", nullptr, nullptr },
  { AC_Element, "inprogress", "<script type='text/javascript'>var ws;window.onload=function(){ws=new WebSocket('ws://'+'#wsserver#');ws.onopen=function(e){ws.onmessage=function(e){var pl=e.data.split(',');if(pl[0]=='#p'){var iv=pl[1].split(':');var pb=document.getElementById('progress').getElementsByTagName('meter');pb[0].setAttribute('value',iv[0]);pb[0].setAttribute('max',iv[1]);}else if(pl[0]=='#e'){location.href='" AUTOCONNECT_URI_UPDATE_RESULT "';}};ws.onclose=function(e){console.log('WS close('+e.code+') '+e.reason);};};ws.onerror=function(e){console.log(e);document.getElementById('status').textContent='Connection failed.';};};window.onbeforeunload=function(){ws.close();};</script>", nullptr }
};
const AutoConnectUpdate::ACPage_t AutoConnectUpdate::_auxProgress PROGMEM = {
  AUTOCONNECT_URI_UPDATE_ACT, "Update", false, AutoConnectUpdate::_elmProgress
};

// Definition of the AUTOCONNECT_URI_UPDATE_RESULT page to notify update results
const AutoConnectUpdate::ACElementProp_t AutoConnectUpdate::_elmResult[] PROGMEM = {
  { AC_Text, "status", nullptr, nullptr },
  { AC_Element, "restart", "<script type=\"text/javascript\">setTimeout(\"location.href='" AUTOCONNECT_URI "'\",1000*5);</script>", nullptr }
};
const AutoConnectUpdate::ACPage_t AutoConnectUpdate::_auxResult PROGMEM = {
  AUTOCONNECT_URI_UPDATE_RESULT, "Update", false, AutoConnectUpdate::_elmResult
};

#endif // _AUTOCONNECTUPDATEPAGE_H
