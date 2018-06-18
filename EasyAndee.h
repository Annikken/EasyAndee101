/* EasyAndee.h - Annikken Andee Library for Arduino
Annikken Pte Ltd
Author: Muhammad Hasif

The Easy Andee Library is to allow users to use a feature similar to Arduino's Firmata to control their Arduino using their Bluetooth enabled phone. Users no longer need to code the UI and logic. With the Easy Andee feature on the Andee App, users can create their UI and immediately control their electronics.

This library is free software. This library also uses parts of the Arduino Firmata Library */

#define BLE_LIMIT 18
#define CONNECTION_ERROR 0
#define OK 1
#define TOO_LONG 2

extern BLEPeripheral blePeripheral;
extern BLEService uartService;
extern BLECharacteristic txCharacteristic;
extern BLECharacteristic rxCharacteristic;

void printDEC(const char*,char*);
void printHEX(const char*,char*);
void printString(const char*,const char*);

void blePeripheralConnectHandler(BLECentral& central);
void blePeripheralDisconnectHandler(BLECentral& central);
void rxCharacteristicWritten(BLECentral&, BLECharacteristic&);

void EasyAndeePoll(void);
int sendData( char* );
void processReceiveQueue(void);
void replyApp(void);
void EasyAndeeBegin(const char*);