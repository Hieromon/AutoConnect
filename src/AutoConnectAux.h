/**
 *  Declaration of AutoConnectAux class and subordinated AutoConnectElement class.
 *  @file AutoConnectAux.h
 *  @author hieromon@gmail.com
 *  @version  0.9.7
 *  @date 2018-11-17
 *  @copyright  MIT license.
 */

#ifndef _AUTOCONNECTAUX_H_
#define _AUTOCONNECTAUX_H_

#include <vector>
#include <memory>
#include <functional>
#include <PageBuilder.h>

class AutoConnect;  // Reference to avoid circular

/**
 *  AutoConnectAux element base.
 *  Placed a labeled button that can be added by user sketch.
 *  @param  name     Button name string.
 *  @param  value    Button value string.
 */
class AutoConnectElement {
 public:
  explicit AutoConnectElement(const char* name, const char* value) : name(String(name)), value(String(value)) {}
  virtual ~AutoConnectElement() {}
  virtual const String  toHTML(void) const { return value; }  /**< HTML code to be generated */

  String  name;     /**< Element name */
  String  value;    /**< Element value */
};

/**
 *  Button arrangement class, a part of AutoConnectAux element.
 *  Placed a labeled button that can be added by user sketch.
 *  @param  name     Button name string.
 *  @param  value    Button value string.
 *  @param  action   A sting of action code, it contains a simple JavaScript code.
 */
class AutoConnectButton : public AutoConnectElement {
 public:
  explicit AutoConnectButton(const char* name = "", const char* value = "", const String action = String()) : AutoConnectElement(name, value), action(action) {}
  const String  toHTML(void) const;

  String  action;   /**< A script for an onclick */
};

/**
 *  Checkbox arrangement class, a part of AutoConnectAux element.
 *  Place a optionally labeled input-box that can be added by user sketch.
 *  @param  name     Checkbox name string.
 *  @param  value    A string value associated with the input.
 *  @param  label    A label string that follows checkbox, optionally.
 *  The label is placed on the right side of the checkbox.
 */
class AutoConnectCheckbox : public AutoConnectElement {
 public:
  explicit AutoConnectCheckbox(const char* name = "", const char* value = "", const char* label = "") : AutoConnectElement(name, value), label(String(label)) {}
  ~AutoConnectCheckbox() {}
  const String  toHTML(void) const;

  String  label;    /**< A label for a subsequent input box */
};

/**
 *  Input-box arrangement class, a part of AutoConnectAux element.
 *  Place a optionally labeled input-box that can be added by user sketch.
 *  @param  name     Input-box name string.
 *  @param  value    Default value string. This string display as a placeholder by the default.
 *  @param  label    A label string that follows Input-box, optionally.
 *  The label is placed in front of Input-box.
 */
class AutoConnectInput : public AutoConnectElement {
 public:
  explicit AutoConnectInput(const char* name = "", const char* value = "", const char* label = "") : AutoConnectElement(name, value), label(String(label)) {}
  ~AutoConnectInput() {}
  const String  toHTML(void) const;

  String  label;    /**< A label for a subsequent input box */
};

/**
 *  Selection-box arrangement class, A part of AutoConnectAux element.
 *  Place a optionally labeled Selection-box that can be added by user sketch.
 *  @param  name     Input-box name string.
 *  @param  options  String array display in a selection list.
 *  @param  label    A label string that follows Input-box, optionally.
 *  The label is placed in front of Input-box.
 */
class AutoConnectSelect : public AutoConnectElement {
 public:
  explicit AutoConnectSelect(const char* name = "", std::vector<String> options = {}, const char* label = "") : AutoConnectElement(name, ""), label(String(label)), _options(options) {}
  ~AutoConnectSelect() {}
  const String  toHTML(void) const;
  void  option(const String value) { _options.push_back(value); }
  void  empty(void) { _options.clear(); }

  String  label;                /**< A label for a subsequent input box */

 protected:
  std::vector<String> _options;  /**< List options array */
};

/**
 *  Submit button arrangement class, a part of AutoConnectAux element.
 *  Place a submit button with a label that can be added by user sketch.
 *  With the button behavior, the values of the elements contained in
 *  the form would be sent using the post method.
 *  @param  name  Button name string.
 *  @param  value Sending value string.
 *  @param  uri   Sending uri string.
 */
class AutoConnectSubmit : public AutoConnectElement {
 public:
  explicit AutoConnectSubmit(const char* name = "", const char* value = "", const char* uri = "") : AutoConnectElement(name, value), uri(String(uri)) {}
  const String  toHTML(void) const;

  String  uri;    /**< An url of submitting to */
};

/**
 *  Text arrangement class, a part of AutoConnectAux element.
 *  @param
 *  @param  name     Text name string.
 *  @param  value    Text value string.
 *  @param  style    A string of style-code for decoration, optionally.
 *  An arrangement text would be placed with <div> contains. A string
 *  of style-codes are given for '<div style=>'.
 */
class AutoConnectText : public AutoConnectElement {
 public:
  explicit AutoConnectText(const char* name = "", const char* value = "", const char* style = "") : AutoConnectElement(name, value), style(String(style)) {}
  ~AutoConnectText() {}
  const String  toHTML(void) const;

  String  style;  /**< CSS style modifier native code */
};

/**
 *  Support declare the AutoConnectElement variable with reducing the
 *  arguments. These macros declare the AutoConnectElement variable
 *  with the same name as a "name" argument.
 */ 
#define ACElement(n, v)    AutoConnectElements n(#n, v)
#define ACButton(n, ...)   AutoConnectButton n(#n, ## __VA_ARGS__)
#define ACCheckbox(n, ...) AutoConnectCheckbox n(#n, ## __VA_ARGS__)
#define ACInput(n, ...)    AutoConnectInput n(#n, ## __VA_ARGS__)
#define ACSelect(n, ...)   AutoConnectSelect n(#n, ## __VA_ARGS__)
#define ACSubmit(n, ...)   AutoConnectSubmit n(#n, ## __VA_ARGS__)
#define ACText(n, ...)     AutoConnectText n(#n, ## __VA_ARGS__)

// Manage placed AutoConnectElement with a vector
typedef std::vector<std::reference_wrapper<AutoConnectElement>> AutoConnectElementVT;

// A type of callback function when  AutoConnectAux page requested.
//typedef std::function<void(AutoConnectAux&, PageArgument&)> AuxHandleFuncT;
typedef std::function<void(PageArgument&)>  AuxHandlerFunctionT;

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
  void  on(const AuxHandlerFunctionT handler) { _handler = handler; }   /**< Set user handler */

 protected:
  void  _concat(AutoConnectAux& aux);
  void  _join(AutoConnect& ac);
  PageElement*  _setupPage(String uri);
  const String  _insertElement(PageArgument& args);
  const String  _injectTitle(PageArgument& args) { return _title; }
  const String  _injectMenu(PageArgument& args);
  const String  _exitHandle(PageArgument& args);

  String  _title;                             /**< A title of the page */
  bool    _menu;                              /**< Switch for menu displaying */
  AutoConnectElementVT  _addonElm;            /**< A vector set of AutoConnectElements placed on this auxiliary page */
  std::unique_ptr<AutoConnectAux> _next;      /**< Auxiliary pages chain list */
  std::unique_ptr<AutoConnect>    _ac;        /**< Hosted AutoConnect instance */
  AuxHandlerFunctionT   _handler;             /**< User sketch callback function when AutoConnectAux page requested. */

  static const char _PAGE_AUX[] PROGMEM;      /**< Auxiliary page template */

  // Protected members can be used from AutoConnect which handles AutoConnectAux pages.
  friend class AutoConnect;
};

#endif  // _AUTOCONNECTAUX_H_