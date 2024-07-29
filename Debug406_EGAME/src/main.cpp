#include <Stepmotor.h>
#include <tft_driver.h>
#include <key_driver.h>
#include <serial.h>
#include <task.h>
#include <pid.h>
#include <wifi_driver.h>


void setup()
{
  // put your setup code here, to run once:
  TFT_init();       // 屏幕初始化
  KEY_init();       // 按键初始化
  Serial_Init();    // 串口初始化
  TASK_Init();      // 任务初始化
  // WIFI_init();      // WIFI初始化
  PIDInit(&MotorPID, 0.35, 0.0, 0.0, 500, -500); // PID参数初始化
  // pinMode(34, INPUT);
  // pinMode(35, INPUT);
  delay(2000);
}

void loop()
{
  // put your main code here, to run repeatedly:
  button1.tick();
  button2.tick();
  button3.tick();
  button4.tick();
  if (taskTimer.TaskRun)
  {
    TASK_RUN();
    taskTimer.TaskRun = false;
  }
  // Serial.print(analogRead(34));
  // Serial.print(",");
  // Serial.println(analogRead(35));
}
