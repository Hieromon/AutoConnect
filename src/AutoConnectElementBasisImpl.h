/**
 * Implementation of AutoConnectElementBasis classes.
 * @file AutoConnectElementBasisImpl.h
 * @author hieromon@gmail.com
 * @version  1.3.2
 * @date 2021-11-24
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

  if (enable) {
    html = String(F("<button type=\"button\" name=\"")) + name + String(F("\" value=\"")) + value + String(F("\" onclick=\"")) + action + String("\">") + value + String(F("</button>"));
    html = AutoConnectElementBasis::posterior(html);
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
    html = String(F("<input type=\"checkbox\" name=\"")) + name + String(F("\" value=\"")) + value + String("\"");
    if (checked)
      html += String(F(" checked"));
    if (label.length())
      html += String(F(" id=\"")) + name + String("\">");
    if (label.length()) {
      String  labelTag = String(F("<label for=\"")) + name + String(F("\">")) + label + String(F("</label>"));
      if (labelPosition == AC_Infront)
        html = labelTag + html;
      else if (labelPosition == AC_Behind)
        html += labelTag;
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
    if (label.length())
      html = String(F("<label for=\"")) + name + String(F("\">")) + label + String(F("</label>"));
    html += String(F("<input type=\"file\" id=\"")) + name + String(F("\" name=\"")) + name + String("\">");
    html = AutoConnectElementBasis::posterior(html);
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
    if (label.length())
      html = String(F("<label for=\"")) + name + String("\">") + label + String(F("</label>"));
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
    html += String(F("<input type=\"")) + String(applyType) + String(F("\" id=\"")) + name + String(F("\" name=\"")) + name + String("\"");
    if (pattern.length())
      html += String(F(" pattern=\"")) + pattern + String("\"");
    if (placeholder.length())
      html += String(F(" placeholder=\"")) + placeholder + String("\"");
    if (value.length())
      html += String(F(" value=\"")) + value + String("\"");
    if (style.length())
      html += String(F(" style=\"")) + style + String("\"");
    html += String(">");
    html = AutoConnectElementBasis::posterior(html);
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
    if (label.length()) {
      html = label;
      if (order == AC_Vertical)
        html += String(F("<br>"));
    }
    uint8_t n = 0;
    for (const String value : _values) {
      n++;
      String  id = name + "_" + String(n);
      String  innerHtml = String(F("<input type=\"radio\" name=\"")) + name + String(F("\" id=\"")) + id + String(F("\" value=\"")) + value + String("\"");
      if (n == checked)
        innerHtml += String(F(" checked"));
      innerHtml += String(F("><label for=\"")) + id + String("\">") + value + String(F("</label>"));
      if (n <= tags.size())
        innerHtml += tags[n - 1];
      if (order == AC_Vertical) {
        html += innerHtml + String(F("<br>"));
      }
      else
        html += AutoConnectElementBasis::posterior(innerHtml);
    }
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
    html = String(F("<input type=\"button\" name=\"")) + name + String(F("\" value=\"")) + value + String(F("\" onclick=\"_sa('")) + uri + String("')\">");
    html = AutoConnectElementBasis::posterior(html);
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
      int   buflen = (value.length() + format.length() + 16 + 1) & (~0xf);
      char* buffer;
      if ((buffer = (char*)malloc(buflen))) {
        snprintf(buffer, buflen, format.c_str(), value.c_str());
        value_f = String(buffer);
        free(buffer);
      }
    }

    if (style.length())
      html = String(F("<div id=\"")) + name + String(F("\" style=\"")) + style + String("\">") + value_f + String(F("</div>"));
    else
      html = AutoConnectElementBasis::posterior(value_f);
  }
  return html;
}

#endif // _AUTOCONNECTELEMENTBASISIMPL_H_
