#include <Arduino.h>
#include <ModbusMaster.h> //Library for using ModbusMaster
// #include <LiquidCrystal_I2C.h> //jangan gunakan lcd i2c karena data akan korup
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <SD.h>

// ---------- GLOBAL VARIABLES --------------------------------
int torsiMotorX = 0;
int nilaiTambah = 0;
int dataArray[] = {0, 0, 0, 0};   // nilai load x,y,z
unsigned long previousMillis = 0; // will store last time micro sd was updated
const long interval = 3000;       // interval penulisan ke microsd (milliseconds)
int adressMotor[3] = {
    1,
    5,
    6,
};

// ---------- MODBUS ------------------------------------------
#define MAX485_DE 3
#define MAX485_RE_NEG 2
#define M_PARITY SERIAL_8E1
ModbusMaster node; // object node for class ModbusMaster

// ---------- LCD ----------------------------------------------
// LiquidCrystal_I2C lcd(0x27, 16, 2); // Object lcd for class Liquidcrystal with LCD pins (RS, E, D4, D5, D6, D7) that are connected with Arduino UNO.

// ---------- RTC ----------------------------------------------
RTC_DS3231 rtc;

// ---------- SD ----------------------------------------------
File myFile;
const int chipSelect = 10;

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
// initialize?, load motor x, load motor y, load motor z
void SDwrite(int initial, int X, int Y, int Z)
{
  myFile.close();
  myFile = SD.open("yuu.txt", FILE_WRITE);

  if (myFile)
  {
    if (initial == 1)
    {
      myFile.print("load X");
      myFile.print(",");
      myFile.print("load Y");
      myFile.print(",");
      myFile.println("load Z");
    }
    else
    {
      myFile.print(X);
      myFile.print(",");
      myFile.print(Y);
      myFile.print(",");
      myFile.println(Z);
    }
  }
  myFile.close();
}

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
  // lcd.init();
  // lcd.backlight();
  // lcd.setCursor(0, 0);
  // lcd.print("Yuuahmad");
  // delay(500);
  // lcd.clear();

  //------ setup SD
  // cek apakah sd card terpasang benar atau tidak
  if (!SD.begin(chipSelect))
  {
    // lcd.setCursor(0, 0);
    // lcd.println("err init sd       ");
    return;
  }
  // lcd.setCursor(0, 0);
  // lcd.println("fin init sd       ");
  // buat data awal untuk keterangan
  SDwrite(1, 0, 0, 0);
}
void loop()
{
  unsigned long currentMillis = millis();
  DateTime waktuSekarang = rtc.now(); // walaupun ada, tapi tidak dipakai

  // cara menulis data ke slave dari master (arduino nano)
  // float value = analogRead(A0);
  // node.writeSingleRegister(0x40000, value);
  // lcd.setCursor(0, 0);
  // lcd.print(":");
  // lcd.print(value);
  // lcd.print(time.timestamp(DateTime::TIMESTAMP_DATE));

  // cara membaca data dari 3 buah slave
  for (int i = 1; i <= 3; i = i++)
  {
    node.begin(i, Serial);
    delay(500); // berikan ruang kepada serial untuk ganti adress
    uint8_t hasilUtama;
    hasilUtama = node.readHoldingRegisters(452, 1); // baca pada adress 452 saja
    if (hasilUtama == node.ku8MBSuccess)
      dataArray[i] = node.getResponseBuffer(0);
    else
      dataArray[i] = 101;
  }

  // tampilkan data load motor di lcd
  // lcd.setCursor(0, 0); // Pindah ke baris 1
  // lcd.print("XYZ ");
  // for (int i = 4; i <= 8; i = i + 2)
  // {
  //   lcd.print(dataArray[i]);
  //   lcd.print(" ");
  //   // delay(50); // Tunggu 2 detik sebelum menampilkan data berikutnya
  // }
  // lcd.print("                 ");

  // tampilkan waktu sekarang
  // lcd.setCursor(0, 1);
  // lcd.print(";");
  // lcd.print(waktuSekarang.timestamp(DateTime::TIMESTAMP_TIME));

  // akan tulis ke micro sd setiap x interval milidetik sekali
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;                       // save the last time you blinked the LED
    SDwrite(0, dataArray[1], dataArray[2], dataArray[3]); // tulis data ke M.sd
  }
}