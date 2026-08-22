#include "stm32f10x.h"
#include "main.h"
#include "Delay.h"
#include "OLED.h"
#include "MyRTC.h"
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
static int16_t smooth_roll = 0;
static int16_t smooth_pitch = 0;

static int16_t LimitTilt(int16_t value)
{
	if (value > 11) return 11;
	if (value < -11) return -11;
	return value;
}

static int16_t SmoothStep(int16_t current, int16_t target)
{
	int16_t difference = target - current;
	if (difference > 3) return current + 3;
	if (difference < -3) return current - 3;
	if (difference > 0) return current + 1;
	if (difference < 0) return current - 1;
	return current;
}

static void ShowNavigation(void)
{
	if (page == 0) OLED_ShowString(4, 1, "[HOME] LEVEL");
	else OLED_ShowString(4, 1, " HOME [LEVEL]");
}

static void ShowHomePage(void)
{
	MyRTC_ReadTime();
	OLED_Clear();
	OLED_ShowString(1, 1, "E-WATCH");
	OLED_ShowNum(1, 11, g_Time[3], 2);
	OLED_ShowChar(1, 13, ':');
	OLED_ShowNum(1, 14, g_Time[4], 2);
	OLED_ShowNum(2, 4, g_Time[3], 2);
	OLED_ShowChar(2, 6, ':');
	OLED_ShowNum(2, 7, g_Time[4], 2);
	OLED_ShowChar(2, 9, ':');
	OLED_ShowNum(2, 10, g_Time[5], 2);
	OLED_ShowNum(3, 1, g_Time[1], 2);
	OLED_ShowChar(3, 3, '/');
	OLED_ShowNum(3, 4, g_Time[2], 2);
	OLED_ShowString(3, 8, led_on ? "LED ON" : "READY");
	ShowNavigation();
}

static void ShowLevelPage(void)
{
	int16_t target_roll;
	int16_t target_pitch;
	if (mpu_ok) MPU6050_ReadAccel(accel);
	target_roll = LimitTilt(accel[0] / 1500);
	target_pitch = LimitTilt(accel[1] / 1500);
	smooth_roll = SmoothStep(smooth_roll, target_roll);
	smooth_pitch = SmoothStep(smooth_pitch, target_pitch);

	OLED_Clear();
	OLED_ClearBuffer();
	OLED_DrawRect(14, 33, 100, 29);
	OLED_DrawLine(20, 47, 108, 47);
	OLED_DrawLine(64, 36, 64, 59);
	OLED_DrawCircle(64 + smooth_roll, 47 + smooth_pitch, 6);
	OLED_DrawPixel(64 + smooth_roll, 47 + smooth_pitch, 1);
	OLED_DrawPixel(64, 63, 1);
	OLED_DrawPixel(72, 63, 1);
	OLED_Update();

	OLED_ShowString(1, 1, "HORIZONTAL");
	OLED_ShowString(2, 1, mpu_ok ? "R:" : "MPU ERROR");
	if (mpu_ok)
	{
		OLED_ShowSignedNum(2, 3, smooth_roll, 2);
		OLED_ShowString(2, 8, "P:");
		OLED_ShowSignedNum(2, 10, smooth_pitch, 2);
		OLED_ShowHexNum(2, 15, who_am_i, 2);
	}
}

static void ShowPage(void)
{
	if (page == 0) ShowHomePage();
	else ShowLevelPage();
}

static void AnimatePageSwitch(int8_t direction)
{
	uint8_t frame;
	int16_t x;
	for (frame = 0; frame < 4; frame++)
	{
		x = (direction > 0) ? frame * 32 : 127 - frame * 32;
		OLED_ClearBuffer();
		OLED_DrawLine(x, 8, x, 55);
		OLED_DrawLine(x - 8, 16, x + 8, 47);
		OLED_DrawLine(x + 8, 16, x - 8, 47);
		OLED_Update();
		delay_ms(18);
	}
	ShowPage();
}

int main(void)
{
	Encoder_Event event;
	uint16_t refresh_count = 0;

	SystemInit();
	LED_Init();
	Encoder_Init();
	OLED_Init();
	MyRTC_Init();
	mpu_ok = MPU6050_Init(&who_am_i);
	ShowPage();

	while (1)
	{
		event = Encoder_Scan();
		if (event == ENCODER_RIGHT)
		{
			page = (page + 1) % 2;
			AnimatePageSwitch(1);
			refresh_count = 0;
		}
		else if (event == ENCODER_LEFT)
		{
			page = (page == 0) ? 1 : 0;
			AnimatePageSwitch(-1);
			refresh_count = 0;
		}
		else if (event == ENCODER_PRESS)
		{
			led_on = !led_on;
			if (led_on) LED_ON(); else LED_OFF();
			ShowPage();
		}

		if (++refresh_count >= 20)
		{
			refresh_count = 0;
			ShowPage();
		}
		delay_ms(10);
	}
}
