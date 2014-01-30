/**
 * r3dfb -- 3D rendering library framebuffer for STM32F429-DISCOVERY
 * author: Andreas Mantler (ands)
 */

#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "r3d.h"
#include "r3dfb.h"

static uint32_t r3dfb_front_buffer = R3DFB_BUFFER0;
static uint32_t r3dfb_back_buffer = R3DFB_BUFFER1;

void r3dfb_init(void)
{
	LCD_Init();

	LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct;
	LTDC_Layer_InitStruct.LTDC_HorizontalStart = 30;
	LTDC_Layer_InitStruct.LTDC_HorizontalStop = (R3DFB_PIXEL_WIDTH + 30 - 1);
	LTDC_Layer_InitStruct.LTDC_VerticalStart = 4;
	LTDC_Layer_InitStruct.LTDC_VerticalStop = (R3DFB_PIXEL_HEIGHT + 4 - 1);
	LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
	LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255;
	LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;
	LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;
	LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;
	LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;
	LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((R3DFB_PIXEL_WIDTH * 2) + 3);
	LTDC_Layer_InitStruct.LTDC_CFBPitch = (R3DFB_PIXEL_WIDTH * 2);
	LTDC_Layer_InitStruct.LTDC_CFBLineNumber = R3DFB_PIXEL_HEIGHT;
	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = r3dfb_front_buffer;
	LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

	LTDC_LayerCmd(LTDC_Layer1, ENABLE);
	LTDC_LayerCmd(LTDC_Layer2, DISABLE);

	LTDC_ReloadConfig(LTDC_IMReload);

	LTDC_Cmd(ENABLE);
}

void r3dfb_clear(void)
{
	memset((void *) R3DFB_DEPTH_BUFFER, 0, R3DFB_DEPTH_BUFFER_SIZE);
	memset((void *) r3dfb_back_buffer, 0xff, R3DFB_COLOR_BUFFER_SIZE);
}

void r3dfb_swap_buffers(void)
{
	uint32_t tmp = r3dfb_front_buffer;
	r3dfb_front_buffer = r3dfb_back_buffer;
	r3dfb_back_buffer = tmp;

	// hack for font rendering
	LCD_SetLayer(r3dfb_back_buffer == R3DFB_BUFFER0 ?
	             LCD_BACKGROUND_LAYER : LCD_FOREGROUND_LAYER);

	LTDC_Cmd(DISABLE);
	LTDC_LayerAddress(LTDC_Layer1, r3dfb_front_buffer);
	LTDC_ReloadConfig(LTDC_IMReload);
	LTDC_Cmd(ENABLE);
}

// r3d interface
void r3d_set_pixel(uint16_t x, uint16_t y, float z, vec3_t color)
{
	// convert color to RGB565
	uint16_t c = ((uint16_t)(color.r * 63488.0f) & 63488) |
	             ((uint16_t)(color.g * 2016.0f) & 2016) |
	             ((uint16_t)(color.b * 31.0f) & 31);
	// set color + depth
	const uint32_t offset = 2 * (x + R3DFB_PIXEL_WIDTH * y);
	*(__IO uint16_t *) (r3dfb_back_buffer + offset) = c;
	*(__IO uint16_t *) (R3DFB_DEPTH_BUFFER + offset) = z * 65535.0f;
}

float r3d_get_depth(uint16_t x, uint16_t y)
{
	const float uint16to1f = 1.0f / 65535.0f;
	const uint32_t offset = 2 * (x + R3DFB_PIXEL_WIDTH * y);
	return (*(__IO uint16_t *) (R3DFB_DEPTH_BUFFER + offset)) * uint16to1f;
}

