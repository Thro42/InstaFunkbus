#include "InstaFunkbus.h"

InstaFunkbus::InstaFunkbus() {
  this->nSenderPin = 3;
  this->pulsState = 0;
  String("0010").toCharArray(this->_sendtype, 4);
  String("1100").toCharArray(this->_subtype, 4);
  String("1010").toCharArray(this->_state, 4);

}

void InstaFunkbus::ToglePuls() {
  if (this->pulsState == 0) {
    this->pulsState = 1;
    digitalWrite(this->nSenderPin, HIGH);
  } else {
    this->pulsState = 0;
    digitalWrite(this->nSenderPin, LOW);
  }
}

void InstaFunkbus::SendPuls(char state) {
  // Wechseln des Ausgangs
  this->ToglePuls();
  // Warten abhÃ¤ngig von 1 oder 0
  switch (state) {
    case PULS_STATE_0:
      delayMicroseconds( PULS_LONG );
      break;
    case PULS_STATE_1:
      delayMicroseconds( PULS_SHORT );
      // Im 1 Fall muss ich noch mahl Togeln
      ToglePuls();
      delayMicroseconds( PULS_SHORT );
      break;
  }
}

void InstaFunkbus::SendSync() {
  // Sync Puls
  digitalWrite(this->nSenderPin, HIGH);
  delayMicroseconds( PULS_START );
  this->pulsState = 1;
}

void InstaFunkbus::SendBuffer() {
  char sbit;
  Serial.println(_sendBuff);
  this->SendSync();
  for ( int i = 0; i < 50; i++) {
    sbit = _sendBuff[i];
    if (sbit != 0)
      SendPuls(sbit);
  }
  digitalWrite(this->nSenderPin, LOW);
}

void InstaFunkbus::setSenderType(char* sendtype, char* subtype) {
  strcpy(this->_sendtype, sendtype);
  strcpy(this->_subtype, subtype);
}

void InstaFunkbus::setState(char* state) {
  strcpy(this->_state, state);
}

void InstaFunkbus::enableSending(int sPin) {
  this->nSenderPin = sPin;
  pinMode(this->nSenderPin , OUTPUT);
  Serial.print(" pin ");
  Serial.println(nSenderPin);
}

void InstaFunkbus::enableSending(int sPin, char* serial) {
  strcpy(this->_serial, serial);
  Serial.print(" Serial ");
  Serial.println(_serial);
  this->enableSending(sPin);
}

void InstaFunkbus::enableSending(int sPin, char* sendtype, char* subtype, char* serial) {
  Serial.println(" enableSending ");
  this->enableSending(sPin, serial);
  strcpy(this->_sendtype, sendtype);
  strcpy(this->_subtype, subtype);
}

void InstaFunkbus::FillBit(char sBit) {
  //  Serial.print(sBit);
  _sendBuff[_buffPos] = sBit;
  _buffPos++;
}

void InstaFunkbus::FillByte(char* sByte) {
  char sbit;
  for ( int i = 0; i < 4; i++) {
    sbit = sByte[i];
    FillBit(sbit);
  }
}

void InstaFunkbus::FillByte(String sByte) {
  char sbit;
  for ( int i = 0; i < 4; i++) {
    sbit = sByte[i];
    FillBit(sbit);
  }
}

void InstaFunkbus::FillSerial(char* sByte) {
  char sbit;
  for ( int i = 0; i < 16; i++) {
    sbit = sByte[i];
    FillBit(sbit);
  }
}

void InstaFunkbus::FillSerial() {
  FillSerial(_serial);
}

void InstaFunkbus::FillChanel(char Chanel) {
  String chanelByte[] = {"000", "100", "010", "110"};
  String chByte;
  switch (Chanel) {
    case 'A':
      chByte = chanelByte[0];
      break;
    case 'B':
      chByte = chanelByte[1];
      break;
    case 'C':
      chByte = chanelByte[2];
      break;
    case 'S':
      chByte = chanelByte[3];
      break;
  }
  char sbit;
  for ( int i = 0; i < 3; i++) {
    sbit = chByte[i];
    FillBit(sbit);
  }
}

void InstaFunkbus::FillSwitchBit(int Switch) {
  String swichByte[] = {"0000", "0100", "0010", "0110", "0001", "0101", "0011", "0111"};
  this->FillByte(swichByte[Switch - 1]);
}

void InstaFunkbus::FillParity() {
  int anz = 0;
  for ( int i = 0; i < _buffPos; i++) {
    if (_sendBuff[i] == '1')
      anz++;
  }
  if ( anz % 2 == 0)
    this->FillBit('0');
  else
    this->FillBit('1');
}

void InstaFunkbus::FillCRC(char Chanel, int Switch) {
  String CRC_A[] = {"00011", "00111", "10011", "10111", "00101", "00001", "10101", "10001"};
  String CRC_B[] = {"11011", "11111", "01011", "01111", "11101", "11001", "01101", "01001"};
  String CRC_C[] = {"00111", "00011", "10111", "10011", "00001", "00101", "10001", "10101"};
  String CRC_S[] = {"00111", "00011", "10111", "10011", "00001", "11001", "11101", "100111"};
  String aCRC;

  switch (Chanel) {
    case 'A':
      aCRC = CRC_A[Switch - 1];
      break;
    case 'B':
      aCRC = CRC_B[Switch - 1];
      break;
    case 'C':
      aCRC = CRC_C[Switch - 1];
      break;
    case 'S':
      aCRC = CRC_S[Switch - 1];
      break;
  }
  char sbit;
  for ( int i = 0; i < 5; i++) {
    sbit = aCRC[i];
    // Bei OFF alle negieren
    if ((_sendBuff[40] == '1') && (i < 4)) {
      if (sbit == '1')
        sbit = '0';
      else
        sbit = '1';
    }
    // Lange Pakete
    if ((_sendBuff[42] == '1') && (i == 0)) {
      if (sbit == '1')
        sbit = '0';
      else
        sbit = '1';
    }
    // Bei zweitem Paket bit 3 negieren
    if ((_sendBuff[41] == '1') && (i == 2)) {
      if (sbit == '1')
        sbit = '0';
      else
        sbit = '1';
    }
    FillBit(sbit);
  }

}

void InstaFunkbus::Fill2Chanel(char Chanel, int Switch) {
  String data0 = "0000";
  _buffPos = 0;

  this->FillByte(this->_sendtype);
  this->FillByte(this->_subtype);
  this->FillSerial();
  this->FillByte(this->_state);
  this->FillByte(data0);
  this->FillSwitchBit(Switch);
  this->FillChanel(Chanel);
}

void InstaFunkbus::FillState(bool State) {
  if (State == false) { // On State
    this->FillBit('1');
    this->FillBit('0');
  } else {
    this->FillBit('0');
    this->FillBit('1');
  }
}

void InstaFunkbus::FillLength(int Sequenz) {
  switch (Sequenz) {
    case 0:
      this->FillBit('0');
      this->FillBit('0');
      break;
    case 1:
      this->FillBit('1');
      this->FillBit('0');
      break;
    case 2:
      this->FillBit('0');
      this->FillBit('1');
      break;
    case 3:
      this->FillBit('1');
      this->FillBit('1');
      break;
  }
}

void InstaFunkbus::SendSwitchOnLong(char Chanel, int Switch ) {
  Serial.print("Chanel:");
  Serial.print(Chanel);
  Serial.print(" Swich:");
  Serial.print(Switch);
  Serial.println(" On ");

  this->Fill2Chanel(Chanel, Switch);
  this->FillState(true);
  this->FillLength(0);
  this->FillParity();
  this->FillCRC( Chanel,  Switch);

  this->SendBuffer();
  delay(PULS_PAUSE);

  this->Fill2Chanel(Chanel, Switch);
  this->FillState(true);
  this->FillLength(2);
  this->FillParity();
  this->FillCRC( Chanel,  Switch);
  this->SendBuffer();
  delay(PULS_PAUSE);
  this->SendBuffer();
  delay(PULS_PAUSE);

  this->Fill2Chanel(Chanel, Switch);
  this->FillState(true);
  this->FillLength(3);
  this->FillParity();
  this->FillCRC( Chanel,  Switch);
  this->SendBuffer();
  delay(PULS_PAUSE);
  this->SendBuffer();
  delay(PULS_PAUSE);

}

void InstaFunkbus::SendSwitchOn(char Chanel, int Switch) {
  Serial.print("Chanel:");
  Serial.print(Chanel);
  Serial.print(" Swich:");
  Serial.print(Switch);
  Serial.println(" On ");

  this->Fill2Chanel(Chanel, Switch);
  this->FillState(true);
  this->FillLength(0);
  this->FillParity();
  this->FillCRC( Chanel,  Switch);

  this->SendBuffer();
  delay(PULS_PAUSE);

  this->Fill2Chanel(Chanel, Switch);
  this->FillState(true);
  this->FillLength(1);
  this->FillParity();
  this->FillCRC( Chanel,  Switch);

  this->SendBuffer();
  delay(PULS_PAUSE);
  this->SendBuffer();
  delay(PULS_PAUSE);
}

void InstaFunkbus::SendSwitchOff(char Chanel, int Switch) {
  Serial.print("Chanel:");
  Serial.print(Chanel);
  Serial.print(" Swich:");
  Serial.print(Switch);
  Serial.println(" Off ");

  this->Fill2Chanel(Chanel, Switch);
  this->FillState(false);
  this->FillLength(0);
  this->FillParity();
  this->FillCRC( Chanel,  Switch);
  this->SendBuffer();
  delay(PULS_PAUSE);

  this->Fill2Chanel(Chanel, Switch);
  this->FillState(false);
  this->FillLength(1);
  this->FillParity();
  this->FillCRC( Chanel,  Switch);
  this->SendBuffer();

  this->SendBuffer();
  delay(PULS_PAUSE);
  this->SendBuffer();
  delay(PULS_PAUSE);
}

void InstaFunkbus::SendSwitchOffLong(char Chanel, int Switch) {
  Serial.print("Chanel:");
  Serial.print(Chanel);
  Serial.print(" Swich:");
  Serial.print(Switch);
  Serial.println("Long Off ");

  this->Fill2Chanel(Chanel, Switch);
  this->FillState(false);
  this->FillLength(0);
  this->FillParity();
  this->FillCRC( Chanel,  Switch);
  this->SendBuffer();
  delay(PULS_PAUSE);

  this->Fill2Chanel(Chanel, Switch);
  this->FillState(false);
  this->FillLength(2);
  this->FillParity();
  this->FillCRC( Chanel,  Switch);
  this->SendBuffer();
  delay(PULS_PAUSE);
  this->SendBuffer();
  delay(PULS_PAUSE);

  this->Fill2Chanel(Chanel, Switch);
  this->FillState(false);
  this->FillLength(3);
  this->FillParity();
  this->FillCRC( Chanel,  Switch);
  this->SendBuffer();
  delay(PULS_PAUSE);
  this->SendBuffer();
  delay(PULS_PAUSE);

}

void InstaFunkbus::loop() {
  unsigned long duration;
  // put your main code here, to run repeatedly:
  int buttonState = digitalRead(nReceiverPin);
  if ( lastState != buttonState ) {
    long time = micros();
    lastState = buttonState;
    if (lastTime == 0) {
      lastTime = time;
    } else {
      duration = time - lastTime;
      lastTime = time;
      if (( duration < 200 )) {
        if (sync == 1) {
          decodeTiming();
        }
        lastState = 1;
        duration = 0;
        sync = 0;
        this->_buffPos = 0;
      }
      if (( duration > 3000 ) && (buttonState = 1)) {
        if (sync == 1) {
          decodeTiming();
        }
        sync = 1;
        duration = 0;
        _buffPos = 0;
      }
      if ((duration > 0) && (sync == 1)) {
        timings[_buffPos] = duration;
        _buffPos++;
      }
    }
  }
}

void InstaFunkbus::decodeTiming() {
  if (this->_buffPos > 30) {
    bitPos = 0;
    for (int i = 0; i < _buffPos ; i++) {
      if (timings[i] > PULS_MIN_LENGTH_0) {
        _recBuff[bitPos] = '0';
        bitPos++;
        //          Serial.print('0');
      }
      if ((timings[i] < PULS_MIN_LENGTH_1) && (timings[i + 1] < PULS_MIN_LENGTH_1)) {
        _recBuff[bitPos] = '1';
        bitPos++;
        i++;
        //          Serial.print('1');
      }
    }
    if (bitPos > 49)
      bitPos = 49;
    _recBuff[bitPos] = 0;
  }
}
