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
  { AC_Element, "spinner", "<style>.loader {border:2px solid #f3f3f3;border-radius:50%;border-top:2px solid #555;width:12px;height:12px;-webkit-animation:spin 1s linear infinite;animation:spin 1s linear infinite}@-webkit-keyframes spin{0%{-webkit-transform:rotate(0deg)}100%{-webkit-transform:rotate(360deg)}}@keyframes spin{0%{transform:rotate(0deg)}100%{transform:rotate(360deg)}}</style>", nullptr },
  { AC_Text, "caption", "Update start...", "<div style=\"font-size:120%%;font-weight:bold\">%s</div>" },
  { AC_Text, "flash", nullptr, "<div style=\"margin-top:18px\">%s<span style=\"display:inline-block;vertical-align:middle;margin-left:7px\"><div class=\"loader\"></div></span></div>" },
  { AC_Element, "inprogress", "<script type=\"text/javascript\">setTimeout(\"location.href='" AUTOCONNECT_URI_UPDATE_RESULT "'\",1000*15);</script>", nullptr }
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
