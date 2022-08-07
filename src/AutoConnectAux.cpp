/**
 * Implementation of AutoConnectAux class.
 * @file AutoConnectAux.cpp
 * @author hieromon@gmail.com
 * @version  1.3.7
 * @date 2022-08-07
 * @copyright  MIT license.
 */
#include <algorithm>
#include "AutoConnect.h"
#include "AutoConnectAux.h"
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
  "{{AUX_CSS}}"
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
  "function _bu(url) {"
  "var uri=document.createElement('input');"
  "uri.setAttribute('type','hidden');"
  "uri.setAttribute('name','" AUTOCONNECT_AUXURI_PARAM "');"
  "uri.setAttribute('value','{{AUX_URI}}');"
  "var fm=document.getElementById('_aux');"
  "fm.appendChild(uri);"
  "fm.action=url;"
  "return fm;"
  "}"
  "function _sa(url) {"
  "_bu(url).submit();"
  "}"
  "{{POSTSCRIPT}}"
  "</script>"
  "</body>"
  "</html>"
};

const char AutoConnectAux::_PAGE_SCRIPT_MA[] PROGMEM = {
  "function _ma(el,pos) {"
  "var mag=pos=='p'?el.previousElementSibling:el.nextElementSibling;"
  "mag.innerText=el.value;"
  "}"
};

/**
 * AutoConnectAux default constructor.
 * @param uri     URI of the page.
 * @param title   Title that applies to both the page and the menu item.
 * @param menu    Appearance in menu.
 * @param addons  Vector of AutoConnect Element that the page contains.
 * @param responsive  This AUX page will response the built HTML via PageBuilder.
 */
AutoConnectAux::AutoConnectAux(const String& uri, const String& title, const bool menu, const AutoConnectElementVT addons, const bool responsive)
    : _title(title),
      _menu(menu),
      _responsive(responsive),
      _addonElm(addons),
      _handler(nullptr),
      _order(AC_EXIT_AHEAD),
      _uploadHandler(nullptr) {
  _uri = uri;
  transferEncoding(PageBuilder::TransferEncoding_t::AUTOCONNECT_HTTP_TRANSFER);
}

/**
 * Destructs container of AutoConnectElement and release a unique
 * pointer of AutoConnect instance.
 */
AutoConnectAux::~AutoConnectAux() {
  _addonElm.clear();
  _addonElm.swap(_addonElm);
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
 * Parses the query parameters contained in the http request and fetches
 * the value of AutoConnectElements carried by AutoConnectAux.
 */
void AutoConnectAux::fetchElement(void) {
  WebServerClass*  _webServer = _ac->_webServer.get();
  if (_webServer->hasArg(String(F(AUTOCONNECT_AUXURI_PARAM)))) {
    _ac->_auxUri = _webServer->arg(String(F(AUTOCONNECT_AUXURI_PARAM)));
    _ac->_auxUri.replace("&#47;", "/");
    AC_DBG("fetch %s", _ac->_auxUri.c_str());
    AutoConnectAux* aux = _ac->_aux;
    while (aux) {
      if (aux->_uri == _ac->_auxUri) {
        // Save the value owned by each element contained in the POST body
        // of a current HTTP request to AutoConnectElements.
        aux->_storeElements(_webServer);
        break;
      }
      aux = aux->_next;
    }
  }
}

/**
 * Get already registered AutoConnectElement.
 * @param  name  Element name as const char ptr.
 * @return A pointer to the registered AutoConnectElement.
 */
AutoConnectElement* AutoConnectAux::getElement(const char* name) {
  for (AutoConnectElement& elm : _addonElm) {
    if (elm.name.length() == strlen(name)) {
      const char* p = name;
      const char* elmName = elm.name.c_str();
      while (tolower(*p++) == tolower(*elmName++)) {
        if (!*p)
          return &elm;
      }
    }
  }
  AC_DBG("Element<%s> not registered\n", name);
  return nullptr;
}

/**
 * Get already registered AutoConnectElement.
 * @param  name  Element name as PROGMEM.
 * @return A pointer to the registered AutoConnectElement.
 */
AutoConnectElement* AutoConnectAux::getElement(const __FlashStringHelper* name) {
  for (AutoConnectElement& elm : _addonElm) {
    if (elm.name.length() == strlen_P(reinterpret_cast<PGM_P>(name))) {
      PGM_P p = reinterpret_cast<PGM_P>(name);
      const char* elmName = elm.name.c_str();
      while (tolower(pgm_read_byte(p++)) == tolower(*elmName++)) {
        if (!pgm_read_byte(p))
          return &elm;
      }
    }
  }
  AC_DBG("Element<%s> not registered\n", String(name).c_str());
  return nullptr;
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
 * Validate all AutoConnectInputs value.
 * @return true  Validation successfull
 * @return false Some elements failed validation.
 */
bool AutoConnectAux::isValid(void) const {
  bool  rc = true;
  for (AutoConnectElement& elm : _addonElm)
    if (elm.typeOf() == AC_Input) {
      AutoConnectInput& elmInput = reinterpret_cast<AutoConnectInput&>(elm);
      rc &= elmInput.isValid();
    }
  return rc;
}

/**
 * Send a redirect response from within the AutoConnectAux handler
 * @param  url
 */
void AutoConnectAux::redirect(const char* url) {
  String  location(url);

  WebServerClass* _webServer = _ac->_webServer.get();
  _webServer->sendHeader(String(F("Location")), location, true);
  _webServer->send(302, String(F("text/plain")), "");
  _webServer->client().stop();
  AC_DBG("%s redirect to %s\n", _uri.c_str(), location.c_str());
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
      else if (elm->typeOf() == AC_Range) {
        AutoConnectRange* elmRange = reinterpret_cast<AutoConnectRange*>(elm);
        elmRange->value = value.toInt();
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
    AC_DBG("%s requests upload to %s\n", requestUri.c_str(), _uri.c_str());
    // Selects a valid upload handler before uploading starts.
    // Identify AutoConnectFile with the current upload request and
    // save the value and mimeType attributes.
    AC_DBG("ACFile %s ", upload.name.c_str());
    String  logContext = "missing";

    AutoConnectElementVT  addons;
    AutoConnectAux* aux = _ac->_aux;
    while (aux) {
      if (aux->_uri == requestUri) {
        addons = aux->_addonElm;
        break;
      }
      aux = aux->_next;
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
        if (_currentUpload->exitStart())
          _currentUpload->upload()->onStart(_currentUpload->exitStart());
        if (_currentUpload->exitEnd())
          _currentUpload->upload()->onEnd(_currentUpload->exitEnd());
        if (_currentUpload->exitError())
          _currentUpload->upload()->onError(_currentUpload->exitError());
        if (_currentUpload->exitProgress())
          _currentUpload->upload()->onProgress(_currentUpload->exitProgress());
        AC_DBG_DUMB("attached(%d)\n", (int)_currentUpload->store);
      }

    if (!_upload) {
      if (_uploadHandler) {
        _upload = _uploadHandler;
        AC_DBG_DUMB("enabled\n");
      }
      else {
        AC_DBG_DUMB("missing\n");
      }
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
 * AutoConnectAuxes.
 * @param  aux   A reference of AutoConnectAux.
 */
void AutoConnectAux::_concat(AutoConnectAux& aux) {
  if (_next)
    _next->_concat(aux);
  else
    _next = &aux;
}

/**
 * Register the AutoConnect that owns itself.
 * AutoConnectAux needs to access the AutoConnect member. Also 
 * AutoConnectAux is cataloged by chain list. The _join function 
 * registers AutoConnect in the following AutoConnectAux chain list.
 * @param  ac    A reference of AutoConnect.
 */
void AutoConnectAux::_join(AutoConnect& ac) {
  _ac = &ac;

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
    menuItem = String(FPSTR("<li class=\"lb-item\"><a href=\"")) + String(_uri) + String("\">") + _title + String(FPSTR("</a></li>"));
  if (_next)
    menuItem += _next->_injectMenu(args);
  return menuItem;
}

/**
 * Insert the uri that caused the request to the aux.
 */
const String AutoConnectAux::_indicateUri(PageArgument& args) {
  AC_UNUSED(args);
  String  lastUri = _uri;
  // The following code contains adding and trimming a blank that is
  // wasteful for this function. It exists for avoiding the bug of
  // WString::replace of ESP8266 arduino core 2.5.2.
  // https://github.com/esp8266/Arduino/issues/6192
  String  reps = "/";
  String  replacement = "&#47;";
  if (lastUri.length() == reps.length() + replacement.length())
    lastUri += " ";
  lastUri.replace(reps, replacement);
  lastUri.trim();
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
  // http argument has been prepared.
  // If the current request argument contains AutoConnectElement, it is
  // the form data of the AutoConnectAux page and with this timing save
  // the value of each element.
  fetchElement();

  // Call user handler before HTML generation.
  if (_handler) {
    if (_order & AC_EXIT_AHEAD) {
      AC_DBG("CB in AHEAD %s\n", uri());
      body += _handler(*this, args);
    }
  }

  // Generate HTML for all AutoConnectElements contained in the page.
  _contains = 0x0000;
  for (AutoConnectElement& addon : _addonElm) {
    if (addon.typeOf() != AC_Unknown)
      // Set the type of current AutoConnectElement
      _contains = _contains | (0b1 << (uint16_t)addon.typeOf());

    // Since the style sheet has already drained at the time of the
    // _insertElement function call, it skips the call to the HTML
    // generator by each element.
    if (addon.typeOf() != AC_Style)
      // Invoke an HTML generator by each element
      body += addon.toHTML();
  }

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
 * Insert the JavaScript required for the dynamic behavior of the elements
 * contained in the page at the tail of the page.
 * AutoConnectRange and AutoConnectFile require JavaScript; if an AutoConnectAux
 * page has these elements, the AutoConnectAux handler will automatically insert
 * the JavaScript necessary for its operation.
 * @param  args  A reference of PageArgument but unused.
 * @return HTML string that should be inserted.
 */
const String AutoConnectAux::_insertScript(PageArgument& args) {
  AC_UNUSED(args);
  String  postscript;

  // Insert a script that advances the progress bar of uploading progress.
  if ((_contains >> (uint16_t)AC_Range) & 0b1)
    postscript += String(FPSTR(_PAGE_SCRIPT_MA));

  return postscript;
}

/**
 * AutoConnectAux with responsive=false setting will not send HTTP
 * response by PageBuilder. Its handler needs to send an HTTP response.
 * However, it has the opportunity to properly handle the AutoConnectElements
 * accepted via the HTTP request. This function is an exit that extracts
 * the value of AutoConnectElements contained in an HTTP request from
 * the client and calls the user-sketched AutoConnectAux handler.
 * @param  args  A reference of PageArgument but unused.
 * @return Response content as null string.
 */
const String AutoConnectAux::_nonResponseExit(PageArgument& args) {
  fetchElement();

  if (_handler) {
    AC_DBG("CB %s with no responsive\n", uri());
    (void)_handler(*this, args);
  }

  // Response sending cancellation due to responsive=false setting.
  _ac->_responsePage->cancel();
  return AutoConnect::_emptyString;
}

/**
 * Insert user defined CSS code to AutoConnectAux page.
 * @param  args  A reference of PageArgument but unused.
 * @return HTML string that should be inserted.
 */
const String AutoConnectAux::_insertStyle(PageArgument& args) {
  AC_UNUSED(args);
  String  css = String("");

  for (AutoConnectElement& elm : _addonElm) {
    if (elm.typeOf() == AC_Style)
      css += elm.toHTML();
  }
  return css;
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
    if (uri != _uri) {
      if (_next) {
        elm = _next->_setupPage(uri);
      }
    } else {
      AutoConnect*  mother = _ac;
      // Overwrite actual AutoConnectMenu title to the Aux. page title
      if (_title.length())
        mother->_menuTitle = _title;

      elm = new PageElement();
      // Construct the auxiliary page
      elm->setMold(FPSTR(_PAGE_AUX));

      if (_responsive) {
        elm->addToken(FPSTR("HEAD"), std::bind(&AutoConnect::_token_HEAD, mother, std::placeholders::_1));
        elm->addToken(FPSTR("AUX_TITLE"), std::bind(&AutoConnectAux::_injectTitle, this, std::placeholders::_1));
        elm->addToken(FPSTR("CSS_BASE"), std::bind(&AutoConnect::_token_CSS_BASE, mother, std::placeholders::_1));
        elm->addToken(FPSTR("CSS_UL"), std::bind(&AutoConnect::_token_CSS_UL, mother, std::placeholders::_1));
        elm->addToken(FPSTR("CSS_INPUT_BUTTON"), std::bind(&AutoConnect::_token_CSS_INPUT_BUTTON, mother, std::placeholders::_1));
        elm->addToken(FPSTR("CSS_INPUT_TEXT"), std::bind(&AutoConnect::_token_CSS_INPUT_TEXT, mother, std::placeholders::_1));
        elm->addToken(FPSTR("CSS_LUXBAR"), std::bind(&AutoConnect::_token_CSS_LUXBAR, mother, std::placeholders::_1));
        elm->addToken(FPSTR("AUX_CSS"), std::bind(&AutoConnectAux::_insertStyle, this, std::placeholders::_1));
        elm->addToken(FPSTR("MENU_PRE"), std::bind(&AutoConnect::_token_MENU_PRE, mother, std::placeholders::_1));
        elm->addToken(FPSTR("MENU_AUX"), std::bind(&AutoConnect::_token_MENU_AUX, mother, std::placeholders::_1));
        elm->addToken(FPSTR("MENU_POST"), std::bind(&AutoConnect::_token_MENU_POST, mother, std::placeholders::_1));
        elm->addToken(FPSTR("AUX_URI"), std::bind(&AutoConnectAux::_indicateUri, this, std::placeholders::_1));
        elm->addToken(FPSTR("ENC_TYPE"), std::bind(&AutoConnectAux::_indicateEncType, this, std::placeholders::_1));
        elm->addToken(FPSTR("AUX_ELEMENT"), std::bind(&AutoConnectAux::_insertElement, this, std::placeholders::_1));
        elm->addToken(FPSTR("POSTSCRIPT"), std::bind(&AutoConnectAux::_insertScript, this, std::placeholders::_1));

        // Register authentication
        // Determine the necessity of authentication from the conditions of
        // AutoConnectConfig::authScope and derive the method.
        bool  auth = ((mother->_apConfig.authScope & AC_AUTHSCOPE_AUX) && (mother->_apConfig.auth != AC_AUTH_NONE))
                  || ((mother->_apConfig.authScope & AC_AUTHSCOPE_PARTIAL) && (_httpAuth != AC_AUTH_NONE));
        HTTPAuthMethod  method;
        if (mother->_apConfig.authScope & AC_AUTHSCOPE_PARTIAL)
          method = _httpAuth == AC_AUTH_BASIC ? HTTPAuthMethod::BASIC_AUTH : HTTPAuthMethod::DIGEST_AUTH;
        else
          method = mother->_apConfig.auth == AC_AUTH_BASIC ? HTTPAuthMethod::BASIC_AUTH : HTTPAuthMethod::DIGEST_AUTH;
        mother->_authentication(auth, method);
      }
      else {
        // AutoConnectAux with responsive=false setting refuses to assemble
        // responsive content. It just obtains the value of AutoConnectElements
        // from the query string carried by the HTTP request and call the
        // user-sketched Aux handler.
        // AutoConnect uses the HEAD token that first appears in the
        // AutoConnectAux template to call the AUX handler.
        elm->addToken(FPSTR("HEAD"), std::bind(&AutoConnectAux::_nonResponseExit, this, std::placeholders::_1));
      }
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

        // Copy a value to other elements declared as global.
        if (elm.global) {
          AutoConnectAux* aux = _ac->_aux;
          while (aux) {
            if (aux != this)
              aux->setElementValue(elm.name, elmValue);
            aux = aux->_next;
          }
        }
      }
    }
  }
  AC_DBG_DUMB(",elements stored\n");
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
 * @param  aux  A JsonVariant object that stores each element of AutoConnectAux.
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
  case AC_Range: {
    AutoConnectRange*  cert_elm = new AutoConnectRange;
    return reinterpret_cast<AutoConnectElement*>(cert_elm);
  }
  case AC_Select: {
    AutoConnectSelect*  cert_elm = new AutoConnectSelect;
    return reinterpret_cast<AutoConnectElement*>(cert_elm);
  }
  case AC_Style: {
    AutoConnectStyle*  cert_elm = new AutoConnectStyle;
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
 * @param  size  JsonDocument buffer size.
 * @return true  The element collection successfully loaded.
 * @return false Invalid JSON data occurred. 
 */
bool AutoConnectAux::load(const String& in, const size_t docSize) {
  return _parseJson<const String&>(in, docSize);
}

/**
 * Constructs an AutoConnectAux instance by reading all the
 * AutoConnectElements of the specified URI from the elements passing
 * pointer to JSON stored in pgm_data array.
 * @param  in    AutoConnectAux element data which is described by JSON.
 * @param  size  JsonDocument buffer size.
 * @return true  The element collection successfully loaded.
 * @return false Invalid JSON data occurred. 
 */
bool AutoConnectAux::load(PGM_P in, const size_t size) {
  return _parseJson<const __FlashStringHelper*>(reinterpret_cast<const __FlashStringHelper*>(in), size);
}

/**
 * Constructs an AutoConnectAux instance by reading all the
 * AutoConnectElements of the specified URI from the elements defined
 * JSON stored in pgm_data array.
 * @param  in    AutoConnectAux element data which is described by JSON.
 * @param  size  JsonDocument buffer size.
 * @return true  The element collection successfully loaded.
 * @return false Invalid JSON data occurred. 
 */
bool AutoConnectAux::load(const __FlashStringHelper* in, const size_t size) {
  return _parseJson<const __FlashStringHelper*>(in, size);
}

/**
 * Constructs an AutoConnectAux instance by reading all the
 * AutoConnectElements of the specified URI from the elements defined
 * JSON stored in a Stream.
 * @param  in    AutoConnectAux element data which is described by JSON.
 * @param  size  JsonDocument buffer size.
 * @return true  The element collection successfully loaded.
 * @return false Invalid JSON data occurred. 
 */
bool AutoConnectAux::load(Stream& in, const size_t size) {
  return _parseJson<Stream&>(in, size);
}

/**
 * Load all elements of AutoConectAux page from JSON object.
 * @param  jb    Reference of JSON object
 * @return true  Successfully loaded.
 * @return false loading unsuccessful, JSON parsing error occurred.
 */
bool AutoConnectAux::_load(JsonObject& jb) {
  if (jb.containsKey(F(AUTOCONNECT_JSON_KEY_TITLE)))
    _title = jb[F(AUTOCONNECT_JSON_KEY_TITLE)].as<String>();
  if (jb.containsKey(F(AUTOCONNECT_JSON_KEY_URI)))
    _uri = jb[F(AUTOCONNECT_JSON_KEY_URI)].as<String>();
  else if (!_uri.length()) {
    AC_DBG("Warn. %s loaded null %s\n", _title.c_str(), AUTOCONNECT_JSON_KEY_TITLE);
  }
  if (jb.containsKey(F(AUTOCONNECT_JSON_KEY_MENU)))
    _menu = jb[F(AUTOCONNECT_JSON_KEY_MENU)].as<bool>();
  if (jb.containsKey(F(AUTOCONNECT_JSON_KEY_RESPONSE)))
    _responsive = jb[F(AUTOCONNECT_JSON_KEY_RESPONSE)].as<bool>();
  String  auth = jb[F(AUTOCONNECT_JSON_KEY_AUTH)].as<String>();
  if (auth.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_BASIC)))
    _httpAuth = AC_AUTH_BASIC;
  else if (auth.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_DIGEST)))
    _httpAuth = AC_AUTH_DIGEST;
  if (auth.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_NONE)))
    _httpAuth = AC_AUTH_NONE;
  if (jb.containsKey(F(AUTOCONNECT_JSON_KEY_ELEMENT))) {
    JsonVariant elements = jb[F(AUTOCONNECT_JSON_KEY_ELEMENT)];
    (void)_loadElement(elements, "");
  }
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
 * @param  docSize Required size to JsonDocument buffer should be allocated.
 * @return A reference of loaded AutoConnectElement instance.
 */
bool AutoConnectAux::loadElement(PGM_P in, const String& name, const size_t docSize) {
  return _parseElement<const __FlashStringHelper*, const String&>(reinterpret_cast<const __FlashStringHelper*>(in), name, docSize);
}

bool AutoConnectAux::loadElement(const __FlashStringHelper* in, const String& name, const size_t docSize) {
  return _parseElement<const __FlashStringHelper*, const String&>(in, name, docSize);
}

bool AutoConnectAux::loadElement(const String& in, const String& name, const size_t docSize) {
  return _parseElement<const String&, const String&>(in, name, docSize);
}
bool AutoConnectAux::loadElement(Stream& in, const String& name, const size_t docSize) {
  return _parseElement<Stream&, const String&>(in, name, docSize);
}

bool AutoConnectAux::loadElement(PGM_P in, std::vector<String> const& names, const size_t docSize) {
  return _parseElement<const __FlashStringHelper*, std::vector<String> const&>(reinterpret_cast<const __FlashStringHelper*>(in), names, docSize);
}

bool AutoConnectAux::loadElement(const __FlashStringHelper* in, std::vector<String> const& names, const size_t docSize) {
  return _parseElement<const __FlashStringHelper*, std::vector<String> const&>(in, names, docSize);
}

bool AutoConnectAux::loadElement(const String& in, std::vector<String> const& names, const size_t docSize) {
  return _parseElement<const String&, std::vector<String> const&>(in, names, docSize);
}

bool AutoConnectAux::loadElement(Stream& in, std::vector<String> const& names, const size_t docSize) {
  return _parseElement<Stream&, std::vector<String> const&>(in, names, docSize);
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
    bufferSize += sizeof(AUTOCONNECT_JSON_KEY_TITLE) + _title.length() + 1 + sizeof(AUTOCONNECT_JSON_KEY_URI) + _uri.length() + 1 + sizeof(AUTOCONNECT_JSON_KEY_RESPONSE) + sizeof(AUTOCONNECT_JSON_KEY_MENU) + sizeof(AUTOCONNECT_JSON_KEY_ELEMENT) + sizeof(AUTOCONNECT_JSON_KEY_AUTH) + sizeof(AUTOCONNECT_JSON_VALUE_DIGEST);
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
      json[F(AUTOCONNECT_JSON_KEY_URI)] = _uri;
      json[F(AUTOCONNECT_JSON_KEY_RESPONSE)] = _responsive;
      json[F(AUTOCONNECT_JSON_KEY_MENU)] = _menu;
      if (_httpAuth == AC_AUTH_BASIC)
        json[F(AUTOCONNECT_JSON_KEY_AUTH)] = String(F(AUTOCONNECT_JSON_VALUE_BASIC));
      else if (_httpAuth == AC_AUTH_DIGEST)
        json[F(AUTOCONNECT_JSON_KEY_AUTH)] = String(F(AUTOCONNECT_JSON_VALUE_DIGEST));
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
    { AUTOCONNECT_JSON_TYPE_ACRANGE, AC_Range },
    { AUTOCONNECT_JSON_TYPE_ACSELECT, AC_Select },
    { AUTOCONNECT_JSON_TYPE_ACSTYLE, AC_Style },
    { AUTOCONNECT_JSON_TYPE_ACSUBMIT, AC_Submit },
    { AUTOCONNECT_JSON_TYPE_ACTEXT, AC_Text }
  };

  ACElement_t  t = AC_Unknown;
  for (size_t n = 0; n < (sizeof(types) / sizeof(ACElementType_t)); n++) {
    if (type.equalsIgnoreCase(String(FPSTR(types[n].tName))))
      return types[n].tEnum;
  }
  AC_DBG("%s unknown type", type.c_str());
  return t;
}

#endif // !AUTOCONNECT_USE_JSON
