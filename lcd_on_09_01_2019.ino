

#include "LiquidCrystal.h"
#include "DHT.h"
#include <SparkFunTSL2561.h>
#include <Wire.h>
#include <virtuabotixRTC.h> 
#include <EEPROM.h>

#define DHTPIN 22
#define DHTTYPE DHT22
#define SOIL_MOISTURE_PWR3 50
#define SOIL_MOISTURE_PWR2 51
#define SOIL_MOISTURE_PWR1 52
#define LCD_LIGHT 53
#define LAMP_PWR1 48
#define LAMP_PWR2 47
#define LAMP_PWR3 49
#define LAMP_PWR4 46
#define SOIL_3_IN  A0
#define SOIL_2_IN  A1
#define SOIL_1_IN  A2
#define JOY_X_IN  A4
#define JOY_Y_IN  A3

int Soil_1_Val;
int Soil_2_Val;
int Soil_3_Val;
int Joy_X;
int Joy_Y;
int lcd_mode=0;
int lcd_modex=0;
int lcd_posx=0;
int lcd_modex1=0;
int lcd_posx1=0;
int lcd_posy=0;
int lcd_modey=0;
int flash=0;


struct Soil_Cal_Obj {
int Zero_1=0;
int Zero_2=0;
int Zero_3=0;
int Max_1=0;
int Max_2=0;
int Max_3=0;
};
Soil_Cal_Obj Soil_Cal;

struct Lamp_Obj{
  int Hour_On[3]={0,0,0};
  int Hour_Off[3]={0,0,0};
  int Min_On[3]={0,0,0};
  int Min_Off[3]={0,0,0};
};
Lamp_Obj Lamp;



// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(23, 24, 25);

DHT dht(DHTPIN, DHTTYPE);
// инициализировать библиотеку, указав ей выводы подключения
LiquidCrystal lcd(8,9,4,5,6,7);


// Create an SFE_TSL2561 object, here called "light":

SFE_TSL2561 light;

// Global variables:

boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds

void setup() 
{
  pinMode(SOIL_1_IN, INPUT);
  pinMode(SOIL_2_IN, INPUT);
  pinMode(SOIL_3_IN, INPUT);
  
  pinMode(LAMP_PWR1, OUTPUT); 
  pinMode(LAMP_PWR2, OUTPUT); 
  pinMode(LAMP_PWR3, OUTPUT); 
  pinMode(LAMP_PWR4, OUTPUT); 

  pinMode(SOIL_MOISTURE_PWR1, OUTPUT); 
  pinMode(SOIL_MOISTURE_PWR2, OUTPUT); 
  pinMode(SOIL_MOISTURE_PWR3, OUTPUT); 
  pinMode(LCD_LIGHT, OUTPUT); 
  digitalWrite(SOIL_MOISTURE_PWR1, HIGH); 
  digitalWrite(SOIL_MOISTURE_PWR2, HIGH); 
  digitalWrite(SOIL_MOISTURE_PWR3, HIGH); 
  digitalWrite(LCD_LIGHT, LOW); 
  digitalWrite(LAMP_PWR1, HIGH); 
  digitalWrite(LAMP_PWR2, HIGH); 
  digitalWrite(LAMP_PWR3, HIGH); 
  digitalWrite(LAMP_PWR4, HIGH); 
  
  lcd.begin(20,4);

  // установить курсор в начало первой строки LCD дисплея
  lcd.setCursor(0,0);
  // напечатать текст
  lcd.print("HIMALAYAN FARM v1.0");
  // установить курсор в начало следующей строки
  lcd.setCursor(0,1);
  lcd.print("AIR: ");
  lcd.setCursor(0,2);
  lcd.print("LIGHT: ");

//  myRTC.setDS1302Time(43, 26, 21, 6, 13, 1, 2019);
/*  Serial.begin(9600);
  Serial.println("DHTxx test!");*/
  dht.begin();

  light.begin();



  // Get factory ID from sensor:
  // (Just for fun, you don't need to do this to operate the sensor)

  unsigned char ID;
  
  if (light.getID(ID))
  {
  }
  else
  {
    byte error = light.getError();
  }

  // The light sensor has a default integration time of 402ms,
  // and a default gain of low (1X).
  
  // If you would like to change either of these, you can
  // do so using the setTiming() command.
  
  // If gain = false (0), device is set to low gain (1X)
  // If gain = high (1), device is set to high gain (16X)

  gain = 0;

  // If time = 0, integration will be 13.7ms
  // If time = 1, integration will be 101ms
  // If time = 2, integration will be 402ms
  // If time = 3, use manual start / stop to perform your own integration

  unsigned char time = 2;

  // setTiming() will set the third parameter (ms) to the
  // requested integration time in ms (this will be useful later):
  
 // Serial.println("Set timing...");
  light.setTiming(gain,time,ms);

  // To start taking measurements, power up the sensor:
  
//  Serial.println("Powerup...");
  light.setPowerUp();
  
  // The sensor will now gather light during the integration time.
  // After the specified time, you can retrieve the result from the sensor.
  // Once a measurement occurs, another integration period will start.

  lcd.setCursor(0,3);
  lcd.print("SOIL 1: ");

  EEPROM.get(0x00, Soil_Cal);
  EEPROM.get(30, Lamp);
    delay(300);
//    time.begin();
}

void loop()
{
float h = dht.readHumidity();
float t = dht.readTemperature();
int i=0;

flash++;
if (flash>=200) flash=0;

myRTC.updateTime(); 

if ((Now_Min(myRTC.hours, myRTC.minutes)>Now_Min(Lamp.Hour_On[0], Lamp.Min_On[0]))&&(lcd_mode==0)) digitalWrite(LAMP_PWR1, LOW); 
else digitalWrite(LAMP_PWR1, HIGH); 
if ((Now_Min(myRTC.hours, myRTC.minutes)>Now_Min(Lamp.Hour_On[1], Lamp.Min_On[1]))&&(lcd_mode==0))  digitalWrite(LAMP_PWR2, LOW); 
else digitalWrite(LAMP_PWR2, HIGH); 
//if (Now_Min(myRTC.hours, myRTC.minutes)>Now_Min(Lamp.Hour_On[2], Lamp.Min_On[2])) digitalWrite(LAMP_PWR3, LOW); 
//else digitalWrite(LAMP_PWR3, HIGH); 
  Joy_X = analogRead(JOY_X_IN);
  Joy_Y = analogRead(JOY_Y_IN);

/*  digitalWrite(LAMP_PWR1, LOW); 
  digitalWrite(LAMP_PWR2, LOW); 
  digitalWrite(LAMP_PWR3, LOW); 
  digitalWrite(LAMP_PWR4, LOW); 
*/
if (lcd_mode==2)
{

    if ((lcd_posx1==0))
  {
      lcd.setCursor(1,lcd_posy);
      if (flash>=100) lcd.print(" ");
      else lcd.print("*");
  }
  if ((lcd_posy>0)&&(lcd_posx1==1))
  {
      lcd.setCursor(5,lcd_posy);
      if (flash>=100) lcd.print("  ");
      else TM_Out(Lamp.Hour_On[lcd_posy-1]);
  }
  if ((lcd_posy>0)&&(lcd_posx1==2))
  {
      lcd.setCursor(8,lcd_posy);
      if (flash>=100) lcd.print("  ");
      else TM_Out(Lamp.Min_On[lcd_posy-1]);
  }
  if ((lcd_posy>0)&&(lcd_posx1==3))
  {
      lcd.setCursor(15,lcd_posy);
      if (flash>=100) lcd.print("  ");
      else TM_Out(Lamp.Hour_Off[lcd_posy-1]);
  }
  if ((lcd_posy>0)&&(lcd_posx1==4))
  {
      lcd.setCursor(18,lcd_posy);
      if (flash>=100) lcd.print("  ");
      else TM_Out(Lamp.Min_Off[lcd_posy-1]);
  }

  if (((Joy_X<=950)&&(Joy_X>=50))&&(lcd_modex1>=50))
  {
    if (lcd_posx1==1) {lcd.setCursor(5,lcd_posy); TM_Out(Lamp.Hour_On[lcd_posy-1]);}
    if (lcd_posx1==2) {lcd.setCursor(8,lcd_posy); TM_Out(Lamp.Min_On[lcd_posy-1]);}
    if (lcd_posx1==3) {lcd.setCursor(15,lcd_posy); TM_Out(Lamp.Hour_Off[lcd_posy-1]);}
    if (lcd_posx1==4) {lcd.setCursor(18,lcd_posy); TM_Out(Lamp.Min_Off[lcd_posy-1]);}
    lcd_posx1=lcd_modex1-50;
    lcd_modex1=0;

  }


    
  if (((Joy_Y<=950)&&(Joy_Y>=50))&&(lcd_modey>=50))

{
  switch (lcd_posx1)
  {
    case 0: lcd.setCursor(1,lcd_posy);
            lcd.print(" ");
            lcd_posy=lcd_modey-50;
            lcd.setCursor(1,lcd_posy);
            lcd.print("*");
            lcd_modey=0;
            break;
    case 1: if ((lcd_modey-lcd_posy-50)>0) Lamp.Hour_On[lcd_posy-1]--;
            else  Lamp.Hour_On[lcd_posy-1]++;
            if (Lamp.Hour_On[lcd_posy-1]<0)Lamp.Hour_On[lcd_posy-1]=24;
            if (Lamp.Hour_On[lcd_posy-1]>24)Lamp.Hour_On[lcd_posy-1]=0;
            lcd.setCursor(5,lcd_posy);
            TM_Out(Lamp.Hour_On[lcd_posy-1]);
            lcd_modey=0;
            break;
    case 2: if ((lcd_modey-lcd_posy-50)>0) Lamp.Min_On[lcd_posy-1]-=5;
            else  Lamp.Min_On[lcd_posy-1]+=5;
            if (Lamp.Min_On[lcd_posy-1]<0)Lamp.Min_On[lcd_posy-1]=55;
            if (Lamp.Min_On[lcd_posy-1]>59)Lamp.Min_On[lcd_posy-1]=0;
            lcd.setCursor(8,lcd_posy);
            TM_Out(Lamp.Min_On[lcd_posy-1]);
            lcd_modey=0;
            break;
    case 3: if ((lcd_modey-lcd_posy-50)>0) Lamp.Hour_Off[lcd_posy-1]--;
            else  Lamp.Hour_Off[lcd_posy-1]++;
            if (Lamp.Hour_Off[lcd_posy-1]<0)Lamp.Hour_Off[lcd_posy-1]=24;
            if (Lamp.Hour_Off[lcd_posy-1]>24)Lamp.Hour_Off[lcd_posy-1]=0;
            lcd.setCursor(15,lcd_posy);
            TM_Out(Lamp.Hour_Off[lcd_posy-1]);
            lcd_modey=0;
            break;
    case 4: if ((lcd_modey-lcd_posy-50)>0) Lamp.Min_Off[lcd_posy-1]-=5;
            else  Lamp.Min_Off[lcd_posy-1]+=5;
            if (Lamp.Min_Off[lcd_posy-1]<0)Lamp.Min_Off[lcd_posy-1]=55;
            if (Lamp.Min_Off[lcd_posy-1]>59)Lamp.Min_Off[lcd_posy-1]=0;
            lcd.setCursor(18,lcd_posy);
            TM_Out(Lamp.Min_Off[lcd_posy-1]);
            lcd_modey=0;
            break;
            
  }

}
if ((lcd_posx1<=3)&&(Joy_X<50))
{
  lcd_modex1=lcd_posx1+1+50;
}
if ((lcd_posx1>=1)&&(Joy_X>950))
{
  lcd_modex1=lcd_posx1-1+50;
}

if ((lcd_posy>=1)&&(Joy_Y<50))
{
  lcd_modey=lcd_posy-1+50;
}
if (((lcd_posy!=3)||(lcd_posx1>0))&&(Joy_Y>950))
{
  lcd_modey=lcd_posy+1+50;
}
if ((lcd_posy!=3)&&(Joy_Y>950))
{
  lcd_modey=lcd_posy+1+50;
}

}
if (((Joy_X<=950)&&(Joy_X>=50))&&(lcd_modex>=50))
{
  if (lcd_mode==2)  EEPROM.put(30, Lamp);
  lcd_mode=lcd_modex-50;
if (lcd_mode==0)
{
  lcd.setCursor(0,1);
  lcd.print("                    ");
  lcd.setCursor(0,2);
  lcd.print("                    ");
  lcd.setCursor(0,3);
  lcd.print("                    ");
  lcd.setCursor(0,1);
  lcd.print("AIR: ");
  lcd.setCursor(0,2);
  lcd.print("LIGHT: ");
  lcd_modex=0;
}
if (lcd_mode==1)
{
  lcd.setCursor(0,0);
  lcd.print("HIMALAYAN FARM v1.0 ");
  lcd.setCursor(0,1);
  lcd.print("                    ");
  lcd.setCursor(0,2);
  lcd.print("                    ");
  lcd.setCursor(0,3);
  lcd.print("                    ");
  digitalWrite(SOIL_MOISTURE_PWR1, LOW);
  digitalWrite(SOIL_MOISTURE_PWR2, LOW);
  digitalWrite(SOIL_MOISTURE_PWR3, LOW);
  lcd_modex=0;
}
if (lcd_mode==2)
{
  lcd.setCursor(0,0);
  lcd.print("0*");
  TM_Out(myRTC.dayofmonth);
  lcd.print(".");                                                                                     //| 
  TM_Out(myRTC.month);
  lcd.print(".");                                                                                     //| 
  TM_Out(myRTC.year-2000);
  lcd.print("  "); 
  TM_Out(myRTC.hours);
  lcd.print(":");                                                                                     //| 
  TM_Out(myRTC.minutes);
  lcd.print(":");                                                                                     //| 
  TM_Out(myRTC.seconds);

for(i=0;i<=2;i++)
{
  lcd.setCursor(0,i+1);
  lcd.print(i+1);
  lcd.print(" ON-");
  TM_Out(Lamp.Hour_On[i]);
  lcd.print(":");                                                                                     //| 
  TM_Out(Lamp.Min_On[i]);
  lcd.print(" OFF-");
  TM_Out(Lamp.Hour_Off[i]);
  lcd.print(":");                                                                                     //| 
  TM_Out(Lamp.Min_Off[i]);
}
  lcd_modex=0;
  lcd_posy=0;
  lcd_posx=0;
  lcd_modey=0;
  }
}
if ((lcd_mode==1)&&(Joy_X>950))
{
  lcd_modex=50;
}
if ((lcd_mode==0)&&(Joy_X<50))
{
  lcd_modex=51;
}
if ((lcd_mode==1)&&(Joy_X<50))
{
  lcd_modex=52;
}
if ((lcd_mode==2)&&(Joy_X>950)&&(lcd_posx1==0))
{
  lcd_modex=51;
}

if (lcd_mode==0)
{
if (isnan(t) || isnan(h)) {
lcd.setCursor(5,1); 
lcd.println("Fail DHT");
}
else {
  lcd.setCursor(5,1);
  lcd.print("               ");
  lcd.setCursor(5,1);
  lcd.print(h);
  lcd.print(" %");
  lcd.setCursor(13,1);
  lcd.print(t);
  lcd.print(" C");
/*  
Serial.print("Humidity: ");
Serial.print(h);
Serial.print(" %\t");
Serial.print("Temperature: ");
Serial.print(t);
Serial.println(" *C");*/
} 


lcd.setCursor(0,0);
// Start printing elements as individuals                                                                //|   
  
  TM_Out(myRTC.dayofmonth);
  lcd.print(".");                                                                                     //| 
  TM_Out(myRTC.month);
  lcd.print(".");                                                                                     //| 
  TM_Out(myRTC.year);
  lcd.print("  "); 
  TM_Out(myRTC.hours);
  lcd.print(":");                                                                                     //| 
  TM_Out(myRTC.minutes);
  lcd.print(":");                                                                                     //| 
  TM_Out(myRTC.seconds);
                                                                                                       //| 
// Delay so the program doesn't print non-stop                                                           //| 
  
  // Wait between measurements before retrieving the result
  // (You can also configure the sensor to issue an interrupt
  // when measurements are complete)
  
  // This sketch uses the TSL2561's built-in integration timer.
  // You can also perform your own manual integration timing
  // by setting "time" to 3 (manual) in setTiming(),
  // then performing a manualStart() and a manualStop() as in the below
  // commented statements:
  
  // ms = 1000;
  // light.manualStart();
  delay(ms);
  // light.manualStop();
  
  // Once integration is complete, we'll retrieve the data.
  
  // There are two light sensors on the device, one for visible light
  // and one for infrared. Both sensors are needed for lux calculations.
  
  // Retrieve the data from the device:

  unsigned int data0, data1;
  
  if (light.getData(data0,data1))
  {
    // getData() returned true, communication was successful
    
 //   Serial.print("data0: ");
 //   Serial.print(data0);
 //   Serial.print(" data1: ");
 //   Serial.print(data1);
  
    // To calculate lux, pass all your settings and readings
    // to the getLux() function.
    
    // The getLux() function will return 1 if the calculation
    // was successful, or 0 if one or both of the sensors was
    // saturated (too much light). If this happens, you can
    // reduce the integration time and/or gain.
    // For more information see the hookup guide at: https://learn.sparkfun.com/tutorials/getting-started-with-the-tsl2561-luminosity-sensor
  
    double lux;    // Resulting lux value
    boolean good;  // True if neither sensor is saturated
    
    // Perform lux calculation:

    good = light.getLux(gain,ms,data0,data1,lux);
    
    // Print out the results:
  lcd.setCursor(8,2);
  lcd.print("           ");
  lcd.setCursor(8,2);
  lcd.print(lux);
  lcd.print(" lux");
//    Serial.print(" lux: ");
//    Serial.print(lux);
//    if (good) Serial.println(" (good)"); else Serial.println(" (BAD)");
  }
  else
  {
    // getData() returned false because of an I2C error, inform the user.

    byte error = light.getError();
  //  printError(error);
  }

  if ((myRTC.seconds<20)&&((myRTC.minutes==0)||(myRTC.minutes==30))) {digitalWrite(SOIL_MOISTURE_PWR1, LOW);}
  else {digitalWrite(SOIL_MOISTURE_PWR1, HIGH);}
  if (((myRTC.seconds>=20)&&(myRTC.seconds<40))&&((myRTC.minutes==30)||(myRTC.minutes==0))) {digitalWrite(SOIL_MOISTURE_PWR2, LOW);}
  else {digitalWrite(SOIL_MOISTURE_PWR2, HIGH);}
  if ((myRTC.seconds>=40)&&((myRTC.minutes==0)||(myRTC.minutes==30))) {digitalWrite(SOIL_MOISTURE_PWR3, LOW);}
  else {digitalWrite(SOIL_MOISTURE_PWR3, HIGH);}

   if (((myRTC.seconds>3)&&(myRTC.seconds<19))&&((myRTC.minutes==0)||(myRTC.minutes==30))) Soil_1_Val = analogRead(SOIL_1_IN);
   if (((myRTC.seconds>23)&&(myRTC.seconds<39))&&((myRTC.minutes==0)||(myRTC.minutes==30))) Soil_2_Val = analogRead(SOIL_2_IN);
   if (((myRTC.seconds>43)&&(myRTC.seconds<59))&&((myRTC.minutes==0)||(myRTC.minutes==30))) Soil_3_Val = analogRead(SOIL_3_IN);
  lcd.setCursor(0,3);
  lcd.print("                ");
  if (myRTC.seconds<20)
  {
    lcd.setCursor(0,3);
    lcd.print("SOIL 1: ");
    lcd.print(map(Soil_1_Val,Soil_Cal.Zero_1,Soil_Cal.Max_1,0,100));
    lcd.print("    ");
  }
  if ((myRTC.seconds>=20)&&(myRTC.seconds<40)) 
  {
    lcd.setCursor(0,3);
    lcd.print("SOIL 2: ");
    lcd.print(map(Soil_2_Val,Soil_Cal.Zero_2,Soil_Cal.Max_2,0,100));
    lcd.print("    ");
  }
  if (myRTC.seconds>=40) 
  {
    lcd.setCursor(0,3);
    lcd.print("SOIL 3: ");
    lcd.print(map(Soil_3_Val,Soil_Cal.Zero_3,Soil_Cal.Max_3,0,100));
    lcd.print("    ");
  }
}
if (lcd_mode==1)
{
    Soil_1_Val = analogRead(SOIL_1_IN);
    Soil_2_Val = analogRead(SOIL_2_IN);
    Soil_3_Val = analogRead(SOIL_3_IN);

    if (Joy_Y>950) 
    {
      Soil_Cal.Zero_1=Soil_1_Val;
      Soil_Cal.Zero_2=Soil_2_Val;
      Soil_Cal.Zero_3=Soil_3_Val;
      EEPROM.put(0x00, Soil_Cal);
    }
    if (Joy_Y<50) 
    {
      Soil_Cal.Max_1=Soil_1_Val;
      Soil_Cal.Max_2=Soil_2_Val;
      Soil_Cal.Max_3=Soil_3_Val;
      EEPROM.put(0x00, Soil_Cal);
    }

    
    lcd.setCursor(0,1);
    lcd.print("SOIL 1: ");
    lcd.print(Soil_1_Val-Soil_Cal.Zero_1);
    lcd.print("     ");  
    lcd.setCursor(15,1);
    lcd.print("     ");  
    lcd.setCursor(15,1);
    lcd.print(map(Soil_1_Val,Soil_Cal.Zero_1,Soil_Cal.Max_1,0,100));
    lcd.setCursor(0,2);
    lcd.print("SOIL 2: ");
    lcd.print(Soil_2_Val-Soil_Cal.Zero_2);
    lcd.print("    ");  
    lcd.setCursor(15,2);
    lcd.print("     ");  
    lcd.setCursor(15,2);
    lcd.print(map(Soil_2_Val,Soil_Cal.Zero_2,Soil_Cal.Max_2,0,100));
    lcd.setCursor(0,3);
    lcd.print("SOIL 3: ");
    lcd.print(Soil_3_Val-Soil_Cal.Zero_3);
    lcd.print("    ");  
    lcd.setCursor(15,3);
    lcd.print("     ");  
    lcd.setCursor(15,3);
    lcd.print(map(Soil_3_Val,Soil_Cal.Zero_3,Soil_Cal.Max_3,0,100));
    delay(200);

}
}
void TM_Out(int i)
{
  if (i<10) lcd.print("0");
  lcd.print(i);                                                                        //| 
}

int Now_Min(int Hour, int Minute)
{
int i;
i=Hour*60+Minute;
return (i);  
}

