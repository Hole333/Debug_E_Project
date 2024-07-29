#include <tft_driver.h>
#include <string.h>

//������Ļ����
TFT_eSPI tft = TFT_eSPI();
cursor_t cursor;
const char* menuName[] = {"1.Set_angle", "2.Run_Speed", "3.Auto_Cor", "4.WIFI", "5.test5", "6.test6"}; 
uint16_t menuNum = sizeof(menuName) / sizeof(char*);

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
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    cursor.ico = (char*)"<=";
    cursor.currLine = 0; // ���ù���ʼλ��
    tft.drawString(cursor.ico, strlen(menuName[cursor.currLine]) * 12 , cursor.currLine * 16);
    Menu_List();
}


