#ifndef _WIFI_DRIVER_H
#define _WIFI_DRIVER_H

#include <WiFi.h>
#include <serial.h>

typedef struct
{
    char* ssid;
    char* password;
}wifi_t;

void WIFI_init(void);


#endif