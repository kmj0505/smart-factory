#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

LiquidCrystal_I2C lcd(0x26, 16, 2);
#define DEBUG
#define RELAY 3
#define BTSerial Serial3
// set the LCD address to 0x27 for a 16 chars and 2 line display
char recv_data[50];
char send_data[20];
char lcd_line1[17] = { 0 };
char lcd_line2[17] = { 0 };
char product[3];
volatile unsigned char TCRT_flag = 0;
volatile unsigned char belt_state = 1;
void setup() {
  pinMode(RELAY, OUTPUT);  // 디지털 3번핀을 출력모드로 설정
  Serial.begin(9600);      // 시리얼 통신을 시작하며, 속도는 9600으로 설정
  Serial3.begin(9600);
#ifdef DEBUG
  Serial.println("DEBUG:HELLO!");
#endif         //DEBUG
  lcd.init();  // initialize the lcd
  lcd.backlight();
  attachInterrupt(digitalPinToInterrupt(18), tcrtControl, RISING);
}

void tcrtControl() {
#ifdef DEBUG
  Serial.println("Interrupt");
#endif  //DEBUG
  belt_state = 0;
  TCRT_flag = 1;
}

void loop() {

  if (belt_state == 1) {
    digitalWrite(RELAY, HIGH);
  } else {
    digitalWrite(RELAY, LOW);
  }

  if (TCRT_flag == 1) {
    TCRT_flag = 0;
    lcdDisplay(0, 0, "Detect Object!");
    lcdDisplay(0, 1, "Wait ...");
    Serial3.print("[JET]DEV@QRON\n");
  }

  if (Serial3.available()) {
    socketEvent();
  }
}

void socketEvent() {
  int i = 0;
  int len;
  char* pToken;
  char* pArray[5] = { 0 };
  char recvBuf[50] = { 0 };
  send_data[0] = '\0';
  len = Serial3.readBytesUntil('\n', recvBuf, 50);
  Serial3.flush();
#ifdef DEBUG
  Serial.print("recv : ");
  Serial.println(recvBuf);
#endif  //DEBUG
  pToken = strtok(recvBuf, "[@]");
  while (pToken != NULL) {
    pArray[i] = pToken;
    if (++i >= 5)
      break;
    pToken = strtok(NULL, "[@]");
  }
  if (pArray[3] != NULL) {
    if (!strcmp(pArray[0], "JET")) {  //참이면 1반환
      strcpy(product, pArray[3]);
      sprintf(lcd_line1, "Product : %s", product);
#ifdef DEBUG
      Serial.println(product);
#endif  //DEBUG
      lcdDisplay(0, 1, lcd_line1);
      if (!strcmp(product, "A")) {
        sprintf(send_data, "[STM]DEV@SERVO@2\n");
      } else if (!strcmp(product, "B")) {
        sprintf(send_data, "[STM]DEV@SERVO@3\n");
      } else if ((!strcmp(product, "C")) || (!strcmp(product, "X"))) {
        sprintf(send_data, "[STM]DEV@SERVO@1\n");
#ifdef DEBUG
        Serial.print(send_data);
#endif  //DEBUG
        Serial3.print(send_data);
        memset(send_data, 0x0, sizeof(send_data));
        delay(100);
        if (!strcmp(product, "X")) {
          sprintf(send_data, "[FCM]DEV@FIND@X\n");
        }
      }
#ifdef DEBUG
      Serial.print(send_data);
#endif  //DEBUG
      Serial3.print(send_data);
      return;
    }

    else if (!strcmp(pArray[0], "STM")) {
#ifdef DEBUG
      Serial.println("get stm");
#endif  //DEBUG
      if (!strcmp(pArray[2], "SERVO")) {
        sprintf(send_data, "[SQL]DEV@Product%s@1\n", product);
#ifdef DEBUG
        Serial.println("belt on");
        Serial.print(send_data);
#endif  //DEBUG
        Serial3.print(send_data);
        belt_state = 1;
        return;
      }
    } else if (!strcmp(pArray[0], "AND")) {
#ifdef DEBUG
      Serial.println("get and");
#endif  //DEBUG
      if (!strcmp(pArray[3], "ON")) {
        sprintf(send_data, "[AND]DEV@BELT@ON\n");
#ifdef DEBUG
        Serial.println("belt switch on");
        Serial.print(send_data);
#endif  //DEBUG
        Serial3.print(send_data);
        belt_state = 1;
        return;
      } else if (!strcmp(pArray[3], "OFF")) {
        sprintf(send_data, "[AND]DEV@BELT@OFF\n");
#ifdef DEBUG
        Serial.println("belt switch off");
        Serial.print(send_data);
#endif  //DEBUG
        Serial3.print(send_data);
        belt_state = 0;
        return;
      }
    } else {
#ifdef DEBUG
      Serial.println("no stm and jet");
#endif  //DEBUG
      return;
    }
  } else {
    strcpy(product, "   ");
    sprintf(lcd_line1, "Product : %s", product);
    lcdDisplay(0, 1, lcd_line1);
    return;
  }
}

void lcdDisplay(int x, int y, char* str) {
  int len = 16 - strlen(str);
  lcd.setCursor(x, y);
  lcd.print(str);
  for (int i = len; i > 0; i--)
    lcd.write(' ');
}
