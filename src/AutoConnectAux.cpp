/**
 * Implementation of AutoConnectAux class.
 * @file AutoConnectAuxBasisImpl.h
 * @author hieromon@gmail.com
 * @version  0.9.7
 * @date 2018-11-17
 * @copyright  MIT license.
 */
#include <algorithm>
#include "AutoConnect.h"
#include "AutoConnectAux.h"
#include "AutoConnectElement.h"
#include "AutoConnectElementBasisImpl.h"
#ifdef AUTOCONNECT_USE_JSON
#include "AutoConnectElementJsonImpl.h"
#endif

/**
 * Template for auxiliary page composed with AutoConnectAux of user sketch.
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
  "var uri=document.createElement('input');"
  "uri.setAttribute('type','hidden');"
  "uri.setAttribute('name','" AUTOCONNECT_AUXURI_PARAM "');"
  "uri.setAttribute('value','{{AUX_URI}}');"
  "document.getElementById('_aux').appendChild(uri);"
  "document.getElementById('_aux').action=url;"
  "document.getElementById('_aux').submit();"
  "}"
  "</script>"
  "</body>"
  "</html>"
};

/**
 * Destructs container of AutoConnectElement and release a unique
 * pointer of AutoConnect instance.
 */
AutoConnectAux::~AutoConnectAux() {
  _addonElm.clear();
  _addonElm.swap(_addonElm);
  if (_ac)
    _ac.release();
}

/**
 * Returns a null element as static storage.
 * This static element is referred by invalid JSON data.
 * @return A reference of a static element defined by name as null.
 */
AutoConnectElement& AutoConnectAux::_nullElement() {
  static AutoConnectElement nullElement("", "");
  return nullElement;
}

/**
 * Add an AutoConnectElement
 * @param  addon A reference of AutoConnectElement.
 */
void AutoConnectAux::add(AutoConnectElement& addon) {
  _addonElm.push_back(addon);
  AC_DBG("%s placed on %s\n", addon.name.length() ? addon.name.c_str() : "*noname", uri());
}

/**
 * Add an AutoConnectElement vector container to the AutoConnectAux page.
 * @param  addons  AutoConnectElementVT collection.
 */
void AutoConnectAux::add(AutoConnectElementVT addons) {
  for (AutoConnectElement& element : addons)
    add(element);
}

/**
 * Get already registered AutoConnectElement.
 * @param  name  Element name
 * @return A pointer to the registered AutoConnectElement.
 */
AutoConnectElement* AutoConnectAux::getElement(const String& name) {
  for (AutoConnectElement& elm : _addonElm)
    if (elm.name.equalsIgnoreCase(name))
      return &elm;
  AC_DBG("Element<%s> not registered\n", name.c_str());
  return nullptr;
}

/**
 * Releases the AutoConnectElements with the specified name from 
 * the AutoConnectAux page. Releases all AutoConnectElements with 
 * the same name in AutoConnectAux.
 * @param  name  
 * @return true  The specified AutoConnectElements have been released.
 * @return false The specified AutoConnectElement not found in AutoConnectAux.
 */
bool AutoConnectAux::release(const String& name) {
  auto itr = std::remove_if(_addonElm.begin(), _addonElm.end(),
    [&](std::reference_wrapper<AutoConnectElement> const elm) {
      return elm.get().name.equalsIgnoreCase(name);
    });
  return _addonElm.erase(itr, _addonElm.end()) != _addonElm.end();
}

/**
 * Set the value to specified element.
 * @param  name  A string of element name to set the value.
 * @param  value Setting value. (String)
 * @return true  The value was set.
 * @return false An element specified name is not registered,
 * or its element value does not match storage type.
 */
bool AutoConnectAux::setElementValue(const String& name, const String value) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() != AC_Select) {
      if (elm->typeOf() == AC_Checkbox) {
        if (value == "checked") {
          AutoConnectCheckbox* elmCheckbox = reinterpret_cast<AutoConnectCheckbox*>(elm);
          elmCheckbox->checked = true;
        }
      }
      else if (elm->typeOf() == AC_Radio) {
        AutoConnectRadio* elmRadio = reinterpret_cast<AutoConnectRadio*>(elm);
        elmRadio->check(value);
      }
      else
        elm->value = value;
      return true;
    }
    else
      AC_DBG("Element<%s> value type mismatch\n", name.c_str());
  }
  return false;
}

/**
 * Set the value to specified element.
 * @param  name  A string of element name to set the value.
 * @param  value Setting value. (String)
 * @return true  The value was set.
 * @return false An element specified name is not registered,
 * or its element value must be array.
 */
bool AutoConnectAux::setElementValue(const String& name, std::vector<String> const& values) {
  bool  rc = false;

  AutoConnectElement* elm = getElement(name);
  if (elm) {
    switch (elm->typeOf()) {
    case AC_Radio: {
      AutoConnectRadio* elmRadio = reinterpret_cast<AutoConnectRadio*>(elm);
      elmRadio->empty();
      for (String v : values)
        elmRadio->add(v);
      rc = true;
      break;
    }
    case AC_Select: {
      AutoConnectSelect* elmSelect = reinterpret_cast<AutoConnectSelect*>(elm);
      elmSelect->empty();
      for (String o : values)
        elmSelect->add(o);
      rc = true;
      break;
    }
    default: {
      AC_DBG("Element<%s> value type mismatch\n", name.c_str());
      break;
    }
    }
  }
  return rc;
}

/**
 * Concatenates subsequent AutoConnectAux pages starting from oneself 
 * to the chain list. 
 * AutoConnectAux is collected in the chain list and each object is 
 * chained by the "_next". AutoConnect follows the "_next" to manage 
 * auxiliary pages. The _concat function concatenates subsequent 
 * AutoConnectAuxs.
 * @param  aux   A reference of AutoConnectAux.
 */
void AutoConnectAux::_concat(AutoConnectAux& aux) {
  if (_next)
    _next->_concat(aux);
  else
    _next.reset(&aux);
}

/**
 * Register the AutoConnect that owns itself.
 * AutoConenctAux needs to access the AutoConnect member. Also 
 * AutoConnectAux is cataloged by chain list. The _join function 
 * registers AutoConnect in the following AutoConnectAux chain list.
 * @param  ac    A reference of AutoConnect.
 */
void AutoConnectAux::_join(AutoConnect& ac) {
  _ac.reset(&ac);

  // Chain to subsequent AutoConnectAux in the list.
  if (_next)
    _next->_join(ac);
}

/**
 * Inject the <li> element depending on the "luxbar-item" attribute
 * for implementing the AutoConnect menu.
 * @param  args  A reference of PageArgument but it's only used for
 * interface alignment and is not actually used.
 * @return A concatenated string of <li> elements for the menu item of
 * AutoConnect.
 */
const String AutoConnectAux::_injectMenu(PageArgument& args) {
  String  menuItem;

  if (_menu)
    menuItem = String(FPSTR("<li class=\"luxbar-item\"><a href=\"")) + String(_uri) + String("\">") + _title + String(FPSTR("</a></li>"));
  if (_next)
    menuItem += _next->_injectMenu(args);
  return menuItem;
}

/**
 * Insert the uri that caused the request to the aux.
 */
const String AutoConnectAux::_indicateUri(PageArgument& args) {
  AC_UNUSED(args);
  String  lastUri = _uriStr;
  lastUri.replace("/", "&#47;");
  return lastUri;
}

/**
 * Insert the token handler of PageBuilder. This handler inserts HTML
 * elements generated by the whole AutoConnectElements to the auxiliary page.
 * @param  args  A reference of PageArgument but unused.
 * @return HTML string that should be inserted.
 */
const String AutoConnectAux::_insertElement(PageArgument& args) {
  String  body = String("");

  if (_handler) {
    if (_order & AC_EXIT_AHEAD) {
      AC_DBG("CB in AHEAD %s\n", uri());
      body += _handler(*this, args);
    }
  }

  for (AutoConnectElement& addon : _addonElm)
    body += addon.toHTML();

  if (_handler) {
    if (_order & AC_EXIT_LATER) {
      AC_DBG("CB in LATER %s\n", uri());
      body += _handler(*this, args);
    }
  }
  return body;
}

/**
 * Generate an auxiliary page assembled with the AutoConnectElement.
 * This function is the core procedure of AutoConnectAux, and uses 
 * PageBuilder from the _PAGE_AUX template to build an AutoConnect 
 * menu and insert HTML elements. A template of an auxiliary page is 
 * fixed and its structure inherits from the AutoConnect.
 * @param  uri   An uri of the auxiliary page.
 * @return A PageElement of auxiliary page.
 */
PageElement* AutoConnectAux::_setupPage(const String& uri) {
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
      elm->addToken(String(FPSTR("HEAD")), std::bind(&AutoConnect::_token_HEAD, mother, std::placeholders::_1));
      elm->addToken(String(FPSTR("AUX_TITLE")), std::bind(&AutoConnectAux::_injectTitle, this, std::placeholders::_1));
      elm->addToken(String(FPSTR("CSS_BASE")), std::bind(&AutoConnect::_token_CSS_BASE, mother, std::placeholders::_1));
      elm->addToken(String(FPSTR("CSS_UL")), std::bind(&AutoConnect::_token_CSS_UL, mother, std::placeholders::_1));
      elm->addToken(String(FPSTR("CSS_INPUT_BUTTON")), std::bind(&AutoConnect::_token_CSS_INPUT_BUTTON, mother, std::placeholders::_1));
      elm->addToken(String(FPSTR("CSS_INPUT_TEXT")), std::bind(&AutoConnect::_token_CSS_INPUT_TEXT, mother, std::placeholders::_1));
      elm->addToken(String(FPSTR("CSS_LUXBAR")), std::bind(&AutoConnect::_token_CSS_LUXBAR, mother, std::placeholders::_1));
      elm->addToken(String(FPSTR("MENU_PRE")), std::bind(&AutoConnect::_token_MENU_PRE, mother, std::placeholders::_1));
      elm->addToken(String(FPSTR("MENU_AUX")), std::bind(&AutoConnect::_token_MENU_AUX, mother, std::placeholders::_1));
      elm->addToken(String(FPSTR("MENU_POST")), std::bind(&AutoConnect::_token_MENU_POST, mother, std::placeholders::_1));
      elm->addToken(String(FPSTR("AUX_URI")), std::bind(&AutoConnectAux::_indicateUri, this, std::placeholders::_1));
      elm->addToken(String(FPSTR("AUX_ELEMENT")), std::bind(&AutoConnectAux::_insertElement, this, std::placeholders::_1));
    }
  }
  return elm;
}

/**
 * Store element values owned by AutoConnectAux that caused the request.
 * Save the current arguments remaining in the Web server object when
 * this function invoked.
 * @param webServer A pointer to the class object of WebServerClass
 */
void AutoConnectAux::_storeElements(WebServerClass* webServer) {
  for (AutoConnectElement& elm : _addonElm) {
    // Overwrite the value of all cataloged AutoConnectElements with
    // arguments inherited from the last http request.
    // Relies on AutoConnectRadio, it restores to false at the being
    // because the checkbox argument will not pass if it is not checked.
    if (elm.typeOf() == AC_Checkbox)
      reinterpret_cast<AutoConnectCheckbox&>(elm).checked = false;

    // Seek by argument, store the value to its element.
    for (int8_t n = 0; n < static_cast<int8_t>(webServer->args()); n++) {
      if (webServer->argName(n).equalsIgnoreCase(elm.name)) {
        String  elmValue = webServer->arg(n);
        if (elm.typeOf() == AC_Checkbox)
          elmValue = "checked";
        setElementValue(elm.name, elmValue);
      }
    }
  }
}

#ifndef AUTOCONNECT_USE_JSON

/**
 * Get AutoConnectButtonBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectButton class.
 */
template<>
AutoConnectButtonBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Button)
      return *(reinterpret_cast<AutoConnectButtonBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectButtonBasis&>(_nullElement());
}

/**
 * Get AutoConnectCheckboxBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectCheckbox class.
 */
template<>
AutoConnectCheckboxBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Checkbox)
      return *(reinterpret_cast<AutoConnectCheckboxBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectCheckboxBasis&>(_nullElement());
}

/**
 * Get AutoConnectInputBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectInput class.
 */
template<>
AutoConnectInputBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Input)
      return *(reinterpret_cast<AutoConnectInputBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectInputBasis&>(_nullElement());
}

/**
 * Get AutoConnectRadioBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectRadio class.
 */
template<>
AutoConnectRadioBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Radio)
      return *(reinterpret_cast<AutoConnectRadioBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectRadioBasis&>(_nullElement());
}

/**
 * Get AutoConnectSelectBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectSelect class.
 */
template<>
AutoConnectSelectBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Select)
      return *(reinterpret_cast<AutoConnectSelectBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectSelectBasis&>(_nullElement());
}

/**
 * Get AutoConnectSubmitBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectSubmit class.
 */
template<>
AutoConnectSubmitBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Submit)
      return *(reinterpret_cast<AutoConnectSubmitBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectSubmitBasis&>(_nullElement());
}

/**
 * Get AutoConnectTextBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectText class.
 */
template<>
AutoConnectTextBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Text)
      return *(reinterpret_cast<AutoConnectTextBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectTextBasis&>(_nullElement());
}

#else

/**
 * Get AutoConnectButtonJson element.
 * @param  name  An element name.
 * @return A reference of AutoConnectButton class.
 */
template<>
AutoConnectButtonJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Button)
      return *(reinterpret_cast<AutoConnectButtonJson*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectButtonJson&>(_nullElement());
}

/**
 * Get AutoConnectCheckboxJson element.
 * @param  name  An element name.
 * @return A reference of AutoConnectCheckbox class.
 */
template<>
AutoConnectCheckboxJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Checkbox)
      return *(reinterpret_cast<AutoConnectCheckboxJson*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectCheckboxJson&>(_nullElement());
}

/**
 * Get AutoConnectInputJson element.
 * @param  name  An element name.
 * @return A reference of AutoConnectInput class.
 */
template<>
AutoConnectInputJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Input)
      return *(reinterpret_cast<AutoConnectInputJson*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectInputJson&>(_nullElement());
}

/**
 * Get AutoConnectRadioJson element.
 * @param  name  An element name.
 * @return A reference of AutoConnectRadio class.
 */
template<>
AutoConnectRadioJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Radio)
      return *(reinterpret_cast<AutoConnectRadioJson*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectRadioJson&>(_nullElement());
}

/**
 * Get AutoConnectSelectJson element.
 * @param  name  An element name.
 * @return A reference of AutoConnectSelect class.
 */
template<>
AutoConnectSelectJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Select)
      return *(reinterpret_cast<AutoConnectSelectJson*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectSelectJson&>(_nullElement());
}

/**
 * Get AutoConnectSubmitJson element.
 * @param  name  An element name.
 * @return A reference of AutoConnectSubmit class.
 */
template<>
AutoConnectSubmitJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Submit)
      return *(reinterpret_cast<AutoConnectSubmitJson*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectSubmitJson&>(_nullElement());
}

/**
 * Get AutoConnectTextJson element.
 * @param  name  An element name.
 * @return A reference of AutoConnectText class.
 */
template<>
AutoConnectTextJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Text)
      return *(reinterpret_cast<AutoConnectTextJson*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectTextJson&>(_nullElement());
}

/**
 * Load AutoConnectAux page from JSON description stored in the sketch.
 * This function can load AutoConnectAux for multiple AUX pages written
 * in JSON and is registered in AutoConnect.
 * @param  aux  JSON description to be load.
 * @return true Successfully loaded.
 */
bool AutoConnect::load(const String& aux) {
  DynamicJsonBuffer jsonBuffer(AUTOCONNECT_JSON_BUFFER_SIZE);
  JsonVariant jv = jsonBuffer.parse(aux);
  return _load(jv);
}

/**
* Load AutoConnectAux page from JSON description stored in PROGMEM.
* This function can load AutoConnectAux for multiple AUX pages written
* in JSON and is registered in AutoConnect.
* @param  aux  JSON description to be load.
* @return true Successfully loaded.
*/
bool AutoConnect::load(const __FlashStringHelper* aux) {
  DynamicJsonBuffer jsonBuffer(AUTOCONNECT_JSON_BUFFER_SIZE);
  JsonVariant jv = jsonBuffer.parse(aux);
  return _load(jv);
}

/**
* Load AutoConnectAux page from JSON description from the stream.
* This function can load AutoConnectAux for multiple AUX pages written
* in JSON and is registered in AutoConnect.
* @param  aux  Stream for read AutoConnectAux elements.
* @return true Successfully loaded.
*/
bool AutoConnect::load(Stream& aux) {
  DynamicJsonBuffer jsonBuffer(AUTOCONNECT_JSON_BUFFER_SIZE);
  JsonVariant jv = jsonBuffer.parse(aux);
  return _load(jv);
}

/**
 * Load AutoConnectAux page from JSON object.
 * @param  aux  A JsonVariant object that stores each element of AutoConnectAuxl.
 * @return true Successfully loaded.
 */
bool AutoConnect::_load(JsonVariant& auxJson) {
  bool  rc = auxJson.success();
  if (rc) {
    if (auxJson.is<JsonArray>()) {
      JsonArray& jb = auxJson.as<JsonArray>();
      for (JsonObject& auxJson : jb) {
        AutoConnectAux* newAux = new AutoConnectAux;
        if (newAux->_load(auxJson))
          join(*newAux);
        else {
          delete newAux;
          rc = false;
          break;
        }
      }
    }
    else {
      JsonObject& jb = auxJson.as<JsonObject>();
      AutoConnectAux* newAux = new AutoConnectAux;
      if (newAux->_load(jb))
        join(*newAux);
      else {
        delete newAux;
        rc = false;
      }
    }
  }
  else {
    AC_DBG("JSON parse error\n");
  }
  return rc;
}

/**
 * Create an instance from the AutoConnectElement of the JSON object.
 * @param  json  A reference of JSON
 * @return A pointer of created AutoConnectElement instance.
 */
AutoConnectElement* AutoConnectAux::_createElement(const JsonObject& json) {
  AutoConnectElement* elm = nullptr;
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));

  switch (_asElementType(type)) {
  case AC_Element:
  case AC_Unknown:
    elm = new AutoConnectElement;
    break;
  case AC_Button: {
    AutoConnectButton*  cert_elm = new AutoConnectButton;
    return reinterpret_cast<AutoConnectElement*>(cert_elm);
  }
  case AC_Checkbox: {
    AutoConnectCheckbox*  cert_elm = new AutoConnectCheckbox;
    return reinterpret_cast<AutoConnectElement*>(cert_elm);
  }
  case AC_Input: {
    AutoConnectInput* cert_elm = new AutoConnectInput;
    return reinterpret_cast<AutoConnectElement*>(cert_elm);
  }
  case AC_Radio: {
    AutoConnectRadio*  cert_elm = new AutoConnectRadio;
    return reinterpret_cast<AutoConnectElement*>(cert_elm);
  }
  case AC_Select: {
    AutoConnectSelect*  cert_elm = new AutoConnectSelect;
    return reinterpret_cast<AutoConnectElement*>(cert_elm);
  }
  case AC_Submit: {
    AutoConnectSubmit*  cert_elm = new AutoConnectSubmit;
    return reinterpret_cast<AutoConnectElement*>(cert_elm);
  }
  case AC_Text: {
    AutoConnectText*  cert_elm = new AutoConnectText;
    return reinterpret_cast<AutoConnectElement*>(cert_elm);
  }
  }
  return elm;
}

/**
 * Constructs an AutoConnectAux instance by reading all the
 * AutoConnectElements of the specified URI from the elements defined 
 * JSON stored in a constant character string.
 * @param  in    AutoConnectAux element data which is described by JSON.
 * @return true  The element collection successfully loaded.
 * @return false Invalid JSON data occurred. 
 */
bool AutoConnectAux::load(const String& in) {
  DynamicJsonBuffer jsonBuffer(AUTOCONNECT_JSON_BUFFER_SIZE);
  JsonObject& jb = jsonBuffer.parseObject(in);
  return _load(jb);
}

/**
 * Constructs an AutoConnectAux instance by reading all the
 * AutoConnectElements of the specified URI from the elements defined
 * JSON stored in pgm_data array.
 * @param  in    AutoConnectAux element data which is described by JSON.
 * @return true  The element collection successfully loaded.
 * @return false Invalid JSON data occurred. 
 */
bool AutoConnectAux::load(const __FlashStringHelper* in) {
  DynamicJsonBuffer jsonBuffer(AUTOCONNECT_JSON_BUFFER_SIZE);
  JsonObject& jb = jsonBuffer.parseObject(in);
  return _load(jb);
}

/**
 * Constructs an AutoConnectAux instance by reading all the
 * AutoConnectElements of the specified URI from the elements defined
 * JSON stored in a Stream.
 * @param  in    AutoConnectAux element data which is described by JSON.
 * @return true  The element collection successfully loaded.
 * @return false Invalid JSON data occurred. 
 */
bool AutoConnectAux::load(Stream& in) {
  DynamicJsonBuffer jsonBuffer(AUTOCONNECT_JSON_BUFFER_SIZE);
  JsonObject& jb = jsonBuffer.parseObject(in);
  return _load(jb);
}

/**
 * Load all elements of AutoConectAux page from JSON object.
 * @param  jb    Reference of JSON object
 * @return true  Successfully loaded.
 * @return false loading unsuccessful, JSON parsing error occurred.
 */
bool AutoConnectAux::_load(JsonObject& jb) {
  if (!jb.success()) {
    AC_DBG("json parse error\n");
    return false;
  }

  _title = jb.get<String>(F(AUTOCONNECT_JSON_KEY_TITLE));
  _uriStr = jb.get<String>(F(AUTOCONNECT_JSON_KEY_URI));
  _uri = _uriStr.c_str();
  _menu = jb.get<bool>(F(AUTOCONNECT_JSON_KEY_MENU));
  JsonVariant elements = jb[F(AUTOCONNECT_JSON_KEY_ELEMENT)];
  (void)_loadElement(elements, "");
  return true;
}

/**
 * Load element specified by the name parameter from the stream
 * described by JSON. Usually, the Stream is specified a storm file of
 * SD or SPIFFS. The Stream must be opened before invoking the function.
 * @param  in    Reference of the Stream which contains the parameter
 * data described by JSON.
 * @param  name  The element name to be loaded. '*'specifies that all
 * elements are to be loaded.
 * @return A reference of loaded AutoConnectElement instance.
 */
bool AutoConnectAux::loadElement(const String& in, const String& name) {
  DynamicJsonBuffer jsonBuffer(AUTOCONNECT_JSON_BUFFER_SIZE);
  JsonVariant jb = jsonBuffer.parse(in);
  return _loadElement(jb, name);
}

bool AutoConnectAux::loadElement(const __FlashStringHelper* in, const String& name) {
  DynamicJsonBuffer jsonBuffer(AUTOCONNECT_JSON_BUFFER_SIZE);
  JsonVariant jb = jsonBuffer.parse(in);
  return _loadElement(jb, name);
}

bool AutoConnectAux::loadElement(Stream& in, const String& name) {
  DynamicJsonBuffer jsonBuffer(AUTOCONNECT_JSON_BUFFER_SIZE);
  JsonVariant jb = jsonBuffer.parse(in);
  return _loadElement(jb, name);
}

bool AutoConnectAux::_loadElement(JsonVariant& jb, const String& name) {
  bool  rc = jb.success();
  if (rc) {
    if (jb.is<JsonArray>()) {
      JsonArray&  elements = jb.as<JsonArray>();
      for (JsonObject& element : elements) {
        AutoConnectElement& elm = _loadElement(element, name);
        if (!elm.name.length()) {
          rc = false;
          break;
        }
      }
    }
    else {
      JsonObject& element = jb.as<JsonObject>();
      AutoConnectElement& elm = _loadElement(element, name);
      if (!elm.name.length())
        rc = false;
    }
  }
  return rc;
}

AutoConnectElement& AutoConnectAux::_loadElement(JsonObject& element, const String& name) {
  AutoConnectElement* auxElm = nullptr;
  String  elmName = element.get<String>(F(AUTOCONNECT_JSON_KEY_NAME));
  if (!name.length() || name.equalsIgnoreCase(elmName)) {
    // The specified element is defined in the JSON stream.
    // Loads from JSON object.
    auxElm = getElement(elmName);
    // The element is not created yet, create new one.
    if (!auxElm) {
      if (elmName.length()) {
        if ((auxElm = _createElement(element))) {
          AC_DBG("%s<%d> of %s created\n", elmName.c_str(), (int)(auxElm->typeOf()), uri());
          add(*auxElm);   // Insert to AutoConnect
        }
        else {
          AC_DBG("%s unknown element type\n", elmName.c_str());
        }
      }
      else {
        AC_DBG("Element name missing\n");
      }
    }
    if (auxElm) {
      if (auxElm->loadMember(element))
        AC_DBG("%s<%d> of %s loaded\n", auxElm->name.c_str(), (int)auxElm->typeOf(), uri());
      else {
        // Element type mismatch
        AC_DBG("Type of %s element mismatched\n", elmName.c_str());
      }
    }
  }
  return auxElm ? *auxElm : _nullElement();
}

/**
 * Serialize an element specified the name into the stream.
 * @param  name  An element name to be output.
 * @return Number of bytes output
 */
size_t AutoConnectAux::saveElement(Stream& out, std::vector<String> const& names) {
  size_t  bufferSize = 0;
  size_t  amount = names.size();
  size_t  size_n = 0;

  // Calculate JSON buffer size
  if (amount == 0)
    bufferSize += JSON_OBJECT_SIZE(4);
  if (amount != 1)
    bufferSize += JSON_ARRAY_SIZE(amount);

  for (String name : names)
    for (AutoConnectElement& elm : _addonElm)
      if (elm.name.equalsIgnoreCase(name)) {
        bufferSize += elm.getObjectSize();
        break;
      }

  if (bufferSize > 0) {
    DynamicJsonBuffer jb(bufferSize);
    if (amount == 1) {
      JsonObject& element = jb.createObject();
      for (AutoConnectElement& elm : _addonElm)
        if (elm.name.equalsIgnoreCase(names[0])) {
          elm.serialize(element);
          break;
        }
      size_n = element.printTo(out);
    }
    else if (amount == 0) {
      JsonObject& json = jb.createObject();
      json[F(AUTOCONNECT_JSON_KEY_TITLE)] = _title;
      json[F(AUTOCONNECT_JSON_KEY_URI)] = _uriStr;
      json[F(AUTOCONNECT_JSON_KEY_MENU)] = _menu;
      JsonArray&  elements = json.createNestedArray(F(AUTOCONNECT_JSON_KEY_ELEMENT));
      for (AutoConnectElement& elm : _addonElm) {
        JsonObject& element = elements.createNestedObject();
        elm.serialize(element);
      }
      size_n = json.prettyPrintTo(out);
    }
    else if (amount >= 2) {
      JsonArray& elements = jb.createArray();
      for (String name : names)
        for (AutoConnectElement& elm : _addonElm)
          if (elm.name.equalsIgnoreCase(name)) {
            JsonObject& element = elements.createNestedObject();
            elm.serialize(element);
            break;
          }
      size_n = elements.prettyPrintTo(out);
    }
  }
  return size_n;
}

/**
 * Convert element type from type as String.
 * @param  type  An element type as String
 * @return A type of ACElement_t
 */
ACElement_t AutoConnectAux::_asElementType(const String& type) {
  typedef struct {
    const char* tName;
    ACElement_t tEnum;
  } ACElementType_t;
  static const ACElementType_t  types[] PROGMEM = {
    { AUTOCONNECT_JSON_TYPE_ACBUTTON, AC_Button },
    { AUTOCONNECT_JSON_TYPE_ACCHECKBOX, AC_Checkbox },
    { AUTOCONNECT_JSON_TYPE_ACELEMENT, AC_Element },
    { AUTOCONNECT_JSON_TYPE_ACINPUT, AC_Input },
    { AUTOCONNECT_JSON_TYPE_ACRADIO, AC_Radio },
    { AUTOCONNECT_JSON_TYPE_ACSELECT, AC_Select },
    { AUTOCONNECT_JSON_TYPE_ACSUBMIT, AC_Submit },
    { AUTOCONNECT_JSON_TYPE_ACTEXT, AC_Text }
  };

  ACElement_t  t = AC_Unknown;
  for (size_t n = 0; n < (sizeof(types) / sizeof(ACElementType_t)); n++) {
    if (type.equalsIgnoreCase(String(FPSTR(types[n].tName))))
      return types[n].tEnum;
  }
  return t;
}

#endif // AUTOCONNECT_USE_JSON
