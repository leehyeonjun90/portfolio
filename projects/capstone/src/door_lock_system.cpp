#include <wiringPi.h>
#include <softPwm.h> // 소프트 PWM 사용
#include <stdio.h>
#include <string.h>

#define BUZZER 8        // GPIO 2 (WiringPi 핀 번호)
#define SERVO 1         // GPIO 18 (WiringPi 핀 번호)
#define MAGNET_SENSOR 9 // GPIO 3 (WiringPi 핀 번호)

// RGB LED 핀 정의
#define RED_PIN 4       // GPIO 23 (R 핀)
#define GREEN_PIN 5     // GPIO 24 (G 핀)
#define YELLOW_PIN 6    // GPIO 25 (Y 핀)

// 키패드 핀 정의
int rowPins[4] = {0, 2, 3, 21};   // GPIO 17, 27, 22, 5
int colPins[4] = {22, 23, 24, 25}; // GPIO 6, 13, 19, 26

// 키패드 매핑
char keyMap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

char password[5] = "0000";  // 초기 비밀번호 설정
char input[5] = "";         // 사용자 입력 비밀번호
int inputIdx = 0;

int miFrequency = 659; // '미' 음계 주파수
int solFrequency = 784; // '솔' 음계 주파수

// 부저 음 재생
void playTone(int frequency, int duration) {
    int period = 1000000 / frequency; // 주기 (마이크로초)
    int pulse = period / 2;          // 듀티 사이클 50%
    for (int i = 0; i < (duration * 1000) / period; i++) {
        digitalWrite(BUZZER, HIGH);
        delayMicroseconds(pulse);
        digitalWrite(BUZZER, LOW);
        delayMicroseconds(pulse);
    }
}

// '미솔' 소리 재생
void playMiSol() {
    playTone(miFrequency, 300); // '미'
    delay(50);
    playTone(solFrequency, 300); // '솔'
}

// 서보모터 각도 설정 (0도~180도)
void setServoAngle(int angle) {
    int pwmValue = 5 + (angle * 10 / 180); // 0도 = 5, 90도 = 15, 180도 = 25
    softPwmWrite(SERVO, pwmValue);
    delay(500); // 각도 이동 대기
}

// 키패드 초기화
void keypadSetup() {
    for (int i = 0; i < 4; i++) {
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], HIGH);
    }
    for (int i = 0; i < 4; i++) {
        pinMode(colPins[i], INPUT);
        pullUpDnControl(colPins[i], PUD_UP);
    }
}

// 키패드 입력 읽기
char readKeypad() {
    for (int row = 0; row < 4; row++) {
        digitalWrite(rowPins[row], LOW);
        delay(5);
        for (int col = 0; col < 4; col++) {
            if (digitalRead(colPins[col]) == LOW) {
                digitalWrite(rowPins[row], HIGH);
                delay(200); // 디바운싱
                return keyMap[row][col];
            }
        }
        digitalWrite(rowPins[row], HIGH);
    }
    return '\0'; // 입력 없음
}

// RGB LED 색상 제어
void setLEDColor(int red, int green, int yellow) {
    digitalWrite(RED_PIN, red);
    digitalWrite(GREEN_PIN, green);
    digitalWrite(YELLOW_PIN, yellow);
}

// 자석 감지 및 서보모터 복귀
void waitForMagnetAndResetServo() {
    printf("자석 감지 대기 중...\n");
    while (digitalRead(MAGNET_SENSOR) == HIGH) {
        delay(100); // 자석 감지 확인 (100ms 간격)
    }
    printf("문이 닫혔습니다. 2초 후 서보모터를 복귀합니다.\n");
    delay(2000); // 문이 닫힌 후 2초 대기
    setServoAngle(0); // 서보모터 복귀
    printf("서보모터가 원래 위치로 복귀했습니다.\n");
    setLEDColor(0, 0, 0); // LED 끄기
}

int main() {
    // WiringPi 초기화
    if (wiringPiSetup() == -1) {
        printf("WiringPi 초기화 실패!\n");
        return 1;
    }

    // 핀 초기화
    pinMode(BUZZER, OUTPUT);
    pinMode(MAGNET_SENSOR, INPUT); // 자석 감지 센서 입력 핀
    pullUpDnControl(MAGNET_SENSOR, PUD_UP); // 풀업 설정

    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);

    // 소프트 PWM 초기화
    if (softPwmCreate(SERVO, 0, 200) != 0) {
        printf("소프트 PWM 초기화 실패!\n");
        return 1;
    }

    keypadSetup();

    printf("시작합니다. 초기 비밀번호는 0000입니다.\n");

    while (1) {
        char key = readKeypad();
        if (key != '\0') {
            printf("입력: %c\n", key);

            // 모든 숫자 키, *, # -> '미' 음계 출력
            if ((key >= '0' && key <= '9') || key == '*' || key == '#') {
                playTone(miFrequency, 300); // '미' 소리 재생
            }

            // A 버튼 -> 비밀번호 변경 모드
            if (key == 'A') {
                playMiSol(); // '미솔' 소리 재생
                printf("비밀번호 변경 모드 시작\n");
                inputIdx = 0;
                memset(input, 0, sizeof(input));
                printf("새로운 비밀번호를 입력하세요. B를 눌러 종료합니다.\n");

                while (1) {
                    key = readKeypad();
                    if (key >= '0' && key <= '9') {
                        if (inputIdx < sizeof(password) - 1) {
                            password[inputIdx++] = key;
                            playTone(miFrequency, 300); // '미' 소리 재생
                            printf("입력: %c\n", key);
                        }
                    } else if (key == 'B') { // 비밀번호 변경 종료
                        password[inputIdx] = '\0';
                        printf("새 비밀번호: %s\n", password);
                        playMiSol(); // '미솔' 소리 재생
                        break;
                    }
                }
            }

            // 비밀번호 입력 모드
            if (key == '#') {
                printf("비밀번호 입력 모드 시작\n");
                inputIdx = 0;
                memset(input, 0, sizeof(input));

                while (1) {
                    key = readKeypad();
                    if (key >= '0' && key <= '9') {
                        if (inputIdx < sizeof(input) - 1) {
                            input[inputIdx++] = key;
                            playTone(miFrequency, 300); // '미' 소리 재생
                            printf("입력: %c\n", key);
                        }
                    } else if (key == '*') { // 입력 종료
                        input[inputIdx] = '\0';
                        break;
                    }
                }

                // 비밀번호 확인
                if (strcmp(password, input) == 0) {
                    printf("비밀번호 일치\n");
                    playMiSol(); // '미솔' 소리
                    setServoAngle(180);     // 서보모터 180도
                    setLEDColor(0, 1, 0);   // 초록색 (GREEN)
                    waitForMagnetAndResetServo(); // 자석 감지 후 복귀
                } else {
                    printf("비밀번호 불일치\n");
                    playTone(500, 1000);    // 삐빅 소리
                    setLEDColor(1, 0, 0);   // 빨간색 (RED)
                    delay(1000);            // 1초 대기
                    setLEDColor(0, 0, 0);   // LED 끄기
                }
            }
        }
    }
    return 0;
}