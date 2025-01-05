Nabaztag-messenger
Nabby - Nabaztag rabbit messagenger Project created by Raj Bose, July 2020

This project was created to convert an old non functioning NABAZTAG rabbit into a messaging system connected on Wifi. Features are:

MP3 player inside (KT403A based)
ESP8266 and ESP32 based
Nabaztag is fully functional:
- Left and right ear positioning using encoder
- Top button to kill sound
- RGB LED's connected via I2C IO expander
- Seperate doorbell unit connecting to Wifi
- Nabaztag (called Nabby) connecting to Wifi and publishing mDNS service 'Nabby'
- Doorbell unit searching for all mDNS Nabby services called 'Nabby'
- communication between doorbell and Nabby via UDP
- Nabby contains command language
- From PC or mobile, via UDP Nabby can be commanded using commandlanguage
- For debugging Nabby can be commanded via command language vis serial (connected via USB)
- Commands support: ear control, LED control, MP3 player control, version readout etc.

The repository contains electronic schematic diagrams of Nabby and of the doorbell unit.

See wiki for pictures and details.

Nabby-tiny is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. Nabby-tiny is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with Nabby-tiny. If not, see <https://www.gnu.org/licenses/>.
