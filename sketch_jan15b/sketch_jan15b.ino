
/*
  Writen by Ronaldo Canesqui
  Copyright Ronaldo Canesqui 2016
*/

#include <EEPROM.h>
#include "customtype.h"
#include <DS3231.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

/*struct CurrentDateTime {
  byte Day;
  byte Month;
  byte Year;
  byte WeekDay;
  byte Hour;
  byte Minute;
  }

  struct WeeklySchedule{
  byte Hour;
  byte Minute;
  bool State;
  byte Relay;
  int WeekDays[]
  }

  struct YearlySchedule{
  byte Hour;
  byte Minute;
  bool State;
  byte Relay;
  byte Day;
  byte Month;
  byte Year;
  }

*/
// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
// Init a Time-data structure
Time  t;

SoftwareSerial BTSerial(10, 11); // RX | TX
 
int estado_antes = -1; // 

CurrentDateTime getCurrentDateTime() {

  t = rtc.getTime();
  CurrentDateTime current;
  
  //CurrentDateTime current;
  current.Day = t.date;
  current.Month = t.mon;
  current.Year = t.year;
  current.WeekDay = t.dow;
  current.Hour = t.hour;
  current.Minute = t.min;
  
  return current;
}



// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int ledManualModeOn = 2;
int ledManualModeOff = 3;
int ledTimerMode = 4;
int ledBuiltIn = 13;

int touchSensor = 7;
int touchSensorVal = LOW;
int oldTouchSensorVal = LOW;
int currentState = 1;
//WeeklySchedule size is 21
//Microcontroller EEPROM has 512 bytes, so 512/21 = maximum schedule entries
int WEEKLY_SCHEDULE_SIZE = 21;
const int MAX_SCHEDULE_ENTRIES = 24;
WeeklySchedule weeklySchedule[MAX_SCHEDULE_ENTRIES];
char scheduleJson[400] = {};

//YearlySchedule yearlySchedule[26];

// the setup routine runs once when you press reset:

byte analogPin;

void setDate(int day, int month, int year, char dow) {
  rtc.setDOW(dow);
  rtc.setDate(day, month, year);
}

void setTime(int hour, int minute, int second) {
  rtc.setTime(hour, minute, second);
}

void saveToEEPROM(WeeklySchedule data[]) {
  Serial.println("Saving to EEPROM ");
  int eeAddress = 0;
  for (int i = 0; i <= (sizeof(data)); i++) {

    EEPROM.put(eeAddress, data[i]);
    eeAddress = eeAddress + sizeof(WeeklySchedule);
  }
}

void readFromEEPROM(WeeklySchedule weeklySchedule[]) {
  int eeAddress = 0;
  for (int i = 0; i <= MAX_SCHEDULE_ENTRIES - 1; i++) {
    EEPROM.get(eeAddress, weeklySchedule[i]);
    eeAddress = eeAddress + sizeof(WeeklySchedule);
  }
}

/*
  char readWeeklyschedule(){
    return readFromEEPROM(WEEKLYSCHEDULEEEADDRESS);
  }
*/
void setup() {
  // initialize the digital pin as an output.
  pinMode(ledManualModeOn, OUTPUT);
  pinMode(ledManualModeOff, OUTPUT);
  pinMode(ledTimerMode, OUTPUT);
  pinMode(touchSensor, INPUT);
  pinMode(ledBuiltIn, OUTPUT);

  readFromEEPROM(weeklySchedule);
  weeklySchedule[0] = {22, 30, 1, 1, {4, 2}};
  weeklySchedule[1] = {21, 0, 1, 1, {2}};
  weeklySchedule[1] = {22, 59, 1, 1, {5}};
  
  //saveToEEPROM(weeklySchedule);
  //weeklySchedule[2] = {20, 0, 1, 1, {2}};
  //  WeeklySchedule weeklySchedule[];
  //  YearlySchedule yearlySchedule[];
  Serial.begin(9600);
  rtc.begin();

  //setDate(14,9,2017,5);
  //setTime(22,55,30);
  pinMode(8, INPUT);  // este pino detecta se há uma conexão Bluetooth ativa
 
  // o pino KEY do HC-05 pode ser deixado sem conexão para o modo de operação normal
  // ou conecte-o ao pino 9 e use o bloco abaixo
  pinMode(9, OUTPUT);  // Este pino está estado_antes ao pino KEY do HC-05
  digitalWrite(9, LOW); // Coloca o HC-05 em modo de operação
  
  BTSerial.begin(9600);  // velocidade padrão HC-05 em operação normal   
  
  for (int i = 0; i < MAX_SCHEDULE_ENTRIES; i++) {
    Serial.println(weeklySchedule[i].Hour);
  }
  delay(1000);
  //  weeklySchedule = getWeekScheduleFromEEPROM();
  //  yearlySchedule = getYearlyScheduleFromEEPROM();


}

//int getArrayLength(array, referenceType){
//  return sizeof(array) / sizeof(referenceType);
//  }

//void HandleRelaysAccordingToSchedule(CurrentDateTime currentDateTime, Array array, Type referenceType){

//     int arraySize = getArrayLength(array, referenceType);

//     for (int count = 0; count < arraySize; count++) {

//        if ((array[count].hour = currentDateTime.hour) && (array[count].minute = currentDateTime.Minute)){

//          }
//    }
// }

void setSchedule(char schedule[]){
  StaticJsonBuffer<200> jsonBuffer;
  Serial.print(schedule);

  //Serial.print("setSchedule");
  JsonObject& root = jsonBuffer.parseObject(schedule);
     
  if (root.success())
  {
    Serial.print("Success");

    //int index = root["position"].as<int>();
    //Serial.print("after parseobject");
    //Serial.print(root["hour"].as<char>());
    //weeklySchedule[index] = {root["hour"], root["minute"], root["state"], root["relay"]};
  
    //for (int i=0; i <= sizeof(root["weekdays"])/sizeof(int); i++){
    //    weeklySchedule[index].WeekDays[i]=root["weekdays"][i];
    //}
    
    //saveToEEPROM(weeklySchedule);
  }
  //else
  //{Serial.print("Root failure");}
}

void turnOffLeds() {
  //turn off all led
  digitalWrite(ledManualModeOn, LOW);
  digitalWrite(ledManualModeOff, LOW);
  digitalWrite(ledTimerMode, LOW);
}

void changeState() {
  if (currentState == 2) {
    currentState = 0;
  }
  else
  {
    currentState = currentState + 1;
  }
  turnOffLeds();
  delay(900);
  displayState();

  //evaluate new state and proceed with relay operations
}

void displayState() {

  switch (currentState) {
    case 0:
      //Pin 2 will be on.
      digitalWrite(ledManualModeOn, HIGH);
      break;
    case 1:
      //Pin 3 will be on.
      digitalWrite(ledManualModeOff, HIGH);
      break;
    case 2:
      //Pin 4 will be on.
      digitalWrite(ledTimerMode, HIGH);
      break;
  }
}

void HandleRelay(int relay, int state, int index)
{
  Serial.println("Handling relay");
  Serial.println("Relay: " + relay);
  Serial.println("State: " + state);
  Serial.println("Index: " + index);
}

void VerifyWeeklySchedule(CurrentDateTime currentDateTime, WeeklySchedule weeklySchedule[]) {
  for (int countSchedule = 0; countSchedule < MAX_SCHEDULE_ENTRIES; countSchedule++) {
    if (weeklySchedule[countSchedule].Hour == currentDateTime.Hour && weeklySchedule[countSchedule].Minute == currentDateTime.Minute) {
      for (int countWeekDay = 0; countWeekDay <= 6; countWeekDay++) {
        if (currentDateTime.WeekDay == weeklySchedule[countSchedule].WeekDays[countWeekDay]) {
          HandleRelay(weeklySchedule[countSchedule].Relay, weeklySchedule[countSchedule].State, countSchedule);
        };
      };
    };
  };
}


// the loop routine runs over and over again forever:
void loop() {
  //digitalWrite(ledBuiltIn, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(1000);               // wait for a second
  //digitalWrite(ledBuiltIn, LOW);    // turn the LED off by making the voltage LOW

  int estado_agora = digitalRead(8); // verifica se há uma conexão
  
  // se o estado de conexão mudou:
  if (estado_antes != estado_agora) {
    if (estado_agora == HIGH) {
      Serial.println("");
      Serial.println(" > CONECTADO");
    }
    else {
      Serial.println("");
      Serial.println(" > desconectado");
    }
    estado_antes = estado_agora; // atualiza para a proxima rodada
  }

  // Ponte entre o Serial do Arduino e a software serial ligada ao HC-05
  int i = 0;
  char inChar;
  if (BTSerial.available()>0)
  {   
  
    inChar = BTSerial.read();
    char roChar[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

    //Serial.print("I received: ");
    //Serial.print(inChar);
    setSchedule(roChar);
  
  }
  
  //Serial.println("after receiving data from bluetooth");
  //Serial.print(inChar);
  //if (root.success())
  //{
    //Serial.write(scheduleJson);
    
    //saveToEEPROM(weeklySchedule);
    //const char* sensor    = root["sensor"];
    //long        time      = root["time"];
    //double      latitude  = root["data"][0];
    //double      longitude = root["data"][1]; 
  //}
  
  //
  // Step 3: Retrieve the values
  //
  
  //if (BTSerial.available())
    
  //if (Serial.available())
  //  BTSerial.write(Serial.read());
    
  CurrentDateTime teste = getCurrentDateTime();
  //getCurrentDateTime(teste);
  //Serial.println("---------");
  //Serial.print(teste.Day);
  //Serial.print(teste.Month);
  //Serial.println(teste.Year);
  //Serial.println(teste.Hour);
  //Serial.println(teste.Minute);
  //Serial.println("-------------");
  
  //Serial.println(weeklySchedule[0].Hour);
  //Serial.println(weeklySchedule[0].Minute);
  //Serial.println("Ronaldo");
  //VerifyWeeklySchedule(teste, weeklySchedule);
 
  //delay(900);
  // changeState();
  

}

//delay(900);               // wait for a second
//digitalWrite(led,LOW);

