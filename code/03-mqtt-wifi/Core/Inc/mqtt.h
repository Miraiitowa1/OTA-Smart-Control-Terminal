#ifndef __MQTT_H_
#define __MQTT_H_

#include "main.h"

extern __IO uint8_t MQTT_UPLoad_Flag; //	数据上传服务器标志位，每隔5s上传一次
extern __IO uint8_t MQTT_DownLoad_Flag; //	开始接收服务器下发的命令，开始解析的标志位

uint8_t MQTT_WIFI_Connect_Server(void);
uint8_t MQTT_SendData(void);
uint8_t MQTT_Parse_JsonData(uint8_t *json);

#endif //	__MQTT_H_
