//
// Nabby-tiny ESP32 based by RKB
// Board used: ESP32Dev module. Baudrate: 115200
// (COM4 on ACER and HP, right-usb-poort; not all usb-cables are OK)
//
// Copyright 2022, Raj Bose
//
// This file is part of Nabby-tiny.
// Nabby-tiny is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nabby-tiny is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nabby-tiny. If not, see <https://www.gnu.org/licenses/>.
//.
     
#define VERSION "23September2022a" 
#include <HardwareSerial.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <Secret.h>
//#include <AsyncUDP.h>
#include <ESPmDNS.h>
#include <dfplayer.h>
#include <DynamicCommandParser.h>
#include <Parsers.h>


#define MP3_SERIAL_SPEED 9600  // DFPlayer Mini suport only 9600-baud
#define MP3_SERIAL_TIMEOUT 100 // average DFPlayer response timeout 100msec..200msec
DFPlayer mp3;                  // connect DFPlayer RX-pin to GPIO15(TX) & DFPlayer TX-pin to GPIO13(RX)
uint8_t response = 0;
#define RXD2 16
#define TXD2 17


WiFiMulti wifiMulti;

void connectWifi()
{
  WiFi.mode(WIFI_STA); // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.disconnect();
  delay(100);

  int n = WiFi.scanNetworks(); // WiFi.scanNetworks will return the number of networks found
  Serial.println("scan done");
  if (n == 0)
  {
    Serial.println("no networks found");
  }
  else
  {
    Serial.printf("Nr networks found: %d\n", n);
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }
  //  wifiMulti.addAP(SSID3, PSW3);
  wifiMulti.addAP(SSID1, PSW1);
  wifiMulti.addAP(SSID2, PSW2);

  Serial.print("Connecting Wifi -");
  while (wifiMulti.run() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to network");

  Serial.print("WiFi connected: ");
  Serial.print(WiFi.SSID());
  Serial.print(";   IP addr: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("Nabby-mini"))
  {
    MDNS.addService("Nabby-mini", "udp", 1234); // Announce service on port x
    Serial.println("MDNS responder started");
  }
  Serial.printf("UDP server on port %d\n", 1234);
}

<<<<<<< HEAD
<<<<<<< HEAD
void loop() {
  // put your main code here, to run repeatedly:.....
}
=======
=======
>>>>>>> e8244c2920c8810f37e042bb84051796103c5db7
String levelString = "xxxxxxxxxxxxxx";

// Initialize the data parser using the start, end and delimiting character
DynamicCommandParser dcp('/', 0x0D, ',');


void setup()
{
  Serial.begin(115200); // debug interface
                        //  Serial2.begin(9600, SERIAL_8N1, 16, 17);  // MP3 interface
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  Serial.print("\nNabby-tiny is starting\n");
 
//  Serial.println("Serial2 Txd is on pin: " + String(TXD2));
//  Serial.println("Serial2 Rxd is on pin: " + String(RXD2));
  Serial.print("Commands:\n");
  Serial.print("  /inf      --- shows version\n");
  Serial.print("  /mvp,x,x  --- dummy command\n");
  Serial.print("  /tra,n    --- select track\n");
  Serial.print("  /vol,n    --- set volume\n");

  mp3.begin(Serial2, MP3_SERIAL_TIMEOUT, DFPLAYER_MINI, false); // DFPLAYER_MINI see NOTE, false=no response from module after the command

  delay(2000);
  mp3.stop(); // if player was runing during ESP8266 reboot
  delay(100);
  mp3.reset(); // reset all setting to default
  delay(100);
  mp3.setSource(2); // 1=USB-Disk, 2=TF-Card, 3=Aux, 4=Sleep, 5=NOR Flash
 // delay(500);
  mp3.wakeup(2); // exit standby mode & initialize sourse 1=USB-Disk, 2=TF-Card, 3=Aux, 5=NOR Flash
  delay(500);
  mp3.setVolume(15); // 0..30, module persists volume on power failure
 // delay(500);
  mp3.playTrack(2); // play track #1, donâ€™t copy 0003.mp3 and then 0001.mp3, because 0003.mp3 will be played firts
 // delay(500);

  // Serial2.begin(9600, SERIAL_8N1, 16, 17);  // MP3 interface

  // connectWifi();  // connect to WiFi access point

  //make VERSION available in parsers
  parsers(VERSION);
  // Add the parser commands to the DynamicCommandParser
  dcp.addParser("inf", getInfo);
  dcp.addParser("mvp", multipleVariableParser);
  dcp.addParser("tra", selectTrack);
  dcp.addParser("vol", setVolume);

  Serial.println("end of setup()");
}

int i = 0;
char c;
void loop()
{
  // Serial2.print(".");
  i++;

  // Serial.printf("i: %d\n", i);
  while (Serial.available() > 0)
  {
    c = Serial.read();
    Serial.print(c);
    dcp.appendChar(c);
  }
}
<<<<<<< HEAD
>>>>>>> 122f9e8 (First commit)
=======
>>>>>>> e8244c2920c8810f37e042bb84051796103c5db7
