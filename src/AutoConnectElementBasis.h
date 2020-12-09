/**
 * Declaration of AutoConnectElement basic class.
 * @file AutoConnectElementBasis.h
 * @author hieromon@gmail.com
 * @version  1.2.0
 * @date 2020-11-11
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTELEMENTBASIS_H_
#define _AUTOCONNECTELEMENTBASIS_H_

#include <vector>
#include <memory>
#include "AutoConnectUpload.h"

// AC_AUTOCONNECTELEMENT_ON_VIRTUAL macro absorbs the difference of
// inheritance attribute of AutoConnectElement depending on the use of JSON.
// In a configuration using JSON, the base class of each
// AutoConnectElement is a virtual inheritance.
#ifdef AUTOCONNECT_USE_JSON
#define AC_AUTOCONNECTELEMENT_ON_VIRTUAL virtual
#else
#define AC_AUTOCONNECTELEMENT_ON_VIRTUAL
#endif

// A set of enumerators of types and attributes subordinate to
// AutoConnectElements

typedef enum {
  AC_Button,
  AC_Checkbox,
  AC_Element,
  AC_File,
  AC_Input,
  AC_Radio,
  AC_Select,
  AC_Style,
  AC_Submit,
  AC_Text,
  AC_Unknown = -1
} ACElement_t;      /**< AutoConnectElement class type */

typedef enum {
  AC_Horizontal,
  AC_Vertical
} ACArrange_t;      /**< The element arrange order */

typedef enum {
  AC_File_FS = 0,
  AC_File_SD,
  AC_File_Extern
} ACFile_t;         /**< AutoConnectFile media type */

typedef enum {
  AC_Tag_None = 0,
  AC_Tag_BR = 1,
  AC_Tag_P = 2
} ACPosterior_t;    /**< Tag to be generated following element */

typedef enum {
  AC_Infront,
  AC_Behind
} ACPosition_t;     /**< Position of label subordinate to element */

typedef enum {
  AC_Input_Text,
  AC_Input_Password,
  AC_Input_Number
} ACInput_t;        /** Input box type attribute */

/**
 * AutoConnectAux element base.
 * Placed a raw text that can be added by user sketch.
 * @param  name     A name string for the element.
 * @param  value    A raw text to be placed in HTML.
 */
class AutoConnectElementBasis {
 public:
  explicit AutoConnectElementBasis(const char* name = "", const char* value = "", const ACPosterior_t post = AC_Tag_None) : name(String(name)), value(String(value)), post(post), enable(true), global(false) {
    _type = AC_Element;
  }
  virtual ~AutoConnectElementBasis() {}
  virtual const String  toHTML(void) const { return enable ? posterior(value) : String(""); }
  ACElement_t typeOf(void) const { return _type; }
  const String  posterior(const String& s) const;
#ifndef AUTOCONNECT_USE_JSON
  template<typename T>
  T&  as(void);
#endif

  String  name;       /**< Element name */
  String  value;      /**< Element value */
  ACPosterior_t post; /**< Tag to be generated with posterior */
  bool    enable;     /**< Enabling the element */
  bool    global;     /**< The value available in global scope */

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
class AutoConnectButtonBasis : AC_AUTOCONNECTELEMENT_ON_VIRTUAL public AutoConnectElementBasis {
 public:
  explicit AutoConnectButtonBasis(const char* name = "", const char* value = "", const String& action = String(""), const ACPosterior_t post = AC_Tag_None) : AutoConnectElementBasis(name, value, post), action(String(action)) {
    _type = AC_Button;
  }
  virtual ~AutoConnectButtonBasis() {}
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
class AutoConnectCheckboxBasis : AC_AUTOCONNECTELEMENT_ON_VIRTUAL public AutoConnectElementBasis {
 public:
  explicit AutoConnectCheckboxBasis(const char* name = "", const char* value = "", const char* label = "", const bool checked = false, const ACPosition_t labelPosition = AC_Behind, const ACPosterior_t post = AC_Tag_BR) : AutoConnectElementBasis(name, value, post), label(String(label)), checked(checked), labelPosition(labelPosition) {
    _type = AC_Checkbox;
  }
  virtual ~AutoConnectCheckboxBasis() {}
  const String  toHTML(void) const override;

  String  label;      /**< A label for a subsequent input box */
  bool    checked;    /**< The element should be pre-selected */
  ACPosition_t  labelPosition;  /**< Output label according to ACPosition_t */
};

/**
 * File-select input arrangement class, a part of AutoConnectAux element.
 * Place a optionally labeled file-select input box that can be added by user sketch.
 * @param  name     File-select input box name string.
 * @param  value    A string value entered by the selected file name.
 * @param  label    A label string that follows file-select box, optionally.
 * The label is placed in front of file-select box.
 */
class AutoConnectFileBasis : AC_AUTOCONNECTELEMENT_ON_VIRTUAL public AutoConnectElementBasis {
 public:
  explicit AutoConnectFileBasis(const char* name = "", const char* value = "", const char* label = "", const ACFile_t store = AC_File_FS, const ACPosterior_t post = AC_Tag_BR) : AutoConnectElementBasis(name, value, post), label(String(label)), store(store), size(0) {
    _type = AC_File;
    _upload.reset();
  }
  virtual ~AutoConnectFileBasis() {}
  const String  toHTML(void) const override;
  bool  attach(const ACFile_t store);
  void  detach(void) { _upload.reset(); }
  AutoConnectUploadHandler*  upload(void) const { return _upload.get(); }

  String   label;     /**< A label for a subsequent input box */
  ACFile_t store;     /**< Type of file store */
  String   mimeType;  /**< Uploading file mime type string */
  size_t   size;      /**< Total uploaded bytes */

 protected:
  std::unique_ptr<AutoConnectUploadHandler> _upload;
};

/**
 * Input-box arrangement class, a part of AutoConnectAux element.
 * Place a optionally labeled input-box that can be added by user sketch.
 * @param  name     Input-box name string.
 * @param  value    Default value string. This string display as a placeholder by the default.
 * @param  label    A label string that follows Input-box, optionally.
 * The label is placed in front of Input-box.
 */
class AutoConnectInputBasis : AC_AUTOCONNECTELEMENT_ON_VIRTUAL public AutoConnectElementBasis {
 public:
  explicit AutoConnectInputBasis(const char* name = "", const char* value = "", const char* label = "", const char* pattern = "", const char* placeholder = "", const ACPosterior_t post = AC_Tag_BR, const ACInput_t apply = AC_Input_Text) : AutoConnectElementBasis(name, value, post), label(String(label)), pattern(String(pattern)), placeholder(String(placeholder)), apply(apply) {
    _type = AC_Input;
  }
  virtual ~AutoConnectInputBasis() {}
  const String  toHTML(void) const override;
  bool  isValid(void) const;

  String  label;      /**< A label for a subsequent input box */
  String  pattern;    /**< Format pattern to aid validation of input value */
  String  placeholder;  /**< Pre-filled placeholder */
  ACInput_t apply;    /**< An input element type attribute */
};

/**
 * Radio-button arrangement class, a part of AutoConnectAux element.
 * Place a group of radio-button items and selectable mark checked.
 * @param  name     Radio-button name string.
 * @param  options  Array of value collection.
 * @param  label    A label string that follows radio-buttons group.
 * @param  checked  Index of check marked item.
 */
class AutoConnectRadioBasis : AC_AUTOCONNECTELEMENT_ON_VIRTUAL public AutoConnectElementBasis {
 public:
  explicit AutoConnectRadioBasis(const char* name = "", std::vector<String> const& values = {}, const char* label = "", const ACArrange_t order = AC_Vertical, const uint8_t checked = 0, const ACPosterior_t post = AC_Tag_BR) : AutoConnectElementBasis(name, "", post), label(label), order(order), checked(checked), _values(values) {
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
  std::vector<String> tags; /**< For private API: Tag of each value */

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
class AutoConnectSelectBasis : AC_AUTOCONNECTELEMENT_ON_VIRTUAL public AutoConnectElementBasis {
 public:
  explicit AutoConnectSelectBasis(const char* name = "", std::vector<String> const& options = {}, const char* label = "", const uint8_t selected = 0, const ACPosterior_t post = AC_Tag_BR) : AutoConnectElementBasis(name, "", post), label(String(label)), selected(selected), _options(options) {
    _type = AC_Select;
  }
  virtual ~AutoConnectSelectBasis() {}
  const String  toHTML(void) const override;
  const String& operator[] (const std::size_t n) const { return at(n); }
  void  add(const String& option) { _options.push_back(String(option)); }
  size_t  size(void) const { return _options.size(); }
  const String& at(const std::size_t n) const { return _options.at(n); }
  void  select(const String& value);
  void  empty(const size_t reserve = 0);
  const String& value(void) const;

  String  label;                /**< A label for a subsequent input box */
  uint8_t selected;             /**< Index of checked value (1-based) */

 protected:
  std::vector<String> _options; /**< List options array */
};

/**
 * An element class for inserting CSS in AutoConnectAux page.
 * @param  name  Style name string.
 * @param  value CSS Native code.
 */
class AutoConnectStyleBasis : AC_AUTOCONNECTELEMENT_ON_VIRTUAL public AutoConnectElementBasis {
 public:
  explicit AutoConnectStyleBasis(const char* name = "", const char* value = "") : AutoConnectElementBasis(name, value, AC_Tag_None) {
    _type = AC_Style;
  }
  virtual ~AutoConnectStyleBasis() {}
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
class AutoConnectSubmitBasis : AC_AUTOCONNECTELEMENT_ON_VIRTUAL public AutoConnectElementBasis {
 public:
  explicit AutoConnectSubmitBasis(const char* name = "", const char* value = "", const char* uri = "", const ACPosterior_t post = AC_Tag_None) : AutoConnectElementBasis(name, value, post), uri(String(uri)) {
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
 * @param  format   C string that contains the value to be formatted.
 * An arrangement text would be placed with <div> contains. A string
 * of style-codes are given for '<div style=>'.
 */
class AutoConnectTextBasis : AC_AUTOCONNECTELEMENT_ON_VIRTUAL public AutoConnectElementBasis {
 public:
  explicit AutoConnectTextBasis(const char* name = "", const char* value = "", const char* style = "", const char* format = "", const ACPosterior_t post = AC_Tag_None) : AutoConnectElementBasis(name, value, post), style(String(style)), format(String(format)) {
    _type = AC_Text;
  }
  virtual ~AutoConnectTextBasis() {}
  const String  toHTML(void) const override;

  String  style;      /**< CSS style modifier native code */
  String  format;     /**< C string that contains the text to be written */
};

#ifndef AUTOCONNECT_USE_JSON
/**
 * Casts only a class derived from the AutoConnectElement class to the
 * actual element class.
 */
template<>
inline AutoConnectButtonBasis& AutoConnectElementBasis::as<AutoConnectButtonBasis>(void) {
  if (typeOf() != AC_Button)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectButtonBasis*>(this));
}

template<>
inline AutoConnectCheckboxBasis& AutoConnectElementBasis::as<AutoConnectCheckboxBasis>(void) {
  if (typeOf() != AC_Checkbox)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectCheckboxBasis*>(this));
}

template<>
inline AutoConnectFileBasis& AutoConnectElementBasis::as<AutoConnectFileBasis>(void) {
  if (typeOf() != AC_File)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectFileBasis*>(this));
}

template<>
inline AutoConnectInputBasis& AutoConnectElementBasis::as<AutoConnectInputBasis>(void) {
  if (typeOf() != AC_Input)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectInputBasis*>(this));
}

template<>
inline AutoConnectRadioBasis& AutoConnectElementBasis::as<AutoConnectRadioBasis>(void) {
  if (typeOf() != AC_Radio)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectRadioBasis*>(this));
}

template<>
inline AutoConnectSelectBasis& AutoConnectElementBasis::as<AutoConnectSelectBasis>(void) {
  if (typeOf() != AC_Select)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectSelectBasis*>(this));
}

template<>
inline AutoConnectStyleBasis& AutoConnectElementBasis::as<AutoConnectStyleBasis>(void) {
  if (typeOf() != AC_Style)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectStyleBasis*>(this));
}

template<>
inline AutoConnectSubmitBasis& AutoConnectElementBasis::as<AutoConnectSubmitBasis>(void) {
  if (typeOf() != AC_Submit)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectSubmitBasis*>(this));
}

template<>
inline AutoConnectTextBasis& AutoConnectElementBasis::as<AutoConnectTextBasis>(void) {
  if (typeOf() != AC_Text)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectTextBasis*>(this));
}
#endif

#endif // _AUTOCONNECTELEMENTBASIS_H_
