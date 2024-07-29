#include <key_driver.h>

uint8_t KeyState = 0;
uint8_t LastLine = 0;
uint8_t rxCmd_[128] = {0}; 
uint8_t rxCount_ = 0;
OneButton button1(S1, true); // ��������2������Ϊ�͵�ƽ
OneButton button2(S2, true); // ��������3������Ϊ�͵�ƽ
OneButton button3(S3, true); // ��������2������Ϊ�͵�ƽ
OneButton button4(S4, true); // ��������3������Ϊ�͵�ƽ
uint8_t Button1 = 0;
uint8_t Button2 = 0;
uint8_t Button3 = 0;
uint8_t Button4 = 0;
// ����״̬����

/**@brief   ����1�ж�
-- @param   ��
-- @date    2023/7/29
**/
void S1_callBack()
{
    Button1 = 1;
}

/**@brief   ����2�ж�
-- @param   ��
-- @date    2023/7/29
**/
void S2_callBack()
{
    Button2 = 1;
}

/**@brief   ����3�ж�
-- @param   ��
-- @date    2023/7/29
**/
void S3_callBack()
{
    Button3 = 1;
}

/**@brief   ����4�ж�
-- @param   ��
-- @date    2023/7/29
**/
void S4_callBack()
{
    Button4 = 1;
}

void S4_duringLongPress()
{
    Button4 = 2;
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
  button4.attachDuringLongPress(S4_duringLongPress);//ע�᳤��
}
