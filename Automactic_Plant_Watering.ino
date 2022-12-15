#include <SoftwareSerial.h>

SoftwareSerial HC05(2, 3); //RX, TX

#define PUMP_CONTROL 5
#define SOIL_MOISURE_INPUT A0
#define WATER_LEVEL A1
#define WATER_ALERT 8

int const delayUnit = 500; //ms
int timeCoef = 5;
int moisureLimit = 65;
int moisure ; //random variable 
int humidity;
int wL;
int waterLevel;
char t;
String command;
void setup() {
  Serial.begin(9600);
  HC05.begin(9600);
  pinMode(PUMP_CONTROL, OUTPUT); 
  pinMode(SOIL_MOISURE_INPUT, INPUT); 
  pinMode(WATER_LEVEL, INPUT);
  pinMode(WATER_ALERT, OUTPUT);
}

void loop(){ 
  moisure = analogRead(SOIL_MOISURE_INPUT);  
  humidity = ((1023 - moisure)/1023.00)*100;

  if(humidity < moisureLimit){
    pumpDueToSchedule();
  }

  wL = analogRead(WATER_LEVEL);
  waterLevel = (wL/660.00)*100;
  if (waterLevel < 50) {
    startAlertWaterLevel();
    Serial.println(waterLevel);
  } else {
    stopAlertWaterLevel();
  }

  if (HC05.available()) {

    t = HC05.read();
    if (t != '\n') {
      command = command + t;
    } else {
      handlingCommand(command);
      command = "";
    }

  }

}

void pumpDueToSchedule() {
  digitalWrite(PUMP_CONTROL, HIGH);
  delay(timeCoef*delayUnit);
  digitalWrite(PUMP_CONTROL, LOW);
}

void handlingCommand(String c) {

  if (c[0] == 'M') {
    int m = convert(c);
    moisureLimit = m;
    feedbackMoisure();
  } else if (c[0] == 'T') {
    int time = convert(c);
    timeCoef = time;
    feedbackTime();
  }
    
}

int convert(String s) {
  int result = 0;
  for (int i = 1;i < s.length();i++)
    result = result*10 + (s[i] - 48);
  return result;  
}

void feedbackMoisure() {
  HC05.print("Moisure Limit: ");
  HC05.print(moisureLimit);
  HC05.println("%");
}

void feedbackTime() {
  
  HC05.print("Time: ");
  HC05.print(timeCoef*delayUnit);
  HC05.println("ms\n");
}

void startAlertWaterLevel() {
  digitalWrite(WATER_ALERT, HIGH);
}

void stopAlertWaterLevel() {
  digitalWrite(WATER_ALERT, LOW);
}