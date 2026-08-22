#include "stm32f10x.h"                  // Device header
#include "main.h"
#include "Delay.h"
#include "OLED.h"
#include "encoder.h"
#include "led.h"
#include "MPU6050.h"

uint8_t Wake_time = 5;
uint8_t Wake_gravity = 4;
uint8_t Wake_gravity2 = 4;
uint8_t light_1 = 30;
uint8_t light_2 = 30;

static uint8_t page = 0;
static uint8_t led_on = 0;
static uint8_t mpu_ok = 0;
static uint8_t who_am_i = 0;
static int16_t accel[3];

static void ShowPage(void)
{
	OLED_Clear();
	if (page == 0)
	{
		OLED_ShowString(1, 1, "BASIC TEST");
		OLED_ShowString(2, 1, "L/R: PAGE");
		OLED_ShowString(3, 1, "PRESS: LED");
		OLED_ShowString(4, 1, led_on ? "LED: ON" : "LED: OFF");
	}
	else if (page == 1)
	{
		OLED_ShowString(1, 1, "LED TEST");
		OLED_ShowString(2, 1, "PB15 OUTPUT");
		OLED_ShowString(3, 1, "PRESS TO TOGGLE");
		OLED_ShowString(4, 1, led_on ? "STATE: ON" : "STATE: OFF");
	}
	else
	{
		MPU6050_ReadAccel(accel);
		OLED_ShowString(1, 1, "MPU6050");
		OLED_ShowString(2, 1, mpu_ok ? "OK ID:" : "ERROR ID:");
		OLED_ShowHexNum(2, 10, who_am_i, 2);
		OLED_ShowString(3, 1, "X:");
		OLED_ShowSignedNum(3, 3, accel[0], 3);
		OLED_ShowString(3, 9, "Y:");
		OLED_ShowSignedNum(3, 11, accel[1], 3);
		OLED_ShowString(4, 1, "Z:");
		OLED_ShowSignedNum(4, 3, accel[2], 5);
	}
}

int main(void)
{
	SystemInit();
	LED_Init();
	Encoder_Init();
	OLED_Init();
	mpu_ok = MPU6050_Init(&who_am_i);
	ShowPage();

	while (1)
	{
		Encoder_Event event = Encoder_Scan();
		if (event == ENCODER_RIGHT)
		{
			page = (page + 1) % 3;
			ShowPage();
		}
		else if (event == ENCODER_LEFT)
		{
			page = (page == 0) ? 2 : page - 1;
			ShowPage();
		}
		else if (event == ENCODER_PRESS)
		{
			led_on = !led_on;
			if (led_on) LED_ON(); else LED_OFF();
			ShowPage();
		}

		if (page == 2)
		{
			static uint16_t refresh_count = 0;
			if (++refresh_count >= 100)
			{
				refresh_count = 0;
				ShowPage();
			}
		}
		delay_ms(5);
	}
}
