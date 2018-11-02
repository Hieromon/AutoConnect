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
  AC_DBG("ELM:%s placed\n", addon.name().c_str());
}

void AutoConnectAux::add(AutoConnectElementVT addons) {
  for (std::size_t n = 0; n < addons.size(); n++)
    add(addons[n]);
}

void AutoConnectAux::_append(AutoConnectAux& aux) {
  if (_next) {
    AutoConnectAux* next = _next.get();
    next->_append(aux);
  }
  else
    _next.reset(&aux);
}

void AutoConnectAux::_join(AutoConnect& ac) {
  _ac.reset(&ac);
  if (_next) {
    AutoConnectAux *next = _next.get();
    next->_join(ac);
  }
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
      mother->_menuTitle = _title;

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

const String  AutoConnectAux::_injectMenu(PageArgument& args) {
  String  menuItem = String(FPSTR("<li class=\"luxbar-item\"><a href=\"")) + String(_uri) + String("\">") + _title + String(FPSTR("</a></li>"));
  if (_next) {
    AutoConnectAux* next = _next.get();
    menuItem += next->_injectMenu(args);
  }
  return menuItem;
}
