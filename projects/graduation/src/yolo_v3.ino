#include <AccelStepper.h>  
#include <SoftwareSerial.h>
#include <Wire.h>                        //LCD를 쓰기위한 Wire 라이브러리를 불러옵니다.
#include <LiquidCrystal_I2C.h>           //I2C LCD 라이브러리를 불러옵니다.

SoftwareSerial btSerial(2, 3); // RX, TX

const int HALL = 4;
const int IR_pin = 11;
const int Button_pin = 12;

const int EN_pin  = 5; 
const int dir_pin  = 6;  
const int step_pin = 7; 

const int Motor_A_pin  = 8; 
const int Motor_B_pin  = 9;  
const int Motor_PWM_pin = 10; 
int Motor_Speed = 200;

//OFF OFF ON ,16STEP,3200PULS //ON OFF ON 1A //기어비 25/100=>360/4=90도(3200펄스동작시90도 회전), 360/5=75도 *0번(0)*1번(2560) *2번(5120) *3번(7680) *4번(11200)
int compartment[] = {0, 2560, 5120, -2560, -5120, 0}; //800///4800,6400based on our EI model (Bottle,Can,Paper,GLASS,other,Noise)

int start_bt=0, mode_s=0;
int ir=0;
int button_SW=0;
int sw_state=0;

char bt;
char pidata;
int selected = 0; //default

int Bottle=0, Can=0, Paper=0, Glass=0, Other=0;
int Bottle_Length=1, Can_Length=1, Paper_Length=1, Glass_Length=1, Other_Length=1;   //LCD잔여 택스트를 없애기위한 문자길이 저장변수

LiquidCrystal_I2C lcd(0x27, 16, 2);       // I2C LCD 객체인 lcd를 설정합니다. (I2C주소,가로크기,세로크기)입니다.
//AccelStepper stepper(AccelStepper::DRIVER, step_pin, dir_pin);   
AccelStepper stepper(1, step_pin, dir_pin);   // (Typeof driver: with 2 pins, STEP, DIR)

void setup()   
{   
  Serial.begin(9600);  
  //piSerial.begin(9600);
  btSerial.begin(9600);

  lcd.init();                       // lcd 객체 초기화
  lcd.backlight();                  // 백라이트를 켜줍니다.
  lcd.clear();                      // 화면 지우고 커서를 왼쪽 상단 모서리로 옮김   

  pinMode(HALL, INPUT);
  pinMode(IR_pin, INPUT);
  pinMode(Button_pin, INPUT);
  
  
  pinMode(Motor_A_pin, OUTPUT);  
  pinMode(Motor_B_pin, OUTPUT);

  pinMode(EN_pin,OUTPUT);
  digitalWrite(EN_pin, LOW); // 모터 on 시키기

  // stepper motor calibration
  stepper.setMaxSpeed(3000); // Set maximum speed value for the stepper
  stepper.setAcceleration(2000); // Set acceleration value for the stepper  

  while(digitalRead(HALL)){
      stepper.setSpeed(-400); // Set the current position to 0 steps
      stepper.runSpeed();
  }
  delay(100);
  stepper.setCurrentPosition(0);
  //end of stepper motor calibration  


  digitalWrite(Motor_A_pin, LOW);
  digitalWrite(Motor_B_pin, LOW);
  analogWrite(Motor_PWM_pin, 0); 

  delay(500); 
}   
  
void loop()   
{   

  if (btSerial.available()) {
    bt=btSerial.read();
    Serial.print( "BT_data:" );
    Serial.println(bt);
    if(bt=='b'){
      selected=0;
      Bottle++;
    }
    if(bt=='c'){
      selected=1;
      Can++;
    }
    if(bt=='p'){
      selected=2;
      Paper++;
    }
    if(bt=='g'){
      selected=3;
      Glass++;
    }
    if(bt=='o'){
      selected=4;
      Other++;
    }    
    if(bt=='n'){
      mode_s=0;
      //selected=5;
    }
    bt='a';
  } 

  char Load_buf[2];
  dtostrf(Bottle, 2, 0, Load_buf);
  Bottle_Length=String(Bottle).length(); 

  lcd.setCursor(0, 0);lcd.print("B="); 
  lcd.setCursor(2, 0);lcd.print(Bottle); 
  for (int i=0;i<2-Bottle_Length;i++)
  {
    lcd.print(" ");
  }

  char Can_buf[2];
  dtostrf(Can, 2, 0, Can_buf);
  Can_Length=String(Can).length(); 

  lcd.setCursor(5, 0);lcd.print("C="); 
  lcd.setCursor(7, 0);lcd.print(Can); 
  for (int i=0;i<2-Can_Length;i++)
  {
    lcd.print(" ");
  }

  char Paper_buf[2];
  dtostrf(Paper, 2, 0, Paper_buf);
  Paper_Length=String(Paper).length(); 

  lcd.setCursor(10, 0);lcd.print("P="); 
  lcd.setCursor(12, 0);lcd.print(Paper); 
  for (int i=0;i<2-Paper_Length;i++)
  {
    lcd.print(" ");
  }    

  char Glass_buf[2];
  dtostrf(Glass, 2, 0, Glass_buf);
  Glass_Length=String(Glass).length(); 

  lcd.setCursor(0, 1);lcd.print("G="); 
  lcd.setCursor(2, 1);lcd.print(Glass); 
  for (int i=0;i<2-Glass_Length;i++)
  {
    lcd.print(" ");
  }   

  char Other_buf[2];
  dtostrf(Other, 2, 0, Other_buf);
  Other_Length=String(Other).length(); 

  lcd.setCursor(5, 1);lcd.print("O="); 
  lcd.setCursor(7, 1);lcd.print(Other); 
  for (int i=0;i<2-Other_Length;i++)
  {
    lcd.print(" ");
  }     

  lcd.setCursor(10, 1);lcd.print("mode="); 
  lcd.setCursor(15, 1);lcd.print(mode_s);   

 if (Serial.available()) {
    pidata=Serial.read();
    if(pidata=='b')selected=0;
    if(pidata=='c')selected=1;
    if(pidata=='p')selected=2;
    if(pidata=='g')selected=3;
    if(pidata=='o')selected=4;
    if(pidata=='n')mode_s=0;
    if(pidata=='1')Motor_Stop();
    if(pidata=='2')forward();
    if(pidata=='3')Motor_Speed=Motor_Speed+5;
    if(pidata=='4')Motor_Speed=Motor_Speed-5;
    if(pidata=='5')mode_s=3;
    Serial.print( "Motor_Speed:" );
    Serial.println( Motor_Speed );
    pidata='a';
  }   

  if(mode_s==0){
    selected=5;
    stepper.moveTo(compartment[selected]);
    stepper.runToPosition();
    Motor_Stop();
    mode_s=1;
  }

  button_SW=digitalRead(Button_pin);
/*
  Serial.print( "mode_s:" );
  Serial.print( mode_s );
  Serial.print( "     button_SW:" );
  Serial.print( button_SW );
  ir=digitalRead(IR_pin);
  Serial.print( "   ir:" );
  Serial.print( ir );
*/
  if(button_SW==HIGH){
    if(sw_state==0){
      mode_s=2;
      sw_state=1;
      Bottle=0;
      Can=0;
      Paper=0;
      Glass=0;
      Other=0;      
      delay(1000);
    }else{
      mode_s=0;
      sw_state=0;
      delay(1000);
    } 
  } 
  //Serial.print( "  sw_state:" );
  //Serial.print( sw_state );

  if(mode_s==2){
    while(digitalRead(IR_pin)==HIGH){
        forward();
        //ir=digitalRead(IR_pin);
        //Serial.print( "ir:" );
        //Serial.println( ir );
        button_SW=digitalRead(Button_pin);
        if(button_SW==HIGH){
          mode_s=0;
          sw_state=0;
          delay(1000);
          break;
        }
    }
    Motor_Stop();
    if(mode_s==0){
      delay(100);
    }else{
      mode_s=4;
      delay(5000);
      btSerial.print("C");
    }

  }

  if(mode_s==3){
    while(digitalRead(IR_pin)==LOW){
        button_SW=digitalRead(Button_pin);
        if(button_SW==HIGH){
          mode_s=0;
          sw_state=0;
          delay(1000);
          break;
        }
        forward();
       // ir=digitalRead(IR_pin);
       // Serial.print( "ir:" );
       // Serial.println( ir );
    }

    if(mode_s==0){
      delay(100);
    }else{
      //forward();
      //delay(1000);
      Motor_Stop();
      delay(2000);
      stepper.moveTo(compartment[0]);
      stepper.runToPosition();    
      delay(1000);
      mode_s=2; 
    }    
   
  }

  if(mode_s==4){
    // if Bottle selected
      if (selected == 0){
          stepper.moveTo(compartment[selected]);
          stepper.runToPosition();
          mode_s=3;
          selected=5;
          delay(500);
        }
        // if GLASS selected
      else if (selected == 1){
          stepper.moveTo(compartment[selected]);
          stepper.runToPosition();
          mode_s=3;
          selected=5;
          delay(500);
      }
        // if PAPER selecte
      else if (selected == 2){
          stepper.moveTo(compartment[selected]);
          stepper.runToPosition();
          mode_s=3;
          selected=5;
          delay(500);
      } 
    // if CAN selected
      else if (selected == 3){
          stepper.moveTo(compartment[selected]);
          stepper.runToPosition();
          mode_s=3;
          selected=5;
          delay(500);
      }
      else if (selected == 4){
          stepper.moveTo(compartment[selected]);
          stepper.runToPosition();
          mode_s=3;  
          selected=5;      
          delay(500);
      }  
      
      else {
          //no statement
    }
  }//mode4 end

  //main end
}  


void forward()
{
  digitalWrite(Motor_A_pin, HIGH);
  digitalWrite(Motor_B_pin, LOW);
  analogWrite(Motor_PWM_pin, Motor_Speed); 
}
void Motor_Stop()
{
  digitalWrite(Motor_A_pin, LOW);
  digitalWrite(Motor_B_pin, LOW);
  analogWrite(Motor_PWM_pin, 0); 
}
