#ifndef _SERIAL_H
#define _SERIAL_H
#include <Arduino.h>
#include <Stepmotor.h>

typedef enum
{
    NONE = 0,
    BLACK_CHE = 2,
    WHITE_CHE = 1,
}ChessState_t;

typedef struct Chess_Board
{   
    uint8_t key;
    ChessState_t state;
    int16_t x;
    int16_t y;
}ChessBoard_t;


typedef struct RX_Buffer
{
    uint8_t Buffer[16];
    uint8_t length;
}RX_Buffer_t;

extern ChessBoard_t chess[9];
extern bool FinishReceiveFlag;
void Serial_Init(void);
void Serial_CallBack(void);

#endif // !_SERIAL_H
