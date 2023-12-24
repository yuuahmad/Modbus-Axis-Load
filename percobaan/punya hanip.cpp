#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

File myFile;

const int analogPin = A1;
const int numSamples = 10;
int samples[numSamples];
const int numRanges = 60; // Number of ranges

unsigned long startTimes[numRanges] = {0};
unsigned long durations[numRanges] = {0};
unsigned long previousMillis = 0; // will store last time LED was updated

// constants won't change:
const long interval = 60000;

void setup()
{
  Serial.begin(9600);
  pinMode(4, 1);
  pinMode(analogPin, INPUT);
  digitalWrite(4, 1);
  Serial.begin(9600);

  for (int i = 0; i < numSamples; i++)
    samples[i] = 0;
  Serial.print("Initializing SD card...");

  if (!SD.begin(10))
  {
    Serial.println("initialization failed!");
    while (1)
      ;
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("Current.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile)
  {
    Serial.print("Writing to test.txt...");
    myFile.println("DATA STARTED");
    // close the file:
    myFile.close();
    Serial.println("done.");
  }
  else
    Serial.println("error opening Currentlog.txt");
}

void loop()
{
  for (int i = 0; i < numSamples; i++)
  {
    samples[i] = analogRead(analogPin);
    delay(10); // Small delay between readings
  }

  int total = 0;
  for (int i = 0; i < numSamples; i++)
    total += samples[i];

  float average = (float)total / numSamples;
  average = average / 33.5;

  for (int i = 0; i < numRanges; i++)
  {
    float lowerRange = i * 0.25;
    float upperRange = (i + 1) * 0.25;
    if (average >= lowerRange && average < upperRange)
      if (startTimes[i] == 0)
        startTimes[i] = millis();
      else if (startTimes[i] != 0)
      {
        durations[i] += millis() - startTimes[i];
        startTimes[i] = 0;
      }
  }
  unsigned long currentMillis = millis();

  // Print durations in seconds in the Serial Monitor
  if (currentMillis - previousMillis >= interval)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    for (int i = 0; i < numRanges; i++)
    {
      myFile = SD.open("Current.txt", FILE_WRITE);
      if (myFile)
      {
        myFile.print("Range ");
        myFile.print(i + 1);
        myFile.print(": ");
        myFile.print(durations[i] / 1000.0, 2); // Convert milliseconds to seconds
        myFile.println(" s");
        myFile.print("Average: ");
        myFile.println(average, 2);
        myFile.close();
      }
      else
        Serial.println("error opening Current.txt");

      Serial.print("Range ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(durations[i] / 1000.0, 2); // Convert milliseconds to seconds
      Serial.println(" s");
      Serial.print("Average: ");
      Serial.println(average, 2); // Print average with 2 decimal places
    }
  }
}
