#ifndef _TFT_DRIVER_H
#define _TFT_DRIVER_H
#include <TFT_eSPI.h> 
#include <key_driver.h>


// //��Ļ���Ŷ���
// #define TFT_CS        32
// #define TFT_RST       33 
// #define TFT_DC        27
#define TFT_BACKLIGHT 12
#define PER_HEIGHT 16

extern TFT_eSPI tft;
extern uint16_t menuNum;

typedef struct cursor{
    char* ico;    // ����ͼ��
    uint16_t currLine;   // ��굱ǰ������
}cursor_t;

extern cursor_t cursor;
extern const char* menuName[];
extern uint16_t Chessboard_RD_X;//��������X����
extern uint16_t Chessboard_RD_Y;//��������Y����
extern uint16_t Chessboard_LU_X;//��������X����
extern uint16_t Chessboard_LU_Y;//��������Y����

extern uint16_t Chess_R_X;//�ұߵ�2������X����
extern uint16_t Chess_R_Y;//�ұߵ�2������Y����
extern uint16_t Chess_L_X;//��ߵ�1������X����
extern uint16_t Chess_L_Y;//��ߵ�1������Y����

extern uint8_t Menu_Mode;
extern uint8_t Chess_Pos;
void TFT_init(void);
void TFT_refresh(void);
void Menu_List(void);
void Menu_Task(void);
void Parameter_Init(void);
#endif // !_TFT_DRIVER_H
