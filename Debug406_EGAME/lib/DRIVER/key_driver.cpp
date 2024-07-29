#include <key_driver.h>

uint8_t KeyState = 0;
uint8_t LastLine = 0;
uint8_t rxCmd_[128] = {0}; 
uint8_t rxCount_ = 0;
OneButton button1(S1, true); // 创建按键2，按下为低电平
OneButton button2(S2, true); // 创建按键3，按下为低电平
OneButton button3(S3, true); // 创建按键2，按下为低电平
OneButton button4(S4, true); // 创建按键3，按下为低电平
uint8_t Button1 = 0;
uint8_t Button2 = 0;
uint8_t Button3 = 0;
uint8_t Button4 = 0;
// 按键状态处理

/**@brief   按键1中断
-- @param   无
-- @date    2023/7/29
**/
void S1_callBack()
{
    Button1 = 1;
}

/**@brief   按键2中断
-- @param   无
-- @date    2023/7/29
**/
void S2_callBack()
{
    Button2 = 1;
}

/**@brief   按键3中断
-- @param   无
-- @date    2023/7/29
**/
void S3_callBack()
{
    Button3 = 1;
}

/**@brief   按键4中断
-- @param   无
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
  // 按键初始化
  // pinMode(S1, INPUT);                       // 输入上拉模式
  // attachInterrupt(S1, S1_callBack, CHANGE); // 使能中断
  // pinMode(S2, INPUT);                       // 输入上拉模式
  // attachInterrupt(S2, S2_callBack, CHANGE); // 使能中断
  // pinMode(S3, INPUT);                       // 输入上拉模式
  // attachInterrupt(S3, S3_callBack, CHANGE); // 使能中断
  // pinMode(S4, INPUT);                       // 输入上拉模式
  // attachInterrupt(S4, S4_callBack, CHANGE); // 使能中断
  button1.reset(); // 清除一下按钮状态机的状态
  button1.attachClick(S1_callBack);
  button2.reset(); // 清除一下按钮状态机的状态
  button2.attachClick(S2_callBack);
  button3.reset(); // 清除一下按钮状态机的状态
  button3.attachClick(S3_callBack);
  button4.reset(); // 清除一下按钮状态机的状态
  button4.attachClick(S4_callBack); 
  button4.attachDuringLongPress(S4_duringLongPress);//注册长按
}
