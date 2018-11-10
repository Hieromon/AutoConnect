/**
 *  Implementation of AutoConnectAux class and subordinated AutoConnectElement class.
 *  @file AutoConnectAux.cpp
 *  @author hieromon@gmail.com
 *  @version  0.9.7
 *  @date 2018-11-17
 *  @copyright  MIT license.
 */

#include "AutoConnect.h"
#include "AutoConnectAux.h"

/**
 * Template for auxiliary page composed with AutoConnectAux of user sketch.
 * 
 * The structure of the auxiliary page depends on this template for 
 * the purpose to be incorporated into the AutoConnect Menu.
 * The page element implemented by AutoConnectElement is placed at the 
 * position of {{AUX_ELEMENT}} token. This token is contained in a 
 * <div> block with a class defined in 'base-panel' and is held by a 
 * <form> element with an ID '_aux'.
 * The JavaScript that named 'sa' at the end of the template determines 
 * the behavior of AutoConnectSubmit.
 */
const char AutoConnectAux::_PAGE_AUX[] PROGMEM = {
  "{{HEAD}}"
  "<title>{{AUX_TITLE}}</title>"
  "<style type=\"text/css\">"
  "{{CSS_BASE}}"
  "{{CSS_UL}}"
  "{{CSS_INPUT_BUTTON}}"
  "{{CSS_INPUT_TEXT}}"
  "{{CSS_LUXBAR}}"
  "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
  "<div class=\"container\">"
  "{{MENU_PRE}}"
  "{{MENU_AUX}}"
  "{{MENU_POST}}"
  "<div class=\"base-panel\"><div class=\"aux-page\">"
  "<form id='_aux' method=\"post\" onsubmit=\"return false;\">"
  "<ul class=\"noorder\">"
  "{{AUX_ELEMENT}}"
  "</ul>"
  "</form>"
  "</div></div>"
  "</div>"
  "<script>"
  "function _sa(url) {"
  "document.getElementById('_aux').action=url;"
  "document.getElementById('_aux').submit();"
  "}"
  "</script>"
  "</body>"
  "</html>"
};

/**
 *  Generate an HTML <button> element. The onclick behavior depends on 
 *  the code held in ÅfactionÅf member.
 *  @return String  an HTML string.
 */
const String AutoConnectButton::toHTML(void) const {
  return String(FPSTR("<button type=\"button\" name=\"")) + name + String(FPSTR("\" value=\"")) + value + String(FPSTR("\" onclick=\"")) + action + String("\">") + value + String(FPSTR("</button>"));
}

/**
 *  Generate an HTML <input type=checkbox> element.
 *  A "value" is associated with the input tag and sent by the form 
 *  action as the value of "name". If the label member is contained, it
 *  is placed to the right side of the checkbox to be labeled.
 *  If the label member is empty, only the checkbox is placed.
 *  @return String  an HTML string.
 */
const String AutoConnectCheckbox::toHTML(void) const {
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
const String AutoConnectInput::toHTML(void) const {
  String  html = String();

  if (label.length())
    html = String(FPSTR("<label for=\"")) + name + String("\">") + label + String(FPSTR("</label>"));
  html += String(FPSTR("<input type=\"text\" id=\"")) + name + String(FPSTR("\" name=\"")) + name;
  if (value.length())
    html += String(FPSTR("\" placeholder=\"")) + value;
  html += String(FPSTR("\"><br>"));

  return html;
}

/**
 *  Generate an HTML <select> element with an <option> element.
 *  The attribute value of the <option> element is given to the 
 *  AutoConnectSelect class as a string array, which would be stored 
 *  in the 'options' member. If a label member is contained, the <label>
 *  element would be generated the preface of <select>.
 *  @return String  an HTML string.
 */
const String AutoConnectSelect::toHTML(void) const {
  String  html = String();

  if (label.length())
    html = String(FPSTR("<label>")) + label + String(FPSTR("</label>"));
  html += String(FPSTR("<select name=\"")) + name + String("\">");
  std::size_t n = _options.size();
  if (n) {
    for (std::size_t n = 0; n < _options.size(); n++)
      html += String(FPSTR("<option value=\"")) + _options[n] + "\">" + _options[n] + String(FPSTR("</option>"));
  }
  html += String(FPSTR("</select><br>"));
  return html;
}

/**
 *  Generate an HTML <input type=button> element. This element is used 
 *  for form submission. An 'onclick' attribute calls fixed JavaScript 
 *  code as 'sa' named and it's included in the template.
 *  @return String  an HTML string.
 */
const String AutoConnectSubmit::toHTML(void) const {
  return String(FPSTR("<input type=\"button\" name=\"")) + name + String(FPSTR("\" value=\"")) + value + String(FPSTR("\" onclick=\"_sa('")) + uri + String("')\">");
}

/**
 *  Generate an HTML text element from a string of the value member. If a style
 *  exists, it gives a style attribute.
 *  @return String  an HTML string.
 */
const String AutoConnectText::toHTML(void) const {
  return String(FPSTR("<div style=\"")) + style + String("\">") + value + String(FPSTR("</div>"));
}

/**
 *  Destructs container of AutoConnectElement and release a unique
 *  pointer of AutoConnect instance.
 */
AutoConnectAux::~AutoConnectAux() {
  _addonElm.clear();
  _addonElm.swap(_addonElm);
  if (_ac)
    _ac.release();
}

/**
 *  Add an AutoConnectElement
 *  @param  addon A reference of AutoConnectElement.
 */
void AutoConnectAux::add(AutoConnectElement& addon) {
  _addonElm.push_back(addon);
  AC_DBG("%s placed on %s\n", addon.name.c_str(), uri());
}

/**
 *  Add an AutoConnectElement vector container to the AutoConnectAux page.
 *  @param  addons  AutoConnectElementVT collection.
 */
void AutoConnectAux::add(AutoConnectElementVT addons) {
  for (std::size_t n = 0; n < addons.size(); n++)
    add(addons[n]);
}

/**
 *  Releases the AutoConnectElements with the specified name from 
 *  the AutoConnectAux page. Releases all AutoConnectElements with 
 *  the same name in AutoConnectAux.
 *  @param  name  
 *  @return true  The specified AutoConnectElements have been released.
 *  @return false The specified AutoConnectElement not found in AutoConnectAux.
 */
bool AutoConnectAux::release(const String name) {
  bool  rc = false;
  for (std::size_t n = 0; n < _addonElm.size(); n++) {
    String  elmName = _addonElm[n].get().name;
    if (elmName == name) {
      AC_DBG("%s release from %s\n", elmName.c_str(), uri());
      _addonElm.erase(_addonElm.begin() + n);
      rc = true;
    }
  }
  return rc;
}

/**
 *  Concatenates subsequent AutoConnectAux pages starting from oneself 
 *  to the chain list. 
 *  AutoConnectAux is collected in the chain list and each object is 
 *  chained by the "_next". AutoConnect follows the "_next" to manage 
 *  auxiliary pages. The _concat function concatenates subsequent 
 *  AutoConnectAuxs.
 *  @param  aux   A reference of AutoConnectAux.
 */
void AutoConnectAux::_concat(AutoConnectAux& aux) {
  if (_next)
    _next->_concat(aux);
  else
    _next.reset(&aux);
}

/**
 *  Register the AutoConnect that owns itself.
 *  AutoConenctAux needs to access the AutoConnect member. Also 
 *  AutoConnectAux is cataloged by chain list. The _join function 
 *  registers AutoConnect in the following AutoConnectAux chain list.
 *  @param  ac    A reference of AutoConnect.
 */
void AutoConnectAux::_join(AutoConnect& ac) {
  _ac.reset(&ac);

  // Chain to subsequent AutoConnectAux in the list.
  if (_next)
    _next->_join(ac);
}

/**
 *  Insert the token handler of PageBuilder. This handler inserts HTML
 *  elements generated by the whole AutoConnectElements to the auxiliary page.
 *  @param  args  A reference of PageArgument but unused.
 *  @return HTML string that should be inserted.
 */
const String AutoConnectAux::_insertElement(PageArgument& args) {
  AC_UNUSED(args);
  String  body = String();

  if (_handler)
    if (_order & AC_EXIT_AHEAD) {
      AC_DBG("CB %s\n", uri());
      body += _handler(args);
    }

  for (std::size_t n = 0; n < _addonElm.size(); n++) {
    AutoConnectElement& addon = _addonElm[n];
    body += addon.toHTML();
  }

  if (_handler)
    if (_order & AC_EXIT_LATER) {
      AC_DBG("CB %s\n", uri());
      body += _handler(args);
    }

  return body;
}

/**
 *  Generate an auxiliary page assembled with the AutoConnectElement.
 *  This function is the core procedure of AutoConnectAux, and uses 
 *  PageBuilder from the _PAGE_AUX template to build an AutoConnect 
 *  menu and insert HTML elements. A template of an auxiliary page is 
 *  fixed and its structure inherits from the AutoConnect.
 *  @param  uri   An uri of the auxiliary page.
 *  @return A PageElement of auxiliary page.
 */
PageElement* AutoConnectAux::_setupPage(String uri) {
  PageElement*  elm = nullptr;

  if (_ac) {
    if (uri != String(_uri)) {
      if (_next) {
        elm = _next->_setupPage(uri);
      }
    } else {
      AutoConnect*  mother = _ac.get();
      // Overwrite actual AutoConnectMenu title to the Aux. page title
      if (_title.length())
        mother->_menuTitle = _title;

      elm = new PageElement();
      // Construct the auxiliary page
      elm->setMold(_PAGE_AUX);
      elm->addToken(PSTR("HEAD"), std::bind(&AutoConnect::_token_HEAD, mother, std::placeholders::_1));
      elm->addToken(PSTR("AUX_TITLE"), std::bind(&AutoConnectAux::_injectTitle, this, std::placeholders::_1));
      elm->addToken(PSTR("CSS_BASE"), std::bind(&AutoConnect::_token_CSS_BASE, mother, std::placeholders::_1));
      elm->addToken(PSTR("CSS_UL"), std::bind(&AutoConnect::_token_CSS_UL, mother, std::placeholders::_1));
      elm->addToken(PSTR("CSS_INPUT_BUTTON"), std::bind(&AutoConnect::_token_CSS_INPUT_BUTTON, mother, std::placeholders::_1));
      elm->addToken(PSTR("CSS_INPUT_TEXT"), std::bind(&AutoConnect::_token_CSS_INPUT_TEXT, mother, std::placeholders::_1));
      elm->addToken(PSTR("CSS_LUXBAR"), std::bind(&AutoConnect::_token_CSS_LUXBAR, mother, std::placeholders::_1));
      elm->addToken(PSTR("MENU_PRE"), std::bind(&AutoConnect::_token_MENU_PRE, mother, std::placeholders::_1));
      elm->addToken(PSTR("MENU_AUX"), std::bind(&AutoConnect::_token_MENU_AUX, mother, std::placeholders::_1));
      elm->addToken(PSTR("MENU_POST"), std::bind(&AutoConnect::_token_MENU_POST, mother, std::placeholders::_1));
      elm->addToken(PSTR("AUX_ELEMENT"), std::bind(&AutoConnectAux::_insertElement, this, std::placeholders::_1));
    }
  }
  return elm;
}

/**
 *  Inject the <li> element depending on the "luxbar-item" attribute 
 *  for implementing the AutoConnect menu.
 *  @param  args  A reference of PageArgument but it's only used for 
 *  interface alignment and is not actually used. 
 *  @return A concatenated string of <li> elements for the menu item of 
 *  AutoConnect.
 */
const String AutoConnectAux::_injectMenu(PageArgument& args) {
  String  menuItem;

  if (_menu)
    menuItem = String(FPSTR("<li class=\"luxbar-item\"><a href=\"")) + String(_uri) + String("\">") + _title + String(FPSTR("</a></li>"));
  if (_next)
    menuItem += _next->_injectMenu(args);
  return menuItem;
}
