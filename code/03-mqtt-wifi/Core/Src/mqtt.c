#include "mqtt.h"
#include "wifi4g.h"
#include "usart.h"
#include "oled.h"
#include "modbus.h"
#include "cjson.h"

__IO uint8_t MQTT_UPLoad_Flag  = 0 ; // 数据上传服务器标志位， 每隔5秒上传1次
__IO uint8_t MQTT_Download_Flag = 0 ;  // 开始接收服务器下发的命令，开始解析的标志位


uint8_t *Get_CPUID(void)
{
	static uint8_t cpuid[32] = {0};
	// 每个芯片都有唯一的 96_bit unique ID
	uint32_t CPU_ID[3];
	CPU_ID[0] = *(__IO uint32_t *)(0X1FFFF7F0); // 高字节
	CPU_ID[1] = *(__IO uint32_t *)(0X1FFFF7EC); //
	CPU_ID[2] = *(__IO uint32_t *)(0X1FFFF7E8); // 低字节

	/* 芯片的唯一ID */
	sprintf((char *)cpuid, "%08X%08X%08X", CPU_ID[0], CPU_ID[1], CPU_ID[2]);	
	return cpuid;
}

uint8_t ESP8266_Connect_MQTTServer(void)
{
	uint8_t buf[64] = {0};
	OLED_ShowStr(0,4,(unsigned char *)"MQTTServer... --",2);	//测试8*16字符
	printf("cpuid:%s\n",Get_CPUID());
	WIFI4G_CMD_Status = WIFI4G_NOT;			// 初始化标志位
	sprintf((char *)buf, "AT+MQTTUSERCFG=0,1,\"%s\",\"\",\"\",0,0,\"\"\r\n",Get_CPUID()); // 单链接
	printf("buf=%s\n",buf);
	HAL_UART_Transmit(&huart3, buf, strlen((char *)buf), 1000);
	if (Test_WIFI4G_CMD_Status(1000) == WIFI4G_ERROR)
	{
		return RESET;												// 等待OK返回
	}
	
	// 连接mqtt服务器
	WIFI4G_CMD_Status = WIFI4G_NOT ;	 // 初始化标志位 
	strcpy((char *)buf,"AT+MQTTCONN=0,\"broker.emqx.io\",1883,1\r\n");
	HAL_UART_Transmit(&huart3,buf,strlen((char *)buf),1000);
	if(Test_WIFI4G_CMD_Status(1000) == WIFI4G_ERROR)  {
		return RESET ;  
	}
	
	//  订阅主题消息 , 订阅一个下行数据的主题 
	WIFI4G_CMD_Status = WIFI4G_NOT ;	 // 初始化标志位 
	OLED_ShowStr(0,4,(unsigned char *)"AT+MQTTSUB",2);				//测试8*16字符
	sprintf((char *)buf,"AT+MQTTSUB=0,\"STM32V9/DownLoad/%s\",0\r\n",Get_CPUID());  
	HAL_UART_Transmit(&huart3,buf,strlen((char *)buf),1000);

	uint8_t ret = Test_WIFI4G_CMD_Status(5*1000) ;
	if (ret == WIFI4G_OK)
	{
			OLED_ShowStr(0,4,(unsigned char *)"MQTTServer... OK",2);	//测试8*16字符
			return SET;
	}
	else 
	{
			OLED_ShowStr(0,4,(unsigned char *)"MQTTServer... --",2);	//测试8*16字符
			return  RESET; 
	}
}


uint8_t MQTT_SendData(void)
{

	static uint8_t sendbuf[128] = {0};
	static uint8_t buf[256] = {0};

	sprintf((char *)sendbuf,"\"{\\\"TP\\\":%d\\,\\\"RH\\\":%d\\,\\\"VO\\\":%d\\,\\\"CU\\\":%d\\,\\\"PW\\\":%d\\,\\\"VR\\\":%d\\,\\\"CPU\\\":%d}\"",
	REG_HOLD_BUF[1],REG_HOLD_BUF[2], REG_HOLD_BUF[3],REG_HOLD_BUF[4],REG_HOLD_BUF[5],REG_HOLD_BUF[6],REG_HOLD_BUF[7]);

	//printf("%s",sendbuf);
	// 发送数据到mqtt服务器
	sprintf((char *)buf,"AT+MQTTPUB=0,\"STM32V9/UPLoad/%s\",%s,0,0\r\n",Get_CPUID(),sendbuf);
	//printf("%s",buf);

	HAL_UART_Transmit(&huart3,(uint8_t*)buf,strlen((char *)buf),1000);

	return SET;
}

uint8_t MQTT_Parse_JsonData(uint8_t *json)
{
		cJSON *cjson_device = NULL;
		cJSON *cjson_led1 = NULL;
		cJSON *cjson_led2 = NULL;
		cJSON *cjson_beep = NULL;
		cJSON *cjson_relay = NULL;
		    /* 解析整段JSO数据 */
    cjson_device = cJSON_Parse((char *)json);
    if(cjson_device == NULL)
    {
        printf("parse fail.\n");
        return RESET;
    }

    /* 依次根据名称提取JSON数据（键值对） */
    cjson_led1 = cJSON_GetObjectItem(cjson_device, "LED1");
		cjson_led2 = cJSON_GetObjectItem(cjson_device, "LED2");
		cjson_beep = cJSON_GetObjectItem(cjson_device, "BEEP");
		cjson_relay = cJSON_GetObjectItem(cjson_device, "RELAY");

//		printf("LED1: %d\n", cjson_led1->valueint);
//		printf("LED1: %d\n", cjson_led1->valueint);
//		printf("LED1: %d\n", cjson_led1->valueint);
//		printf("LED1: %d\n", cjson_led1->valueint);
		if(cjson_led1 != NULL)
		{
				if(cjson_led1->valueint == 0 ) // led1 off 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] & (~LED1_CMD) ;  // 第0位 写0
				}
				else 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] | LED1_CMD    ; // 第0位 写1
				}

		}
		
		if(cjson_led2 != NULL)
		{
				if(cjson_led2->valueint == 0 ) // led2 off 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] & (~LED2_CMD) ;  // 第1位 写0
				}
				else 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] | LED2_CMD    ; // 第1位 写1
				}

		}
		
		if(cjson_beep != NULL)
		{
				if(cjson_beep->valueint == 0 ) // beep off 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] & (~BEEP_CMD) ;  // 第2位 写0
				}
				else 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] | BEEP_CMD    ; // 第2位 写1
				}

		}
		
		if(cjson_relay != NULL)
		{
				if(cjson_relay->valueint == 0 ) // relay off 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] & (~RELAY_CMD) ;  // 第3位 写0
				}
				else 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] | RELAY_CMD    ; // 第3位 写1
				}

		}
		
		cJSON_Delete(cjson_device); // 一定要释放内存
		return SET;
}

