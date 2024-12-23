// This file is part of Nabby-tiny.
// Nabby-tiny is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nabby-tiny is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nabby-tiny. If not, see <https://www.gnu.org/licenses/>.
// Originally DynamicCommandparser was  Created by Mathias Djärv, June 31, 2014.
// And Released under Creative Commons Attribution 4.0 International (CC BY 4.0)    http://creativecommons.org/licenses/by/4.0/
//

#ifndef NABBY_H
#define NABBY_H

#include "Arduino.h"

extern volatile int LedNotificationCount;

void initNabby(void);
void blinkLeds(void);
void leftEarGoto(unsigned int targetPosition, int speed);
void rightEarGoto(unsigned int targetPosition, int speed);

#endif
