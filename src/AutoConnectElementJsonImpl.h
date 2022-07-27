/**
 * Implementation of AutoConnectElementJson classes.
 * @file AutoConnectElementJsonImpl.h
 * @author hieromon@gmail.com
 * @version  1.3.6
 * @date 2022-07-27
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTELEMENTJSONIMPL_H_
#define _AUTOCONNECTELEMENTJSONIMPL_H_

#include "AutoConnectElementJson.h"

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectElementJson::getObjectSize(void) const {
  size_t  size = JSON_OBJECT_SIZE(6);
  size += sizeof(AUTOCONNECT_JSON_KEY_NAME) + sizeof(AUTOCONNECT_JSON_KEY_TYPE) + sizeof(AUTOCONNECT_JSON_KEY_VALUE) + sizeof(AUTOCONNECT_JSON_TYPE_ACELEMENT) + sizeof(AUTOCONNECT_JSON_KEY_GLOBAL);
  size += name.length() + sizeof('\0') + value.length() + sizeof('\0');
  size += sizeof(AUTOCONNECT_JSON_KEY_POSTERIOR);
  size_t  postSize = 0;
  switch (post) {
  case AC_Tag_BR:
    postSize = sizeof(AUTOCONNECT_JSON_VALUE_BR);
    break;
  case AC_Tag_P:
    postSize = sizeof(AUTOCONNECT_JSON_VALUE_PAR);
    break;
  case AC_Tag_DIV:
    postSize = sizeof(AUTOCONNECT_JSON_VALUE_DIV);
    break;
  case AC_Tag_None:
  default:
    postSize = sizeof(AUTOCONNECT_JSON_VALUE_NONE);
  }
  size += postSize;
  return size;
}

/**
 * Load an element member value from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectElementJson::loadMember(const JsonObject& json) {
  String  type = json[F(AUTOCONNECT_JSON_KEY_TYPE)].as<String>();
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACELEMENT))) {
    _setMember(json);
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectElement to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectElementJson::serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) {
  _serialize(json);
  json[F(AUTOCONNECT_JSON_KEY_TYPE)] = String(F(AUTOCONNECT_JSON_TYPE_ACELEMENT));
  json[F(AUTOCONNECT_JSON_KEY_VALUE)] = value;
}

/**
 * Serialize AutoConnectElement to JSON.
 * This function is base for each element.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectElementJson::_serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) {
  json[F(AUTOCONNECT_JSON_KEY_NAME)] = name;
  if (post != _defaultPost) {
    PGM_P posterior;
    switch (post) {
    case AC_Tag_BR:
      posterior = PSTR(AUTOCONNECT_JSON_VALUE_BR);
      break;
    case AC_Tag_P:
      posterior = PSTR(AUTOCONNECT_JSON_VALUE_PAR);
      break;
    case AC_Tag_DIV:
      posterior = PSTR(AUTOCONNECT_JSON_VALUE_DIV);
      break;
    case AC_Tag_None:
    default:
      posterior = PSTR(AUTOCONNECT_JSON_VALUE_NONE);
      break;
    }
    json[F(AUTOCONNECT_JSON_KEY_POSTERIOR)] = String(FPSTR(posterior));
  }
  if (global)
    json[F(AUTOCONNECT_JSON_KEY_GLOBAL)] = true;
}

/**
 * Set items common to any type of AutoConnectElement from JSON objects.
 * @param  json  JSON object with the definition of AutoConnectElement.
 */
void AutoConnectElementJson::_setMember(const JsonObject& json) {
  name = json[F(AUTOCONNECT_JSON_KEY_NAME)].as<String>();
  if (json.containsKey(F(AUTOCONNECT_JSON_KEY_VALUE)))
    value = json[F(AUTOCONNECT_JSON_KEY_VALUE)].as<String>();
  if (json.containsKey(F(AUTOCONNECT_JSON_KEY_POSTERIOR))) {
    String  posterior = json[F(AUTOCONNECT_JSON_KEY_POSTERIOR)].as<String>();
    if (posterior.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_NONE)))
      post = AC_Tag_None;
    else if (posterior.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_BR)))
      post = AC_Tag_BR;
    else if (posterior.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_PAR)))
      post = AC_Tag_P;
    else if (posterior.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_DIV)))
      post = AC_Tag_DIV;
    else
      AC_DBG("Warning '%s' loading, unknown posterior '%s'\n", name.c_str(), posterior.c_str());
  }
  if (json.containsKey(F(AUTOCONNECT_JSON_KEY_GLOBAL))) {
    global = json[F(AUTOCONNECT_JSON_KEY_GLOBAL)].as<bool>();
  }
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectButtonJson::getObjectSize(void) const {
  size_t  size = AutoConnectElementJson::getObjectSize() + JSON_OBJECT_SIZE(1);
  size += sizeof(AUTOCONNECT_JSON_KEY_ACTION) + action.length() + sizeof('\0');
  return size;
}

/**
 * Load a button element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectButtonJson::loadMember(const JsonObject& json) {
  String  type = json[F(AUTOCONNECT_JSON_KEY_TYPE)].as<String>();
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACBUTTON))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_ACTION)))
      action = json[F(AUTOCONNECT_JSON_KEY_ACTION)].as<String>();
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectButton to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectButtonJson::serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) {
  _serialize(json);
  json[F(AUTOCONNECT_JSON_KEY_TYPE)] = String(F(AUTOCONNECT_JSON_TYPE_ACBUTTON));
  json[F(AUTOCONNECT_JSON_KEY_VALUE)] = value;
  json[F(AUTOCONNECT_JSON_KEY_ACTION)] = action;
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectCheckboxJson::getObjectSize(void) const {
  size_t  size = AutoConnectElementJson::getObjectSize() + JSON_OBJECT_SIZE(4);
  size += sizeof(AUTOCONNECT_JSON_KEY_LABEL) + label.length() + sizeof('\0') + sizeof(AUTOCONNECT_JSON_KEY_CHECKED) + sizeof(AUTOCONNECT_JSON_KEY_LABELPOSITION) + sizeof(AUTOCONNECT_JSON_VALUE_INFRONT);
  return size;
}

/**
 * Load a checkbox element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectCheckboxJson::loadMember(const JsonObject& json) {
  String  type = json[F(AUTOCONNECT_JSON_KEY_TYPE)].as<String>();
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACCHECKBOX))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_LABEL)))
      label = json[F(AUTOCONNECT_JSON_KEY_LABEL)].as<String>();
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_CHECKED)))
      checked = json[F(AUTOCONNECT_JSON_KEY_CHECKED)].as<bool>();
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_LABELPOSITION))) {
      String  position = json[F(AUTOCONNECT_JSON_KEY_LABELPOSITION)].as<String>();
      if (position.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_BEHIND)))
        labelPosition = AC_Behind;
      else if (position.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_INFRONT)))
        labelPosition = AC_Infront;
      else {
        AC_DBG("Failed to load %s element, unknown label position:%s\n", name.c_str(), position.c_str());
        return false;
      }
    }
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectCheckbox to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectCheckboxJson::serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) {
  _serialize(json);
  json[F(AUTOCONNECT_JSON_KEY_TYPE)] = String(F(AUTOCONNECT_JSON_TYPE_ACCHECKBOX));
  json[F(AUTOCONNECT_JSON_KEY_NAME)] = name;
  json[F(AUTOCONNECT_JSON_KEY_VALUE)] = value;
  json[F(AUTOCONNECT_JSON_KEY_LABEL)] = label;
  json[F(AUTOCONNECT_JSON_KEY_CHECKED)] = checked;
  if (labelPosition == AC_Infront)
    json[F(AUTOCONNECT_JSON_KEY_LABELPOSITION)] = AUTOCONNECT_JSON_VALUE_INFRONT;
  else if (labelPosition == AC_Behind)
    json[F(AUTOCONNECT_JSON_KEY_LABELPOSITION)] = AUTOCONNECT_JSON_VALUE_BEHIND;
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectFileJson::getObjectSize(void) const {
  size_t  size = AutoConnectElementJson::getObjectSize() + JSON_OBJECT_SIZE(2);
  size += sizeof(AUTOCONNECT_JSON_KEY_LABEL) + label.length() + sizeof('\0') + sizeof(AUTOCONNECT_JSON_KEY_STORE) + sizeof(AUTOCONNECT_JSON_VALUE_EXTERNAL);
  return size; 
}

/**
 * Load a file-select element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectFileJson::loadMember(const JsonObject& json) {
  String  type = json[F(AUTOCONNECT_JSON_KEY_TYPE)].as<String>();
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACFILE))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_LABEL)))
      label = json[F(AUTOCONNECT_JSON_KEY_LABEL)].as<String>();
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_STORE))) {
      String  media = json[F(AUTOCONNECT_JSON_KEY_STORE)].as<String>();
      if (media.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_FS)))
        store = AC_File_FS;
      else if (media.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_SD)))
        store = AC_File_SD;
      else if (media.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_EXTERNAL)))
        store = AC_File_Extern;
      else {
        AC_DBG("Failed to load %s element, unknown %s\n", name.c_str(), media.c_str());
        return false;
      }
    }
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectFile to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectFileJson::serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) {
  _serialize(json);
  json[F(AUTOCONNECT_JSON_KEY_TYPE)] = String(F(AUTOCONNECT_JSON_TYPE_ACFILE));
  json[F(AUTOCONNECT_JSON_KEY_VALUE)] = value;
  json[F(AUTOCONNECT_JSON_KEY_LABEL)] = label;
  PGM_P media;
  switch (store) {
  case AC_File_SD:
    media = PSTR(AUTOCONNECT_JSON_VALUE_SD);
    break;
  case AC_File_Extern:
    media = PSTR(AUTOCONNECT_JSON_VALUE_EXTERNAL);
    break;
  case AC_File_FS:
  default:
    media = PSTR(AUTOCONNECT_JSON_VALUE_FS);
    break;
  }
  json[F(AUTOCONNECT_JSON_KEY_STORE)] = String(FPSTR(media));
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectInputJson::getObjectSize(void) const {
  size_t  size = AutoConnectElementJson::getObjectSize() + JSON_OBJECT_SIZE(5);
  size += sizeof(AUTOCONNECT_JSON_KEY_LABEL) + label.length() + sizeof('\0') + sizeof(AUTOCONNECT_JSON_KEY_PATTERN) + pattern.length() + sizeof('\0') + sizeof(AUTOCONNECT_JSON_KEY_PLACEHOLDER) + placeholder.length() + sizeof(AUTOCONNECT_JSON_KEY_APPLY) + sizeof(AUTOCONNECT_JSON_VALUE_PASSWORD) + sizeof(AUTOCONNECT_JSON_KEY_STYLE) + style.length() + sizeof('\0');
  return size;
}

/**
 * Load a input-box element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectInputJson::loadMember(const JsonObject& json) {
  String  type = json[F(AUTOCONNECT_JSON_KEY_TYPE)].as<String>();
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACINPUT))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_LABEL)))
      label = json[F(AUTOCONNECT_JSON_KEY_LABEL)].as<String>();
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_PATTERN)))
      pattern = json[F(AUTOCONNECT_JSON_KEY_PATTERN)].as<String>();
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_PLACEHOLDER)))
      placeholder = json[F(AUTOCONNECT_JSON_KEY_PLACEHOLDER)].as<String>();
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_STYLE)))
      style = json[F(AUTOCONNECT_JSON_KEY_STYLE)].as<String>();
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_APPLY))) {
      String  applyType = json[F(AUTOCONNECT_JSON_KEY_APPLY)].as<String>();
      if (applyType.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_TEXT)))
        apply = AC_Input_Text;
      else if (applyType.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_PASSWORD)))
        apply = AC_Input_Password;
      else if (applyType.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_NUMBER)))
        apply = AC_Input_Number;
      else {
        AC_DBG("Failed to load %s element, unknown %s\n", name.c_str(), applyType.c_str());
        return false;
      }
    }
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectInput to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectInputJson::serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) {
  _serialize(json);
  json[F(AUTOCONNECT_JSON_KEY_TYPE)] = String(F(AUTOCONNECT_JSON_TYPE_ACINPUT));
  json[F(AUTOCONNECT_JSON_KEY_VALUE)] = value;
  json[F(AUTOCONNECT_JSON_KEY_LABEL)] = label;
  json[F(AUTOCONNECT_JSON_KEY_PATTERN)] = pattern;
  json[F(AUTOCONNECT_JSON_KEY_PLACEHOLDER)] = placeholder;
  json[F(AUTOCONNECT_JSON_KEY_STYLE)] = style;
  PGM_P applyType;
  switch (apply) {
  case AC_Input_Password:
    applyType = PSTR(AUTOCONNECT_JSON_VALUE_PASSWORD);
    break;
  case AC_Input_Number:
    applyType = PSTR(AUTOCONNECT_JSON_VALUE_NUMBER);
    break;
  case AC_Input_Text:
  default:
    applyType = PSTR(AUTOCONNECT_JSON_VALUE_TEXT);
    break;
  }
  json[F(AUTOCONNECT_JSON_KEY_APPLY)] = String(FPSTR(applyType));
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectRadioJson::getObjectSize(void) const {
  size_t  size = AutoConnectElementJson::getObjectSize() + JSON_OBJECT_SIZE(3) +  JSON_ARRAY_SIZE(_values.size());
  size += sizeof(AUTOCONNECT_JSON_KEY_LABEL) + label.length() + sizeof('\0') + sizeof(AUTOCONNECT_JSON_KEY_ARRANGE) + sizeof(AUTOCONNECT_JSON_VALUE_HORIZONTAL) + sizeof(AUTOCONNECT_JSON_KEY_CHECKED);
  for (const String& _value : _values)
    size += _value.length() + sizeof('\0');
  return size;
}

/**
 * Load a radio-button element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectRadioJson::loadMember(const JsonObject& json) {
  String  type = json[F(AUTOCONNECT_JSON_KEY_TYPE)].as<String>();
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACRADIO))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_LABEL)))
      label = json[F(AUTOCONNECT_JSON_KEY_LABEL)].as<String>();
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_VALUE))) {
      ArduinoJsonArray optionArray = json[AUTOCONNECT_JSON_KEY_VALUE];
      empty(optionArray.size());
      for (auto value : optionArray)
        add(value.as<String>());
    }
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_CHECKED)))
      checked = static_cast<uint8_t>(json[F(AUTOCONNECT_JSON_KEY_CHECKED)].as<int>());
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_ARRANGE))) {
      String  arrange = json[F(AUTOCONNECT_JSON_KEY_ARRANGE)].as<String>();
      if (arrange.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_VERTICAL)))
        order = AC_Vertical;
      else if (arrange.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_HORIZONTAL)))
        order = AC_Horizontal;
      else {
        AC_DBG("Failed to load %s element, unknown %s\n", name.c_str(), arrange.c_str());
        return false;
      }
    }
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectRadio to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectRadioJson::serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) {
  _serialize(json);
  json[F(AUTOCONNECT_JSON_KEY_TYPE)] = String(F(AUTOCONNECT_JSON_TYPE_ACRADIO));
  json[F(AUTOCONNECT_JSON_KEY_LABEL)] = label;
  ArduinoJsonArray  values = json.createNestedArray(F(AUTOCONNECT_JSON_KEY_VALUE));
  for (const String& v : _values)
    values.add(v);
  PGM_P direction;
  switch (order) {
  case AC_Horizontal:
    direction = PSTR(AUTOCONNECT_JSON_VALUE_HORIZONTAL);
    break;
  case AC_Vertical:
  default:
    direction = PSTR(AUTOCONNECT_JSON_VALUE_VERTICAL);
    break;
  }
  json[F(AUTOCONNECT_JSON_KEY_ARRANGE)] = String(FPSTR(direction));
  if (checked > 0)
    json[F(AUTOCONNECT_JSON_KEY_CHECKED)] = checked;
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectRangeJson::getObjectSize(void) const {
  size_t  size = AutoConnectElementJson::getObjectSize() + JSON_OBJECT_SIZE(5);
  size += sizeof(AUTOCONNECT_JSON_KEY_LABEL) + label.length() + sizeof('\0') + sizeof(AUTOCONNECT_JSON_KEY_MIN) + sizeof(AUTOCONNECT_JSON_KEY_MAX) + sizeof(AUTOCONNECT_JSON_KEY_STEP) + sizeof(AUTOCONNECT_JSON_KEY_MAGNIFY) + sizeof(AUTOCONNECT_JSON_VALUE_INFRONT) + sizeof(AUTOCONNECT_JSON_KEY_STYLE) + style.length() + sizeof('\0');
  return size;
}

/**
 * Load a input-range element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectRangeJson::loadMember(const JsonObject& json) {
  String  type = json[F(AUTOCONNECT_JSON_KEY_TYPE)].as<String>();
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACRANGE))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_LABEL)))
      label = json[F(AUTOCONNECT_JSON_KEY_LABEL)].as<String>();
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_VALUE)))
      value = json[F(AUTOCONNECT_JSON_KEY_VALUE)].as<int>();
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_MIN)))
      min = json[F(AUTOCONNECT_JSON_KEY_MIN)].as<int>();
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_MAX)))
      max = json[F(AUTOCONNECT_JSON_KEY_MAX)].as<int>();
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_STEP)))
      step = json[F(AUTOCONNECT_JSON_KEY_STEP)].as<int>();
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_MAGNIFY))) {
      String  magPosition = json[F(AUTOCONNECT_JSON_KEY_MAGNIFY)].as<String>();
      if (magPosition.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_INFRONT)))
        magnify = AC_Infront;
      else if (magPosition.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_BEHIND)))
        magnify = AC_Behind;
      else if (magPosition.equalsIgnoreCase(F(AUTOCONNECT_JSON_VALUE_VOID)))
        magnify = AC_Void;
      else {
        AC_DBG("Failed to load %s element, unknown %s\n", name.c_str(), magPosition.c_str());
        return false;
      }
    }
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_STYLE)))
      style = json[F(AUTOCONNECT_JSON_KEY_STYLE)].as<String>();
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectInput to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectRangeJson::serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) {
  _serialize(json);
  json[F(AUTOCONNECT_JSON_KEY_TYPE)] = String(F(AUTOCONNECT_JSON_TYPE_ACRANGE));
  json[F(AUTOCONNECT_JSON_KEY_VALUE)] = value;
  json[F(AUTOCONNECT_JSON_KEY_LABEL)] = label;
  json[F(AUTOCONNECT_JSON_KEY_MIN)] = min;
  json[F(AUTOCONNECT_JSON_KEY_MAX)] = max;
  json[F(AUTOCONNECT_JSON_KEY_STEP)] = step;
  PGM_P magPosition;
  switch (magnify) {
  case AC_Infront:
    magPosition = PSTR(AUTOCONNECT_JSON_VALUE_INFRONT);
    break;
  case AC_Behind:
    magPosition = PSTR(AUTOCONNECT_JSON_VALUE_BEHIND);
    break;
  case AC_Void:
  default:
    magPosition = PSTR(AUTOCONNECT_JSON_VALUE_VOID);
    break;
  }
  json[F(AUTOCONNECT_JSON_KEY_MAGNIFY)] = magPosition;
  json[F(AUTOCONNECT_JSON_KEY_STYLE)] = style;
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectSelectJson::getObjectSize(void) const {
  size_t  size = AutoConnectElementJson::getObjectSize() + JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(_options.size());
  size += sizeof(AUTOCONNECT_JSON_KEY_LABEL) + label.length() + sizeof('\0') + sizeof(AUTOCONNECT_JSON_KEY_SELECTED);
  for (const String& _option : _options)
    size += _option.length() + sizeof('\0');
  return size;
}

/**
 * Load a select element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectSelectJson::loadMember(const JsonObject& json) {
  String  type = json[F(AUTOCONNECT_JSON_KEY_TYPE)].as<String>();
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACSELECT))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_LABEL)))
      label = json[F(AUTOCONNECT_JSON_KEY_LABEL)].as<String>();
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_OPTION))) {
      ArduinoJsonArray optionArray = json[F(AUTOCONNECT_JSON_KEY_OPTION)];
      empty(optionArray.size());
      for (auto value : optionArray)
        add(value.as<String>());
    }
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_SELECTED)))
      selected = static_cast<uint8_t>(json[F(AUTOCONNECT_JSON_KEY_SELECTED)].as<int>());
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectSelect to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectSelectJson::serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) {
  _serialize(json);
  json[F(AUTOCONNECT_JSON_KEY_TYPE)] = String(F(AUTOCONNECT_JSON_TYPE_ACSELECT));
  ArduinoJsonArray options = json.createNestedArray(F(AUTOCONNECT_JSON_KEY_OPTION));
  for (String o : _options)
    options.add(o);
  json[F(AUTOCONNECT_JSON_KEY_LABEL)] = label;
  if (selected > 0)
    json[F(AUTOCONNECT_JSON_KEY_SELECTED)] = selected;
}

/**
 * Load an element member value from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectStyle.
 * @return true  AutoConnectStyle loaded
 * @return false Type of AutoConnectStyle is mismatched.
 */
bool AutoConnectStyleJson::loadMember(const JsonObject& json) {
  String  type = json[F(AUTOCONNECT_JSON_KEY_TYPE)].as<String>();
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACSTYLE))) {
    _setMember(json);
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectStyle to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectStyleJson::serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) {
  _serialize(json);
  json[F(AUTOCONNECT_JSON_KEY_TYPE)] = String(F(AUTOCONNECT_JSON_TYPE_ACSTYLE));
  json[F(AUTOCONNECT_JSON_KEY_VALUE)] = value;
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectSubmitJson::getObjectSize(void) const {
  size_t  size = AutoConnectElementJson::getObjectSize();
  size += sizeof(AUTOCONNECT_JSON_KEY_URI) + uri.length() + 1;
  return size;
}

/**
 * Load a submit element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectSubmitJson::loadMember(const JsonObject& json) {
  String  type = json[F(AUTOCONNECT_JSON_KEY_TYPE)].as<String>();
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACSUBMIT))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_URI)))
      uri = json[F(AUTOCONNECT_JSON_KEY_URI)].as<String>();
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectSubmit to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectSubmitJson::serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) {
  _serialize(json);
  json[F(AUTOCONNECT_JSON_KEY_TYPE)] = String(F(AUTOCONNECT_JSON_TYPE_ACSUBMIT));
  json[F(AUTOCONNECT_JSON_KEY_VALUE)] = value;
  json[F(AUTOCONNECT_JSON_KEY_URI)] = uri;
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectTextJson::getObjectSize(void) const {
  size_t  size = AutoConnectElementJson::getObjectSize() + JSON_OBJECT_SIZE(2);
  size += sizeof(AUTOCONNECT_JSON_KEY_STYLE) + style.length() + sizeof('\0') + sizeof(AUTOCONNECT_JSON_KEY_FORMAT) + format.length() + sizeof('\0');
  return size;
}

/**
 * Load a text element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectTextJson::loadMember(const JsonObject& json) {
  String  type = json[F(AUTOCONNECT_JSON_KEY_TYPE)].as<String>();
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACTEXT))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_STYLE)))
      style = json[F(AUTOCONNECT_JSON_KEY_STYLE)].as<String>();
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_FORMAT)))
      format = json[F(AUTOCONNECT_JSON_KEY_FORMAT)].as<String>();
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectText to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectTextJson::serialize(ARDUINOJSON_OBJECT_REFMODIFY JsonObject& json) {
  _serialize(json);
  json[F(AUTOCONNECT_JSON_KEY_TYPE)] = String(F(AUTOCONNECT_JSON_TYPE_ACTEXT));
  json[F(AUTOCONNECT_JSON_KEY_VALUE)] = value;
  json[F(AUTOCONNECT_JSON_KEY_STYLE)] = style;
  json[F(AUTOCONNECT_JSON_KEY_FORMAT)] = format;
}

#endif // _AUTOCONNECTELEMENTJSONIMPL_H_
