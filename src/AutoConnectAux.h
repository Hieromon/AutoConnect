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

// Add-on element base class
class AutoConnectElement {
 public:
  explicit AutoConnectElement(const char* name, const char* value) : _name(String(name)), _value(String(value)) {}
  virtual ~AutoConnectElement() {}
  const String  name(void) const { return _name; }          /**< Element name */
  const String  value(void) const { return _value; }        /**< Element value */
  virtual const String  toHTML(void) { return String(); };  /**< HTML code to be generated */
  void  setValue(const char* value) { _value = String(value); }
  void  setValue(const String value) { _value = value; }

 protected:
  String  _name;
  String  _value;
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
  explicit AutoConnectText(const char* name = "", const char* value = "", const char* style = "") : AutoConnectElement(name, value), _style(String(style)) {}
   ~AutoConnectText() {}
  const String  style(void) const { return _style; }  /**< A modify of HTML 'style' code */
  void  setStyle(const char* style) { _style = String(style); }
  void  setStyle(const String style) { _style = style; }
  const String  toHTML(void) {
    return String(FPSTR("<div style=\"")) + _style + String("\">") + _value + String(FPSTR("</div>"));
  }

 protected:
  String  _style;
};

/**
 *  Input-box arrangement class, a part of AutoConnectAux element.
 *  Place a optionally labeled input-box that can be added by user sketch.
 *  @param  name     Input-box name string.
 *  @param  value    Button value string.
 *  @param  label    A label string that follows Input-box, optionally.
 *  The label is placed in front of Input-box.
 */
class AutoConnectInput : public AutoConnectElement {
 public:
  explicit AutoConnectInput(const char* name = "", const char* value = "", const char* label = "") : AutoConnectElement(name, value), _label(String(label)) {}
  ~AutoConnectInput() {}
  const String  label(void) const { return _label; }
  void  setLabel(const char* label) { _label = String(label); }
  void  setLabel(const String label) { _label = label; }
  const String  toHTML(void) {
    return _label + String(FPSTR("<input type=\"text\" name=\"")) + _name + String(FPSTR("\" value=\"")) + _value + String("\"><br>");
  }

 protected:
  String  _label;
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
  explicit AutoConnectButton(const char* name = "", const char* value = "", const String action = String()) : AutoConnectElement(name, value), _action(action) {}
  const String  action(void) const { return _action; }
  void  setAction(const char* action) { _action = String(action); }
  void  setAction(const String action) { _action = action; }
  const String  toHTML(void) {
    return String(FPSTR("<button type=\"button\" name=\"")) + _name + String(FPSTR("\" value=\"")) + _value + String(FPSTR("\" onclick=\"")) + _action + String("\">") + _value + String(FPSTR("</button>"));
  }

 protected:
  String  _action;
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
  explicit AutoConnectSubmit(const char* name = "", const char* value = "", const char* uri = "") : AutoConnectElement(name, value), _uri(String(uri)) {}
  const String  uri(void) const { return _uri; }
  void  setUri(const char* uri) { _uri = String(uri); }
  void  setUri(const String uri) { _uri = uri; }
  const String  toHTML(void) {
    return String(FPSTR("<button type=\"submit\" name=\"")) + _name + String(FPSTR("\" value=\"")) + _value + String(FPSTR("\" onclick=\"sa('")) + _uri + String("')\">") + _value + String(FPSTR("</button>"));
  }

 protected:
  String  _uri;
};

/**
 *  Support declare the AutoConnectElement variable with reducing the
 *  arguments. These macros declare the AutoConnectElement variable
 *  with the same name as a "name" argument.
 */ 
#define ACText(name, ...) AutoConnectText name(#name, ## __VA_ARGS__)
#define ACInput(name, ...)  AutoConnectInput name(#name, ## __VA_ARGS__)
#define ACButton(name, ...) AutoConnectButton name(#name, ## __VA_ARGS__)
#define ACSubmit(name, ...) AutoConnectSubmit name(#name, ## __VA_ARGS__)

// Manage placed AutoConnectElement with a vector
typedef std::vector<std::reference_wrapper<AutoConnectElement>> AutoConnectElementVT;

// To solve the forward reference.
class AutoConnectAux;

// A type of callback function when  AutoConnectAux page requested.
typedef std::function<void(AutoConnectAux&, PageArgument&)> AuxHandleFuncT;

/**
 *  A class that handles an auxiliary page with AutoConnectElement
 *  that placed on it by binding it to the AutoConnect menu.
 *  @param  uri     An uri string of this page.
 *  @param  title   A title string of this page.
 *  @param  addons  A set of AutoConnectElement vector.
 */
class AutoConnectAux : public PageBuilder {
 public:
  explicit AutoConnectAux(const char* uri = nullptr, const char* title = "", const AutoConnectElementVT addons = AutoConnectElementVT()) :
    _title(String(title)), _addonElm(addons) { setUri(uri); _next.release(); _ac.release(); _handler = nullptr; }
  ~AutoConnectAux();
  void  add(AutoConnectElement& addon);                                 /**< Add an element to the auxiliary page. */
  void  add(AutoConnectElementVT addons);                               /**< Add the element set to the auxiliary page. */
  void  setTitle(const char* title) { _title = String(title); }         /**< Set a title of the auxiliary page. */
  void  on(const AuxHandleFuncT handler) { _handler = &handler; }        /**< Set user handler */

 protected:
  void  _concat(AutoConnectAux& aux);
  void  _join(AutoConnect& ac);
  PageElement*  _setupPage(String uri);
  const String  _insertElement(PageArgument& args);
  const String  _injectTitle(PageArgument& args) { return _title.length() > 0 ? _title : _activeTitle; }
  const String  _injectMenu(PageArgument& args);
  const String  _exitHandle(PageArgument& args);

  String  _title;                             /**< A title of the page */
  String  _activeTitle;                       /**< Previous title of the page */

  AutoConnectElementVT  _addonElm;            /**< A vector set of AutoConnectElements placed on this auxiliary page */
  std::unique_ptr<AutoConnectAux> _next;      /**< Auxiliary pages chain list */
  std::unique_ptr<AutoConnect>    _ac;        /**< Hosted AutoConnect instance */
  const AuxHandleFuncT*   _handler;                 /**< User sketch callback function when AutoConnectAux page requested. */

  static const char _PAGE_AUX[] PROGMEM;      /**< Auxiliary page template */

  // Protected members can be used from AutoConnect which handles AutoConnectAux pages.
  friend class AutoConnect;
};

#endif  // _AUTOCONNECTAUX_H_