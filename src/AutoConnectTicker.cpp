/**
 *  AutoConnectTicker class implementation.
 *  Provides a service that shows the flicker signal according to WiFi
 *  connection status. 
 *  @file   AutoConnectTicker.cpp
 *  @author hieromon@gmail.com
 *  @version    1.4.2
 *  @date   2023-01-23
 *  @copyright  MIT license.
 */

#include "AutoConnectTicker.h"

// Support for Ticker Longer delays with ESP8266.
// Details for https://github.com/esp8266/Arduino/pull/8625
#define AC_TICKER_CALLBACK_ARG_T  <AutoConnectTicker*>
#if defined(ARDUINO_ESP8266_MAJOR) && defined(ARDUINO_ESP8266_MINOR) && defined(ARDUINO_ESP8266_REVISION)
#if ARDUINO_ESP8266_MAJOR >= 3 && ARDUINO_ESP8266_MINOR >= 1 && ARDUINO_ESP8266_REVISION >= 0
#define AC_TICKER_LONGER_DELAY
#undef  AC_TICKER_CALLBACK_ARG_T
#define AC_TICKER_CALLBACK_ARG_T
#endif
#endif

/**
 * Start ticker cycle
 * @param cycle Cycle time in [ms]
 * @param duty  Duty cycle in [ms]
 */
void AutoConnectTicker::start(const uint32_t cycle, const uint32_t duty) {
  _cycle = cycle;
  if (duty <= _cycle)
    _duty = duty;
  start();
}

/**
 * Start ticker cycle
 */
void AutoConnectTicker::start(void) {
  pinMode(_port, OUTPUT);
  _pulse.detach();
  _period.attach_ms AC_TICKER_CALLBACK_ARG_T(_cycle, AutoConnectTicker::_onPeriod, this);
}

/**
 * Turn on the flicker signal and reserves a ticker to turn off the
 * signal. This behavior will perform every cycle to generate the
 * pseudo-PWM signal.
 * If the function is registered, call the callback function at the
 * end of one cycle.
 * @param  t  Its own address
 */
void AutoConnectTicker::_onPeriod(AutoConnectTicker* t) {
  digitalWrite(t->_port, t->_turnOn);
  t->_pulse.once_ms AC_TICKER_CALLBACK_ARG_T(t->_duty, AutoConnectTicker::_onPulse, t);
  if (t->_callback)
    t->_callback();
}

/**
 * Turn off the flicker signal
 * @param  t  Its own address
 */
void AutoConnectTicker::_onPulse(AutoConnectTicker* t) {
  digitalWrite(t->_port, !(t->_turnOn));
}
