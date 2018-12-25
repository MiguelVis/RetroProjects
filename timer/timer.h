/*	timer.h

	Timer library for MESCC.

	Copyright (c) 2018 Miguel Garcia / FloppySoftware

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 3, or (at your option) any
	later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

	Author's contact:

		www.floppysoftware.es
		cpm-connections.blogspot.com
		floppysoftware@gmail.com

	Revisions:

		09 Oct 2018 : 1st version.
*/

#define TIMER_MAX 8

#define TIMER_DEFAULT 0
#define TIMER_ENABLE  1
#define TIMER_ONCE    2

int _timer_interval[TIMER_MAX];    /* Time interval; 0 == free slot */
int _timer_left[TIMER_MAX];        /* Time remaining to next fire */
int _timer_attributes[TIMER_MAX];  /* 0 == disabled, else enabled */
int _timer_callback[TIMER_MAX];    /* Function to call on fire */

// Execute timer callbacks as needed.
TimerLoop()
{
	int i;

	for(i = 0; i < TIMER_MAX; ++i) {
		if(_timer_interval[i]) {
			if(_timer_attributes[i] & TIMER_ENABLE) {
				if(!(--_timer_left[i])) {
					if(_timer_attributes[i] & TIMER_ONCE) {
						_timer_interval[i] = 0;
					}
					else {
						_timer_left[i] = _timer_interval[i];
					}

					_timer_callback[i](i);
				}
			}
		}
	}
}

// Create timer. Returns timer id, or -1 on failure
Timer(interval, attributes, callback)
int interval;
int attributes;
WORD callback;
{
	int i;

	for(i = 0; i < TIMER_MAX; ++i) {
		if(!_timer_interval[i]) {
			_timer_left[i] = _timer_interval[i] = interval;
			_timer_attributes[i] = attributes;
			_timer_callback[i] = callback;

			return i;
		}
	}

	return -1;
}

// Delete timer
TimerKill(id)
int id;
{
	if(TimerExists(id)) {
		_timer_interval[id] = 0;
	}
}

// Enable timer
TimerEnable(id, enable)
int id;
int enable;
{
	if(TimerExists(id)) {
		if(enable) {
			_timer_attributes[id] |= TIMER_ENABLE;
		}
		else {
			_timer_attributes[id] &= ~TIMER_ENABLE;
		}
	}
}

// Test if timer is enabled. Returns 1 if enabled, else 0.
TimerIsEnabled(id)
int id;
{
	if(TimerExists(id)) {
		return (_timer_attributes[id] & TIMER_ENABLE ? 1 : 0);
	}

	return 0;
}

// Test if timer exists. Returns 1 if exists, else 0.
TimerExists(id)
int id;
{
	if(id >= 0 && id < TIMER_MAX) {
		if(_timer_interval[id]) {
			return 1;
		}
	}

	return 0;
}

