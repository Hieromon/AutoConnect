/**
 * Declaration of AutoConnectAux basic class.
 * @file AutoConnectAuxBasis.h
 * @author hieromon@gmail.com
 * @version  0.9.7
 * @date 2018-11-17
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTAUX_H_
#define _AUTOCONNECTAUX_H_

#include "AutoConnectDefs.h"
#include <vector>
#include <memory>
#include <functional>
#ifdef AUTOCONNECT_USE_JSON
#include <Stream.h>
#endif
#include <PageBuilder.h>
#include "AutoConnectElement.h"

#define AUTOCONENCT_JSONOBJECTTREE_MAXDEPTH   3  

class AutoConnect;  // Reference to avoid circular

// Manage placed AutoConnectElement with a vector
typedef std::vector<std::reference_wrapper<AutoConnectElement>> AutoConnectElementVT;

// A type of callback function when  AutoConnectAux page requested.
//typedef std::function<void(AutoConnectAux&, PageArgument&)> AuxHandleFuncT;
typedef std::function<String(PageArgument&)>  AuxHandlerFunctionT;

// A type for the order in which callback functions are called.
typedef enum {
  AC_EXIT_AHEAD = 1,    /**< */
  AC_EXIT_LATER = 2,
  AC_EXIT_BOTH = 3
} AutoConnectExitOrder_t;

/**
 *  A class that handles an auxiliary page with AutoConnectElement
 *  that placed on it by binding it to the AutoConnect menu.
 *  @param  uri     An uri string of this page.
 *  @param  title   A title string of this page.
 *  @param  addons  A set of AutoConnectElement vector.
 *  @param  menu    A switch for item displaying in AutoConnect menu.
 */
class AutoConnectAux : public PageBuilder {
 public:
  explicit AutoConnectAux(const char* uri = nullptr, const char* title = "", const bool menu = true, const AutoConnectElementVT addons = AutoConnectElementVT()) :
    _title(String(title)), _menu(menu), _addonElm(addons) { setUri(uri); _next.release(); _ac.release(); }
  ~AutoConnectAux();
  void  add(AutoConnectElement& addon);                                 /**< Add an element to the auxiliary page. */
  void  add(AutoConnectElementVT addons);                               /**< Add the element set to the auxiliary page. */
  bool  release(const char* name) { return release(String(name)); }     /**< Release an AutoConnectElement */
  bool  release(const String name);                                     /**< Release an AutoConnectElement */
  void  setTitle(const char* title) { _title = String(title); }         /**< Set a title of the auxiliary page. */
  void  menu(const bool post) { _menu = post; }                         /**< Set or reset the display as menu item for this aux. */
  void  on(const AuxHandlerFunctionT handler, const AutoConnectExitOrder_t order = AC_EXIT_AHEAD) { _handler = handler; _order = order; }   /**< Set user handler */

#ifdef AUTOCONNECT_USE_JSON
  bool load(const char* in);                                            /**< Load whole elements to AutoConnectAux Page */
  bool load(const __FlashStringHelper* in);                             /**< Load whole elements to AutoConnectAux Page */
  bool load(Stream& in, const size_t bufferSize = AUTOCONNECT_JSON_BUFFER_SIZE);  /**< Load whole elements to AutoConnectAux Page */
  AutoConnectElement& loadElement(const char* in, const String name = "*");  /**< Load specified element */
  AutoConnectElement& loadElement(const __FlashStringHelper* in, const String name = "*");  /**< Load specified element */
  AutoConnectElement& loadElement(Stream& in, const String name = "*", const size_t bufferSize = AUTOCONNECT_JSON_BUFFER_SIZE);   /**< Load specified element */
  size_t  saveElement(Stream& out, const AutoConnectElement& element);    /**< Load specified element */
#endif

 protected:
  void  _concat(AutoConnectAux& aux);                                   /**< Make up chain of AutoConnectAux */
  void  _join(AutoConnect& ac);                                         /**< Make a link to AutoConnect */
  PageElement*  _setupPage(String uri);                                 /**< AutoConnectAux page builder */
  const String  _insertElement(PageArgument& args);                     /**< Insert a generated HTML to the page built by PageBuilder */
  const String  _injectTitle(PageArgument& args) { return _title; }     /**< Returns title of this page to PageBuilder */
  const String  _injectMenu(PageArgument& args);                        /**< Inject menu title of this page to PageBuilder */

#ifdef AUTOCONNECT_USE_JSON
  bool _load(JsonObject& in);                                           /**< Load all elements from JSON object */
  AutoConnectElement& _loadElement(JsonObject& in, const String name);  /**< Load an element as specified name from JSON object */
  AutoConnectElement* _createElement(const JsonObject& json);           /**< Create an AutoConnectElement instance from JSON object */
  AutoConnectElement* _getElement(const String name);                   /**< Get registered AutoConnectElement as specified name */
  static const ACElement_t  _asElementType(const String type);          /**< Convert a string of element type to the enumeration value */
  static AutoConnectElement&  _nullElement(void);                       /**< A static returning value as invalid */
#endif

  String  _title;                             /**< A title of the page */
  bool    _menu;                              /**< Switch for menu displaying */
  String  _uriStr;                            /**< uri as String */
  AutoConnectElementVT  _addonElm;            /**< A vector set of AutoConnectElements placed on this auxiliary page */
  std::unique_ptr<AutoConnectAux> _next;      /**< Auxiliary pages chain list */
  std::unique_ptr<AutoConnect>    _ac;        /**< Hosted AutoConnect instance */
  AuxHandlerFunctionT   _handler;             /**< User sketch callback function when AutoConnectAux page requested. */
  AutoConnectExitOrder_t  _order;             /**< The order in which callback functions are called. */

  static const char _PAGE_AUX[] PROGMEM;      /**< Auxiliary page template */

  // Protected members can be used from AutoConnect which handles AutoConnectAux pages.
  friend class AutoConnect;

 private:
  size_t  _calcJsonBufferSize(const char* in);  /**< Calculate JSON buffer size for constant character array */
  size_t  _calcJsonBufferSize(const __FlashStringHelper* in);  /**< Calculate JSON buffer size for pgm_data */
  void    _initJsonBufferSize(void);          /**< Initialize the stacks for JSON Dynamic buffer size calculation */
  void    _accJsonBufferSize(const char c);   /**< Accumulate JSON Dynamic buffer size */
  size_t  _resultJsonBufferSize(void);        /**< Retrieve accumulated result value */

  int16_t   _jbSize;                          /**< JSON dynamic buffer size */
  uint16_t  _jbByte;                          /**< Byte count for calculation of JSON buffer */
  uint8_t   _jbObject;                        /**< Object count for calculation of JSON buffer */
  uint8_t   _jbArray;                         /**< Array count for calculation of JSON buffer */
  uint8_t   _jbNest;                          /**< JSON array nest count */
  uint8_t   _kStack[AUTOCONENCT_JSONOBJECTTREE_MAXDEPTH]; /**< JSON array counter stack */
  uint8_t   _nStack[AUTOCONENCT_JSONOBJECTTREE_MAXDEPTH]; /**< JSON object counter stack */
  int8_t    _kp;                              /**< Stack pointer for JSON array counter */
  int8_t    _np;                              /**< Stack pointer for JSON object counter */
  bool      _jbOpen;                          /**< JSON object paring status */
  bool      _jbLiteral;                       /**< JSON object lexical status */
};

#endif // _AUTOCONNECTAUX_H_