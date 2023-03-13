#if 1

void tank_turn(uint8 angle, int Direction, uint32 delay) {
  int negativeSpeed = 0 - angle;
  if (Direction == 1) {
    SetMotors(0, 1, negativeSpeed, angle, delay);
  } else {
    SetMotors(1, 0, negativeSpeed, angle, delay);
  }
}

void zmain(void) {
  TickType_t Ttime = xTaskGetTickCount(); // take start time
  TickType_t timeFromLastLine = 0;
  TickType_t previousTime = 0;
  IR_Start();
  motor_start();
  motor_forward(0, 0);
  Ultra_Start();
  struct sensors_ dig;
  reflectance_start();
  reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
  int d = Ultra_GetDistance();
  while (SW1_Read() == 1) {
    vTaskDelay(100);
  }
  reflectance_digital( & dig);
  //Aja viivalle
  while (dig.L3 == 0 && dig.R3 == 0) {
    reflectance_digital( & dig);
    motor_forward(40, 20);
  }
  //Pysäytä moottori
  motor_forward(0, 0);
  send_mqtt("Zumo001/Ready", "Zumo");
  //Lähetä aloitusviesti
  IR_wait();
  //Odota IR
  Ttime = xTaskGetTickCount();
  print_mqtt("Zumo001/Start", "%d", Ttime);
  previousTime = Ttime;
  timeFromLastLine = xTaskGetTickCount();
  //Aja sisälle
  while (dig.L3 == 1 && dig.R3 == 1) {
    reflectance_digital( & dig);
    motor_forward(40, 0);
  }

  while (true) {
    reflectance_digital( & dig);
    if (dig.L3 == 1 || dig.R3 == 1) {
      timeFromLastLine = xTaskGetTickCount();

    }
    int sub = xTaskGetTickCount() - timeFromLastLine;
    if (sub > 2000) {
      print_mqtt("Zumo001/stop", "%d", xTaskGetTickCount());
      print_mqtt("Zumo001/time", "%d", xTaskGetTickCount() - previousTime);
      motor_forward(0, 0);
      while (true) {
        vTaskDelay(100);
      }
    } else if (dig.L1 == 1 && dig.R1 == 1) {
      reflectance_digital( & dig);
      motor_forward(40, 5);
      reflectance_digital( & dig);
      if (dig.L3 == 1 && dig.R3 == 1) {
        reflectance_digital( & dig);
        motor_forward(0, 0);
        tank_turn(180, 1, 40);
        int d = Ultra_GetDistance();
        if (d < 19) {
          tank_turn(180, 0, 40);
        } else {
          motor_forward(40, 5);
        }
      }
    } else if (dig.L1 == 1 || dig.R1 == 0) {
      reflectance_digital( & dig);
      motor_turn(0, 200, 10);

    } else if (dig.L1 == 0 || dig.R1 == 1) {
      reflectance_digital( & dig);
      motor_turn(200, 0, 10);
    }
    d = Ultra_GetDistance();
    if (d <= 9) {
      tank_turn(180, 0, 40);
      if (dig.L3 && dig.R3 == 1) {
        motor_turn(200, 0, 10);
      }
    }
  }
}

#endif