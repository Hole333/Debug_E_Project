#include <Stepmotor.h>
#include <tft_driver.h>
#include <key_driver.h>
#include <serial.h>
#include <task.h>
#include <pid.h>
#include <wifi_driver.h>
#include<Chess_decision.h>

//变量定义
#define Electromagnet 14//继电器引脚
#define Electromagnet_ON digitalWrite(Electromagnet, HIGH)
#define Electromagnet_OFF digitalWrite(Electromagnet, LOW)
#define LED 13
#define LED_ON digitalWrite(LED, HIGH)
#define LED_OFF digitalWrite(LED, LOW)
#define X_Stepmotor 1
#define Y_Stepmotor 2
#define Z_Stepmotor 3
#define Take_Up Emm_V5_Pos_Control(Z_Stepmotor,CW,50,0,0,true,false) //Z轴抬起
#define Take_Down Emm_V5_Pos_Control(Z_Stepmotor,CW,50,0,1000,true,false) //Z轴下降

uint16_t Step_Time = 0;//计算发送命令时间,电机地址1是X轴，2是Y轴，3是Z轴
uint16_t Step_Time_Z = 0;//计算发送命令时间,电机地址1是X轴，2是Y轴，3是Z轴
uint16_t Step_Time_Receive = 0;//计算发送命令时间,电机地址1是X轴，2是Y轴，3是Z轴
uint16_t Step_X = 0;
uint16_t Step_Y = 0;
uint16_t Chessboard_Pos[10][2]={{0,0},{132,142},{100,142},{68,142},{132,110},{100,110},{68,110},{132,78},{100,78},{68,78},};//后面0表示X坐标，1表示Y坐标
//uint16_t chesssorce_Pos[12][2]={{0,0},{185,160},{185,135},{185,110},{185,85},{185,60},{15,160},{15,135},{15,110},{15,85},{15,60},{0,0}};//后面0表示X坐标，1表示Y坐标
uint16_t chesssorce_Pos[12][2]={{0,0},{185,60},{185,85},{185,110},{185,135},{185,160},{15,60},{15,85},{15,110},{15,135},{15,160},{0,0}};//后面0表示X坐标，1表示Y坐标
uint16_t BigChess_Pos[10]={0,0,0,0,0,0,0,0,0,0};//后面0表示无棋，1表示黑，2表示白
uint16_t BigChess_Posold[10]={0,0,0,0,0,0,0,0,0,0};//后面0表示无棋，1表示黑，2表示白
uint8_t Uart_Busy = 1;//是否在使用步进电机串口
uint8_t All_OK_Flag = 0;//X、Y都到位
uint8_t Receive_Count = 0;//接受次数
//x 1mm对应 79个脉冲 y 1mm对应 75个脉冲
//函数定义
void StepMotor_Control(int16_t X_Step, int16_t Y_Step,uint16_t Speed);
void Menu_Task(void);
void Control_Task(void);

//初始化
void setup()
{
    // put your setup code here, to run once:
    delay(1000);
    TFT_init();       // 屏幕初始化
    Parameter_Init(); //参数初始化
    KEY_init();       // 按键初始化
    Serial_Init();    // 串口初始化
    for(uint8_t j = 1;j<=9;j++)
    {
        Serial.print("(");
        Serial.print(Chessboard_Pos[j][0]);
        Serial.print(",");
        Serial.print(Chessboard_Pos[j][1]);
        Serial.println(")");
    }
    TASK_Init();      // 任务初始化
    pinMode(Electromagnet, OUTPUT);//继电器初始化
    pinMode(LED, OUTPUT);//继电器初始化
}


void loop()
{
    // put your main code here, to run repeatedly:
    button1.tick();
    button2.tick();
    button3.tick();
    button4.tick();
    Menu_Task();
    Control_Task();

    if(Button4==1)
    {
        StepMotor_Control(Step_X,Step_Y,300);
    }
    else if(Button4==2)
    {
        StepMotor_Control(0,0,300); 
    }
}


/**@brief     设置X、Y轴的电机运动
-- @param     X_Step X轴走的绝对角度
-- @param     Y_Step Y轴走的绝对角度
-- @param     Speed 速度
-- @date      2024/7/30
**/
void StepMotor_Control(int16_t X_Step, int16_t Y_Step,uint16_t Speed)
{
    X_Step=X_Step*80;
    Y_Step=Y_Step*80;
    static uint8_t Step_Mode = 0;
    switch (Step_Mode)
    {
        case 0://发送X轴命令
            if(Step_Time==0)
            {
                if(X_Step >= 0)
                {
                    Emm_V5_Pos_Control(X_Stepmotor,CW,Speed,0,X_Step,true,true);//绝对角度，多机同步
                    Uart_Busy = 1;
                }
                else
                {
                    Emm_V5_Pos_Control(X_Stepmotor,CCW,Speed,0,X_Step,true,true);
                    Uart_Busy = 1;
                }
                Step_Time = 1;
            }
            if(Step_Time>=5)
            {
                Step_Mode = 1;
                Step_Time = 0;
            }
        break;
        case 1://发送Y轴命令
            if(Step_Time==0)
            {
                if(Y_Step > 0)
                {
                    Emm_V5_Pos_Control(Y_Stepmotor,CW,Speed,0,Y_Step,true,true);//绝对角度，多机同步
                    Uart_Busy = 1;
                }
                else
                {
                    Emm_V5_Pos_Control(Y_Stepmotor,CCW,Speed,0,Y_Step,true,true);
                    Uart_Busy = 1;
                }
                Step_Time = 1;
            }

            if(Step_Time>=5)
            {
                Step_Mode = 2;
                Step_Time = 0;
            }
        break;
        case 2:
            if(Step_Time==0)
            {
                Emm_V5_Synchronous_motion(0x00);
                Step_Time = 1;
            }

            if(Step_Time>=5)
            {
                Step_Mode = 0;
                Step_Time = 0;
                // Button4 = 0;
                Uart_Busy = 0;
            }
        break;
    }
}

/**@brief     X、Y是否到位
-- @param     无
**/
void Step_All_OK(void)
{
    static uint8_t Step_Mode = 0;
    static uint8_t X_Step_OK = 0;
    static uint8_t Y_Step_OK = 0;
    
    switch (Step_Mode)
    {
        case 0://发送X轴命令
            if(Uart_Busy==0)
            {
                if(Step_Time==0)
                {
                    Step_Time = 1;
                }

                if(Step_Time>=5)
                {
                    Emm_V5_Read_Sys_Params(X_Stepmotor, S_FLAG);

                    // 等待返回命令，命令数据缓存在数组rxCmd上，长度为rxCount
                    Emm_V5_Receive_Data(rxCmd, &rxCount);
                    if (rxCmd[0] == 0x01 && rxCmd[1] == 0x3A)
                    {
                        X_Step_OK = rxCmd[2] & 0x02;
                    }
                    Step_Time = 0;
                }

                if(X_Step_OK)
                {
                    Step_Mode = 1;
                }
            }
        break;
        case 1://发送Y轴命令
            if(Uart_Busy==0)
            {
                if(Step_Time==0)
                {
                    Step_Time = 1;
                }

                if(Step_Time>=5)
                {
                    Emm_V5_Read_Sys_Params(Y_Stepmotor, S_FLAG);

                    // 等待返回命令，命令数据缓存在数组rxCmd上，长度为rxCount
                    Emm_V5_Receive_Data(rxCmd_Y, &rxCount);
                    if (rxCmd_Y[0] == 0x02 && rxCmd_Y[1] == 0x3A)
                    {
                        Y_Step_OK = rxCmd_Y[2] & 0x02;
                    }
                    Step_Time = 0;
                }

                if((Y_Step_OK) && (X_Step_OK))
                {
                    All_OK_Flag = 1;
                    X_Step_OK = 0;
                    Y_Step_OK = 0;
                    Step_Mode = 0;
                    Button4 = 0; 
                }
                else
                {
                    All_OK_Flag = 0;
                    // Button4 = 1;
                }
            }
        break;
    }
}

/**@brief     要求一控制任务
-- @param     无
**/
void Mode1_Control()
{
    static uint8_t Mode1_State = 0;
    switch (Mode1_State)
    {
        case 0://移动到任意黑棋从拿黑棋
            if(Chesssource_Count!=0)
            {
                Step_X = chesssorce_Pos[Chesssource_Count][0];
                Step_Y = chesssorce_Pos[Chesssource_Count][1];
                if(Step_Time_Receive==0 && Button4==1)
                {
                    Step_Time_Receive = 1;
                }

                if(Step_Time_Receive >= 800)
                {
                    Step_Time_Receive = 0;
                    All_OK_Flag = 1;
                    Button4 = 0;
                }

                if(All_OK_Flag)
                {
                    Step_Time_Receive = 0;
                    if(Step_Time_Z == 0)
                    {
                        Step_Time_Z = 1;
                    }
                    
                    if((Step_Time_Z >= 1) && (Step_Time_Z < 45))
                    {
                        Take_Down;
                    }

                    if(Step_Time_Z >= 75)
                    {
                        Electromagnet_ON;
                        Take_Up;
                    }

                    if(Step_Time_Z >= 110)
                    {
                        Step_Time_Z = 0;
                        Mode1_State = 1;
                        Step_X = Chessboard_Pos[5][0];
                        Step_Y = Chessboard_Pos[5][1];
                        All_OK_Flag = 0;
                        Button4 = 1; 
                    }
                }
            }
        break;
        case 1://放到5号格子
            Step_X = Chessboard_Pos[5][0];
            Step_Y = Chessboard_Pos[5][1];

            if(Step_Time_Receive==0 && Button4==1)
            {
                Step_Time_Receive = 1;
            }

            if(Step_Time_Receive >= 400)
            {
                Step_Time_Receive = 0;
                All_OK_Flag = 1;
                Button4 = 0;
            }

            if(All_OK_Flag)
            {
                if(Step_Time_Z == 0)
                {
                    Step_Time_Z = 1;
                }

                if((Step_Time_Z >= 1) && (Step_Time_Z < 45))
                {
                    Take_Down;
                }

                if(Step_Time_Z >= 35)
                {
                    Electromagnet_OFF;
                }

                if(Step_Time_Z >= 60)
                {
                    Take_Up;
                }

                if(Step_Time_Z >= 120)
                {
                    Step_Time_Z = 0;
                    Mode1_State = 2;
                }
            }
        break;
        case 2://空状态
            Step_X = 0;
            Step_Y = 0;
            Button4 = 2;//回原点
            All_OK_Flag = 0;
        break;
    }
}

void Mode2_Control()
{
    static uint8_t Mode2_State = 0;
    static uint8_t Take_Count = 0;
    switch (Mode2_State)
    {
        case 0://移动到任意黑棋 白棋中拿棋子
            if(Chesssource_Count!=0)
            {
                Step_X = chesssorce_Pos[Chesssource_Count][0];
                Step_Y = chesssorce_Pos[Chesssource_Count][1];
                if(Step_Time_Receive==0 && Button4==1)
                {
                    Step_Time_Receive = 1;
                }

                if(Step_Time_Receive >= 800)
                {
                    Step_Time_Receive = 0;
                    All_OK_Flag = 1;
                    Button4 = 0;
                }

                if(All_OK_Flag)
                {
                    Step_Time_Receive = 0;
                    if(Step_Time_Z == 0)
                    {
                        Step_Time_Z = 1;
                    }
                    
                    if((Step_Time_Z >= 1) && (Step_Time_Z < 45))
                    {
                        Take_Down;
                    }

                    if(Step_Time_Z >= 75)
                    {
                        Electromagnet_ON;
                        Take_Up;
                    }

                    if(Step_Time_Z >= 110)
                    {
                        Step_Time_Z = 0;
                        Mode2_State = 1;
                       
                        Step_X = Chessboard_Pos[Chess_Pos][0];
                        Step_Y = Chessboard_Pos[Chess_Pos][1];
                        All_OK_Flag = 0;
                        Button4 = 1; 
                    }
                }
            }

           
        break;
        case 1://放到5号格子
            Step_X = Chessboard_Pos[Chess_Pos][0];
            Step_Y = Chessboard_Pos[Chess_Pos][1];

            if(Step_Time_Receive==0 && Button4==1)
            {
                Step_Time_Receive = 1;
            }

            if(Step_Time_Receive >= 600)
            {
                Step_Time_Receive = 0;
                All_OK_Flag = 1;
                Button4 = 0;
            }

            if(All_OK_Flag)
            {
                if(Step_Time_Z == 0)
                {
                    Step_Time_Z = 1;
                }

                if((Step_Time_Z >= 1) && (Step_Time_Z < 45))
                {
                    Take_Down;
                }

                if(Step_Time_Z >= 35)
                {
                    Electromagnet_OFF;
                }

                if(Step_Time_Z >= 60)
                {
                    Take_Up;
                }

                if(Step_Time_Z >= 120)
                {
                    Step_Time_Z = 0;
                    Mode2_State = 2;
                }
            }
        break;
        case 2://等待状态
            Step_X = 0;
            Step_Y = 0;
            Button4 = 2;//回原点
            All_OK_Flag = 0;
            if(Step_Time_Receive==0)
            {
                Step_Time_Receive = 1;
            }

            if(Step_Time_Receive >= 600)
            {
                Mode2_State = 0;
                Button4 = 0;
                Step_Time_Receive=0;
            }

        break;
        case 3:
            Step_X = 0;
            Step_Y = 0;
            Button4 = 2;//回原
            All_OK_Flag = 0;
        break;
    }
}

void Mode3_Control(void)
{
    static uint8_t Mode3_State = 4;
    static uint8_t Take_Count = 0;
    switch (Mode3_State)
    {
        case 0://移动到任意黑棋 白棋中拿棋子
            if(Chesssource_Count!=0)
            {
                Step_X = chesssorce_Pos[Chesssource_Count][0];
                Step_Y = chesssorce_Pos[Chesssource_Count][1];
                if(Step_Time_Receive==0 && Button4==1)
                {
                    Step_Time_Receive = 1;
                }

                if(Step_Time_Receive >= 800)
                {
                    Step_Time_Receive = 0;
                    All_OK_Flag = 1;
                    Button4 = 0;
                }

                if(All_OK_Flag)
                {
                    Step_Time_Receive = 0;
                    if(Step_Time_Z == 0)
                    {
                        Step_Time_Z = 1;
                    }
                    
                    if((Step_Time_Z >= 1) && (Step_Time_Z < 45))
                    {
                        Take_Down;
                    }

                    if(Step_Time_Z >= 75)
                    {
                        Electromagnet_ON;
                        Take_Up;
                    }

                    if(Step_Time_Z >= 110)
                    {
                        Step_Time_Z = 0;
                        Mode3_State = 1;
                       
                        Step_X = Chessboard_Pos[Chess_Pos][0];
                        Step_Y = Chessboard_Pos[Chess_Pos][1];
                        All_OK_Flag = 0;
                        Button4 = 1; 
                        Take_Count+=1;
                    }
                }
            }

           
        break;
        case 1://放到5号格子
            Step_X = Chessboard_Pos[Chess_Pos][0];
            Step_Y = Chessboard_Pos[Chess_Pos][1];

            if(Step_Time_Receive==0 && Button4==1)
            {
                Step_Time_Receive = 1;
            }

            if(Step_Time_Receive >= 600)
            {
                Step_Time_Receive = 0;
                All_OK_Flag = 1;
                Button4 = 0;
            }

            if(All_OK_Flag)
            {
                if(Step_Time_Z == 0)
                {
                    Step_Time_Z = 1;
                }

                if((Step_Time_Z >= 1) && (Step_Time_Z < 45))
                {
                    Take_Down;
                }

                if(Step_Time_Z >= 35)
                {
                    Electromagnet_OFF;
                }

                if(Step_Time_Z >= 60)
                {
                    Take_Up;
                }

                if(Step_Time_Z >= 120)
                {
                    Step_Time_Z = 0;
                    Mode3_State = 2;
                }
            }
        break;
        case 2://等待状态
            Step_X = 0;
            Step_Y = 0;
            Button4 = 2;//回原点
            All_OK_Flag = 0;
            if(Step_Time_Receive==0)
            {
                Step_Time_Receive = 1;
            }

            if(Step_Time_Receive >= 600)
            {
                Mode3_State = 0;
                Button4 = 0;
                Step_Time_Receive=0;
            }

        break;
        
        case 4:
            Requst_Data;
            if(FinishReceiveFlag)
            {
                FinishReceiveFlag = false;
                for(uint8_t j = 0;j<=8;j++)
                {
                    Serial.print(j);
                    Serial.print("(");
                    Serial.print(chess[j].x);
                    Serial.print(",");
                    Serial.print(chess[j].y);
                    Serial.println(")");

                    Chessboard_Pos[j+1][0] = chess[j].x;
                    Chessboard_Pos[j+1][1] = chess[j].y;
                    BigChess_Pos[j+1]=chess[j].state;
                    Serial.print(j+1);
                    Serial.print("(");
                    Serial.print(Chessboard_Pos[j+1][0]);
                    Serial.print(",");
                    Serial.print(Chessboard_Pos[j+1][1]);
                    Serial.println(")");
                }
                Mode3_State = 0;
            }
        break;
    }
    
    
}
void Mode4_Control(void)
{
    static uint8_t Mode4_State = 4;
    static uint8_t Take_Count = 0;
    switch (Mode4_State)
    {
        case 0://移动到任意黑棋 白棋中拿棋子
            if(Chess_selmodebuff==Chess_selmode)
            {
                Step_X = chesssorce_Pos[Chesssource_Count][0];
                Step_Y = chesssorce_Pos[Chesssource_Count][1];
                if(Step_Time_Receive==0 && Button4==1)
                {
                    Step_Time_Receive = 1;
                }

                if(Step_Time_Receive >= 800)
                {
                    Step_Time_Receive = 0;
                    All_OK_Flag = 1;
                    Button4 = 0;
                }

                if(All_OK_Flag)
                {
                    if(Step_Time_Z == 0)
                    {
                        Step_Time_Z = 1;
                    }
                    
                    if((Step_Time_Z >= 1) && (Step_Time_Z < 45))
                    {
                        Take_Down;
                    }

                    if(Step_Time_Z >= 75)
                    {
                        Electromagnet_ON;
                        Take_Up;
                    }

                    if(Step_Time_Z >= 110)
                    {
                        Step_Time_Z = 0;
                        Mode4_State = 1;
                       
                        Step_X = Chessboard_Pos[Chess_Pos][0];
                        Step_Y = Chessboard_Pos[Chess_Pos][1];
                        All_OK_Flag = 0;
                        Button4 = 1; 
                        Take_Count+=1;
                    }
                }
            }

           
        break;
        case 1://放到棋盘上
            Step_X = Chessboard_Pos[Chess_Pos][0];
            Step_Y = Chessboard_Pos[Chess_Pos][1];

            if(Step_Time_Receive==0 && Button4==1)
            {
                Step_Time_Receive = 1;
            }

            if(Step_Time_Receive >= 600)
            {
                Step_Time_Receive = 0;
                All_OK_Flag = 1;
                Button4 = 0;
            }

            if(All_OK_Flag)
            {
                if(Step_Time_Z == 0)
                {
                    Step_Time_Z = 1;
                }

                if((Step_Time_Z >= 1) && (Step_Time_Z < 45))
                {
                    Take_Down;
                }

                if(Step_Time_Z >= 35)
                {
                    Electromagnet_OFF;
                }

                if(Step_Time_Z >= 60)
                {
                    Take_Up;
                }

                if(Step_Time_Z >= 120)
                {
                    Step_Time_Z = 0;
                    Mode4_State = 2;
                    LED_ON;
                }
            }
        break;
        case 2://等待状态
            Step_X = 0;
            Step_Y = 0;
            Button4 = 2;//回原点
            All_OK_Flag = 0;
            if(Step_Time_Receive==0)
            {
                Step_Time_Receive = 1;
            }

            if(Step_Time_Receive >= 600)
            {
                Mode4_State = 3;
                Chess_selmode++;
                Step_Time_Receive=0;
            }
        break;
        case 3://等待状态
            if(Chess_selmodebuff==Chess_selmode)
            {
                // FinishReceiveFlag=true;
                Mode4_State = 4;
                LED_OFF;
            }
        break;
        case 4:
            Requst_Data;
            // delay(300);
            // Requst_Data;
            if(FinishReceiveFlag)
            {
                FinishReceiveFlag = false;
                for(uint8_t j = 0;j<=8;j++)
                {
                    // Serial.print(j);
                    // Serial.print("(");
                    // Serial.print(chess[j].x);
                    // Serial.print(",");
                    // Serial.print(chess[j].y);
                    // Serial.println(")");
                    Serial.print(chess[j].state);
                    Serial.println("!");

                    Chessboard_Pos[j+1][0] = chess[j].x;
                    Chessboard_Pos[j+1][1] = chess[j].y;
                    BigChess_Pos[j+1]=chess[j].state;
                    // Serial.print(j+1);
                    // Serial.print("(");
                    // Serial.print(Chessboard_Pos[j+1][0]);
                    // Serial.print(",");
                    // Serial.print(Chessboard_Pos[j+1][1]);
                    // Serial.println(")");
                    Serial.print(BigChess_Pos[j+1]);
                    Serial.println(")");
                }
                if(Chess_selmode>0)
                {
                    
                    uint8_t board[3][3];
                    board[0][0]=BigChess_Pos[1];
                    board[0][1]=BigChess_Pos[2];
                    board[0][2]=BigChess_Pos[3];
                    board[1][0]=BigChess_Pos[4];
                    board[1][1]=BigChess_Pos[5];
                    board[1][2]=BigChess_Pos[6];
                    board[2][0]=BigChess_Pos[7];
                    board[2][1]=BigChess_Pos[8];
                    board[2][2]=BigChess_Pos[9];

                    getBestMove(board, 2, &bestRow, &bestCol);
                    Chesssource_Count=Chess_selmode+1;
                    Chess_Pos=bestRow*3+bestCol+1;
                }
                Receive_Count+=1;
            }
            if(Receive_Count>=10)
            {
                Receive_Count = 0;
                Mode4_State = 0;
                if(Chess_selmode>0)
                {
                    Button4 = 1;
                }
            }
        break;
    }
    
    
}
void Mode5_Control(void)
{
    static uint8_t Mode5_State = 4;
    static uint8_t Take_Count = 0;
    switch (Mode5_State)
    {
        case 0://移动到任意黑棋 白棋中拿棋子
            if(Chess_selmodebuff==Chess_selmode)
            {
                Step_X = chesssorce_Pos[Chesssource_Count+5][0];
                Step_Y = chesssorce_Pos[Chesssource_Count+5][1];
                if(Step_Time_Receive==0 && Button4==1)
                {
                    Step_Time_Receive = 1;
                }

                if(Step_Time_Receive >= 800)
                {
                    Step_Time_Receive = 0;
                    All_OK_Flag = 1;
                    Button4 = 0;
                }

                if(All_OK_Flag)
                {
                    Step_Time_Receive = 0;
                    if(Step_Time_Z == 0)
                    {
                        Step_Time_Z = 1;
                    }
                    
                    if((Step_Time_Z >= 1) && (Step_Time_Z < 45))
                    {
                        Take_Down;
                    }

                    if(Step_Time_Z >= 75)
                    {
                        Electromagnet_ON;
                        Take_Up;
                    }

                    if(Step_Time_Z >= 110)
                    {
                        Step_Time_Z = 0;
                        Mode5_State = 1;
                       
                        Step_X = Chessboard_Pos[Chess_Pos][0];
                        Step_Y = Chessboard_Pos[Chess_Pos][1];
                        All_OK_Flag = 0;
                        Button4 = 1; 
                        Take_Count+=1;
                    }
                }
            }
        break;
        case 1://放到5号格子
            Step_X = Chessboard_Pos[Chess_Pos][0];
            Step_Y = Chessboard_Pos[Chess_Pos][1];

            if(Step_Time_Receive==0 && Button4==1)
            {
                Step_Time_Receive = 1;
            }

            if(Step_Time_Receive >= 600)
            {
                Step_Time_Receive = 0;
                All_OK_Flag = 1;
                Button4 = 0;
            }

            if(All_OK_Flag)
            {
                if(Step_Time_Z == 0)
                {
                    Step_Time_Z = 1;
                }

                if((Step_Time_Z >= 1) && (Step_Time_Z < 45))
                {
                    Take_Down;
                }

                if(Step_Time_Z >= 35)
                {
                    Electromagnet_OFF;
                }

                if(Step_Time_Z >= 60)
                {
                    Take_Up;
                }

                if(Step_Time_Z >= 120)
                {
                    LED_ON;
                    Step_Time_Z = 0;
                    Mode5_State = 2;
                }
            }
        break;
        case 2://等待状态
            Step_X = 0;
            Step_Y = 0;
            Button4 = 2;//回原点
            All_OK_Flag = 0;
            if(Step_Time_Receive==0)
            {
                Step_Time_Receive = 1;
            }

            if(Step_Time_Receive >= 600)
            {
                Mode5_State = 3;
                Chess_selmode++;
                Step_Time_Receive=0;
            }
        break;
        case 3://等待状态
            if(Chess_selmodebuff==Chess_selmode)
            {
                // FinishReceiveFlag=true;
                Mode5_State = 4;
                LED_OFF;
            }
        break;
        case 4:
        if(Chess_selmodebuff==Chess_selmode)
            {
            Requst_Data;
            // delay(300);
            // Requst_Data;
            if(FinishReceiveFlag)
            {
                FinishReceiveFlag = false;
                for(uint8_t j = 0;j<=8;j++)
                {
                    // Serial.print(j);
                    // Serial.print("(");
                    // Serial.print(chess[j].x);
                    // Serial.print(",");
                    // Serial.print(chess[j].y);
                    // Serial.println(")");
                    Serial.print(chess[j].state);
                    Serial.println("!");

                    Chessboard_Pos[j+1][0] = chess[j].x;
                    Chessboard_Pos[j+1][1] = chess[j].y;
                    BigChess_Pos[j+1]=chess[j].state;
                    // Serial.print(j+1);
                    // Serial.print("(");
                    // Serial.print(Chessboard_Pos[j+1][0]);
                    // Serial.print(",");
                    // Serial.print(Chessboard_Pos[j+1][1]);
                    // Serial.println(")");
                    Serial.print(BigChess_Pos[j+1]);
                    Serial.println(")");
                }
                
                    
                uint8_t board[3][3];
                board[0][0]=BigChess_Pos[1];
                board[0][1]=BigChess_Pos[2];
                board[0][2]=BigChess_Pos[3];
                board[1][0]=BigChess_Pos[4];
                board[1][1]=BigChess_Pos[5];
                board[1][2]=BigChess_Pos[6];
                board[2][0]=BigChess_Pos[7];
                board[2][1]=BigChess_Pos[8];
                board[2][2]=BigChess_Pos[9];

                getBestMove(board, 1, &bestRow, &bestCol);
                Chesssource_Count=Chess_selmode+1;
                Chess_Pos=bestRow*3+bestCol+1;
                
                Receive_Count+=1;
            }
            if(Receive_Count>=10)
            {
                Receive_Count = 0;
                Mode5_State = 0;
                Button4 = 1;
            }
         }
        break;
    }
    
    
}
void Mode6_Control(void)
{
    static uint8_t Mode5_State = 4;
    static uint8_t Take_Count = 0;
    switch (Mode5_State)
    {
        case 0://移动到任意黑棋 白棋中拿棋子
            if(Chess_selmodebuff==Chess_selmode)
            {
                Step_X = chesssorce_Pos[Chesssource_Count+5][0];
                Step_Y = chesssorce_Pos[Chesssource_Count+5][1];
                if(Step_Time_Receive==0 && Button4==1)
                {
                    Step_Time_Receive = 1;
                }

                if(Step_Time_Receive >= 800)
                {
                    Step_Time_Receive = 0;
                    All_OK_Flag = 1;
                    Button4 = 0;
                }

                if(All_OK_Flag)
                {
                    Step_Time_Receive = 0;
                    if(Step_Time_Z == 0)
                    {
                        Step_Time_Z = 1;
                    }
                    
                    if((Step_Time_Z >= 1) && (Step_Time_Z < 45))
                    {
                        Take_Down;
                    }

                    if(Step_Time_Z >= 75)
                    {
                        Electromagnet_ON;
                        Take_Up;
                    }

                    if(Step_Time_Z >= 110)
                    {
                        Step_Time_Z = 0;
                        Mode5_State = 1;
                       
                        Step_X = Chessboard_Pos[Chess_Pos][0];
                        Step_Y = Chessboard_Pos[Chess_Pos][1];
                        All_OK_Flag = 0;
                        Button4 = 1; 
                        Take_Count+=1;
                    }
                }
            }
        break;
        case 1://放到5号格子
            Step_X = Chessboard_Pos[Chess_Pos][0];
            Step_Y = Chessboard_Pos[Chess_Pos][1];

            if(Step_Time_Receive==0 && Button4==1)
            {
                Step_Time_Receive = 1;
            }

            if(Step_Time_Receive >= 600)
            {
                Step_Time_Receive = 0;
                All_OK_Flag = 1;
                Button4 = 0;
            }

            if(All_OK_Flag)
            {
                if(Step_Time_Z == 0)
                {
                    Step_Time_Z = 1;
                }

                if((Step_Time_Z >= 1) && (Step_Time_Z < 45))
                {
                    Take_Down;
                }

                if(Step_Time_Z >= 35)
                {
                    Electromagnet_OFF;
                }

                if(Step_Time_Z >= 60)
                {
                    Take_Up;
                }

                if(Step_Time_Z >= 120)
                {
                    Step_Time_Z = 0;
                    Mode5_State = 2;
                    LED_ON;
                }
            }
        break;
        case 2://等待状态
            Step_X = 0;
            Step_Y = 0;
            Button4 = 2;//回原点
            All_OK_Flag = 0;
            if(Step_Time_Receive==0)
            {
                Step_Time_Receive = 1;
            }

            if(Step_Time_Receive >= 600)
            {
                Mode5_State = 3;
                //偷棋不算步数，正常一轮加一
                if(Chesssource_Count!=6)
                {Chess_selmode++;}
                Chess_selmodebuff=100;
                Step_Time_Receive=0;
            }
        break;
        case 3://等待状态
            if(Chess_selmodebuff==Chess_selmode)
            {
                // FinishReceiveFlag=true;
                LED_OFF;
                Mode5_State = 4;
            }
        break;
        case 4:
        if(Chess_selmodebuff==Chess_selmode)
        {
            Requst_Data;
            // delay(300);
            // Requst_Data;
            if(FinishReceiveFlag)
            {
                FinishReceiveFlag = false;
                for(uint8_t j = 0;j<=8;j++)
                {
                    // Serial.print(j);
                    // Serial.print("(");
                    // Serial.print(chess[j].x);
                    // Serial.print(",");
                    // Serial.print(chess[j].y);
                    // Serial.println(")");
                    Serial.print(chess[j].state);
                    Serial.println("!");

                    Chessboard_Pos[j+1][0] = chess[j].x;
                    Chessboard_Pos[j+1][1] = chess[j].y;
                    BigChess_Pos[j+1]=chess[j].state;
                    // Serial.print(j+1);
                    // Serial.print("(");
                    // Serial.print(Chessboard_Pos[j+1][0]);
                    // Serial.print(",");
                    // Serial.print(Chessboard_Pos[j+1][1]);
                    // Serial.println(")");
                    Serial.print(BigChess_Pos[j+1]);
                    Serial.println(")");
                }
                
                
                Receive_Count+=1;
            }
            if(Receive_Count>=10)
            {
                Receive_Count = 0;
                Mode5_State = 5;
                //Button4 = 1;
            }
         }
        break;
        case 5://偷棋检查
        if(Chess_selmode==0)//第一步放置不检查
        {
            for(uint8_t j = 0;j<=9;j++)
            {
                BigChess_Posold[j]=BigChess_Pos[j];    
            }
            //第一步没有偷棋，就继续下棋
            uint8_t board[3][3];
            board[0][0]=BigChess_Pos[1];
            board[0][1]=BigChess_Pos[2];
            board[0][2]=BigChess_Pos[3];
            board[1][0]=BigChess_Pos[4];
            board[1][1]=BigChess_Pos[5];
            board[1][2]=BigChess_Pos[6];
            board[2][0]=BigChess_Pos[7];
            board[2][1]=BigChess_Pos[8];
            board[2][2]=BigChess_Pos[9];

            getBestMove(board, 1, &bestRow, &bestCol);
            Chesssource_Count=Chess_selmode+1;
            Chess_Pos=bestRow*3+bestCol+1;
            BigChess_Posold[Chess_Pos]=1;
                Mode5_State = 0;
            Button4 = 1;
        }
        else{
            uint8_t falsenumber=0;
            uint8_t falsenumberpos[4]={0,0,0,0};
            //如果遍历前后两次棋盘，有2处不同，偷棋了
            for(uint8_t j = 0;j<=9;j++)
            {
                if(BigChess_Posold[j]!=BigChess_Pos[j])   
                {
                    falsenumber++;
                    falsenumberpos[falsenumber]=j;
                }
            }
            if(falsenumber>=2)
            {
                //偷棋后先确定偷的是啥棋，然后把坐标放入第11颗棋位置（偷棋位）
                Chesssource_Count=6;
                //根据有没有棋确定两个错误点先后
                if(BigChess_Posold[falsenumberpos[1]]==0)
                {
                    Chess_Pos=falsenumberpos[2];
                    chesssorce_Pos[Chesssource_Count+5][0]=Chessboard_Pos[falsenumberpos[1]][0];
                    chesssorce_Pos[Chesssource_Count+5][1]=Chessboard_Pos[falsenumberpos[1]][1];
                }
                else{
                    Chess_Pos=falsenumberpos[1];
                    chesssorce_Pos[Chesssource_Count+5][0]=Chessboard_Pos[falsenumberpos[2]][0];
                    chesssorce_Pos[Chesssource_Count+5][1]=Chessboard_Pos[falsenumberpos[2]][1];
                }
                Mode5_State = 0;
                Button4 = 1;


            }else
            {//没有偷棋，就继续下棋
                for(uint8_t j = 0;j<=9;j++)
                {
                    BigChess_Posold[j]=BigChess_Pos[j];    
                }
                 uint8_t board[3][3];
                board[0][0]=BigChess_Pos[1];
                board[0][1]=BigChess_Pos[2];
                board[0][2]=BigChess_Pos[3];
                board[1][0]=BigChess_Pos[4];
                board[1][1]=BigChess_Pos[5];
                board[1][2]=BigChess_Pos[6];
                board[2][0]=BigChess_Pos[7];
                board[2][1]=BigChess_Pos[8];
                board[2][2]=BigChess_Pos[9];

                getBestMove(board, 1, &bestRow, &bestCol);
                Chesssource_Count=Chess_selmode+1;
                Chess_Pos=bestRow*3+bestCol+1;
                BigChess_Posold[Chess_Pos]=1;
                 Mode5_State = 0;
                Button4 = 1;
            }


        }
        
        break;
    }
    
    
}
/**@brief     控制任务
-- @param     无
**/
void Control_Task(void)
{
    switch (Menu_Mode)
    {
        case 1:
            Mode1_Control();
        break;
        case 2:
            Mode2_Control();
        break;
        case 3:
            Mode3_Control();
        break;
         case 4:
            Mode4_Control();
        break;
         case 5:
            Mode5_Control();
        break;
         case 6:
            Mode6_Control();
        break;
    }
}


