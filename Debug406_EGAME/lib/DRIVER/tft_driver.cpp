#include <tft_driver.h>
#include <string.h>
#include <Preferences.h>
//������Ļ����
TFT_eSPI tft = TFT_eSPI();
cursor_t cursor;
const char* menuName[] = {"1.Set_angle", "2.Run_Speed", "3.Auto_Cor", "4.WIFI", "5.test5", "6.test6"}; 
uint16_t menuNum = sizeof(menuName) / sizeof(char*);
uint8_t Set_Line = 0;//�ڼ���
uint16_t Chessboard_RD_X;//��������X����
uint16_t Chessboard_RD_Y;//��������Y����
uint16_t Chessboard_LU_X;//��������X����
uint16_t Chessboard_LU_Y;//��������Y����

uint16_t Chess_R_X;//�ұߵ�2������X����
uint16_t Chess_R_Y;//�ұߵ�2������Y����

uint16_t Chess_L_X;//��ߵ�1������X����
uint16_t Chess_L_Y;//��ߵ�1������Y����

int8_t Menu_Mode = 0;
int8_t Menu_Modebuff = 0;
uint8_t Chess_selmode = 0;//����״̬
uint8_t Chess_selmodebuff = 0;//����״̬
uint8_t Chess_Pos = 1;//����Ҫ�ŵ����̵��ĸ�λ�õ�λ��
uint8_t Chesssource_Count = 1;//��ɫ���ӱ��
uint8_t bestRow=0;
uint8_t bestCol=0;



Preferences prefs; // ����Preferences����
void Menu_List(void)
{
    //tft.fillScreen(ST77XX_BLACK);
    for (uint8_t i = 0; i < menuNum; i++)
    {
        tft.drawString(menuName[i], 0, PER_HEIGHT * i);
    }
}

void TFT_init(void)
{
    pinMode(TFT_BACKLIGHT, OUTPUT);
    digitalWrite(TFT_BACKLIGHT, HIGH);
        //��ʾ����ʼ�� 120*160
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    // cursor.ico = (char*)"<=";
    // cursor.currLine = 0; // ���ù���ʼλ��
    // tft.drawString(cursor.ico, strlen(menuName[cursor.currLine]) * 12 , cursor.currLine * 16);
    // Menu_List();
}

/**@brief   ������ʼ��
-- @param   ��
**/
void Parameter_Init(void)
{
    prefs.begin("mynamespace"); // �������ռ�mynamespace
    Chessboard_RD_X = prefs.getUInt("Chessboard_RD_X", 4900);
    prefs.putUInt("Chessboard_RD_X", Chessboard_RD_X);

    Chessboard_RD_Y = prefs.getUInt("Chessboard_RD_Y", 10700);
    prefs.putUInt("Chessboard_RD_Y", Chessboard_RD_Y);

    Chessboard_LU_X = prefs.getUInt("Chessboard_LU_X", 4900);
    prefs.putUInt("Chessboard_LU_X", Chessboard_LU_X);

    Chessboard_LU_Y = prefs.getUInt("Chessboard_LU_Y", 10700);
    prefs.putUInt("Chessboard_LU_Y", Chessboard_LU_Y);

    Chess_R_X = prefs.getUInt("Chess_R_X", 10700);
    prefs.putUInt("Chess_R_X", Chess_R_X);

    Chess_R_Y = prefs.getUInt("Chess_R_Y", 10700);
    prefs.putUInt("Chess_R_Y", Chess_R_Y);

    Chess_L_X = prefs.getUInt("Chess_L_X", 10700);
    prefs.putUInt("Chess_L_X", Chess_L_X);

    Chess_L_Y = prefs.getUInt("Chess_L_Y", 10700);
    prefs.putUInt("Chess_L_Y", Chess_L_Y);
    // prefs.clear(); // ɾ����ǰ�����ռ��е�����Ԫ��
    prefs.end(); // �رյ�ǰ�����ռ�
}

/**@brief   ��ʾ��ͷ
-- @param   int Row ��Ҫ��ʾ���к�
-- @verbatim  ��16 ��13
**/
static void Arrow_Display(int8_t Line)
{
    for(int Line_Num = 0;Line_Num <= 9;Line_Num++)
    {
        if(Line == Line_Num)
        {
            tft.drawString("<",147,Line_Num*16);
        }
        else
        {
            tft.drawString(" ",147,Line_Num*16);
        }
    }
}

/**@brief   ���л�
-- @param   ��
**/
static void Line_Change()
{
    if((Button2==1) && (Set_Line < 7))//˳ʱ��ת
    {
        Button1 = 0;
        Button2 = 0;
        Set_Line++;
    }
    else if((Button1==1) && (Set_Line > 0))//��ʱ��ת
    {
        Button1 = 0;
        Button2 = 0;
        Set_Line--;
    }
}


extern uint16_t Step_X;
extern uint16_t Step_Y;

/**@brief   ��ʾ����
-- @param   ��
**/
void Menu_Task(void)
{
    static uint8_t Detailed = 0;//��ϸ����ģʽ
    static uint8_t Second_Set = 0;


    if(Menu_Mode == 0)
    {
        tft.drawString("Mode0",0,0);
        tft.drawString("sel mode:",0,16);
        tft.drawNumber(Menu_Modebuff,108,16);
        if(Button1==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            Menu_Modebuff++;
            if(Menu_Modebuff>6)
            {Menu_Modebuff=1;}
        }
        else if(Button2==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            Menu_Modebuff--;
            if(Menu_Modebuff<1)
            {Menu_Modebuff=6;}
        }
        else if(Button3==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            Menu_Mode=Menu_Modebuff;
            tft.fillScreen(TFT_BLACK);
            if(Menu_Mode==5||Menu_Mode==6)
            {Chess_selmodebuff=100;}
            
        }
        
        
    }
    if(Menu_Mode == 1)
    {
        tft.drawString("Mode1",0,0);
        tft.drawString("sel bchess:",0,16);
        tft.drawNumber(Chesssource_Count,132,16);
        {tft.drawString("wait but4!",0,32);}
        
         if(Button1==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            Chesssource_Count++;
            if(Chesssource_Count>5)
            {Chesssource_Count=1;}
        }
        else if(Button2==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            Chesssource_Count--;
            if(Chesssource_Count<1)
            {Chesssource_Count=5;}
        }
        if(Button3==2)
        {
            tft.fillScreen(TFT_BLACK);
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            Menu_Mode=0;
            Chesssource_Count=1;
            tft.fillScreen(TFT_BLACK);
        }

        
        
    }

    if(Menu_Mode == 2)
    {
        tft.drawString("Mode2",0,0);
        tft.drawString("chess:",0,16);
        tft.drawNumber(Chesssource_Count,72,16);
        tft.drawString("pos:",0,32);
        tft.drawNumber(Chess_Pos,48,32);
        if(Chess_selmode==0)
        {tft.drawString("-",120,16);}
        else 
        {tft.drawString("-",120,32);}
        {tft.drawString("wait but4",0,48);}
        
         if(Button1==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            if(Chess_selmode==0)
            { Chesssource_Count++;
            if(Chesssource_Count>10)
            {Chesssource_Count=1;}
            }
            else 
            {Chess_Pos++;
            if(Chess_Pos>9)
            {Chess_Pos=1;}}
            tft.fillScreen(TFT_BLACK);
           
        }
        else if(Button2==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            if(Chess_selmode==0)
            { Chesssource_Count--;
            if(Chesssource_Count<1)
            {Chesssource_Count=10;}
            }
            else 
            {Chess_Pos--;
            if(Chess_Pos<1)
            {Chess_Pos=9;}}
            tft.fillScreen(TFT_BLACK);
        }
        else if(Button3==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            tft.fillScreen(TFT_BLACK);
            Chess_selmode++;
            if(Chess_selmode>1)
            {Chess_selmode=0;}
            tft.fillScreen(TFT_BLACK);
        }
        else if(Button3==2)
        {
            
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            Menu_Mode=0;
            Chess_selmode=0;
            Chess_Pos=1;
            Chesssource_Count=1;
            tft.fillScreen(TFT_BLACK);
        }

        
        
    }
    if(Menu_Mode == 3)
    {
        tft.drawString("Mode3",0,0);
        tft.drawString("chess:",0,16);
        tft.drawNumber(Chesssource_Count,72,16);
        tft.drawString("pos:",0,32);
        tft.drawNumber(Chess_Pos,48,32);
        if(Chess_selmode==0)
        {tft.drawString("-",120,16);}
        else 
        {tft.drawString("-",120,32);}
        {tft.drawString("wait but4",0,48);}
        
         if(Button1==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            if(Chess_selmode==0)
            { Chesssource_Count++;
            if(Chesssource_Count>10)
            {Chesssource_Count=1;}
            }
            else 
            {Chess_Pos++;
            if(Chess_Pos>9)
            {Chess_Pos=1;}}
            tft.fillScreen(TFT_BLACK);
           
        }
        else if(Button2==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            if(Chess_selmode==0)
            { Chesssource_Count--;
            if(Chesssource_Count<1)
            {Chesssource_Count=10;}
            }
            else 
            {Chess_Pos--;
            if(Chess_Pos<1)
            {Chess_Pos=9;}}
            tft.fillScreen(TFT_BLACK);
        }
        else if(Button3==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            tft.fillScreen(TFT_BLACK);
            Chess_selmode++;
            if(Chess_selmode>1)
            {Chess_selmode=0;}
            tft.fillScreen(TFT_BLACK);
        }
        else if(Button3==2)
        {
            
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            Menu_Mode=0;
            Chess_selmode=0;
            Chess_Pos=1;
            Chesssource_Count=1;
            tft.fillScreen(TFT_BLACK);
        }

        
        
    }
     if(Menu_Mode == 4)
    {
        tft.drawString("M4",0,0);
        tft.drawString("pos:",0,16);
        tft.drawNumber(Chess_Pos,48,16);
        tft.drawString("step:",48,0);
        tft.drawNumber(Chess_selmode,96,0);
        tft.drawString("best:",0,32);
        tft.drawNumber(bestRow,80,32);
        tft.drawNumber(bestCol,112,32);
        tft.drawNumber(BigChess_Pos[1],0,48);
        tft.drawNumber(BigChess_Pos[2],24,48);
        tft.drawNumber(BigChess_Pos[3],48,48);
        tft.drawNumber(BigChess_Pos[4],0,64);
        tft.drawNumber(BigChess_Pos[5],24,64);
        tft.drawNumber(BigChess_Pos[6],48,64);
        tft.drawNumber(BigChess_Pos[7],0,80);
        tft.drawNumber(BigChess_Pos[8],24,80);
        tft.drawNumber(BigChess_Pos[9],48,80);
        
        
         if(Button1==1&&Chess_selmode==0)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            Chess_Pos++;
            if(Chess_Pos>9)
            {Chess_Pos=1;}
            tft.fillScreen(TFT_BLACK);
           
        }
        else if(Button2==1&&Chess_selmode==0)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
           Chess_Pos--;
            if(Chess_Pos<1)
            {Chess_Pos=9;}
            tft.fillScreen(TFT_BLACK);
        }
        else if(Button3==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            
            tft.fillScreen(TFT_BLACK);
            Chess_selmodebuff=Chess_selmode;
            
            tft.fillScreen(TFT_BLACK);
        }
        else if(Button3==2)
        {
            
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            Menu_Mode=0;
            Chess_selmode=0;
            Chess_Pos=1;
            Chesssource_Count=1;
            tft.fillScreen(TFT_BLACK);
        }

        
        
    }
if(Menu_Mode == 5)
    {
        tft.drawString("M5",0,0);
        tft.drawString("step:",48,0);
        tft.drawNumber(Chess_selmode,96,0);
        tft.drawString("best:",0,32);
        tft.drawNumber(bestRow,80,32);
        tft.drawNumber(bestCol,112,32);
        tft.drawNumber(BigChess_Pos[1],0,48);
        tft.drawNumber(BigChess_Pos[2],24,48);
        tft.drawNumber(BigChess_Pos[3],48,48);
        tft.drawNumber(BigChess_Pos[4],0,64);
        tft.drawNumber(BigChess_Pos[5],24,64);
        tft.drawNumber(BigChess_Pos[6],48,64);
        tft.drawNumber(BigChess_Pos[7],0,80);
        tft.drawNumber(BigChess_Pos[8],24,80);
        tft.drawNumber(BigChess_Pos[9],48,80);
        
        
        
        if(Button3==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            tft.fillScreen(TFT_BLACK);
            Chess_selmodebuff=Chess_selmode;
            
            tft.fillScreen(TFT_BLACK);
        }
        else if(Button3==2)
        {
            
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            Menu_Mode=0;
            Chess_selmode=0;
            Chess_Pos=1;
            Chesssource_Count=1;
            tft.fillScreen(TFT_BLACK);
        }

        
        
    }
    if(Menu_Mode == 6)
    {
        tft.drawString("M6",0,0);
        tft.drawString("step:",48,0);
        tft.drawNumber(Chess_selmode,96,0);
        tft.drawString("best:",0,32);
        tft.drawNumber(bestRow,80,32);
        tft.drawNumber(bestCol,112,32);
        tft.drawNumber(BigChess_Pos[1],0,48);
        tft.drawNumber(BigChess_Pos[2],24,48);
        tft.drawNumber(BigChess_Pos[3],48,48);
        tft.drawNumber(BigChess_Pos[4],0,64);
        tft.drawNumber(BigChess_Pos[5],24,64);
        tft.drawNumber(BigChess_Pos[6],48,64);
        tft.drawNumber(BigChess_Pos[7],0,80);
        tft.drawNumber(BigChess_Pos[8],24,80);
        tft.drawNumber(BigChess_Pos[9],48,80);
        
        
        
        if(Button3==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            tft.fillScreen(TFT_BLACK);
            Chess_selmodebuff=Chess_selmode;
            
            tft.fillScreen(TFT_BLACK);
        }
        else if(Button3==2)
        {
            
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            Menu_Mode=0;
            Chess_selmode=0;
            Chess_Pos=1;
            Chesssource_Count=1;
            tft.fillScreen(TFT_BLACK);
        }

        
        
    }

}