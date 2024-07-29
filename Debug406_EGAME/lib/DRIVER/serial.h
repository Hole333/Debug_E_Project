#ifndef _SERIAL_H
#define _SERIAL_H
#include <Arduino.h>
#include <Stepmotor.h>

typedef struct RX_Buffer
{
    uint8_t Buffer[5];
    uint8_t length;
}RX_Buffer_t;

void Serial_Init(void);
void Serial_CallBack(void);

#endif // !_SERIAL_H
