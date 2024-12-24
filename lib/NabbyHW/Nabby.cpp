// This file is part of Nabby-tiny.
// Nabby-tiny is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nabby-tiny is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nabby-tiny. If not, see <https://www.gnu.org/licenses/>.
// Originally DynamicCommandparser was  Created by Mathias Djärv, June 31, 2014.
// And Released under Creative Commons Attribution 4.0 International (CC BY 4.0)    http://creativecommons.org/licenses/by/4.0/
//

#include <Nabby.h>
#include "Wire.h" //used for I2C communication with IO expander MCP23017
#include <dfplayer.h>

extern DFPlayer mp3;

// Left and Right motor pins and PWM definitions
#define MOTOR_L_IN1 27    // D27 is Left ear motor IN1
#define MOTOR_L_IN2 14    // D14 is Left ear motor IN2
#define MOTOR_R_IN1 12    // D12 is Right ear motor IN1
#define MOTOR_R_IN2 13    // D13 is Right ear motor IN2
#define PWM_LED_D23 0     // PWM channel LED on D23
#define PWM_MOTOR_L_IN1 1 // PWM channel Left ear motor IN1
#define PWM_MOTOR_L_IN2 2 // PWM channel Left ear motor IN2
#define PWM_MOTOR_R_IN1 3 // PWM channel Right ear motor IN1
#define PWM_MOTOR_R_IN2 4 // PWM channel Right ear motor IN2

// Left and Right Encoder pins
#define ENC_L 32 // left ear encoder input
#define ENC_R 33 // right ear encoder input

// setting PWM properties for heartbeat LED and PWM for left&right ear motors
const int freq = 15000;    // in Hz
const int pwmChannel0 = 0; // PWM channels: 0 to 15
const int resolution = 8;  // Resolution in bits: 8, 10, 12, 15

// Heartbeat LD and Nabby head button
#define LED_D23 23      // heartbeat LED on pin 23
#define NabbysButton 25 // Nabbys head button on pin 25

// volatiles for LED blinking and PWM channel
// volatile int toggle = 1;    // used for slow flashing led
volatile int prev_secs = 0;            // previous time readin from millis() for Heartbeat led
volatile int prev_secs_cLeds = 0;      // prev. time reading from millis() for colour leds
volatile int PWM = 0;                  // temp variable for duty cycle of LED
volatile int pwmsign = 20;             // incremental step for PWM of LED
volatile byte colourLow = 1;           // value for colour LED on PORT A
volatile byte colourHigh = 1;          // value for colour LED on PORT B
volatile int cledArrayindex = 0;       // index used in the const array with cled colours
volatile int LedNotificationCount = 0; // TRUE if Belly LED's are ON

// left Ear globals and definitions
volatile unsigned long leftEarLastTriggerTime = 0;
volatile unsigned long leftEarTriggerTime = 0;
volatile unsigned int leftEarState = 0; // states in statemachine
volatile unsigned int leftEarTargetPosition = 0;
volatile unsigned int leftEarPosition = 0;
volatile boolean leftEarPositionKnown = false;
#define LEFT_EAR_STATE_INIT 0    // Initialize state
#define LEFT_EAR_STATE_RUNNING 1 // running state: trying to maintain position

// right Ear globals and definitions
volatile unsigned long rightEarLastTriggerTime = 0;
volatile unsigned long rightEarTriggerTime = 0;
volatile unsigned int rightEarState = 0; // states in statemachine
volatile unsigned int rightEarTargetPosition = 0;
volatile unsigned int rightEarPosition = 0;
volatile boolean rightEarPositionKnown = false;
#define RIGHT_EAR_STATE_INIT 0    // Initialize state
#define RIGHT_EAR_STATE_RUNNING 1 // running state: trying to maintain position

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// on 7 jan 2023 the original Nabby code was compiled the first time on platformio/visual studio.
// Surprisingly following external definitions had to be added, which where not required in Arduino build env.
/*extern void rightEarGoto(unsigned int targetPosition, int speed); // new added on 7jan2023
extern void leftEarGoto(unsigned int targetPosition, int speed);  // new added on 7jan2023
extern void rightEarSetSpeed(int speed);                          // new added on 7jan2023
extern void leftEarSetSpeed(int speed);                           // new added on 7jan2023
extern void portMCP23017(byte valueA, byte valueB);               // new added on 7jan2023
*/

// initialize timers
hw_timer_t *leftEarTimer = NULL;  // timer used for leftEar statemachine
hw_timer_t *rightEarTimer = NULL; // timer used for rightEar statemachine

void leftEarSetSpeed(int speed)
{
  if (speed > 0)
  {
    if (speed > 255)
      speed = 255;
    ledcWrite(PWM_MOTOR_L_IN1, speed);
    ledcWrite(PWM_MOTOR_L_IN2, 0);
    //    Serial.printf("L-speed positive: %d \n", speed);
  }
  if (speed < 0)
  {
    if (speed < -255)
      speed = -255;
    ledcWrite(PWM_MOTOR_L_IN1, 0);
    ledcWrite(PWM_MOTOR_L_IN2, -1 * speed);
    //    Serial.printf("L-speed negative: %d \n", -1 * speed);
  }
}

void IRAM_ATTR leftEarEncoderInterrupt()
{
  // Debounch the encoder signal. Note Nabby's encoders output a triangle, not a square wave. This causes multiple interrupts.
  // The timer interrupts will mask the encoder interrupts for a short period. Better would have been a hardware schmitt trigger
  detachInterrupt(digitalPinToInterrupt(ENC_L)); // disable interrupt for debouncing
  timerAlarmWrite(leftEarTimer, 500000, false);  // true: reload, false: trigger one time
  timerWrite(leftEarTimer, 0);                   // reset timer
  timerAlarmEnable(leftEarTimer);

  leftEarTriggerTime = millis();

  if ((leftEarState == LEFT_EAR_STATE_RUNNING) and (leftEarTriggerTime - leftEarLastTriggerTime) > 1300)
  {
    leftEarPositionKnown = true;
    leftEarPosition = 0;
  }
  leftEarPosition++;
  //  Serial.printf("leftEarInt,  prev.state: %d, pos: %d, known: %d,  time: %d\n", leftEarState, leftEarPosition, leftEarPositionKnown, (leftEarTriggerTime - leftEarLastTriggerTime));
  leftEarLastTriggerTime = leftEarTriggerTime;
  if (leftEarPositionKnown and (leftEarPosition == leftEarTargetPosition))
  {
    leftEarSetSpeed(1);
    leftEarState = LEFT_EAR_STATE_INIT;
    leftEarPositionKnown = false;
  }
  else
    leftEarState = LEFT_EAR_STATE_RUNNING;
}

void IRAM_ATTR leftEarTimerInterrupt()
{
  //  Serial.println("leftEarTimerInt");
  attachInterrupt(digitalPinToInterrupt(ENC_L), leftEarEncoderInterrupt, RISING); // RISING, FALLING, HIGH
}

void leftEarInitialize()
{
  pinMode(ENC_L, INPUT); // A 10K external resistor is used as pull down. Enabling internal pullup would create divider
  leftEarState = LEFT_EAR_STATE_INIT;
  leftEarTargetPosition = 0;
  leftEarPosition = 0;
  leftEarPositionKnown = false;
  leftEarLastTriggerTime = millis();
  leftEarTimer = timerBegin(0, 80, true);                                         // initialize timer to timer 0; prescaler 80 (= 1Mhz); and TRUE (=cnt up)
  timerAttachInterrupt(leftEarTimer, &leftEarTimerInterrupt, true);               // interrupt function. TRUE (= edge type)
  attachInterrupt(digitalPinToInterrupt(ENC_L), leftEarEncoderInterrupt, RISING); // RISING, FALLING, HIGH, LOW
}

void leftEarGoto(unsigned int targetPosition, int speed)
{
  portENTER_CRITICAL(&mux);
  leftEarTargetPosition = targetPosition;
  portEXIT_CRITICAL(&mux);
  leftEarSetSpeed(speed);
}

void rightEarSetSpeed(int speed)
{
  if (speed > 0)
  {
    if (speed > 255)
      speed = 255;
    ledcWrite(PWM_MOTOR_R_IN1, speed);
    ledcWrite(PWM_MOTOR_R_IN2, 0);
    //    Serial.printf("R-speed positive: %d \n", speed);
  }
  if (speed < 0)
  {
    if (speed < -255)
      speed = -255;
    ledcWrite(PWM_MOTOR_R_IN1, 0);
    ledcWrite(PWM_MOTOR_R_IN2, -1 * speed);
    //    Serial.printf("R-speed negative: %d \n", -1 * speed);
  }
}

void IRAM_ATTR rightEarEncoderInterrupt()
{
  // Debounch the encoder signal. Note Nabby's encoders output a triangle, not a square wave. This causes multiple interrupts.
  // The timer interrupts will mask the encoder interrupts for a short period. Better would have been a hardware schmitt trigger
  detachInterrupt(digitalPinToInterrupt(ENC_R)); // disable interrupt for debouncing
  timerAlarmWrite(rightEarTimer, 500000, false); // true: reload, false: trigger one time
  timerWrite(rightEarTimer, 0);                  // reset timer
  timerAlarmEnable(rightEarTimer);
  rightEarTriggerTime = millis();
  if ((rightEarState == RIGHT_EAR_STATE_RUNNING) and (rightEarTriggerTime - rightEarLastTriggerTime) > 1300)
  {
    rightEarPositionKnown = true;
    rightEarPosition = 0;
  }
  rightEarPosition++;
  //  Serial.printf("rightEarInt,  prev.state: %d, pos: %d, known: %d,  time: %d\n", rightEarState, rightEarPosition, rightEarPositionKnown, (rightEarTriggerTime - rightEarLastTriggerTime));
  rightEarLastTriggerTime = rightEarTriggerTime;
  if (rightEarPositionKnown and (rightEarPosition == rightEarTargetPosition))
  {
    rightEarSetSpeed(1);
    rightEarState = RIGHT_EAR_STATE_INIT;
    rightEarPositionKnown = false;
  }
  else
    rightEarState = RIGHT_EAR_STATE_RUNNING;
}

void IRAM_ATTR rightEarTimerInterrupt()
{
  //  Serial.println("rightEarTimerInt");
  attachInterrupt(digitalPinToInterrupt(ENC_R), rightEarEncoderInterrupt, RISING); // RISING, FALLING, HIGH
}

void rightEarInitialize()
{
  pinMode(ENC_R, INPUT); // A 10K external resistor is used as pull down. Enabling internal pullup would create divider
  rightEarState = RIGHT_EAR_STATE_INIT;
  rightEarTargetPosition = 0;
  rightEarPosition = 0;
  rightEarPositionKnown = false;
  rightEarLastTriggerTime = millis();
  rightEarTimer = timerBegin(1, 80, true);                                         // initialize timer to timer 1; prescaler 80 (= 1Mhz); and TRUE (=cnt up)
  timerAttachInterrupt(rightEarTimer, &rightEarTimerInterrupt, true);              // interrupt function. TRUE (= edge type)
  attachInterrupt(digitalPinToInterrupt(ENC_R), rightEarEncoderInterrupt, RISING); // RISING, FALLING, HIGH, LOW
}

void rightEarGoto(unsigned int targetPosition, int speed)
{
  portENTER_CRITICAL(&mux);
  rightEarTargetPosition = targetPosition;
  portEXIT_CRITICAL(&mux);
  rightEarSetSpeed(speed);
}

void initMCP23017(void)
{
  Wire.begin(21, 22);
  // set IO pins to outputs
  Wire.beginTransmission(0x20); // with three address pins @gnd, address is 0x20
  Wire.write(0x00);             // IODIRA register
  Wire.write(0x00);             // set all ports on port A to outputs
  Wire.endTransmission();
  Wire.beginTransmission(0x20); // with three address pins @gnd, address is 0x20
  Wire.write(0x01);             // IODIRB register
  Wire.write(0x00);             // set all ports on port B to outputs
  Wire.endTransmission();
}

void portMCP23017(byte valueA, byte valueB)
{
  Wire.beginTransmission(0x20);
  Wire.write(0x12); // GPIOA
  Wire.write(valueA);
  Wire.endTransmission();
  Wire.beginTransmission(0x20);
  Wire.write(0x13); // GPIOB
  Wire.write(valueB);
  Wire.endTransmission();
}
int mappingHeartbeat(int x)
{ // optional non linear heartbeat LED light intensity
  // return (x*x*x/(255^3));
  return (x);
}

// LED sequence to be played (for low byte on IO expander: GPA)
// A0, A1, A2: G,R,B - Belly
// A3, A4, A5: G, R, B - LEFT
// A6, A7: G, R - MID
#define lLG B00001000 // LeftGreen
#define lMG B01000000 // MidGreen
#define lRG B00000000 // RightGreen off on lowbyte
#define lBG B00000001 // BellyGreen, not connected
#define lLR B00010000 // LeftRed
#define lMR B10000000 // MidRed
#define lRR B00000000 // RightRed off on lowbyte
#define lBR B00000010 // BellyRed, not connected
#define lLB B00100000 // LeftBlue
#define lMB B00000000 // MidBlue off on lowbyte
#define lRB B00000000 // RightBlue, not connected
#define lBB B00000100 // BellyBlue, not connected
#define l00 B00000000 // OFF

const byte cledLowArray[] = {
    lLG | lMG | lRG, // 0
    lLG | lMG | lRG, // 1
    lLG | lMG | lRG, // 2
    lLG | lMG | lRG, // 3
    lLG | lMG | lRG, // 4
    lLG | lMG | lRG, // 5
    lLG | lMG | lRG, // 6
    lLG | lMG | lRG, // 7
    lLG | lMG | lRG  // 8
};

// LED sequence to be played (for High byte on IO expander: GPB)
// B0: B - MID
// B1, B2, B3: G,R,B - RIGHT
// B4, B5, B6: G,R,B - TOP
#define hLB B00000000 // LeftBlue off on highbyte
#define hMB B00000001 // MidBlue
#define hRB B00001000 // RightBlue
#define hLG B00000000 // LeftGreen off on highbyte
#define hMG B00000000 // MidGreen off on highbyte
#define hRG B00000010 // RightGreen
#define hLR B00000000 // LeftRed off on highbyte
#define hMR B00000000 // MidRed off on highbyte
#define hRR B00000100 // RightRed
#define hTG B00010000 // TopGreen
#define hTR B00100000 // TopRed
#define hTB B01000000 // TopBlue
#define h00 B00000000 // OFF

const byte cledHighArray[] = {

    hLG | hMG | hRG | hTB, // 0
    hLG | hMG | hRG | hTB | hTR, // 1
    hLG | hMG | hRG | hTB, // 2
    hLG | hMG | hRG | hTB | hTR, // 3
    hLG | hMG | hRG | hTB, // 4
    hLG | hMG | hRG | hTB | hTR, // 5
    hLG | hMG | hRG | hTB, // 6
    hLG | hMG | hRG | hTB | hTR, // 7
    hLG | hMG | hRG | hTB  // 8
};
/*
const byte cledLowArray[] = {
    // LED sequence to be played (for low byte on IO expander: GPA)
    // A0, A1, A2: G,R,B - Belly
    // A3, A4, A5: G, R, B - LEFT
    // A6, A7: G, R - MID
    B00001000, // 0  LG
    B01000010, // 1  MG
    B00000000, // 2  RG
    // B00000010, // 3 OFF
    B00010000, // 4  LR
    B10000010, // 5  MR
    B00000000, // 6  RR
    // B00000010, // 7 OFF
    B00100000, // 8  LB
    B00000010, // 9  MB
    B00000000  // 10  RB
               // B00000000  // 11  OFF
};

const byte cledHighArray[] = {
    // LED sequence to be played (for High byte on IO expander: GPB)
    // B0: B - MID
    // B1, B2, B3: G,R,B - RIGHT
    // B4, B5, B6: G,R,B - TOP
    B00100000, // 0   LG
    B00100000, // 1   MG
    B00100010, // 2   RG
    //  B00100000, // 3   OFF
    B00100000, // 4   LR
    B00100000, // 5   MR
    B00100100, // 6   RR
    // B00100000, // 7   OFF
    B00100000, // 8   LB
    B00100001, // 9   MB
    B00101000  // 10   RB
               // B00000000 // 11  OFF
};*/
void blinkLeds(void)
{ // time based routine for LED blinking and Nabbys button polling
  int secs = millis();

  if (secs > (prev_secs + 15))
  { // each 15 mSec the heartbeat Led changes brightness
    prev_secs = secs;
    PWM = PWM + pwmsign;
    if (PWM > 150)
    {
      pwmsign = -2;
      PWM = 150;
    }
    if (PWM < 0)
    {
      pwmsign = 2;
      PWM = 0;
    }
    ledcWrite(PWM_LED_D23, mappingHeartbeat(PWM)); // heartbeat LED
  }

  if ((LedNotificationCount > 0) and (secs > (prev_secs_cLeds + 350)))
  { // each 350 mSec the Leds change
    prev_secs_cLeds = secs;
    LedNotificationCount--;
    if (LedNotificationCount < 0)
      LedNotificationCount = 0;

    cledArrayindex++;
    if (cledArrayindex > sizeof(cledLowArray) - 1)
      cledArrayindex = 0;
    colourLow = cledLowArray[cledArrayindex];
    colourHigh = cledHighArray[cledArrayindex];
    portMCP23017(colourLow, colourHigh);
  }
  if (LedNotificationCount == 1) // on 1 the leds are turned off. Only 1 time, because of audible sound clicks
  {
    portMCP23017(0, 0);
    LedNotificationCount = 0;
    cledArrayindex = 0;
  }
  if (digitalRead(NabbysButton) == 0)
  {
    mp3.stop();
    LedNotificationCount = 2;
  }
}

void initNabby(void)
{
  pinMode(2, OUTPUT);                  // enable on-board led
  pinMode(LED_D23, OUTPUT);            // enable external LED on D23
  pinMode(NabbysButton, INPUT_PULLUP); // Nabbys head button

  initMCP23017(); // initialize I2C bus for MCP23017 IO expander

  // make motordriver pins output
  pinMode(MOTOR_L_IN1, OUTPUT);
  pinMode(MOTOR_L_IN2, OUTPUT);
  pinMode(MOTOR_R_IN1, OUTPUT);
  pinMode(MOTOR_R_IN2, OUTPUT);

  // initialize PWM channels for led and motorpins
  ledcSetup(PWM_LED_D23, freq, resolution);
  ledcAttachPin(LED_D23, PWM_LED_D23); // attach the channel to the GPIO2 to be controlled

  ledcSetup(PWM_MOTOR_L_IN1, freq, resolution);
  ledcAttachPin(MOTOR_L_IN1, PWM_MOTOR_L_IN1); // attach the channel to the GPIO

  ledcSetup(PWM_MOTOR_L_IN2, freq, resolution);
  ledcAttachPin(MOTOR_L_IN2, PWM_MOTOR_L_IN2); // attach the channel to the GPIO

  ledcSetup(PWM_MOTOR_R_IN1, freq, resolution);
  ledcAttachPin(MOTOR_R_IN1, PWM_MOTOR_R_IN1); // attach the channel to the GPIO

  ledcSetup(PWM_MOTOR_R_IN2, freq, resolution);
  ledcAttachPin(MOTOR_R_IN2, PWM_MOTOR_R_IN2); // attach the channel to the GPIO

  //  timerAlarmWrite(leftEarTimer, 5000000, false);  // TRUE = reload
  //  timerAlarmEnable(leftEarTimer);
  leftEarInitialize();
  rightEarInitialize();
  leftEarGoto(3, -200);
  rightEarGoto(3, 200);
  delay(12000);
  leftEarSetSpeed(1);
  rightEarSetSpeed(1);
}
