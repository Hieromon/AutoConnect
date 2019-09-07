/**
 * Wrapping definition to ensure version compatibility of ArduinoJson.
 * @file AutoConnectJsonDefs.h
 * @author hieromon@gmail.com
 * @version  1.0.0
 * @date 2019-04-25
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTJSONDEFS_H_
#define _AUTOCONNECTJSONDEFS_H_

#include <ArduinoJson.h>

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
#define ArduinoJsonStaticBuffer           StaticJsonBuffer
#define ARDUINOJSON_CREATEOBJECT(doc)     doc.createObject()
#define ARDUINOJSON_CREATEARRAY(doc)      doc.createArray()
#define ARDUINOJSON_PRETTYPRINT(doc, out) ({ size_t s = doc.prettyPrintTo(out); s; })
#define ARDUINOJSON_PRINT(doc, out)       ({ size_t s = doc.printTo(out); s; })
using ArduinoJsonObject = JsonObject&;
using ArduinoJsonArray = JsonArray&;
using ArduinoJsonBuffer = DynamicJsonBuffer;
#define AUTOCONNECT_JSONBUFFER_PRIMITIVE_SIZE AUTOCONNECT_JSONBUFFER_SIZE
#else
#define ArduinoJsonStaticBuffer           StaticJsonDocument
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
    return heap_caps_malloc(size, caps);
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

#endif // _AUTOCONNECTJSONDEFS_H_
