/*
 * wifi.c
 *
 *  Created on: Apr 4, 2023
 *  	base : KSH
 *      modified by JUB, KMJ
 */

#include <stdio.h>
#include <string.h>
#include "wifi.h"
#include "usart.h"

#define DEBUG_WIFI
#define TIMEOUT 250L
extern volatile unsigned long systick_count;
extern volatile unsigned char rx2Data;
extern volatile unsigned char rx2Flag ; // 수신 인터럽트가 발생했는지를 알아보는 변수
extern void Serial2_Send_String(char *s);


volatile char uart2_rxdata[5][100];

int WIFI_init()
{
	int i;
	printf("Wifi Module Restart!!\r\n");
	if (!wifi_send_cmd("AT+RST","OK",TIMEOUT))   //wifi reset
	{
		printf("Module have no response.\r\n");
	}
	for(i=0;i<5;i++)
	{
		if(connectWiFi())
		{
			printf("OK, Connected to WiFi.\r\n");
			break;
		}
	}
	if(i == 5)
	{
		printf("Wifi connect fail.\r\n");
		return 0;
	}
	if (!wifi_send_cmd("AT+CIFSR","+CIFSR:STAIP,\"192",TIMEOUT)) //local ip
		;

	if (!wifi_send_cmd("AT+CIPMUX=0","OK",TIMEOUT))   //single mode
		;
	return 1;
}
int connectWiFi()
{
	char cmd[50]={0};

	if (!wifi_send_cmd("AT+CWMODE=1","OK",TIMEOUT))	// timeout : 250 * 0.004 = 1Sec
		;

	sprintf(cmd,"AT+CWJAP=\"%s\",\"%s\"",SSID,PASS);		//AP name, password
	if (wifi_send_cmd(cmd,"OK",1000L))  { 					//AP 접속
		return 1;
	}
	return 0;
}
int wifi_send_cmd (char *str,const char *rstr, unsigned long timeout)
{
	int ret=0;
	Serial2_Send_String(str);
	Serial2_Send_String("\r\n");

	ret = wifi_wait(str,rstr, timeout);

	if(ret)
		;
	else
	{
#ifdef  DEBUG_WIFI
		printf("\n[Recv_NG] wifi_wait(%s): TIMEOUT\r\n",str);

		for(int i=0;i<5;i++)
		{
			printf("RECV TEST %d : %s\r\n",i,uart2_rxdata[i]);
		}
#endif
	}
	return ret;

}
int wifi_wait(const char *str, const char *rstr, unsigned long timeout)
{

	int ucount_recv=0;
	int i=0;
	unsigned char data=0;
	unsigned long tick = systick_count;

	ucount_recv = 0;
	memset((void *)uart2_rxdata,0x00,sizeof(uart2_rxdata));
	while (systick_count - tick < timeout)
	{
		if (rx2Flag == 0)
			continue;
		else if(rx2Flag == 1)
		{
			uart2_rxdata[i][ucount_recv] = data = rx2Data;
			rx2Flag = 0;
			ucount_recv++;
		}
		if (data == '\n')  //CR:0x0d:'\r', LF:0x0a:'\n'
		{
			uart2_rxdata[i][ucount_recv] = 0;
			if (strncmp(( char *)uart2_rxdata[i], rstr, strlen(rstr)) == 0)  //FIND
			{
				if(!strncmp((char *)uart2_rxdata[i],"+CIFSR:STAIP",3))
					printf("Local IP : %s\n",uart2_rxdata[i]+13);
				return 1;
			}

			ucount_recv = 0;
			i++;
			if(i == 5)
				break;
			continue;
		}

	}//close while
	return 0;
}

int WIFI_send(char *data)
{
	static int connect_flag = 0;

	char cmd[200]={0};
	if(!connect_flag)
	{
		sprintf(cmd,"AT+CIPSTART=\"TCP\",\"%s\",%d",DST_IP,DST_PORT);
		if (wifi_send_cmd(cmd,"CONNECT",TIMEOUT*2))   // timeout : 250 * 0.004 = 1Sec
			connect_flag = 1;

		else if (wifi_send_cmd(cmd,"ALREADY",TIMEOUT))   // timeout : 250 * 0.004 = 1Sec
			connect_flag = 1;
	}

	sprintf(cmd,"AT+CIPSEND=%d",strlen(data));

	if (!wifi_send_cmd(cmd,"OK",TIMEOUT))   // <AT+CIPSEND=10>   응답메시지
		;

	if (!wifi_send_cmd(data,">",TIMEOUT))
		connect_flag = 0;

	return connect_flag;
}
