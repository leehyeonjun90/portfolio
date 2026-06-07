# 영상인식 컨베이어 분리수거 시스템 (졸업작품)

## 프로젝트 개요

YOLO 기반 영상인식 기술을 활용한 자동 분리수거 시스템입니다. 컨베이어 벨트 위의 쓰레기를 실시간으로 인식하여 자동으로 분류하는 완전 자동화 시스템입니다.

## 기술 스택

- **Vision**: YOLO v8 (객체탐지)
- **Backend**: Python 3.x
- **Embedded**: Arduino (제어)
- **Hardware**: Raspberry Pi, Motors, Sensors
- **CAD**: SolidWorks

## 프로젝트 구조

```
graduation/
├─ src/
│  ├─ code/
│  │  ├─ separate trash_v2.py      (YOLO 모델 v2)
│  │  ├─ separate trash_v3.py      (YOLO 모델 v3)
│  │  ├─ AR_yolo_1/
│  │  │  └─ AR_yolo_1.ino          (기본 제어)
│  │  ├─ AR_yolo_2/
│  │  │  └─ AR_yolo_2.ino          (센서 통합)
│  │  └─ AR_yolo_3/
│  │     └─ AR_yolo_3.ino          (LCD 디스플레이)
│  ├─ circuit/                      (회로도)
│  ├─ datasheet/                    (부품 데이터시트)
│  └─ design/                       (CAD 도면 & STL)
│
└─ images/
   └─ work_process/                 (개발 과정 사진 55개)
```

## 주요 기능

### 1. 영상 인식
- YOLO 객체탐지 모델로 실시간 쓰레기 분류
- 플라스틱, 종이, 금속 등 5가지 분류

### 2. 자동 제어
- Arduino 기반 모터 제어
- 센서 연동으로 정확한 분류
- LCD 디스플레이로 상태 표시

### 3. 기계 설계
- SolidWorks로 설계된 완전한 3D 모델
- 3D 프린팅 부품 및 금속 부품 조립
- 컨베이어 벨트 시스템

## 개발 코드

### Python YOLO 모델
```
separate trash_v2.py  - 기본 YOLO 영상인식
separate trash_v3.py  - 최적화 버전
Separate_Trash_best.pt - 학습된 모델 가중치
```

### Arduino 임베디드
```
AR_yolo_1.ino - 모터 기본 제어
AR_yolo_2.ino - 센서 통합
AR_yolo_3.ino - LCD 디스플레이 표시
```

## 개발 과정

`images/work_process/` 폴더에 55개의 개발 과정 사진이 포함되어 있습니다:
- 회로 조립 과정
- CAD 설계 및 3D 모델링
- 프로토타입 테스트
- 최종 조립 및 검증

## 설계 도면

`src/design/` 폴더의 SolidWorks 파일들:
- 메인 프레임 (SLDPRT)
- 모터 마운트
- 센서 홀더
- 컨베이어 벨트 시스템
- STL 파일 (3D 프린팅용)
- DWG 파일 (기계 도면)

## 성과

✅ 대학 졸업작품 우수상 수상
✅ 영상인식 정확도 94%
✅ 완전 자동화 시스템 완성
✅ IoT 기반 스마트 분리수거 시스템 구현

## 참고 자료

`src/datasheet/` 폴더: 사용된 부품의 데이터시트
- Raspberry Pi 명세
- DC 모터 특성
- 센서 사양
- 기타 전자 부품

---

**작업 기간**: 2024년 8월 ~ 2025년 10월
**개발 인원**: 개인 프로젝트
