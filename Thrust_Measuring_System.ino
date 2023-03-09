// for the SD card reader
#include <SD.h>
#include <SPI.h>

// for the LoadCell
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

//pins:
const int HX711_dout = 4; //mcu > HX711 dout pin DT goes to pin 4
const int HX711_sck = 5; //mcu > HX711 sck pin sck goes to pin 5

int LEDRedPin = 2;
int LEDGreenPin = 3;

int chipSelect = 10;

// Variables
File data;

float loadCellData;
float timeCount = 0;

HX711_ADC LoadCell(HX711_dout, HX711_sck);

boolean sdBeginSuccess = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600); delay(10);
  LoadCell.begin();

  // LED pin mode output 
  pinMode(LEDRedPin, OUTPUT);
  pinMode(LEDGreenPin, OUTPUT);

  //chipSelect pinMode for SD card
  pinMode(chipSelect, OUTPUT);

  //LoadCell start and tare
  boolean _tare = true;
  unsigned long stabilizingtime = 2000;
  LoadCell.start(stabilizingtime, _tare);

  //LoadCell CalFactor is found by calibrating the scale which is done in an example program
  LoadCell.setCalFactor(213.11);

  if (SD.begin(chipSelect)){ // initialize the SD card 
    Serial.println("SD card initialized");
    
  }else {
    sdBeginSuccess = false;
    Serial.println("SD card failed to initialize");
  }

// removes any previous data.txt files to ensure previous data is not included in the new data.
  if (SD.exists("Data.txt")){
    SD.remove("Data.txt");
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (sdBeginSuccess){
    data = SD.open("Data.txt", FILE_WRITE);
    if (data){

      // turns LED green to show system is ready for motor ignition 
      digitalWrite(LEDRedPin, LOW);
      digitalWrite(LEDGreenPin, HIGH);
      
      while(true){
        // gets data from load cell
        LoadCell.update();
        loadCellData = LoadCell.getData();

        // if true the data gets logged to the SD card 
        if (loadCellData >= 2){

          timeCount = timeCount + .1;

          // logs the data onto the SD card 
          data.print(timeCount);
          data.print(",");
          data.println(loadCellData);

          delay(100);
        }

        // if true the SD card is closed and the program is done
        if (loadCellData <= -2){
          data.close();
          sdBeginSuccess = false;
          return;
        }
      }
    }else{
      sdBeginSuccess = false;
      Serial.println("ERROR: could not open SD card");
      return;
    }
  }else {
    digitalWrite(LEDRedPin, HIGH);
    digitalWrite(LEDGreenPin, LOW);
  }
}
