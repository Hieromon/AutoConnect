/**
 * Declaration of AutoConnectElement extended classes using JSON.
 * @file AutoConnectElementJson.h
 * @author hieromon@gmail.com
 * @version  1.3.2
 * @date 2021-11-24
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTELEMENTJSON_H_
#define _AUTOCONNECTELEMENTJSON_H_

#include "AutoConnectElementBasis.h"
#include "AutoConnectJsonDefs.h"

#define AUTOCONNECT_JSON_KEY_ACTION       "action"
#define AUTOCONNECT_JSON_KEY_APPLY        "apply"
#define AUTOCONNECT_JSON_KEY_ARRANGE      "arrange"
#define AUTOCONNECT_JSON_KEY_AUTH         "auth"
#define AUTOCONNECT_JSON_KEY_CHECKED      "checked"
#define AUTOCONNECT_JSON_KEY_ELEMENT      "element"
#define AUTOCONNECT_JSON_KEY_FORMAT       "format"
#define AUTOCONNECT_JSON_KEY_GLOBAL       "global"
#define AUTOCONNECT_JSON_KEY_LABEL        "label"
#define AUTOCONNECT_JSON_KEY_LABELPOSITION "labelposition"
#define AUTOCONNECT_JSON_KEY_MAGNIFY      "magnify"
#define AUTOCONNECT_JSON_KEY_MAX          "max"
#define AUTOCONNECT_JSON_KEY_MIN          "min"
#define AUTOCONNECT_JSON_KEY_MENU         "menu"
#define AUTOCONNECT_JSON_KEY_NAME         "name"
#define AUTOCONNECT_JSON_KEY_OPTION       "option"
#define AUTOCONNECT_JSON_KEY_PATTERN      "pattern"
#define AUTOCONNECT_JSON_KEY_PLACEHOLDER  "placeholder"
#define AUTOCONNECT_JSON_KEY_POSTERIOR    "posterior"
#define AUTOCONNECT_JSON_KEY_RESPONSE     "response"
#define AUTOCONNECT_JSON_KEY_SELECTED     "selected"
#define AUTOCONNECT_JSON_KEY_STEP         "step"
#define AUTOCONNECT_JSON_KEY_STORE        "store"
#define AUTOCONNECT_JSON_KEY_STYLE        "style"
#define AUTOCONNECT_JSON_KEY_TITLE        "title"
#define AUTOCONNECT_JSON_KEY_TYPE         "type"
#define AUTOCONNECT_JSON_KEY_URI          "uri"
#define AUTOCONNECT_JSON_KEY_VALUE        "value"
#define AUTOCONNECT_JSON_TYPE_ACBUTTON    "ACButton"
#define AUTOCONNECT_JSON_TYPE_ACCHECKBOX  "ACCheckBox"
#define AUTOCONNECT_JSON_TYPE_ACELEMENT   "ACElement"
#define AUTOCONNECT_JSON_TYPE_ACFILE      "ACFile"
#define AUTOCONNECT_JSON_TYPE_ACINPUT     "ACInput"
#define AUTOCONNECT_JSON_TYPE_ACRADIO     "ACRadio"
#define AUTOCONNECT_JSON_TYPE_ACRANGE     "ACRange"
#define AUTOCONNECT_JSON_TYPE_ACSELECT    "ACSelect"
#define AUTOCONNECT_JSON_TYPE_ACSTYLE     "ACStyle"
#define AUTOCONNECT_JSON_TYPE_ACSUBMIT    "ACSubmit"
#define AUTOCONNECT_JSON_TYPE_ACTEXT      "ACText"
#define AUTOCONNECT_JSON_VALUE_BASIC      "basic"
#define AUTOCONNECT_JSON_VALUE_BEHIND     "behind"
#define AUTOCONNECT_JSON_VALUE_BR         "br"
#define AUTOCONNECT_JSON_VALUE_DIGEST     "digest"
#define AUTOCONNECT_JSON_VALUE_DIV        "div"
#define AUTOCONNECT_JSON_VALUE_EXTERNAL   "extern"
#define AUTOCONNECT_JSON_VALUE_FS         "fs"
#define AUTOCONNECT_JSON_VALUE_HORIZONTAL "horizontal"
#define AUTOCONNECT_JSON_VALUE_INFRONT    "infront"
#define AUTOCONNECT_JSON_VALUE_NONE       "none"
#define AUTOCONNECT_JSON_VALUE_NUMBER     "number"
#define AUTOCONNECT_JSON_VALUE_PAR        "par"
#define AUTOCONNECT_JSON_VALUE_PASSWORD   "password"
#define AUTOCONNECT_JSON_VALUE_TEXT       "text"
#define AUTOCONNECT_JSON_VALUE_SD         "sd"
#define AUTOCONNECT_JSON_VALUE_VERTICAL   "vertical"
#define AUTOCONNECT_JSON_VALUE_VOID       "void"

/**
 * AutoConnectAux element base with handling with JSON object.
 * Placed a raw text that can be added by user sketch.
 * @param  name     A name string for the element.
 * @param  value    A raw text to be placed in HTML.
 */
class AutoConnectElementJson : virtual public AutoConnectElementBasis {
 public:
  explicit AutoConnectElementJson(const char* name = "", const char* value = "", const ACPosterior_t post = AC_Tag_None) : _defaultPost(AC_Tag_None) {
    AutoConnectElementBasis::name = String(name);
    AutoConnectElementBasis::value = String(value);
    AutoConnectElementBasis::post = post;
  }
  ~AutoConnectElementJson() {}
  virtual size_t  getObjectSize(void) const;
  virtual bool  loadMember(const JsonObject& json);
  virtual void  serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json);
  template<typename T>
  T&  as(void);

 protected:
  void  _setMember(const JsonObject& json);
  void  _serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json);

 protected:
  ACPosterior_t   _defaultPost;
};

/**
 * Button arrangement class, a part of AutoConnectAux element with
 * handling JSON object.
 * Place a labeled button that can be added by user sketch.
 * @param  name     Button element name string.
 * @param  value    Value string with the placed button.
 * @param  action   Script code to execute with the button pushed.
 */
class AutoConnectButtonJson : public AutoConnectElementJson, public AutoConnectButtonBasis {
 public:
  explicit AutoConnectButtonJson(const char* name = "", const char* value = "", const String& action = String(""), const ACPosterior_t post = AC_Tag_None) {
    AutoConnectButtonBasis::name = String(name);
    AutoConnectButtonBasis::value = String(value);
    AutoConnectButtonBasis::action = String(action);
    AutoConnectButtonBasis::post = post;
    _defaultPost = AC_Tag_None;
  }
  ~AutoConnectButtonJson() {}
  size_t  getObjectSize(void) const override;
  bool  loadMember(const JsonObject& json) override;
  void  serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) override;
};

/**
 * Checkbox arrangement class, a part of AutoConnectAux element with
 * handling JSON object.
 * Place a optionally labeled input-box that can be added by user sketch.
 * @param  name     Checkbox name string.
 * @param  value    A string value associated with the input.
 * @param  label    A label string that follows checkbox, optionally.
 * The label is placed on the right side of the checkbox.
 */
class AutoConnectCheckboxJson : public AutoConnectElementJson, public AutoConnectCheckboxBasis {
 public:
  explicit AutoConnectCheckboxJson(const char* name = "", const char* value = "", const char* label = "", const bool checked = false, const ACPosition_t labelPosition = AC_Behind, const ACPosterior_t post = AC_Tag_BR) {
    AutoConnectCheckboxBasis::name = String(name);
    AutoConnectCheckboxBasis::value = String(value);
    AutoConnectCheckboxBasis::label = String(label);
    AutoConnectCheckboxBasis::checked = checked;
    AutoConnectCheckboxBasis::labelPosition = labelPosition;
    AutoConnectCheckboxBasis::post = post;
    _defaultPost = AC_Tag_BR;
  }
  ~AutoConnectCheckboxJson() {}
  size_t  getObjectSize(void) const override;
  bool  loadMember(const JsonObject& json) override;
  void  serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) override;
};

/**
 * File-select input arrangement class, a part of AutoConnectAux element.
 * Place a optionally labeled file-select input box that can be added by user sketch.
 * @param  name     File-select input box name string.
 * @param  value    A string value entered by the selected file name.
 * @param  label    A label string that follows file-select box, optionally.
 * The label is placed in front of file-select box.
 * @param  store    An enumeration value of store type.
 */
class AutoConnectFileJson : public AutoConnectElementJson, public AutoConnectFileBasis {
 public:
  explicit AutoConnectFileJson(const char* name = "", const char* value= "", const char* label = "", const ACFile_t store = AC_File_FS, const ACPosterior_t post = AC_Tag_BR) {
    AutoConnectFileBasis::name = String(name);
    AutoConnectFileBasis::value = String(value);
    AutoConnectFileBasis::label = String(label);
    AutoConnectFileBasis::store = store;
    AutoConnectFileBasis::post = post;
    _defaultPost = AC_Tag_BR;
  }
  ~AutoConnectFileJson() {}
  size_t  getObjectSize(void) const override;
  bool  loadMember(const JsonObject& json) override;
  void  serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) override;
};

/**
 * Input-box arrangement class, a part of AutoConnectAux element with
 * handling JSON object.
 * Place a optionally labeled input-box that can be added by user sketch.
 * @param  name     Input-box name string.
 * @param  value    Default value string. This string display as a placeholder by the default.
 * @param  label    A label string that follows Input-box, optionally.
 * The label is placed in front of Input-box.
 */
class AutoConnectInputJson : public AutoConnectElementJson, public AutoConnectInputBasis {
 public:
  explicit AutoConnectInputJson(const char* name = "", const char* value = "", const char* label = "", const char* pattern = "", const char* placeholder = "", const ACPosterior_t post = AC_Tag_BR, const ACInput_t apply = AC_Input_Text, const char* style = "") {
    AutoConnectInputBasis::name = String(name);
    AutoConnectInputBasis::value = String(value);
    AutoConnectInputBasis::label = String(label);
    AutoConnectInputBasis::pattern = String(pattern);
    AutoConnectInputBasis::placeholder = String(placeholder);
    AutoConnectInputBasis::style = String(style);
    AutoConnectInputBasis::apply = apply;
    AutoConnectInputBasis::post = post;
    _defaultPost = AC_Tag_BR;
  }
  ~AutoConnectInputJson() {}
  size_t  getObjectSize(void) const override;
  bool  loadMember(const JsonObject& json) override;
  void  serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) override;
};

/**
* Radio-button arrangement class, a part of AutoConnectAux element.
* Place a group of radio-button items and selectable mark checked.
* @param  name     Radio-button name string.
* @param  options  Array of value collection.
* @param  label    A label string that follows radio-buttons group.
* @param  checked  Index of check marked item.
*/
class AutoConnectRadioJson : public AutoConnectElementJson, public AutoConnectRadioBasis {
 public:
  explicit AutoConnectRadioJson(const char* name = "", std::vector<String> const& values = {}, const char* label = "", const ACArrange_t order = AC_Vertical, const uint8_t checked = 0, const ACPosterior_t post = AC_Tag_BR) {
    AutoConnectRadioBasis::name = String(name);
    AutoConnectRadioBasis::_values = values;
    AutoConnectRadioBasis::label = String(label);
    AutoConnectRadioBasis::order = order;
    AutoConnectRadioBasis::checked = checked;
    AutoConnectRadioBasis::post = post;
    _defaultPost = AC_Tag_BR;
  }
  ~AutoConnectRadioJson() {}
  size_t  getObjectSize(void) const override;
  bool  loadMember(const JsonObject& json) override;
  void  serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) override;
};

/**
 * Range-value arrangement class, a part of AutoConnectAux element.
 * Place an optionally labeled slider-like control that can be added by user sketch.
 * @param  name     Range-slider name string.
 * @param  value    Default value.
 * @param  label    A label string that follows range-slider control.
 * @param  min      Minimum value possible range.
 * @param  max      Maximum possible range.
 * @param  step     Incremental values that are valid.
 * @param  magnify  Place a value display field in front of the slider.
 * @param  style    A string of style-code for decoration, optionally.
 */
class AutoConnectRangeJson : public AutoConnectElementJson, public AutoConnectRangeBasis {
 public:
  explicit AutoConnectRangeJson(const char* name = "", const int value = 0, const char* label = "", const int min = 0, const int max = 0, const int step = 1, const ACPosition_t magnify = AC_Void, const ACPosterior_t post = AC_Tag_BR, const char* style = "") {
    AutoConnectRangeBasis::name = String(name);
    AutoConnectRangeBasis::value = value;
    AutoConnectRangeBasis::label = String(label);
    AutoConnectRangeBasis::min = min;
    AutoConnectRangeBasis::max = max;
    AutoConnectRangeBasis::step = step;
    AutoConnectRangeBasis::magnify = magnify;
    AutoConnectRangeBasis::style = String(style);
    AutoConnectRangeBasis::post = post;
    _defaultPost = AC_Tag_BR;
  }
  ~AutoConnectRangeJson() {}
  size_t  getObjectSize(void) const override;
  bool  loadMember(const JsonObject& json) override;
  void  serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) override;
};

/**
 * Selection-box arrangement class, A part of AutoConnectAux element.
 * Place a optionally labeled Selection-box that can be added by user sketch.
 * @param  name     Input-box name string.
 * @param  options  String array display in a selection list.
 * @param  label    A label string that follows Input-box, optionally.
 * The label is placed in front of Input-box.
 */
class AutoConnectSelectJson : public AutoConnectElementJson, public AutoConnectSelectBasis {
 public:
  explicit AutoConnectSelectJson(const char* name = "", std::vector<String> const& options = {}, const char* label = "", const uint8_t selected = 0, const ACPosterior_t post = AC_Tag_BR) {
    AutoConnectSelectBasis::name = String(name);
    AutoConnectSelectBasis::_options = options;
    AutoConnectSelectBasis::label = String(label);
    AutoConnectSelectBasis::selected = selected;
    AutoConnectSelectBasis::post = post;
    _defaultPost = AC_Tag_BR;
  }
  ~AutoConnectSelectJson() {}
  size_t  getObjectSize(void) const override;
  bool  loadMember(const JsonObject& json) override;
  void  serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) override;
};

/**
 * CSS style arrangement class, a part of AutoConnectAux element.
 * This element assumes CSS that came into effect as a style code will
 * assign. Therefore, it does not check whether the CSS error exists in
 * the value set in AutoConnectStyle. Also, because AutoConnect inserts
 * its style code at the end of the style block on the AutoConnectAux
 * page, it may affect the AutoConnect web page elements.
 * @param  name  A style name string.
 * @param  value CSS style code.
 */
class AutoConnectStyleJson : public AutoConnectElementJson, public AutoConnectStyleBasis {
 public:
  explicit AutoConnectStyleJson(const char* name = "", const char* value = "") {
    AutoConnectStyleBasis::name = String(name);
    AutoConnectStyleBasis::value = String(value);
    AutoConnectStyleBasis::post = AC_Tag_None;
  }
  ~AutoConnectStyleJson() {}
  bool  loadMember(const JsonObject& json) override;
  void  serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) override;
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
class AutoConnectSubmitJson : public AutoConnectElementJson, public AutoConnectSubmitBasis {
 public:
  explicit AutoConnectSubmitJson(const char* name = "", const char* value = "", const char* uri = "", const ACPosterior_t post = AC_Tag_None) {
    AutoConnectSubmitBasis::name = String(name);
    AutoConnectSubmitBasis::value = String(value);
    AutoConnectSubmitBasis::uri = String(uri);
    AutoConnectSubmitBasis::post = post;
    _defaultPost = AC_Tag_None;
  }
  ~AutoConnectSubmitJson() {}
  size_t  getObjectSize(void) const override;
  bool  loadMember(const JsonObject& json) override;
  void  serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) override;
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
class AutoConnectTextJson : public AutoConnectElementJson, public AutoConnectTextBasis {
 public:
  explicit AutoConnectTextJson(const char* name = "", const char* value = "", const char* style = "", const char* format = "", const ACPosterior_t post = AC_Tag_None) {
    AutoConnectTextBasis::name = String(name);
    AutoConnectTextBasis::value = String(value);
    AutoConnectTextBasis::style = String(style);
    AutoConnectTextBasis::format = String(format);
    AutoConnectTextBasis::post = post;
    _defaultPost = AC_Tag_None;
  }
  ~AutoConnectTextJson() {}
  size_t  getObjectSize(void) const override;
  bool  loadMember(const JsonObject& json) override;
  void  serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) override;
};

/**
 * Casts only a class derived from the AutoConnectElement class to the
 * actual element class.
 */
template<>
inline bool AutoConnectElementBasis::_isCompatible<AutoConnectButtonJson>(void) {
  return (_type == AC_Button);
}

template<>
inline bool AutoConnectElementBasis::_isCompatible<AutoConnectCheckboxJson>(void) {
  return (_type == AC_Checkbox);
}

template<>
inline bool AutoConnectElementBasis::_isCompatible<AutoConnectFileJson>(void) {
  return (_type == AC_File);
}

template<>
inline bool AutoConnectElementBasis::_isCompatible<AutoConnectInputJson>(void) {
  return (_type == AC_Input);
}

template<>
inline bool AutoConnectElementBasis::_isCompatible<AutoConnectRadioJson>(void) {
  return (_type == AC_Radio);
}

template<>
inline bool AutoConnectElementBasis::_isCompatible<AutoConnectRangeJson>(void) {
  return (_type == AC_Range);
}

template<>
inline bool AutoConnectElementBasis::_isCompatible<AutoConnectSelectJson>(void) {
  return (_type == AC_Select);
}

template<>
inline bool AutoConnectElementBasis::_isCompatible<AutoConnectStyleJson>(void) {
  return (_type == AC_Style);
}

template<>
inline bool AutoConnectElementBasis::_isCompatible<AutoConnectSubmitJson>(void) {
  return (_type == AC_Submit);
}

template<>
inline bool AutoConnectElementBasis::_isCompatible<AutoConnectTextJson>(void) {
  return (_type == AC_Text);
}

template<typename T>
inline T& AutoConnectElementJson::as(void) {
  if (!AutoConnectElementBasis::_isCompatible<T>())
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<T*>(this));
}

#endif // _AUTOCONNECTELEMENTJSON_H_
