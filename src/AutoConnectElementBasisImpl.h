/**
 * Implementation of AutoConnectElementBasis classes.
 * @file AutoConnectElementBasisImpl.h
 * @author hieromon@gmail.com
 * @version  1.3.6
 * @date 2022-07-27
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTELEMENTBASISIMPL_H_
#define _AUTOCONNECTELEMENTBASISIMPL_H_

#include <stdlib.h>
#include <stdio.h>
#if defined(ARDUINO_ARCH_ESP8266)
#include <regex.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <regex>
#endif
#include "AutoConnectElementBasis.h"
#include "AutoConnectFS.h"

namespace AutoConnectElementBasisImpl {
  template<size_t N>
  constexpr size_t  _sizeof(const char (&)[N]) { return N - sizeof('\0'); }
} // AutoConnectElementBasisImpl

/**
 * Append post-tag according by the post attribute.
 * @param  s  An original string
 * @return    A string that appended the post tag
 */
const String AutoConnectElementBasis::posterior(const String& s) const {
  String  html;

  static const char elmNoTempl[]  PROGMEM = "%s";
  static const char elmBrTempl[]  PROGMEM = "%s<br>";
  static const char elmParTempl[] PROGMEM = "<p>%s</p>";
  static const char elmDivTempl[] PROGMEM = "<div>%s</div>";
  PGM_P elmTempl;

  switch (post) {
  case AC_Tag_BR:
    elmTempl = (PGM_P)elmBrTempl;
    break;
  case AC_Tag_P:
    elmTempl = (PGM_P)elmParTempl;
    break;
  case AC_Tag_DIV:
    elmTempl = (PGM_P)elmDivTempl;
    break;
  case AC_Tag_None:
  default:
    elmTempl = (PGM_P)elmNoTempl;
    break;
  }

  size_t elmLen = strlen_P(elmTempl) + s.length() - AutoConnectElementBasisImpl::_sizeof("%s") + sizeof('\0');
  char* elm = new char[elmLen];
  if (elm) {
    snprintf_P(elm, elmLen, elmTempl, s.c_str());
    html = String(elm);
    delete elm;
  }
  return html;
}

/**
 * Generate an HTML <button> element. The onclick behavior depends on
 * the code held in factionf member.
 * @return  An HTML string.
 */
const String AutoConnectButtonBasis::toHTML(void) const {
  String  html;

  // Conversion of the AutoConnectButton element to HTML.
  // <button id="name" name="name" value="value" onclick="action">value</button>
  if (enable) {
    static const char elmButtonTempl[] PROGMEM = "<%s type=\"%s\" id=\"%s\" name=\"%s\" value=\"%s\" onclick=\"%s\">%s</%s>";
    static const char tagButton[] PROGMEM = "button";

    size_t  elmLen = (AutoConnectElementBasisImpl::_sizeof(elmButtonTempl) + (AutoConnectElementBasisImpl::_sizeof(tagButton) * 3) + (name.length() * 2) + (value.length() * 2) + action.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 8) + sizeof('\0') + 16) & (~0xf);
    char*   elmButton = new char[elmLen];
    if (elmButton) {
      snprintf_P(elmButton, elmLen, elmButtonTempl, (PGM_P)tagButton, (PGM_P)tagButton, name.c_str(), name.c_str(), value.c_str(), action.c_str(), value.c_str(), (PGM_P)tagButton);
      html = AutoConnectElementBasis::posterior(String(elmButton));
      delete elmButton;
    }
  }
  return html;
}

/**
 * Generate an HTML <input type=checkbox> element.
 * A "value" is associated with the input tag and sent by the form
 * action as the value of "name". If the label member is contained, it
 * is placed to the right side of the checkbox to be labeled.
 * f the label member is empty, only the checkbox is placed.
 * @return  An HTML string.
 */
const String AutoConnectCheckboxBasis::toHTML(void) const {
  String  html;

  if (enable) {
    // Conversion of the AutoConnectCheckbox element to HTML.
    // <input type="checkbox" id="name" name="name" value="value"[ checked]>
    // [<label for="name">label</label>]
    static const char elmCheckboxTempl[] PROGMEM = "%s<input type=\"checkbox\" id=\"%s\" name=\"%s\" value=\"%s\"%s>%s";
    static const char elmLabelTempl[] PROGMEM = "<%s for=\"%s\">%s</%s>";
    static const char elmNone[] PROGMEM = "";
    static const char tagLabel[] PROGMEM = "label";
    static const char attrChecked[] PROGMEM = " checked";
    PGM_P applyChecked = (PGM_P)elmNone;
    PGM_P elmLabelPre  = (PGM_P)elmNone;
    PGM_P elmLabelPost = (PGM_P)elmNone;
    char* elmLabel = nullptr;
    size_t  elmLen = 0;

    if (label.length()) {
      elmLen = ((AutoConnectElementBasisImpl::_sizeof(tagLabel) * 2) + AutoConnectElementBasisImpl::_sizeof(elmLabelTempl) + name.length() + label.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 4) + sizeof('\0') + 16) & (~0xf);
      elmLabel = new char[elmLen];
      if (elmLabel) {
        snprintf_P(elmLabel, elmLen, elmLabelTempl, (PGM_P)tagLabel, name.c_str(), label.c_str(), (PGM_P)tagLabel);
        if (labelPosition == AC_Infront)
          elmLabelPre = elmLabel;
        else if (labelPosition == AC_Behind)
          elmLabelPost = elmLabel;
      }
    }

    if (checked) {
      applyChecked = (PGM_P)attrChecked;
      elmLen += AutoConnectElementBasisImpl::_sizeof(attrChecked);
    }
    elmLen = (elmLen + (AutoConnectElementBasisImpl::_sizeof(tagLabel) * 2) + AutoConnectElementBasisImpl::_sizeof(elmCheckboxTempl) + (name.length() * 2) + value.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 6) + sizeof('\0') + 16) & (~0xf);
    char* elmCheckbox = new char[elmLen];
    if (elmCheckbox) {
      snprintf_P(elmCheckbox, elmLen, elmCheckboxTempl, elmLabelPre, name.c_str(), name.c_str(), value.c_str(), applyChecked, elmLabelPost);
      html = AutoConnectElementBasis::posterior(String(elmCheckbox));
      delete elmCheckbox;
    }

    if (elmLabel)
      delete elmLabel;
  }
  return html;
}

/**
 * Generate an HTML <input type=file> element.
 * The entered value can be obtained using the user callback function
 * registered by AutoConnectAux::on after the form is sent in
 * combination with AutoConnectSubmit.
 * @return String  an HTML string.
 */
const String AutoConnectFileBasis::toHTML(void) const {
  String  html;

  if (enable) {
    // Conversion of the AutoConnectFile element to HTML.
    // [<label for="name">label</label>]
    // <input type="file" id="name" name="name" accept="application/octet-stream">
    static const char elmFileTempl[] PROGMEM = "%s<input type=\"file\" id=\"%s\" name=\"%s\" accept=\"application/octet-steam\">";
    static const char elmLabelTempl[] PROGMEM ="<%s for=\"%s\">%s</%s>";
    static const char elmNone[] PROGMEM = "";
    static const char tagLabel[] PROGMEM = "label";
    PGM_P elmLabelPre = (PGM_P)elmNone;
    char* elmLabel = nullptr;
    size_t  elmLen = 0;

    if (label.length()) {
      elmLen = (AutoConnectElementBasisImpl::_sizeof(elmLabelTempl) + (AutoConnectElementBasisImpl::_sizeof(tagLabel) * 2) + name.length() + label.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 4) + sizeof('\0') + 16) & (~0xf);
      elmLabel = new char[elmLen];
      if (elmLabel) {
        snprintf_P(elmLabel, elmLen, elmLabelTempl, (PGM_P)tagLabel, name.c_str(), label.c_str(), (PGM_P)tagLabel);
        elmLabelPre = elmLabel;
      }
    }

    elmLen = (elmLen + AutoConnectElementBasisImpl::_sizeof(elmFileTempl) + (name.length() * 2) - (AutoConnectElementBasisImpl::_sizeof("%s") * 3) + sizeof('\0') + 16) & (~0xf);
    char*   elmFile = new char[elmLen];
    if (elmFile) {
      snprintf_P(elmFile, elmLen, elmFileTempl, elmLabelPre, name.c_str(), name.c_str());
      html = AutoConnectElementBasis::posterior(String(elmFile));
      delete elmFile;
    }

    if (elmLabel)
      delete elmLabel;
  }
  return html;
}

/**
 * Instantiate the upload handler with the specified store type.
 * @param store An enumeration value of ACFile_t
 */
bool AutoConnectFileBasis::attach(const ACFile_t store) {
  AutoConnectUploadFS*  handlerFS;
  AutoConnectUploadSD*  handlerSD;

  // Release previous handler
  detach();
  _status = AutoConnectUploadHandler::AC_UPLOAD_IDLE;
  // Classify a handler type and create the corresponding handler
  switch (store) {
  case AC_File_FS:
    handlerFS = new AutoConnectUploadFS(AUTOCONNECT_APPLIED_FILESYSTEM);
    _upload.reset(reinterpret_cast<AutoConnectUploadHandler*>(handlerFS));
    break;
  case AC_File_SD:
    handlerSD = new AutoConnectUploadSD(SD);
    _upload.reset(reinterpret_cast<AutoConnectUploadHandler*>(handlerSD));
    break;
  case AC_File_Extern:
    break;
  }
  return _upload ? true : false;
}

/**
 * Refresh the status of the upload to maintain the last status.
 * The upload progress status in AutoConnectUploadHandler is kept within
 * _status, but it does not indicate the latest status. This status function
 * takes the latest status of the upload handler into the internal _status value. 
 */
AutoConnectUploadHandler::AC_UPLOADStatus_t AutoConnectFileBasis::status(void) {
  if (_upload)
    _status = _upload.get()->status();
  return _status;
}

/**
 * Generate an HTML <input type=text> element.
 * If the value member is contained, it is reflected in the placeholder
 * attribute. The entered value can be obtained using the user callback
 * function registered by AutoConnectAux::on after the form is sent in
 * combination with AutoConnectSubmit.
 * @return String  an HTML string.
 */
const String AutoConnectInputBasis::toHTML(void) const {
  String  html;

  if (enable) {
    // Conversion of the AutoConnectInput element to HTML.
    // [<label for="name">label</label>]
    // <input type="number|password|text" id="name" name="name"[ pattern="pattern"][ placeholder="placeholder"][ value="value"][ style="style"]>
    static const char elmInputTempl[] PROGMEM = "%s<input type=\"%s\" id=\"%s\" name=\"%s\"%s%s%s%s>";
    static const char elmLabelTempl[] PROGMEM ="<%s for=\"%s\">%s</%s>";
    static const char elmNone[] PROGMEM = "";
    static const char tagLabel[] PROGMEM = "label";
    static const char attrNumber[] PROGMEM = "number";
    static const char attrPassword[] PROGMEM = "password";
    static const char attrText[] PROGMEM = "text";
    PGM_P attrType;
    PGM_P elmLabelPre = (PGM_P)elmNone;
    char* elmLabel = nullptr;
    size_t  elmLen = 0;

    if (label.length()) {
      elmLen = ((AutoConnectElementBasisImpl::_sizeof(tagLabel) * 2) + AutoConnectElementBasisImpl::_sizeof(elmLabelTempl) + name.length() + label.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 4) + sizeof('\0') + 16) & (~0xf);
      elmLabel = new char[elmLen];
      if (elmLabel) {
        snprintf_P(elmLabel, elmLen, elmLabelTempl, (PGM_P)tagLabel, name.c_str(), label.c_str(), (PGM_P)tagLabel);
        elmLabelPre = elmLabel;
      }
    }

    static const char attrPatternTempl[] PROGMEM = " pattern=\"%s\"";
    static const char attrPlaceholderTempl[] PROGMEM = " placeholder=\"%s\"";
    static const char attrValueTempl[] PROGMEM = " value=\"%s\"";
    static const char attrStyleTempl[] PROGMEM = " style=\"%s\"";
    PGM_P attrPattern = (PGM_P)elmNone;
    PGM_P attrPlaceholder = (PGM_P)elmNone;
    PGM_P attrValue = (PGM_P)elmNone;
    PGM_P attrStyle = (PGM_P)elmNone;
    char* applyPattern = nullptr;
    char* applyPlaceholder = nullptr;
    char* applyValue = nullptr;
    char* applyStyle = nullptr;

    if (pattern.length()) {
      size_t  attrLen = (AutoConnectElementBasisImpl::_sizeof(attrPatternTempl) + pattern.length() - AutoConnectElementBasisImpl::_sizeof("%s") + sizeof('\0') + 16) & (~0xf);
      applyPattern = new char[attrLen];
      if (applyPattern) {
        snprintf_P(applyPattern, attrLen, attrPatternTempl, pattern.c_str());
        attrPattern = applyPattern;
        elmLen += attrLen;
      }
    }

    if (placeholder.length()) {
      size_t  attrLen = (AutoConnectElementBasisImpl::_sizeof(attrPlaceholderTempl) + placeholder.length() - AutoConnectElementBasisImpl::_sizeof("%s") + sizeof('\0') + 16) & (~0xf);
      applyPlaceholder = new char[attrLen];
      if (applyPlaceholder) {
        snprintf_P(applyPlaceholder, attrLen, attrPlaceholderTempl, placeholder.c_str());
        attrPlaceholder = applyPlaceholder;
        elmLen += attrLen;
      }
    }

    if (value.length()) {
      size_t  attrLen = (AutoConnectElementBasisImpl::_sizeof(attrValueTempl) + value.length() - AutoConnectElementBasisImpl::_sizeof("%s") + sizeof('\0') + 16) & (~0xf);
      applyValue = new char[attrLen];
      if (applyValue) {
        snprintf_P(applyValue, attrLen, attrValueTempl, value.c_str());
        attrValue = applyValue;
        elmLen += attrLen;
      }
    }

    if (style.length()) {
      size_t  attrLen = (AutoConnectElementBasisImpl::_sizeof(attrStyleTempl) + style.length() - AutoConnectElementBasisImpl::_sizeof("%s") + sizeof('\0') + 16) & (~0xf);
      applyStyle = new char[attrLen];
      if (applyStyle) {
        snprintf_P(applyStyle, attrLen, attrStyleTempl, style.c_str());
        attrStyle = applyStyle;
        elmLen += attrLen;
      }
    }

    switch (apply) {
    case AC_Input_Number:
      attrType = (PGM_P)attrNumber;
      break;
    case AC_Input_Password:
      attrType = (PGM_P)attrPassword;
      break;
    case AC_Input_Text:
    default:
      attrType = (PGM_P)attrText;
      break;
    }

    elmLen = (elmLen + AutoConnectElementBasisImpl::_sizeof(elmInputTempl) + strlen_P(attrType) + (name.length() * 2) - (AutoConnectElementBasisImpl::_sizeof("%s") * 8) + sizeof('\0') + 16) & (~0xf);
    char* elmInput = new char[elmLen];
    if (elmInput) {
      snprintf_P(elmInput, elmLen, elmInputTempl, elmLabelPre, attrType, name.c_str(), name.c_str(), attrPattern, attrPlaceholder, attrValue, attrStyle);
      html = AutoConnectElementBasis::posterior(String(elmInput));
      delete elmInput;
    }

    if (applyPattern)
      delete applyPattern;
    if (applyPlaceholder)
      delete applyPlaceholder;
    if (applyValue)
      delete applyValue;
    if (applyStyle)
      delete attrStyle;
    if (elmLabel)
      delete elmLabel;
  }
  return html;
}

/**
 * Evaluate the pattern as a regexp and return whether value matches.
 * Always return true if the pattern is undefined.
 * @return true  The value matches a pattern.
 * @return false The value does not match a pattern.
 */
bool AutoConnectInputBasis::isValid(void) const {
  bool  rc = true;
  if (pattern.length()) {
#if defined(ARDUINO_ARCH_ESP8266)
    regex_t preg;
    if (regcomp(&preg, pattern.c_str(), REG_EXTENDED) != 0) {
      AC_DBG("%s regex compile failed\n", pattern.c_str());
      rc = false;
    }
    else {
      regmatch_t  p_match[1];
      rc = regexec(&preg, value.c_str(), 1, p_match, 0) == 0 ? true : false;
      regfree(&preg);
    }
#elif defined(ARDUINO_ARCH_ESP32)
    const std::regex  re(std::string(pattern.c_str()));
    rc = std::regex_match(value.c_str(), re);
#endif
  }
  return rc;
}

/**
* Indicate an entry with the specified value in the value's collection.
* @param value     The value to indicates in the collection.
*/
void AutoConnectRadioBasis::check(const String& value) {
  for (std::size_t n = 0; n < _values.size(); n++) {
    if (at(n).equalsIgnoreCase(value)) {
      checked = n + 1;
      break;
    }
  }
}

/**
 * Clear value items of AutoConnectRadio and reallocate new storage.
 * All hold items are released.
 * @param reserve  If 'reserve' is greater than 0, this function
 * allocates new holding storage with the value.
 */
void AutoConnectRadioBasis::empty(const size_t reserve) {
  _values.clear();
  std::vector<String>().swap(_values);
  if (reserve)
    _values.reserve(reserve);
  checked = 0;
}

/**
 * Generate an HTML <input type=radio> element with an <option> element.
 * @return String  an HTML string.
 */
const String AutoConnectRadioBasis::toHTML(void) const {
  String  html;

  if (enable) {
    // Conversion of the AutoConnectRadio element to HTML.
    // [<label>label</label>[<br>]]
    // <input type="radio" id="name_N" name="name" value=values[N]"[ checked]><label for="name_N">values[N]</label>[<br>]
    static const char elmRadioTempl[] PROGMEM = "<input type=\"radio\" id=\"%s_%" PRIu8 "\" name=\"%s\" value=\"%s\"%s><%s for=\"%s_%" PRIu8 "\">%s</%s>%s%s";
    static const char elmLabelTempl[] PROGMEM = "<%s>%s</%s>%s";
    static const char elmNone[] PROGMEM = "";
    static const char tagLabel[] PROGMEM = "label";
    static const char attrChecked[] PROGMEM = " checked";
    static const char tagBr[] PROGMEM = "<br>";
    PGM_P applyBr = (PGM_P)elmNone;
    PGM_P applyChecked;
    size_t  elmLen = 0;

    if (order == AC_Vertical) {
      applyBr = (PGM_P)tagBr;
      elmLen = AutoConnectElementBasisImpl::_sizeof(tagBr);
    }

    if (label.length()) {
      elmLen = (elmLen + AutoConnectElementBasisImpl::_sizeof(elmLabelTempl) + (AutoConnectElementBasisImpl::_sizeof(tagLabel) * 2) + label.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 4) + sizeof('\0') + 16) & (~0xf);
      char* elmLabel = new char[elmLen];
      if (elmLabel) {
        snprintf_P(elmLabel, elmLen, elmLabelTempl, (PGM_P)tagLabel, label.c_str(), (PGM_P)tagLabel, applyBr);
        html = String(elmLabel);
        delete elmLabel;
      }
    }

    uint8_t n = 0;
    for (const String& value : _values) {
      applyChecked = (PGM_P)elmNone;
      PGM_P applyTag = (PGM_P)elmNone;
      elmLen = AutoConnectElementBasisImpl::_sizeof(elmRadioTempl) + (name.length() * 3) + (value.length() * 2) - (AutoConnectElementBasisImpl::_sizeof("%s") * 10) - (AutoConnectElementBasisImpl::_sizeof(PRIu8) * 2) + (AutoConnectElementBasisImpl::_sizeof("99") * 2) + (AutoConnectElementBasisImpl::_sizeof(tagLabel) * 2) + sizeof('\0');
      n++;
      if (n == checked) {
        applyChecked = (PGM_P)attrChecked;
        elmLen += AutoConnectElementBasisImpl::_sizeof(attrChecked);
      }
      if (n <= tags.size()) {
        applyTag = tags[n - 1].c_str();
        elmLen += tags[n - 1].length();
      }
      elmLen = (elmLen + 16) & (~0xf);
      char* elmRadio = new char[elmLen];
      if (elmRadio) {
        snprintf_P(elmRadio, elmLen, elmRadioTempl, name.c_str(), n, name.c_str(), value.c_str(), applyChecked, (PGM_P)tagLabel, name.c_str(), n, value.c_str(), tagLabel, applyBr, applyTag);
        html += String(elmRadio);
        delete elmRadio;
      }
    }

    if (order == AC_Horizontal)
      html = AutoConnectElementBasis::posterior(html);
  }
  return html;
}

/**
 * Returns current selected value in the radio same group
 */
const String& AutoConnectRadioBasis::value(void) const {
  static const String _nullString = String();
  return checked ? _values.at(checked - 1) : _nullString;
}

/**
 * Generate an HTML <input type=range> element.
 * If the magnify is true, place a span field to display the current
 * value. The entered value can be obtained using the user callback
 * function registered by AutoConnectAux::on after the form is sent in
 * combination with AutoConnectSubmit.
 * @return String  an HTML string.
 */
const String AutoConnectRangeBasis::toHTML(void) const {
  String  html;

  if (enable) {
    // Conversion of the AutoConnectRange element to HTML.
    // [<label for="name">label</label>]
    // [<span class="magnify" style="padding-right:3px;text-align:left>value</span>]
    // <input type="range" id="name" name="name" value="value" min="min" max="max"[ step="step"][ style="style"][ oninput="_ma(this,'p|n')"]>
    // [<span class="magnify" style="padding-left:3px;text-align:right>value</span>]
    static const char elmRangeTempl[] PROGMEM = "%s%s<input type=\"range\" id=\"%s\" name=\"%s\" value=\"%d\" min=\"%d\" max=\"%d\"%s%s%s>%s";
    static const char attrOninputTempl[] PROGMEM = " oninput=\"_ma(this,'%c')\"";
    static const char attrStepTempl[] PROGMEM = " step=\"%d\"";
    static const char attrStyleTempl[] PROGMEM = " style=\"%s\"";
    static const char elmSpanTempl[] PROGMEM = "<%s class=\"magnify\" style=\"padding-%s:3px;text-align:%s\">%d</%s>";
    static const char elmLabelTempl[] PROGMEM = "<%s for=\"%s\">%s</%s>";
    static const char elmNone[] PROGMEM = "";
    static const char tagLabel[] PROGMEM = "label";
    static const char tagSpan[] PROGMEM = "span";
    static const char attrRight[] PROGMEM = "right";
    static const char attrLeft[] PROGMEM = "left";
    PGM_P elmLabelPre = (PGM_P)elmNone;
    PGM_P elmSpanPre  = (PGM_P)elmNone;
    PGM_P elmSpanPost = (PGM_P)elmNone;
    PGM_P attrOninput = (PGM_P)elmNone;
    PGM_P attrStep = (PGM_P)elmNone;
    PGM_P attrStyle = (PGM_P)elmNone;
    PGM_P posPadding;
    PGM_P posAlign;
    char  posMagnify;
    char* elmLabel = nullptr;
    char* elmSpan = nullptr;
    char* applyOninput = nullptr;
    char* applyStep = nullptr;
    char* applyStyle = nullptr;
    size_t  elmLen = 0;

    if (label.length()) {
      elmLen = (AutoConnectElementBasisImpl::_sizeof(elmLabelTempl) + (AutoConnectElementBasisImpl::_sizeof(tagLabel) * 2) + name.length() + label.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 4) + sizeof('\0') + 16) & (~0xf);
      elmLabel = new char[elmLen];
      if (elmLabel) {
        snprintf_P(elmLabel, elmLen, elmLabelTempl, (PGM_P)tagLabel, name.c_str(), label.c_str(), (PGM_P)tagLabel);
        elmLen = strlen(elmLabel);
        elmLabelPre = elmLabel;
      }
    }

    switch (magnify) {
    case AC_Infront:
      posMagnify = 'p';
      posPadding = (PGM_P)attrRight;
      posAlign = (PGM_P)attrLeft;
      break;
    case AC_Behind:
      posMagnify = 'n';
      posPadding = (PGM_P)attrLeft;
      posAlign = (PGM_P)attrRight;
      break;
    case AC_Void:
    default:
      posMagnify = '\0';
      posPadding = (PGM_P)elmNone;
      posAlign = (PGM_P)elmNone;
    }

    if (magnify != AC_Void) {
      size_t  elmSpanLen = (AutoConnectElementBasisImpl::_sizeof(elmSpanTempl) + (AutoConnectElementBasisImpl::_sizeof(tagSpan) * 2) + AutoConnectElementBasisImpl::_sizeof(attrRight) + AutoConnectElementBasisImpl::_sizeof(attrLeft) + 4 - (AutoConnectElementBasisImpl::_sizeof("%s") * 4 + AutoConnectElementBasisImpl::_sizeof("%d")) + sizeof('\0') + 16) & (~0xf);
      elmSpan = new char[elmSpanLen];
      if (elmSpan) {
        snprintf_P(elmSpan, elmSpanLen, elmSpanTempl, (PGM_P)tagSpan, posPadding, posAlign, value, (PGM_P)tagSpan);
        elmLen += strlen(elmSpan);
        if (magnify == AC_Infront)
          elmSpanPre = elmSpan;
        else if (magnify == AC_Behind)
          elmSpanPost = elmSpan;
      }
      size_t  attrOninputLen = (AutoConnectElementBasisImpl::_sizeof(attrOninputTempl) + sizeof(posMagnify) - AutoConnectElementBasisImpl::_sizeof("%c") + sizeof('\0') + 16) & (~0xf);
      applyOninput = new char[attrOninputLen];
      if (applyOninput) {
        snprintf_P(applyOninput, attrOninputLen, attrOninputTempl, posMagnify);
        elmLen += strlen(applyOninput);
        attrOninput = applyOninput;
      }
    }

    if (step != 1) {
      size_t  attrStepLen = (AutoConnectElementBasisImpl::_sizeof(attrStepTempl) + AutoConnectElementBasisImpl::_sizeof("999") - AutoConnectElementBasisImpl::_sizeof("%d") + sizeof('\0') + 16) & (~0xf);
      applyStep = new char[attrStepLen];
      if (applyStep) {
        snprintf_P(applyStep, attrStepLen, attrStepTempl, step);
        elmLen += strlen(applyStep);
        attrStep = applyStep;
      }
    }

    if (style.length()) {
      size_t  attrStyleLen = (AutoConnectElementBasisImpl::_sizeof(attrStyleTempl) + style.length() - AutoConnectElementBasisImpl::_sizeof("%s") + sizeof('\0') + 16) & (~0xf);
      applyStyle = new char[attrStyleLen];
      if (applyStyle) {
        snprintf_P(applyStyle, attrStyleLen, attrStyleTempl, style.c_str());
        elmLen += strlen(applyStyle);
        attrStyle = applyStyle;
      }
    }

    elmLen = (elmLen + AutoConnectElementBasisImpl::_sizeof(elmRangeTempl) + (name.length() * 2) + (AutoConnectElementBasisImpl::_sizeof("00000") * 3) - (AutoConnectElementBasisImpl::_sizeof("%s") * 8) - (AutoConnectElementBasisImpl::_sizeof("%d") * 3) + sizeof('\0') + 16) & (~0xf);
    char* elmRange = new char[elmLen];
    if (elmRange) {
      snprintf_P(elmRange, elmLen, elmRangeTempl, elmLabelPre, elmSpanPre, name.c_str(), name.c_str(), value, min, max, attrStep, attrStyle, attrOninput, elmSpanPost);
      html = AutoConnectElementBasis::posterior(String(elmRange));
      delete elmRange;
    }
    
    if (elmLabel)
      delete elmLabel;
    if (elmSpan)
      delete elmSpan;
    if (applyOninput)
      delete applyOninput;
    if (applyStep)
      delete applyStep;
    if (applyStyle)
      delete applyStyle;
  }
  return html;
}

/**
 * Clear option items of AutoConnectSelect and reallocate new storage.
 * All hold items are released.
 * @param reserve  If 'reserve' is greater than 0, this function
 * allocates new holding storage with the value.
 */
void AutoConnectSelectBasis::empty(const size_t reserve) {
  _options.clear();
  std::vector<String>().swap(_options);
  if (reserve)
    _options.reserve(reserve);
  selected = 0;
}

/**
* Indicate an entry with the specified value in the value's collection.
* @param value     The value to indicates in the collection.
*/
void AutoConnectSelectBasis::select(const String& value) {
  for (std::size_t n = 0; n < _options.size(); n++) {
    if (at(n).equalsIgnoreCase(value)) {
      selected = n + 1;
      break;
    }
  }
}

/**
 * Generate an HTML <select> element with an <option> element.
 * The attribute value of the <option> element is given to the
 * AutoConnectSelect class as a string array, which would be stored
 * in the 'options' member. If a label member is contained, the <label>
 * element would be generated the preface of <select>.
 * @return String  an HTML string.
 */
const String AutoConnectSelectBasis::toHTML(void) const {
  String  html;

  if (enable) {
    // Conversion of the AutoConnectSelect element to HTML.
    // [<label for="name">label</label>]
    // <select name="name" id="name">
    // <option value="_options[N]"[ selected]</option>
    // </select>
    static const char elmSelectTempl[] PROGMEM = "%s<select id=\"%s\" name=\"%s\">%s</select>";
    static const char elmOptionTempl[] PROGMEM = "<option value=\"%s\"%s>%s</option>";
    static const char attrSelected[] PROGMEM = " selected";
    static const char elmLabelTempl[] PROGMEM = "<%s for=\"%s\">%s</%s>";
    static const char elmNone[] PROGMEM = "";
    static const char tagLabel[] PROGMEM = "label";
    PGM_P elmLabelPre = (PGM_P)elmNone;
    char* elmLabel = nullptr;
    size_t  elmLen = 0;


    if (label.length()) {
      elmLen = ((AutoConnectElementBasisImpl::_sizeof(tagLabel) * 2) + AutoConnectElementBasisImpl::_sizeof(elmLabelTempl) + name.length() + label.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 4) + sizeof('\0') + 16) & (~0xf);
      elmLabel = new char[elmLen];
       if (elmLabel) {
        snprintf_P(elmLabel, elmLen, elmLabelTempl, (PGM_P)tagLabel, name.c_str(), label.c_str(), (PGM_P)tagLabel);
        elmLabelPre = elmLabel;
      }
    }

    String  elmOptions;
    uint8_t n = 1;
    for (const String option : _options) {
      size_t  elmOptionLen = AutoConnectElementBasisImpl::_sizeof(elmOptionTempl) + (option.length() * 2) - (AutoConnectElementBasisImpl::_sizeof("%s") * 3);
      PGM_P applySelected = (PGM_P)elmNone;
      if (n++ == selected) {
        applySelected = (PGM_P)attrSelected;
        elmOptionLen += AutoConnectElementBasisImpl::_sizeof(attrSelected);
      }
      elmOptionLen = (elmOptionLen + sizeof('\0') + 16) & (~0xf);
      char* elmOption = new char[elmOptionLen];
      if (elmOption) {
        snprintf_P(elmOption, elmOptionLen, elmOptionTempl, option.c_str(), applySelected, option.c_str());
        elmOptions += String(elmOption);
        elmLen += strlen(elmOption);
        delete elmOption;
      }
    }

    elmLen = (elmLen + AutoConnectElementBasisImpl::_sizeof(elmSelectTempl) + (name.length() * 2) - (AutoConnectElementBasisImpl::_sizeof("%s") * 4) + sizeof('\0') + 16) & (~0xf);
    char* elmSelect = new char[elmLen];
    if (elmSelect)
      snprintf_P(elmSelect, elmLen, elmSelectTempl, elmLabelPre, name.c_str(), name.c_str(), elmOptions.c_str());

    elmOptions.~String();
    if (elmLabel)
      delete elmLabel;

    if (elmSelect) {
      html = AutoConnectElementBasis::posterior(String(elmSelect));
      delete elmSelect;
    }
  }
  return html;
}

/**
 * Returns current selected value in the radio same group
 */
const String& AutoConnectSelectBasis::value(void) const {
  static const String _nullString = String();
  return selected ? _options.at(selected - 1) : _nullString;
}

/**
 * Generate an HTML <input type=button> element. This element is used
 * for form submission. An 'onclick' attribute calls fixed JavaScript
 * code as 'sa' named and it's included in the template.
 * @return String  an HTML string.
 */
const String AutoConnectSubmitBasis::toHTML(void) const {
  String  html;

  if (enable) {
    // Conversion of the AutoConnectSubmit element to HTML.
    // <input type="button" name="name" value="value" onclick="_sa('uri')">
    static const char elmSubmitTempl[] PROGMEM ="<input type=\"button\" name=\"%s\" value=\"%s\" onclick=\"_sa('%s')\">";
    size_t elmLen = (AutoConnectElementBasisImpl::_sizeof(elmSubmitTempl) + name.length() + value.length() + uri.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 3) + sizeof('\0') + 16) & (~0xf);
    char*  elmSubmit = new char[elmLen];
    if (elmSubmit) {
      snprintf_P(elmSubmit, elmLen, elmSubmitTempl, name.c_str(), value.c_str(), uri.c_str());
      html = AutoConnectElementBasis::posterior(String(elmSubmit));
      delete elmSubmit;
    }
  }
  return html;
}

/**
 * Generate an HTML text element from a string of the value member. If a style
 * exists, it gives a style attribute.
 * @return String  an HTML string.
 */
const String AutoConnectTextBasis::toHTML(void) const {
  String  html;

  if (enable) {
    String  value_f = value;

    // Obtain a formatted value in advance.
    if (format.length()) {
      size_t  buflen = (value.length() + format.length() + sizeof('\0') + 16) & (~0xf);
      char*   buffer = new char[buflen];
      if (buffer) {
        snprintf(buffer, buflen, format.c_str(), value.c_str());
        value_f = String(buffer);
        delete buffer;
      }
    }

    // Conversion of the AutoConnectText element to HTML according to the post attributes.
    // <span id="name"[ style="style"]>formatted value</span>[<br>]
    // <p id="name"[ style="style"]>formatted value</p>
    // <div id="name"[ style="style"]>formatted value</div>
    static const char elmTextTempl[] PROGMEM = "<%s id=\"%s\"%s>%s</%s>%s";
    static const char elmNone[] PROGMEM = "";
    static const char tagSpan[] PROGMEM = "span";
    static const char tagPar[] PROGMEM = "p";
    static const char tagDiv[] PROGMEM = "Div";
    static const char tagBr[] PROGMEM = "<br>";
    static const char attrStyleTempl[] PROGMEM = " style=\"%s\"";
    PGM_P elmBr = (PGM_P)elmNone;
    PGM_P applyTag = (PGM_P)elmNone;
    PGM_P attrStyle = (PGM_P)elmNone;
    char* applyStyle = nullptr;
    size_t elmLen = 0;
    
    if (style.length()) {
      elmLen = (AutoConnectElementBasisImpl::_sizeof(attrStyleTempl) + style.length() - AutoConnectElementBasisImpl::_sizeof("%s") + sizeof('\0') + 16) & (~0xf);
      applyStyle = new char[elmLen];
      if (applyStyle) {
        snprintf_P(applyStyle, elmLen, attrStyleTempl, style.c_str());
        attrStyle = applyStyle;
      }
    }

    switch (post) {
    case AC_Tag_BR:
      elmBr = (PGM_P)tagBr;
      elmLen += AutoConnectElementBasisImpl::_sizeof(tagBr);
      [[fallthrough]];
    case AC_Tag_None:
      applyTag = (PGM_P)tagSpan;
      elmLen += AutoConnectElementBasisImpl::_sizeof(tagSpan) * 2;
      break;
    case AC_Tag_P:
      applyTag = (PGM_P)tagPar;
      elmLen += AutoConnectElementBasisImpl::_sizeof(tagPar) * 2;
      break;
    case AC_Tag_DIV:
      applyTag = (PGM_P)tagDiv;
      elmLen += AutoConnectElementBasisImpl::_sizeof(tagDiv) * 2;
      break;
    }

    elmLen = (elmLen + AutoConnectElementBasisImpl::_sizeof(elmTextTempl) + name.length() + value_f.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 6) + sizeof('\0') + 16) & (~0xf);
    char* elmText = new char[elmLen];
    if (elmText) {
      snprintf_P(elmText, elmLen, elmTextTempl, applyTag, name.c_str(), attrStyle, value_f.c_str(), applyTag, elmBr);
      html = String(elmText);
      delete elmText;
    }

    if (applyStyle)
      delete applyStyle;
  }
  return html;
}

#endif // _AUTOCONNECTELEMENTBASISIMPL_H_
