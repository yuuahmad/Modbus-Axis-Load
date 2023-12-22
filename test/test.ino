#include <Arduino.h>
#include <ModbusMaster.h>

#define re 14
#define de 14
#define modbus Serial2
#define com Serial

#define mbaud 19200
#define cbaud 9600
#define mParity SERIAL_8E1
#define timeout 100
#define polling 100
#define retry_count 10

#define ID_M 1
#define ToR 4


#define reg1 452
#define reg2 452
#define reg3 452
#define reg4 452

uint16_t reg_addr[4]{
  reg1,
  reg2,
  reg3,
  reg4,
};

float DATA_MODBUS[ToR];
int z = 0;


void preTransmission(){
  digitalWrite(re,1);
}
void postTransmission(){
  digitalWrite(re,0);
}

ModbusMaster node;

float readModbusFloat(char addr,uint16_t REG){
  float i = 0;
  uint8_t result,j;

  uint16_t data[4];
  uint32_t value = 0;
  node.begin(addr,modbus);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  result = node.readHoldingRegisters(REG,4);
  delay(polling);
  if(result == node.ku8MBSuccess){
    for(j=0;j<4;j++){
      data[j] = node.getResponseBuffer(j);
    }
    value = data[0];
    int hasil = data[0];
    return hasil;
  }
  else{
    com.print(F("Fail addr>>> ")); com.println(REG);
    delay(1000);
    return 0;
  }
}


void(* resetFunc) (void) = 0;  // declare reset fuction at address 0


void setup() {
  com.begin(cbaud);
  modbus.begin(mbaud,mParity);
  pinMode(re,OUTPUT);

  postTransmission();


  // put your setup code here, to run once:
}

void loop() {
  z = readModbusFloat(1,452);
  com.print(z);
  com.print("   ");
  z = abs(z);
  delay(1000);
  z=z/10;
  com.println(z);
  delay(1000);
}