#define potPin A1
#include <Wire.h>          // load Wire-Library
#define GP2A_ADDRESS 0x45  
#define flexPin A2


void setup() {

  Serial.begin(9600);
  Serial.println("Begin");
  pinMode(potPin, INPUT);
  Wire.begin();         // initialize I2C 
  initGP2A();           // initilize sensor and setup start values
}


void loop() {

  delay(1);
  int potValue= analogRead(potPin);
  int flexValue = analogRead(flexPin);
  int proximityValue = readProximity() ;  // this is just a preliminary rough sensor settin

  Serial.print(proximityValue);
  Serial.print(",");
  Serial.print(flexValue);
  Serial.print(",");
  Serial.println(potValue);
}


// initialize the sensor with all settings
// please refer to the data sheet for details
void initGP2A(){
  
  scan();                     // check whether the sensor is online
  writeGP2AByte(0x01, 0x88);  // measurement cycles; INT detection/non-detection; initialize all registers 
//  D7 D6 D5 D4 D3 D2 D1 D0
//  1  0  0  0  1  0  1  1    0x8B
//  X  X  -  -  -  -  -  -    intermittened operation   00: 0msec, 01: 1.56msec, 10: 6.25msec, 11: 25msec 
//  -  -  X  X  X  -  -  -    number of measurement cycles 000:once; .... 111:8 cycles 
//  -  -  -  -  -  X  X  X    range  000:×1 - 111:×128; 001:x2


  writeGP2AByte(0x00, 0xC0);
//  D7 D6 D5 D4 D3 D2 D1 D0   // no shut-down, continous operation
//  1  1  0  0  0  0  0  0    0xC0
//  X  -  -  -  -  -  -  -    0: shutdown, 1: operation
//  -  X  -  -  -  -  -  -    0: auto shutdown, 1: continous operation
//  -  -  -  -  X  -  -  -    0: non detection, 1: detection
//  -  -  -  -  -  X  -  -    0: no interrupt, 1: interrupt

  writeGP2AByte(0x02, 0x01);
//  D7 D6 D5 D4 D3 D2 D1 D0   // # of cycles(1); resolution (14 bits); range (x2)
//  0  0  0  0  0  0  0  1    0x01 
 
  writeGP2AByte(0x03, 0xF0);
//  D7 D6 D5 D4 D3 D2 D1 D0   // LED peak current (140mA); LED pulses (x16); LED pulse width (9.16µs)
//  0  1  1  1  0  0  0  0    0x70 LED oeak 140 mA
//  1  1  1  1  0  0  0  0    0xF0 LED peak 193 mA

  Serial.println("Initialization finished");
  Serial.println();
  
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


unsigned int readProximity(){
  byte temp  = readGP2AByte(0x18);  // read the LSB portion of the data
  byte temp2 = readGP2AByte(0x19);	// read the MSB portion of the data
  unsigned int data = 0;			// assing a variable for the result
  data = ((temp2 << 8) + temp);		// shift the MSB portion 8 bits to left and add 8 bits of LSB
  
  return data;						
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int shut_down ()  {
byte temp = readGP2AByte (0x00);	// read the whole byte
temp = temp & 0b01111111;  			// set bit D7 from 1 to 0
// temp = temp | 0b10000000;  		// set bit D7 from 0 to 1
  writeGP2AByte(0x00, temp);		// now write it to the register
}
unsigned int wake_up ()  {
byte temp = readGP2AByte (0x00);	// read the whole byte
// temp = temp & 0b01111111;  		// set bit D7 from 1 to 0
temp = temp | 0b10000000;  		    // set bit D7 from 0 to 1
  writeGP2AByte(0x00, temp);		// now write it to the register
}
//-----------------------------------------------------------------------------------------------------------------
unsigned int continous_operation () {
byte temp = readGP2AByte (0x00);	// read the whole byte
temp = temp & 0b10111111;  			// set bit D6 from 1 to 0
// temp = temp | 0b01000000;  		// set bit D6 from 0 to 1
  writeGP2AByte(0x00, temp);		// now write it to the register
}
unsigned int auto_shutdown () {
byte temp = readGP2AByte (0x00);	// read the whole byte
// temp = temp & 0b10111111;  		// set bit D6 from 1 to 0
temp = temp | 0b01000000;  			// set bit D6 from 0 to 1
  writeGP2AByte(0x00, temp);		// now write it to the register
}


//-----------------------------------------------------------------------------------------------------------------
unsigned int readD0(){
    byte temp  = readGP2AByte(0x10);		// read the LSB portion of the data
  byte temp2 = readGP2AByte(0x11);			// read the MSB portion of the data (also 8 bit)
  unsigned int data = 0;					// assing a variable for the result
  data = ((temp2 &0b00111111) <<8) + temp;	// with the &0b00...... the first 2 bits are set to zero (and ignored)

    return data;							// send data back
}
//-----------------------------------------------------------------------------------------------------------------
unsigned int readD1(){
  byte temp  = readGP2AByte(0x12);			// read the LSB portion of the data
  byte temp2 = readGP2AByte(0x13);			// read the MSB portion of the data (also 8 bit)
  unsigned int data = 0;					// assing a variable for the result
  data = ((temp2 &0b00111111) <<8) + temp;	// with the &0b00...... the first 2 bits are set to zero (and ignored)
											// then shifted 8 bits to left and 8 bits of LSB added.
    return data;							// send data back
}

//-----------------------------------------------------------------------------------------------------------------
unsigned int readD2(){
  byte temp  = readGP2AByte(0x14);			// read the LSB portion of the data
  byte temp2 = readGP2AByte(0x15);			// read the MSB portion of the data (also 8 bit)
  unsigned int data = 0;					// assing a variable for the result
  data = ((temp2 &0b00111111) <<8) + temp;	
  return data;							// send data back
}

unsigned int readD3(){
  // readProximity() returns a 14-bit value from the GP2A's proximity data registers
  byte temp  = readGP2AByte(0x16);                // read the LSB portion of the data
  byte temp2 = readGP2AByte(0x17);                // read the MSB portion of the data (also 8 bit)
  unsigned int data = 0;                          // assing a variable for the result
  // data = ((temp2  << 8) + temp);          // this still works
  // but when I set the first 2 bits to zero with &0b00111111 the compiler error occures in Arduino 1.0.5.!
  // when I use Arduino 1.5.2 BETA then it compiles.
  temp2 = temp2 &0b00111111;
  data =( (unsigned int) (temp2 <<8)) + temp;     // this works in Arduino 1.0.5. !
  return data;                                    // send data back

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void writeGP2AByte(byte address, byte data){
 byte error;
 error = Wire.endTransmission();
  Wire.beginTransmission(GP2A_ADDRESS);  //(GP2A_ADDRESS);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
//*
    if (error == 0)
    {
      Serial.println("Transmission successful");
    }
    else
    {
     Serial.println("No Transmission "); 
    }
//*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// read data from the registers
byte readGP2AByte(byte address){
  Wire.beginTransmission(GP2A_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(GP2A_ADDRESS, 1);
//Serial.println("requesting data");
  while(!Wire.available());
//Serial.println("reading data");
  byte data = Wire.read();
//Serial.println("returning data");
  return data;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// check whether the sensor is online
void scan()
{
  byte error;
  Serial.println("Scanning...");
    Wire.beginTransmission(GP2A_ADDRESS);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("SHARP sensor found at address 0x");
      Serial.println(GP2A_ADDRESS,HEX);
    }
    else
    {
     Serial.println("No sensor found "); 
    }
  delay(30);
}

