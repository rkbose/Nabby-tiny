// This file is part of Nabby-tiny.
// Nabby-tiny is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nabby-tiny is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nabby-tiny. If not, see <https://www.gnu.org/licenses/>.

#include "Watchdog.h"
#include <esp32-hal-timer.h>

bool isrTriggered = false;
hw_timer_t *watchdogTimer = NULL;

/* watchdog interrupts will run when hw timer reached timeout */
static void IRAM_ATTR wdInterrupt()
{
  isrTriggered = true;
}

/**************************************************************************/
/*
  Initilizes the hwTimer.
*/
/**************************************************************************/
void wdInit(int timeout)
{
  isrTriggered = false;
  watchdogTimer = timerBegin(0, 80, true);                 // init hw timer 0; prescaler 80 (for 1msec @80 MHz clk); counting up
  timerAttachInterrupt(watchdogTimer, &wdInterrupt, true); // attached interrupt; ISR routing; edge triggerd
  timerAlarmWrite(watchdogTimer, timeout * 1000, false);   // set alarm in mSec; autoreload = false
  timerAlarmEnable(watchdogTimer);
}

/**************************************************************************/
/*
  Triggers the watchdog.
*/
/**************************************************************************/
void wdTrigger(void)
{
  timerWrite(watchdogTimer, 0); // reset the timer
}

/**************************************************************************/
/*
   Returns boolean with triggerStatus. TRUE means triggered.
*/
/**************************************************************************/
bool wdStatus(void)
{
  return (isrTriggered);
}

/**************************************************************************/
/*
  Stops the watchdog, disables the interrupt and stops hardware timer.
*/
/**************************************************************************/
/*
void Watchdog::stop(void)
{
  timerAlarmDisable(watchdogTimer);
}
*/
