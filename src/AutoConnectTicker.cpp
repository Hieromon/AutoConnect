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
#ifdef AC_TICKER_LONGER_DELAY
  _period.attach_ms<void(AutoConnectTicker*), AutoConnectTicker*>(_cycle, AutoConnectTicker::_onPeriod, this);
#else
  _period.attach_ms<AutoConnectTicker*>(_cycle, AutoConnectTicker::_onPeriod, this);
#endif
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
#ifdef AC_TICKER_LONGER_DELAY
  t->_pulse.once_ms<void(AutoConnectTicker*), AutoConnectTicker*>(t->_duty, AutoConnectTicker::_onPulse, t);
#else
  t->_pulse.once_ms<AutoConnectTicker*>(t->_duty, AutoConnectTicker::_onPulse, t);
#endif
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
