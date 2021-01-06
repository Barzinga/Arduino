#include <IRremote.h>
#include <Stepper.h>
#include <MotorDC.h>
#include <LCD.h> 
#include <LiquidCrystal_I2C.h> 

const int DC_MotorSpeed = 150;  
int outputValue = 0;
int ContorlVar = 0;
int ContorlVar1 = 0;
int apin = 0;//For frog;

const int LightPin = A1; //光敏电阻
const int ledPin = 7;
const int RainDropAnalog = A0;
const int RainDropPin = 6;
const int irReceiverPin = 5;
const int StepMotor_1 = 10;
const int StepMotor_2 = 11;
const int StepMotor_3 = 12;
const int StepMotor_4 = 13;
//const int PMPower = 4;
//const int PM_measurePin = A2;            
const int DC_Motor_1 = 8;
const int DC_Motor_2 = 9;
const int FrogPin = A2;
/*
//PM
unsigned int samplingTime = 280;  //取样时间,单位毫秒
unsigned int deltaTime = 40;
unsigned int sleepTime = 9680;    //休眠

int measurePin = 2;

float voMeasured = 0;   //电压取样值
float calcVoltage = 0;  //电压初始值
float dustDensity = 0;  //灰尘密度初始值
int ledPower = LOW;
*/
IRrecv irrecv(irReceiverPin);
decode_results results;
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified backpack 

void setup()

{
  pinMode(ledPin,OUTPUT); 
  pinMode(StepMotor_1, OUTPUT);
  pinMode(StepMotor_2, OUTPUT);
  pinMode(StepMotor_3, OUTPUT);
  pinMode(StepMotor_4, OUTPUT); 
  pinMode(DC_Motor_1,OUTPUT);
  pinMode(DC_Motor_2,OUTPUT);
  pinMode(RainDropPin, INPUT); 
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  
  Serial.begin(9600);
  irrecv.enableIRIn();
  lcd.begin (16,2); // for 16 x 2 LCD module 
  lcd.setBacklightPin(3,POSITIVE); //背光引脚
  lcd.setBacklight(HIGH); 
}

/*void LCD_Sensor()
{
  lcd.setCursor (0,0);
  lcd.print("Mode"); 
  lcd.setCursor (3,1); // go to start of 2nd line 
  lcd.print("step"); 
  delay(5000); 
}*/

void loop()
{
  //LCD_Sensor();
  lcd.setCursor (0,0);
  lcd.print("Welcome!");
  delay(3000);
  lcd.clear(); 
  Mode();
}

void Mode()
{
  while(1)
  {
    RemoteContorl();
    lcd.setCursor (0,0);
    lcd.print("Press'CH-'Manual");
    lcd.setCursor (0,1);
    lcd.print("Press'CH+' Auto");
    if(results.value == 0xFFA25D)//CH-
    {
      lcd.clear();
      Manual();
    }
    else if(results.value == 0xFFE21D)//CH+
    {
      lcd.clear();
      Automatic();
    }
  }
}

void RemoteContorl()
{
  delay(500);
  if(irrecv.decode(&results))
  {
    Serial.print("irCode:");
    Serial.println(results.value,HEX);
    Serial.print("bits:");
    Serial.println(results.bits);
    irrecv.resume();
  }
  else
  {
    Serial.println("无接收!");
  }
  delay(300);
}



void Automatic()
{
  while(1)
  {
    lcd.setCursor (0,0);
    lcd.print("Press'7' RainDrop");
    lcd.setCursor (0,1);
    lcd.print("Press'8' to exit");
    Light_Sensor();
    Frog_Sensor();
    RemoteContorl();
    if(results.value == 0xFF42BD)//7
    {
      while(1)
      {
        RemoteContorl();
        RainDrop_Sensor();
        if(results.value == 0xFF4AB5)//8
        {
          break;
        }
      }
    }
    /*PM_Sensor();
    */
    if(results.value == 0xFF906F)//EQ
    {
      lcd.clear();
      break;
    }
  }
}

void RainDrop_Sensor()
{
  int Astate=0;
  boolean Dstate=0;
  Astate=analogRead(RainDropAnalog);
  Serial.print("A0:");
  Serial.println(Astate);
  Dstate=digitalRead(RainDropPin);
  Serial.print("D0:");
  Serial.println(Dstate);
  if(Dstate==HIGH)//没下
  {
    if(ContorlVar==1)
    {
      StepMotor(-360,1);
      delay(300);
      ContorlVar=0;
    }
  }
  else
  {
    if(ContorlVar==0)
    {
      StepMotor(360,1);
      delay(300);
      ContorlVar=1;
    }
  }
}

void Light_Sensor()
{
  outputValue = analogRead(LightPin);
  Serial.println(outputValue); 
  if(outputValue >= 400) 
  {
    if(ContorlVar1==0)
    {
      digitalWrite(ledPin,HIGH);
      StepMotor(-360,1);
      Stop();
      delay(500); 
      ContorlVar1=1;
    }
  }
  else
  {
    if(ContorlVar1==1)
    {
      digitalWrite(ledPin,LOW); 
      StepMotor(360,1);
      Stop();
      delay(500); 
      ContorlVar1=0;
    }
  }
}
/*
void PM_Sensor()
{
  digitalWrite(ledPower,LOW);
  delayMicroseconds(samplingTime);  //空出取样时间
  voMeasured = analogRead(measurePin);  //电压实时取样

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH);
  delayMicroseconds(sleepTime);

  calcVoltage = voMeasured*(5.0/1024);  //测量电压值
  dustDensity = 0.17*calcVoltage-0.1;   //灰尘密度值

  if(dustDensity < 0)
  {
    dustDensity = 0.00;  
  }

  Serial.println("Voltage: ");
  Serial.println(calcVoltage);

  Serial.println("Dust Density: ");
  Serial.println(dustDensity);

  delay(1000);    //1000ms延迟后继续下一次测量
  
}
*/

void Frog_Sensor()
{
  apin=analogRead(FrogPin);
  Serial.print("PM: ");
  Serial.println(apin);
  if(apin>=350)
  {
    digitalWrite(2,HIGH);
    delay(1000);
    digitalWrite(2,LOW);
    digitalWrite(3,HIGH);
    delay(1000);
    digitalWrite(3,LOW);
    digitalWrite(4,HIGH);
    delay(1000);
    digitalWrite(4,LOW);
  }
}

void Manual()
{
  while(1)
  {
    RemoteContorl();
    lcd.setCursor (0,0);
    lcd.print("Press'<-' is fan");
    lcd.setCursor (0,1);
    lcd.print("Press'->' is win");
    if(results.value == 0xFF22DD)//后退
    {
      lcd.clear();
      MFan();
    }
    else if(results.value == 0xFF02FD)//前进
    {
      lcd.clear();
      MWindow();
    }
    else if(results.value == 0xFFC23D)//暂停
    {
      lcd.clear();
      break;
    }
  }
}

void MFan()
{
  while(1)
  {
    RemoteContorl();
    lcd.setCursor (0,0);
    lcd.print("Press'4'open '5'");
    lcd.setCursor (0,1);
    lcd.print(" close '6'exit  ");
    if(results.value == 0xFF10EF)//打开;4
    {
      analogWrite(DC_Motor_1,0);
      analogWrite(DC_Motor_2,DC_MotorSpeed);
      /*while(1)
      {
        if(results.value == )//判断风扇速度
        {
          Speed = DC_MotorSpeed-30;
          analogWrite(DC_Motor_1,0);
          analogWrite(DC_Motor_2,Speed);
        }
        else if(results.value == )
        {
          Speed = DC_MotorSpeed+30;
          analogWrite(DC_Motor_1,0);
          analogWrite(DC_Motor_2,Speed);
        }
        else if(results.value == )
        {
          break;
        }
      }*/
    }
    else if(results.value == 0xFF38C7)//停止;5
    {
      analogWrite(DC_Motor_1,0);
      analogWrite(DC_Motor_2,0);
    }
    else if(results.value == 0xFF5AA5)//6
    {
      lcd.clear();
      break;
    }
  }
}


void MWindow()
{
  while(1)
  {
    RemoteContorl();
    lcd.setCursor (0,0);
    lcd.print("'1'up '2'down"); 
    lcd.setCursor (0,1); // go to start of 2nd line 
    lcd.print("'3'exit'9'stop"); 

    if(results.value == 0xFF30CF)//1
    {
      StepMotor(360,1);
      Stop();
      delay(10); 
    }
    else if(results.value == 0xFF18E7)//2
    {
      StepMotor(-360,1);
      Stop();
      delay(10);
    }
    else if(results.value == 0xFF52AD)//9
    {
      Stop();
      delay(10);
    }
    else if(results.value == 0xFF7A85)//3
    {
      lcd.clear();
      break;
    }
  }
}

void StepMotor(int angle,int speeds)
{
  for(int i=0;i<abs(angle);i++)
  {
    if(angle>0)
    {
      for(int j=0;j<7;j++)
      {
        switch(j){
          case 0:
            digitalWrite(StepMotor_1, LOW);
            digitalWrite(StepMotor_2, LOW);
            digitalWrite(StepMotor_3, LOW);
            digitalWrite(StepMotor_4, HIGH);
          break;
          case 1:
            digitalWrite(StepMotor_1, LOW);
            digitalWrite(StepMotor_2, LOW);
            digitalWrite(StepMotor_3, HIGH);
            digitalWrite(StepMotor_4, HIGH);
          break;
      
          case 2:
            digitalWrite(StepMotor_1, LOW);
            digitalWrite(StepMotor_2, LOW);
            digitalWrite(StepMotor_3, HIGH);
            digitalWrite(StepMotor_4, LOW);
          break;
      
          case 3:
            digitalWrite(StepMotor_1, LOW);
            digitalWrite(StepMotor_2, HIGH);
            digitalWrite(StepMotor_3, HIGH);
            digitalWrite(StepMotor_4, LOW);
          break;
      
          case 4:
            digitalWrite(StepMotor_1, LOW);
            digitalWrite(StepMotor_2, HIGH);
            digitalWrite(StepMotor_3, LOW);
            digitalWrite(StepMotor_4, LOW);
          break;
      
          case 5:
            digitalWrite(StepMotor_1, HIGH);
            digitalWrite(StepMotor_2, HIGH);
            digitalWrite(StepMotor_3, LOW);
            digitalWrite(StepMotor_4, LOW);
            break;
      
            case 6:
            digitalWrite(StepMotor_1, HIGH);
            digitalWrite(StepMotor_2, LOW);
            digitalWrite(StepMotor_3, LOW);
            digitalWrite(StepMotor_4, LOW);
          break;
      
          case 7:
            digitalWrite(StepMotor_1, HIGH);
            digitalWrite(StepMotor_2, LOW);
            digitalWrite(StepMotor_3, LOW);
            digitalWrite(StepMotor_4, HIGH);
          break;
      
          default:
            digitalWrite(StepMotor_1, LOW);
            digitalWrite(StepMotor_2, LOW);
            digitalWrite(StepMotor_3, LOW);
            digitalWrite(StepMotor_4, LOW);
          break;
        }
        delay(speeds);
      }
    }
    else
    {
      for(int k=7;k>0;k--)
      {
        switch(k){
          case 0:
            digitalWrite(StepMotor_1, LOW);
            digitalWrite(StepMotor_2, LOW);
            digitalWrite(StepMotor_3, LOW);
            digitalWrite(StepMotor_4, HIGH);
          break;
          case 1:
            digitalWrite(StepMotor_1, LOW);
            digitalWrite(StepMotor_2, LOW);
            digitalWrite(StepMotor_3, HIGH);
            digitalWrite(StepMotor_4, HIGH);
          break;
      
          case 2:
            digitalWrite(StepMotor_1, LOW);
            digitalWrite(StepMotor_2, LOW);
            digitalWrite(StepMotor_3, HIGH);
            digitalWrite(StepMotor_4, LOW);
          break;
      
          case 3:
            digitalWrite(StepMotor_1, LOW);
            digitalWrite(StepMotor_2, HIGH);
            digitalWrite(StepMotor_3, HIGH);
            digitalWrite(StepMotor_4, LOW);
          break;
      
          case 4:
            digitalWrite(StepMotor_1, LOW);
            digitalWrite(StepMotor_2, HIGH);
            digitalWrite(StepMotor_3, LOW);
            digitalWrite(StepMotor_4, LOW);
          break;
      
          case 5:
            digitalWrite(StepMotor_1, HIGH);
            digitalWrite(StepMotor_2, HIGH);
            digitalWrite(StepMotor_3, LOW);
            digitalWrite(StepMotor_4, LOW);
            break;
      
            case 6:
            digitalWrite(StepMotor_1, HIGH);
            digitalWrite(StepMotor_2, LOW);
            digitalWrite(StepMotor_3, LOW);
            digitalWrite(StepMotor_4, LOW);
          break;
      
          case 7:
            digitalWrite(StepMotor_1, HIGH);
            digitalWrite(StepMotor_2, LOW);
            digitalWrite(StepMotor_3, LOW);
            digitalWrite(StepMotor_4, HIGH);
          break;
      
          default:
            digitalWrite(StepMotor_1, LOW);
            digitalWrite(StepMotor_2, LOW);
            digitalWrite(StepMotor_3, LOW);
            digitalWrite(StepMotor_4, LOW);
          break;
        }
        delay(speeds);
      }
    }
  }
}

void Stop()
{
  digitalWrite(StepMotor_1, LOW);
  digitalWrite(StepMotor_2, LOW);
  digitalWrite(StepMotor_3, LOW);
  digitalWrite(StepMotor_4, LOW);
}
