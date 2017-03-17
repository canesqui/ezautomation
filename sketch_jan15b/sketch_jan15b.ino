
/*
  Writen by Ronaldo Canesqui
  Copyright Ronaldo Canesqui 2016
*/

#include <EEPROM.h>
#include "customtype.h"
#include <DS3231.h>



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
int MAX_SCHEDULE_ENTRIES = 24;
WeeklySchedule weeklySchedule[24];
//YearlySchedule yearlySchedule[26];

// the setup routine runs once when you press reset:

byte analogPin;

// The following lines can be uncommented to set the date and time
//rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
//rtc.setTime(23, 5, 0);     // Set the time to 12:00:00 (24hr format)
//rtc.setDate(15, 3, 2017);   // Set the date to January 1st, 2014

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
  saveToEEPROM(weeklySchedule);
  //weeklySchedule[2] = {20, 0, 1, 1, {2}};
  //  WeeklySchedule weeklySchedule[];
  //  YearlySchedule yearlySchedule[];
  Serial.begin(9600);
  rtc.begin();

  //setDate(16,3,2017,4);
  //setTime(22,22,30);

  
  
  for (int i = 0; i <= MAX_SCHEDULE_ENTRIES - 1; i++) {
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
  Serial.println(relay);
  Serial.println(state);
  Serial.println(index);
}

void VerifyWeeklySchedule(CurrentDateTime currentDateTime, WeeklySchedule weeklySchedule[]) {
  for (int countSchedule = 0; countSchedule < 52; countSchedule++) {
    if (weeklySchedule[countSchedule].Hour == currentDateTime.Hour && weeklySchedule[countSchedule].Minute == currentDateTime.Minute) {
      for (int countWeekDay = 0; countWeekDay < 6; countWeekDay++) {
        if (currentDateTime.WeekDay == weeklySchedule[countSchedule].WeekDays[countWeekDay]) {
          HandleRelay(weeklySchedule[countWeekDay].Relay, weeklySchedule[countWeekDay].State, countSchedule);
        };
      };
    };
  };
}


// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(ledBuiltIn, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(900);               // wait for a second
  digitalWrite(ledBuiltIn, LOW);    // turn the LED off by making the voltage LOW
  //delay(100);          +     // wait for a second

  CurrentDateTime teste = getCurrentDateTime();
  //getCurrentDateTime(teste);
  Serial.println(teste.Day);
  Serial.println(teste.Month);
  Serial.println(teste.Year);
  Serial.println(teste.Hour);
  Serial.println(teste.Minute);
  //Serial.println(readWeeklyschedule());
  //saveToEEPROM(WEEKLYSCHEDULEEEADDRESS, weeklySchedule[0].Hour);
  //Serial.println(weeklySchedule[0].Hour);
 // Serial.println(weeklySchedule[0].Minute);
  //Serial.println("Ronaldo");
  VerifyWeeklySchedule(teste, weeklySchedule);
  // displayState();
  // touchSensorVal = digitalRead(touchSensor);
  delay(900);
  // changeState();

}

//delay(900);               // wait for a second
//digitalWrite(led,LOW);

