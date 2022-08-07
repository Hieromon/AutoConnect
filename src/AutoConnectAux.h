/**
 * Declaration of AutoConnectAux basic class.
 * @file AutoConnectAux.h
 * @author hieromon@gmail.com
 * @version  1.3.7
 * @date 2022-08-07
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTAUX_H_
#define _AUTOCONNECTAUX_H_

#include "AutoConnectDefs.h"
#include <vector>
#include <memory>
#include <functional>
#include <type_traits>
#ifdef AUTOCONNECT_USE_JSON
#include <Stream.h>
#endif // !AUTOCONNECT_USE_JSON
#include <PageBuilder.h>
#include "AutoConnectElement.h"
#include "AutoConnectTypes.h"

class AutoConnect;  // Reference to avoid circular
class AutoConnectAux;  // Reference to avoid circular

// Manage placed AutoConnectElement with a vector
typedef std::vector<std::reference_wrapper<AutoConnectElement>> AutoConnectElementVT;

// A type of callback function when  AutoConnectAux page requested.
typedef std::function<String(AutoConnectAux&, PageArgument&)>  AuxHandlerFunctionT;

// A type for the order in which callback functions are called.
typedef enum {
  AC_EXIT_AHEAD = 1,    /**< Callback before building HTML */
  AC_EXIT_LATER = 2,    /**< Callback after building HTML */
  AC_EXIT_BOTH = 3      /**< Callback twice before and after building HTML */
} AutoConnectExitOrder_t;

/**
 * A class that handles an auxiliary page with AutoConnectElement
 * that placed on it by binding it to the AutoConnect menu.
 * @param  uri        An uri string of this page.
 * @param  title      A title string of this page.
 * @param  menu       A switch for item displaying in AutoConnect menu.
 * @param  addons     A set of AutoConnectElement vector.
 * @param  responsive A switch for whether to make HTTP response or not.
 */
class AutoConnectAux : public PageBuilder {
 public:
  explicit AutoConnectAux(const String& uri = String(""), const String& title = String(""), const bool menu = true, const AutoConnectElementVT addons = AutoConnectElementVT(), const bool responsive = true);
  ~AutoConnectAux();
  AutoConnectElement& operator[](const char* name) { return *getElement(name); }
  AutoConnectElement& operator[](const __FlashStringHelper* name) { return *getElement(name); }
  AutoConnectElement& operator[](const String& name) { return *getElement(name); }
  void  add(AutoConnectElement& addon);                                 /**< Add an element to the auxiliary page */
  void  add(AutoConnectElementVT addons);                               /**< Add the element set to the auxiliary page */
  AutoConnectElement&   at(const unsigned int index) { return _addonElm.at(index); } /**< Gets the AutoConnectElement pointed to by the index */
  void  authentication(const AC_AUTH_t auth) { _httpAuth = auth; }      /**< Set certain page authentication */
  size_t  content(void) const { return _addonElm.size(); }              /**< Returns number of contained AutoConnectElements */
  void  fetchElement(void);                                             /**< Fetch AutoConnectElements values from http query parameters */
  template<typename T>
  T&    getElement(const char* name);
  template<typename T>
  T&    getElement(const __FlashStringHelper* name);
  template<typename T>
  T&    getElement(const String& name);
  AutoConnectElement*   getElement(const char* name);                   /**< Get registered AutoConnectElement as specified name */
  AutoConnectElement*   getElement(const __FlashStringHelper* name);    /**< Get registered AutoConnectElement as specified name */
  AutoConnectElement*   getElement(const String& name);                 /**< Get registered AutoConnectElement as specified name */
  AutoConnectElementVT& getElements(void) { return _addonElm; }         /**< Get vector of all elements */
  void  menu(const bool post) { _menu = post; }                         /**< Set or reset the display as menu item for this aux */
  bool  isMenu(void) { return _menu; }                                  /**< Return whether embedded in the menu or not */
  bool  isValid(void) const;                                            /**< Validate all AutoConnectInput value */
  void  redirect(const char* url);                                      /**< Send a redirect response from within the AutoConnectAux handler */ 
  bool  release(const String& name);                                    /**< Release an AutoConnectElement */
  bool  setElementValue(const String& name, const String value);        /**< Set value to specified element */
  bool  setElementValue(const String& name, std::vector<String> const& values);  /**< Set values collection to specified element */
  void  setTitle(const String& title) { _title = title; }               /**< Set a title of the auxiliary page */
  void  on(const AuxHandlerFunctionT handler, const AutoConnectExitOrder_t order = AC_EXIT_AHEAD) { _handler = handler; _order = order; }   /**< Set user handler */
  void  onUpload(PageBuilder::UploadFuncT uploadFunc) override { _uploadHandler = uploadFunc; }
  template<typename T>
  void  onUpload(T& uploadClass) {
    static_assert(std::is_base_of<AutoConnectUploadHandler, T>::value, "onUpload type must be inherited AutoConnectUploadHandler");
    _uploadHandler = std::bind(&T::upload, &uploadClass, std::placeholders::_1, std::placeholders::_2);
  }

#ifdef AUTOCONNECT_USE_JSON
  bool  load(PGM_P in, const size_t docSize = AUTOCONNECT_JSONDOCUMENT_SIZE);                       /**< Load whole elements to AutoConnectAux Page */
  bool  load(const __FlashStringHelper* in, const size_t docSize = AUTOCONNECT_JSONDOCUMENT_SIZE);  /**< Load whole elements to AutoConnectAux Page */
  bool  load(const String& in, const size_t docSize = AUTOCONNECT_JSONDOCUMENT_SIZE);               /**< Load whole elements to AutoConnectAux Page */
  bool  load(Stream& in, const size_t docSize = AUTOCONNECT_JSONDOCUMENT_SIZE);                     /**< Load whole elements to AutoConnectAux Page */
  bool  loadElement(PGM_P in, const String& name = String(""), const size_t docSize = AUTOCONNECT_JSONDOCUMENT_SIZE);   /**< Load specified element */
  bool  loadElement(PGM_P in, std::vector<String> const& names, const size_t docSize = AUTOCONNECT_JSONDOCUMENT_SIZE);  /**< Load any specified elements */
  bool  loadElement(const __FlashStringHelper* in, const String& name = String(""), const size_t docSize = AUTOCONNECT_JSONDOCUMENT_SIZE);  /**< Load specified element */
  bool  loadElement(const __FlashStringHelper* in, std::vector<String> const& names, const size_t docSize = AUTOCONNECT_JSONDOCUMENT_SIZE); /**< Load any specified elements */
  bool  loadElement(const String& in, const String& name = String(""), const size_t docSize = AUTOCONNECT_JSONDOCUMENT_SIZE); /**< Load specified element */
  bool  loadElement(const String& in, std::vector<String> const& names, const size_t docSize = AUTOCONNECT_JSONDOCUMENT_SIZE);/**< Load any specified elements */
  bool  loadElement(Stream& in, const String& name = String(""), const size_t docSize = AUTOCONNECT_JSONDOCUMENT_SIZE);       /**< Load specified element */
  bool  loadElement(Stream& in, std::vector<String> const& names, const size_t docSize = AUTOCONNECT_JSONDOCUMENT_SIZE);      /**< Load any specified elements */
  size_t  saveElement(Stream& out, std::vector<String> const& names = {});  /**< Write elements of AutoConnectAux to the stream */
#endif // !AUTOCONNECT_USE_JSON

  // Attribute definition of the element to be placed on the update page.
  typedef struct {
    const ACElement_t type;
    const char*  name;      /**< Name to assign to AutoConnectElement */
    const char*  value;     /**< Value owned by an element */
    const char*  peculiar;  /**< Specific ornamentation for the element */
  } ACElementProp_t;

  // Attributes to treat included update pages as AutoConnectAux.
  typedef struct {
    const char*  uri;       /**< URI for the page */
    const char*  title;     /**< Menu title of update page */
    const bool   menu;      /**< Whether to display in menu */
    const ACElementProp_t* element;
  } ACPage_t;

 protected:
  void  upload(const String& requestUri, const HTTPUpload& upload);     /**< Uploader wrapper */
  void  _concat(AutoConnectAux& aux);                                   /**< Make up chain of AutoConnectAux */
  virtual void  _join(AutoConnect& ac);                                 /**< Make a link to AutoConnect */
  PageElement*  _setupPage(const String& uri);                          /**< AutoConnectAux page builder */
  const String  _insertElement(PageArgument& args);                     /**< Insert a generated HTML to the page built by PageBuilder */
  const String  _insertScript(PageArgument& args);                      /**< Insert post-javascript to the page built by PageBuilder */
  const String  _insertStyle(PageArgument& args);                       /**< Insert CSS style */
  const String  _injectTitle(PageArgument& args) const { (void)(args); return _title; } /**< Returns title of this page to PageBuilder */
  const String  _injectMenu(PageArgument& args);                        /**< Inject menu title of this page to PageBuilder */
  const String  _indicateUri(PageArgument& args);                       /**< Inject the uri that caused the request */
  const String  _indicateEncType(PageArgument& args);                   /**< Inject the ENCTYPE attribute */
  const String  _nonResponseExit(PageArgument& args);                   /**< Exit for responsive=false setting */
  void  _storeElements(WebServerClass* webServer);                      /**< Store element values from contained in request arguments */
  template<typename T>
  bool  _isCompatible(const AutoConnectElement* element) const;         /**< Validate a type of AutoConnectElement entity conformity */
  static AutoConnectElement&  _nullElement(void);                       /**< A static returning value as invalid */

#ifdef AUTOCONNECT_USE_JSON
  bool  _load(JsonObject& in);                                          /**< Load all elements from JSON object */
  bool  _loadElement(JsonVariant& in, const String& name);              /**< Load an element as specified name from JSON object */
  bool  _loadElement(JsonVariant& in, std::vector<String> const& names);  /**< Load any elements as specified name from JSON object */
  AutoConnectElement& _loadElement(JsonObject& in, const String& name); /**< Load an element as specified name from JSON object */
  AutoConnectElement* _createElement(const JsonObject& json);           /**< Create an AutoConnectElement instance from JSON object */
  static ACElement_t  _asElementType(const String& type);               /**< Convert a string of element type to the enumeration value */
  /**
   * Parse and load a JSON document which marks up a custom web page.
   * The compiler instantiates this template according to the stored data
   * type that contains the JSON document.
   * This template also generates different parsing function calls
   * depending on the ArduinoJson version.
   * @param  T  An object type of the JSON document which must be a
   * passable object to ArduinoJson.
   * @param  in An instance of a source JSON document to load.
   */
  template<typename T>
  bool _parseJson(T in, const size_t size) {
    ArduinoJsonBuffer jsonBuffer(AUTOCONNECT_JSONBUFFER_PRIMITIVE_SIZE);
  #if ARDUINOJSON_VERSION_MAJOR<=5
    JsonObject& jb = jsonBuffer.parseObject(in);
    if (!jb.success()) {
      AC_DBG("JSON parse error\n");
      return false;
    }
  #else
    DeserializationError  err = deserializeJson(jsonBuffer, in);
    if (err) {
      AC_DBG("Deserialize:%s\n", err.c_str());
      return false;
    }
    JsonObject jb = jsonBuffer.as<JsonObject>();
  #endif
    return _load(jb);
  }

  /**
   * Parse and load a JSON document which declares one of the AutoConnectElement.
   * The compiler instantiates this template according to the stored data type that contains the JSON document.
   * This template also generates different parsing function calls depending on the ArduinoJson version.
   * @param  T  An object type of the JSON document which must be a passable object to ArduinoJson.
   * @param  U  An instance of a source name to load.
   */
  template<typename T, typename U,
  typename std::enable_if<std::is_same<U, const String&>::value || std::is_same<U, std::vector<String> const&>::value>::type* = nullptr>
  bool _parseElement(T in, U name, const size_t size) {
    ArduinoJsonBuffer jsonBuffer(size);
    JsonVariant jb;
  #if ARDUINOJSON_VERSION_MAJOR<=5
    jb = jsonBuffer.parse(in);
    if (!jb.success()) {
      AC_DBG("JSON parse error\n");
      return false;
    }
  #else
    DeserializationError  err = deserializeJson(jsonBuffer, in);
    if (err) {
      AC_DBG("Deserialize:%s\n", err.c_str());
      return false;
    }
    jb = jsonBuffer.as<JsonVariant>();
  #endif
    return _loadElement(jb, name);
  }
#endif // !AUTOCONNECT_USE_JSON

  String  _title;                             /**< A title of the page */
  bool    _menu;                              /**< Switch for menu displaying */
  bool    _deletable = false;                 /**< Allow deleting itself. */
  bool    _responsive;                        /**< Whether suppress the sending of HTTP response in PageBuilder */
  uint16_t  _contains;                        /**< Bitmask the type of elements this page contains */
  AC_AUTH_t _httpAuth = AC_AUTH_NONE;         /**< Applying HTTP authentication */
  AutoConnectElementVT  _addonElm;            /**< A vector set of AutoConnectElements placed on this auxiliary page */
  AutoConnectAux*       _next = nullptr;      /**< Auxiliary pages chain list */
  AutoConnect*          _ac = nullptr;        /**< Hosted AutoConnect instance */
  AuxHandlerFunctionT   _handler;             /**< User sketch callback function when AutoConnectAux page requested. */
  AutoConnectExitOrder_t  _order;             /**< The order in which callback functions are called. */
  PageBuilder::UploadFuncT    _uploadHandler; /**< The AutoConnectFile corresponding to current upload */
  AutoConnectFile*      _currentUpload;       /**< AutoConnectFile handling the current upload */
  static const char _PAGE_AUX[] PROGMEM;      /**< Auxiliary page template */
  static const char _PAGE_SCRIPT_MA[] PROGMEM; /**< Auxiliary page javascript */

  // Protected members can be used from AutoConnect which handles AutoConnectAux pages.
  friend class AutoConnect;
};

/**
 * Implementation of template functions of AutoConnectAux. 
 * This implementation instantiates completely the void AutoConnectElement
 * as each type and also absorbs interface differences due to ArduinoJson
 * version differences. 
 */
/**
 * The validity of AutoConnectElement, the base class of
 * other AutoConnectElement types, is always true.
 * @param  element  AutoConnectElement
 * @return true     The element is feasible. Always true.
 */
template<>
inline bool AutoConnectAux::_isCompatible<AutoConnectElement>(const AutoConnectElement* element) const {
  AC_UNUSED(element);
  return true;
}

/**
 * Validate a type of AutoConnectElement entity conformity.
 * @param  element  AutoConnectButton
 * @return true     The element is feasible.
 */
template<>
inline bool AutoConnectAux::_isCompatible<AutoConnectButton>(const AutoConnectElement* element) const {
  return (element->typeOf() == AC_Button);
}

/**
 * Validate a type of AutoConnectElement entity conformity.
 * @param  element  AutoConnectCheckbox
 * @return true     The element is feasible.
 */
template<>
inline bool AutoConnectAux::_isCompatible<AutoConnectCheckbox>(const AutoConnectElement* element) const {
  return (element->typeOf() == AC_Checkbox);
}

/**
 * Validate a type of AutoConnectElement entity conformity.
 * @param  element  AutoConnectFile
 * @return true     The element is feasible.
 */
template<>
inline bool AutoConnectAux::_isCompatible<AutoConnectFile>(const AutoConnectElement* element) const {
  return (element->typeOf() == AC_File);
}

/**
 * Validate a type of AutoConnectElement entity conformity.
 * @param  element  AutoConnectInput
 * @return true     The element is feasible.
 */
template<>
inline bool AutoConnectAux::_isCompatible<AutoConnectInput>(const AutoConnectElement* element) const {
  return (element->typeOf() == AC_Input);
}

/**
 * Validate a type of AutoConnectElement entity conformity.
 * @param  element  AutoConnectRadio
 * @return true     The element is feasible.
 */
template<>
inline bool AutoConnectAux::_isCompatible<AutoConnectRadio>(const AutoConnectElement* element) const {
  return (element->typeOf() == AC_Radio);
}

/**
 * Validate a type of AutoConnectElement entity conformity.
 * @param  element  AutoConnectRange
 * @return true     The element is feasible.
 */
template<>
inline bool AutoConnectAux::_isCompatible<AutoConnectRange>(const AutoConnectElement* element) const {
  return (element->typeOf() == AC_Range);
}

/**
 * Validate a type of AutoConnectElement entity conformity.
 * @param  element  AutoConnectSelect
 * @return true     The element is feasible.
 */
template<>
inline bool AutoConnectAux::_isCompatible<AutoConnectSelect>(const AutoConnectElement* element) const {
  return (element->typeOf() == AC_Select);
}

/**
 * Validate a type of AutoConnectElement entity conformity.
 * @param  element  AutoConnectSubmit
 * @return true     The element is feasible.
 */
template<>
inline bool AutoConnectAux::_isCompatible<AutoConnectStyle>(const AutoConnectElement* element) const {
  return (element->typeOf() == AC_Style);
}

/**
 * Validate a type of AutoConnectElement entity conformity.
 * @param  element  AutoConnectSubmit
 * @return true     The element is feasible.
 */
template<>
inline bool AutoConnectAux::_isCompatible<AutoConnectSubmit>(const AutoConnectElement* element) const {
  return (element->typeOf() == AC_Submit);
}

/**
 * Validate a type of AutoConnectElement entity conformity.
 * @param  element  AutoConnectText
 * @return true     The element is feasible.
 */
template<>
inline bool AutoConnectAux::_isCompatible<AutoConnectText>(const AutoConnectElement* element) const {
  return (element->typeOf() == AC_Text);
}

/**
 * Get AutoConnectElementJson element.
 * @param  name  an element name.
 * @return A reference of AutoConnectElement class.
 */
template<typename T>
T& AutoConnectAux::getElement(const char* name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (_isCompatible<T>(elm))
      return *(reinterpret_cast<T*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name, elm->typeOf());
  }
  return reinterpret_cast<T&>(_nullElement());
}

/**
 * Get AutoConnectElementJson element.
 * @param  name  an element name.
 * @return A reference of AutoConnectElement class.
 */
template<typename T>
T& AutoConnectAux::getElement(const __FlashStringHelper* name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (_isCompatible<T>(elm))
      return *(reinterpret_cast<T*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", String(name).c_str(), elm->typeOf());
  }
  return reinterpret_cast<T&>(_nullElement());
}

/**
 * Get AutoConnectElementJson element.
 * @param  name  an element name.
 * @return A reference of AutoConnectElement class.
 */
template<typename T>
T& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (_isCompatible<T>(elm))
      return *(reinterpret_cast<T*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<T&>(_nullElement());
}

#endif // !_AUTOCONNECTAUX_H_
