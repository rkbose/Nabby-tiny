#include <HardwareSerial.h>
#include <dfplayer.h>
#include "Parsers.h"

extern DFPlayer mp3;
String version;

void parsers(String versie)
{
  version = versie;
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