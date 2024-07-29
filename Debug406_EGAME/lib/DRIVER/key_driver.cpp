#include <key_driver.h>

uint8_t KeyState = 0;
uint8_t LastLine = 0;
uint8_t rxCmd_[128] = {0}; 
uint8_t rxCount_ = 0;
OneButton button1(S1, true); // ��������2������Ϊ�͵�ƽ
OneButton button2(S2, true); // ��������3������Ϊ�͵�ƽ
OneButton button3(S3, true); // ��������2������Ϊ�͵�ƽ
OneButton button4(S4, true); // ��������3������Ϊ�͵�ƽ

// ����״̬����

/**@brief   ����1�ж�
-- @param   ��
-- @date    2023/7/29
**/
void S1_callBack()
{
  if (KeyState == 0)
  {
    Menu_List();
    LastLine = cursor.currLine;
    tft.fillRect(strlen(menuName[LastLine]) * 12, LastLine * 16, 40, 20, TFT_BLACK);
    cursor.currLine += 1; // �кż�1
    if (cursor.currLine >= menuNum)
    {
      cursor.currLine = 0;
    }
    tft.drawString(cursor.ico, strlen(menuName[cursor.currLine]) * 12, cursor.currLine * 16);
  }
}

/**@brief   ����2�ж�
-- @param   ��
-- @date    2023/7/29
**/
void S2_callBack()
{
  if (KeyState == 0)
  {
    switch (cursor.currLine)
    {
    case 0:
      tft.fillScreen(TFT_BLACK);
      tft.drawString("currAngle:", 0, 0);
      tft.drawString("setAngle:", 0, 40);
      currTask = 1;
      KeyState = 1;
      break;
    case 1:
      tft.fillScreen(TFT_BLACK);
      tft.drawString("isRun:", 0, 0);
      tft.drawString("currSpeed:", 0, 40);
      tft.drawString("setSpeed:", 0, 80);
      currTask = 2;
      KeyState = 2;
      break;
    case 2:
      tft.fillScreen(TFT_BLACK);
      tft.drawString("dx:", 0, 0);
      tft.drawString("dy:", 0, 40);
      tft.drawString("PIDdx:", 0, 80);
      currTask = 3;
      KeyState = 3;
      break;
    default:
      break;
    }
  }
  else if (KeyState == 1)
  {
    Motor.targetAngle += 5.0;
    Emm_V5_Pos_Control(Motor.addr, CW, 1000, 0,  (5.0f/ 360.0f) * 3200, false, 0);
    // Emm_V5_Receive_Data(rxCmd_, &rxCount_);
    // while(rxCmd[rxCount_ - 1] != 0x6B)
    // {
    //   Emm_V5_Pos_Control(Motor.addr, CW, 1000, 0,  (5.0f/ 360.0f) * 3200, false, 0);
    //   Emm_V5_Receive_Data(rxCmd_, &rxCount_);
    // }
    // memset(rxCmd_, 0, sizeof(rxCmd_));
  }
  else if(KeyState == 2)
  {
    Motor.targerSpeed += 5.0f;
    if (Motor.targerSpeed > 0)
    {
      Emm_V5_Vel_Control(Motor.addr, CW, abs(Motor.targerSpeed), Motor.acc, 0);
    }
    else
    {
      Emm_V5_Vel_Control(Motor.addr, CCW, abs(Motor.targerSpeed), Motor.acc, 0);
    }
  }
}

/**@brief   ����3�ж�
-- @param   ��
-- @date    2023/7/29
**/
void S3_callBack()
{
  if (KeyState == 1)
  {
    Motor.targetAngle -= 5.0f;
    Emm_V5_Pos_Control(Motor.addr, CCW, 1000, 0, (5.0f / 360.0f) * 3200, false, 0);
    // Emm_V5_Receive_Data(rxCmd_, &rxCount_);
    // while(rxCmd[rxCount_ - 1] != 0x6B)
    // {
    //   Emm_V5_Pos_Control(Motor.addr, CCW, 1000, 0,  (5.0f/ 360.0f) * 3200, false, 0);
    //   Emm_V5_Receive_Data(rxCmd_, &rxCount_);
    // }
    // memset(rxCmd_, 0, sizeof(rxCmd_));
    //Motor.currAngle = Motor.targetAngle;
  }
  else if(KeyState == 2)
  {
    Motor.targerSpeed -= 5;
    if (Motor.targerSpeed > 0)
    {
      Emm_V5_Vel_Control(Motor.addr, CW,  abs(Motor.targerSpeed), Motor.acc, 0);
    }
    else
    {
      Emm_V5_Vel_Control(Motor.addr, CCW,  abs(Motor.targerSpeed), Motor.acc, 0);
    }
  }
}

/**@brief   ����4�ж�
-- @param   ��
-- @date    2023/7/29
**/
void S4_callBack()
{
    KeyState = 0;
    currTask = 0;
    cursor.currLine = 0;
    Motor.targetAngle = 0;
    Motor.currAngle = 0;
    Motor.targerSpeed = 0;
    //Emm_V5_Stop_Now(Motor.addr, 0);
    Emm_V5_Reset_CurPos_To_Zero(Motor.addr);
    tft.fillScreen(TFT_BLACK);
    Menu_List();
    LastLine = cursor.currLine;
    tft.drawString(cursor.ico, strlen(menuName[cursor.currLine]) * 12, cursor.currLine * 16);
}

void KEY_init(void)
{
  // ������ʼ��
  // pinMode(S1, INPUT);                       // ��������ģʽ
  // attachInterrupt(S1, S1_callBack, CHANGE); // ʹ���ж�
  // pinMode(S2, INPUT);                       // ��������ģʽ
  // attachInterrupt(S2, S2_callBack, CHANGE); // ʹ���ж�
  // pinMode(S3, INPUT);                       // ��������ģʽ
  // attachInterrupt(S3, S3_callBack, CHANGE); // ʹ���ж�
  // pinMode(S4, INPUT);                       // ��������ģʽ
  // attachInterrupt(S4, S4_callBack, CHANGE); // ʹ���ж�
  button1.reset(); // ���һ�°�ť״̬����״̬
  button1.attachClick(S1_callBack);
  button2.reset(); // ���һ�°�ť״̬����״̬
  button2.attachClick(S2_callBack);
  button3.reset(); // ���һ�°�ť״̬����״̬
  button3.attachClick(S3_callBack);
  button4.reset(); // ���һ�°�ť״̬����״̬
  button4.attachClick(S4_callBack);
}
