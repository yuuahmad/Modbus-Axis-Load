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

// variabel rtc
RTC_DS3231 rtc; // nama variabel utama rtc

// variabel sd
const int chipSelectSD = 10; // pin ss microsd module
File myFile;                 // nama variabel utama microsd

// variabel modbus
// #define DE 2
#define RE 2                 // Pin receive enable max485. #re dan de satu pin
#define ModbusBaudrate 19200 // Baudrate Komunikasi Modbus

// global variabel
int nilaiTambah = 0;        // nilai untuk testing awal
#define SerialBaudrate 9600 // Baudrate Komunikasi Serial
#define ID_MX 1             // Modbus ID Axis Driver X
#define ID_MY 2             // Modbus ID Axis Driver Y
#define ID_MZ 3             // Modbus ID Axis Driver Z
#define ID_MI 4             // Modbus ID Spindle Inverter

// fungsi untuk menyimpan data yg dikumpulkan ke microsd
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
    Serial.begin(SerialBaudrate);
    // set tanggal menjadi sekarang! kalau sudah di set
    // jangan lupa dikomen agar tidak ke set lagi
    rtc.adjust(DateTime(2023, 12, 22, 21, 16, 0));

    // ----------- RTC --------------------------------
    // cek apakah rtc terpasang benar atau tidak
    if (!rtc.begin())
        while (1)
            ;

    // ketika mcu kehilangan daya, buat mcu mengingat waktu terakhir
    if (rtc.lostPower())
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // ----------- SD card -----------------------------
    // cek apakah sd card terpasang benar atau tidak
    if (!SD.begin(chipSelectSD))
    {
        Serial.println("inisialisasi SD gagal!");
        return;
    }
    Serial.println("inisialisasi SD berhasil");

    // cek apakah ada file bernama load.csv
    // jika ada, buka file tersebut
    myFile = SD.open("load.csv", FILE_WRITE);
    if (myFile)
    {
        // tulis nama variabel disetiap awal mulai melogger data
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

    myFile = SD.open("load.csv", FILE_WRITE);
    simpanData(waktuSekarang, myFile);

    nilaiTambah++;
    delay(1000);
    Serial.println(nilaiTambah);
}