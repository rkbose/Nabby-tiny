// This file is part of Nabby-tiny.
// Nabby-tiny is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nabby-tiny is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nabby-tiny. If not, see <https://www.gnu.org/licenses/>.

#ifndef SERIALDATAPARSER_H
#define SERIALDATAPARSER_H

#include <stdlib.h>
#include <string.h>
#include <AsyncUDP.h>

#define BUFFER_SIZE 64

//typedef void (* ParserFunction)(char **values, int valueCount);
typedef String (* ParserFunction)(char **values, int valueCount);

typedef struct
{
  char *command;
  ParserFunction function;
} ParserFunctionLookup;

class DynamicCommandParser
{
public:
  DynamicCommandParser(char start, char end, char delim, bool udp)
  {
    mInCommand = false;
    mStart = start;
    mEnd = end;
    mDelimiter = delim;

    mParserLookup = NULL;
    mParserLookupSize = 0;
    buffer[0] = '\0';
//    if (udp == true) udppackets = true; else udppackets = false;
    udppackets = udp;
  }

  ~DynamicCommandParser()
  {
    free(mParserLookup);
  }

  void addParser(char *cmd, ParserFunction function);
  void append(char *str);
  void append(AsyncUDPPacket *pct);
  void appendChar(char c);
 // String getresult();

private:
  bool mInCommand;
  char buffer[BUFFER_SIZE];
  char mStart;
  char mEnd;
  char mDelimiter;
  AsyncUDPPacket *packet;
  bool udppackets; 

  size_t mParserLookupSize;
  ParserFunctionLookup *mParserLookup;

  void parseBuffer();
  int getBufferPartCount();
};

#endif
