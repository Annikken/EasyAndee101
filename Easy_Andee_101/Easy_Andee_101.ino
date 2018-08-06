/* Easy_Andee_101 - Annikken Andee Firmata Type Sketch for Arduino 101
Annikken Pte Ltd
Author: Muhammad Hasif
Version:0.8
The Easy Andee is a feature to allow users to use a feature similar to Arduino's Firmata to 
control their Arduino using their Bluetooth enabled phone. Users no longer need to code 
the UI and logic. With the Easy Andee feature on the Andee App, users can create their UI 
and immediately control their electronics.

Just load and upload to the Arduino 101. No editing of the sketch needed
*/

#include <CurieBLE.h>
#include <EasyAndee101.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  EasyAndeeBegin("EZAndee");
  //EasyAndee.begin();
}

void loop() {
  // put your main code here, to run repeatedly:   
  EasyAndeePoll();  
}
