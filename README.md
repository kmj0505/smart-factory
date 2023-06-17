# Smart-Factory
"Smart Factory" can enable more efficient management of "Product Line Management"

## 프로젝트 기간

2023.04 ~ 2023.04

## 프로젝트 요약
- 제품 생산 라인별 성능 모니터링 및 관리를 근로자 없이도 자동화로 가동되는 시스템 개발
- 자동으로 데이터베이스를 만들어 그 기반으로 공장 가동을 최적화 상태로 업데이트

## 프로젝트 개요
1. 제품 생산라인 별 성능 모니터링 및 관리를 위한 시스템 개발
2. 제조 공정에서 자동화 및 최적화를 위한 자료 수집과 분석을 위한 시스템 개발
3. 근로자가 없이도 결함 있는 물품을 자동으로 분류 및 Database화

## 프로젝트 사용 기술
- C
- C++
- OpenCV
- TCP/IP Socket communication
- Linux
- Android
- Google Firebase
- Apache
- PHP
- MariaDB

## 시스템 구성
<p align="center"><img src="https://github.com/kmj0505/smart-farm-project/assets/123744547/196335b1-8c70-4a7b-a921-576c88f892af"></p>

- 본 시스템은 제품명에 따른 분류를 위해 컨테이너 벨트의 제품 유무를 적외선 센서로 감지하고, 제품에 부착된 QR코드를 카메라로 인식한 뒤 OpenCV 기능으로 제품을 분류하여 제품의 코드에 따라 적재하고 실시간으로 데이터베이스에 저장한다. Main 서버와 Client들이 연결되어있는 일대다 통신이고, 주로 각 기능을 담당하는 Client들끼리 상호 간의 통신이 이루어진다.
- 본 시스템을 모니터링하기 위해 Application을 제작하여 실시간으로 컨베이어 벨트의 강제 멈춤 기능과 제품의 재고 파악을 구현하였고, Web Server를 제작하여 재고 Database를 테이블화 및 그래프화하여 시각화하였다.

## Conveyor Belt Control 시스템 구성
<p align="center"><img src="https://github.com/kmj0505/smart-farm-project/assets/123744547/df189fd3-29cf-4c31-81c8-90432de27583"></p>
- Arduino Mega에 적외선 센서로 컨베이어 벨트 위의 제품을 인지하고, 릴레이 모듈을 사용하여 컨베이어 벨트의 On/Off를 제어할 수 있게 구현하였다. Jetson 보드와 연결된 카메라에서 제품명(QR코드)을 인식하면 Arduino에 정보를 전달하여 LCD 모듈에서 제품명을 출력한다. STM32 보드와 연결된 서보모터를 제품명에 따라 설정한 각도로 조절하여 자동적으로 분류할 수 있게 구현하였다.

## Android UI Interface 구성
<p align="center"><img src="https://github.com/kmj0505/smart-farm-project/assets/123744547/fc7cb5f5-a384-4f35-b195-7eb8bac9b9fa"></p>
- 컨베이어 벨트의 실시간 제어와 제품의 재고 현황을 파악하기 위해 Android Studio를 이용해 Application을 제작하였다. Main 서버에 Client로 접속할 수 있게 로그인 창을 제작하였고, 재고 최신화 버튼을 통해 실시간으로 분류되고 적재된 제품들의 수량을 파악할 수 있다. 또한, 컨베이어 벨트 제어 스위치를 이용해 실시간으로 컨베이어 벨트의 제어가 가능하다.

## Database 구축 및 시각화
### 1) Maria DB
<p align="center"><img src="https://github.com/kmj0505/smart-farm-project/assets/123744547/0333597e-8663-439f-8bf5-ea9d55d1bf62"></p>
- 분류한 제품을 제품명에 따라 수량과 목표 진행률을 업데이트하는 Database를 구축하였다. 목표 진행률은 ’분류한 제품 수량(Count)*100/목표 수량(Goal)‘ 으로 값을 설정하여 나타냈다.

### 2) PHP
<p align="center"><img src="https://github.com/kmj0505/smart-farm-project/assets/123744547/623241c8-d876-4c49-b409-1659a3826478"></p>
- Maria DB에 저장된 Database를 Web Server에서 테이블 PHP, 그래프 PHP를 사용하여 실시간 재고 현황을 시각화된 자료로 제작하였다.

### Android Firebase FCM 제작
<p align="center"><img src="https://github.com/kmj0505/smart-farm-project/assets/123744547/aae14a71-7747-4756-9c4b-78e812784991"></p>
- QR코드를 인식해서 불량품이라고 가정한 ‘Product X’를 발견하면 Arduino에서 Android로 “[FCM]DEV@FIND@X” 메시지를 보낸다.
- Android는 받은 메시지 중 “FIND”가 포함되어있으면 불량품으로 인식해 “[Title : Smart Factory, Body : 물건의 이름]”의 형식으로 FCM을 보낸다.
- Application 내에서는 토스트 알림으로 “불량품이 발견되었습니다.” 메시지를 수신받아 불량품 여부를 실시간으로 확인할 수 있다.

## 기대효과
- 자동화 시스템으로 근로자를 최소화하고 자원의 효율성을 높임
- 정해진 기준으로 기계가 제품 분류를 하므로 보다 정확한 결과를 얻을 수 있음
- 자동으로 Database 화하여 최적화된 공장 가동을 기대할 수 있음
