/* First version created by Gilberto Coelho in February 2023. Updated in August 2024. */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keyboard.h>
#include <EEPROM.h>
#include <Servo.h>
LiquidCrystal_I2C lcd(0x27,16,2);
Servo myservo;

#define pinDT 6
#define pinCLK 7
#define pinSW 8
#define pinButton 10
#define pinStart 9

int screen;
int ajuste;
int ajuste2;
int timeOnOff;
int timeFull;
String timeShort = "";
unsigned long readTime;
uint8_t degree[8]  = {140, 146, 146, 140, 128, 128, 128, 128};

byte HPLC;
byte Read;
byte memoryHPLC = 1;
byte memoryRead = 2;

int timeOn[6];
int timeOff[6];
byte memoryTimeOn1[6] = {5, 7, 9, 11, 13, 15};
byte memoryTimeOn2[6] = {6, 8, 10, 12, 14, 16};
byte memoryTimeOff1[6] = {21, 23, 25, 27, 29, 31};
byte memoryTimeOff2[6] = {22, 24, 26, 28, 30, 32};

byte degreeOff;
byte degreeOn;
byte memoryDegreeOff = 37;
byte memoryDegreeOn = 38;

void runStart();
void roleTimeOn();
void roleTimeOff();
void Save();
void Clock();
void convert();

 void setup() {
  pinMode(pinCLK, INPUT);
  pinMode(pinDT, INPUT);
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(pinButton, INPUT_PULLUP);
  pinMode(pinStart, INPUT_PULLUP); 

  lcd.begin();
  lcd.backlight();
  lcd.createChar(0, degree);
  
  HPLC = EEPROM.read(memoryHPLC);
  Read = EEPROM.read(memoryRead);
  degreeOff = EEPROM.read(memoryDegreeOff);
  degreeOn = EEPROM.read(memoryDegreeOn);
  for (int i = 0; i < 6; i++) {
    timeOn[i] = (EEPROM.read(memoryTimeOn1[i]) << 8) + EEPROM.read(memoryTimeOn2[i]);
    timeOff[i] = (EEPROM.read(memoryTimeOff1[i]) << 8) + EEPROM.read(memoryTimeOff2[i]);
  }
  
  myservo.write(degreeOff);
  myservo.attach(5);      
  lcd.setCursor(0, 0); lcd.print("By");
  lcd.setCursor(0, 1); lcd.print("Gilberto Coelho");
  delay(300);  
  myservo.detach();
}

void loop() {      
  if (screen == 0) {
    if (HPLC == 0) { lcd.setCursor(0, 0); lcd.print(F("Manual mode     ")); lcd.setCursor(0, 1); lcd.print(F("Press start     ")); }
    if (HPLC == 1) { lcd.setCursor(0, 0); lcd.print(F("Waiting HPLC    ")); lcd.setCursor(0, 1); lcd.print(F("                ")); }
    while(screen == 0) { 
    if(digitalRead(pinSW) == LOW) { delay(50); while(digitalRead(pinSW) == LOW) { ; } screen = screen + 1; }
    if(digitalRead(pinButton) == LOW && HPLC == 0) { delay (50); delay(50); while(digitalRead(pinButton) == LOW) { ; } delay(50); runStart(); break; }
    if(digitalRead(pinStart) == LOW && HPLC == 1) { runStart(); break; }
  }}
  
  if (screen == 1) {
    lcd.setCursor(0, 0); lcd.print(F("Start from HPLC:")); 
    if (HPLC == 0){ lcd.setCursor(0, 1); lcd.print(F(" Yes  >No   ")); }
    if (HPLC == 1){ lcd.setCursor(0, 1); lcd.print(F(">Yes   No   ")); }
    while(screen == 1) {
      if(digitalRead(pinSW) == LOW) { delay(50); while(digitalRead(pinSW) == LOW) { ; } screen = screen + 1; }
      if(digitalRead(pinButton) == LOW) { delay(50); while(digitalRead(pinButton) == LOW) { ; } screen -= 1; lcd.setCursor(0, 0); lcd.print(F("Change NOT SAVED")); lcd.setCursor(0, 1); lcd.print(F("in memory       ")); delay (3000); }
      if ((digitalRead(pinDT) == 1) && (digitalRead(pinCLK) == 1)) { ajuste = 0; }  
      if ((digitalRead(pinDT) != digitalRead(pinCLK)) && ajuste == 0) {    
      if ((digitalRead(pinDT) == 1) && digitalRead(pinCLK) == 0) { HPLC = 1; delay(50); ajuste = 1; break;}
      if ((digitalRead(pinDT) == 0) && digitalRead(pinCLK) == 1) { HPLC = 0; delay(50); ajuste = 1; break;}
  }}}  
  
  if (screen == 2) {
    lcd.setCursor(0, 0); lcd.print(F("Initiate AAS:   "));   
    if (Read == 0){ lcd.setCursor(0, 1); lcd.print(F(" Yes  >No  ")); }
    if (Read == 1){ lcd.setCursor(0, 1); lcd.print(F(">Yes   No  ")); }
    while(screen == 2) {
      if(digitalRead(pinSW) == LOW) { delay(50); while(digitalRead(pinSW) == LOW) { ; } screen = screen + 1; break; }
      if(digitalRead(pinButton) == LOW) { delay(50); while(digitalRead(pinButton) == LOW) { ; } screen -= 1;  }
      if ((digitalRead(pinDT) == 1) && (digitalRead(pinCLK) == 1)) { ajuste = 0; }  
      if ((digitalRead(pinDT) != digitalRead(pinCLK)) && ajuste == 0) {    
      if ((digitalRead(pinDT) == 1) && digitalRead(pinCLK) == 0) { Read = 1; delay(50); ajuste = 1; break;}
      if ((digitalRead(pinDT) == 0) && digitalRead(pinCLK) == 1) { Read = 0; delay(50); ajuste = 1; break;}
  }}} 
    
  if (screen == 3) {
    lcd.setCursor(0, 0); lcd.print(F("Step 01:   >OPEN"));
    timeFull = timeOn[0];
    convert();
    lcd.print(timeShort);
    while(screen == 3) { roleTimeOn(0); if (ajuste2 == 1){ajuste2 = 0; break; }
  }}

  if (screen == 4) {
    lcd.setCursor(0, 0); lcd.print(F("Step 02:  >CLOSE"));
    timeFull = timeOff[0] + timeOn[0];
    convert();
    lcd.print(timeShort);       
    while(screen == 4) { roleTimeOff(0); if (ajuste2 == 1){ajuste2 = 0; break; }
  }}
  
  if (screen == 5) {
    lcd.setCursor(0, 0);  lcd.print(F("Step 03:   >OPEN"));
    timeFull = timeOn[1] + timeOff[0] + timeOn[0];
    convert();
    lcd.print(timeShort);      
    while(screen == 5) { roleTimeOn(1); if (ajuste2 == 1){ajuste2 = 0; break; }
  }}

  if (screen == 6) {
    lcd.setCursor(0, 0); lcd.print(F("Step 04:  >CLOSE"));
    timeFull = timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];
    convert();
    lcd.print(timeShort);     
    while(screen == 6) { roleTimeOff(1); if (ajuste2 == 1){ajuste2 = 0; break; }
  }}

  if (screen == 7) {
    lcd.setCursor(0, 0); lcd.print(F("Step 05:   >OPEN"));
    timeFull = timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];
    convert();
    lcd.print(timeShort);      
    while(screen == 7) { roleTimeOn(2); if (ajuste2 == 1){ajuste2 = 0; break; }
  }}

  if (screen == 8) {
    lcd.setCursor(0, 0); lcd.print(F("Step 06:  >CLOSE"));
    timeFull = timeOff[2] + timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];
    convert();
    lcd.print(timeShort);   
    while(screen == 8) { roleTimeOff(2); if (ajuste2 == 1){ajuste2 = 0; break; }
  }}

  if (screen == 9) {
    lcd.setCursor(0, 0); lcd.print(F("Step 07:   >OPEN"));
    timeFull = timeOn[3] + timeOff[2] + timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];
    convert();
    lcd.print(timeShort);       
    while(screen == 9) { roleTimeOn(3); if (ajuste2 == 1){ajuste2 = 0; break; }
  }}

  if (screen == 10) {
    lcd.setCursor(0, 0); lcd.print(F("Step 08:  >CLOSE"));
    timeFull = timeOff[3] + timeOn[3] + timeOff[2] + timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];
    convert();
    lcd.print(timeShort);   
    while(screen == 10) { roleTimeOff(3); if (ajuste2 == 1){ajuste2 = 0; break; }
  }}

  if (screen == 11) {
    lcd.setCursor(0, 0); lcd.print(F("Step 09:   >OPEN"));
    timeFull = timeOn[4] + timeOff[3] + timeOn[3] + timeOff[2] + timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];
    convert();
    lcd.print(timeShort);       
    while(screen == 11) { roleTimeOn(4); if (ajuste2 == 1){ajuste2 = 0; break; }
  }}

  if (screen == 12) {
    lcd.setCursor(0, 0); lcd.print(F("Step 10:  >CLOSE"));
    timeFull = timeOff[4] + timeOn[4] + timeOff[3] + timeOn[3] + timeOff[2] + timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];
    convert();
    lcd.print(timeShort);    
    while(screen == 12) { roleTimeOff(4); if (ajuste2 == 1){ajuste2 = 0; break; }
  }}

  if (screen == 13) {
    lcd.setCursor(0, 0); lcd.print(F("Step 11:   >OPEN"));
    timeFull = timeOn[5] + timeOff[4] + timeOn[4] + timeOff[3] + timeOn[3] + timeOff[2] + timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];
    convert();
    lcd.print(timeShort);   
    while(screen == 13) { roleTimeOn(5); if (ajuste2 == 1){ajuste2 = 0; break; }
  }}

  if (screen == 14) {
    lcd.setCursor(0, 0); lcd.print(F("Step 12:  >CLOSE"));
    timeFull = timeOff[5] + timeOn[5] + timeOff[4] + timeOn[4] + timeOff[3] + timeOn[3] + timeOff[2] + timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];
    convert();
    lcd.print(timeShort);     
    while(screen == 14) { roleTimeOff(5); if (ajuste2 == 1){ajuste2 = 0; break; }
  }}

  if (screen == 15) {
    lcd.setCursor(0, 0); lcd.print(F("Position CLOSE  "));
    lcd.setCursor(0, 1); lcd.print(F("Degree: ")); lcd.print(degreeOff); lcd.write((byte)0); lcd.print(F("      "));
    myservo.write(degreeOff);
    myservo.attach(5);
    while(screen == 15) {
      if(digitalRead(pinSW) == LOW) {  delay(50); while(digitalRead(pinSW) == LOW) { ; } screen = screen + 1; }
      if(digitalRead(pinButton) == LOW) { delay(50); while(digitalRead(pinButton) == LOW) { ; } myservo.detach(); screen -= 1; }
      if ((digitalRead(pinDT) == 1) && (digitalRead(pinCLK) == 1)) { ajuste = 0; }  
      if ((digitalRead(pinDT) != digitalRead(pinCLK)) && ajuste == 0) {    
      if ((digitalRead(pinDT) == 0) && digitalRead(pinCLK) == 1 && degreeOff < 180) { degreeOff ++; ajuste = 1; delay(10); break; }
      if ((digitalRead(pinDT) == 1) && digitalRead(pinCLK) == 0 && degreeOff > 0) { degreeOff --; ajuste = 1; delay(10); break; }
      if ((digitalRead(pinDT) == 1) && digitalRead(pinCLK) == 0 && degreeOff == 0) { ajuste = 1; delay(10); }
  }}}

  if (screen == 16) {
    lcd.setCursor(0, 0); lcd.print(F("Position OPEN   "));
    lcd.setCursor(0, 1); lcd.print(F("Degree: ")); lcd.print(degreeOn); lcd.write((byte)0); lcd.print(F("      "));
    myservo.write(degreeOn);
    while(screen == 16) {
      if(digitalRead(pinSW) == LOW) {  delay(50); while(digitalRead(pinSW) == LOW) { ; } myservo.write(degreeOff); Save(); myservo.detach(); screen = 0; }
      if(digitalRead(pinButton) == LOW) { delay(50); while(digitalRead(pinButton) == LOW) { ; } screen -= 1;  }
      if ((digitalRead(pinDT) == 1) && (digitalRead(pinCLK) == 1)) { ajuste = 0; }  
      if ((digitalRead(pinDT) != digitalRead(pinCLK)) && ajuste == 0) {    
      if ((digitalRead(pinDT) == 0) && digitalRead(pinCLK) == 1 && degreeOn < 180) { degreeOn ++; ajuste = 1; delay(10); break; }
      if ((digitalRead(pinDT) == 1) && digitalRead(pinCLK) == 0 && degreeOn > 0) { degreeOn --; ajuste = 1; delay(10); break; }
      if ((digitalRead(pinDT) == 1) && digitalRead(pinCLK) == 0 && degreeOn == 0) { ajuste = 1; delay(10); }
  }}}
}

void runStart(){
  myservo.write(degreeOff);
  myservo.attach(5);
  if(Read == 1 && digitalRead(pinButton) == HIGH && HPLC == 0) {
    lcd.setCursor(0, 0); lcd.print(F("Waiting process ")); 
    lcd.setCursor(0, 1); lcd.print(F("of injection    ")); 
    while(digitalRead(pinStart) == HIGH) { 
      if (digitalRead(pinButton) == LOW) { break; }
   }}
       
  readTime = millis();
  readTime /= 1000;
  if (digitalRead(pinButton) == HIGH) {
    lcd.clear();
    if(Read == 1 && digitalRead(pinButton) == HIGH) { 
      lcd.setCursor(0, 0); lcd.print(F("Sample injected"));
      lcd.setCursor(0, 1); lcd.print(F("Read requested"));
      Keyboard.press(KEY_RETURN); Keyboard.release(KEY_RETURN);
        if(digitalRead(pinButton) == HIGH) { delay(500); }
        if(digitalRead(pinButton) == HIGH) { delay(500); }
        lcd.setCursor(0, 0); lcd.print(F("Reading         "));
        lcd.setCursor(0, 1); lcd.print(F("                "));
    }
    if(Read == 0) {
      lcd.setCursor(0, 0); lcd.print(F("Delay           "));
  }}
          
//Step 01  >OPEN
  if (timeOn[0] > 0 && digitalRead(pinButton) == HIGH) {
    timeOnOff = timeOn[0];            
    Clock();
    lcd.setCursor(0, 0); lcd.print(F("Step 01:   >OPEN")); 
    myservo.write(degreeOn);
    myservo.attach(5);
    delay(15);
  }
      
//Step 02  >CLOSE     
  if (timeOff[0] > 0 && digitalRead(pinButton) == HIGH) {
    timeOnOff = timeOff[0] + timeOn[0];            
    Clock();
    lcd.setCursor(0, 0); lcd.print(F("Step 02:  >CLOSE"));
    myservo.write(degreeOff);
    myservo.attach(5);
    delay(15); 
  }
  
//Step 03  >OPEN
  if (timeOn[1] > 0 && digitalRead(pinButton) == HIGH) {
    timeOnOff = timeOn[1] + timeOff[0] + timeOn[0];            
    Clock();
    lcd.setCursor(0, 0); lcd.print(F("Step 03:   >OPEN")); 
    myservo.write(degreeOn);
    myservo.attach(5);
    delay(15);
  }

//Step 04  >CLOSE
  if (timeOff[1] > 0 && digitalRead(pinButton) == HIGH) {
    timeOnOff = timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];            
    Clock();
    lcd.setCursor(0, 0); lcd.print(F("Step 04:  >CLOSE"));
    myservo.write(degreeOff);
    myservo.attach(5);
    delay(15);
  }
      
//Step 05  >OPEN
  if (timeOn[2] > 0 && digitalRead(pinButton) == HIGH) {
    timeOnOff = timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];            
    Clock();
    lcd.setCursor(0, 0); lcd.print(F("Step 05:   >OPEN")); 
    myservo.write(degreeOn);
    myservo.attach(5);
    delay(15);
  }
  
//Step 06  >CLOSE    
  if (timeOff[2] > 0 && digitalRead(pinButton) == HIGH) {
    timeOnOff = timeOff[2] + timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];            
    Clock();
    lcd.setCursor(0, 0); lcd.print(F("Step 06:  >CLOSE")); 
    myservo.write(degreeOff);
    myservo.attach(5);
    delay(15);
  }
  
//Step 07  >OPEN
  if (timeOn[3] > 0 && digitalRead(pinButton) == HIGH) {
    timeOnOff = timeOn[3] + timeOff[2] + timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];            
    Clock();
    lcd.setCursor(0, 0); lcd.print(F("Step 07:   >OPEN")); 
    myservo.write(degreeOn);
    myservo.attach(5);
    delay(15);
  }

//Step 08  >CLOSE    
  if (timeOff[3] > 0 && digitalRead(pinButton) == HIGH) {
    timeOnOff = timeOff[3] + timeOn[3] + timeOff[2] + timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];            
    Clock();
    lcd.setCursor(0, 0); lcd.print(F("Step 08:  >CLOSE"));
    myservo.write(degreeOff);
    myservo.attach(5);
    delay(15); 
  }

//Step 09  >OPEN
  if (timeOn[4] > 0 && digitalRead(pinButton) == HIGH) {
    timeOnOff = timeOn[4] + timeOff[3] + timeOn[3] + timeOff[2] + timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];            
    Clock();
    lcd.setCursor(0, 0); lcd.print(F("Step 09:   >OPEN"));
    myservo.write(degreeOn);
    myservo.attach(5);
    delay(15); 
  }

//Step 10  >CLOSE 
  if (timeOff[4] > 0 && digitalRead(pinButton) == HIGH) {
    timeOnOff = timeOff[4] + timeOn[4] + timeOff[3] + timeOn[3] + timeOff[2] + timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];            
    Clock();
    lcd.setCursor(0, 0); lcd.print(F("Step 10:  >CLOSE")); 
    myservo.write(degreeOff);
    myservo.attach(5);
    delay(15);
  }

//Step 11  >OPEN
  if (timeOn[5] > 0 && digitalRead(pinButton) == HIGH) {
    timeOnOff = timeOn[5] + timeOff[4] + timeOn[4] + timeOff[3] + timeOn[3] + timeOff[2] + timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];            
    Clock();
    lcd.setCursor(0, 0); lcd.print(F("Step 11:   >OPEN"));
    myservo.write(degreeOn);
    myservo.attach(5);
    delay(15); 
  }
  
//Step 12  >CLOSE   
  if (timeOff[5] > 0 && digitalRead(pinButton) == HIGH) {
    timeOnOff = timeOff[5] + timeOn[5] + timeOff[4] + timeOn[4] + timeOff[3] + timeOn[3] + timeOff[2] + timeOn[2] + timeOff[1] + timeOn[1] + timeOff[0] + timeOn[0];            
    Clock();
    lcd.setCursor(0, 0); lcd.print(F("Step 12:  >CLOSE"));
  }
  myservo.write(degreeOff);
  myservo.attach(5);
  if(digitalRead(pinButton) == HIGH) { delay(2000); }
  lcd.setCursor(0, 0); lcd.print(F("Program finished"));
  lcd.setCursor(0, 1); lcd.print(F("Valve closed    "));       
  delay(2000);
  myservo.detach();
}

void roleTimeOn (int a){
    if(digitalRead(pinSW) == LOW) { delay(50); while(digitalRead(pinSW) == LOW) { ; } screen = screen + 1; }
    if(digitalRead(pinButton) == LOW) { delay(50); while(digitalRead(pinButton) == LOW) { ; } screen -= 1;  }
    if ((digitalRead(pinDT) == 1) && (digitalRead(pinCLK) == 1)) { ajuste = 0; }  
    if ((digitalRead(pinDT) != digitalRead(pinCLK)) && ajuste == 0) {    
      if ((digitalRead(pinDT) == 0) && digitalRead(pinCLK) == 1 && timeOn[a] < 1499) { timeOn[a] ++; ajuste = 1; delay(10); ajuste2 = 1; }
      if ((digitalRead(pinDT) == 1) && digitalRead(pinCLK) == 0 && timeOn[a] > 0) { timeOn[a] --; ajuste = 1; delay(10); ajuste2 = 1; }
      if ((digitalRead(pinDT) == 1) && digitalRead(pinCLK) == 0 && timeOn[a] == 0) { ajuste = 1; delay(10); }
}}
  
void roleTimeOff (int b){
    if(digitalRead(pinSW) == LOW) { delay(50); while(digitalRead(pinSW) == LOW) { ; } screen = screen + 1; }
    if(digitalRead(pinButton) == LOW) { delay(50); while(digitalRead(pinButton) == LOW) { ; } screen -= 1;  }
    if ((digitalRead(pinDT) == 1) && (digitalRead(pinCLK) == 1)) { ajuste = 0; }  
    if ((digitalRead(pinDT) != digitalRead(pinCLK)) && ajuste == 0) {    
      if ((digitalRead(pinDT) == 0) && digitalRead(pinCLK) == 1 && timeOff[b] < 1499) { timeOff[b] ++; ajuste = 1; delay(10); ajuste2 = 1; }
      if ((digitalRead(pinDT) == 1) && digitalRead(pinCLK) == 0 && timeOff[b] > 0) { timeOff[b] --; ajuste = 1; delay(10); ajuste2 = 1; }
      if ((digitalRead(pinDT) == 1) && digitalRead(pinCLK) == 0 && timeOff[b] == 0) { ajuste = 1; delay(10); }
}}

void Save (){
  EEPROM.put(memoryHPLC, HPLC);
  EEPROM.put(memoryRead, Read);
  EEPROM.put(memoryDegreeOff, degreeOff);
  EEPROM.put(memoryDegreeOn, degreeOn);
  for (int i = 0; i < 6; i++) {
    EEPROM.put(memoryTimeOn1[i], timeOn[i] >> 8);   EEPROM.put(memoryTimeOn2[i], timeOn[i] & 0xFF);
    EEPROM.put(memoryTimeOff1[i], timeOff[0] >> 8);  EEPROM.put(memoryTimeOff2[i], timeOff[i] & 0xFF);
  }
  lcd.setCursor(0, 0); lcd.print(F("Information were"));   
  lcd.setCursor(0, 1); lcd.print(F("saved in memory "));
  delay(2000);
}

void Clock() {
  while(((millis() / 1000) - readTime) <= timeOnOff && digitalRead(pinButton) == HIGH) {      
    unsigned long time1 = millis();
    time1 /= 1000;
    unsigned long time2 = time1 - readTime;
    lcd.setCursor(0, 1); 
    lcd.print(F("Time: ")); 
    if(time2 < 60){ lcd.print(F("00:")); if(time2 < 10){ lcd.print(F("0")); } lcd.print(time2); }
    if(time2 > 59 && time2 < 120){ lcd.print(F("01:")); if(time2 < 70){ lcd.print(F("0")); } lcd.print(time2 - 60); lcd.print(F(" "));}
    if(time2 > 119 && time2 < 180){ lcd.print(F("02:")); if(time2 < 130){ lcd.print(F("0")); } lcd.print(time2 - 120); lcd.print(F(" "));}
    if(time2 > 179 && time2 < 240){ lcd.print(F("03:")); if(time2 < 190){ lcd.print(F("0")); } lcd.print(time2 - 180); lcd.print(F(" "));}
    if(time2 > 239 && time2 < 300){ lcd.print(F("04:")); if(time2 < 250){ lcd.print(F("0")); } lcd.print(time2 - 240); lcd.print(F(" "));}
    if(time2 > 299 && time2 < 360){ lcd.print(F("05:")); if(time2 < 310){ lcd.print(F("0")); } lcd.print(time2 - 300); lcd.print(F(" "));}
    if(time2 > 359 && time2 < 420){ lcd.print(F("06:")); if(time2 < 370){ lcd.print(F("0")); } lcd.print(time2 - 360); lcd.print(F(" "));}
    if(time2 > 419 && time2 < 480){ lcd.print(F("07:")); if(time2 < 430){ lcd.print(F("0")); } lcd.print(time2 - 420); lcd.print(F(" "));}
    if(time2 > 479 && time2 < 540){ lcd.print(F("08:")); if(time2 < 490){ lcd.print(F("0")); } lcd.print(time2 - 480); lcd.print(F(" "));}
    if(time2 > 539 && time2 < 600){ lcd.print(F("09:")); if(time2 < 550){ lcd.print(F("0")); } lcd.print(time2 - 540); lcd.print(F(" "));}
    if(time2 > 599 && time2 < 660){ lcd.print(F("10:")); if(time2 < 610){ lcd.print(F("0")); } lcd.print(time2 - 600); lcd.print(F(" "));}
    if(time2 > 659 && time2 < 720){ lcd.print(F("11:")); if(time2 < 670){ lcd.print(F("0")); } lcd.print(time2 - 660); lcd.print(F(" "));}
    if(time2 > 719 && time2 < 780){ lcd.print(F("12:")); if(time2 < 730){ lcd.print(F("0")); } lcd.print(time2 - 720); lcd.print(F(" "));}
    if(time2 > 779 && time2 < 840){ lcd.print(F("13:")); if(time2 < 790){ lcd.print(F("0")); } lcd.print(time2 - 780); lcd.print(F(" "));}
    if(time2 > 839 && time2 < 900){ lcd.print(F("14:")); if(time2 < 850){ lcd.print(F("0")); } lcd.print(time2 - 840); lcd.print(F(" "));}
    if(time2 > 899 && time2 < 960){ lcd.print(F("15:")); if(time2 < 910){ lcd.print(F("0")); } lcd.print(time2 - 900); lcd.print(F(" "));}
    if(time2 > 959 && time2 < 1020){ lcd.print(F("16:")); if(time2 < 970){ lcd.print(F("0")); } lcd.print(time2 - 960); lcd.print(F(" "));}
    if(time2 > 1019 && time2 < 1080){ lcd.print(F("17:")); if(time2 < 1030){ lcd.print(F("0")); } lcd.print(time2 - 1020); lcd.print(F(" "));}
    if(time2 > 1079 && time2 < 1140){ lcd.print(F("18:")); if(time2 < 1090){ lcd.print(F("0")); } lcd.print(time2 - 1080); lcd.print(F(" "));}
    if(time2 > 1139 && time2 < 1200){ lcd.print(F("19:")); if(time2 < 1150){ lcd.print(F("0")); } lcd.print(time2 - 1140); lcd.print(F(" "));}
    if(time2 > 1199 && time2 < 1260){ lcd.print(F("20:")); if(time2 < 1210){ lcd.print(F("0")); } lcd.print(time2 - 1200); lcd.print(F(" "));}
    if(time2 > 1259 && time2 < 1320){ lcd.print(F("21:")); if(time2 < 1270){ lcd.print(F("0")); } lcd.print(time2 - 1260); lcd.print(F(" "));}
    if(time2 > 1319 && time2 < 1380){ lcd.print(F("22:")); if(time2 < 1330){ lcd.print(F("0")); } lcd.print(time2 - 1320); lcd.print(F(" "));}
    if(time2 > 1379 && time2 < 1440){ lcd.print(F("23:")); if(time2 < 1390){ lcd.print(F("0")); } lcd.print(time2 - 1380); lcd.print(F(" "));}
    if(time2 > 1439 && time2 < 1500){ lcd.print(F("24:")); if(time2 < 1450){ lcd.print(F("0")); } lcd.print(time2 - 1440); lcd.print(F(" "));}
}}

void convert() {
  timeShort = "";
  if(timeFull < 60){ timeShort += "00:"; if(timeFull < 10){ timeShort += "0"; } timeShort += timeFull; }
  if(timeFull > 59 && timeFull < 120){ timeShort += "01:"; if(timeFull < 70){ timeShort += "0"; } timeShort += (timeFull - 60); }
  if(timeFull > 119 && timeFull < 180){ timeShort += "02:"; if(timeFull < 130){ timeShort += "0"; } timeShort += (timeFull - 120); }
  if(timeFull > 179 && timeFull < 240){ timeShort += "03:"; if(timeFull < 190){ timeShort += "0"; } timeShort += (timeFull - 180); }
  if(timeFull > 239 && timeFull < 300){ timeShort += "04:"; if(timeFull < 250){ timeShort += "0"; } timeShort += (timeFull - 240); }
  if(timeFull > 299 && timeFull < 360){ timeShort += "05:"; if(timeFull < 310){ timeShort += "0"; } timeShort += (timeFull - 300); }
  if(timeFull > 359 && timeFull < 420){ timeShort += "06:"; if(timeFull < 370){ timeShort += "0"; } timeShort += (timeFull - 360); }
  if(timeFull > 419 && timeFull < 480){ timeShort += "07:"; if(timeFull < 430){ timeShort += "0"; } timeShort += (timeFull - 420); }
  if(timeFull > 479 && timeFull < 540){ timeShort += "08:"; if(timeFull < 490){ timeShort += "0"; } timeShort += (timeFull - 480); }
  if(timeFull > 539 && timeFull < 600){ timeShort += "09:"; if(timeFull < 550){ timeShort += "0"; } timeShort += (timeFull - 540); }
  if(timeFull > 599 && timeFull < 660){ timeShort += "10:"; if(timeFull < 610){ timeShort += "0"; } timeShort += (timeFull - 600); }
  if(timeFull > 659 && timeFull < 720){ timeShort += "11:"; if(timeFull < 670){ timeShort += "0"; } timeShort += (timeFull - 660); }
  if(timeFull > 719 && timeFull < 780){ timeShort += "12:"; if(timeFull < 730){ timeShort += "0"; } timeShort += (timeFull - 720); }
  if(timeFull > 779 && timeFull < 840){ timeShort += "13:"; if(timeFull < 790){ timeShort += "0"; } timeShort += (timeFull - 780); }
  if(timeFull > 839 && timeFull < 900){ timeShort += "14:"; if(timeFull < 850){ timeShort += "0"; } timeShort += (timeFull - 840); }
  if(timeFull > 899 && timeFull < 960){ timeShort += "15:"; if(timeFull < 910){ timeShort += "0"; } timeShort += (timeFull - 900); }
  if(timeFull > 959 && timeFull < 1020){ timeShort += "16:"; if(timeFull < 970){ timeShort += "0"; } timeShort += (timeFull - 960); }
  if(timeFull > 1019 && timeFull < 1080){ timeShort += "17:"; if(timeFull < 1030){ timeShort += "0"; } timeShort += (timeFull - 1020); }
  if(timeFull > 1079 && timeFull < 1140){ timeShort += "18:"; if(timeFull < 1090){ timeShort += "0"; } timeShort += (timeFull - 1080); }
  if(timeFull > 1139 && timeFull < 1200){ timeShort += "19:"; if(timeFull < 1150){ timeShort += "0"; } timeShort += (timeFull - 1140); }
  if(timeFull > 1199 && timeFull < 1260){ timeShort += "20:"; if(timeFull < 1210){ timeShort += "0"; } timeShort += (timeFull - 1200); }
  if(timeFull > 1259 && timeFull < 1320){ timeShort += "21:"; if(timeFull < 1270){ timeShort += "0"; } timeShort += (timeFull - 1260); }
  if(timeFull > 1319 && timeFull < 1380){ timeShort += "22:"; if(timeFull < 1330){ timeShort += "0"; } timeShort += (timeFull - 1320); }
  if(timeFull > 1379 && timeFull < 1440){ timeShort += "23:"; if(timeFull < 1390){ timeShort += "0"; } timeShort += (timeFull - 1380); }
  if(timeFull > 1439 && timeFull < 1500){ timeShort += "24:"; if(timeFull < 1450){ timeShort += "0"; } timeShort += (timeFull - 1440); }
  lcd.setCursor(0, 1); lcd.print(F("Time: "));      
}
