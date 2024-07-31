#ifndef _TFT_DRIVER_H
#define _TFT_DRIVER_H
#include <TFT_eSPI.h> 
#include <key_driver.h>


// //屏幕引脚定义
// #define TFT_CS        32
// #define TFT_RST       33 
// #define TFT_DC        27
#define TFT_BACKLIGHT 12
#define PER_HEIGHT 16

extern TFT_eSPI tft;
extern uint16_t menuNum;

typedef struct cursor{
    char* ico;    // 光标的图标
    uint16_t currLine;   // 光标当前所在行
}cursor_t;

extern cursor_t cursor;
extern const char* menuName[];
extern uint16_t Chessboard_RD_X;//棋盘右下X坐标
extern uint16_t Chessboard_RD_Y;//棋盘右下Y坐标
extern uint16_t Chessboard_LU_X;//棋盘左上X坐标
extern uint16_t Chessboard_LU_Y;//棋盘左上Y坐标

extern uint16_t Chess_R_X;//右边第2个棋子X坐标
extern uint16_t Chess_R_Y;//右边第2个棋子Y坐标
extern uint16_t Chess_L_X;//左边第1个棋子X坐标
extern uint16_t Chess_L_Y;//左边第1个棋子Y坐标


extern int8_t Menu_Mode;
extern int8_t Menu_Modebuff;
extern uint8_t Chess_selmode;//棋子状态
extern uint8_t Chess_selmodebuff;//棋子状态
extern uint8_t Chess_Pos;//棋子要放到棋盘的哪个位置的位置
extern uint8_t Chesssource_Count;//黑色棋子编号
extern uint8_t bestRow;
extern uint8_t bestCol;


extern uint16_t BigChess_Pos[10];

void TFT_init(void);
void TFT_refresh(void);
void Menu_List(void);
void Menu_Task(void);
void Parameter_Init(void);
#endif // !_TFT_DRIVER_H
