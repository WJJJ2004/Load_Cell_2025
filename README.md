# LoadCell_2025

> **휴머노이드 로봇 발바닥 로드셀 센서**의 실시간 시각화 및 보정 및 com 기반 zmp 계산을 제공하는 ROS 2 기반 GUI 패키지  
> Ubuntu 22.04 + ROS 2 Humble 환경에서 작동합니다.

---

## 📁 목차

1. [개요](#개요)  
2. [설치 및 실행 방법](#설치-및-실행-방법)  
3. [기능 및 구성 요소](#기능-및-구성-요소)  
4. [보정 및 파라미터 관리](#보정-및-파라미터-관리)  
5. [개발 환경](#개발-환경)  
6. [의존 패키지](#의존-패키지)

---

## 개요

이 패키지는 로봇 발바닥에 부착된 **8개의 로드셀 센서 데이터를 serial port로 수신**하고,  
실시간 시각화, 디지털 필터링, 보정 (zero/unit gain), zmp 계산, 그리고 ROS 2 메시지 퍼블리시 기능을 수행합니다.

- **Qt 기반 GUI**로 센서 선택, 필터 적용, 보정값 저장/로드 가능  
- **QCustomPlot**으로 실시간 그래프 출력  
- **FilterManager**로 평균, 중앙값, LPF 적용  
- 향후 로봇 ZMP 계산 및 자세 안정화에 활용 가능

---

## 설치 및 실행 방법

### 1. 워크스페이스 생성
```bash
mkdir -p ~/colcon_ws/src
cd ~/colcon_ws/src
```
2. 리포지토리 클론

```bash
git clone https://github.com/WJJJ2004/load_cell_2025.git

```
3. 빌드

```bash
cd ~/colcon_ws
colcon build --packages-select load_cell_2025

```
5. 실행

```bash
cd ~/colcon_ws
source install/setup.bash
ros2 run load_cell_2025 load_cell_2025

```
## 기능 및 구성 요소
구성 요소	설명
MainWindow	Qt 기반 UI / 버튼 핸들링 및 시각화
QNode	ROS2 인터페이스 관리 (humanoid_interfaces/msg/lc_msgs)
SerialReceiver	로드셀 MCU로부터 시리얼 데이터 수신
FilterManager	평균/중앙값/LPF 필터 클래스
CalibrationManager	보정값(YAML) 저장 및 불러오기 기능
QCustomPlot	실시간 그래프 출력용 외부 라이브러리 (third_party/)

## 보정 및 파라미터 관리
보정 값
Zero Gain (add2zero): 정지 상태에서 로드셀의 오프셋을 제거

Unit Gain (add2unit): 센서 출력을 무게 단위로 환산하는 계수

관련 파일

버튼 기능
Insert: 현재 로드셀 데이터를 zero gain으로 저장

Save: 현재 보정값을 TXT 파일로 저장

Load: 보정값을 불러와 적용

## 개발 환경
OS: Ubuntu 22.04

ROS 2: Humble

언어: C++17, Qt5

## 의존 패키지
rclcpp

std_msgs

humanoid_interfaces

Qt5 Widgets, Qt5 SerialPort

yaml-cpp

QCustomPlot (third_party/qcustomplot)
