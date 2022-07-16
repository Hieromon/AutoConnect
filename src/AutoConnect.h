/**
 * Declares the full AutoConnect component class, including the
 * AutoConnectAux structure. The AutoConnect.h ensures backward
 * compatibility of full AutoConnect components for sketches.
 * @file AutoConnectCore.h
 * @author hieromon@gmail.com
 * @version 1.4.0
 * @date 2022-07-14
 * @copyright MIT license.
 */

#ifndef _AUTOCONNECT_H_
#define _AUTOCONNECT_H_

#include "AutoConnectExtImpl.hpp"

using AutoConnectConfig = AutoConnectConfigExt;
using AutoConnect = AutoConnectExt<AutoConnectConfig>;

#endif // !_AUTOCONNECT_H_
