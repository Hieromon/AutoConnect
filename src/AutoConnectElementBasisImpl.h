/**
 * Implementation of AutoConnectElementBasis classes.
 * @file AutoConnectElementBasisImpl.h
 * @author hieromon@gmail.com
 * @version  1.3.5
 * @date 2022-06-15
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
  if (post == AC_Tag_BR)
    html = s + String(F("<br>"));
  else if (post == AC_Tag_P)
    html = String("<p>") + s + String(F("</p>"));
  else if (post == AC_Tag_DIV)
    html = String("<div>") + s + String(F("</div>"));
  else
    html = s;
  return html;
}

/**
 * Generate an HTML <button> element. The onclick behavior depends on
 * the code held in factionf member.
 * @return  An HTML string.
 */
const String AutoConnectButtonBasis::toHTML(void) const {
  String  html = String("");

  // Conversion of the AutoConnectButton element to HTML.
  // <button id="name" name="name" value="value" onclick="action">value</button>
  if (enable) {
    static const char elmButtonTempl[] PROGMEM = "<%s id=\"%s\" name=\"%s\" value=\"%s\" onclick=\"%s\">%s</%s>";
    static const char applyTag[] PROGMEM = "button";

    size_t  elmLen = (AutoConnectElementBasisImpl::_sizeof(elmButtonTempl) + (AutoConnectElementBasisImpl::_sizeof(applyTag) * 2) + (name.length() * 2) + (value.length() * 2) + action.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 7) + sizeof('\0') + 16) & (~0xf);
    char*   elmBuffer = new char[elmLen];
    if (elmBuffer) {
      snprintf_P(elmBuffer, elmLen, elmButtonTempl, (PGM_P)applyTag, name.c_str(), name.c_str(), value.c_str(), action.c_str(), value.c_str(), (PGM_P)applyTag);
      html = AutoConnectElementBasis::posterior(String(elmBuffer));
      delete elmBuffer;
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
  String  html = String("");

  if (enable) {
    // Conversion of the AutoConnectCheckbox element to HTML.
    // <input type="checkbox" id="name" name="name" value="value"[ checked]>
    // [<label for="name">label</label>]
    static const char elmCheckboxTempl[] PROGMEM = "<input type=\"checkbox\" id=\"%s\" name=\"%s\" value=\"%s\"%s>";
    static const char attrChecked[] PROGMEM = " checked";
    static const char tagLabel[] PROGMEM = "label";
    static const char elmLabelTempl[] PROGMEM = "<%s for=\"%s\">%s</%s>";
    PGM_P  applyChecked = PSTR("");
    size_t elmLen = 0;

    if (checked) {
      applyChecked = (PGM_P)attrChecked;
      elmLen = AutoConnectElementBasisImpl::_sizeof(attrChecked);
    }
    elmLen = (elmLen + (AutoConnectElementBasisImpl::_sizeof(tagLabel) * 2) + AutoConnectElementBasisImpl::_sizeof(elmCheckboxTempl) + (name.length() * 2) + value.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 4) + sizeof('\0') + 16) & (~0xf);
    char* elmBuffer = new char[elmLen];
    if (elmBuffer) {
      snprintf_P(elmBuffer, elmLen, elmCheckboxTempl, name.c_str(), name.c_str(), value.c_str(), applyChecked);
      html = String(elmBuffer);
      delete elmBuffer;
    }

    if (label.length()) {
      size_t elmLabelLen = ((AutoConnectElementBasisImpl::_sizeof(tagLabel) * 2) + AutoConnectElementBasisImpl::_sizeof(elmLabelTempl) + name.length() + label.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 4) + sizeof('\0') + 16) & (~0xf);
      char*  elmLabel = new char[elmLabelLen];
      if (elmLabel) {
        snprintf_P(elmLabel, elmLabelLen, elmLabelTempl, (PGM_P)tagLabel, name.c_str(), label.c_str(), (PGM_P)tagLabel);
        if (labelPosition == AC_Infront)
          html = String(elmLabel) + html;
        else if (labelPosition == AC_Behind)
          html += String(elmLabel);
        delete elmLabel;
      }
    }
    html = AutoConnectElementBasis::posterior(html);
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
  String  html = String("");

  if (enable) {
    // Conversion of the AutoConnectFile element to HTML.
    // [<label for="name">label</label>]
    // <input type="file" id="name" name="name" accept="application/octet-stream">
    static const char elmFileTempl[] PROGMEM = "%s<input type=\"file\" id=\"%s\" name=\"%s\" accept=\"application/octet-steam\">";
    static const char tagLabel[] PROGMEM = "label";
    static const char elmLabelTempl[] PROGMEM ="<%s for=\"%s\">%s</%s>";
    static const char*  applyLabel = "";
    char*   elmLabel = nullptr;
    size_t  elmLabelLen = 0;

    if (label.length()) {
      elmLabelLen = (AutoConnectElementBasisImpl::_sizeof(elmLabelTempl) + name.length() + label.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 4) + sizeof('\0') + 16) & (~0xf);
      elmLabel = new char[elmLabelLen];
      if (elmLabel) {
        snprintf_P(elmLabel, elmLabelLen, elmLabelTempl, (PGM_P)tagLabel, name.c_str(), label.c_str(), (PGM_P)tagLabel);
        applyLabel = elmLabel;
      }
    }

    size_t  elmLen = (elmLabelLen + AutoConnectElementBasisImpl::_sizeof(elmFileTempl) + (name.length() * 2) - (AutoConnectElementBasisImpl::_sizeof("%s") * 3) + sizeof('\0') + 16) & (~0xf);
    char*   elmBuffer = new char[elmLen];
    if (elmBuffer) {
      snprintf_P(elmBuffer, elmLen, elmFileTempl, applyLabel, name.c_str(), name.c_str());
      html = AutoConnectElementBasis::posterior(String(elmBuffer));
      delete elmBuffer;
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
const AutoConnectUploadHandler::AC_UPLOADStatus_t AutoConnectFileBasis::status(void) {
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
  String  html = String("");

  if (enable) {
    // Conversion of the AutoConnectInput element to HTML.
    // [<label for="name">label</label>]
    // <input type="number|password|text" id="name" name="name"[ pattern="pattern"][ placeholder="placeholder"][ value="value"][ style="style"]>
    static const char elmInputTempl[] PROGMEM = "%s<input type=\"%s\" id=\"%s\" name=\"%s\"%s%s%s%s>";
    static const char tagLabel[] PROGMEM = "label";
    static const char elmLabelTempl[] PROGMEM ="<%s for=\"%s\">%s</%s>";
    static const char*  applyLabel = "";
    char*   elmLabel = nullptr;
    size_t  elmLabelLen = 0;

    if (label.length()) {
      elmLabelLen = (AutoConnectElementBasisImpl::_sizeof(elmLabelTempl) + name.length() + label.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 4) + sizeof('\0') + 16) & (~0xf);
      elmLabel = new char[elmLabelLen];
       if (elmLabel) {
        snprintf_P(elmLabel, elmLabelLen, elmLabelTempl, (PGM_P)tagLabel, name.c_str(), label.c_str(), (PGM_P)tagLabel);
        applyLabel = elmLabel;
      }
    }

    PGM_P applyType;
    switch (apply) {
    case AC_Input_Number:
      applyType = PSTR("number");
      break;
    case AC_Input_Password:
      applyType = PSTR("password");
      break;
    case AC_Input_Text:
    default:
      applyType = PSTR("text");
      break;
    }

    static const char attrPatternTempl[] PROGMEM = " pattern=\"%s\"";
    static const char attrPlaceholderTempl[] PROGMEM = " placeholder=\"%s\"";
    static const char attrValueTempl[] PROGMEM = " value=\"%s\"";
    static const char attrStyleTempl[] PROGMEM = " style=\"%s\"";
    static const char*  applyPattern = "";
    static const char*  applyPlaceholder = "";
    static const char*  applyValue = "";
    static const char*  applyStyle = "";
    char*   attrPattern = nullptr;
    char*   attrPlaceholder = nullptr;
    char*   attrValue = nullptr;
    char*   attrStyle = nullptr;

    if (pattern.length()) {
      size_t  attrLen = (AutoConnectElementBasisImpl::_sizeof(attrPatternTempl) + pattern.length() - AutoConnectElementBasisImpl::_sizeof("%s") + sizeof('\0') + 16) & (~0xf);
      char  *attrPattern = new char[attrLen];
      if (attrPattern) {
        snprintf_P(attrPattern, attrLen, attrPatternTempl, pattern.c_str());
        applyPattern = attrPattern;
      }
    }

    if (placeholder.length()) {
      size_t  attrLen = (AutoConnectElementBasisImpl::_sizeof(attrPlaceholderTempl) + placeholder.length() - AutoConnectElementBasisImpl::_sizeof("%s") + sizeof('\0') + 16) & (~0xf);
      char  *attrPlaceholder = new char[attrLen];
      if (attrPlaceholder) {
        snprintf_P(attrPlaceholder, attrLen, attrPlaceholderTempl, placeholder.c_str());
        applyPlaceholder = attrPlaceholder;
      }
    }

    if (value.length()) {
      size_t  attrLen = (AutoConnectElementBasisImpl::_sizeof(attrValueTempl) + value.length() - AutoConnectElementBasisImpl::_sizeof("%s") + sizeof('\0') + 16) & (~0xf);
      char  *attrValue = new char[attrLen];
      if (attrValue) {
        snprintf_P(attrValue, attrLen, attrValueTempl, value.c_str());
        applyValue = attrValue;
      }
    }

    if (style.length()) {
      size_t  attrLen = (AutoConnectElementBasisImpl::_sizeof(attrStyleTempl) + style.length() - AutoConnectElementBasisImpl::_sizeof("%s") + sizeof('\0') + 16) & (~0xf);
      char  *attrStyle = new char[attrLen];
      if (attrStyle) {
        snprintf_P(attrStyle, attrLen, attrStyleTempl, style.c_str());
        applyStyle = attrStyle;
      }
    }

    size_t  elmLen = (strlen(applyLabel) + AutoConnectElementBasisImpl::_sizeof(elmInputTempl) + strlen(applyType) + (name.length() * 2) + strlen(applyPattern) + strlen(applyPlaceholder) + strlen(applyValue) + strlen(applyStyle) - (AutoConnectElementBasisImpl::_sizeof("%s") * 8) + sizeof('\0') + 16) & (~0xf);
    char* elmInput = new char[elmLen];
    if (elmInput) {
      snprintf_P(elmInput, elmLen, elmInputTempl, applyLabel, applyType, name.c_str(), name.c_str(), applyPattern, applyPlaceholder, applyValue, applyStyle);
      html = AutoConnectElementBasis::posterior(String(elmInput));
      delete elmInput;
    }

    if (elmLabel)
      delete elmLabel;
    if (attrPattern)
      delete attrPattern;
    if (attrPlaceholder)
      delete attrPlaceholder;
    if (attrValue)
      delete attrValue;
    if (attrStyle)
      delete attrStyle;
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
  String  html = String("");

  if (enable) {
    // Conversion of the AutoConnectRadio element to HTML.
    // [<label>label</label>][<br>]
    // <input type="radio" id="name_N" name="name" value=values[N]"[ checked]><label for="name_N">values[N]</label>[<br>]
    static const char elmInputTempl[] PROGMEM = "<input type=\"radio\" id=\"%s_%" PRIu8 "\" name=\"%s\" value=\"%s\"%s><%s for=\"%s_%" PRIu8 "\">%s</%s>%s%s";
    static const char elmLabelTempl[] PROGMEM = "<%s>%s</%s>%s";
    static const char tagLabel[] PROGMEM = "label";
    static const char attrChecked[] PROGMEM = " checked";
    static const char attrNone[] PROGMEM = "";
    static const char tagBr[] PROGMEM = "<br>";
    PGM_P applyBr;
    PGM_P applyChecked;

    if (label.length()) {
      size_t  elmLabelLen = AutoConnectElementBasisImpl::_sizeof(elmLabelTempl) + (AutoConnectElementBasisImpl::_sizeof(tagLabel) * 2) + label.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 4) + sizeof('\0');
      applyBr = order == AC_Vertical ? (PGM_P)tagBr : (PGM_P)attrNone;
      elmLabelLen += strlen(applyBr);
      elmLabelLen = (elmLabelLen + 16) & (~0xf);
      char* elmLabel = new char[elmLabelLen];
      if (elmLabel) {
        snprintf_P(elmLabel, elmLabelLen, elmLabelTempl, (PGM_P)tagLabel, label.c_str(), (PGM_P)tagLabel, applyBr);
        html = String(elmLabel);
        delete elmLabel;
      }
    }

    uint8_t n = 0;
    for (const String& value : _values) {
      applyChecked = (PGM_P)attrNone;
      applyBr = (PGM_P)attrNone;
      n++;
      size_t  elmLen = AutoConnectElementBasisImpl::_sizeof(elmInputTempl) + (name.length() * 4) + (value.length() * 2) - (AutoConnectElementBasisImpl::_sizeof("%s") * 10) - ((AutoConnectElementBasisImpl::_sizeof(PRIu8) - AutoConnectElementBasisImpl::_sizeof("99")) * 2) + sizeof('\0');
      if (n == checked) {
        applyChecked =(PGM_P)attrChecked;
        elmLen += AutoConnectElementBasisImpl::_sizeof(attrChecked);
      }
      if (order == AC_Vertical) {
        applyBr =(PGM_P)tagBr; 
        elmLen += AutoConnectElementBasisImpl::_sizeof(tagBr);
      }
      elmLen = (elmLen + 16) & (~0xf);
      char* elmBuffer = new char[elmLen];
      if (elmBuffer) {
        PGM_P applyTag = (PGM_P)attrNone;
        if (n <= tags.size())
          applyTag = tags[n -1].c_str();
        snprintf_P(elmBuffer, elmLen, elmInputTempl, name.c_str(), n, name.c_str(), value.c_str(), applyChecked, (PGM_P)tagLabel, name.c_str(), n, value.c_str(), tagLabel, applyBr, applyTag);
        html += String(elmBuffer);
        delete elmBuffer;
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
  String  html = String("");

  if (enable) {
    // Conversion of the AutoConnectRange element to HTML.
    // [<label for="name">label</label>][<br>]
    // <input type="radio" id="name" name="name" value=values[N]"[ checked]><label for="id_N">values[N]</label>[<br>]
    if (label.length())
      html = String(F("<label for=\"")) + name + String("\">") + label + String(F("</label>"));
    
    String  dispFil("");
    String  onInput("");
    char  posMagnify;
    PGM_P posRight = PSTR("right");
    PGM_P posLeft = PSTR("left");
    PGM_P posPadding;
    PGM_P posAlign;
    switch (magnify) {
    case AC_Infront:
      posMagnify = 'p';
      posPadding = posRight;
      posAlign = posLeft;
      break;
    case AC_Behind:
      posMagnify = 'n';
      posPadding = posLeft;
      posAlign = posRight;
      break;
    case AC_Void:
    default:
      posMagnify = '\0';
      posPadding = nullptr;
      posAlign = nullptr;
    }
    if  (magnify != AC_Void) {
      dispFil = String(F("<span class=\"magnify\" style=\"padding-")) + String(posPadding) + String(F(":3px;text-align:")) + String(posAlign) + String("\">") + String(value) + String(F("</span>"));
      onInput = String(F(" oninput=\"_ma(this,'")) + String(posMagnify) + String(F("')\""));
    }

    if (magnify == AC_Infront)
      html += dispFil;

    html += String(F("<input type=\"range\" name=\"")) + name + String(F("\" id=\"")) + name + String(F("\" value=\"")) + String(value) + String(F("\" min=\"")) + String(min) + String(F("\" max=\"")) + String(max) + String("\"");
    if (step != 1)
      html += String(F(" step=\"")) + String(step) + String("\"");
    html += onInput;
    if (style.length())
      html += String(F(" style=\"")) + style + String("\"");
    html += String(">");

    if (magnify == AC_Behind)
      html += dispFil;

    html = AutoConnectElementBasis::posterior(html);
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
  String  html = String("");

  if (enable) {
    if (label.length())
      html = String(F("<label for=\"")) + name + String("\">") + label + String(F("</label>"));
    html += String(F("<select name=\"")) + name + String(F("\" id=\"")) + name + String("\">");
    uint8_t n = 1;
    for (const String option : _options) {
      html += String(F("<option value=\"")) + option + "\"";
      if (n++ == selected)
        html += String(F(" selected"));
      html += ">" + option + String(F("</option>"));
    }
    html += String(F("</select>"));
    html = AutoConnectElementBasis::posterior(html);
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
  String  html = String("");

  if (enable) {
    // Conversion of the AutoConnectSubmit element to HTML.
    // <input type="button" name="name" value="value" onclick="_sa('uri')">
    static const char elmSubmitTempl[] PROGMEM ="<input type=\"button\" name=\"%s\" value=\"%s\" onclick=\"_sa('%s')\">";
    size_t elmLen = (AutoConnectElementBasisImpl::_sizeof(elmSubmitTempl) + name.length() + value.length() + uri.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 3) + sizeof('\0') + 16) & (~0xf);
    char*  elmSubmit = new char[elmLen];
    if (elmSubmit) {
      snprintf_P(elmSubmit, elmLen, name.c_str(), value.c_str(), uri.c_str());
      delete elmSubmit;
    }
    html = AutoConnectElementBasis::posterior(String(elmSubmit));
  }
  return html;
}

/**
 * Generate an HTML text element from a string of the value member. If a style
 * exists, it gives a style attribute.
 * @return String  an HTML string.
 */
const String AutoConnectTextBasis::toHTML(void) const {
  String  html = String("");

  if (enable) {
    String  value_f = value;

    if (format.length()) {
      size_t  buflen = (value.length() + format.length() + 16 + 1) & (~0xf);
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
    PGM_P applyTag = PSTR("");
    PGM_P br = PSTR("");
    
    switch (post) {
    case AC_Tag_BR:
      br = PSTR("<br>");
    case AC_Tag_None:
      applyTag = PSTR("span");
      break;
    case AC_Tag_P:
      applyTag = PSTR("p");
      break;
    case AC_Tag_DIV:
      applyTag = PSTR("div");
      break;
    }

    static const char attrStyleTempl[] PROGMEM = " style=\"%s\"";
    static const char* applyStyle = "";
    size_t  buflen = (AutoConnectElementBasisImpl::_sizeof(attrStyleTempl) + style.length() - AutoConnectElementBasisImpl::_sizeof("%s") + 16 + 1) & (~0xf);
    char*   styleBuffer = new char[buflen];

    if (style.length()) {
      if (styleBuffer) {
        snprintf_P(styleBuffer, buflen, attrStyleTempl, style.c_str());
        applyStyle = styleBuffer;
      }
    }

    size_t elmLen = (AutoConnectElementBasisImpl::_sizeof(elmTextTempl) + (strlen(applyTag) * 2) + strlen(applyStyle) + strlen(br) + name.length() + value_f.length() - (AutoConnectElementBasisImpl::_sizeof("%s") * 6) + 16 + 1) & (~0xf);
    char*  elmBuffer = new char[elmLen];
    if (elmBuffer) {
      snprintf_P(elmBuffer, elmLen, elmTextTempl, applyTag, name.c_str(), applyStyle, value_f.c_str(), applyTag, br);
      html = String(elmBuffer);
      delete elmBuffer;
    }

    if (styleBuffer)
      delete styleBuffer;
  }
  return html;
}

#endif // _AUTOCONNECTELEMENTBASISIMPL_H_
