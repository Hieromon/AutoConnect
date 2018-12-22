/**
 * Implementation of AutoConnectElementBasis classes.
 * @file AutoConnectElementImpl.h
 * @author hieromon@gmail.com
 * @version  0.9.7
 * @date 2018-11-17
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTELEMENTBASISIMPL_H_
#define _AUTOCONNECTELEMENTBASISIMPL_H_

#include "AutoConnectElementBasis.h"

/**
 * Generate an HTML <button> element. The onclick behavior depends on
 * the code held in factionf member.
 * @return  An HTML string.
 */
const String AutoConnectButtonBasis::toHTML(void) const {
  return String(FPSTR("<button type=\"button\" name=\"")) + name + String(FPSTR("\" value=\"")) + value + String(FPSTR("\" onclick=\"")) + action + String("\">") + value + String(FPSTR("</button>"));
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

  html = String(FPSTR("<input type=\"checkbox\" name=\"")) + name + String(FPSTR("\" value=\"")) + value + String("\"");
  if (checked)
    html += String(FPSTR(" checked"));
  if (label.length())
    html += String(" id=\"") + name + String("\"><label for=\"") + name + String("\">") + label + String(FPSTR("</label"));
  html += String(FPSTR("><br>"));
  return html;
}

/**
 *  Generate an HTML <input type=text> element.
 *  If the value member is contained, it is reflected in the placeholder
 *  attribute. The entered value can be obtained using the user callback
 *  function registered by AutoConnectAux::on after the form is sent in
 *  combination with AutoConnectSubmit.
 *  @return String  an HTML string.
 */
const String AutoConnectInputBasis::toHTML(void) const {
  String  html = String();

  if (label.length())
    html = String(FPSTR("<label for=\"")) + name + String("\">") + label + String(FPSTR("</label>"));
  html += String(FPSTR("<input type=\"text\" id=\"")) + name + String(FPSTR("\" name=\"")) + name + String("\"");
  if (placeholder.length())
    html += String(FPSTR(" placeholder=\"")) + placeholder + String("\"");
  if (value.length())
    html += String(FPSTR(" value=\"")) + value + String("\"");
  html += String(FPSTR("><br>"));

  return html;
}

/**
 *  Generate an HTML <input type=radio> element with an <option> element.
 *  @return String  an HTML string.
 */
const String AutoConnectRadioBasis::toHTML(void) const {
  String  html = String();

  if (label.length()) {
    html = label;
    if (order == AC_Vertical)
      html += String("<br>");
  }
  for (std::size_t n = 0; n < _values.size(); n++) {
    String value = _values[n];
    html += String(FPSTR("<input type=\"radio\" name=\"")) + name + String(FPSTR("\" id=\"")) + value + String(FPSTR("\" value=\"")) + value + String("\"");
    if (n == checked - 1)
      html += String(FPSTR(" checked"));
    html += String(FPSTR("><label for=\"")) + value + String("\">") + value + String(FPSTR("</label>"));
    if (order == AC_Vertical)
      html += String("<br>");
  }
  return html;
}

/**
 * Indicate an entry with the specified value in the value's collection.
 * @param value     The value to indicates in the collection.
 */
void AutoConnectRadioBasis::check(const String& value) {
  for (std::size_t n = 0; n < _values.size(); n++) {
    String& v = _values[n];
    if (v.equalsIgnoreCase(value)) {
      checked = n + 1;
      break;
    }
  }
}

/**
 *  Generate an HTML <select> element with an <option> element.
 *  The attribute value of the <option> element is given to the
 *  AutoConnectSelect class as a string array, which would be stored
 *  in the 'options' member. If a label member is contained, the <label>
 *  element would be generated the preface of <select>.
 *  @return String  an HTML string.
 */
const String AutoConnectSelectBasis::toHTML(void) const {
  String  html = String();

  if (label.length())
    html = String(FPSTR("<label for=\"")) + name + String("\">") + label + String(FPSTR("</label>"));
  html += String(FPSTR("<select name=\"")) + name + String("\" id=\"") + name + String("\">");
  std::size_t n = _options.size();
  if (n) {
    for (std::size_t n = 0; n < _options.size(); n++)
      html += String(FPSTR("<option value=\"")) + _options[n] + "\">" + _options[n] + String(FPSTR("</option>"));
  }
  html += String(FPSTR("</select>"));
  return html;
}

/**
 *  Generate an HTML <input type=button> element. This element is used
 *  for form submission. An 'onclick' attribute calls fixed JavaScript
 *  code as 'sa' named and it's included in the template.
 *  @return String  an HTML string.
 */
const String AutoConnectSubmitBasis::toHTML(void) const {
  return String(FPSTR("<input type=\"button\" name=\"")) + name + String(FPSTR("\" value=\"")) + value + String(FPSTR("\" onclick=\"_sa('")) + uri + String("')\">");
}

/**
 *  Generate an HTML text element from a string of the value member. If a style
 *  exists, it gives a style attribute.
 *  @return String  an HTML string.
 */
const String AutoConnectTextBasis::toHTML(void) const {
  return String(FPSTR("<div style=\"")) + style + String("\">") + value + String(FPSTR("</div>"));
}

#endif // _AUTOCONNECTELEMENTBASISIMPL_H_
