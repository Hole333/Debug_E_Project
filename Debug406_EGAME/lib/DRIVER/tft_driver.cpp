#include <tft_driver.h>
#include <string.h>
#include <Preferences.h>
#include <Chinese.h>

//创建屏幕对象
TFT_eSPI tft = TFT_eSPI();
cursor_t cursor;
const char* menuName[] = {"1.Set_angle", "2.Run_Speed", "3.Auto_Cor", "4.WIFI", "5.test5", "6.test6"}; 
uint16_t menuNum = sizeof(menuName) / sizeof(char*);
uint8_t Set_Line = 0;//第几行
uint16_t Chessboard_RD_X;//棋盘右下X坐标
uint16_t Chessboard_RD_Y;//棋盘右下Y坐标
uint16_t Chessboard_LU_X;//棋盘左上X坐标
uint16_t Chessboard_LU_Y;//棋盘左上Y坐标

uint16_t Chess_R_X;//右边第2个棋子X坐标
uint16_t Chess_R_Y;//右边第2个棋子Y坐标

uint16_t Chess_L_X;//左边第1个棋子X坐标
uint16_t Chess_L_Y;//左边第1个棋子Y坐标

int8_t Menu_Mode = 0;
int8_t Menu_Modebuff = 0;
uint8_t Chess_selmode = 0;//棋子状态
uint8_t Chess_selmodebuff = 0;//棋子状态
uint8_t Chess_Pos = 1;//棋子要放到棋盘的哪个位置的位置
uint8_t Chesssource_Count = 1;//黑色棋子编号
uint8_t bestRow=0;
uint8_t bestCol=0;



Preferences prefs; // 声明Preferences对象
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
        //显示屏初始化 120*160
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    // cursor.ico = (char*)"<=";
    // cursor.currLine = 0; // 设置光标初始位置
    // tft.drawString(cursor.ico, strlen(menuName[cursor.currLine]) * 12 , cursor.currLine * 16);
    // Menu_List();
}

/**@brief   参数初始化
-- @param   无
**/
void Parameter_Init(void)
{
    prefs.begin("mynamespace"); // 打开命名空间mynamespace
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
    // prefs.clear(); // 删除当前命名空间中的所有元素
    prefs.end(); // 关闭当前命名空间
}

/**@brief   显示箭头
-- @param   int Row 需要显示的行号
-- @verbatim  高16 宽13
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

/**@brief   行切换
-- @param   无
**/
static void Line_Change()
{
    if((Button2==1) && (Set_Line < 7))//顺时针转
    {
        Button1 = 0;
        Button2 = 0;
        Set_Line++;
    }
    else if((Button1==1) && (Set_Line > 0))//逆时针转
    {
        Button1 = 0;
        Button2 = 0;
        Set_Line--;
    }
}


extern uint16_t Step_X;
extern uint16_t Step_Y;

/**@brief   显示任务
-- @param   无
**/
void Menu_Task(void)
{
    static uint8_t Detailed = 0;//详细设置模式
    static uint8_t Second_Set = 0;


    if(Menu_Mode == 0)
    {
        draw_Chinese(0, 0, "模", TFT_WHITE);
        draw_Chinese(16, 0, "式", TFT_WHITE);
        tft.drawNumber(0,32,0);
        draw_Chinese(0, 16, "选", TFT_WHITE);
        draw_Chinese(16, 16, "择", TFT_WHITE);
        tft.drawString(":",32,16);
        tft.drawNumber(Menu_Modebuff,48,16);
        if(Button1==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            Menu_Modebuff++;
            if(Menu_Modebuff>8)
            {Menu_Modebuff=1;}
        }
        else if(Button2==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            Menu_Modebuff--;
            if(Menu_Modebuff<1)
            {Menu_Modebuff=8;}
        }
        else if(Button3==1)
        {
            Button1 = 0;
            Button2 = 0;
            Button3 = 0;
            Menu_Mode=Menu_Modebuff;
            tft.fillScreen(TFT_BLACK);
            if(Menu_Mode>=5)
            {Chess_selmodebuff=100;}
            
        }
        
        
    }
    if(Menu_Mode == 1)
    {
        draw_Chinese(0, 0, "模", TFT_WHITE);
        draw_Chinese(16, 0, "式", TFT_WHITE);
        tft.drawNumber(1,32,0);
        draw_Chinese(0, 16, "棋", TFT_WHITE);
        draw_Chinese(16, 16, "子", TFT_WHITE);
        tft.drawString(":",32,16);
        tft.drawNumber(Chesssource_Count,48,16);

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
        draw_Chinese(0, 0, "模", TFT_WHITE);
        draw_Chinese(16, 0, "式", TFT_WHITE);
        tft.drawNumber(2,32,0);
        draw_Chinese(0, 16, "棋", TFT_WHITE);
        draw_Chinese(16, 16, "子", TFT_WHITE);
        tft.drawString(":",32,16);
        tft.drawNumber(Chesssource_Count,48,16);
        draw_Chinese(0, 32, "位", TFT_WHITE);                     
        draw_Chinese(16, 32, "置", TFT_WHITE); 
        tft.drawString(":",32,32);  
        tft.drawNumber(Chess_Pos,48,32);

        if(Chess_selmode==0)
        {
            tft.drawString("<",120,16);
        }
        else 
        {
            tft.drawString("<",120,32);
        }
        
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
        draw_Chinese(0, 0, "模", TFT_WHITE);
        draw_Chinese(16, 0, "式", TFT_WHITE);
        tft.drawNumber(3,32,0);
        draw_Chinese(0, 16, "棋", TFT_WHITE);
        draw_Chinese(16, 16, "子", TFT_WHITE);
        tft.drawString(":",32,16);
        tft.drawNumber(Chesssource_Count,48,16);
        draw_Chinese(0, 32, "位", TFT_WHITE);                     
        draw_Chinese(16, 32, "置", TFT_WHITE); 
        tft.drawString(":",32,32);  
        tft.drawNumber(Chess_Pos,48,32);
        if(Chess_selmode==0)
        {
            tft.drawString("<",120,16);
        }
        else 
        {
            tft.drawString("<",120,32);
        }
        // tft.drawString("Mode3",0,0);
        // tft.drawString("chess:",0,16);
        // tft.drawNumber(Chesssource_Count,72,16);
        // tft.drawString("pos:",0,32);
        // tft.drawNumber(Chess_Pos,48,32);
        // if(Chess_selmode==0)
        // {tft.drawString("-",120,16);}
        // else 
        // {tft.drawString("-",120,32);}
        // {tft.drawString("wait but4",0,48);}
        
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
        draw_Chinese(0, 0, "模", TFT_WHITE);
        draw_Chinese(16, 0, "式", TFT_WHITE);
        tft.drawNumber(4,32,0);
        draw_Chinese(0, 16, "位", TFT_WHITE);
        draw_Chinese(16, 16, "置", TFT_WHITE);
        tft.drawString(":",32,16);
        tft.drawNumber(Chess_Pos,48,16);
        draw_Chinese(0, 32, "步", TFT_WHITE);
        draw_Chinese(16, 32, "骤", TFT_WHITE);
        tft.drawString(":",32,32);
        tft.drawNumber(Chess_selmode,48,32);

        draw_Chinese(64, 32, "最", TFT_WHITE);
        draw_Chinese(80, 32, "佳", TFT_WHITE);
        tft.drawString(":",96,32);
        tft.drawNumber(bestRow, 112,32);
        tft.drawNumber(bestCol,128,32);

        tft.drawLine(0,60,66,60,TFT_WHITE);//一横线
        tft.drawLine(0,81,66,81,TFT_WHITE);//二横线
        tft.drawLine(0,102,66,102,TFT_WHITE);//三横线
        tft.drawLine(0,123,66,123,TFT_WHITE);//四横线
        tft.drawLine(0,60,0,123,TFT_WHITE);//左一横线
        tft.drawLine(21,60,21,123,TFT_WHITE);//左二横线
        tft.drawLine(44,60,44,123,TFT_WHITE);//左三横线
        tft.drawLine(66,60,66,123,TFT_WHITE);//左四横线
        tft.drawNumber(BigChess_Pos[1],5,64);
        tft.drawNumber(BigChess_Pos[2],29,64);
        tft.drawNumber(BigChess_Pos[3],53,64);
        tft.drawNumber(BigChess_Pos[4],5,85);
        tft.drawNumber(BigChess_Pos[5],29,85);
        tft.drawNumber(BigChess_Pos[6],53,85);
        tft.drawNumber(BigChess_Pos[7],5,106);
        tft.drawNumber(BigChess_Pos[8],29,106);
        tft.drawNumber(BigChess_Pos[9],53,106);
        draw_Chinese(80, 80, "棋", TFT_WHITE);
        draw_Chinese(96, 80, "盘", TFT_WHITE);
        
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
        draw_Chinese(0, 0, "模", TFT_WHITE);
        draw_Chinese(16, 0, "式", TFT_WHITE);
        tft.drawNumber(5,32,0);
        // draw_Chinese(0, 16, "棋", TFT_WHITE);
        // draw_Chinese(16, 16, "子", TFT_WHITE);
        // tft.drawString(":",32,16);
        // tft.drawNumber(Chesssource_Count,48,16);
        draw_Chinese(0, 32, "步", TFT_WHITE);
        draw_Chinese(16, 32, "骤", TFT_WHITE);
        tft.drawString(":",32,32);
        tft.drawNumber(Chess_selmode,48,32);

        draw_Chinese(64, 32, "最", TFT_WHITE);
        draw_Chinese(80, 32, "佳", TFT_WHITE);
        tft.drawString(":",96,32);
        tft.drawNumber(bestRow, 112,32);
        tft.drawNumber(bestCol,128,32);

        tft.drawLine(0,60,66,60,TFT_WHITE);//一横线
        tft.drawLine(0,81,66,81,TFT_WHITE);//二横线
        tft.drawLine(0,102,66,102,TFT_WHITE);//三横线
        tft.drawLine(0,123,66,123,TFT_WHITE);//四横线
        tft.drawLine(0,60,0,123,TFT_WHITE);//左一横线
        tft.drawLine(21,60,21,123,TFT_WHITE);//左二横线
        tft.drawLine(44,60,44,123,TFT_WHITE);//左三横线
        tft.drawLine(66,60,66,123,TFT_WHITE);//左四横线
        tft.drawNumber(BigChess_Pos[1],5,64);
        tft.drawNumber(BigChess_Pos[2],29,64);
        tft.drawNumber(BigChess_Pos[3],53,64);
        tft.drawNumber(BigChess_Pos[4],5,85);
        tft.drawNumber(BigChess_Pos[5],29,85);
        tft.drawNumber(BigChess_Pos[6],53,85);
        tft.drawNumber(BigChess_Pos[7],5,106);
        tft.drawNumber(BigChess_Pos[8],29,106);
        tft.drawNumber(BigChess_Pos[9],53,106);
        draw_Chinese(80, 80, "棋", TFT_WHITE);
        draw_Chinese(96, 80, "盘", TFT_WHITE);
        
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
        draw_Chinese(0, 0, "模", TFT_WHITE);
        draw_Chinese(16, 0, "式", TFT_WHITE);
        tft.drawNumber(6,32,0);

        draw_Chinese(0, 32, "步", TFT_WHITE);
        draw_Chinese(16, 32, "骤", TFT_WHITE);
        tft.drawString(":",32,32);
        tft.drawNumber(Chess_selmode,48,32);

        draw_Chinese(64, 32, "最", TFT_WHITE);
        draw_Chinese(80, 32, "佳", TFT_WHITE);
        tft.drawString(":",96,32);
        tft.drawNumber(bestRow, 112,32);
        tft.drawNumber(bestCol,128,32);

        draw_Chinese(80, 80, "棋", TFT_WHITE);
        draw_Chinese(96, 80, "盘", TFT_WHITE);
        tft.drawLine(0,60,66,60,TFT_WHITE);//一横线
        tft.drawLine(0,81,66,81,TFT_WHITE);//二横线
        tft.drawLine(0,102,66,102,TFT_WHITE);//三横线
        tft.drawLine(0,123,66,123,TFT_WHITE);//四横线
        tft.drawLine(0,60,0,123,TFT_WHITE);//左一横线
        tft.drawLine(21,60,21,123,TFT_WHITE);//左二横线
        tft.drawLine(44,60,44,123,TFT_WHITE);//左三横线
        tft.drawLine(66,60,66,123,TFT_WHITE);//左四横线
        tft.drawNumber(BigChess_Pos[1],5,64);
        tft.drawNumber(BigChess_Pos[2],29,64);
        tft.drawNumber(BigChess_Pos[3],53,64);
        tft.drawNumber(BigChess_Pos[4],5,85);
        tft.drawNumber(BigChess_Pos[5],29,85);
        tft.drawNumber(BigChess_Pos[6],53,85);
        tft.drawNumber(BigChess_Pos[7],5,106);
        tft.drawNumber(BigChess_Pos[8],29,106);
        tft.drawNumber(BigChess_Pos[9],53,106);

        // tft.drawString("M6",0,0);
        // tft.drawString("step:",48,0);
        // tft.drawNumber(Chess_selmode,96,0);
        // tft.drawString("best:",0,32);
        // tft.drawNumber(bestRow,80,32);
        // tft.drawNumber(bestCol,112,32);
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
    if(Menu_Mode == 7)
    {
        draw_Chinese(0, 0, "模", TFT_WHITE);
        draw_Chinese(16, 0, "式", TFT_WHITE);
        tft.drawNumber(7,32,0);
        draw_Chinese(0, 32, "步", TFT_WHITE);
        draw_Chinese(16, 32, "骤", TFT_WHITE);
        tft.drawString(":",32,32);
        tft.drawNumber(Chess_selmode,48,32);
        draw_Chinese(64, 32, "最", TFT_WHITE);
        draw_Chinese(80, 32, "佳", TFT_WHITE);
        tft.drawString(":",96,32);
        tft.drawNumber(bestRow, 112,32);
        tft.drawNumber(bestCol,128,32);
        draw_Chinese(80, 80, "棋", TFT_WHITE);
        draw_Chinese(96, 80, "盘", TFT_WHITE);
        tft.drawLine(0,60,66,60,TFT_WHITE);//一横线
        tft.drawLine(0,81,66,81,TFT_WHITE);//二横线
        tft.drawLine(0,102,66,102,TFT_WHITE);//三横线
        tft.drawLine(0,123,66,123,TFT_WHITE);//四横线
        tft.drawLine(0,60,0,123,TFT_WHITE);//左一横线
        tft.drawLine(21,60,21,123,TFT_WHITE);//左二横线
        tft.drawLine(44,60,44,123,TFT_WHITE);//左三横线
        tft.drawLine(66,60,66,123,TFT_WHITE);//左四横线
        tft.drawNumber(BigChess_Pos[1],5,64);
        tft.drawNumber(BigChess_Pos[2],29,64);
        tft.drawNumber(BigChess_Pos[3],53,64);
        tft.drawNumber(BigChess_Pos[4],5,85);
        tft.drawNumber(BigChess_Pos[5],29,85);
        tft.drawNumber(BigChess_Pos[6],53,85);
        tft.drawNumber(BigChess_Pos[7],5,106);
        tft.drawNumber(BigChess_Pos[8],29,106);
        tft.drawNumber(BigChess_Pos[9],53,106);

        
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
    if(Menu_Mode == 8)
    {
        draw_Chinese(0, 0, "模", TFT_WHITE);
        draw_Chinese(16, 0, "式", TFT_WHITE);
        tft.drawNumber(8,32,0);

        draw_Chinese(0, 32, "步", TFT_WHITE);
        draw_Chinese(16, 32, "骤", TFT_WHITE);
        tft.drawString(":",32,32);
        tft.drawNumber(Chess_selmode,48,32);

        draw_Chinese(64, 32, "最", TFT_WHITE);
        draw_Chinese(80, 32, "佳", TFT_WHITE);
        tft.drawString(":",96,32);
        tft.drawNumber(bestRow, 112,32);
        tft.drawNumber(bestCol,128,32);

        draw_Chinese(80, 80, "棋", TFT_WHITE);
        draw_Chinese(96, 80, "盘", TFT_WHITE);
        tft.drawLine(0,60,66,60,TFT_WHITE);//一横线
        tft.drawLine(0,81,66,81,TFT_WHITE);//二横线
        tft.drawLine(0,102,66,102,TFT_WHITE);//三横线
        tft.drawLine(0,123,66,123,TFT_WHITE);//四横线
        tft.drawLine(0,60,0,123,TFT_WHITE);//左一横线
        tft.drawLine(21,60,21,123,TFT_WHITE);//左二横线
        tft.drawLine(44,60,44,123,TFT_WHITE);//左三横线
        tft.drawLine(66,60,66,123,TFT_WHITE);//左四横线
        tft.drawNumber(BigChess_Pos[1],5,64);
        tft.drawNumber(BigChess_Pos[2],29,64);
        tft.drawNumber(BigChess_Pos[3],53,64);
        tft.drawNumber(BigChess_Pos[4],5,85);
        tft.drawNumber(BigChess_Pos[5],29,85);
        tft.drawNumber(BigChess_Pos[6],53,85);
        tft.drawNumber(BigChess_Pos[7],5,106);
        tft.drawNumber(BigChess_Pos[8],29,106);
        tft.drawNumber(BigChess_Pos[9],53,106);
        
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