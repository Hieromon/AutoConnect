/**
 * Declaration of AutoConnectElement basic class.
 * @file AutoConnectElementBasis.h
 * @author hieromon@gmail.com
 * @version  0.9.7
 * @date 2018-12-29
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTELEMENTBASIS_H_
#define _AUTOCONNECTELEMENTBASIS_H_

#include <vector>
#include <memory>

typedef enum {
  AC_Button,
  AC_Checkbox,
  AC_Element,
  AC_Input,
  AC_Radio,
  AC_Select,
  AC_Submit,
  AC_Text,
  AC_Unknown
} ACElement_t;      /**< AutoConnectElement class type */

typedef enum {
  AC_Horizontal,
  AC_Vertical
} ACArrange_t;      /**< The element arrange order */

/**
 * AutoConnectAux element base.
 * Placed a raw text that can be added by user sketch.
 * @param  name     A name string for the element.
 * @param  value    A raw text to be placed in HTML.
 */
class AutoConnectElementBasis {
 public:
  explicit AutoConnectElementBasis(const char* name = "", const char* value = "") : name(String(name)), value(String(value)) {
    _type = AC_Element;
  }
  virtual ~AutoConnectElementBasis() {}
  virtual const String  toHTML(void) const { return value; }
  ACElement_t typeOf(void) const { return _type; }

  String  name;       /**< Element name */
  String  value;      /**< Element value */

 protected:
  ACElement_t _type;  /**< Element type identifier */
};

/**
 * Button arrangement class, a part of AutoConnectAux element.
 * Place a labeled button that can be added by user sketch.
 * @param  name     Button element name string.
 * @param  value    Value string with the placed button.
 * @param  action   Script code to execute with the button pushed.
 */
class AutoConnectButtonBasis : virtual public AutoConnectElementBasis {
 public:
  explicit AutoConnectButtonBasis(const char* name = "", const char* value = "", const String& action = String("")) : AutoConnectElementBasis(name, value), action(String(action)) {
    _type = AC_Button;
  }
  ~AutoConnectButtonBasis() {}
  const String  toHTML(void) const override;

  String  action;
};

/**
 * Checkbox arrangement class, a part of AutoConnectAux element.
 * Place a optionally labeled input-box that can be added by user sketch.
 * @param  name     Checkbox name string.
 * @param  value    A string value associated with the input.
 * @param  label    A label string that follows checkbox, optionally.
 * The label is placed on the right side of the checkbox.
 */
class AutoConnectCheckboxBasis : virtual public AutoConnectElementBasis {
 public:
  explicit AutoConnectCheckboxBasis(const char* name = "", const char* value = "", const char* label = "", const bool checked = false) : AutoConnectElementBasis(name, value), label(String(label)), checked(checked) {
    _type = AC_Checkbox;
  }
  virtual ~AutoConnectCheckboxBasis() {}
  const String  toHTML(void) const override;

  String  label;      /**< A label for a subsequent input box */
  bool    checked;    /**< The element should be pre-selected */
};

/**
 * Input-box arrangement class, a part of AutoConnectAux element.
 * Place a optionally labeled input-box that can be added by user sketch.
 * @param  name     Input-box name string.
 * @param  value    Default value string. This string display as a placeholder by the default.
 * @param  label    A label string that follows Input-box, optionally.
 * The label is placed in front of Input-box.
 */
class AutoConnectInputBasis : virtual public AutoConnectElementBasis {
 public:
  explicit AutoConnectInputBasis(const char* name = "", const char* value = "", const char* label = "", const char* pattern = "", const char* placeholder = "") : AutoConnectElementBasis(name, value), label(String(label)), pattern(String(pattern)), placeholder(String(placeholder))  {
    _type = AC_Input;
  }
  virtual ~AutoConnectInputBasis() {}
  const String  toHTML(void) const override;
  bool  isValid(void) const;

  String  label;      /**< A label for a subsequent input box */
  String  pattern;    /**< Format pattern to aid validation of input value */
  String  placeholder;  /**< Pre-filled placeholder */
};

/**
 * Radio-button arrangement class, a part of AutoConnectAux element.
 * Place a group of radio-button items and selectable mark checked.
 * @param  name     Radio-button name string.
 * @param  options  Array of value collection.
 * @param  label    A label string that follows radio-buttons group.
 * @param  checked  Index of check marked item.
 */
class AutoConnectRadioBasis : virtual public AutoConnectElementBasis {
 public:
  explicit AutoConnectRadioBasis(const char* name = "", std::vector<String> const& values = {}, const char* label = "", const ACArrange_t order = AC_Vertical, const uint8_t checked = 0) : AutoConnectElementBasis(name, ""), label(label), order(order), checked(checked), _values(values) {
    _type = AC_Radio;
  }
  virtual ~AutoConnectRadioBasis() {}
  const String  toHTML(void) const override;
  const String& operator[] (const std::size_t n) const { return at(n); }
  void  add(const String& value) { _values.push_back(String(value)); }
  size_t  size(void) const { return _values.size(); }
  const String& at(const std::size_t n) const { return _values.at(n); }
  void  check(const String& value);
  void  empty(const size_t reserve = 0);
  const String& value(void) const;

  String      label;    /**< A label for a subsequent radio buttons */
  ACArrange_t order;    /**< layout order */
  uint8_t     checked;  /**< Index of check marked item */

 protected:
  std::vector<String> _values; /**< Items in a group */
};

/**
 * Selection-box arrangement class, A part of AutoConnectAux element.
 * Place a optionally labeled Selection-box that can be added by user sketch.
 * @param  name     Input-box name string.
 * @param  options  String array display in a selection list.
 * @param  label    A label string that follows Input-box, optionally.
 * The label is placed in front of Input-box.
 */
class AutoConnectSelectBasis : virtual public AutoConnectElementBasis {
 public:
  explicit AutoConnectSelectBasis(const char* name = "", std::vector<String> const& options = {}, const char* label = "") : AutoConnectElementBasis(name, ""), label(String(label)), _options(options) {
    _type = AC_Select;
  }
  virtual ~AutoConnectSelectBasis() {}
  const String  toHTML(void) const override;
  const String& operator[] (const std::size_t n) const { return at(n); }
  void  add(const String& option) { _options.push_back(String(option)); }
  size_t  size(void) const { return _options.size(); }
  const String& at(const std::size_t n) const { return _options.at(n); }
  void  empty(const size_t reserve = 0);

  String  label;                /**< A label for a subsequent input box */

 protected:
  std::vector<String> _options; /**< List options array */
};

/**
 * Submit button arrangement class, a part of AutoConnectAux element.
 * Place a submit button with a label that can be added by user sketch.
 * With the button behavior, the values of the elements contained in
 * the form would be sent using the post method.
 * @param  name  Button name string.
 * @param  value Sending value string.
 * @param  uri   Sending uri string.
 */
class AutoConnectSubmitBasis : virtual public AutoConnectElementBasis {
 public:
  explicit AutoConnectSubmitBasis(const char* name = "", const char* value = "", const char* uri = "") : AutoConnectElementBasis(name, value), uri(String(uri)) {
    _type = AC_Submit;
  }
  virtual ~AutoConnectSubmitBasis() {}
  const String  toHTML(void) const override;

  String  uri;        /**< An url of submitting to */
};

/**
 * Text arrangement class, a part of AutoConnectAux element.
 * @param
 * @param  name     Text name string.
 * @param  value    Text value string.
 * @param  style    A string of style-code for decoration, optionally.
 * An arrangement text would be placed with <div> contains. A string
 * of style-codes are given for '<div style=>'.
 */
class AutoConnectTextBasis : virtual public AutoConnectElementBasis {
 public:
  explicit AutoConnectTextBasis(const char* name = "", const char* value = "", const char* style = "") : AutoConnectElementBasis(name, value), style(String(style)) {
    _type = AC_Text;
  }
  virtual ~AutoConnectTextBasis() {}
  const String  toHTML(void) const override;

  String  style;      /**< CSS style modifier native code */
};

#endif // _AUTOCONNECTELEMENTBASIS_H_
