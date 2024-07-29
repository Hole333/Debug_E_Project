#include <serial.h>


RX_Buffer_t RX_Buffer;
RX_Buffer_t RX_DiffBuffer;

void RX_Buffer_Init(void)
{
    memset(RX_Buffer.Buffer,0,sizeof(RX_Buffer.Buffer));
    RX_Buffer.length = 0;
    memset(RX_DiffBuffer.Buffer,0,sizeof(RX_DiffBuffer.Buffer));
    RX_DiffBuffer.length = 0;
}

/**
 * @brief : 读取上位机串口数据
 * @data:   第一位数据 确定步进电机运动状态
 * 
 */
void Serial_Get(void)
{
    uint8_t rxData = Serial.read();
    RX_Buffer.Buffer[RX_Buffer.length] = rxData;
    RX_Buffer.length++;
    // 检测帧尾
    if (rxData == 0x5A)
    {
        if (RX_Buffer.length == 5)
        {
            if (RX_Buffer.Buffer[0] == 0x01)
            {
                Motor.isRun = true;
                if (Motor.targerSpeed > 0)
                {
                    Emm_V5_Vel_Control(Motor.addr, CW,  abs(Motor.targerSpeed), Motor.acc, 0);
                }
                else
                {
                    Emm_V5_Vel_Control(Motor.addr, CCW,  abs(Motor.targerSpeed), Motor.acc, 0);
                }
            }
            else
            {
                Motor.isRun = false;
            }
            memset(RX_Buffer.Buffer,0,sizeof(RX_Buffer.Buffer));
            RX_Buffer.length = 0;
        }
        else
        {
            memset(RX_Buffer.Buffer,0,sizeof(RX_Buffer.Buffer));
            RX_Buffer.length = 0;
        }
    }
}

void Serial2_Get(void)
{
    uint8_t rxData = Serial2.read();
    RX_DiffBuffer.Buffer[RX_DiffBuffer.length] = rxData;
    RX_DiffBuffer.length++;
    // 检测帧尾
    if (rxData == 0x5B)
    {
        if (RX_DiffBuffer.length == 5)
        {
            pixelDiff.dx = (float)((RX_DiffBuffer.Buffer[1] >> 7) == 0) ? ((RX_DiffBuffer.Buffer[0] + (RX_DiffBuffer.Buffer[1] << 8))) : (-(65536 - (RX_DiffBuffer.Buffer[0] + (RX_DiffBuffer.Buffer[1] << 8))));
            pixelDiff.dy = (float)((RX_DiffBuffer.Buffer[3] >> 7) == 0) ? ((RX_DiffBuffer.Buffer[2] + (RX_DiffBuffer.Buffer[3] << 8))) : (-(65536 - (RX_DiffBuffer.Buffer[2] + (RX_DiffBuffer.Buffer[3] << 8))));
            memset(RX_DiffBuffer.Buffer,0,sizeof(RX_DiffBuffer.Buffer));
            RX_DiffBuffer.length = 0;
        }
        else
        {
            memset(RX_DiffBuffer.Buffer,0,sizeof(RX_DiffBuffer.Buffer));
            RX_DiffBuffer.length = 0;
        }
    }
}


void Serial_CallBack(void)
{
    while(Serial.available())//存在数据
    {
        Serial_Get();
    }
}

void Serial2_CallBack(void)
{
    while(Serial2.available())//存在数据
    {
        Serial2_Get();
    }
}

void Serial_Init(void)
{
    Serial.begin(115200);  // 上位机串口
    Serial1.begin(115200,SERIAL_8N1,25, 26); // 步进电机驱动串口
    Serial2.begin(115200);
    Serial.onReceive(Serial_CallBack);
    Serial2.onReceive(Serial2_CallBack);
    RX_Buffer_Init();
}




