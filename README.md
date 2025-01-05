Nabaztag-messenger Nabby a Nabaztag rabbit based messenger Project
created by Raj Bose, July 2020

This project was created to convert an old non functioning NABAZTAG rabbit into a messaging system connected on Wifi.
Main funtion is a wifi connected doorbell unit. The Nabbaztag is nicknamed "Nabby". 
It is an ESP32 based project. The repo 'Nabby repo' contains the doorbell unit design which is connected to the doorbell. 
The repo xxx contains an Android app design which can be used as remote control for Nabby.

Nabby is fully functional with following features:
- mDNS (multicast) based service discovery on WiFi Expandable: Nabby and doorbellunit support multiple Nabby's and multible doorbell units
- MP3 player with SD-Card for sounds, music and speech (KT403A based).
- Left and right ear positioning using native Nabaztag encoders
- Top button to kill sound
- RGB LED's connected via I2C IO expander
- Seperate doorbell unit connecting to Wifi with TFT status display and animation (see repository Nabby-tiny-doorbell)
- Remote control via Android APP created in Android Studio (see repository Nabby-ABI)
- Nabaztag (called Nabby) connecting to Wifi and publishing mDNS service 'Nabby'
- Doorbell unit searching for all mDNS Nabby services called 'Nabby'. communication between doorbell and Nabby via UDP
- From PC or mobile, via UDP Nabby can be commanded using a command language via a parser class
- For debugging Nabby can be commanded via command language via serial commands (connected via USB)
- Commands support: ear control, LED control, MP3 player control, version readout, playing music etc.
- Watchdog class to detect loss of connection between doorbell and Nabby

The repository contains electronic schematic diagrams of Nabby and of the doorbell unit.

Nabby-tiny is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. Nabby-tiny is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with Nabby-tiny. If not, see https://www.gnu.org/licenses/.
