#include <Arduino.h>
#include <ModbusMaster.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

// ####################################################

// #define re 2
// #define de 3

#define rede 2 // Pin enable max485
#define ss 4
#define com Serial
#define modbus SERIAL_PORT_HARDWARE2
#define mvcc 5
#define mgnd 6

#define M_BDR 19200 // Baudrate Komunikasi Modbus
#define C_BDR 9600  // Baudrate Komunikasi Serial
#define M_PARITY SERIAL_8E1
#define TIMEOUT 100
#define POLLING 100
#define retry_count 10

#define ID_MX 1 // Modbus ID Axis Driver X
#define ID_MY 2 // Modbus ID Axis Driver Y
#define ID_MZ 3 // Modbus ID Axis Driver Z
#define ID_MI 4 // Modbus ID Spindle Inverter
#define ToR 3   // Total of Read

ModbusMaster node;

/* Modbus Address
 * # DRIVER
 * 452 = torsi
 *
 * #INVERTER
 * 8501 = CMD
 *      - 6        ENABLE
 *      - 7        STOP
 *      - 15       RUN
 *      - 2062     REVERSE RUN
 *
 * 8502 = FREQ
 */
uint16_t addr[]{
    452,  // DP_TRQ
    8501, //
    8502,
    8601,
    8602,
};

float DATA_MODBUS[ToR];
File myFile;

// uint16_t drv_addr[ToR]{
//   reg1,
//   reg2,
//   reg3,
// };

void preTransmission()
{
  digitalWrite(rede, 1);
}
void postTransmission()
{
  digitalWrite(rede, 0);
}

float readModbusFloat(char addr, uint16_t REG)
{
  float i = 0;
  uint8_t result, j;

  uint16_t data[4];
  uint32_t value = 0;
  node.begin(addr, modbus);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  result = node.readHoldingRegisters(REG, 4);
  delay(POLLING);
  if (result == node.ku8MBSuccess)
  {
    for (j = 0; j < 4; j++)
    {
      data[j] = node.getResponseBuffer(j);
    }
    value = data[0];
    int hasil = data[0];
    return hasil;
  }
  else
  {
    com.print(F("Fail addr>>> "));
    com.println(REG);
    delay(1000);
    return 0;
  }
}

// BACA TORSI DRIVER
void DRV_TRQ(int v)
{
  // delay(POLLING);
  for (char i = 0; i < ToR; i++)
  {
    DATA_MODBUS[i] = readModbusFloat(v, addr[0]);
  }
}

void SDwrite(String q)
{
  myFile = SD.open("XYLoad.txt", FILE_WRITE);
  if (myFile)
  {
    if (myFile.println(q))
    {
      com.println(F("Ok"));
    }
  }
  myFile.close();
}

void SDwriteraw(String v)
{
  myFile.close();
  myFile = SD.open("XYLoadraw.txt", FILE_WRITE);
  if (myFile)
  {
    myFile.println(v);
    com.println(F("Ok"));
  }
  myFile.close();
}

void (*resetFunc)(void) = 0; // declare reset fuction at address 0

void setup()
{
  com.begin(C_BDR);
  modbus.begin(M_BDR, M_PARITY);

  pinMode(rede, OUTPUT);

  pinMode(mvcc, 1);
  pinMode(mgnd, 1);

  digitalWrite(mvcc, 1);
  digitalWrite(mgnd, 0);

  postTransmission();

  // if(SD.begin(ss)) //cek SDcard
  // {
  //   com.println(F("SD Ready!"));
  //   SDwrite("\n\n\n############## START ###############\n\n\n");

  // }
  // else{
  //   com.println(F("SD ERROR!"));
  //   delay(1000);
  //   resetFunc();
  // }

  // put your setup code here, to run once:
}

/*
 * Baca torsi driver
 * dp_trq(Slave ID , Address);
 */
void dp_trq(int a, int b)
{
  int c = readModbusFloat(a, b);
}

void loop() {
  // put your main code here, to run repeatedly:
  int z = readModbusFloat(1,452);
  com.print(z);
  com.print("   ");
  z = abs(z);
  delay(1000);
  z=z/10;
  com.println(z);
  SDwriteraw(String(z));
  SDwrite(String(z));
  delay(1000);
}

void loop()
{
  Serial.println(readModbusFloat(1, 1));
  delay(3000);
}