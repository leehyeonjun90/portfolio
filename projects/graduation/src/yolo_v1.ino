/*
=============================================================================
【프로젝트】다중 모터 제어 시스템 - Serial 통신 기반 분산 제어
【버전】AR_yolo_1.ino (초기 버전)
【작성자】이현준
【작성일】2025년 초기

【프로젝트 개요】
라즈베리파이의 AI 인식 결과를 받아서 아두이노가 3가지 모터를 제어하는
분산 제어 시스템입니다.

【시스템 구성】
1. 상위 제어부: Raspberry Pi 4B (AI 영상인식)
   - YOLOv8으로 물품 분류 (캔, 병, 종이, 유리, 기타)

2. 중위 제어부: Arduino UNO R3 (이 파일)
   - Serial 통신으로 라즈베리파이로부터 신호 수신
   - 3가지 모터 제어 로직 실행

3. 하위 제어부: 모터 & 드라이버
   - DC모터 (L298N 드라이버): 컨베이어 구동
   - 스테핑모터 (TB6600 드라이버): 슬라이드 위치 이동
   - 서보모터: 분류함 개폐 (추가 예정)

【신호 정의】
라즈베리파이에서 수신하는 한 문자 신호:
- 'b': 플라스틱병 감지
- 'c': 캔 감지
- 'p': 종이 감지
- 'g': 유리 감지
- 'o': 기타/인식 불가능
- 'n': 초기화 신호

【개발 진행 사항】
v1: 기본 모터 제어 + Serial 통신 (이 파일)
v2: 버튼 추가, 모드 상태 관리 추가
v3: LCD 표시 추가, 카운팅 기능 추가 (최종 버전)

=============================================================================
*/

#include <AccelStepper.h>
#include <SoftwareSerial.h>

// ===== Serial 통신 설정 =====
SoftwareSerial btSerial(2, 3); // RX, TX (블루투스용 - 현재 사용 안 함)

// ===== 센서 핀 정의 =====
const int HALL = 4;        // 홀센서: 슬라이드 위치 감지 (초기 위치 감지용)
const int IR_pin = 11;     // 적외선 근접센서: 컨베이어 끝단에 물품 도착 감지

// ===== 스테핑모터 제어 핀 (TB6600 드라이버) =====
const int EN_pin  = 5;     // Enable: 모터 활성화
const int dir_pin  = 6;    // Direction: 회전 방향 제어
const int step_pin = 7;    // Step: 스텝 신호 (펄스)

// ===== DC모터 제어 핀 (L298N 드라이버) =====
const int Motor_A_pin  = 8;     // IN1: 방향 제어1
const int Motor_B_pin  = 9;     // IN2: 방향 제어2
const int Motor_PWM_pin = 10;   // ENA: PWM으로 속도 제어 (0~255)
int Motor_Speed = 200;          // 초기 모터 속도 (0~255)




// ===== 스테핑모터 각도 제어 값 =====
// TB6600 설정: 1/16 마이크로 스텝 → 3200펄스 = 90도 회전
// 계산: 360도 / 4개 분류함 = 90도씩 회전
// compartment[0] = 초기위치 (0도)
// compartment[1] = 플라스틱병 위치 (90도 = 2560펄스)
// compartment[2] = 캔 위치 (180도 = 5120펄스)
// compartment[3] = 종이 위치 (-90도 = -2560펄스)
// compartment[4] = 유리 위치 (-180도 = -5120펄스)
// compartment[5] = 기타/초기화 (0도)
int compartment[] = {0, 2560, 5120, -2560, -5120, 0};

int start_bt=0, mode_s=0;
int ir=0;

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
  

  if (Serial.available()) {

    pidata=Serial.read();

    if(pidata=='b'){
      selected=0;
    }
    if(pidata=='c'){
      selected=1;
    }
    if(pidata=='p'){
      selected=2;
    }
    if(pidata=='g'){
      selected=3;
    }
    if(pidata=='o'){
      selected=4;
    }    
    if(pidata=='n'){
      mode_s=0;
      //selected=5;
    }

    if(pidata=='1'){
      Motor_Stop();
      //selected=5;
    }
    if(pidata=='2'){
      forward();
      //selected=5;
    }
    if(pidata=='3'){
      Motor_Speed=Motor_Speed+5;
    }
    if(pidata=='4'){
      Motor_Speed=Motor_Speed-5;
    }
    if(pidata=='5'){
      mode_s=3;
    }

    Serial.print( "Motor_Speed:" );
    Serial.println( Motor_Speed );

    pidata='a';
  } 



  if(mode_s==3){
    while(digitalRead(IR_pin)==HIGH){
        forward();
        ir=digitalRead(IR_pin);
        Serial.print( "ir:" );
        Serial.println( ir );
    }
    Motor_Stop();
    mode_s=0;
  }

  if(mode_s==0){
    selected=5;
    stepper.moveTo(compartment[selected]);
    stepper.runToPosition();
    mode_s=1;
  }
  

  if((mode_s==1)&&(selected==5)){

  ///////////////////////////////////////////////
/*
    if(Front_Sensor <= 38){
      delay(2000);
      Serial.print("C");
      mode_s=2;
    }
*/
///////////////////////////////////////////////
    delay(50); 
/*
    btSerial.print("S");
    if(Front_Sensor>=10)btSerial.print(Front_Sensor);
    else{
      btSerial.print("0");
      btSerial.print(Front_Sensor);
    }    
    if(Bottle_Sensor>=10)btSerial.print(Bottle_Sensor);
    else{
      btSerial.print("0");
      btSerial.print(Bottle_Sensor);
    }
    if(Glass_Sensor>=10)btSerial.print(Glass_Sensor);
    else{
      btSerial.print("0");
      btSerial.print(Glass_Sensor);
    }
    if(Paper_Sensor>=10)btSerial.print(Paper_Sensor);
    else{
      btSerial.print("0");
      btSerial.print(Paper_Sensor);
    }
    if(Can_Sensor>=10)btSerial.print(Can_Sensor);
    else{
      btSerial.print("0");
      btSerial.print(Can_Sensor);
    }    
  */
    stepper.moveTo(compartment[selected]);
    stepper.runToPosition();       

  }  


 // if Bottle selected
  if (selected == 0){
      stepper.moveTo(compartment[selected]);
      stepper.runToPosition();
      selected=5;
      mode_s=1;
      delay(3000);
    }
    // if GLASS selected
  else if (selected == 1){
      stepper.moveTo(compartment[selected]);
      stepper.runToPosition();
      selected=5;
      mode_s=1;
      delay(3000);
  }
    // if PAPER selecte
  else if (selected == 2){
      stepper.moveTo(compartment[selected]);
      stepper.runToPosition();
      selected=5;
      mode_s=1;
      delay(3000);
  } 
 // if CAN selected
  else if (selected == 3){
      stepper.moveTo(compartment[selected]);
      stepper.runToPosition();
      selected=5;
      mode_s=1;
      delay(3000);
  }
  else if (selected == 4){
      stepper.moveTo(compartment[selected]);
      stepper.runToPosition();
      selected=5;
      mode_s=1;
      delay(3000);
  }  
   
  else {
      //no statement
  }
 
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
