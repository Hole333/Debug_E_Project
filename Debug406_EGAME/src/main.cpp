#include <Stepmotor.h>
#include <tft_driver.h>
#include <key_driver.h>
#include <serial.h>
#include <task.h>
#include <pid.h>
#include <wifi_driver.h>

//��������
#define Electromagnet 14//�̵�������
#define Electromagnet_ON digitalWrite(Electromagnet, HIGH)
#define Electromagnet_OFF digitalWrite(Electromagnet, LOW)
#define X_Stepmotor 1
#define Y_Stepmotor 2
#define Z_Stepmotor 3
#define Take_Up Emm_V5_Pos_Control(Z_Stepmotor,CW,200,0,0,true,false) //Z��̧��
#define Take_Down Emm_V5_Pos_Control(Z_Stepmotor,CW,200,0,800,true,false) //Z���½�

uint16_t Step_Time = 0;//���㷢������ʱ��,�����ַ1��X�ᣬ2��Y�ᣬ3��Z��
uint16_t Step_Time_Z = 0;//���㷢������ʱ��,�����ַ1��X�ᣬ2��Y�ᣬ3��Z��
uint16_t Step_Time_Receive = 0;//���㷢������ʱ��,�����ַ1��X�ᣬ2��Y�ᣬ3��Z��
uint16_t Step_X = 0;
uint16_t Step_Y = 0;
uint16_t Chessboard_Pos[10][2]={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},};//����0��ʾX���꣬1��ʾY����
uint16_t Chessboard_Pos_Image[10][2]={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},};//����0��ʾX���꣬1��ʾY����
uint16_t Black_Pos[6][2]={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};//����0��ʾX���꣬1��ʾY����
uint16_t White_Pos[6][2]={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};//����0��ʾX���꣬1��ʾY����
uint8_t Uart_Busy = 1;//�Ƿ���ʹ�ò����������
uint8_t All_OK_Flag = 0;//X��Y����λ
//x 1mm��Ӧ 79������ y 1mm��Ӧ 75������
//��������
void StepMotor_Control(int16_t X_Step, int16_t Y_Step,uint16_t Speed);
void Menu_Task(void);
void Control_Task(void);

//��ʼ��
void setup()
{
    // put your setup code here, to run once:
    delay(1000);
    TFT_init();       // ��Ļ��ʼ��
    Parameter_Init(); //������ʼ��
//����λ�ý���
    // Chessboard_Pos[1][0] = Chessboard_LU_X;
    // Chessboard_Pos[1][1] = Chessboard_LU_Y;

    // Chessboard_Pos[2][0] = Chessboard_LU_X - ((Chessboard_LU_X - Chessboard_RD_X)/2.0f);
    // Chessboard_Pos[2][1] = Chessboard_LU_Y;

    // Chessboard_Pos[3][0] = Chessboard_LU_X - (((Chessboard_LU_X - Chessboard_RD_X)/2.0f)*2);
    // Chessboard_Pos[3][1] = Chessboard_LU_Y;

    // Chessboard_Pos[4][0] = Chessboard_LU_X;
    // Chessboard_Pos[4][1] = Chessboard_LU_Y - ((Chessboard_LU_Y - Chessboard_RD_Y)/2.0f);

    // Chessboard_Pos[5][0] = Chessboard_LU_X - ((Chessboard_LU_X - Chessboard_RD_X)/2.0f);
    // Chessboard_Pos[5][1] = Chessboard_LU_Y - ((Chessboard_LU_Y - Chessboard_RD_Y)/2.0f);

    // Chessboard_Pos[6][0] = Chessboard_LU_X - (((Chessboard_LU_X - Chessboard_RD_X)/2.0f)*2);
    // Chessboard_Pos[6][1] = Chessboard_LU_Y - ((Chessboard_LU_Y - Chessboard_RD_Y)/2.0f);

    // Chessboard_Pos[7][0] = Chessboard_LU_X;
    // Chessboard_Pos[7][1] = Chessboard_LU_Y - (((Chessboard_LU_Y - Chessboard_RD_Y)/2.0f)*2);

    // Chessboard_Pos[8][0] = Chessboard_LU_X - ((Chessboard_LU_X - Chessboard_RD_X)/2.0f);
    // Chessboard_Pos[8][1] = Chessboard_LU_Y - (((Chessboard_LU_Y - Chessboard_RD_Y)/2.0f)*2);

    // Chessboard_Pos[9][0] = Chessboard_LU_X - (((Chessboard_LU_X - Chessboard_RD_X)/2.0f)*2);
    // Chessboard_Pos[9][1] = Chessboard_LU_Y - (((Chessboard_LU_Y - Chessboard_RD_Y)/2.0f)*2);
    Chessboard_Pos[1][0] = 10428;
    Chessboard_Pos[1][1] = 9900;

    Chessboard_Pos[2][0] = 7500;
    Chessboard_Pos[2][1] = 9900;

    Chessboard_Pos[3][0] = 5372;
    Chessboard_Pos[3][1] = 9900;

    Chessboard_Pos[4][0] = 10428;
    Chessboard_Pos[4][1] = 7500;

    Chessboard_Pos[5][0] = 7900;
    Chessboard_Pos[5][1] = 7500;

    Chessboard_Pos[6][0] = 5372;
    Chessboard_Pos[6][1] = 7500;

    Chessboard_Pos[7][0] = 10428;
    Chessboard_Pos[7][1] = 5100;

    Chessboard_Pos[8][0] = 7900;
    Chessboard_Pos[8][1] = 5100;

    Chessboard_Pos[9][0] = 5372;
    Chessboard_Pos[9][1] = 5100;
//����λ�ý���
    Black_Pos[1][0] = 14615;
    Black_Pos[1][1] = 3950;

    Black_Pos[2][0] = 14615;
    Black_Pos[2][1] = 5925;

    Black_Pos[3][0] = 14615;
    Black_Pos[3][1] = 7900;

    Black_Pos[4][0] = 14615;
    Black_Pos[4][1] = 9875;

    Black_Pos[5][0] = 14615;
    Black_Pos[5][1] = 11850;

    White_Pos[1][0] = 1185;
    White_Pos[1][1] = 3950;

    White_Pos[2][0] = 1185;
    White_Pos[2][1] = 5925;

    White_Pos[3][0] = 1185;
    White_Pos[3][1] = 7900;

    White_Pos[4][0] = 1185;
    White_Pos[4][1] = 9875;

    White_Pos[5][0] = 1185;
    White_Pos[5][1] = 11850;
    // Step_X = Chessboard_Pos[5][0];
    // Step_Y = Chessboard_Pos[5][1];
    KEY_init();       // ������ʼ��
    Serial_Init();    // ���ڳ�ʼ��
    for(uint8_t j = 1;j<=9;j++)
    {
        Serial.print("(");
        Serial.print(Chessboard_Pos[j][0]);
        Serial.print(",");
        Serial.print(Chessboard_Pos[j][1]);
        Serial.println(")");
    }
    TASK_Init();      // �����ʼ��
    pinMode(Electromagnet, OUTPUT);//�̵�����ʼ��
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
    // Serial.print("X_Step:");
    // Serial.println(Step_X);
    // Serial.print("ok:");
    // Serial.println(All_OK_Flag);
    if(Button4==1)
    {
        StepMotor_Control(Step_X,Step_Y,500);
    }
    else if(Button4==2)
    {
        StepMotor_Control(0,0,500); 
    }
}


/**@brief     ����X��Y��ĵ���˶�
-- @param     X_Step X���ߵľ��ԽǶ�
-- @param     Y_Step Y���ߵľ��ԽǶ�
-- @param     Speed �ٶ�
-- @date      2024/7/30
**/
void StepMotor_Control(int16_t X_Step, int16_t Y_Step,uint16_t Speed)
{
    static uint8_t Step_Mode = 0;
    switch (Step_Mode)
    {
        case 0://����X������
            if(Step_Time==0)
            {
                if(X_Step >= 0)
                {
                    Emm_V5_Pos_Control(X_Stepmotor,CW,Speed,0,X_Step,true,true);//���ԽǶȣ����ͬ��
                    Uart_Busy = 1;
                }
                else
                {
                    Emm_V5_Pos_Control(X_Stepmotor,CCW,Speed,0,X_Step,true,true);
                    Uart_Busy = 1;
                }
                Step_Time = 1;
            }

            // if(Step_Time==5)
            // {
            //     if(X_Step >= 0)
            //     {
            //         Emm_V5_Pos_Control(X_Stepmotor,CW,Speed,0,X_Step,true,true);//���ԽǶȣ����ͬ��
            //         Uart_Busy = 1;
            //     }
            //     else
            //     {
            //         Emm_V5_Pos_Control(X_Stepmotor,CCW,Speed,0,X_Step,true,true);
            //         Uart_Busy = 1;
            //     }
            //     Step_Time = 1;
            // }

            if(Step_Time>=5)
            {
                Step_Mode = 1;
                Step_Time = 0;
            }
        break;
        case 1://����Y������
            if(Step_Time==0)
            {
                if(Y_Step > 0)
                {
                    Emm_V5_Pos_Control(Y_Stepmotor,CW,Speed,0,Y_Step,true,true);//���ԽǶȣ����ͬ��
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

/**@brief     X��Y�Ƿ�λ
-- @param     ��
**/
void Step_All_OK(void)
{
    static uint8_t Step_Mode = 0;
    static uint8_t X_Step_OK = 0;
    static uint8_t Y_Step_OK = 0;
    
    switch (Step_Mode)
    {
        case 0://����X������
            if(Uart_Busy==0)
            {
                if(Step_Time==0)
                {
                    Step_Time = 1;
                }

                if(Step_Time>=5)
                {
                    Emm_V5_Read_Sys_Params(X_Stepmotor, S_FLAG);

                    // �ȴ���������������ݻ���������rxCmd�ϣ�����ΪrxCount
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
        case 1://����Y������
            if(Uart_Busy==0)
            {
                if(Step_Time==0)
                {
                    Step_Time = 1;
                }

                if(Step_Time>=5)
                {
                    Emm_V5_Read_Sys_Params(Y_Stepmotor, S_FLAG);

                    // �ȴ���������������ݻ���������rxCmd�ϣ�����ΪrxCount
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

/**@brief     Ҫ��һ��������
-- @param     ��
**/
void Mode1_Control()
{
    static uint8_t Mode1_State = 0;
    switch (Mode1_State)
    {
        case 0://�ƶ������������ú���
            if(Black_Chess_Count!=0)
            {
                Step_X = Black_Pos[Black_Chess_Count][0];
                Step_Y = Black_Pos[Black_Chess_Count][1];
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
                        Take_Down;
                        Electromagnet_ON;
                    }

                    if(Step_Time_Z >= 45)
                    {
                        Take_Up;
                    }

                    if(Step_Time_Z >= 90)
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
        case 1://�ŵ�5�Ÿ���
            Step_X = Chessboard_Pos[5][0];
            Step_Y = Chessboard_Pos[5][1];

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
                    Take_Down;
                }

                if(Step_Time_Z >= 90)
                {
                    Electromagnet_OFF;
                    Take_Up;
                }

                if(Step_Time_Z >= 180)
                {
                    Step_Time_Z = 0;
                    Mode1_State = 2;
                }
            }
        break;
        case 2://��״̬
            Step_X = 0;
            Step_Y = 0;
            Button4 = 2;//��ԭ��
            Black_Chess_Count = 0;
            White_Chess_Count = 0;
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
        case 0://�ƶ���������� ������������
            if(Black_Chess_Count!=0)
            {
                Step_X = Black_Pos[Black_Chess_Count][0];
                Step_Y = Black_Pos[Black_Chess_Count][1];
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
                        Take_Down;
                        Electromagnet_ON;
                    }

                    if(Step_Time_Z >= 45)
                    {
                        Take_Up;
                    }

                    if(Step_Time_Z >= 90)
                    {
                        Step_Time_Z = 0;
                        Mode2_State = 1;
                        Black_Chess_Count = 0;
                        White_Chess_Count = 0;
                        Step_X = Chessboard_Pos[Chess_Pos][0];
                        Step_Y = Chessboard_Pos[Chess_Pos][1];
                        All_OK_Flag = 0;
                        Button4 = 1; 
                        Take_Count+=1;
                    }
                }
            }

            if(White_Chess_Count!=0)
            {
                Step_X = White_Pos[White_Chess_Count][0];
                Step_Y = White_Pos[White_Chess_Count][1];
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
                        Take_Down;
                        Electromagnet_ON;
                    }

                    if(Step_Time_Z >= 90)
                    {
                        Take_Up;
                    }

                    if(Step_Time_Z >= 180)
                    {
                        Step_Time_Z = 0;
                        Mode2_State = 1;
                        Black_Chess_Count = 0;
                        White_Chess_Count = 0;                        
                        Step_X = Chessboard_Pos[Chess_Pos][0];
                        Step_Y = Chessboard_Pos[Chess_Pos][1];
                        All_OK_Flag = 0;
                        Button4 = 1; 
                        Take_Count+=1;
                    }
                }
            }
        break;
        case 1://�ŵ�5�Ÿ���
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
                    Take_Down;
                }

                if(Step_Time_Z >= 90)
                {
                    Electromagnet_OFF;
                    Take_Up;
                }

                if(Step_Time_Z >= 180)
                {
                    Step_Time_Z = 0;
                    Mode2_State = 2;
                    Chess_Pos = 0;
                }
            }
        break;
        case 2://�ȴ�״̬
            Step_X = 0;
            Step_Y = 0;
            Button4 = 2;//��ԭ��
            All_OK_Flag = 0;
            if(Take_Count>4)//�ĸ���������
            {
                Mode2_State = 3;
            }
            else
            {
                if(((White_Chess_Count!=0 )||( Black_Chess_Count!=0)) && (Chess_Pos!=0))
                {
                    Mode2_State = 0;
                    Button4 = 0;
                }
            }
        break;
        case 3:
            Step_X = 0;
            Step_Y = 0;
            Button4 = 2;//��ԭ��
            Black_Chess_Count = 0;
            White_Chess_Count = 0;
            All_OK_Flag = 0;
        break;
    }
}

void Mode3_Control(void)
{
    static uint8_t Mode3_State = 0;
    static uint8_t Take_Count = 0;
    switch(Mode3_State)
    {
        case 0:
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

                    Chessboard_Pos_Image[j+1][0] = chess[j].x*79;
                    Chessboard_Pos_Image[j+1][1] = chess[j].y*75;
                    Serial.print(j+1);
                    Serial.print("(");
                    Serial.print(Chessboard_Pos_Image[j+1][0]);
                    Serial.print(",");
                    Serial.print(Chessboard_Pos_Image[j+1][1]);
                    Serial.println(")");
                }
                Mode3_State = 1;
            }
        break;
        case 1://�ƶ���������� ������������
            if(Black_Chess_Count!=0)
            {
                Step_X = Black_Pos[Black_Chess_Count][0];
                Step_Y = Black_Pos[Black_Chess_Count][1];
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
                        Take_Down;
                        Electromagnet_ON;
                    }

                    if(Step_Time_Z >= 45)
                    {
                        Take_Up;
                    }

                    if(Step_Time_Z >= 90)
                    {
                        Step_Time_Z = 0;
                        Mode3_State = 2;
                        Black_Chess_Count = 0;
                        White_Chess_Count = 0;
                        Step_X = Chessboard_Pos[Chess_Pos][0];
                        Step_Y = Chessboard_Pos[Chess_Pos][1];
                        All_OK_Flag = 0;
                        Button4 = 1; 
                        Take_Count+=1;
                    }
                }
            }

            if(White_Chess_Count!=0)
            {
                Step_X = White_Pos[White_Chess_Count][0];
                Step_Y = White_Pos[White_Chess_Count][1];
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
                        Take_Down;
                        Electromagnet_ON;
                    }

                    if(Step_Time_Z >= 90)
                    {
                        Take_Up;
                    }

                    if(Step_Time_Z >= 180)
                    {
                        Step_Time_Z = 0;
                        Mode3_State = 2;
                        Black_Chess_Count = 0;
                        White_Chess_Count = 0;                        
                        Step_X = Chessboard_Pos[Chess_Pos][0];
                        Step_Y = Chessboard_Pos[Chess_Pos][1];
                        All_OK_Flag = 0;
                        Button4 = 1; 
                        Take_Count+=1;
                    }
                }
            }
        break;
        case 2://�ŵ�5�Ÿ���
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
                    Take_Down;
                }

                if(Step_Time_Z >= 90)
                {
                    Electromagnet_OFF;
                    Take_Up;
                }

                if(Step_Time_Z >= 180)
                {
                    Step_Time_Z = 0;
                    Mode3_State = 3;
                    Chess_Pos = 0;
                }
            }
        break;
        case 3://�ȴ�״̬
            Step_X = 0;
            Step_Y = 0;
            Button4 = 2;//��ԭ��
            All_OK_Flag = 0;
            if(Take_Count>4)//�ĸ���������
            {
                Mode3_State = 4;
            }
            else
            {
                if(((White_Chess_Count!=0 )||( Black_Chess_Count!=0)) && (Chess_Pos!=0))
                {
                    Mode3_State = 1;
                    Button4 = 0;
                }
            }
        break;
        case 4:
            Step_X = 0;
            Step_Y = 0;
            Button4 = 2;//��ԭ��
            Black_Chess_Count = 0;
            White_Chess_Count = 0;
            All_OK_Flag = 0;
        break;
    }

}

/**@brief     ��������
-- @param     ��
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
    }
}


