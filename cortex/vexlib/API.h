#ifndef __API_H
#define __API_H

#include <stdio.h>

void IO_Initialization(void);
void DefineControllerIO(unsigned char ucP1, unsigned char ucP2, unsigned char ucP3, unsigned char ucP4,
						unsigned char ucP5, unsigned char ucP6, unsigned char ucP7, unsigned char ucP8,
						unsigned char ucP9, unsigned char ucP10, unsigned char ucP11, unsigned char ucP12);

void SetCompetitionMode(int ucCompTime, int ucOperTime);

unsigned char GetDigitalInput(unsigned char ucPort);
void SetDigitalOutput(unsigned char ucPort, unsigned char ucValue);
unsigned int GetAnalogInput(unsigned char ucPort);

void StartEncoder(unsigned char ucPort);
void StopEncoder(unsigned char ucPort);
long GetEncoder(unsigned char ucPort);
void PresetEncoder(unsigned char ucPort, long lPresetValue);

void StartQuadEncoder(unsigned char ucChannelA, unsigned char ucChannelB, unsigned char ucInvert);
void StopQuadEncoder(unsigned char ucChannelA, unsigned char ucChannelB);
long GetQuadEncoder(unsigned char ucChannelA, unsigned char ucChannelB);
void PresetQuadEncoder(unsigned char ucChannelA, unsigned char ucChannelB, long lPresetValue);

void StartInterruptWatcher(unsigned char port, unsigned char direction);
void StopInterruptWatcher(unsigned char port);
unsigned char GetInterruptWatcher(unsigned char port);

void SetMotor(unsigned char ucMotor, int iSpeed);
void SetServo(unsigned char ucMotor, int iSpeed);

void Arcade2(unsigned char ucJoystick,
                unsigned char ucMoveChannel, unsigned char ucRotateChannel,
                unsigned char ucLeftMotor, unsigned char ucRightMotor,
                unsigned char ucLeftInvert, unsigned char ucRightInvert);              
void Arcade4(unsigned char ucJoystick,
            unsigned char ucMoveChannel, unsigned char ucRotateChannel,
            unsigned char ucLeftfrontMotor, unsigned char ucRightfrontMotor,
            unsigned char ucLeftrearMotor, unsigned char ucRightrearMotor,
            unsigned char ucLeftfrontInvert, unsigned char ucRightfrontInvert,
            unsigned char ucLeftrearInvert, unsigned char ucRightrearInvert);
void Tank2(unsigned char ucJoystick,
            unsigned char ucLeftChannel, unsigned char ucRightChannel,
            unsigned char ucLeftMotor, unsigned char ucRightMotor,
            unsigned char ucLeftInvert, unsigned char ucRightInvert);
void Tank4(unsigned char ucJoystick,
            unsigned char ucLeftChannel, unsigned char ucRightChannel,
            unsigned char ucLeftfrontMotor, unsigned char ucRightfrontMotor,
            unsigned char ucLeftrearMotor, unsigned char ucRightrearMotor,
            unsigned char ucLeftfrontInvert, unsigned char ucRightfrontInvert,
            unsigned char ucLeftrearInvert, unsigned char ucRightrearInvert);                  
void JoystickToMotor(unsigned char ucJoystick, unsigned char ucChannel, unsigned char ucMotor, unsigned char ucInv);
void JoystickToMotorAndLimit(unsigned char ucJoystick, unsigned char ucChannel, unsigned char ucMotor, unsigned char ucInv, unsigned char ucPositiveLimitSwitch, unsigned char ucNegativeLimitSwitch);
void JoystickToServo(unsigned char ucJoystick, unsigned char ucChannel, unsigned char ucMotor, unsigned char ucInv);
void JoystickToDigitalOutput(unsigned char ucJoystick, unsigned char ucChannel, unsigned char ucButton, unsigned char ucDout);
int GetJoystickAnalog(unsigned char ucJoystick, unsigned char ucChannel);
unsigned char GetJoystickDigital(unsigned char ucJoystick, unsigned char ucChannel, unsigned char ucButton);
int GetJoystickAccelerometer(unsigned char ucJoystick, unsigned char ucAxis);
void GetJoystickAccelerometerEx(unsigned char ucJoystick, int *pnAxisX, int *pnAxisY);

void PrintToScreen(const char *fmt, ...);
            
void Wait(unsigned long ulTime);
unsigned long GetMsClock(void);

void StartTimer(unsigned char ucTimerNumber);
void StopTimer(unsigned char ucTimerNumber);
void PresetTimer(unsigned char ucTimerNumber, unsigned long ulValue);
unsigned long GetTimer(unsigned char ucTimerNumber);

unsigned int GetUltrasonic(unsigned char ucEcho, unsigned char ucPing);
void StartUltrasonic(unsigned char ucEcho, unsigned char ucPing);
void StopUltrasonic(unsigned char ucEcho, unsigned char ucPing);

void ResetGD(void);
void ClearGD(unsigned char ucRow1, unsigned char ucCol1, unsigned char ucRow2, unsigned char ucCol2, unsigned char ucFrame);
void PrintTextToGD(unsigned char ucRow, unsigned char ucCol, unsigned long ulColor, const char *szText, ...);
void PrintFrameToGD(unsigned char ucRow1, unsigned char ucCol1, unsigned char ucRow2, unsigned char ucCol2, unsigned long ulColor);

float GetMainBattery();
float GetBackupBattery();

unsigned int GetRandomNumber(int nMin, int nMax);

void InitLCD(unsigned char ucPort);
unsigned char SetLCDText(unsigned char ucPort, unsigned char nLine, const char *szMsg, ...);
unsigned char SetLCDLight(unsigned char ucPort, unsigned char nLight);
void StartLCDButtonsWatcher(unsigned char ucPort);
void StopLCDButtonsWatcher(unsigned char ucPort);
void GetLCDButtonsWatcher(unsigned char ucPort, unsigned char *b1, unsigned char *b2, unsigned char *b3);

void InitAccelerometer(unsigned char ucPort);
void StartAccelerometer(unsigned char ucPort);
int GetAcceleration(unsigned char ucPort);
void StopAccelerometer(unsigned char ucPort);

void SetDebugInfo(unsigned char ucIsOn);

#endif