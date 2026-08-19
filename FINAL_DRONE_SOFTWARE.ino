#include <BLEDevice.h>//includes the right esp32 based BLE libraries
#include <BLEutils.h>
#include <BLEServer.h>
#include <Wire.h>
float PitchRate;
float YawRate;
float RollRate;
float Rateroll;
float Ratepitch;
float Rateyaw;
float sum1 = 0;
float sum2 = 0;
float sum3= 0;
int MOTOR1 = 0;
int MOTOR2 = 0;
int MOTOR3 = 0;
int MOTOR4 = 0;
uint8_t throttle_value;
uint8_t Yaw_value;
uint8_t Pitch_value;
uint32_t LoopTimer;
float DesiredRateRoll, DesiredRatePitch, DesiredRateYaw;
float ErrorRateRoll, ErrorRatePitch, ErrorRateYaw;
float InputRoll, InputThrottle, InputPitch, InputYaw;
float PrevErrorRateRoll, PrevErrorRatePitch, PrevErrorRateYaw;
float PrevItermRateRoll, PrevItermRatePitch, PrevItermRateYaw;
float PIDReturn[]={0, 0, 0};
float PRateRoll=0.6; 
float PRatePitch = PRateRoll;
float PRateYaw=2;
float IRateRoll=3.5;
float IRatePitch=IRateRoll;
float IRateYaw=12;
float DRateRoll=0.03;
float DRatePitch=DRateRoll;
float DRateYaw=0;
float MotorInput1, MotorInput2, MotorInput3, MotorInput4;
void pid_equation (float Error, float P, float I, float D, float PrevError, float PrevIterm){
  float Pterm = P*Error;
  float Iterm = PrevIterm + I*(Error+PrevError)*0.004/2;
  if (Iterm>400)Iterm=400;
  else if (Iterm<-400)Iterm=-400;
  float Dterm=D*(Error-PrevError)/0.004;
  float PIDOutput=Pterm+Iterm+Dterm;
  if (PIDOutput>400)PIDOutput=-400;
  else if (PIDOutput<-400)PIDOutput=-400;  
  PIDReturn[0]=PIDOutput;
  PIDReturn[1]=Error;
  PIDReturn[2]=Iterm; 
}
void reset_pid(){
  PrevErrorRateRoll=0;PrevErrorRatePitch=0;PrevErrorRateYaw=0;
  PrevItermRateRoll=0;PrevItermRatePitch=0;PrevItermRateYaw=0;
}
void gyro_signal () {
  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission(false);
    Wire.requestFrom(0x68, 6);
  if (Wire.available()>=6){
    int16_t GyroX = Wire.read()<<8|Wire.read();
    int16_t GyroY = Wire.read()<<8|Wire.read();
    int16_t GyroZ = Wire.read()<<8|Wire.read();
    Rateroll =(float)GyroX/65.5;
    Ratepitch =(float)GyroY/65.5;
    Rateyaw =(float)GyroZ/65.5;
    
  }
  
}
#define ServiceUUID "a4d18b1d-590f-4cfe-b569-6d951816dc49" //defines a variable named ServerUUID which stores the UUID crazy right
#define CharacteristicUUID "13cc3dac-71c9-4f9b-b550-78e84716ed42"
#define Characteristic1UUID "30140440-74a7-4661-91b5-56baa79e427b"//defines a variable named characteristicUUID which stores the UUID of the characteristic crazy right
#define Characteristic2UUID "c8b65fc7-0c30-4b3f-a4d8-e2bc6e7a5af9"
#define Characteristic3UUID "d8e8225b-9257-49f0-af1d-4cbe1a65b0d5"
#define Characteristic4UUID "5ad3fe2c-f2df-4fed-9440-625eef816318"

bool deviceConnected = false;//keeps the initial state of device connected false
class Gaurd_doggy: public BLEServerCallbacks{//Writes a class by the name of Gaurd_doggy which inherits from the public class BLEServerCallback
void onConnect (BLEServer *pserver){//This is a function from the BLE libraries which detects when the phone connects to the esp
 deviceConnected = true;
  Serial.println("Phone connected!");

}
void onDisconnect (BLEServer *pserver ){//This is a fucntion from the BLE libaries which detects when the phone disconnects to the esp 
  deviceConnected = false;
  throttle_value=0;
  Serial.println("Phone disconnected, Restarting advertising again...");
  BLEDevice::startAdvertising();//Starts sending radio wavees once the phone disconnects
}
;};

class PIDCallBacks: public BLECharacteristicCallbacks {
  void onwrite(BLECharacteristic *pCharacteristic){
    String rxvalue = pCharacteristic->getValue();
  if (rxvalue.length() >0){
    char axis;
    float p_gain, i_gain, d_gain;
    int parsed = sscanf(rxvalue.c_str(), "%c,%f,%f,%f", &axis, &p_gain, &i_gain, &d_gain);
  
if (parsed == 4){
  if(axis == 'R' ){
    PRateRoll = p_gain;
    IRateRoll = i_gain;
    DRateRoll = d_gain;
    Serial.println("Roll PID Updated!");
  }
  else if(axis == 'P'){
    PRatePitch = p_gain;
    IRatePitch = i_gain;
    DRatePitch = d_gain;
    Serial.println("Pitch PID Updated!");
  }
  else if(axis == 'Y'){
    PRateYaw = p_gain;
    IRateYaw= i_gain;
    DRateYaw= d_gain;
    Serial.println("Yaw PID Updated!");
  }
}
 }
  }
};
class ThrottleCallBacks: public BLECharacteristicCallbacks{
  void onWrite(BLECharacteristic *pCharacteristic1) {
    String rxvalue1 = pCharacteristic1->getValue();
    
    // Check if we received at least 1 byte
    if (rxvalue1.length() > 0){ 
      // Read exactly the first byte 
      throttle_value = (uint8_t)rxvalue1[0]; 
      
      Serial.print("Throttle Received: ");
      Serial.println(throttle_value);
    }
  }
};

class PitchCallBacks: public BLECharacteristicCallbacks{
  void onWrite(BLECharacteristic *pCharacteristic2){
    String rxvalue2 = pCharacteristic2->getValue();
    if (rxvalue2.length()>0){    
    uint8_t raw_pitch = (uint8_t)rxvalue2[0] ;
    DesiredRatePitch = ((float)raw_pitch - 127.0)/4.0;
    Serial.print("Rate Pitch Recieved:");
    Serial.print(DesiredRatePitch);
    }

  }
};

class RollCallBacks: public BLECharacteristicCallbacks{
  void onWrite (BLECharacteristic *pCharacteristic3){
    String rxvalue3 = pCharacteristic3->getValue();
    if (rxvalue3.length()>0){    
    uint8_t value3 = (uint8_t)rxvalue3[0];
    DesiredRateRoll = ((float)value3-127.0)/4.0;
    Serial.print("Rate Roll Recieved:");
    Serial.println(DesiredRateRoll);
  }}

};
class YawCallBacks: public BLECharacteristicCallbacks{
  void onWrite (BLECharacteristic *pCharacteristic4){
    String rxvalue4 = pCharacteristic4->getValue();
  if (rxvalue4.length()>0){
    uint8_t value4 = (uint8_t)rxvalue4[0];
    DesiredRateYaw = ((float)value4-127.0)/4.0;
    Serial.print("Rate Yaw Recieved:");
    Serial.print(DesiredRateYaw);
  }
  }
};

void setup() {
  Serial.begin(115200);//start serial comms
Wire.setClock(400000);
Wire.begin(2,1);
  pinMode(MOTOR1, OUTPUT);
  pinMode(MOTOR2, OUTPUT);
  pinMode(MOTOR3, OUTPUT);
  pinMode(MOTOR4, OUTPUT);
  analogWrite(5, 0);
  analogWrite(MOTOR2, 0);
  analogWrite(MOTOR3, 0);
  Wire.beginTransmission(0x68);
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission(false);
  Wire.beginTransmission(0x68);
  Wire.write(0x1B);
  Wire.write(0x08);
  Wire.endTransmission();
  Wire.beginTransmission(0x68);
Wire.write(0x6B);
Wire.write(0x00);
Wire.endTransmission();

int i=0;
for (i=0; i<= 2000;i++){
  gyro_signal();
  sum1 += Rateroll;
  sum2 += Ratepitch;
  sum3 += Rateyaw;
}
BLEDevice::init("Drone");//Turns on the bluetooth chip on the esp32
BLEServer *pServer = BLEDevice::createServer();//calls the create server function and stores its address in the variable p server
pServer->setCallbacks(new Gaurd_doggy());
BLEService *pService = pServer->createService(ServiceUUID);//Creates a service with the server uuid of our uuid and uses p server address 
BLECharacteristic *pCharacteristic1 = pService->createCharacteristic(Characteristic1UUID, //This creates a characteristic inside of the service cabinet and stores the address of the characteristic inside of the variable p characteristic
                                                                    BLECharacteristic::PROPERTY_WRITE |//Since the function takes only one argument for permission the| combines two arguments which allow reading writing data
                                                                    BLECharacteristic::PROPERTY_READ);
pCharacteristic1->setCallbacks(new ThrottleCallBacks());
BLECharacteristic *pCharacteristic2 = pService->createCharacteristic(Characteristic2UUID, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);
pCharacteristic2->setCallbacks(new PitchCallBacks());
BLECharacteristic *pCharacteristic3 = pService->createCharacteristic(Characteristic3UUID, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);
pCharacteristic3->setCallbacks(new RollCallBacks());
BLECharacteristic *pCharacteristic4 = pService->createCharacteristic(Characteristic4UUID, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);
pCharacteristic4->setCallbacks(new YawCallBacks());
BLECharacteristic *pCharacteristic = pService->createCharacteristic(CharacteristicUUID, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);
pService->start();//starts the thing
BLEAdvertising *pAdvertising = BLEDevice::getAdvertising(); //Creates a variable pAdvertising and stores the advertising modules address in there
pAdvertising->addServiceUUID(ServiceUUID); //starts advertising at that address with the specfic UUID
pAdvertising->setScanResponse(true); 
BLEDevice::startAdvertising();
LoopTimer=micros();

}

void loop() {
  gyro_signal();
  RollRate = Rateroll - (sum1/2000);
  PitchRate = Ratepitch - (sum2/2000);
  YawRate = Rateyaw - (sum3/2000);
  ErrorRateRoll=DesiredRateRoll-RollRate;
  ErrorRatePitch=DesiredRatePitch-PitchRate;
  ErrorRateYaw=DesiredRateYaw-YawRate;
  pid_equation(ErrorRateRoll, PRateRoll, IRateRoll, DRateRoll, PrevErrorRateRoll, PrevItermRateRoll);
  InputRoll=PIDReturn[0];
  PrevErrorRateRoll=PIDReturn[1];
  PrevItermRateRoll=PIDReturn[2];
  pid_equation(ErrorRatePitch, PRatePitch, IRatePitch, DRatePitch, PrevErrorRatePitch, PrevItermRatePitch);
  InputPitch=PIDReturn[0];
  PrevErrorRatePitch=PIDReturn[1];
  PrevItermRatePitch=PIDReturn[2];
  pid_equation(ErrorRateYaw, PRateYaw, IRateYaw, DRateYaw, PrevErrorRateYaw, PrevItermRateYaw);
  InputYaw=PIDReturn[0];
  PrevErrorRateYaw=PIDReturn[1];
  PrevItermRateYaw=PIDReturn[2];
  InputThrottle=throttle_value*4.0;
  MotorInput1=InputThrottle-InputRoll-InputPitch-InputYaw;
  MotorInput2=InputThrottle-InputRoll+InputPitch+InputYaw;
  MotorInput3=InputThrottle+InputRoll+InputPitch-InputYaw;
  MotorInput4=InputThrottle+InputRoll-InputPitch+InputYaw;
  MotorInput1=constrain(MotorInput1, 0, 1023);
  MotorInput2=constrain(MotorInput2, 0, 1023);
  MotorInput3=constrain(MotorInput3, 0, 1023);
  MotorInput4=constrain(MotorInput4, 0, 1023);
if(throttle_value<15){
  MotorInput1=0;
  MotorInput2=0;
  MotorInput3=0;
  MotorInput4=0;

}
analogWrite(MOTOR1, MotorInput1);
analogWrite(MOTOR2, MotorInput2);
analogWrite(MOTOR3, MotorInput3);
analogWrite(MOTOR4, MotorInput4);
while(micros()-LoopTimer<4000);
LoopTimer=micros();
}
