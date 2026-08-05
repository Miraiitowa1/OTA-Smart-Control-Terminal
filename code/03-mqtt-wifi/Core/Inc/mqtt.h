#ifndef __MQTT_H_
#define __MQTT_H_
 
#include "main.h"

extern  __IO uint8_t MQTT_UPLoad_Flag  ; // 数据上传服务器标志位， 每隔5秒上传1次
extern  __IO uint8_t MQTT_Download_Flag ;  // 开始接收服务器下发的命令，开始解析的标志位
uint8_t ESP8266_Connect_MQTTServer(void);
uint8_t MQTT_SendData(void);
uint8_t MQTT_Parse_JsonData(uint8_t *json);
#endif //  __MQTT_H_
