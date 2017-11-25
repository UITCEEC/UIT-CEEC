#include "stm32f10x.h" // Device header
#include "math.h"
#include "delay.h"
#include "DirectionData.h"
#include "UART_Driver.h"
#include "SSD1306.h"

#define EARTH_RADIUS 6378.137
#define PI 3.14159265359
#define MESSAGE_DELIMITERS_STRING "()	, "



static DirectionDataListT list;

uint8_t messageToDirectionData(char *message, int len, uint8_t *nextDirection, float *longtitude, float *latetidude);
void UART2_ReceiveMessage(char *message, uint8_t length);
float coordinatesToMeters(float lat1, float long1, float lat2, float long2);
void myI2C_Init(void);
unsigned int convert(char *array);

//ngat()
//{
//	DirectionDataT *ptr = DirectionDataList_Get(&list);
//	ptr->
//	free(ptr);
//}

int main(void)
{
	
	delay_Init();
	UART_Init(UART2_ReceiveMessage);
	DirectionDataList_Init(&list);
	UART_SendStr("SmartGo\n\0");
	delay_us(50000); // 50ms

	while (1)
	{
	}
}

uint8_t messageToDirectionData(char *message, int len, uint8_t *nextDirection, float *longtitude, float *latetidude)
{
	char *tokenPointer;
//	float temp_lat,temp_lon;
	if (message != NULL && message[0] == '(' && message[len - 2] == ')')
	{
		tokenPointer = strtok(message, MESSAGE_DELIMITERS_STRING);
		*nextDirection = atoi(tokenPointer);
		tokenPointer = strtok(NULL, MESSAGE_DELIMITERS_STRING);
		if (tokenPointer != NULL)
		{
		//	temp_lon = atof(tokenPointer);
		//	*longtitude = temp_lon;
			//*longtitude = atof(tokenPointer);
			*longtitude=convert(tokenPointer);
		}
		else
		{
			return 0;
		}

		tokenPointer = strtok(NULL, MESSAGE_DELIMITERS_STRING);
		if (tokenPointer != NULL)
		{
			//temp_lat = atof(tokenPointer);
			*latetidude = convert(tokenPointer);
		}
		else
		{
			return 0;
		}

		tokenPointer = strtok(NULL, MESSAGE_DELIMITERS_STRING);
		if (tokenPointer == NULL)
		{
			return 1;
		}
		else
		{
			return 0;
		}
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
//		if (result == STATUS_OK)
//		{
//			GPIO_ResetBits(GPIOB, GPIO_Pin_4);
//			GPIO_ResetBits(GPIOB, GPIO_Pin_14);
//			GPIO_ResetBits(GPIOA, GPIO_Pin_0);
//			GPIO_ResetBits(GPIOA, GPIO_Pin_7);
//	
//		}
//		else if (result == STATUS_EXCEED_LIMIT)
//		{
//			GPIO_SetBits(GPIOB, GPIO_Pin_4);
//			GPIO_SetBits(GPIOA, GPIO_Pin_7);
//		
//		}
//		else if (result == STATUS_NOT_ENOUGH_MEMORY)
//		{
//			GPIO_SetBits(GPIOB, GPIO_Pin_4);
//			GPIO_SetBits(GPIOB, GPIO_Pin_14);
//			GPIO_SetBits(GPIOA, GPIO_Pin_0);
//			GPIO_SetBits(GPIOA, GPIO_Pin_7);
//		}
//		else
//		{
//			GPIO_ResetBits(GPIOB, GPIO_Pin_4);
//			GPIO_ResetBits(GPIOB, GPIO_Pin_14);
//			GPIO_ResetBits(GPIOA, GPIO_Pin_0);
//			GPIO_ResetBits(GPIOA, GPIO_Pin_7);
//		}
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
unsigned int convert(char *array)
{
	float temp = atof(array);
	temp*=10000000;
	return temp;
}
