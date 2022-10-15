// This file is part of Nabby-tiny.
// Nabby-tiny is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nabby-tiny is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nabby-tiny. If not, see <https://www.gnu.org/licenses/>.

#include <HardwareSerial.h>
#include <dfplayer.h>
#include "Parsers.h"

extern DFPlayer mp3;
extern String version;

void printParserCommands(void) {
Serial.print("\n   ===> Commands:\n");
  Serial.print("      /inf      --- shows version\n");
  Serial.print("      /hlp      --- print commands\n");
  Serial.print("      /mvp,x,x  --- dummy command\n");
  Serial.print("      /tra,n    --- select track\n");
  Serial.print("      /vol,n    --- set volume\n");
}

// Parser for the MVP command
void multipleVariableParser(char **values, int valueCount)
{
  Serial.println("   ===> multipleVariableParser:");
  for (int i = 1; i < valueCount; i++)
  {
    Serial.print("  values[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.println(values[i]);
    int jj;
    sscanf(values[i], "%d", &jj);
    Serial.printf("the value is %d\n", jj);
  }
}

// Parser for the getInfo command
void getInfo(char **values, int valueCount)
{
  if (valueCount > 1)
    Serial.println("   ===> getInfo does not accept parameters.");
  else
  {
    Serial.print("   ===> Software version Nabby-tiny: ");
    Serial.print(version);
  }
}

// Parser printing help on commands
void printHelp(char **values, int valuecount)
{
printParserCommands();
}

// Parser for track selection
void selectTrack(char **values, int valueCount)
{
  if (valueCount != 2)
    Serial.print("   ===> selectTrack requires one parameter.");
  else
  {
    int jj;
    sscanf(values[1], "%d", &jj);
    Serial.printf("   ===> Selected track: %d", jj);
    mp3.playTrack(jj);
    delay(500);
  }
}

// Parser for the setVolume command
void setVolume(char **values, int valueCount)
{
  if (valueCount != 2)
    Serial.println("   ===> setVolume requires one parameter.");
  else
  {
    int jj;
    sscanf(values[1], "%d", &jj);
    Serial.printf("   ===> Volume set to: %d", jj);
    mp3.setVolume(jj); // 0..30, module persists volume on power failure
    delay(500);
  }
}