/**
 * Implementation of template functions of AutoConnect and AutoConnectAux. 
 * This implementation instantiates completely the void AutoConnectElement
 * as each type and also absorbs interface differences due to ArduinoJson
 * version differences. 
 * @file AutoConnectAuxImpl.h
 * @author hieromon@gmail.com
 * @version  0.9.8
 * @date 2019-03-21
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTAUXIMPL_H_
#define _AUTOCONNECTAUXIMPL_H_

#include "AutoConnectDefs.h"

#ifndef AUTOCONNECT_USE_JSON

/**
 * Get AutoConnectElementBasis element.
 * @param  name  an element name.
 * @return A reference of AutoConnectElement class.
 */
template<>
AutoConnectElementBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    return *(reinterpret_cast<AutoConnectElementBasis*>(elm));
  }
  return reinterpret_cast<AutoConnectElementBasis&>(_nullElement());
}

/**
 * Get AutoConnectButtonBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectButton class.
 */
template<>
AutoConnectButtonBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Button)
      return *(reinterpret_cast<AutoConnectButtonBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectButtonBasis&>(_nullElement());
}

/**
 * Get AutoConnectCheckboxBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectCheckbox class.
 */
template<>
AutoConnectCheckboxBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Checkbox)
      return *(reinterpret_cast<AutoConnectCheckboxBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectCheckboxBasis&>(_nullElement());
}

/**
 * Get AutoConnectFileBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectFile class.
 */
template<>
AutoConnectFileBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_File)
      return *(reinterpret_cast<AutoConnectFileBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectFileBasis&>(_nullElement());
}

/**
 * Get AutoConnectInputBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectInput class.
 */
template<>
AutoConnectInputBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Input)
      return *(reinterpret_cast<AutoConnectInputBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectInputBasis&>(_nullElement());
}

/**
 * Get AutoConnectRadioBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectRadio class.
 */
template<>
AutoConnectRadioBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Radio)
      return *(reinterpret_cast<AutoConnectRadioBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectRadioBasis&>(_nullElement());
}

/**
 * Get AutoConnectSelectBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectSelect class.
 */
template<>
AutoConnectSelectBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Select)
      return *(reinterpret_cast<AutoConnectSelectBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectSelectBasis&>(_nullElement());
}

/**
 * Get AutoConnectStyleBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectStyle class.
 */
template<>
AutoConnectStyleBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Style)
      return *(reinterpret_cast<AutoConnectStyleBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectStyleBasis&>(_nullElement());
}

/**
 * Get AutoConnectSubmitBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectSubmit class.
 */
template<>
AutoConnectSubmitBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Submit)
      return *(reinterpret_cast<AutoConnectSubmitBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectSubmitBasis&>(_nullElement());
}

/**
 * Get AutoConnectTextBasis element.
 * @param  name  An element name.
 * @return A reference of AutoConnectText class.
 */
template<>
AutoConnectTextBasis& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Text)
      return *(reinterpret_cast<AutoConnectTextBasis*>(elm));
    else
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
  }
  return reinterpret_cast<AutoConnectTextBasis&>(_nullElement());
}

#else

/**
 * Parse and load a JSON document which marks up multiple custom web
 * pages. The compiler instantiates this template according to the stored
 * data type that contains the JSON document.
 * This template also generates different parsing function calls
 * depending on the ArduinoJson version.
 * @param  T  An object type of the JSON document which must be a
 * passable object to ArduinoJson.
 * @param  in An instance of a source JSON document to load.
 */
template<typename T>
bool AutoConnect::_parseJson(T in) {
  ArduinoJsonBuffer jsonBuffer(AUTOCONNECT_JSONBUFFER_PRIMITIVE_SIZE);
  JsonVariant jv;
#if ARDUINOJSON_VERSION_MAJOR<=5
  jv = jsonBuffer.parse(in);
  if (!jv.success()) {
    AC_DBG("JSON parse error\n");
    return false;
  }
#else
  DeserializationError  err = deserializeJson(jsonBuffer, in);
  if (err) {
    AC_DBG("Deserialize error:%s\n", err.c_str());
    return false;
  }
  jv = jsonBuffer.as<JsonVariant>();
#endif
  return _load(jv);
}

/**
 * Parse and load a JSON document which marks up a custom web page.
 * The compiler instantiates this template according to the stored data
 * type that contains the JSON document.
 * This template also generates different parsing function calls
 * depending on the ArduinoJson version.
 * @param  T  An object type of the JSON document which must be a
 * passable object to ArduinoJson.
 * @param  in An instance of a source JSON document to load.
 */
template<typename T>
bool AutoConnectAux::_parseJson(T in) {
  ArduinoJsonBuffer jsonBuffer(AUTOCONNECT_JSONBUFFER_PRIMITIVE_SIZE);
#if ARDUINOJSON_VERSION_MAJOR<=5
  JsonObject& jb = jsonBuffer.parseObject(in);
  if (!jb.success()) {
    AC_DBG("JSON parse error\n");
    return false;
  }
#else
  DeserializationError  err = deserializeJson(jsonBuffer, in);
  if (err) {
    AC_DBG("Deserialize:%s\n", err.c_str());
    return false;
  }
  JsonObject jb = jsonBuffer.as<JsonObject>();
#endif
  return _load(jb);
}

/**
 * Get AutoConnectElementJson element.
 * @param  name  an element name.
 * @return A reference of AutoConnectElement class.
 */
template<>
AutoConnectElementJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    return *(reinterpret_cast<AutoConnectElementJson*>(elm));
  }
  return reinterpret_cast<AutoConnectElementJson&>(_nullElement());
}

/**
 * Get AutoConnectButtonJson element.
 * @param  name  An element name.
 * @return A reference of AutoConnectButton class.
 */
template<>
AutoConnectButtonJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Button)
      return *(reinterpret_cast<AutoConnectButtonJson*>(elm));
    else {
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
    }
  }
  return reinterpret_cast<AutoConnectButtonJson&>(_nullElement());
}

/**
 * Get AutoConnectCheckboxJson element.
 * @param  name  An element name.
 * @return A reference of AutoConnectCheckbox class.
 */
template<>
AutoConnectCheckboxJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Checkbox)
      return *(reinterpret_cast<AutoConnectCheckboxJson*>(elm));
    else {
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
    }
  }
  return reinterpret_cast<AutoConnectCheckboxJson&>(_nullElement());
}

/**
 * Get AutoConnectFile element.
 * @param  name  An element name.
 * @return A reference of AutoConnectFile class.
 */
template<>
AutoConnectFileJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_File)
      return *(reinterpret_cast<AutoConnectFileJson*>(elm));
    else {
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
    }
  }
  return reinterpret_cast<AutoConnectFileJson&>(_nullElement());
}

/**
 * Get AutoConnectInputJson element.
 * @param  name  An element name.
 * @return A reference of AutoConnectInput class.
 */
template<>
AutoConnectInputJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Input)
      return *(reinterpret_cast<AutoConnectInputJson*>(elm));
    else {
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
    }
  }
  return reinterpret_cast<AutoConnectInputJson&>(_nullElement());
}

/**
 * Get AutoConnectRadioJson element.
 * @param  name  An element name.
 * @return A reference of AutoConnectRadio class.
 */
template<>
AutoConnectRadioJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Radio)
      return *(reinterpret_cast<AutoConnectRadioJson*>(elm));
    else {
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
    }
  }
  return reinterpret_cast<AutoConnectRadioJson&>(_nullElement());
}

/**
 * Get AutoConnectSelectJson element.
 * @param  name  An element name.
 * @return A reference of AutoConnectSelect class.
 */
template<>
AutoConnectSelectJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Select)
      return *(reinterpret_cast<AutoConnectSelectJson*>(elm));
    else {
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
    }
  }
  return reinterpret_cast<AutoConnectSelectJson&>(_nullElement());
}

/**
 * Get AutoConnectSubmitJson element.
 * @param  name  An element name.
 * @return A reference of AutoConnectSubmit class.
 */
template<>
AutoConnectSubmitJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Submit)
      return *(reinterpret_cast<AutoConnectSubmitJson*>(elm));
    else {
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
    }
  }
  return reinterpret_cast<AutoConnectSubmitJson&>(_nullElement());
}

/**
 * Get AutoConnectTextJson element.
 * @param  name  An element name.
 * @return A reference of AutoConnectText class.
 */
template<>
AutoConnectTextJson& AutoConnectAux::getElement(const String& name) {
  AutoConnectElement* elm = getElement(name);
  if (elm) {
    if (elm->typeOf() == AC_Text)
      return *(reinterpret_cast<AutoConnectTextJson*>(elm));
    else {
      AC_DBG("Element<%s> type mismatch<%d>\n", name.c_str(), elm->typeOf());
    }
  }
  return reinterpret_cast<AutoConnectTextJson&>(_nullElement());
}

#endif // !AUTOCONNECT_USE_JSON

#endif // !_AUTOCONNECTAUXIMPL_H_
