/*
 * wifi.h
 *
 *  Created on: Apr 4, 2023
 *  	base : KSH
 *      modified by JUB, KMJ
 */

#ifndef INC_WIFI_H_
#define INC_WIFI_H_

#define SSID "iot1"
#define PASS "iot10000"
#define LOGID "STM"
#define PASSWD "PASSWD"
#define DST_IP "10.10.141.63"
#define DST_PORT 5000

int connectWiFi();
int wifi_send_cmd (char *str,const char *rstr, unsigned long timeout);
int wifi_wait(const char *str, const char *rstr, unsigned long timeout);
int WIFI_init();
int WIFI_send(char * data);

#endif /* INC_WIFI_H_ */
