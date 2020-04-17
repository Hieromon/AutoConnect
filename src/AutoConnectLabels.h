/**
 *  AutoConnect proper menu label constant definition.
 *  @file   AutoConnectLabels.h
 *  @author hieromon@gmail.com
 *  @version    1.1.6
 *  @date   2020-04-17
 *  @copyright  MIT license.
 */

#ifndef _AUTOCONNECTLABELS_H_
#define _AUTOCONNECTLABELS_H_

/**
 * The upper row of each of the following definition items is the
 * default value. The lower row is an alternative label string as
 * a sample and can be changed you wish.
 * 
 * Notes:
 * You can find some tokens for the PageBuilder if you parse
 * AutoConnectPage.cpp. Some of the tokens are valid at the time
 * the AutoConnect menu will generate. For example, the token
 * `{{CUR_SSID}}` returns the SSID of the currently participating
 * AP by AutoConnect::_token_CURRENT_SSID function.
 * You can use these tokens to display dynamic menus showing the
 * current situation, but you need to know the internal structure
 * of AutoConnect in order to display the appropriate menu.
 */

// Menu item: Configure new AP
#ifndef AUTOCONNECT_MENULABEL_CONFIGNEW
#define AUTOCONNECT_MENULABEL_CONFIGNEW   "Configure new AP"
//#define AUTOCONNECT_MENULABEL_CONFIGNEW   "Establish a new AP"
#endif // !AUTOCONNECT_MENULABEL_CONFIGNEW

// Menu item: Open SSIDs
#ifndef AUTOCONNECT_MENULABEL_OPENSSIDS
#define AUTOCONNECT_MENULABEL_OPENSSIDS   "Open SSIDs"
//#define AUTOCONNECT_MENULABEL_OPENSSIDS   "Open credentials"
#endif // !AUTOCONNECT_MENULABEL_OPENSSIDS

// Menu item: Disconnect
#ifndef AUTOCONNECT_MENULABEL_DISCONNECT
#define AUTOCONNECT_MENULABEL_DISCONNECT  "Disconnect"
//#define AUTOCONNECT_MENULABEL_DISCONNECT  "Leave {{CUR_SSID}}"
#endif // !AUTOCONNECT_MENULABEL_DISCONNECT

// Menu item: Reset...
#ifndef AUTOCONNECT_MENULABEL_RESET
#define AUTOCONNECT_MENULABEL_RESET       "Reset..."
//#define AUTOCONNECT_MENULABEL_RESET       "Reboot..."
#endif // !AUTOCONNECT_MENULABEL_RESET

// Menu item: HOME
#ifndef AUTOCONNECT_MENULABEL_HOME
#define AUTOCONNECT_MENULABEL_HOME        "HOME"
//#define AUTOCONNECT_MENULABEL_HOME        "Main"
#endif // !AUTOCONNECT_MENULABEL_HOME

// Menu item: Update
#ifndef AUTOCONNECT_MENULABEL_UPDATE
#define AUTOCONNECT_MENULABEL_UPDATE      "Update"
#endif // !AUTOCONNECT_MENULABEL_UPDATE

// Menu item: Device Info
#ifndef AUTOCONNECT_MENULABEL_DEVINFO
#define AUTOCONNECT_MENULABEL_DEVINFO     "Device info"
#endif // !AUTOCONNECT_MENULABEL_DEVINFO

// Button label: RESET
#ifndef AUTOCONNECT_BUTTONLABEL_RESET
#define AUTOCONNECT_BUTTONLABEL_RESET     "RESET"
//#define AUTOCONNECT_BUTTONLABEL_RESET     "Reboot"
#endif // !AUTOCONNECT_BUTTONLABEL_RESET

// Button label: UPDATE
#ifndef AUTOCONNECT_BUTTONLABEL_UPDATE
#define AUTOCONNECT_BUTTONLABEL_UPDATE     "UPDATE"
#endif // !AUTOCONNECT_BUTTONLABEL_UPDATE

// Page title: Page not found
#ifndef AUTOCONNECT_PAGETITLE_NOTFOUND
#define AUTOCONNECT_PAGETITLE_NOTFOUND "Page not found"
#endif // !AUTOCONNECT_PAGETITLE_NOTFOUND

// Page title: AutoConnect resetting
#ifndef AUTOCONNECT_PAGETITLE_RESETTING
#define AUTOCONNECT_PAGETITLE_RESETTING "AutoConnect resetting"
#endif // !AUTOCONNECT_PAGETITLE_RESETTING

// Page title: AutoConnect statistics
#ifndef AUTOCONNECT_PAGETITLE_STATISTICS
#define AUTOCONNECT_PAGETITLE_STATISTICS "AutoConnect statistics"
#endif // !AUTOCONNECT_PAGETITLE_STATISTICS

// Page statistics row: Established connection
#ifndef AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION
#define AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION "Established connection"
#endif // !AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION

// Page statistics row: Mode
#ifndef AUTOCONNECT_PAGESTATS_MODE
#define AUTOCONNECT_PAGESTATS_MODE "Mode"
#endif // !AUTOCONNECT_PAGESTATS_MODE

// Page statistics row: IP
#ifndef AUTOCONNECT_PAGESTATS_IP
#define AUTOCONNECT_PAGESTATS_IP "IP"
#endif // !AUTOCONNECT_PAGESTATS_IP

// Page statistics row: GW
#ifndef AUTOCONNECT_PAGESTATS_GATEWAY
#define AUTOCONNECT_PAGESTATS_GATEWAY "GW"
#endif // !AUTOCONNECT_PAGESTATS_GATEWAY

// Page statistics row: Subnet mask
#ifndef AUTOCONNECT_PAGESTATS_SUBNETMASK
#define AUTOCONNECT_PAGESTATS_SUBNETMASK "Subnet mask"
#endif // !AUTOCONNECT_PAGESTATS_SUBNETMASK

// Page statistics row: SoftAP IP
#ifndef AUTOCONNECT_PAGESTATS_SOFTAPIP
#define AUTOCONNECT_PAGESTATS_SOFTAPIP "SoftAP IP"
#endif // !AUTOCONNECT_PAGESTATS_SOFTAPIP

// Page statistics row: AP MAC
#ifndef AUTOCONNECT_PAGESTATS_APMAC
#define AUTOCONNECT_PAGESTATS_APMAC "AP MAC"
#endif // !AUTOCONNECT_PAGESTATS_APMAC

// Page statistics row: STA MAC
#ifndef AUTOCONNECT_PAGESTATS_STAMAC
#define AUTOCONNECT_PAGESTATS_STAMAC "STA MAC"
#endif // !AUTOCONNECT_PAGESTATS_STAMAC

// Page statistics row: Channel
#ifndef AUTOCONNECT_PAGESTATS_CHANNEL
#define AUTOCONNECT_PAGESTATS_CHANNEL "Channel"
#endif // !AUTOCONNECT_PAGESTATS_CHANNEL

// Page statistics row: dBm
#ifndef AUTOCONNECT_PAGESTATS_DBM
#define AUTOCONNECT_PAGESTATS_DBM "dBm"
#endif // !AUTOCONNECT_PAGESTATS_DBM

// Page statistics row: Chip ID
#ifndef AUTOCONNECT_PAGESTATS_CHIPID
#define AUTOCONNECT_PAGESTATS_CHIPID "Chip ID"
#endif // !AUTOCONNECT_PAGESTATS_CHIPID

// Page statistics row: CPU Freq.
#ifndef AUTOCONNECT_PAGESTATS_CPUFREQ
#define AUTOCONNECT_PAGESTATS_CPUFREQ "CPU Freq."
#endif // !AUTOCONNECT_PAGESTATS_CPUFREQ

// Page statistics row: Flash size
#ifndef AUTOCONNECT_PAGESTATS_FLASHSIZE
#define AUTOCONNECT_PAGESTATS_FLASHSIZE "Flash size"
#endif // !AUTOCONNECT_PAGESTATS_FLASHSIZE

// Page statistics row: Free memory
#ifndef AUTOCONNECT_PAGESTATS_FREEMEM
#define AUTOCONNECT_PAGESTATS_FREEMEM "Free memory"
#endif // !AUTOCONNECT_PAGESTATS_FREEMEM

// Page title: AutoConnect config
#ifndef AUTOCONNECT_PAGETITLE_CONFIG
#define AUTOCONNECT_PAGETITLE_CONFIG "AutoConnect config"
#endif // !AUTOCONNECT_PAGETITLE_CONFIG

// Page config text: Total:
#ifndef AUTOCONNECT_PAGECONFIG_TOTAL
#define AUTOCONNECT_PAGECONFIG_TOTAL "Total:"
#endif // !AUTOCONNECT_PAGECONFIG_TOTAL

// Page config text: Hidden:
#ifndef AUTOCONNECT_PAGECONFIG_HIDDEN
#define AUTOCONNECT_PAGECONFIG_HIDDEN "Hidden:"
#endif // !AUTOCONNECT_PAGECONFIG_HIDDEN

// Page config text: SSID
#ifndef AUTOCONNECT_PAGECONFIG_SSID
#define AUTOCONNECT_PAGECONFIG_SSID "SSID"
#endif // !AUTOCONNECT_PAGECONFIG_SSID

// Page config text: Passphrase
#ifndef AUTOCONNECT_PAGECONFIG_PASSPHRASE
#define AUTOCONNECT_PAGECONFIG_PASSPHRASE "Passphrase"
#endif // !AUTOCONNECT_PAGECONFIG_PASSPHRASE

// Page config text: Enable DHCP
#ifndef AUTOCONNECT_PAGECONFIG_ENABLEDHCP
#define AUTOCONNECT_PAGECONFIG_ENABLEDHCP "Enable DHCP"
#endif // !AUTOCONNECT_PAGECONFIG_ENABLEDHCP

// Page config text: Apply
#ifndef AUTOCONNECT_PAGECONFIG_APPLY
#define AUTOCONNECT_PAGECONFIG_APPLY "Apply"
#endif // !AUTOCONNECT_PAGECONFIG_APPLY

// Page title: AutoConnect credentials
#ifndef AUTOCONNECT_PAGETITLE_CREDENTIALS
#define AUTOCONNECT_PAGETITLE_CREDENTIALS "AutoConnect credentials"
#endif // !AUTOCONNECT_PAGETITLE_CREDENTIALS

// Page title: AutoConnect connecting
#ifndef AUTOCONNECT_PAGETITLE_CONNECTING
#define AUTOCONNECT_PAGETITLE_CONNECTING "AutoConnect connecting"
#endif // !AUTOCONNECT_PAGETITLE_CONNECTING

// Page title: AutoConnect connection failed
#ifndef AUTOCONNECT_PAGETITLE_CONNECTIONFAILED
#define AUTOCONNECT_PAGETITLE_CONNECTIONFAILED "AutoConnect connection failed"
#endif // !AUTOCONNECT_PAGETITLE_CONNECTIONFAILED

// Page connection failed: Connection Failed
#ifndef AUTOCONNECT_PAGECONNECTIONFAILED_CONNECTIONFAILED
#define AUTOCONNECT_PAGECONNECTIONFAILED_CONNECTIONFAILED "Connection Failed"
#endif // !AUTOCONNECT_PAGECONNECTIONFAILED_CONNECTIONFAILED

// Page title: AutoConnect disconnected
#ifndef AUTOCONNECT_PAGETITLE_DISCONNECTED
#define AUTOCONNECT_PAGETITLE_DISCONNECTED "AutoConnect disconnected"
#endif // !AUTOCONNECT_PAGETITLE_DISCONNECTED

// Text: No saved credentials.
#ifndef AUTOCONNECT_TEXT_NOSAVEDCREDENTIALS
#define AUTOCONNECT_TEXT_NOSAVEDCREDENTIALS "No saved credentials."
#endif // !AUTOCONNECT_TEXT_NOSAVEDCREDENTIALS

// Text: The update page caption
#ifndef AUTOCONNECT_TEXT_UPDATINGFIRMWARE
#define AUTOCONNECT_TEXT_UPDATINGFIRMWARE "Updating firmware"
#endif // !AUTOCONNECT_TEXT_UPDATINGFIRMWARE

// Text: The update page's file selection button label
#ifndef AUTOCONNECT_TEXT_SELECTFIRMWARE
#define AUTOCONNECT_TEXT_SELECTFIRMWARE "Select firmware: "
#endif // !AUTOCONNECT_TEXT_SELECTFIRMWARE

// Text: OTA success
#ifndef AUTOCONNECT_TEXT_OTASUCCESS
#define AUTOCONNECT_TEXT_OTASUCCESS "Successfully updated, rebooting..."
#endif // !AUTOCONNECT_TEXT_OTASUCCESS

// Text: OTA failure
#ifndef AUTOCONNECT_TEXT_OTAFAILURE
#define AUTOCONNECT_TEXT_OTAFAILURE "Failed to update: "
#endif // !AUTOCONNECT_TEXT_OTAFAILURE

// Menu Text: Connecting
#ifndef AUTOCONNECT_MENUTEXT_CONNECTING
#define AUTOCONNECT_MENUTEXT_CONNECTING "Connecting"
#endif // !AUTOCONNECT_MENUTEXT_CONNECTING

// Menu Text: Disconnect
#ifndef AUTOCONNECT_MENUTEXT_DISCONNECT
#define AUTOCONNECT_MENUTEXT_DISCONNECT "Disconnect"
#endif // !AUTOCONNECT_MENUTEXT_DISCONNECT

// Menu Text: Failed
#ifndef AUTOCONNECT_MENUTEXT_FAILED
#define AUTOCONNECT_MENUTEXT_FAILED "Failed"
#endif // !AUTOCONNECT_MENUTEXT_FAILED

// Menu colors
// The following three color code items determine the color scheme of
// the menu. In addition to hexadecimal color values, you can specify
// 140 standard color names that apply to CSS.
// To be no sense of discomfort, select the same series of values for
// the background and the active item.

// Menu text foreground color
#ifndef AUTOCONNECT_MENUCOLOR_TEXT
#define AUTOCONNECT_MENUCOLOR_TEXT        "#fff"
//#define AUTOCONNECT_MENUCOLOR_TEXT        "#fffacd"
#endif // !AUTOCONNECT_MENUCOLOR_TEXT

// Menu background color
#ifndef AUTOCONNECT_MENUCOLOR_BACKGROUND
#define AUTOCONNECT_MENUCOLOR_BACKGROUND  "#263238"
// #define AUTOCONNECT_MENUCOLOR_BACKGROUND  "#696969"
#endif // !AUTOCONNECT_MENUCOLOR_BACKGROUND

// Background color with active menu items and mouse hover
#ifndef AUTOCONNECT_MENUCOLOR_ACTIVE
#define AUTOCONNECT_MENUCOLOR_ACTIVE      "#37474f"
// #define AUTOCONNECT_MENUCOLOR_ACTIVE      "#808080"
#endif

/**< Override the hardcoded strings contained in the AutoConnect pages.    */
/**< e.g. for PlatformIO, you can add your environment in platformio.ini   */
/**< along with AC_LABLES macro which specifies the user-defined label     */
/**< constants as follows:                                                 */
/**<                                                                       */
/**< build_flags =                                                         */
/**<   -DAC_LABELS='"${PROJECT_SRC_DIR}/mylabels.h"'                       */
/**<                                                                       */
/**< And places mylabels.h, it needs a structure of the define directive   */
/**< provided per label string definition for the change your wants.       */
/**<                                                                       */
/**< #ifdef [ID YOU WANT TO CHANGE]                                        */
/**< #undef [ID YOU WANT TO CHANGE]                                        */
/**< #define [ID YOU WANT TO CHANGE] "NEW_STRING_FOR_THISONE"              */
/**< #endif                                                                */
/**<                                                                       */
/**< example:
#ifdef AUTOCONNECT_MENULABEL_CONFIGNEW
#undef AUTOCONNECT_MENULABEL_CONFIGNEW
#define AUTOCONNECT_MENULABEL_CONFIGNEW   "NEW_STRING_FOR_THISONE"
#endif
*/
#ifdef AC_LABELS
#include AC_LABELS
#endif

#endif
