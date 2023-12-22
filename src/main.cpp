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

RTC_DS3231 rtc; // nama variabel untuk mengeksekusi program2 ke rtc

const int chipSelectSD = 10; // pin ss microsd module
File myFile;                 // nama variabel untuk mengeksekusi program2 ke microsd

// global variabel
int nilaiTambah = 0;

void simpanData(DateTime waktu, File perintahFile)
{
  if (perintahFile)
  {
    Serial.print("tulis data ke load.csv...");
    myFile.print(waktu.timestamp(DateTime::TIMESTAMP_DATE));
    myFile.print(",");
    myFile.print(waktu.timestamp(DateTime::TIMESTAMP_TIME));
    myFile.print(",");
    myFile.println(nilaiTambah);
    myFile.close();
    Serial.println("berhasil tulis data");
  }
  else
    Serial.println("error tulis data ke load.csv");
}

void setup()
{
  Serial.begin(9600);
  // set tanggal menjadi sekarang! kalau sudah di set, jangan lupa dikomen agar tidak ke set lagi
  // rtc.adjust(DateTime(2023, 12, 22, 13, 2, 0));

  // ----------- RTC --------------------------------
  // cek apakah rtc terpasang benar atau tidak
  if (!rtc.begin())
    while (1)
      ;

  // ketika mcu kehilangan daya, buat mcu mengingat waktu terakhir dengan perintah seperti ini
  if (rtc.lostPower())
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // ----------- SD card -----------------------------
  // cek apakah sd card terpasang benar atau tidak
  if (!SD.begin(chipSelectSD))
  {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // cek apakah ada file bernama load.csv
  // if (SD.exists("load.csv"))
  // jika ada, buka file tersebut
  myFile = SD.open("load.csv", FILE_WRITE);

  // tulis nama variabel disetiap awal mulai melogger data
  if (myFile)
  {
    Serial.print("tulis data ke load.csv...");
    myFile.print("timestamp Date");
    myFile.print(",");
    myFile.print("timestamp Time");
    myFile.print(",");
    myFile.println("nilai-ke");
    myFile.close();
    Serial.println("berhasil tulis data");
  }
  else
    Serial.println("error tulis data ke load.csv");
}

void loop()
{
  DateTime waktuSekarang = rtc.now();

  // tulis semua data yang telah terkumpul di file
  // if (myFile)
  // {
  //   myFile.print(String("DateTime::TIMESTAMP_FULL:\t") + waktuSekarang.timestamp(DateTime::TIMESTAMP_FULL));
  //   myFile.print(",");
  //   myFile.println(nilaiTambah);
  // }

  myFile = SD.open("load.csv", FILE_WRITE);
  simpanData(waktuSekarang, myFile);

  nilaiTambah++;
  delay(1000);
  Serial.println(nilaiTambah);
}