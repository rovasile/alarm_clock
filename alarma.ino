#include "SD.h"
#define SD_ChipSelectPin 10
#include "TMRpcm.h"
#include "SPI.h"
#include <LiquidCrystal.h>
#include "RTClib.h"
#include <EEPROM.h>

TMRpcm tmrpcm;
LiquidCrystal lcd = LiquidCrystal(2, 3, 4, 5, 6, 7);
RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"D", "L", "M", "m", "J", "V", "S"};


int button1=A0, button2=A1, button3=A2, button4=A3;
int alarmDay[7]={0,0,0,0,0,0,0};
int dayHour[7]={0,0,0,0,0,0,0};
int dayMinute[7]={0,0,0,0,0,0,0};
int currentDay=0;

void setup() {
lcd.begin(16, 2);
tmrpcm.speakerPin=9;
Serial.begin(9600);
if(!SD.begin(SD_ChipSelectPin))
{
  Serial.println("SD fail");
  return;
}

if(!rtc.begin()) Serial.println("RTC ERROR");
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  loadAlarms();
}



DateTime now;
int ringtoneNumber=0, ringtones=2;


void loop() {
currentTime();
buttonActions();
alarmDays();
alarmTime();
//saveAlarms();
checkAlarm();



}

void currentTime()
{ 
  
    now = rtc.now();

lcd.setCursor(0,0);
lcd.print(now.hour());
lcd.print(":");
lcd.print(now.minute());
lcd.print(" ");
lcd.setCursor(6,0);
lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);

}

void alarmDays()
{
 if(alarmDay[0]==1)    //                       LUNII E ALARMA
{lcd.setCursor(9,0);
lcd.print("L");}
else
{lcd.setCursor(9,0);
lcd.print(" ");}

 if(alarmDay[1]==1)  //                          MARTI E ALARMA
{lcd.setCursor(10,0);
lcd.print("M");}
else
{lcd.setCursor(10,0);
lcd.print(" ");}

 if(alarmDay[2]==1)  //                          MIERCURI E ALARMA
{lcd.setCursor(11,0);
lcd.print("m");}
else
{lcd.setCursor(11,0);
lcd.print(" ");}

 if(alarmDay[3]==1)  //                          JOI E ALARMA
{lcd.setCursor(12,0);
lcd.print("J");}
else
{lcd.setCursor(12,0);
lcd.print(" ");}

 if(alarmDay[4]==1)  //                          VINERI E ALARMA
{lcd.setCursor(13,0);
lcd.print("V");}
else
{lcd.setCursor(13,0);
lcd.print(" ");}

 if(alarmDay[5]==1)  //                          SAMBATA E ALARMA
{lcd.setCursor(14,0);
lcd.print("S");}
else
{lcd.setCursor(14,0);
lcd.print(" ");}

 if(alarmDay[6]==1)  //                          DUMINICA E ALARMA
{lcd.setCursor(15,0);
lcd.print("D");}
else
{lcd.setCursor(15,0);
lcd.print(" ");}

  lcd.setCursor(0,1);
  lcd.print("Zi:");
  lcd.print(daysOfTheWeek[(currentDay+1)%7]);
  if(alarmDay[currentDay]==1)
  {lcd.setCursor(4,1); lcd.print("*");}
  else
  {lcd.setCursor(4,1); lcd.print(" ");}

  lcd.setCursor(6,1); lcd.print(ringtoneNumber);
  
  
}

String ringtone = "0.wav";

void buttonActions()
{  



  if (tmrpcm.isPlaying())
  {
    if (digitalRead(button1)==0 || digitalRead(button2)==0 || digitalRead(button3)==0 || digitalRead(button4)==0)
    {
      tmrpcm.disable();
      while(digitalRead(button1)==0 || digitalRead(button2)==0 || digitalRead(button3)==0 || digitalRead(button4)==0);
    }
      
  }
int backup;
  
  if (digitalRead(button1)==0)        // BUTONUL CARE SCHIMBA ZIUA
    {
      currentDay=(currentDay+1)%7;
      saveAlarms();
      Serial.println("Btn1 pressed.");
      while(digitalRead(button1)==0);
    }
  if (digitalRead(button2)==0)        // BUTONUL CARE SETEAZA/ANULEAZA ALARMA
    {alarmDay[currentDay]=!alarmDay[currentDay];
    saveAlarms();
    Serial.println("Btn2 pressed.");
      while(digitalRead(button2)==0);
    }
  if (digitalRead(button4)==0)        // butonul care SETEAZA ORA
    {backup=dayHour[currentDay];
      dayHour[currentDay]=(dayHour[currentDay]+1)%24;
      Serial.println("Btn4 pressed.");
      while(digitalRead(button4)==0)
      if (digitalRead(button3)==0)
        {ringtoneNumber=(ringtoneNumber+1)%ringtones; changeRingtone();dayHour[currentDay]=backup;
        while(digitalRead(button3)==0);
        }
        
    }
  if (digitalRead(button3)==0)        // BUTONUL CARE SETEAZA MINUTELE
    {
      dayMinute[currentDay]=(dayMinute[currentDay]+2)%60;
      Serial.println("Btn3 pressed.");
      while(digitalRead(button3)==0);
    }
    
  
}

void changeRingtone()
{
  ringtone=String(String(ringtoneNumber)+".wav");
  
}

void alarmTime()
{
  lcd.setCursor(11,1);
  lcd.print("     ");
  
  lcd.setCursor(11,1);
  lcd.print(dayHour[currentDay]);
  lcd.setCursor(13,1);
  lcd.print(":");
  lcd.setCursor(14,1);
  lcd.print(dayMinute[currentDay]);
  
}



int adjustedDay;

void checkAlarm()
{
  adjustedDay=(now.dayOfTheWeek()+6)%7;
if (alarmDay[adjustedDay] && now.hour()==dayHour[adjustedDay] && now.minute()==dayMinute[adjustedDay] && now.second()==0)
{ 
  delay(1000);
  alarmAction();
}

}


char ringtoneChar[6];
void alarmAction()
{
tmrpcm.setVolume(5);
ringtone.toCharArray(ringtoneChar,6);
Serial.print(ringtone);Serial.print(" ");Serial.println(ringtoneChar); 

tmrpcm.play(ringtoneChar);
tmrpcm.loop(1);
}


int startAddress=22;
int address,i;
void saveAlarms()
{
 address=startAddress; 
for (i=0; i<7;i++)
{
  EEPROM.write(address,alarmDay[i]);
  address++;
}
for (i=0; i<7;i++)
{
  EEPROM.write(address,dayHour[i]);
  address++;
}

for (i=0; i<7;i++)
{
  EEPROM.write(address,dayMinute[i]);
  address++;
}

  
}

void loadAlarms()
{
 address=startAddress; 
for (i=0; i<7;i++)
{
  alarmDay[i]=EEPROM.read(address);
  address++;
}
for (i=0; i<7;i++)
{
  dayHour[i]=EEPROM.read(address);
  address++;
}

for (i=0; i<7;i++)
{
  dayMinute[i]=EEPROM.read(address);
  address++;
}
  
}
