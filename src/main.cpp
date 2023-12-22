/* program data logger load motor
revisi oleh yuuahmad

komponen yang digunakan
-arduino nano
-modul rtc DS3231
-module microsd
-module modbus max485

fitur
-adanya module rtc sehingga waktu data terekan dapat diketahui
-data yang direkam masuk dengan format csv. sehingga dapat diproses dengan python / excell
-komunikasi menggunakan modbus sehingga minim kabel
-program lebih efsien sehingga minim resources dan pembacaan lebih cepat
-data yang dikumpulkan terkumpul lebih banyak daripada program sebelumnya
*/

#include <Arduino.h>
#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <ModbusMaster.h>
#include <LiquidCrystal_I2C.h>

// ---------------- global variabel ----------------
int nilaiTambah = 0;
int torsiMotorX = 0;
int torsiMotorY = 0;
int torsiMotorZ = 0;

// ---------------- variabel dan fungsi lcd ------------------
LiquidCrystal_I2C lcd(0x27, 16, 2); // object node lcd.

// ---------------- variabel dan fungsi modbus ------------------
#define MAX485_DE 3
#define MAX485_RE_NEG 2
ModbusMaster node;     // object node for class ModbusMaster
void preTransmission() // Function for setting stste of Pins DE & RE of RS-485
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}
void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

// ---------------- variabel dan fungsi rtc -----
RTC_DS3231 rtc; // nama variabel utama rtc

// ---------------- variabel dan fungsi sd ----------------
const int chipSelectSD = 10; // pin ss microsd module
File myFile;                 // nama variabel utama microsd

// fungsi untuk menyimpan data yg dikumpulkan ke microsd
void simpanData(DateTime waktu, int torsiMotorX, File perintahFile)
{
  if (perintahFile)
  {
    // Serial.print("tulis data ke load.csv...");
    myFile.print(waktu.timestamp(DateTime::TIMESTAMP_DATE));
    myFile.print(",");
    myFile.print(waktu.timestamp(DateTime::TIMESTAMP_TIME));
    myFile.print(",");
    myFile.print(torsiMotorX);
    // myFile.print(",");
    // myFile.print(torsiMotorY);
    // myFile.print(",");
    // myFile.print(torsiMotorZ);
    myFile.print(",");
    myFile.println(nilaiTambah);
    myFile.close();
    // Serial.println("berhasil tulis data");
  }
  // else
  // Serial.println("error tulis data ke load.csv");
}

void setup()
{
  // ---------------- lcd ----------------
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("init lcd       ");
  delay(1000);

  // ---------------- modbus ----------------
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
  Serial.begin(115200);  // Baud Rate 115200 untuk modbus
  node.begin(1, Serial); // Slave ID 1 sebagai default

  // Callback for configuring RS-485 Transreceiver correctly
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  lcd.setCursor(0, 0);
  lcd.print("init modbus    ");
  delay(1000);

  // ----------- RTC --------------------------------
  // cek apakah rtc terpasang benar atau tidak
  lcd.setCursor(0, 0);
  lcd.print("init rtc       ");
  delay(5000);
  if (!rtc.begin())
  {
    lcd.setCursor(0, 0);
    lcd.print("rtc fail       ");
    delay(1000);
    while (1)
      ;
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("init rtc sucses");
  }

  // ketika mcu kehilangan daya, buat mcu mengingat waktu terakhir
  if (rtc.lostPower())
  {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    lcd.setCursor(0, 0);
    lcd.print("set rtc time   ");
    delay(1000);
  }

  // ---------------- SD card ----------------
  // cek apakah sd card terpasang benar atau tidak
  lcd.setCursor(0, 0);
  lcd.print("init sd        ");
  delay(1000);
  if (!SD.begin(chipSelectSD))
  {
    // Serial.println("inisialisasi SD gagal!");
    lcd.setCursor(0, 0);
    lcd.print("sd fail!!      ");
    delay(1000);
    while (1)
      ; // ketika fail, jamgan lanjutkan program
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("init sd sucsess ");
    delay(1000);
  }

  // Serial.println("inisialisasi SD berhasil");

  // cek apakah ada file bernama load.csv
  // jika ada, buka file tersebut
  myFile = SD.open("load.csv", FILE_WRITE);
  if (myFile)
  {
    // tulis nama variabel disetiap awal mulai melogger data
    // Serial.print("tulis data ke load.csv...");
    lcd.setCursor(0, 0);
    lcd.print("tulis data init ");
    myFile.print("timestamp Date");
    myFile.print(",");
    myFile.print("timestamp Time");
    myFile.print(",");
    myFile.print("load motor x");
    myFile.print(",");
    myFile.println("nilai-ke");
    myFile.close(); // tak perlu di close karena hanya tulis di satu file saja
    // Serial.println("berhasil tulis data");
    lcd.setCursor(0, 0);
    lcd.print("done tulis     ");
    delay(1000);
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("fail tulis     ");
    delay(1000);
    while (1)
      ; // gagal tulis data awal? berhentikan program
  }

  lcd.setCursor(0, 0);
  lcd.print("done all init   ");
  delay(1000);
}
void loop()
{
  DateTime waktuSekarang = rtc.now();

  // float value = analogRead(A0);
  // node.writeSingleRegister(0x40000, value); // Writes value to 0x40000 holding register
  lcd.setCursor(0, 0);
  lcd.print("Val: ");
  // lcd.print(value);
  lcd.print("             ");

  // fungsi membaca nilai torsi motor
  // ini adalah cara membaca data modbus
  uint8_t hasilUtama;
  // float hasilBacaTorsiMotor;
  // node.begin(1, Serial);
  hasilUtama = node.readInputRegisters(0x30002, 1); // baca pada adress 3002 saja
  if (hasilUtama == node.ku8MBSuccess)
  {
    torsiMotorX = node.getResponseBuffer(0x00);
    lcd.setCursor(0, 1);
    lcd.print(torsiMotorX);
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print(";");
  }

  // buka file bernama load.csv dan simpan nilai kesana
  myFile = SD.open("load.csv", FILE_WRITE);
  simpanData(waktuSekarang, torsiMotorX, myFile);

  nilaiTambah++;
  delay(5000);
  // Serial.println(nilaiTambah);
}
