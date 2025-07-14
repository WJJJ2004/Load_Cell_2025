# LoadCell_Node 2025

## 개요
`LoadCell_Node` 프로젝트는 로봇의 **발바닥에 부착된 로드셀 센서**로부터 데이터를 수신하고 이를 활용하여 **ZMP (Zero Moment Point)**를 계산, **로봇의 자세 안정화에 활용**하는 ROS2 + Qt 기반 GUI 응용 프로그램입니다. 이 노드는 다음 기능을 제공합니다:

- serial port로 센서 데이터 수신
- ZMP 좌표 계산 (한발/두발 지지 상황별)
- 실시간 필터링 및 그래프 출력
- 센서 보정 기능 (zero/unit 보정)
- 센서 이상값에 대한 리미트 설정

## 개발 환경
- **OS**: Ubuntu 22.04
- **ROS2**: Humble
- **Qt**: 5.15 이상
- **C++11**, CMake
- **QCustomPlot**: third_party 포함

## 실행 방법

```bash
cd ~/colcon_ws
colcon build --packages-select load_cell_2025
source install/setup.bash
ros2 run load_cell_2025 load_cell_2025_node
```
