/*
  ESPFirmware - This Library can be used for Recive and Send Signals to
  Insta Funkbus devices
  Copyright (c) 2018 R.Thomitzek.  All right reserved.

  Project home: https://github.com/Thro42/InstaFunkbus/
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with library. If not, see <http://www.gnu.org/licenses/>
*/

#ifndef _INSTAFUNKBUS_H
#define _INSTAFUNKBUS_H

#include <stdint.h>
#include <Arduino.h>

#define PULS_START 3860
#define PULS_PAUSE 55
#define PULS_SHORT 499
#define PULS_LONG PULS_SHORT + PULS_SHORT
#define PULS_STATE_0 '0'
#define PULS_STATE_1 '1'
#define PULS_MIN_LENGTH_1 700
#define PULS_MIN_LENGTH_0 940

#define RCSWITCH_MAX_CHANGES 100

class InstaFunkbus {
public:
  InstaFunkbus();
  void enableSending(int sPin);
  void enableSending(int sPin, char* serial);
  void enableSending(int sPin, char* sendtype, char* subtype, char* serial);
  void setSenderType(char* sendtype, char* subtype);
  void setState(char* state);
  void SendSwitchOn(char Chanel, int Switch);
  void SendSwitchOnLong(char Chanel, int Switch);
  void SendSwitchOff(char Chanel, int Switch);
  void SendSwitchOffLong(char Chanel, int Switch);
  void SendPuls(char state);
  void loop();

private:
  int nSenderPin;
  int pulsState;
  int lastState;
  void SendSync();
  void ToglePuls();
  void FillBit(char sbit);
  void FillByte(char* sbyte);
  void FillByte(String sByte);
  void FillSerial(char* sbyte);
  void FillSerial();
  void FillSwitchBit(int Switch);
  void FillChanel(char Chanel);
  void FillParity();
  void FillCRC(char Chanel, int Switch);
  void Fill2Chanel(char Chanel, int Switch);
  void FillState(bool State);
  void FillLength(int Sequenz);
  void SendBuffer();
  void decodeTiming();

  char _sendtype[5];
  char _subtype[5];
  char _serial[17];
  char _state[5];
  char _data[17];
  char _crc[5];
  char _sendBuff[55];
  int _buffPos;

/* Recive Funktion*/
    int nReceiverPin;
    unsigned long lastTime;
    int sync;
    int bitPos;
    char _recBuff[100];
    unsigned int timings[RCSWITCH_MAX_CHANGES];
};

#endif
