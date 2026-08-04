#include <Wire.h>
float rateroll;
float ratepitch;
float rateyaw;
float sum1 = 0;
float sum2 = 0;
float sum3= 0;
void gyro_signals(){

  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,6);//Ask the mpu6050 for 6 bits to read the 6 registers 43-48
  if (Wire.available() >= 6) {
  int16_t GyroX = Wire.read()<<8|Wire.read();// add the values from register 43 and 44 and then store them in the variable GyroX
  int16_t GyroY = Wire.read()<<8|Wire.read();
  int16_t GyroZ = Wire.read()<<8|Wire.read();
  rateroll = (float)GyroY/65.5;
  ratepitch = (float)GyroX/65.5;
  rateyaw = (float)(GyroZ/65.5);
  }
}

void setup(){
  Wire.beginTransmission(0x68);//Start talking to mpu6050 with the address 0x68
  Wire.write(0x1A);//acceess the register with low pass filter which has an adress 0x1A
  Wire.write(0x05);//This is the value for the low pass filter which has bancwith of 10hz
  Wire.endTransmission();
  Wire.beginTransmission(0x68);
  Wire.write(0x1B);//Access the register that has the senstivity settings
  Wire.write(0x8);//Select the senstivity setting of 65.5 lsb per degrees which corrosponds to a hexadecimal address of 8
  Wire.endTransmission();
Serial.begin(2000000);
Wire.setClock(400000);
Wire.begin(2, 1);
delay(300);
Wire.beginTransmission(0x68);
Wire.write(0x6B);//Access the register with the power management stuff
Wire.write(0x00);//turn on the mpu6050
Wire.endTransmission();//stop communication with the mpu 6050
int i;
for (i=0; i<2300; i++){
  gyro_signals();
  sum1 += rateroll;
  sum2 += ratepitch;
  sum3 += rateyaw;
}
}
void loop(){
gyro_signals();
float rollerror = sum1/2300;
float pitcherror = sum2/2300;
float yawerror = sum3/2300;
float Rateroll = rateroll - rollerror;
float Ratepitch = ratepitch - pitcherror;
float Rateyaw = rateyaw - yawerror;
Serial.print(" Rollrate/° = ");
Serial.print(Rateroll);
Serial.print(" Pitchrate/° = ");
Serial.print(Ratepitch);
Serial.print(" Yawrate/° = ");
Serial.println(Rateyaw);
delay(50);
}