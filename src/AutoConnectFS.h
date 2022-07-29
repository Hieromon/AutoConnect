/**
 * Predefined file system applied for AutoConnect usage.
 * @file AutoConnectFS.h
 * @author hieromon@gmail.com
 * @version  1.3.5
 * @date 2022-06-03
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTFS_H_
#define _AUTOCONNECTFS_H_

// Globally deploy the applicable file system classes and instances.
// The type of the applicable file system to be used is switched
// according to the AC_USE_SPIFFS and AC_USE_LITTLEFS definition.
// The file system indicator to apply is expanded by the macro definition,
// AUTOCONNECT_APPLIED_FILECLASS is assigned the class name,
// and AUTOCONNECT_APPLIED_FILESYSTEM is assigned the global instance name.
#if defined(ARDUINO_ARCH_ESP8266)
#define AC_DEFAULT_FILESYSTEM 2
#define AUTOCONNECT_FS_INITIALIZATION
#elif defined(ARDUINO_ARCH_ESP32)
#define AC_DEFAULT_FILESYSTEM 1
#define AUTOCONNECT_FS_INITIALIZATION  true
#endif

#if !defined(AC_USE_SPIFFS) && !defined(AC_USE_LITTLEFS)
#define AC_USE_FILESYSTEM AC_DEFAULT_FILESYSTEM
#elif defined(AC_USE_SPIFFS)
#define AC_USE_FILESYSTEM 1
#elif defined(AC_USE_LITTLEFS)
#define AC_USE_FILESYSTEM 2
#endif

// Note: If LittleFS.h becomes Not Found in PlatformIO, try specifying
// lib_ldf_mode=deep with platformio.ini. Due to the deep nesting by
// preprocessor instructions, the include file cannot be detected by the
// chain mode (nested include search) of PlatformIO's dependent library
// search.
#ifdef ARDUINO_ARCH_ESP8266
#define AUTOCONNECT_APPLIED_FILECLASS     fs::FS
#endif
#if AC_USE_FILESYSTEM == 1
#include <FS.h>
#define AUTOCONNECT_APPLIED_FILESYSTEM    SPIFFS
#ifdef ARDUINO_ARCH_ESP32
#include <SPIFFS.h>
#define AUTOCONNECT_APPLIED_FILECLASS     fs::SPIFFSFS
extern "C" {
#include <esp_spiffs.h>
}
#endif
#elif AC_USE_FILESYSTEM == 2
#include <LittleFS.h>
#define AUTOCONNECT_APPLIED_FILESYSTEM    LittleFS
#ifdef ARDUINO_ARCH_ESP32
extern "C" {
#include <esp_littlefs.h>
}
#define AUTOCONNECT_APPLIED_FILECLASS     fs::LittleFSFS

// With ESP32 platform core version less 2.0, reverts the LittleFS class and
// the exported instance to the ordinary LittleFS_esp32 library owns.
#if !defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR<2
#undef AUTOCONNECT_APPLIED_FILESYSTEM
#define AUTOCONNECT_APPLIED_FILESYSTEM    LITTLEFS
#undef AUTOCONNECT_APPLIED_FILECLASS
#define AUTOCONNECT_APPLIED_FILECLASS     fs::LITTLEFSFS
#endif
#endif
#endif

// Deploy the file class for the AutoConnect scope.
namespace AutoConnectFS {
  using FS = AUTOCONNECT_APPLIED_FILECLASS;
  inline bool _isMounted(AutoConnectFS::FS* filesystem) {
#if defined(ARDUINO_ARCH_ESP8266)
    FSInfo  info;
    return filesystem->info(info);
#elif defined( ARDUINO_ARCH_ESP32)
    (void)(filesystem);
#if AC_USE_FILESYSTEM == 1
    return esp_spiffs_mounted(NULL);
#elif AC_USE_FILESYSTEM == 2
    return esp_littlefs_mounted(NULL);
#endif
#endif
  }
};

// Export an actual file system indicator
#if AC_USE_FILESYSTEM == 1
#define AUTOCONNECT_USE_SPIFFS
#elif AC_USE_FILESYSTEM == 2
#define AUTOCONNECT_USE_LITTLEFS
#endif

#endif // !_AUTOCONNECTFS_H_
