#ifndef _KEY_DRIVER_H
#define _KEY_DRIVER_H
#include <Arduino.h>
#include <tft_driver.h>
#include <Stepmotor.h>
#include <task.h>
#include <OneButton.h>

//按键引脚定义
#define S1 0
#define S2 35
#define S3 34
#define S4 39


extern uint8_t KeyState;
extern OneButton button1;
extern OneButton button2;
extern OneButton button3;
extern OneButton button4;
 
void KEY_init(void);


#endif // !_KEY_DRIVER
