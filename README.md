260321 22:15 edited

<SYSTEM SCENARIO>
1. joystick으로 조종을 한다. input되는 입력은 아래와 같다.   A 버튼 : auto, manual toggle, P : 시스템 init 버튼
2. 현재 main의 while 안에는 ST_MACHINE만 있는 상태이다. 
3. BMS 는 정지든, 주행이든 항시 작동해야한다. 또한 주행중일 경우, 즉 current speed >0 일 경우, bms_safty_manager에 따라, 아래와 같은 감속 및 복구 알고리즘을 거친다. 이 감속 및 복구 알고리즘은, auto일때나 manual일때나 둘 다 current_speed에 영향을 준다. 
    1.  temp, gas, current, voltage 이렇게 4개를 받을겨. 
    2. warning을 count. temp +, gas +, 이 때 current나 voltage는 둘 중 하나만 켜져도 +. 즉 warning count 는 max가 3. 
    3.  waring count 가 1이면 -1, 2이면 -3, 3이면 -5로 감속.
    4. 이 떄 30까지 제한속도. 
    5. 만약 3 상태 모두 다 3초동안 safe를 유지하면, 다시 천천히 원래 current로 복구. (80) 
    6. 그리고 만약, 3중 하나라도 danger를 넘어가게된다면, -10의 속도로 0까지 낮춤. 이때는 복구 알고리즘 없음. 
    7. 만약 auto 상태에서 감속이 된 상태라면, (예를 들어 80 -> 70)manual mode로 가도 70인 상태에서 주행을 해야한다. 
4. Trace 동작
    1. Auto에서는 D input이 들어가도 무조건 init상태. 
    2. 즉, 정지중일때만 동작한다. 
    3. Manual 에서는 input 시 동작 (toggle이 되겠지 .다시 d input을 누르면, init 또는 동작이 왓다갓다 .)
    4. But 정차중일때도, warning 중 3단계가 되거나, danger 상태가 된다면, 무조건 init으로 감. 
5. 위 감속 및 정차 시나리오는, manual button(P) 를 누르면 초기화된다. 
