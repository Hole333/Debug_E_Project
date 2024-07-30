#include <task.h>

TASKTimer_t taskTimer = 
{
    .TaskRun = false,
    .timerCount = 0
};
hw_timer_t *timer = NULL; 
uint8_t currTask; 




void SET_ANGLE_TASK(void)
{
    uint32_t pos = 0;
    // 读取电机实时位置
    Emm_V5_Read_Sys_Params(1, S_CPOS);

    // 等待返回命令，命令数据缓存在数组rxCmd上，长度为rxCount
    #if DEBUG
        Serial.println("wait");
    #endif
    Emm_V5_Receive_Data(rxCmd, &rxCount);
    #if DEBUG
        Serial.println("receive");
    #endif
    if (rxCmd[0] == 1 && rxCmd[1] == 0x36 && rxCount == 8)
    {
        // 拼接成uint32_t类型
        pos = (uint32_t)(((uint32_t)rxCmd[3] << 24) |
                         ((uint32_t)rxCmd[4] << 16) |
                         ((uint32_t)rxCmd[5] << 8) |
                         ((uint32_t)rxCmd[6] << 0));

        // 转换成角度
        Motor.currAngle = (float)pos * 360.0f / 65536.0f;

        // 符号
        if (rxCmd[2])
        {
            Motor.currAngle = -Motor.currAngle;
        }
    }
    tft.drawFloat(Motor.currAngle, 2, 0, 16);
    tft.drawFloat(Motor.targetAngle, 2, 0, 56);
}

void RUN_MOTOR_TASK(void)
{
    uint16_t vel = 0;
    // 读取电机实时转速
    Emm_V5_Read_Sys_Params(1, S_VEL);

    // 等待返回命令，命令数据缓存在数组rxCmd上，长度为rxCount
    Emm_V5_Receive_Data(rxCmd, &rxCount);

    // 校验地址、功能码、返回数据长度，验证成功则点亮LED灯，否则熄灭LED灯
    if (rxCmd[0] == 1 && rxCmd[1] == 0x35 && rxCount == 6)
    {
        // 点亮LED灯

        // 拼接成uint16_t类型数据
        vel = (uint16_t)(((uint16_t)rxCmd[3] << 8) |
                         ((uint16_t)rxCmd[4] << 0));

        // 实时转速
        Motor.currSpeed = vel;

        // 符号
        if (rxCmd[2])
        {
            Motor.currSpeed = -Motor.currSpeed;
        }
    }
    if (Motor.isRun == true)
    {   
        tft.drawString("RUN ", 0, 24);
    }
    else
    {
        tft.drawString("STOP", 0, 24);
        Emm_V5_Stop_Now(Motor.addr, false); // 让电机立即停止运动
    }
    tft.drawFloat(Motor.currSpeed, 2, 0, 60);
    #if DEBUG == 1
        Serial.println(Motor.currSpeed);
    #endif
    tft.drawFloat(Motor.targerSpeed, 2, 0, 100);
}


void CORRECT_ERR_MOTOR(void)
{
    float PID_Err = GetPIDValue(&MotorPID, pixelDiff.dx);
    tft.drawFloat(pixelDiff.dx, 2, 0, 16);
    tft.drawFloat(pixelDiff.dy, 2, 0, 56);
    tft.drawFloat(PID_Err, 2, 0, 96);
    if (PID_Err < 0)
    {
        //Emm_V5_Vel_Control(Motor.addr, CW, PID_Err, Motor.acc, 0)
        Emm_V5_Pos_Control(Motor.addr, CW, (uint32_t)abs(PID_Err), 0,  (uint32_t)PID_Err, false, 0);
    }
    else
    {
        //Emm_V5_Vel_Control(Motor.addr, CCW, PID_Err, Motor.acc, 0);
        Emm_V5_Pos_Control(Motor.addr, CCW, (uint32_t)abs(PID_Err), 0,  (uint32_t)PID_Err, false, 0);
    }
}

void TASK_RUN(void)
{
    switch (currTask)
    {
    case 1:
        SET_ANGLE_TASK();
        break;
    case 2:
        RUN_MOTOR_TASK();
        break;
    case 3:
        CORRECT_ERR_MOTOR();
        break;
    default:
        //Emm_V5_Reset_CurPos_To_Zero(Motor.addr);
        break;
    }
}

extern uint16_t Step_Time;
extern uint16_t Step_Time_Z;
extern uint16_t Step_Time_Receive;
void TASK_Handle(void)
{
    taskTimer.timerCount++;
    if (taskTimer.timerCount == 4)
    {
        taskTimer.TaskRun = true;
        taskTimer.timerCount = 0;
    }
    if(Step_Time>0)
    {
        Step_Time++;
    }
    if(Step_Time_Z>0)
    {
        Step_Time_Z++;
    }
    if(Step_Time_Receive>0)
    {
        Step_Time_Receive++;
    }
}

void TASK_Init(void)
{
    timer = timerBegin(0, 80, true);
    // 定时器中断服务函数
    timerAttachInterrupt(timer, &TASK_Handle, true);

    // 启动定时器 设置20ms中断
    timerAlarmWrite(timer, 5000, true);
    timerAlarmEnable(timer);
}