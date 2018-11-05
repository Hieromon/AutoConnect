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
  "{{EXIT_HANDLE}}"
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
  "<div class=\"base-panel\">"
  "<form id='_aux' method=\"post\">"
  "<ul class=\"noorder\">"
  "{{AUX_ELEMENT}}"
  "</ul>"
  "</form>"
  "</div>"
  "</div>"
  "<script>"
  "function sa(url) {"
  "document.getElementById('_aux').action=url;"
  "document.getElementIyId('_aux').submit();"
  "}"
  "</script>"
  "</body>"
  "</html>"
};

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

void AutoConnectAux::add(AutoConnectElement& addon) {
  _addonElm.push_back(addon);
  AC_DBG("%s placed in %s\n", addon.name().c_str(), uri());
}

void AutoConnectAux::add(AutoConnectElementVT addons) {
  for (std::size_t n = 0; n < addons.size(); n++)
    add(addons[n]);
}

void AutoConnectAux::_concat(AutoConnectAux& aux) {
  if (_next)
    _next->_concat(aux);
  else
    _next.reset(&aux);
}

void AutoConnectAux::_join(AutoConnect& ac) {
  _ac.reset(&ac);
  if (_next)
    _next->_join(ac);
}

const String AutoConnectAux::_insertElement(PageArgument& args) {
  String  body = String();

  for (std::size_t n = 0; n < _addonElm.size(); n++) {
    AutoConnectElement& addon = _addonElm[n];
    body += addon.toHTML();
  }
  return body;
}

PageElement* AutoConnectAux::_setupPage(String uri) {
  PageElement*  elm = nullptr;

  if (_ac) {
    if (uri != String(_uri)) {
      if (_next) {
        elm = _next->_setupPage(uri);
      }
    } else {
      AutoConnect*  mother = _ac.get();
      elm = new PageElement();

      // Overwrite the default menu title
      if (_title.length()) {
        mother->_menuTitle = _title;
        _activeTitle = String();
      }
      else
        _activeTitle = mother->_menuTitle;

      // Construct the auxiliary page
      elm->setMold(_PAGE_AUX);
      elm->addToken(PSTR("EXIT_HANDLE"), std::bind(&AutoConnectAux::_exitHandle, this, std::placeholders::_1));
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

const String AutoConnectAux::_injectMenu(PageArgument& args) {
  String  menuItem;
  
  if (_title.length())
    menuItem = String(FPSTR("<li class=\"luxbar-item\"><a href=\"")) + String(_uri) + String("\">") + _title + String(FPSTR("</a></li>"));
  if (_next)
    menuItem += _next->_injectMenu(args);
  return menuItem;
}

const String AutoConnectAux::_exitHandle(PageArgument& args) {
  if (_handler) {
    AC_DBG("CB %s\n", *this->uri());
    (*_handler)(*this, args);
  }
  return "";
}
