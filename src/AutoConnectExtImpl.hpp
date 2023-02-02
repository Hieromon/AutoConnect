/**
 * AutoConnectExt class implementation.
 * @file AutoConnectExtImpl.hpp
 * @author hieromon@gmail.com
 * @version 1.4.2
 * @date 2023-01-13
 * @copyright MIT license.
 */

#ifndef _AUTOCONNECTEXTIMPL_HPP_
#define _AUTOCONNECTEXTIMPL_HPP_

#include "AutoConnectExt.hpp"

/**
 * A destructor. Free AutoConnect web pages and release Web server.
 * When the server is hosted it will be purged.
 */
template<typename T>
AutoConnectExt<T>::~AutoConnectExt() {
  end();
}

/**
 * Stops AutoConnect captive portal service.
 */
template<typename T>
void AutoConnectExt<T>::end(void) {
  // _currentPageElement.reset();
  // _ticker.reset();
  _update.reset();
  _ota.reset();

  // _stopPortal();
  // _dnsServer.reset();
  // _webServer.reset();
  AutoConnectCore<T>::end();
}

/**
 * Creates an AutoConnectAux dynamically with the specified URI and
 * integrates it into the menu. Returns false if a menu item with
 * the same URI already exists.
 * @param  uri    An uri of a new item to add
 * @param  title  Title of the menu item
 * @return A pointer to an added AutoConnectAux
 */
template<typename T>
AutoConnectAux* AutoConnectExt<T>::append(const String& uri, const String& title) {
  AutoConnectAux* reg = aux(uri);
  if (!reg) {
    reg = new AutoConnectAux(uri, title);
    AC_DBG("%s newly added\n", uri.c_str());
    reg->_deletable = true;
    join(*reg);
    return reg;
  }
  AC_DBG("%s has already listed\n", uri.c_str());
  return nullptr;
}

/**
 * Creates an AutoConnectAux dynamically with the specified URI and
 * integrates it into the menu. It will register the request handler
 * for the WebServer after the addMenuItem works. It has similar
 * efficacy to calling addMenuItem and WebSever::on at once.
 * @param  uri      An uri of a new item to add
 * @param  title    Title of the menu item
 * @param  handler  Function of the request handler for WebServer class
 * @return A pointer to an added AutoConnectAux
 */
template<typename T>
AutoConnectAux* AutoConnectExt<T>::append(const String& uri, const String& title, WebServer::THandlerFunction handler) {
  if (AutoConnectCore<T>::_webServer) {
    AutoConnectAux* reg = append(uri, title);
    if (reg)
      AutoConnectCore<T>::_webServer->on(uri, handler);
    return reg;
  }
  AC_DBG("No WebServer instance\n");
  return nullptr;
}

/**
 * Detach a AutoConnectAux from the portal.
 * @param  uri    An uri of the AutoConnectAux should be released
 * @return true   Specified AUX has released
 * @return false  Specified AUX not registered
 */
template<typename T>
bool AutoConnectExt<T>::detach(const String &uri) {
  AutoConnectAux**  self = &_aux;
  while (*self) {
    if (!strcmp((*self)->uri(), uri.c_str())) {
      AC_DBG("%s released\n", (*self)->uri());
      AutoConnectAux* ref = *self;
      *self = (*self)->_next;
      if (ref->_deletable)
        delete ref;
      return true;
    }
    self = &((*self)->_next);
  }
  AC_DBG("%s not listed\n", uri.c_str());
  return false;
}

/**
 * Append auxiliary pages made up with AutoConnectAux.
 * @param  aux  A vector of reference to AutoConnectAux that made up
 * the auxiliary page to be added.
 */
template<typename T>
void AutoConnectExt<T>::join(AutoConnectAuxVT auxVector) {
  for (std::reference_wrapper<AutoConnectAux> aux : auxVector)
    join(aux.get());
}

/**
 * Register the exit routine for AutoConnectAux.
 * @param  uri      Specify the URI of the AutoConnectAux page that
 * registers the exit routine.
 * @param  handler  A handler function of the exit routine.
 * @param  order    Specify an enumeration type of
 * AutoConnectExitOrder_t for the call timing of the exit routine.
 * @return true     An exit routine registered.
 * @return false    AutoConnectAux page for the specified URI is not
 * registered.
 */
template<typename T>
bool AutoConnectExt<T>::on(const String& uri, const AuxHandlerFunctionT handler, AutoConnectExitOrder_t order) {
  AutoConnectAux* aux = _aux;
  while (aux) {
    if (!strcmp(uri.c_str(), aux->uri())) {
      aux->on(handler, order);
      return true;
    }
    aux = aux->_next;
  }
  AC_DBG("%s not registered, handler ignored\n", uri.c_str());
  return false;
}

/**
 * Register a status change notification callback function
 * @param  fn  A status change notification callback function.
 */
template<typename T>
void AutoConnectExt<T>::onOTAStart(OTAStartExit_ft fn) {
  _onOTAStartExit = fn;
}

/**
 * Register a status change notification callback function
 * @param  fn  A status change notification callback function.
 */
template<typename T>
void AutoConnectExt<T>::onOTAEnd(OTAEndExit_ft fn) {
  _onOTAEndExit = fn;
}

/**
 * Register a status change notification callback function
 * @param  fn  A status change notification callback function.
 */
template<typename T>
void AutoConnectExt<T>::onOTAError(OTAErrorExit_ft fn) {
  _onOTAErrorExit = fn;
}

/**
 * Register a status change notification callback function
 * @param  fn  A status change notification callback function.
 */
template<typename T>
void AutoConnectExt<T>::onOTAProgress(OTAProgressExit_ft fn) {
  _onOTAProgressExit = fn;
}

/**
 * Attach AutoConnectOTA to the AutoConnect::handleRequest loop.
 * Sketches can dynamically control AutoConnectOTA activity during the
 * handleRequest loop: the value of AutoConnectConfig::ota is evaluated
 * each time a handleRequest is executed and the OTA is activated accordingly.
 */
template<typename T>
inline void AutoConnectExt<T>::_attachOTA(void) {
  if (AutoConnectCore<T>::_apConfig.ota == AC_OTA_BUILTIN) {
    if (!_ota) {
      _ota.reset(new AutoConnectOTA());
      _ota->extraCaption = AutoConnectCore<T>::_apConfig.otaExtraCaption;
      _ota->attach(*this);
      _ota->authentication(AutoConnectCore<T>::_apConfig.auth);
      _ota->setTicker(AutoConnectCore<T>::_apConfig.tickerPort, AutoConnectCore<T>::_apConfig.tickerOn);
      if (_onOTAStartExit)
        _ota->onStart(_onOTAStartExit);
      if (_onOTAEndExit)
        _ota->onEnd(_onOTAEndExit);
      if (_onOTAErrorExit)
        _ota->onError(_onOTAErrorExit);
      if (_onOTAStartExit)
        _ota->onProgress(_onOTAProgressExit);
    }
  }
}

/**
 * Activate AutoConnectUpdate if it is attached and incorporate it into
 * the AutoConnect menu.
 */
template<typename T>
inline void AutoConnectExt<T>::_enableUpdate(void) {
  if (_update)
    _update->enable();
}

/**
 * Allow AutoConnect to respond to `/_ac/worker` endpoint requests. That handler
 * as AutoConnectAux::_fetchEndpoint is registered as a callback function for
 * the PageElements that make up the AutoConnectAux page. Its organizer is an
 * AutoConnectExt instance that configures a PageBuilder instance.
 * @param   uri  URL of the requested endpoint. URL of the requested endpoint.
 * This argument is unused in this function and is declared only for interface
 * consistency.
 * @return  Registered PageElement of the handler returning the endpoint response.
 */
template<typename T>
PageElement* AutoConnectExt<T>::_setupFetch(const String& uri) {
  AC_UNUSED(uri);
  PageElement*  endpointElement = new PageElement();

  endpointElement->setMold(FPSTR("{{RES}}"));
  endpointElement->addToken(FPSTR("RES"), std::bind(&AutoConnectAux::_fetchEndpoint, _aux, std::placeholders::_1));
  return endpointElement;
}

/**
 * For OTA, the upload process is handled with each turn of the handleRequst.
 * When the upload is complete and the system is waiting for a reboot, it will
 * prompt a reset operation. If the upload is still in progress, it returns an
 * indicator that post-processing is required for the blinking ticker.
 * @return true   No post-treatment of the ticker is required.
 * @return false  Indicates that the ticker needs to flash.
 */
template<typename T>
inline bool AutoConnectExt<T>::_handleOTA(void) {
  bool  ignoreTicker = false;

  if (_ota) {
    if (_ota->status() == AutoConnectOTA::AC_OTA_RIP) {
      // Indicate the reboot at the next handleClient turn
      // with on completion of the update via OTA.
      if (AutoConnectCore<T>::_webServer->client().connected()) {
        AutoConnectCore<T>::_webServer->client().setNoDelay(true);
        _ota->reset();
      }

      if (_ota->dest() == AutoConnectOTA::OTA_DEST_FIRM)
        // OTA for firmware update requires module reset.
        AutoConnectCore<T>::_rfReset = true;
    }
    else if (_ota->status() == AutoConnectOTA::AC_OTA_PROGRESS)
      ignoreTicker = true;
    // Reflect the menu display specifier from AutoConnectConfig to
    // AutoConnectOTA page
    _ota->menu(AutoConnectCore<T>::_apConfig.menuItems & AC_MENUITEM_UPDATE);
  }
  return ignoreTicker;
}

/**
 * Handle the update behaviors for attached AutoConnectUpdate.
 * Indicate that not disturb the ticker cycle during OTA.
 * It will be set to true during OTA in progress due to subsequent
 * OTA handling.
 * The ticker blinks while the Update process is in progress; when Update
 * is in the active state, each turn of the handleRequest loop indicates
 * the need to control the blinking of the ticker as a return value.
 * @return true   No post-treatment of the ticker is required.
 * @return false  Indicates that the ticker needs to flash.
 */
template<typename T>
inline bool AutoConnectExt<T>::_handleUpdate(void) {
  if (_update) {
    _update->handleUpdate();
    if (_update->status() == UPDATE_PROGRESS)
      return true;
  }
  return false;
}

/**
 * A wrapper of the upload function for the WebServerClass. Invokes the
 * upload function of the AutoConnectAux which has a destination URI.
 */
template<typename T>
void AutoConnectExt<T>::_handleUpload(const String& requestUri, const HTTPUpload& upload) {
  AutoConnectAux* aux = _aux;
  while (aux) {
    if (aux->_uri == requestUri) {
      aux->upload(_prevUri, upload);
      break;
    }
    aux = aux->_next;
  }
}

/**
 * Register a handler for AutoConnect's upload mechanism to WebServer::RequestHandler
 * via PageBuilder.
 * @param  page   PageBuilder that inherits from RequestHandler and handles
 * upload events fired by WebServer.
 */
template<typename T>
inline void AutoConnectExt<T>::_registerOnUpload(PageBuilder* page) {
  page->onUpload(std::bind(&AutoConnectExt<T>::_handleUpload, this, std::placeholders::_1, std::placeholders::_2));
}

/**
 * If the requested URL is AutoConnect Aux, delegate the release of the 
 * page to the page itself.
 * @params uri  Requested URL
 */
template<typename T>
inline void AutoConnectExt<T>::_releaseAux(const String& uri) {
  if (!AutoConnectCore<T>::_currentPageElement && _aux) {
    if (uri == String(F(AUTOCONNECT_URI_FETCH))) {
      // Fetch-API requested. Prepare to accept by pseudo-endpoint.
      AutoConnectCore<T>::_currentPageElement.reset(_setupFetch(uri));
    }
    else {
      // Requested URL is not a normal page, exploring AUX pages
      AutoConnectCore<T>::_currentPageElement.reset(_aux->_setupPage(uri));
    }
  }
}

/**
 * Save current uri for the upload request, it protects the currently
 * requested AutoConnectAux URL from being overwritten by the upload process.
 * @param uri   URL of the currently active AutoConnectAux page.
 */
template<typename T>
void AutoConnectExt<T>::_saveCurrentUri(const String& uri) {
  if (uri.length())
    _prevUri = uri;
}

#ifdef AUTOCONNECT_USE_JSON
// A set of functions for processing JSON descriptions in AutoConnectAux.
// It is instantiated according to the sketch implementation.

/**
 * Parse and load a JSON document which marks up multiple custom web
 * pages. The compiler instantiates this template according to the stored
 * data type that contains the JSON document.
 * This template also generates different parsing function calls
 * depending on the ArduinoJson version.
 * @param  U  An object type of the JSON document which must be a
 * passable object to ArduinoJson.
 * @param  in An instance of a source JSON document to load.
 */
template<typename T> template<typename U>
bool AutoConnectExt<T>::_parseJson(U in) {
  ArduinoJsonBuffer jsonBuffer(AUTOCONNECT_JSONBUFFER_PRIMITIVE_SIZE);
  JsonVariant jv;
#if ARDUINOJSON_VERSION_MAJOR<=5
  jv = jsonBuffer.parse(in);
  if (!jv.success()) {
    AC_DBG("JSON parse error\n");
    return false;
  }
#else
  DeserializationError  err = deserializeJson(jsonBuffer, in);
  if (err) {
    AC_DBG("Deserialize error:%s\n", err.c_str());
    return false;
  }
  jv = jsonBuffer.as<JsonVariant>();
#endif
  return _load(jv);
}

/**
 * Load AutoConnectAux page from JSON description stored in PROGMEM.
 * This function can load AutoConnectAux for multiple AUX pages written
 * in JSON and is registered in AutoConnect.
 * @param  aux  JSON description to be load.
 * @return true Successfully loaded.
 */
template<typename T>
bool AutoConnectExt<T>::load(PGM_P aux) {
  return _parseJson<const __FlashStringHelper*>(reinterpret_cast<const __FlashStringHelper*>(aux));
}

/**
 * Load AutoConnectAux page from JSON description stored in PROGMEM.
 * This function can load AutoConnectAux for multiple AUX pages written
 * in JSON and is registered in AutoConnect.
 * @param  aux  JSON description to be load.
 * @return true Successfully loaded.
 */
template<typename T>
bool AutoConnectExt<T>::load(const __FlashStringHelper* aux) {
  return _parseJson<const __FlashStringHelper*>(aux);
}

/**
 * Load AutoConnectAux page from JSON description stored in the sketch.
 * This function can load AutoConnectAux for multiple AUX pages written
 * in JSON and is registered in AutoConnect.
 * @param  aux  JSON description to be load.
 * @return true Successfully loaded.
 */
template<typename T>
bool AutoConnectExt<T>::load(const String& aux) {
  return _parseJson<const String&>(aux);
}

/**
 * Load AutoConnectAux page from JSON description from the stream.
 * This function can load AutoConnectAux for multiple AUX pages written
 * in JSON and is registered in AutoConnect.
 * @param  aux  Stream for read AutoConnectAux elements.
 * @return true Successfully loaded.
 */
template<typename T>
bool AutoConnectExt<T>::load(Stream& aux) {
  return _parseJson<Stream&>(aux);
}

/**
 * Load AutoConnectAux page from JSON object.
 * @param  aux  A JsonVariant object that stores each element of AutoConnectAux.
 * @return true Successfully loaded.
 */
template<typename T>
bool AutoConnectExt<T>::_load(JsonVariant& auxJson) {
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

#endif // !AUTOCONNECT_USE_JSON

/**
 * Of the token handlers that make up AutoConnectPage, this handles tokens
 * that depend on the AutoConnectAux component. It mixes HTML into the 
 * utoConnect menu to materialize AutoConnectAux as a menu item.
 */
template<typename T>
String AutoConnectExt<T>::_mold_MENU_AUX(PageArgument& args) {
  String  menuItem = String("");
  if (_aux)
    menuItem = _aux->_injectMenu(args);
  return menuItem;
}

#endif // !_AUTOCONNECTEXTIMPL_HPP_
