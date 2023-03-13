#if 1 //race track
void tank_turn(uint8 angle, bool Direction, uint32 delay) {
  int negativeSpeed = 0 - angle;
  if (Direction == true) {
    SetMotors(0, 1, negativeSpeed, angle, delay);
  } else {
    SetMotors(1, 0, negativeSpeed, angle, delay);
  }
}

void zmain(void) {

  TickType_t Ttime = xTaskGetTickCount(); // take start time
  TickType_t previousTime = 0;
  IR_Start();
  motor_start();
  motor_forward(0, 0);
  Ultra_Start();
  struct sensors_ dig;
  reflectance_start();
  reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); 
  int line_amount = 0;
  bool started = false;
  while (SW1_Read() == 1) {
    vTaskDelay(100);
  }
  reflectance_digital( & dig);
  //Aja viivalle
  while (dig.L3 == 0 && dig.R3 == 0) {
    reflectance_digital( & dig);
    motor_forward(100, 20);
  }
  //Pysäytä moottori
  motor_forward(0, 0);
  send_mqtt("Zumo001/Ready", "Zumo");
  //Lähetä aloitusviesti
  IR_wait();
  //Odota IR
  Ttime = xTaskGetTickCount();
  print_mqtt("Zumo001/Start", "%u", Ttime);
  previousTime = Ttime;
  //Aja sisälle
  while (dig.L3 == 1 && dig.R3 == 1) {
    reflectance_digital( & dig);
    motor_forward(100, 0);
  }

  while (true) {
    if (dig.L3 == 1 && dig.R3 == 1) {
      line_amount++;
      if (line_amount == 2) {
        while (true) {
          motor_forward(0, 0);
          Ttime = xTaskGetTickCount();
          print_mqtt("Zumo01/Stop", "%u", Ttime);
          print_mqtt("Zumo01/Time", "%u", Ttime);
          while (true) {
            vTaskDelay(100);
          }
        }
      } else {
        while ((dig.L3 == 1) && (dig.R3 == 1)) {
          reflectance_digital( & dig);
          motor_forward(100, 20);
        }
      }
    }
    reflectance_digital( & dig);
    if (dig.L1 == 1 && dig.R1 == 1) {
      reflectance_digital( & dig);
      motor_forward(200, 20);
    } else if (dig.L1 == 1 || dig.R1 == 0) {
      reflectance_digital( & dig);
      motor_turn(10, 200, 10);
    } else if (dig.L1 == 0 || dig.R1 == 1) {
      reflectance_digital( & dig);
      motor_turn(200, 10, 10);
    }
  }

}