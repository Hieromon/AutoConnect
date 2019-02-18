/**
 * Implementation of AutoConnectElementJson classes.
 * @file AutoConnectElementImpl.h
 * @author hieromon@gmail.com
 * @version  0.9.7
 * @date 2018-11-17
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTELEMENTJSONIMPL_H_
#define _AUTOCONNECTELEMENTJSONIMPL_H_

#include "AutoConnectElementJson.h"

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectElementJson::getObjectSize() const {
  return JSON_OBJECT_SIZE(3);
}

/**
 * Load an element member value from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectElementJson::loadMember(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
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
void AutoConnectElementJson::serialize(JsonObject& json) {
  _serialize(json);
  json.set(F(AUTOCONNECT_JSON_KEY_TYPE), F(AUTOCONNECT_JSON_TYPE_ACELEMENT));
  json.set(F(AUTOCONNECT_JSON_KEY_VALUE), value);
}

/**
 * Serialize AutoConnectElement to JSON.
 * This function is base for each element.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectElementJson::_serialize(JsonObject& json) {
  json.set(F(AUTOCONNECT_JSON_KEY_NAME), name);
}

/**
 * Set items common to any type of AutoConnectElement from JSON objects.
 * @param  json  JSON object with the definition of AutoConnectElement.
 */
void AutoConnectElementJson::_setMember(const JsonObject& json) {
  name = json.get<String>(F(AUTOCONNECT_JSON_KEY_NAME));
  if (json.containsKey(F(AUTOCONNECT_JSON_KEY_VALUE)))
    value = json.get<String>(F(AUTOCONNECT_JSON_KEY_VALUE));
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectButtonJson::getObjectSize() const {
  return AutoConnectElementJson::getObjectSize() + JSON_OBJECT_SIZE(1);
}

/**
 * Load a button element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectButtonJson::loadMember(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACBUTTON))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_ACTION)))
      action = json.get<String>(F(AUTOCONNECT_JSON_KEY_ACTION));
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectButton to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectButtonJson::serialize(JsonObject& json) {
  _serialize(json);
  json.set(F(AUTOCONNECT_JSON_KEY_TYPE), F(AUTOCONNECT_JSON_TYPE_ACBUTTON));
  json.set(F(AUTOCONNECT_JSON_KEY_VALUE), value);
  json.set(F(AUTOCONNECT_JSON_KEY_ACTION), action);
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectCheckboxJson::getObjectSize() const {
  return AutoConnectElementJson::getObjectSize() + JSON_OBJECT_SIZE(2);
}

/**
 * Load a checkbox element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectCheckboxJson::loadMember(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACCHECKBOX))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_LABEL)))
      label = json.get<String>(F(AUTOCONNECT_JSON_KEY_LABEL));
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_CHECKED)))
      checked = json.get<bool>(F(AUTOCONNECT_JSON_KEY_CHECKED));
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectCheckbox to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectCheckboxJson::serialize(JsonObject& json) {
  _serialize(json);
  json.set(F(AUTOCONNECT_JSON_KEY_TYPE), F(AUTOCONNECT_JSON_TYPE_ACCHECKBOX));
  json.set(F(AUTOCONNECT_JSON_KEY_NAME), name);
  json.set(F(AUTOCONNECT_JSON_KEY_VALUE), value);
  json.set(F(AUTOCONNECT_JSON_KEY_LABEL), label);
  json.set(F(AUTOCONNECT_JSON_KEY_CHECKED), checked);
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectInputJson::getObjectSize() const {
  return AutoConnectElementJson::getObjectSize() + JSON_OBJECT_SIZE(3);
}

/**
 * Load a input-box element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectInputJson::loadMember(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACINPUT))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_LABEL)))
      label = json.get<String>(F(AUTOCONNECT_JSON_KEY_LABEL));
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_PATTERN)))
      pattern = json.get<String>(F(AUTOCONNECT_JSON_KEY_PATTERN));
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_PLACEHOLDER)))
      placeholder = json.get<String>(F(AUTOCONNECT_JSON_KEY_PLACEHOLDER));
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectInput to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectInputJson::serialize(JsonObject& json) {
  _serialize(json);
  json.set(F(AUTOCONNECT_JSON_KEY_TYPE), F(AUTOCONNECT_JSON_TYPE_ACINPUT));
  json.set(F(AUTOCONNECT_JSON_KEY_VALUE), value);
  json.set(F(AUTOCONNECT_JSON_KEY_LABEL), label);
  json.set(F(AUTOCONNECT_JSON_KEY_PATTERN), pattern);
  json.set(F(AUTOCONNECT_JSON_KEY_PLACEHOLDER), placeholder);
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectRadioJson::getObjectSize() const {
  return AutoConnectElementJson::getObjectSize() + JSON_OBJECT_SIZE(3) + _values.size() * JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(1);
}

/**
 * Load a radio-button element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectRadioJson::loadMember(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACRADIO))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_LABEL)))
      label = json.get<String>(F(AUTOCONNECT_JSON_KEY_LABEL));
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_CHECKED)))
      checked = static_cast<uint8_t>(json.get<int>(F(AUTOCONNECT_JSON_KEY_CHECKED)));
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_ARRANGE))) {
      String  arrange = json.get<String>(F(AUTOCONNECT_JSON_KEY_ARRANGE));
      if (arrange.equalsIgnoreCase(F(AUTOCONNECT_JSON_KEY_VERTICAL)))
        order = AC_Vertical;
      else if (arrange.equalsIgnoreCase(F(AUTOCONNECT_JSON_KEY_HORIZONTAL)))
        order = AC_Horizontal;
    }
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_VALUE))) {
      empty();
      JsonArray& optionArray = json[AUTOCONNECT_JSON_KEY_VALUE];
      for (auto value : optionArray)
        add(value.as<String>());
    }
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectRadio to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectRadioJson::serialize(JsonObject& json) {
  _serialize(json);
  json.set(F(AUTOCONNECT_JSON_KEY_TYPE), F(AUTOCONNECT_JSON_TYPE_ACRADIO));
  json.set(F(AUTOCONNECT_JSON_KEY_LABEL), label);
  JsonArray&  values = json.createNestedArray(F(AUTOCONNECT_JSON_KEY_VALUE));
  for (String v : _values)
    values.add(v);
  switch (order) {
  case AC_Horizontal:
    json.set(F(AUTOCONNECT_JSON_KEY_ARRANGE), AUTOCONNECT_JSON_KEY_HORIZONTAL);
    break;
  case AC_Vertical:
    json.set(F(AUTOCONNECT_JSON_KEY_ARRANGE), AUTOCONNECT_JSON_KEY_VERTICAL);
    break;
  }
  json.set(F(AUTOCONNECT_JSON_KEY_CHECKED), checked);
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectSelectJson::getObjectSize() const {
  return AutoConnectElementJson::getObjectSize() + JSON_OBJECT_SIZE(3) + _options.size() * JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(1);
}

/**
 * Load a select element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectSelectJson::loadMember(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACSELECT))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_LABEL))) {
      label = json.get<String>(F(AUTOCONNECT_JSON_KEY_LABEL));
    }
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_OPTION))) {
      empty();
      JsonArray& optionArray = json[AUTOCONNECT_JSON_KEY_OPTION];
      for (auto value : optionArray)
        add(value.as<String>());
      return true;
    }
  }
  return false;
}

/**
 * Serialize AutoConnectSelect to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectSelectJson::serialize(JsonObject& json) {
  _serialize(json);
  json.set(F(AUTOCONNECT_JSON_KEY_TYPE), F(AUTOCONNECT_JSON_TYPE_ACSELECT));
  JsonArray&  options = json.createNestedArray(F(AUTOCONNECT_JSON_KEY_OPTION));
  for (String o : _options)
    options.add(o);
  json.set(F(AUTOCONNECT_JSON_KEY_LABEL), label);
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectSubmitJson::getObjectSize() const {
  return AutoConnectElementJson::getObjectSize() + JSON_OBJECT_SIZE(1);
}

/**
 * Load a submit element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectSubmitJson::loadMember(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACSUBMIT))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_URI)))
      uri = json.get<String>(F(AUTOCONNECT_JSON_KEY_URI));
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectSubmit to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectSubmitJson::serialize(JsonObject& json) {
  _serialize(json);
  json.set(F(AUTOCONNECT_JSON_KEY_TYPE), F(AUTOCONNECT_JSON_TYPE_ACSUBMIT));
  json.set(F(AUTOCONNECT_JSON_KEY_VALUE), value);
  json.set(F(AUTOCONNECT_JSON_KEY_URI), uri);
}

/**
 * Returns JSON object size.
 * @return  An object size for JsonBuffer.
 */
size_t AutoConnectTextJson::getObjectSize() const {
  return AutoConnectElementJson::getObjectSize() + JSON_OBJECT_SIZE(1);
}

/**
 * Load a text element attribute member from the JSON object.
 * @param  json  JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectTextJson::loadMember(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACTEXT))) {
    _setMember(json);
    if (json.containsKey(F(AUTOCONNECT_JSON_KEY_STYLE)))
      style = json.get<String>(F(AUTOCONNECT_JSON_KEY_STYLE));
    return true;
  }
  return false;
}

/**
 * Serialize AutoConnectText to JSON.
 * @param  json  JSON object to be serialized.
 */
void AutoConnectTextJson::serialize(JsonObject& json) {
  _serialize(json);
  json.set(F(AUTOCONNECT_JSON_KEY_TYPE), F(AUTOCONNECT_JSON_TYPE_ACTEXT));
  json.set(F(AUTOCONNECT_JSON_KEY_VALUE), value);
  json.set(F(AUTOCONNECT_JSON_KEY_STYLE), style);
}

#endif // _AUTOCONNECTELEMENTJSONIMPL_H_
