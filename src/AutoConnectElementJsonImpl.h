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
 * Set items common to any type of AutoConnectElement from JSON objects.
 * @param  json  A JSON object with the definition of AutoConnectElement.
 */
void AutoConnectElementJson::_setElement(const JsonObject& json) {
  name = json.get<String>(F(AUTOCONNECT_JSON_KEY_NAME));
  value = json.get<String>(F(AUTOCONNECT_JSON_KEY_VALUE));
}

/**
 * Load an element member value from the JSON object.
 * @param  json  A JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectElementJson::loadElement(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACELEMENT))) {
    _setElement(json);
    return true;
  }
  return false;
}

/**
 * Load a button element attribute member from the JSON object.
 * @param  json  A JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectButtonJson::loadElement(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACBUTTON))) {
    _setElement(json);
    action = json.get<String>(F(AUTOCONNECT_JSON_KEY_ACTION));
    return true;
  }
  return false;
}

/**
 * Load a checkbox element attribute member from the JSON object.
 * @param  json  A JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectCheckboxJson::loadElement(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACCHECKBOX))) {
    _setElement(json);
    label = json.get<String>(F(AUTOCONNECT_JSON_KEY_LABEL));
    checked = json.get<bool>(F(AUTOCONNECT_JSON_KEY_CHECKED));
    return true;
  }
  return false;
}

/**
 * Load a input-box element attribute member from the JSON object.
 * @param  json  A JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectInputJson::loadElement(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACINPUT))) {
    _setElement(json);
    placeholder = json.get<String>(F(AUTOCONNECT_JSON_KEY_PLACEHOLDER));
    label = json.get<String>(F(AUTOCONNECT_JSON_KEY_LABEL));
    return true;
  }
  return false;
}

/**
* Load a radio-button element attribute member from the JSON object.
* @param  json  A JSON object with the definition of AutoConnectElement.
* @return true  AutoConnectElement loaded
* @return false Type of AutoConnectElement is mismatched.
*/
bool AutoConnectRadioJson::loadElement(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACRADIO))) {
    _setElement(json);
    label = json.get<String>(F(AUTOCONNECT_JSON_KEY_LABEL));
    checked = static_cast<uint8_t>(json.get<int>(F(AUTOCONNECT_JSON_KEY_CHECKED)));
    String  arrange = json.get<String>(F(AUTOCONNECT_JSON_KEY_ARRANGE));
    if (arrange.equalsIgnoreCase(F(AUTOCONNECT_JSON_KEY_VERTICAL)))
      order = AC_Vertical;
    else if (arrange.equalsIgnoreCase(F(AUTOCONNECT_JSON_KEY_HORIZONTAL)))
      order = AC_Horizontal;
    empty();
    JsonArray& optionArray = json[AUTOCONNECT_JSON_KEY_VALUE];
    for (auto value : optionArray)
      add(value.as<String>());
    return true;
  }
  return false;
}

/**
 * Load a select element attribute member from the JSON object.
 * @param  json  A JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectSelectJson::loadElement(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACSELECT))) {
    _setElement(json);
    label = json.get<String>(F(AUTOCONNECT_JSON_KEY_LABEL));
    empty();
    JsonArray& optionArray = json[AUTOCONNECT_JSON_KEY_OPTION];
    for (auto value : optionArray)
      add(value.as<String>());
    return true;
  }
  return false;
}

/**
 * Load a submit element attribute member from the JSON object.
 * @param  json  A JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectSubmitJson::loadElement(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACSUBMIT))) {
    _setElement(json);
    uri = json.get<String>(F(AUTOCONNECT_JSON_KEY_URI));
    return true;
  }
  return false;
}

/**
 * Load a text element attribute member from the JSON object.
 * @param  json  A JSON object with the definition of AutoConnectElement.
 * @return true  AutoConnectElement loaded
 * @return false Type of AutoConnectElement is mismatched.
 */
bool AutoConnectTextJson::loadElement(const JsonObject& json) {
  String  type = json.get<String>(F(AUTOCONNECT_JSON_KEY_TYPE));
  if (type.equalsIgnoreCase(F(AUTOCONNECT_JSON_TYPE_ACTEXT))) {
    _setElement(json);
    style = json.get<String>(F(AUTOCONNECT_JSON_KEY_STYLE));
    return true;
  }
  return false;
}

#endif // _AUTOCONNECTELEMENTJSONIMPL_H_
