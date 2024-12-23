// This file is part of Nabby-tiny.
// Nabby-tiny is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nabby-tiny is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nabby-tiny. If not, see <https://www.gnu.org/licenses/>.

#include <HardwareSerial.h>
#include <dfplayer.h>
#include "Parsers.h"
#include <AsyncUDP.h>
#include <Watchdog.h>
#include <Nabby.h>

extern DFPlayer mp3;
extern String version;

void printParserCommands(void)
{
  Serial.print("\n   ===> Commands:\n");
  Serial.print("      /inf      --- shows version\n");
  Serial.print("      /hlp      --- print commands\n");
  Serial.print("      /mvp,x,x  --- dummy command\n");
  Serial.print("      /tra,n    --- select track (0=stop)\n");
  Serial.print("      /all,x      --- play all tracks\n");
  Serial.print("      /vol,n    --- set volume\n");
  Serial.print("      /rng      --- play bell sound\n");
  Serial.print("      /twd      --- trigger watchdog");
}

/**************************************************************************/
/*
   Parser for multiplevariable commands. This is a dummy and test command currently

    NOTE:
    - On serial stream will print xx
    - On UDP stream: will return xx
*/
/**************************************************************************/
String multipleVariableParser(char **values, int valueCount, bool udppackets)
{
  Serial.println("   ===> multipleVariableParser:");
  for (int i = 1; i < valueCount; i++)
  {
    Serial.print("  values[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.print(values[i]);
    int jj;
    sscanf(values[i], "%d", &jj);
    Serial.printf("the value is %d\n", jj);
  }
  return ("MVP is done   ");
}

/**************************************************************************/
/*
    Parser for the getInfo command

    NOTE:
    - On serial stream will print the SW version
    - On UDP stream: will return SW version
*/
/**************************************************************************/
String getInfo(char **values, int valueCount, bool udppackets)
{
  char buffer[100];
  if (valueCount > 1)
    Serial.print("   ===> getInfo does not accept parameters.");
  else
  {
    if (!udppackets)
    {
      Serial.print("   ===> Software version Nabby-tiny: ");
      Serial.print(version);
    }
  }
  snprintf(buffer, 100, "Nabby-tiny Software version: {%s}   [INF done]", version.c_str());
  return (buffer);
}

// Parser printing help on commands
String printHelp(char **values, int valuecount, bool udppackets)
{
  printParserCommands();
  return ("HLP is done    ");
}

/**************************************************************************/
/*
    Parser for track selection. Requires one parameter: the selected track.

    NOTE:
    - On serial stream will print selected track
    - On UDP stream will command completed
*/
/**************************************************************************/
String selectTrack(char **values, int valueCount, bool udppackets)
{
  if (valueCount != 2)
  {
    if (!udppackets)
      Serial.print("   ===> selectTrack requires one parameter.");
  }
  else
  {
    int jj;
    sscanf(values[1], "%d", &jj);
    if (!udppackets)
      Serial.printf("   ===> Selected track: %d", jj);
    mp3.playTrack(jj);
    delay(500);
  }
  return ("[TRC done] ");
}

/**************************************************************************/
/*
    Parser for playing all tracks. Requires one parameter: 1: to enable, 0 to disbale.

    NOTE:
    - On serial stream will print command completed
    - On UDP stream will command completed
*/
/**************************************************************************/
String playAllTracks(char **values, int valueCount, bool udppackets)
{
  if (valueCount != 2)
  {
    Serial.print("   ===> select ALL requires one parameter");
  }
  else
  {
    int jj;
    sscanf(values[1], "%d", &jj);
    if (jj == 1)
      mp3.repeatAll(true);
    else if (jj == 0)
      mp3.repeatAll(false);
    if (!udppackets)
    {
      Serial.print("   ===> ALL track playing selected");
    }
  }
  return ("[ALL done] ");
}

/**************************************************************************/
/*
    Parser for volume setting. Requires one parameter: the volume 0..30.

    NOTE:
    - On serial stream will print command completed
    - On UDP stream will command completed
*/
/**************************************************************************/
String setVolume(char **values, int valueCount, bool udppackets)
{
  if (valueCount != 2)
  {
    if (!udppackets)
      Serial.print("   ===> setVolume requires one parameter.");
  }
  else
  {
    int jj;
    sscanf(values[1], "%d", &jj);
    if (!udppackets)
      Serial.printf("   ===> Volume set to: %d", jj);
    mp3.setVolume(jj); // 0..30, module persists volume on power failure
    delay(500);
  }
  return ("[VOL done] ");
}

/**************************************************************************/
/*
   Parser for playing doorbell sound.

    NOTE:
    - On serial stream will print xx
    - On UDP stream: will return xx
*/
/**************************************************************************/
String RingBell(char **values, int valueCount, bool udppackets)
{
  if (valueCount > 1)
    Serial.print("   ===> RingBell does not accept parameters.");
  else
  {
    if (!udppackets)
      Serial.printf("   ===> doorbell sound played");
    mp3.playTrack(9);
    LedNotificationCount = 30;  //30 secs led sequence
    delay(500);
  }
  return ("[RNG done] ");
}

/**************************************************************************/
/*
   Parser for ping. The doorbell unit sends frequent pings.
   On reception, the watchdog will be triggered.
*/
/**************************************************************************/
String Ping(char **values, int valueCount, bool udppackets)
{
  if (valueCount > 1)
    Serial.print("   ===> Ping does not accept parameters.");
  else
  {
    if (!udppackets)
      Serial.printf("   ===> watchdog triggered.");
  }
  Serial.print(" ping handled\n");
  wdTrigger();
  return ("[PNG done] ");
}
