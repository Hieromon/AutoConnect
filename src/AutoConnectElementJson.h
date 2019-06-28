/**
 * Declaration of AutoConnectElement extended classes using JSON.
 * @file AutoConnectElementJson.h
 * @author hieromon@gmail.com
 * @version  0.9.11
 * @date 2019-06-25
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTELEMENTJSON_H_
#define _AUTOCONNECTELEMENTJSON_H_

#include "AutoConnectElementBasis.h"
#include <ArduinoJson.h>

#define AUTOCONNECT_JSON_KEY_ACTION       "action"
#define AUTOCONNECT_JSON_KEY_ARRANGE      "arrange"
#define AUTOCONNECT_JSON_KEY_CHECKED      "checked"
#define AUTOCONNECT_JSON_KEY_ELEMENT      "element"
#define AUTOCONNECT_JSON_KEY_FORMAT       "format"
#define AUTOCONNECT_JSON_KEY_LABEL        "label"
#define AUTOCONNECT_JSON_KEY_MENU         "menu"
#define AUTOCONNECT_JSON_KEY_NAME         "name"
#define AUTOCONNECT_JSON_KEY_OPTION       "option"
#define AUTOCONNECT_JSON_KEY_PATTERN      "pattern"
#define AUTOCONNECT_JSON_KEY_PLACEHOLDER  "placeholder"
#define AUTOCONNECT_JSON_KEY_LABELFIRST   "labelfirst"
#define AUTOCONNECT_JSON_KEY_POSTERIOR    "posterior"
#define AUTOCONNECT_JSON_KEY_SELECTED     "selected"
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
#define AUTOCONNECT_JSON_TYPE_ACSELECT    "ACSelect"
#define AUTOCONNECT_JSON_TYPE_ACSUBMIT    "ACSubmit"
#define AUTOCONNECT_JSON_TYPE_ACTEXT      "ACText"
#define AUTOCONNECT_JSON_VALUE_BR         "br"
#define AUTOCONNECT_JSON_VALUE_EXTERNAL   "extern"
#define AUTOCONNECT_JSON_VALUE_FS         "fs"
#define AUTOCONNECT_JSON_VALUE_HORIZONTAL "horizontal"
#define AUTOCONNECT_JSON_VALUE_NONE       "none"
#define AUTOCONNECT_JSON_VALUE_PAR        "par"
#define AUTOCONNECT_JSON_VALUE_SD         "sd"
#define AUTOCONNECT_JSON_VALUE_VERTICAL   "vertical"

/**
 * Make the Json types and functions consistent with the ArduinoJson
 * version. These declarations share the following type definitions:
 * - Difference between reference and proxy of JsonObject and JsonArray.
 * - Difference of check whether the parsing succeeded or not.
 * - The print function name difference.
 * - The buffer class difference.
 * - When PSRAM present, enables the buffer allocation it with ESP32 and
 *   supported version.
 */
#if ARDUINOJSON_VERSION_MAJOR<=5
#define ARDUINOJSON_CREATEOBJECT(doc)     doc.createObject()
#define ARDUINOJSON_CREATEARRAY(doc)      doc.createArray()
#define ARDUINOJSON_PRETTYPRINT(doc, out) ({ size_t s = doc.prettyPrintTo(out); s; })
#define ARDUINOJSON_PRINT(doc, out)       ({ size_t s = doc.printTo(out); s; })
using ArduinoJsonObject = JsonObject&;
using ArduinoJsonArray = JsonArray&;
using ArduinoJsonBuffer = DynamicJsonBuffer;
#define AUTOCONNECT_JSONBUFFER_PRIMITIVE_SIZE AUTOCONNECT_JSONBUFFER_SIZE
#else
#define ARDUINOJSON_CREATEOBJECT(doc)     doc.to<JsonObject>()
#define ARDUINOJSON_CREATEARRAY(doc)      doc.to<JsonArray>()
#define ARDUINOJSON_PRETTYPRINT(doc, out) ({ size_t s = serializeJsonPretty(doc, out); s; })
#define ARDUINOJSON_PRINT(doc, out)       ({ size_t s = serializeJson(doc, out); s; })
using ArduinoJsonObject = JsonObject;
using ArduinoJsonArray = JsonArray;
#if defined(BOARD_HAS_PSRAM) && ((ARDUINOJSON_VERSION_MAJOR==6 && ARDUINOJSON_VERSION_MINOR>=10) || ARDUINOJSON_VERSION_MAJOR>6)
// JsonDocument is assigned to PSRAM by ArduinoJson's custom allocator.
struct SpiRamAllocatorST {
  void* allocate(size_t size) {
    uint32_t  caps;
    if (psramFound())
      caps = MALLOC_CAP_SPIRAM;
    else {
      caps = MALLOC_CAP_8BIT;
      AC_DBG("PSRAM not found, JSON buffer allocates to the heap.\n");
    } 
  }
  void  deallocate(void* pointer) {
    heap_caps_free(pointer);
  }
};
#define AUTOCONNECT_JSONBUFFER_PRIMITIVE_SIZE AUTOCONNECT_JSONPSRAM_SIZE
using ArduinoJsonBuffer = BasicJsonDocument<SpiRamAllocatorST>;
#else
#define AUTOCONNECT_JSONBUFFER_PRIMITIVE_SIZE AUTOCONNECT_JSONDOCUMENT_SIZE
using ArduinoJsonBuffer = DynamicJsonDocument;
#endif
#endif

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
  virtual void  serialize(JsonObject& json);
  template<typename T>
  T&  as(void);

 protected:
  void  _setMember(const JsonObject& json);
  void  _serialize(JsonObject& json);

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
  void  serialize(JsonObject& json) override;
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
  explicit AutoConnectCheckboxJson(const char* name = "", const char* value = "", const char* label = "", const bool checked = false, const bool labelFirst = false, const ACPosterior_t post = AC_Tag_BR) {
    AutoConnectCheckboxBasis::name = String(name);
    AutoConnectCheckboxBasis::value = String(value);
    AutoConnectCheckboxBasis::label = String(label);
    AutoConnectCheckboxBasis::checked = checked;
    AutoConnectCheckboxBasis::labelFirst = String(labelFirst);
    AutoConnectCheckboxBasis::post = post;
    _defaultPost = AC_Tag_BR;
  }
  ~AutoConnectCheckboxJson() {}
  size_t  getObjectSize(void) const override;
  bool  loadMember(const JsonObject& json) override;
  void  serialize(JsonObject& json) override;
};

/**
 * File-select input arrangement class, a part of AutoConnectAux element.
 * Place a optionally labeled file-select input box that can be added by user sketch.
 * @param  name     File-select input box name string.
 * @param  value    A string value entered by the selected file name.
 * @param  label    A label string that follows file-select box, optionally.
 * The label is placed in front of file-select box.
 * @param  store    An enumuration value of store type.
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
  void  serialize(JsonObject& json) override;
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
  explicit AutoConnectInputJson(const char* name = "", const char* value = "", const char* label = "", const char* pattern = "", const char* placeholder = "", const ACPosterior_t post = AC_Tag_BR) {
    AutoConnectInputBasis::name = String(name);
    AutoConnectInputBasis::value = String(value);
    AutoConnectInputBasis::label = String(label);
    AutoConnectInputBasis::pattern = String(pattern);
    AutoConnectInputBasis::placeholder = String(placeholder);
    AutoConnectInputBasis::post = post;
    _defaultPost = AC_Tag_BR;
  }
  ~AutoConnectInputJson() {}
  size_t  getObjectSize(void) const override;
  bool  loadMember(const JsonObject& json) override;
  void  serialize(JsonObject& json) override;
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
  void  serialize(JsonObject& json) override;
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
  void  serialize(JsonObject& json) override;
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
  void  serialize(JsonObject& json) override;
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
  void  serialize(JsonObject& json) override;
};

/**
 * Casts only a class derived from the AutoConnectElement class to the
 * actual element class.
 */
template<>
inline AutoConnectButtonJson& AutoConnectElementJson::as<AutoConnectButtonJson>(void) {
  if (typeOf() != AC_Button)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectButtonJson*>(this));
}

template<>
inline AutoConnectCheckboxJson& AutoConnectElementJson::as<AutoConnectCheckboxJson>(void) {
  if (typeOf() != AC_Checkbox)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectCheckboxJson*>(this));
}

template<>
inline AutoConnectFileJson& AutoConnectElementJson::as<AutoConnectFileJson>(void) {
  if (typeOf() != AC_File)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectFileJson*>(this));
}

template<>
inline AutoConnectInputJson& AutoConnectElementJson::as<AutoConnectInputJson>(void) {
  if (typeOf() != AC_Input)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectInputJson*>(this));
}

template<>
inline AutoConnectRadioJson& AutoConnectElementJson::as<AutoConnectRadioJson>(void) {
  if (typeOf() != AC_Radio)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectRadioJson*>(this));
}

template<>
inline AutoConnectSelectJson& AutoConnectElementJson::as<AutoConnectSelectJson>(void) {
  if (typeOf() != AC_Select)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectSelectJson*>(this));
}

template<>
inline AutoConnectSubmitJson& AutoConnectElementJson::as<AutoConnectSubmitJson>(void) {
  if (typeOf() != AC_Submit)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectSubmitJson*>(this));
}

template<>
inline AutoConnectTextJson& AutoConnectElementJson::as<AutoConnectTextJson>(void) {
  if (typeOf() != AC_Text)
    AC_DBG("%s mismatched type as <%d>\n", name.c_str(), (int)typeOf());
  return *(reinterpret_cast<AutoConnectTextJson*>(this));
}

#endif // _AUTOCONNECTELEMENTJSON_H_
