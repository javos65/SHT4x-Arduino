/*************************************************** 
  This is an example for the SHT4x Humidity & Temp Sensor
   NO LIBRARIES, SIMPLE AND DIRECT
  These sensors use I2C to communicate, 2 pins are required to  
  interface

  2025- Jay Fox
  https://github.com/javos65/SHT4x-Arduino
 ****************************************************/

#include <Wire.h>
#include "DEBUGF.h"

/*********** Sensor Defines  **********/
#define SHT4X_ACCHIGH    0xFD
#define SHT4X_ACCMEDIUM  0xF6
#define SHT4X_ACCLOW     0xE0
#define SHT4X_ID         0x89
#define SHT4X_HEAT110MW  0x2F

#define SHT4X_HDURATION   10
#define SHT4X_MDURATION    4
#define SHT4X_LDURATION    2
#define SHT4X_IDDURATION   2
#define SHT4X_HEATDURATION   1010

#define I2CADDRESS      0x44
#define DATALENGTH        6

float G_Temperature=0;
float G_Humidity=0;

void setup() {
/*********** Serial SETUP  **********/
int t=10;  //Initialize serial and wait for port to open, max 10 second waiting
  Serial.begin(115200);
  while (!Serial) {
    ; delay(1000);
    if ( (t--)== 0 ) break;
  }
/*********** I2C SHT4x SETUP  **********/
delay(2000);
uint8_t data[6];
boolean st;
Wire.end();
Wire.begin();
st = readsensor(SHT4X_ID,data,SHT4X_IDDURATION);
DEBUGF("Sensor status = %d\n",st);
DEBUGF("Serial : %02x-%02x-%02x-%02X\n",data[0],data[1],data[4],data[5]);
}


void loop() {
ReadSH4x();
Serial.print("Tc=");Serial.print(G_Temperature);
Serial.print(", h%=");Serial.println(G_Humidity);
delay(2000);
}

// Read sensor, Calculate %h (0.0...1.0) and C (-45...80)
boolean ReadSH4x()
{
boolean st;
uint8_t data[6]; 
uint16_t val; 
if (G_Humidity >0.75){  st = readsensor(SHT4X_HEAT110MW,data,SHT4X_HEATDURATION);}
else {st = readsensor(SHT4X_ACCMEDIUM,data,SHT4X_MDURATION);}
if(st){
  val = ((uint16_t) (data[0] << 8) + data[1]);
  G_Temperature = -45 + 175 * ((float) val / 0xFFFF);
  val = ((uint16_t) (data[3] << 8) + data[4]);
  G_Humidity = ( -6 + 125 * ((float) val / 0xFFFF) )/100;
  if (G_Humidity<0) G_Humidity=0;
  if (G_Humidity>1) G_Humidity=1;  
  return(true);
}
else return(false);
}

/*********I2C Read and Write routines *************/
boolean readsensor(uint8_t Command, uint8_t *data, uint8_t duration)
{
  Wire.beginTransmission(I2CADDRESS);
  if (Wire.write(Command) != 1) {DEBUGF("! write command[0] failed\n");return false;}
  if (Wire.write(0x00) != 1) {DEBUGF("! write command[1] failed\n");return false;}
  Wire.endTransmission(); // returns a code 3: Data notacknowledged ? - ignored
  delay(duration);
  Wire.requestFrom(I2CADDRESS, DATALENGTH);

  // check if the same number of bytes are received that are requested.
  if (Wire.available() != DATALENGTH) {DEBUGF("! Datalength not available\n");return false;}

  for (int i = 0; i < DATALENGTH; ++i) {
    data[i] = Wire.read();
  }
  Wire.endTransmission();
  return true;
}