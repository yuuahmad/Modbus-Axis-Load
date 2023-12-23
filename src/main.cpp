#include <Arduino.h>
#include <ModbusMaster.h> //Library for using ModbusMaster
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
// #include <SD.h>

// ---------- GLOBAL VARIABLES --------------------------------
int torsiMotorX = 0;
int nilaiTambah = 0;

// ---------- MODBUS ------------------------------------------
#define MAX485_DE 3
#define MAX485_RE_NEG 2
#define M_PARITY SERIAL_8E1
ModbusMaster node; // object node for class ModbusMaster

// ---------- LCD ----------------------------------------------
LiquidCrystal_I2C lcd(0x27, 16, 2); // Object lcd for class Liquidcrystal with LCD pins (RS, E, D4, D5, D6, D7) that are connected with Arduino UNO.

// ---------- RTC ----------------------------------------------
RTC_DS3231 rtc;

// ---------- SD ----------------------------------------------
// File myFile;
// const int chipSelect = 10;

// ---------- FUNGSI ------------------------------------------
// Function for setting stste of Pins DE & RE of RS-485
void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}
void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}
// fungsi untuk menyimpan data yg dikumpulkan ke microsd
// void simpanData(DateTime waktu, File perintahFile)
// {
//   if (perintahFile)
//   {
//     lcd.setCursor(0, 1);
//     lcd.print("write           ");
//     delay(200);
//     myFile.print(waktu.timestamp(DateTime::TIMESTAMP_DATE));
//     myFile.print(",");
//     myFile.print(waktu.timestamp(DateTime::TIMESTAMP_TIME));
//     myFile.print(",");
//     myFile.println(nilaiTambah);
//     myFile.close();
//     lcd.print("done :3           ");
//     delay(200);
//   }
//   else
//   {
//     lcd.setCursor(0, 1);
//     lcd.print("err :(           ");
//     delay(200);
//   }
// }

// -------------- VOID SETUP() -------------------------
void setup()
{
  //------ setup MODBUS
  Serial.begin(19200, M_PARITY); // Baud Rate as 19200
  node.begin(1, Serial);         // Slave ID as 1
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
  node.preTransmission(preTransmission); // Callback for configuring RS-485 Transreceiver correctly
  node.postTransmission(postTransmission);

  //------ setup RTC
  if (!rtc.begin())
  {
    while (1)
      ;
  }
  // upload pertama, lalu komen agar data waktu tersimpan di rtc
  // jangan lepas rtc dari arduino nano
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  //------ setup LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Yuuahmad");
  delay(500);
  lcd.clear();

  //------ setup SD
  // cek apakah sd card terpasang benar atau tidak
  // if (!SD.begin(chipSelect))
  // {
  //   lcd.setCursor(0, 0);
  //   lcd.println("err init sd       ");
  //   return;
  // }
  // lcd.setCursor(0, 0);
  // lcd.println("fin init sd       ");

  // cek apakah ada file bernama load.csv
  // jika ada, buka file tersebut
  // myFile = SD.open("load.csv", FILE_WRITE);
  // if (myFile)
  // {
  //   // tulis nama variabel disetiap awal mulai melogger data
  //   lcd.setCursor(0, 0);
  //   lcd.print("write init data    ");
  //   delay(500);
  //   myFile.print("timestamp Date");
  //   myFile.print(",");
  //   myFile.print("timestamp Time");
  //   myFile.print(",");
  //   myFile.println("nilai-ke");
  //   myFile.close();
  //   lcd.print("done write init    ");
  //   delay(500);
// }
// else lcd.print("err tulis data   ");
// delay(2000);
// }
}
void loop()
{
  DateTime waktuSekarang = rtc.now();

  // cara menulis data ke slave dari master (arduino nano)
  // float value = analogRead(A0);
  // node.writeSingleRegister(0x40000, value);
  // lcd.setCursor(0, 0);
  // lcd.print(":");
  // lcd.print(value);
  // lcd.print(time.timestamp(DateTime::TIMESTAMP_DATE));

  // cara membaca data dari slave
  uint8_t hasilUtama;
  hasilUtama = node.readHoldingRegisters(452, 1); // baca pada adress 452 saja
  if (hasilUtama == node.ku8MBSuccess)
  {
    torsiMotorX = node.getResponseBuffer(0);
    lcd.setCursor(0, 1);
    lcd.print(torsiMotorX);
    lcd.print(" ");
    lcd.print(waktuSekarang.timestamp(DateTime::TIMESTAMP_TIME));
    lcd.print("       ");
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print(";");
    // lcd.print(time.timestamp(DateTime::TIMESTAMP_TIME));
    lcd.print("       ");
  }
  // nilaiTambah++;
  // myFile = SD.open("load.csv", FILE_WRITE);
  // simpanData(waktuSekarang, myFile);
  // delay(200);
}