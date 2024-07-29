#ifndef _TASK_H
#define _TASK_H
#define DEBUG 0
#include <Arduino.h>
#include <Stepmotor.h>
#include <tft_driver.h>
#include <pid.h>
#include <math.h>
#include <serial.h>


typedef struct TASKTimer
{
    bool TaskRun;
    uint32_t timerCount;
}TASKTimer_t;

extern uint8_t currTask;
extern TASKTimer_t taskTimer;

void TASK_RUN(void);
void TASK_Init(void);

#endif // !_TASK_H
