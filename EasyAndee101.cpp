/* EasyAndee101.h - Annikken Andee Library for Arduino
Annikken Pte Ltd
Author: Muhammad Hasif

The Easy Andee Library is to allow users to use a feature similar to Arduino's Firmata to control their Arduino using their Bluetooth enabled phone. Users no longer need to code the UI and logic. With the Easy Andee feature on the Andee App, users can create their UI and immediately control their electronics.

This library is free software. This library also uses parts of the Arduino Firmata Library */

#include <CurieBLE.h>
#include <EasyAndee.h>
#include <stdlib.h>

#define MAXBUFFER 9
#define QUEUESIZE 5

bool dataLog = false;

BLEPeripheral blePeripheral; // create peripheral instance
BLEService uartService = BLEService("516e7d06-c4ea-4103-9bd2-c560221a0c16");
BLEDescriptor uartNameDescriptor = BLEDescriptor("2901", "UART");

BLECharacteristic rxCharacteristic = BLECharacteristic("516e7d08-c4ea-4103-9bd2-c560221a0c16", BLEWriteWithoutResponse, 19);
BLEDescriptor rxNameDescriptor = BLEDescriptor("2901", "RX - Receive Data (Write)");

BLECharacteristic txCharacteristic = BLECharacteristic("516e7d07-c4ea-4103-9bd2-c560221a0c16", BLEWrite|BLENotify, 19);
BLEDescriptor txNameDescriptor = BLEDescriptor("2901", "TX - Transfer Data (Notify)");

char receiveQueue [QUEUESIZE][MAXBUFFER];
char sendQueue[QUEUESIZE][MAXBUFFER];

////////////////////////////////////////////////////////////////////////////////////////
//Debugging Functions
////////////////////////////////////////////////////////////////////////////////////////

void printDEC(const char*title,char* buffer)
{
	if(dataLog == true)
	{
		Serial.print(title);
		Serial.print(",DEC:");
		for(unsigned int v = 0; v < (strlen(buffer)) ; v++)
		{
			Serial.print(buffer[v] & 0xFF,DEC);Serial.print(" ");
		}
		Serial.println("");
	}	
}

void printHEX(const char*title,char* buffer)
{
	if(dataLog == true && buffer[0] != 0x00)
	{		
		Serial.print(title);
		Serial.print(",HEX:");		
		for(unsigned int v = 0; v < (strlen(buffer)) ; v++)
		{
			Serial.print(buffer[v] & 0xFF,HEX);Serial.print(" ");
		}
		Serial.println("");	
	}	
}

void printString(const char*title,const char* buffer)
{
	if(dataLog == true)
	{
		Serial.print(title);
		Serial.print(",");
		Serial.println(buffer);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
//EasyAndee BLE Event
////////////////////////////////////////////////////////////////////////////////////////

void blePeripheralConnectHandler(BLECentral& central) {
  // central connected event handler
  Serial.print("Easy Andee Connected, central: ");
  Serial.println(central.address());    
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  // central disconnected event handler
  Serial.print("Easy Andee Disconnected, central: ");
  Serial.println(central.address());
}

void rxCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic) {
  // central wrote new value to characteristic, update LED  
  int i = 0;  
  char buffer[MAXBUFFER];
  memset(buffer,0x00,MAXBUFFER);
  if (rxCharacteristic.value()) 	  
  {			
	//printHEX("receiveData",(char*)rxCharacteristic.value());
	for (i = 0;i < QUEUESIZE;i++)
	{
		if(receiveQueue[i][0] == 0x00)
		{
			memcpy(receiveQueue[i],(char*)rxCharacteristic.value(),(strlen((const char*)rxCharacteristic.value())));
			break;
		}
		else
		{
			//printHEX("queueUsed",receiveQueue[i]);
		}
	}
  }  
}
/////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
//EasyAndee Sketch Functions
////////////////////////////////////////////////////////////////////////////////////////

void EasyAndeePoll(void)
{
  blePeripheral.poll();
  processReceiveQueue();
  replyApp();  
}

int sendData( char*data )
{
  //char sendMsg[18] = {'\0'};
  int msgLen = 0;
  msgLen = strlen(data);
  if(txCharacteristic.subscribed() == true)
  {
	 //printString("sendData","txsubscribed is true");
    if (msgLen < 18)
    {
		data[msgLen] = '\0';
		//printString("sendData","msgLen is less than 18");
		txCharacteristic.setValue( (const unsigned char* )data,msgLen + 1 );
      return OK;
    }
    else
    {
      Serial.println("Message too LONG");
      return TOO_LONG;   
    }
  }
  else
  {
    Serial.println("ERROR with Connection");
    return CONNECTION_ERROR;
  }    
}

void processReceiveQueue()
{
	int j = 0;
	char processQueue[MAXBUFFER];
	memset(processQueue,0x00,MAXBUFFER);
	
	//Serial.println("processing received string");
	
	for(j = 0;j < QUEUESIZE;j++)
	{
		if(receiveQueue[j][0] != 0x00)
		{
			memcpy( processQueue,receiveQueue[j],( strlen (receiveQueue[j]) ) );
			memset(receiveQueue[j],0x00,MAXBUFFER);
			break;
		}
	}	
		
	//printHEX("processQueue",processQueue);
		
    if(processQueue[1] == 'D')//set digital out
    {
		if(processQueue[2] < 111)//pin 0 to 13
		{
			pinMode(processQueue[2] - 97,OUTPUT);
			digitalWrite(processQueue[2] - 97,processQueue[3]-48);
		}
		else
		{	//pin A0 to A5
			if(processQueue[2] == 'o')
			{
				pinMode(A0,OUTPUT);
				digitalWrite(A0,processQueue[3]-48);
			}
			else if(processQueue[2] == 'p')
			{
				pinMode(A1,OUTPUT);
				digitalWrite(A1,processQueue[3]-48);
			}
			else if(processQueue[2] == 'q')
			{
				pinMode(A2,OUTPUT);
				digitalWrite(A2,processQueue[3]-48);
			}
			else if(processQueue[2] == 'r')
			{
				pinMode(A3,OUTPUT);
				digitalWrite(A3,processQueue[3]-48);
			}
			else if(processQueue[2] == 's')
			{
				pinMode(A4,OUTPUT);
				digitalWrite(A4,processQueue[3]-48);
			}
			else if(processQueue[2] == 't')
			{
				pinMode(A5,OUTPUT);
				digitalWrite(A5,processQueue[3]-48);
			}
		}	  
    }
	
    else if(processQueue[1] == 'E')//set digital in
    {
		
	  int k = 0;
      //Serial.println("Digital input detected");
	  for(k = 0;k < QUEUESIZE;k++)
	  {
		  if(sendQueue[k][0] == 0x00)
		  {
			  memcpy(sendQueue[k],processQueue,strlen(processQueue));
			  break;
		  }
		  
	  }
    }
	
    else if(processQueue[1] == 'A')//set analog out
    {
	  char analogBuffer[4];
	  analogBuffer[0] = processQueue[3];
	  analogBuffer[1] = processQueue[4];
	  analogBuffer[2] = processQueue[5];
	  analogBuffer[3] = '\0';
	  
	  int analogValue = strtol(analogBuffer,NULL,10);	  
	  
      if(processQueue[2] == 'd')//pin 3
      {
        pinMode(3,OUTPUT);
        analogWrite(3,analogValue);
	  }
      else if(processQueue[2] == 'f')//pin 5
      {
        pinMode(5,OUTPUT);
        analogWrite(5,analogValue);
      }
      else if(processQueue[2] == 'g')//pin 6
      {
        pinMode(6,OUTPUT);
        analogWrite(6,analogValue);
      }
      else if(processQueue[2] == 'j')//pin 9
      {
        pinMode(9,OUTPUT);
        analogWrite(9,analogValue);
      }      
    }
	
    else if(processQueue[1] == 'B')//set analog in
    {
	  int l = 0;
      //Serial.println("Analog input detected");
	  for(l = 0;l < QUEUESIZE;l++)
	  {
		  if(sendQueue[l][0] == 0x00)
		  {
			  memcpy(sendQueue[l],processQueue,strlen(processQueue));
			  break;
		  }
		  
	  }
    }
	
	//Serial.println("finished processing received string");
}

void replyApp()
{
	int m = 0;	
	char replyAppBuffer[MAXBUFFER];
	char send[MAXBUFFER];
	memset(replyAppBuffer,0x00,MAXBUFFER);
	memset(send,0x00,MAXBUFFER);
	
	for(m = 0;m < QUEUESIZE;m++)
	{
		if(sendQueue[m][0] != 0x00)
		{
			memcpy(replyAppBuffer,sendQueue[m],strlen(sendQueue[m]));
			memset(sendQueue[m],0x00,MAXBUFFER);
			break;
		}
	}
	
	//printHEX("replyAppBuffer",replyAppBuffer);
	
	if (replyAppBuffer[1] == 'E')//Digital In
	{
		int digitalValue = 0;
		
		if(replyAppBuffer[2] < 111)
		{
			pinMode(replyAppBuffer[2] - 97,INPUT);
			delay(1);
			digitalValue = digitalRead(replyAppBuffer[2] - 97);					
		}
		else
		{	//pin A0 to A5
			if(replyAppBuffer[2] == 'o')
			{
				pinMode(A0,INPUT);
				delay(1);
				digitalValue = digitalRead(A0);
			}
			else if(replyAppBuffer[2] == 'p')
			{
				pinMode(A1,INPUT);
				delay(1);
				digitalValue = digitalRead(A1);
			}
			else if(replyAppBuffer[2] == 'q')
			{
				pinMode(A2,INPUT);
				delay(1);
				digitalValue = digitalRead(A2);
			}
			else if(replyAppBuffer[2] == 'r')
			{
				pinMode(A3,INPUT);
				delay(1);
				digitalValue = digitalRead(A3);
			}
			else if(replyAppBuffer[2] == 's')
			{
				pinMode(A4,INPUT);
				delay(1);
				digitalValue = digitalRead(A4);
			}
			else if(replyAppBuffer[2] == 't')
			{
				pinMode(A5,INPUT);
				delay(1);
				digitalValue = digitalRead(A5);
			}
		}				
		sprintf(send,"%c%c%c%c",replyAppBuffer[0],'F',replyAppBuffer[2],(digitalValue+48));
		//printHEX("sendDigital",send);	
		sendData(send);			
	}
	else if(replyAppBuffer[1] == 'B')//Analog In
	{
		int analogValue = 0;
		
		if(replyAppBuffer[2] == 'o')
		{
			pinMode(A0,INPUT);
			delay(1);
			analogValue = analogRead(A0);			
		}
		else if(replyAppBuffer[2] == 'p')
		{
			pinMode(A1,INPUT);
			delay(1);
			analogValue = analogRead(A1);
		}
		else if(replyAppBuffer[2] == 'q')
		{
			pinMode(A2,INPUT);
			delay(1);
			analogValue = analogRead(A2);
		}
		else if(replyAppBuffer[2] == 'r')
		{
			pinMode(A3,INPUT);
			delay(1);
			analogValue = analogRead(A3);
		}
		else if(replyAppBuffer[2] == 's')
		{
			pinMode(A4,INPUT);
			delay(1);
			analogValue = analogRead(A4);
		}
		else if(replyAppBuffer[2] == 't')
		{
			pinMode(A5,INPUT);
			delay(1);
			analogValue = analogRead(A5);
		}
		Serial.print(analogValue);
		sprintf(send,"%c%c%c%04d",replyAppBuffer[0],'C',replyAppBuffer[2],analogValue);
		//printHEX(" sendAnalog",send);
		sendData(send);
	}	
}

/////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
//EasyAndee CLASS Functions
////////////////////////////////////////////////////////////////////////////////////////

/* void EasyAndeeClass::begin()
{
  // set the local name peripheral advertises
  blePeripheral.setLocalName("EZAndee");
  // set the UUID for the service this peripheral advertises:
  blePeripheral.setAdvertisedServiceUuid(uartService.uuid());

  // add service and attributes
  blePeripheral.addAttribute(uartService);
  blePeripheral.addAttribute(uartNameDescriptor);

  // add attributes for TX and RX
  blePeripheral.addAttribute(rxCharacteristic);
  blePeripheral.addAttribute(rxNameDescriptor);
  blePeripheral.addAttribute(txCharacteristic);
  blePeripheral.addAttribute(txNameDescriptor);

  // set event handlers for connecting,disconnecting and reading values
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  rxCharacteristic.setEventHandler(BLEWritten, rxCharacteristicWritten);

  blePeripheral.begin();
	
} */

void EasyAndeeBegin(const char* name)
{
	int m = 0;
	// set the local name peripheral advertises
	blePeripheral.setLocalName(name);
	blePeripheral.setDeviceName(name);
	delay(5);
	
	// set the UUID for the service this peripheral advertises:
	blePeripheral.setAdvertisedServiceUuid(uartService.uuid());
	
	// add service and attributes
	blePeripheral.addAttribute(uartService);
	blePeripheral.addAttribute(uartNameDescriptor);

	// add attributes for TX and RX
	blePeripheral.addAttribute(rxCharacteristic);
	blePeripheral.addAttribute(rxNameDescriptor);
	blePeripheral.addAttribute(txCharacteristic);
	blePeripheral.addAttribute(txNameDescriptor);

	// set event handlers for connecting,disconnecting and reading values
	blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
	blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
	rxCharacteristic.setEventHandler(BLEWritten, rxCharacteristicWritten);

	blePeripheral.begin();
	
	for(m = 0;m<5;m++)
	{
		memset(receiveQueue[m],0x00,MAXBUFFER);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
