#include "wifi_driver.h"

wifi_t wifi_client = {
    .ssid = "DeBug406",
    .password = "debug406",
};


void WIFI_init(void)
{
    WiFi.begin(wifi_client.ssid, wifi_client.password);
    while (!WiFi.isConnected())
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
}