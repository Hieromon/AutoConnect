/**
 * Implementation of AutoConnectAux class.
 * @file AutoConnectAuxBasisImpl.h
 * @author hieromon@gmail.com
 * @version  0.9.9
 * @date 2019-05-25
 * @copyright  MIT license.
 */
#include <algorithm>
#include "AutoConnect.h"
#include "AutoConnectAux.h"
#include "AutoConnectAuxImpl.h"
#include "AutoConnectUploadImpl.h"
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
  "<form id='_aux' method=\"post\" onsubmit=\"return false;\" {{ENC_TYPE}}>"
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
    if (elm->typeOf() == AC_Select) {
      AutoConnectSelect* elmSelect = reinterpret_cast<AutoConnectSelect*>(elm);
      elmSelect->select(value);
    }
    else {
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
 * The upload function that overrides the RequestHandler class
 * attached with ESP8266WebServer.
 * This function invokes the upload handler registered by the onUpload
 * function which will be implemented by the user sketch.
 */
void AutoConnectAux::upload(const String& requestUri, const HTTPUpload& upload) {
  if (upload.status == UPLOAD_FILE_START) {
    AC_DBG("%s requests upload to %s\n", requestUri.c_str(), _uriStr.c_str());
    // Selects a valid upload handler before uploading starts.
    // Identify AutoConnectFile with the current upload request and
    // save the value and mimeType attributes.
    AC_DBG("ACFile %s ", upload.name.c_str());
    String  logContext = "missing";

    AutoConnectElementVT  addons;
    AutoConnectAux* aux = _ac->_aux.get();
    while (aux) {
      if (aux->_uriStr == requestUri) {
        addons = aux->_addonElm;
        break;
      }
      aux = aux->_next.get();
    }

    _currentUpload = nullptr;
    for (AutoConnectElement& elm : addons) {
      if (elm.typeOf() == AC_File) {
        _currentUpload = reinterpret_cast<AutoConnectFile*>(&elm);
        // Reset previous value
        _currentUpload->value = String("");
        _currentUpload->mimeType = String("");
        _currentUpload->size = 0;
        // Overwrite with current upload request
        if (upload.name.equalsIgnoreCase(_currentUpload->name)) {
          _currentUpload->value = upload.filename;
          _currentUpload->mimeType = upload.type;
          logContext = "accepted " + _currentUpload->value;
          break;
        }
      }
    }
    AC_DBG_DUMB("%s, handler ", logContext.c_str());

    // If the current upload request is AutoConnectFile without default
    // AutoConnectUpload (i.e. the store attribute is AC_File_Ex),
    // enable the user-owned upload handler activated by the onUpload.
    _upload = nullptr;
    if (_currentUpload)
      if (_currentUpload->attach(_currentUpload->store)) {
        _upload = std::bind(&AutoConnectUploadHandler::upload, _currentUpload->upload(), std::placeholders::_1, std::placeholders::_2);
        AC_DBG_DUMB("attached(%d)\n", (int)_currentUpload->store);
      }

    if (!_upload) {
      if (_uploadHandler) {
        _upload = _uploadHandler;
        AC_DBG_DUMB("enabled\n");
      }
      else
        AC_DBG_DUMB("missing\n");
    }
  }

  // Invokes upload handler
  if (_upload) {
    _upload(requestUri, upload);
    if (_currentUpload)
      _currentUpload->size = upload.totalSize;
    // Upload ended, purge handler
    if (upload.status == UPLOAD_FILE_END || upload.status == UPLOAD_FILE_ABORTED) {
      if (_currentUpload)
        _currentUpload->detach();
      AC_DBG("%d bytes uploaded\n", upload.totalSize);
    }
  }
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
 * Modifying the form of attribute depending on the type of `input` tag
 * contained. If the custom web page contains `input type=file` then
 * allows multipart as ENCTYPE attribute.
 * @param  args  A reference of PageArgument but unused.
 * @return HTML string that should be inserted.
 */
const String AutoConnectAux::_indicateEncType(PageArgument& args) {
  AC_UNUSED(args);
  String  encType = String("");
  for (AutoConnectElement& elm : _addonElm)
    if (elm.typeOf() == AC_File) {
      return String(F("enctype='multipart/form-data'"));
    }
  return AutoConnect::_emptyString;
}

/**
 * Insert the token handler of PageBuilder. This handler inserts HTML
 * elements generated by the whole AutoConnectElements to the auxiliary page.
 * @param  args  A reference of PageArgument but unused.
 * @return HTML string that should be inserted.
 */
const String AutoConnectAux::_insertElement(PageArgument& args) {
  String  body = String("");

  // When WebServerClass::handleClient calls RequestHandler, the parsed
  //  http argument has been prepared.
  // If the current request argument contains AutoConnectElement, it is
  // the form data of the AutoConnectAux page and with this timing save
  // the value of each element.
  WebServerClass*  _webServer = _ac->_webServer.get();
  if (_webServer->hasArg(String(F(AUTOCONNECT_AUXURI_PARAM)))) {
    _ac->_auxUri = _webServer->arg(String(F(AUTOCONNECT_AUXURI_PARAM)));
    _ac->_auxUri.replace("&#47;", "/");
    AutoConnectAux* aux = _ac->_aux.get();
    while (aux) {
      if (aux->_uriStr == _ac->_auxUri) {
        // Save the value owned by each element contained in the POST body
        // of a current HTTP request to AutoConnectElements.
        aux->_storeElements(_webServer);
        break;
      }
      aux = aux->_next.get();
    }
  }

  // Call user handler before HTML generation.
  if (_handler) {
    if (_order & AC_EXIT_AHEAD) {
      AC_DBG("CB in AHEAD %s\n", uri());
      body += _handler(*this, args);
    }
  }

  // Generate HTML for all AutoConnectElements contained in the page.
  for (AutoConnectElement& addon : _addonElm)
    body += addon.toHTML();

  // Call user handler after HTML generation.
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
      elm->addToken(String(FPSTR("ENC_TYPE")), std::bind(&AutoConnectAux::_indicateEncType, this, std::placeholders::_1));
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
  // Retrieve each element value, Overwrites the value of all cataloged
  // AutoConnectElements with arguments inherited from last http request.
  for (AutoConnectElement& elm : _addonElm) {

    // The POST body does not contain the value of the AutoConnectFile,
    // so it can not be obtained with the WebServerClass::arg function.
    // The AutoConnectFile value will be restored from least recent
    // upload request.
    if (elm.typeOf() == AC_File)
      continue;

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

#ifdef AUTOCONNECT_USE_JSON

/**
 * Load AutoConnectAux page from JSON description stored in PROGMEM.
 * This function can load AutoConnectAux for multiple AUX pages written
 * in JSON and is registered in AutoConnect.
 * @param  aux  JSON description to be load.
 * @return true Successfully loaded.
 */
bool AutoConnect::load(PGM_P aux) {
  return _parseJson<const __FlashStringHelper*>(reinterpret_cast<const __FlashStringHelper*>(aux));
}

/**
 * Load AutoConnectAux page from JSON description stored in PROGMEM.
 * This function can load AutoConnectAux for multiple AUX pages written
 * in JSON and is registered in AutoConnect.
 * @param  aux  JSON description to be load.
 * @return true Successfully loaded.
 */
bool AutoConnect::load(const __FlashStringHelper* aux) {
  return _parseJson<const __FlashStringHelper*>(aux);
}

/**
 * Load AutoConnectAux page from JSON description stored in the sketch.
 * This function can load AutoConnectAux for multiple AUX pages written
 * in JSON and is registered in AutoConnect.
 * @param  aux  JSON description to be load.
 * @return true Successfully loaded.
 */
bool AutoConnect::load(const String& aux) {
  return _parseJson<const String&>(aux);
}

/**
 * Load AutoConnectAux page from JSON description from the stream.
 * This function can load AutoConnectAux for multiple AUX pages written
 * in JSON and is registered in AutoConnect.
 * @param  aux  Stream for read AutoConnectAux elements.
 * @return true Successfully loaded.
 */
bool AutoConnect::load(Stream& aux) {
  return _parseJson<Stream&>(aux);
}

/**
 * Load AutoConnectAux page from JSON object.
 * @param  aux  A JsonVariant object that stores each element of AutoConnectAuxl.
 * @return true Successfully loaded.
 */
bool AutoConnect::_load(JsonVariant& auxJson) {
  bool  rc = true;
  if (auxJson.is<JsonArray>()) {
    ArduinoJsonArray  jb = auxJson.as<JsonArray>();
    for (ArduinoJsonObject  auxJson : jb) {
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
    ArduinoJsonObject jb = auxJson.as<JsonObject>();
    AutoConnectAux* newAux = new AutoConnectAux;
    if (newAux->_load(jb))
      join(*newAux);
    else {
      delete newAux;
      rc = false;
    }
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
  String  type = json[F(AUTOCONNECT_JSON_KEY_TYPE)].as<String>();

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
  case AC_File: {
    AutoConnectFile* cert_elm = new AutoConnectFile;
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
  return _parseJson<const String&>(in);
}

/**
 * Constructs an AutoConnectAux instance by reading all the
 * AutoConnectElements of the specified URI from the elements passing
 * pointer to JSON stored in pgm_data array.
 * @param  in    AutoConnectAux element data which is described by JSON.
 * @return true  The element collection successfully loaded.
 * @return false Invalid JSON data occurred. 
 */
bool AutoConnectAux::load(PGM_P in) {
  return _parseJson<const __FlashStringHelper*>(reinterpret_cast<const __FlashStringHelper*>(in));
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
  return _parseJson<const __FlashStringHelper*>(in);
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
  return _parseJson<Stream&>(in);
}

/**
 * Load all elements of AutoConectAux page from JSON object.
 * @param  jb    Reference of JSON object
 * @return true  Successfully loaded.
 * @return false loading unsuccessful, JSON parsing error occurred.
 */
bool AutoConnectAux::_load(JsonObject& jb) {
  _title = jb[F(AUTOCONNECT_JSON_KEY_TITLE)].as<String>();
  _uriStr = jb[F(AUTOCONNECT_JSON_KEY_URI)].as<String>();
  _uri = _uriStr.c_str();
  _menu = jb[F(AUTOCONNECT_JSON_KEY_MENU)].as<bool>();
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
bool AutoConnectAux::loadElement(PGM_P in, const String& name) {
  return _parseElement<const __FlashStringHelper*, const String&>(reinterpret_cast<const __FlashStringHelper*>(in), name);
}

bool AutoConnectAux::loadElement(const __FlashStringHelper* in, const String& name) {
  return _parseElement<const __FlashStringHelper*, const String&>(in, name);
}

bool AutoConnectAux::loadElement(const String& in, const String& name) {
  return _parseElement<const String&, const String&>(in, name);
}
bool AutoConnectAux::loadElement(Stream& in, const String& name) {
  return _parseElement<Stream&, const String&>(in, name);
}

bool AutoConnectAux::loadElement(PGM_P in, std::vector<String> const& names) {
  return _parseElement<const __FlashStringHelper*, std::vector<String> const&>(reinterpret_cast<const __FlashStringHelper*>(in), names);
}

bool AutoConnectAux::loadElement(const __FlashStringHelper* in, std::vector<String> const& names) {
  return _parseElement<const __FlashStringHelper*, std::vector<String> const&>(in, names);
}

bool AutoConnectAux::loadElement(const String& in, std::vector<String> const& names) {
  return _parseElement<const String&, std::vector<String> const&>(in, names);
}

bool AutoConnectAux::loadElement(Stream& in, std::vector<String> const& names) {
  return _parseElement<Stream&, std::vector<String> const&>(in, names);
}

bool AutoConnectAux::_loadElement(JsonVariant& jb, std::vector<String> const& names) {
  bool  rc = true;
  for (const String& name : names)
    rc &= _loadElement(jb, name);
  return rc;
}

bool AutoConnectAux::_loadElement(JsonVariant& jb, const String& name) {
  bool  rc = false;
  if (jb.is<JsonArray>()) {
    ArduinoJsonArray  elements = jb.as<JsonArray>();
    for (ArduinoJsonObject  element : elements) {
      if (name.length()) {
        //Finds an element with the specified name in the JSON array and loads it.
        if (!name.equalsIgnoreCase(element[F(AUTOCONNECT_JSON_KEY_NAME)].as<String>()))
          continue;
      }
      AutoConnectElement& elm = _loadElement(element, name);
      if (elm.name.length())
        rc = true;
      if (name.length())
        break;
    }
  }
  else {
    ArduinoJsonObject element = jb.as<JsonObject>();
    AutoConnectElement& elm = _loadElement(element, name);
    if (elm.name.length())
      rc = true;
  }
  return rc;
}

AutoConnectElement& AutoConnectAux::_loadElement(JsonObject& element, const String& name) {
  AutoConnectElement* auxElm = nullptr;
  String  elmName = element[F(AUTOCONNECT_JSON_KEY_NAME)].as<String>();
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
  if (amount == 0) {
    bufferSize += JSON_OBJECT_SIZE(4);
    bufferSize += sizeof(AUTOCONNECT_JSON_KEY_TITLE) + _title.length() + 1 + sizeof(AUTOCONNECT_JSON_KEY_URI) + _uriStr.length() + 1 + sizeof(AUTOCONNECT_JSON_KEY_MENU) + sizeof(AUTOCONNECT_JSON_KEY_ELEMENT);
    bufferSize += JSON_ARRAY_SIZE(_addonElm.size());
  }
  else
    bufferSize += JSON_ARRAY_SIZE(amount);

  for (AutoConnectElement& elmEach : _addonElm) {
    AutoConnectElement* elm = &elmEach;
    if (amount > 0) {
      String& elmName = elm->name;
      auto aim = std::find_if(names.begin(), names.end(), [&](const String& n) { return n.equalsIgnoreCase(elmName); });
      if (aim == names.end())
        continue;
    }
    bufferSize += elm->getObjectSize();
  }
  // Round up to 16 boundary
  bufferSize = bufferSize > 0 ? ((bufferSize + 16) & (~0xf)) : bufferSize;
  AC_DBG("JSON buffer size:%d\n", bufferSize);

  // Serialization
  if (bufferSize > 0) {
    ArduinoJsonBuffer jb(bufferSize);
    if (amount == 1) {
      ArduinoJsonObject element = ARDUINOJSON_CREATEOBJECT(jb);
      for (AutoConnectElement& elm : _addonElm)
        if (elm.name.equalsIgnoreCase(names[0])) {
          elm.serialize(element);
          break;
        }
      size_n = ARDUINOJSON_PRINT(element, out);
    }
    else if (amount == 0) {
      ArduinoJsonObject json = ARDUINOJSON_CREATEOBJECT(jb);
      json[F(AUTOCONNECT_JSON_KEY_TITLE)] = _title;
      json[F(AUTOCONNECT_JSON_KEY_URI)] = _uriStr;
      json[F(AUTOCONNECT_JSON_KEY_MENU)] = _menu;
      ArduinoJsonArray  elements = json.createNestedArray(F(AUTOCONNECT_JSON_KEY_ELEMENT));
      for (AutoConnectElement& elm : _addonElm) {
        ArduinoJsonObject element = elements.createNestedObject();
        elm.serialize(element);
      }
      size_n = ARDUINOJSON_PRETTYPRINT(json, out);
    }
    else if (amount >= 2) {
      ArduinoJsonArray  elements = ARDUINOJSON_CREATEARRAY(jb);
      for (String name : names)
        for (AutoConnectElement& elm : _addonElm)
          if (elm.name.equalsIgnoreCase(name)) {
            ArduinoJsonObject element = elements.createNestedObject();
            elm.serialize(element);
            break;
          }
      size_n = ARDUINOJSON_PRETTYPRINT(elements, out);
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
    { AUTOCONNECT_JSON_TYPE_ACFILE, AC_File },
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

#endif // !AUTOCONNECT_USE_JSON
