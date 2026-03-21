260321 22:15 edited

# <SYSTEM SCENARIO>

## 1. Input

* joystick으로 조종을 한다. input되는 입력은 아래와 같다.

  * A 버튼 : auto, manual toggle
  * P : 시스템 init 버튼


## 2. Main Loop

* 현재 main의 while 안에는 ST_MACHINE만 있는 상태이다.


## 3. BMS 동작

* BMS 는 정지든, 주행이든 항시 작동해야한다.
* 또한 주행중일 경우, 즉 actual_speed >0 일 경우, bms_safty_manager에 따라 아래와 같은 감속 및 복구 알고리즘을 거친다.
* 이 감속 및 복구 알고리즘은, auto일때나 manual일때나 둘 다 current_speed에 영향을 준다.

### 3-1. 입력 센서

* temp
* gas
* current
* voltage


### 3-2. Warning Count 계산

* warning을 count한다.

  * temp → +1
  * gas → +1
  * current 또는 voltage 중 하나라도 → +1
* 즉 warning count 는 max가 3


### 3-3. 감속 로직

* waring count 가 1이면 → -1
* waring count 가 2이면 → -3
* waring count 가 3이면 → -5


### 3-4. 최소 속도 제한

* 속도는 최소 30까지 제한한다.


### 3-5. 복구 로직

* 만약 3 상태 모두 다 3초동안 safe를 유지하면
  → 다시 천천히 원래 current로 복구 (80)


### 3-6. Danger 로직

* 3중 하나라도 danger를 넘어가게 된다면 → -10의 속도로 0까지 낮춘다.
* 이때는 복구 알고리즘 없음


### 3-7. Mode 전환 시 속도 유지

* 만약 auto 상태에서 감속이 된 상태라면
  (예: 80 -> 70)
  → manual mode로 가도 70인 상태에서 주행을 해야한다.


## 4. Trace 동작

### 4-1. Auto mode

* Auto에서는 D input이 들어가도 무조건 init 상태


### 4-2. 기본 동작 조건

* Trace는 정지중일때만 동작한다.


### 4-3. Manual mode

* Manual 에서는 input 시 동작
* D 입력 시 toggle

  * init ↔ act 상태 전환


### 4-4. 강제 init 조건

* 정차중일때도 아래 조건이면 무조건 init으로 전환

  * warning 3단계
  * danger 상태


## 5. 시스템 초기화

* manual button(P)를 누르면 전체 초기화
