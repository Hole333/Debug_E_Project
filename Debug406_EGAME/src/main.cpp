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
  TFT_init();       // ��Ļ��ʼ��
  KEY_init();       // ������ʼ��
  Serial_Init();    // ���ڳ�ʼ��
  TASK_Init();      // �����ʼ��
  // WIFI_init();      // WIFI��ʼ��
  PIDInit(&MotorPID, 0.35, 0.0, 0.0, 500, -500); // PID������ʼ��
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
