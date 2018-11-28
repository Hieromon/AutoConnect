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

class AutoConnect;

// Manage placed AutoConnectElement with a vector
typedef std::vector<std::reference_wrapper<AutoConnectElement>> AutoConnectElementVT;

// A type of callback function when  AutoConnectAux page requested.
//typedef std::function<void(AutoConnectAux&, PageArgument&)> AuxHandleFuncT;
typedef std::function<String(PageArgument&)>  AuxHandlerFunctionT;

// A type for the order in which callback functions are called.
typedef enum {
  AC_EXIT_AHEAD = 1,
  AC_EXIT_LATER = 2,
  AC_EXIT_BOTH = 3
} AutoConnectExitOrder_t;

//class AutoConnect;  // Reference to avoid circular

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
  bool load(const char* in);
  bool load(const __FlashStringHelper* in);
  bool load(Stream& in);
  AutoConnectElement& loadElement(const char* in, const String name = "*");
  AutoConnectElement& loadElement(const __FlashStringHelper* in, const String name = "*");
  AutoConnectElement& loadElement(Stream& in, const String name = "*");
  size_t  saveElement(Stream& out, const AutoConnectElement& element);
#endif

 protected:
  void  _concat(AutoConnectAux& aux);
  void  _join(AutoConnect& ac);
  PageElement*  _setupPage(String uri);
  const String  _insertElement(PageArgument& args);
  const String  _injectTitle(PageArgument& args) { return _title; }
  const String  _injectMenu(PageArgument& args);

#ifdef AUTOCONNECT_USE_JSON
  bool _load(JsonObject& in);
  AutoConnectElement& _loadElement(JsonObject& in, const String name);
  AutoConnectElement* _createElement(const JsonObject& json);
  AutoConnectElement* _getElement(const String name);
  static const ACElement_t  _asElementType(const String type);
  static AutoConnectElement&  _nullElement(void);
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
};

#endif // _AUTOCONNECTAUX_H_