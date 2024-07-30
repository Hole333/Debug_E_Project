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
    // ��ȡ���ʵʱλ��
    Emm_V5_Read_Sys_Params(1, S_CPOS);

    // �ȴ���������������ݻ���������rxCmd�ϣ�����ΪrxCount
    #if DEBUG
        Serial.println("wait");
    #endif
    Emm_V5_Receive_Data(rxCmd, &rxCount);
    #if DEBUG
        Serial.println("receive");
    #endif
    if (rxCmd[0] == 1 && rxCmd[1] == 0x36 && rxCount == 8)
    {
        // ƴ�ӳ�uint32_t����
        pos = (uint32_t)(((uint32_t)rxCmd[3] << 24) |
                         ((uint32_t)rxCmd[4] << 16) |
                         ((uint32_t)rxCmd[5] << 8) |
                         ((uint32_t)rxCmd[6] << 0));

        // ת���ɽǶ�
        Motor.currAngle = (float)pos * 360.0f / 65536.0f;

        // ����
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
    // ��ȡ���ʵʱת��
    Emm_V5_Read_Sys_Params(1, S_VEL);

    // �ȴ���������������ݻ���������rxCmd�ϣ�����ΪrxCount
    Emm_V5_Receive_Data(rxCmd, &rxCount);

    // У���ַ�������롢�������ݳ��ȣ���֤�ɹ������LED�ƣ�����Ϩ��LED��
    if (rxCmd[0] == 1 && rxCmd[1] == 0x35 && rxCount == 6)
    {
        // ����LED��

        // ƴ�ӳ�uint16_t��������
        vel = (uint16_t)(((uint16_t)rxCmd[3] << 8) |
                         ((uint16_t)rxCmd[4] << 0));

        // ʵʱת��
        Motor.currSpeed = vel;

        // ����
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
        Emm_V5_Stop_Now(Motor.addr, false); // �õ������ֹͣ�˶�
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
    // ��ʱ���жϷ�����
    timerAttachInterrupt(timer, &TASK_Handle, true);

    // ������ʱ�� ����20ms�ж�
    timerAlarmWrite(timer, 5000, true);
    timerAlarmEnable(timer);
}