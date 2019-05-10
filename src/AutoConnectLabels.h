/**
 *  AutoConnect proper menu label constant definition.
 *  @file   AutoConnectLabels.h
 *  @author hieromon@gmail.com
 *  @version    0.9.8
 *  @date   2019-03-11
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

// Button label: RESET
#ifndef AUTOCONNECT_BUTTONLABEL_RESET
#define AUTOCONNECT_BUTTONLABEL_RESET     "RESET"
//#define AUTOCONNECT_BUTTONLABEL_RESET     "Reboot"
#endif // !AUTOCONNECT_BUTTONLABEL_RESET

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
#endif
