#include <AccelStepper.h>  
#include <SoftwareSerial.h>


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

//AccelStepper stepper(AccelStepper::DRIVER, step_pin, dir_pin);   
AccelStepper stepper(1, step_pin, dir_pin);   // (Typeof driver: with 2 pins, STEP, DIR)

void setup()   
{   
  Serial.begin(9600);  
  //piSerial.begin(9600);
  btSerial.begin(9600);

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
    }
    if(bt=='c'){
      selected=1;
    }
    if(bt=='p'){
      selected=2;
    }
    if(bt=='g'){
      selected=3;
    }
    if(bt=='o'){
      selected=4;
    }    
    if(bt=='n'){
      mode_s=0;
      //selected=5;
    }
    bt='a';
  } 

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

  Serial.print( "mode_s:" );
  Serial.print( mode_s );
  Serial.print( "     button_SW:" );
  Serial.print( button_SW );
  ir=digitalRead(IR_pin);
  Serial.print( "   ir:" );
  Serial.print( ir );

  if(button_SW==HIGH){
    if(sw_state==0){
      mode_s=2;
      sw_state=1;
      delay(1000);
    }else{
      mode_s=0;
      sw_state=0;
      delay(1000);
    } 
  } 
  Serial.print( "  sw_state:" );
  Serial.print( sw_state );

  if(mode_s==2){
    while(digitalRead(IR_pin)==HIGH){
        forward();
        ir=digitalRead(IR_pin);
        Serial.print( "ir:" );
        Serial.println( ir );
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
        ir=digitalRead(IR_pin);
        Serial.print( "ir:" );
        Serial.println( ir );
    }

    if(mode_s==0){
      delay(100);
    }else{
      forward();
      delay(1000);
      Motor_Stop();
      delay(1000);
      stepper.moveTo(compartment[0]);
      stepper.runToPosition();    
      mode_s=2; 
    }    
   
  }

  Serial.print( "  mode_s:" );
  Serial.println( mode_s );

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
