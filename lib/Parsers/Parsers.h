// This file is part of Nabby-tiny.
// Nabby-tiny is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nabby-tiny is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nabby-tiny. If not, see <https://www.gnu.org/licenses/>.

#ifndef PARSERS_h
#define PARSERS_h

//parser commands for serial connection:
void printParserCommands(void);
String multipleVariableParser(char **values, int valueCount);
String getInfo(char **values, int valueCount);
String printHelp(char **values, int valuecount);
String selectTrack(char **values, int valueCount);
String setVolume(char **values, int valueCount);

//parser commands for UDP connection:
String getInfo_udp(char **values, int valueCount);

#endif
