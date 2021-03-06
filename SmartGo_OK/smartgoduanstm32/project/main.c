#include "stm32f10x.h"
#include "math.h"
#include "delay.h"
#include "DirectionData.h"
#include "UART_Driver.h"
#include "SSD1306.h"
#include "stm32f10x_i2c.h"
#include <stdlib.h>


#define EARTH_RADIUS 6378.137
#define PI 3.14159265359
#define MESSAGE_DELIMITERS_STRING "()	, "

USART_InitTypeDef USART_InitStructure;
GPIO_InitTypeDef  GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;

// Agrument
DirectionDataT *node_get = NULL;
static DirectionDataListT list;
char latOK[70] = "";
char lonOK[70] = "";
char ok[70]= "";
char s[70] = "";
char pos = 0;
float flat, flon;
bool daQua = false, sapQua = false;
	
// Function
void USART_init(void);
void GPIO_Configuration(void);
void EXTILine0_Config(void);
void Delay( uint32_t nCount);
void getLatLon(char *mess);
float convertNmeaToDec(double nmea);


uint8_t messageToDirectionData(char *message, int len, uint8_t *nextDirection, float *longtitude, float *latetidude);
void UART2_ReceiveMessage(char *message, uint8_t length);
float coordinatesToMeters(float lat1, float long1, float lat2, float long2);
void myI2C_Init(void);
void kc50();
void kc15();
void thang();
void trai();
void phai();
void lui();

const uint8_t character0[]=
{
	
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xF0, 0x30, 0x30, 0x08, 0x08, 0x08,		//0
0x08, 0x08, 0x38, 0xF0, 0xF0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x80, 0xF0, 0xF0, 0xFC, 0xFF, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFC, 0xF0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0xE0, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00,
0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0x00, 0x01, 0x01, 0x7F, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0xFF, 0xFF, 0x7F, 0x7F, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x0F, 0xFF, 0xFC, 0xFC, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0xC0, 0xC0, 0xFC, 0xFF, 0xFF, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x07, 0x07, 0x04, 0x04, 0x04,
0x04, 0x04, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

};

	
	const uint8_t character1[]=
{

0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xE0,		
0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x06, 0x06, 0x06, 0x06, 0x01, 0x01, 0x01, 0x01, 0x01, 0x07, 0x07, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F,
0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0E, 0x0E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00
};

	const uint8_t character5[]=
{
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xE0, 0xE0, 0xE0,
0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xFC, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0xFE, 0xFF, 0xFF, 0xF8, 0xF8, 0xF8, 0xF8, 0xE0, 0xE0,
0xE0, 0xE0, 0xE0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
0x3F, 0x3F, 0x3F, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xF8, 0xF0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x0F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xF0, 0x3C, 0x3C, 0x0F, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
0x0C, 0x0F, 0x0F, 0x0F, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};



int main(void)
{
	int flag = 0;
	delay_Init();
	USART_init();
	DirectionDataList_Init(&list);
	UART_Init(UART2_ReceiveMessage);
	UART_SendStr("SmartGo\n\0");
	myI2C_Init();
	init(0x3C);
	clearDisplay();
	while (1)
	{
		if(list.count>0){
			node_get = DirectionDataList_Get(&list);
			thang();
			if(daQua == true){
				node_get = DirectionDataList_Get(&list);
				daQua = false;
				sapQua = false;
			}
			while(coordinatesToMeters(flat,flon,node_get->latetitude,node_get->longtitude)<=50){
				kc50();
				while(coordinatesToMeters(flat,flon,node_get->latetitude,node_get->longtitude)<=7){
						sapQua = true;
						if(node_get->nextDirection==1){
							kc15();	
							trai();
						}
						else if(node_get->nextDirection==2){
							kc15();
							phai();
						}
						if(sapQua==true && coordinatesToMeters(flat,flon,node_get->latetitude,node_get->longtitude)>7){
						daQua = true;
						break;
					}
				}
				if(daQua == true) break;
			}
		}
	}
}

uint8_t messageToDirectionData(char *message, int len, uint8_t *nextDirection, float *longtitude, float *latetidude)
{
	char *tokenPointer;
	if (message != NULL )
	{
		tokenPointer = strtok(message, MESSAGE_DELIMITERS_STRING);
		*nextDirection = atoi(tokenPointer);
		tokenPointer = strtok(NULL, MESSAGE_DELIMITERS_STRING);
		if (tokenPointer != NULL)
		{
			*longtitude = atof(tokenPointer);
		}
		else
		{
			return 0;
		}

		tokenPointer = strtok(NULL, MESSAGE_DELIMITERS_STRING);
		if (tokenPointer != NULL)
		{
			*latetidude = atof(tokenPointer);
		}
		else
		{
			return 0;
		}

		tokenPointer = strtok(NULL, MESSAGE_DELIMITERS_STRING);
		return 1;
	}
	else
	{
		*nextDirection = 0;
		*longtitude = 0;
		*latetidude = 0;

		return 0;
	}
}

void UART2_ReceiveMessage(char *message, uint8_t length)
{
	uint8_t nextDi;
	float longtitude, latetitude;
	int result;

	UART_SendStr(message);

	if (messageToDirectionData(message, length, &nextDi, &longtitude, &latetitude) == 1)
	{
  		result = DirectionDataList_Put(&list, nextDi, longtitude, latetitude);
	}
}

float coordinatesToMeters(float lat1, float long1, float lat2, float long2)
{
	float dLat = lat2 * PI / 180 - lat1 * PI / 180;
	float dLon = long2 * PI / 180 - long1 * PI / 180;
	float a = sin(dLat / 2) * sin(dLat / 2) + cos(lat1 * PI / 180) * cos(lat2 * PI / 180) * sin(dLon / 2) * sin(dLon / 2);
	float c = 2 * atan2(sqrt(a), sqrt(1 - a));
	float d = EARTH_RADIUS * c;
	return d * 1000;
}

void myI2C_Init(void){
		I2C_InitTypeDef I2C_InitStructure;
		GPIO_InitTypeDef GPIO_InitStruct;
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
/***********************************************************************************************
*											GPIO_Init						
***********************************************************************************************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	/* Set Led */
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	  GPIO_Init(GPIOC, &GPIO_InitStruct);
		
		GPIO_SetBits(GPIOC, GPIO_Pin_13);
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		
    //GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure I2C pins: SCL and SDA */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;// MPU6050 7-bit adress = 0x68, 8-bit adress = 0xD0;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;

    /* Apply I2C configuration after enabling it */
    I2C_Init(I2C2, &I2C_InitStructure);
    /* I2C Peripheral Enable */
    I2C_Cmd(I2C2, ENABLE);
}
void kc50()
{
	clearDisplay();
	drawBitmap(32,1,character5,32,64,WHITE);
	drawBitmap(64,1,character0,32,64,WHITE);
	display();
}

void kc15(){
	clearDisplay();
	drawBitmap(32,1,character1,32,64,WHITE);
	drawBitmap(64,1,character5,32,64,WHITE);
	display();
	}
float convertNmeaToDec(double nmea){
	int deg = ((int)nmea / 100);
	float decPos = deg + ((nmea - (deg * 100)) / 60);
	return decPos;
}
void getLatLon(char* mess){
//	float flat, flon;
	float lat_temp, lon_temp;
	int count_comma;
	int i = 0, length = 0, lat_index = 0, lon_index = 0;
	char lat[11], lon[12];
	while (mess[i] != NULL)
	{
		i++;
		length++;
	}

	count_comma = 1;
	i = 7;

	while (count_comma != 5)
	{
		if (mess[i] == ',')
		{
			i++;
			count_comma++;
		}
		else
		{
			switch (count_comma)
			{
			case 1:
			case 2:
				lat[lat_index] = mess[i];
				i++;
				lat_index++;
				break;
			case 3:
			case 4:
				lon[lon_index] = mess[i];
				i++;
				lon_index++;
				break;
			}
		}
	}

	lat[lat_index-1] = '\0';
	lon[lon_index-1] = '\0';
	strcpy(latOK, lat);
	strcpy(lonOK, lon);
	lat_temp = atof(lat);
	lon_temp = atof(lon);
	flat = convertNmeaToDec(lat_temp);	 // -10
	flon = convertNmeaToDec(lon_temp); // -106
}
void USART1_IRQHandler(void){
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
			s[pos] = (char)USART_ReceiveData(USART1);
			pos++;
			if (s[pos - 1] == '\n')
			{
				if (strstr(s, "$GNGLL") != NULL)
				{
					strcpy(ok,s);
					getLatLon(s);
					GPIO_ResetBits(GPIOC, GPIO_Pin_13);
				}
				pos = 0;
				strcpy(s, "");
			}
		}
}
void USART_init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	
	/* Config USART1 Tx - PA9*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Config USART1 Rx - PA10*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* NVIC Config */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/* Configuration */
	USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}
void thang(){}
void trai(){}
void phai(){}
void lui(){}