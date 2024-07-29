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

void TFT_init(void);
void TFT_refresh(void);
void Menu_List(void);

#endif // !_TFT_DRIVER_H
