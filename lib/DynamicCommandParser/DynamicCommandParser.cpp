// This file is part of Nabby-tiny.
// Nabby-tiny is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nabby-tiny is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nabby-tiny. If not, see <https://www.gnu.org/licenses/>.
// Originally DynamicCommandparser was  Created by Mathias Djärv, June 31, 2014.
// And Released under Creative Commons Attribution 4.0 International (CC BY 4.0)    http://creativecommons.org/licenses/by/4.0/
//

#include <HardwareSerial.h>
#include "DynamicCommandParser.h"
#include <string.h>
#include <AsyncUDP.h>

void DynamicCommandParser::addParser(char *cmd, ParserFunction function)
{
  mParserLookupSize++;
  mParserLookup = (ParserFunctionLookup *)realloc(mParserLookup, (mParserLookupSize) * sizeof(ParserFunctionLookup));
  mParserLookup[mParserLookupSize - 1].command = cmd;
  mParserLookup[mParserLookupSize - 1].function = function;
}

void DynamicCommandParser::append(AsyncUDPPacket *pct)
{
  packet = pct;
  udppackets = true;
  append((char *)packet->data());
}

void DynamicCommandParser::append(char *str)
{
  for (size_t i = 0; i < strlen(str); i++)
  {
    appendChar(str[i]);
  }
}

void DynamicCommandParser::appendChar(char c)
{
  size_t bufferLength = strlen(buffer);

  if (c == mStart)
  {
    mInCommand = true;
    buffer[0] = 0;
    return;
  }
  else if (c == mEnd)
  {
    parseBuffer();
    buffer[0] = '\0';
    mInCommand = false;
  }
  else if (mInCommand)
  {
    buffer[bufferLength] = c;
    buffer[bufferLength + 1] = '\0';
  }
}

void DynamicCommandParser::parseBuffer()
{
  // Split buffer
  int partCount = getBufferPartCount();
  char **parts = (char **)malloc(partCount * sizeof(char *));

  parts[0] = buffer;
  int currentPart = 0;

  for (int i = 0; buffer[i] != 0; i++)
  {
    if (buffer[i] == mDelimiter)
    {
      buffer[i] = 0;
      currentPart++;
      parts[currentPart] = &buffer[i + 1];
    }
  }

  String result = "";

  for (size_t i = 0; i < mParserLookupSize; i++)
  {
    if (strcmp(mParserLookup[i].command, parts[0]) == 0)
    {
      result = mParserLookup[i].function(parts, partCount, udppackets);
      if (udppackets)
      {
        packet->print(result);
        udppackets = false;
      } // return result in UDP packet
      break;
    }
  }
  free(parts);
}

int DynamicCommandParser::getBufferPartCount()
{
  int count = 1;
  for (size_t i = 0; i < strlen(buffer); i++)
  {
    if (buffer[i] == mDelimiter)
      count++;
  }
  return count;
}
/*
String DynamicCommandParser::getresult()
{
return response;
}
*/
