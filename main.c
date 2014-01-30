#include "stm32f4xx.h"
#include "arm_math.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"
#include <r3dfb.h>
#include <r3d.h>
#include <string.h>
#include <stdio.h>

static char *itoa(int value, char* result, int base);

// vertex format
typedef struct {
	uint16_t x, y, z;
	uint8_t nx, ny, nz;
	uint8_t u, v;
} vertex_t;

// meshes:
#include "teapot.h"
#include "box.h"
#include "pony.h"

// textures:
#include "nyan.h"
#include "box_texture.h"
#include "twilight.h"

static const r3d_drawcall_t meshes[] = {
	{ R3D_PRIMITIVE_TYPE_TRIANGLES, vertices0,
	  sizeof(vertex_t), sizeof(vertices0) / sizeof(vertex_t), 0 },
	{ R3D_PRIMITIVE_TYPE_TRIANGLES, vertices1,
	  sizeof(vertex_t), sizeof(vertices1) / sizeof(vertex_t), 0 },
	{ R3D_PRIMITIVE_TYPE_TRIANGLES, vertices2,
	  sizeof(vertex_t), sizeof(vertices2) / sizeof(vertex_t), 0 },
};
static const r3d_primitive_winding_t windings[] = {
	R3D_PRIMITIVE_WINDING_CW,
	R3D_PRIMITIVE_WINDING_CCW,
	R3D_PRIMITIVE_WINDING_CCW
};
static const r3d_texture_t *textures[] = {
	(r3d_texture_t *) &nyan,
	(r3d_texture_t *) &box_texture,
	(r3d_texture_t *) &twilight
};

// simulation stuff
static float time = 0.0f, frametime = 0.0f;
static uint32_t fps = 0;
static uint8_t fps_str[8] = "? FPS";
static uint8_t info_str[16];
static mat4_t model, view, projection, mv, mvp;
static int mesh = 0;
static float axes[3] = {0};

// shader
typedef struct {
	vec3_t position;
	vec3_t normal;
	vec2_t uv;
} vs_to_fs_t;

static void vertex_shader(const vertex_t *in, vs_to_fs_t *out)
{
	// decode vertex
	const vec3_t pc = { 0.5f, 0.5f, 0.5f }, nc = { 1.0f, 1.0f, 1.0f };
	const float pi = 1.0f / 65535.0f, ni = 2.0f / 255.0f, uvi = 1.0f / 255.0f;
	vec3_t position = vec3_sub(vec3_mul(vec3(in->x, in->y, in->z), pi), pc);
	vec3_t normal = vec3_sub(vec3_mul(vec3(in->nx, in->ny, in->nz), ni), nc);
	vec2_t uv = vec2_mul(vec2(in->u, in->v), uvi);
	// transform vertex
	out->position = mat4_transform_position(mvp, position);
	out->normal = mat4_transform_vector(mv, normal);
	out->uv = uv;
}

static vec4_t fragment_shader(const vs_to_fs_t *in)
{
	const vec3_t E = { 0, 0, 1 };
	const vec3_t L = { -0.577350269f, 0.577350269f, 0.577350269f };
	vec3_t N = vec3_normalize(in->normal);
	vec3_t H = vec3_normalize(vec3_add(E, L));

	const float ambient = 0.05f;
	float diffuse = vec3_dot(N, L) * 0.6f;
	float specular = vec3_dot(N, H);
	specular *= specular;
	specular *= specular;
	specular *= specular;
	specular *= specular;
	specular *= specular;
	specular *= specular * 0.6f;

	vec3_t c = r3d_texture_nearest(textures[mesh], in->uv); // read texel
	return vec4(ambient + diffuse * c.r + specular,
	            ambient + diffuse * c.g + specular,
	            ambient + diffuse * c.b + specular, 1.0f);
}

static r3d_shader_t shader = {
	(r3d_vertexshader_func)vertex_shader,
	(r3d_fragmentshader_func)fragment_shader,
	sizeof(vs_to_fs_t) / sizeof(float)
};

static void init(void)
{
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);

	r3dfb_init();
	LCD_SetColors(0x0000, 0xffff);
	LCD_SetFont(&Font8x8);

	itoa(meshes[mesh].count / 3, info_str, 10);
	strcat(info_str, " tris");

	r3d_viewport(1, 1, R3DFB_PIXEL_WIDTH - 1, R3DFB_PIXEL_HEIGHT - 1);
	r3d_backface_culling = R3D_ENABLE;
	r3d_primitive_winding = windings[mesh];

	projection = mat4_perspective(60.0f, (float)R3DFB_PIXEL_WIDTH / (float)R3DFB_PIXEL_HEIGHT, 0.5f, 5.0f);
	view = mat4_lookat(vec3(0.0f, 0.25f, 1.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	L3GD20_InitTypeDef L3GD20_InitStructure;
	L3GD20_InitStructure.Power_Mode = L3GD20_MODE_ACTIVE;
	L3GD20_InitStructure.Output_DataRate = L3GD20_OUTPUT_DATARATE_1;
	L3GD20_InitStructure.Axes_Enable = L3GD20_AXES_ENABLE;
	L3GD20_InitStructure.Band_Width = L3GD20_BANDWIDTH_4;
	L3GD20_InitStructure.BlockData_Update = L3GD20_BlockDataUpdate_Continous;
	L3GD20_InitStructure.Endianness = L3GD20_BLE_LSB;
	L3GD20_InitStructure.Full_Scale = L3GD20_FULLSCALE_250;
	L3GD20_Init(&L3GD20_InitStructure);

	L3GD20_FilterConfigTypeDef L3GD20_FilterStructure;
	L3GD20_FilterStructure.HighPassFilter_Mode_Selection = L3GD20_HPM_NORMAL_MODE_RES;
	L3GD20_FilterStructure.HighPassFilter_CutOff_Frequency = L3GD20_HPFCF_0;
	L3GD20_FilterConfig(&L3GD20_FilterStructure);
	L3GD20_FilterCmd(L3GD20_HIGHPASSFILTER_ENABLE);
}

static void update(void)
{
	uint8_t tmp[6] = {0};
	int16_t a[3] = {0};
	uint8_t tmpreg = 0;

	L3GD20_Read(&tmpreg, L3GD20_CTRL_REG4_ADDR, 1);
	L3GD20_Read(tmp, L3GD20_OUT_X_L_ADDR, 6);

	/* check in the control register 4 the data alignment (Big Endian or Little Endian)*/
	if (!(tmpreg & 0x40)) {
		for (int i = 0; i < 3; i++)
			a[i] = (int16_t)(((uint16_t)tmp[2 * i + 1] << 8) | (uint16_t)tmp[2 * i]);
	} else {
		for (int i = 0; i < 3; i++)
			a[i] = (int16_t)(((uint16_t)tmp[2 * i] << 8) | (uint16_t)tmp[2 * i + 1]);
	}

	if (STM_EVAL_PBGetState(BUTTON_USER)) {
		mesh = (mesh + 1) % 3; // next mesh
		r3d_primitive_winding = windings[mesh];
		itoa(meshes[mesh].count / 3, info_str, 10);
		strcat(info_str, " tris");
		while (STM_EVAL_PBGetState(BUTTON_USER)); // debounce
	}

	float delta = frametime - time;
	frametime = time; // make frametime a consistent time value during the frames
	for (int i = 0; i < 3; i++)
		axes[i] += (float)a[i] * delta / 114.285f;
	if (axes[0] < 0) axes[0] = 0;
	if (axes[0] > 180) axes[0] = 180;
	model = mat4_mul(
	            mat4_rotation(axes[0], vec3(1.0f, 0.0f, 0.0f)),
	            mat4_rotation(axes[1], vec3(0.0f, 1.0f, 0.0f)));
	if (mesh == 1) model = mat4_mul(model, mat4_scaling(vec3(0.5f, 0.5f, 0.5f)));
	mv = mat4_mul(view, model);
	mvp = mat4_mul(projection, mv);
}

static void render(void)
{
	r3dfb_clear();

	r3d_shader = shader;
	r3d_draw(&meshes[mesh]);

	LCD_DisplayStringLine(LCD_LINE_1, info_str);
	LCD_DisplayStringLine(LCD_LINE_3, fps_str);

	char str[16] = "X: ";
	itoa(axes[0], str + 3, 10);
	LCD_DisplayStringLine(LCD_LINE_5, str);
	str[0] = 'Y';
	itoa(axes[1], str + 3, 10);
	LCD_DisplayStringLine(LCD_LINE_6, str);
	str[0] = 'Z';
	itoa(axes[2], str + 3, 10);
	LCD_DisplayStringLine(LCD_LINE_7, str);

	r3dfb_swap_buffers();
	fps++;
}

int main(void)
{
	SysTick_Config(SystemCoreClock / 100); // SysTick event each 10ms
	init();

	while (1) {
		update();
		render();
	}
}

static char* itoa(int value, char* result, int base)
{
	if (base < 2 || base > 36) {
		*result = '\0';
		return result;
	}
	char *ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while (value);

	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while (ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr-- = *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

uint32_t fps_countdown = 100;
void OnSysTick(void)
{
	if (--fps_countdown == 0) {
		itoa(fps, fps_str, 10);
		strcat(fps_str, " FPS");
		fps = 0;
		fps_countdown = 100;
	}
	time += 0.01f;
}

uint32_t L3GD20_TIMEOUT_UserCallback(void)
{
	return 0;
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
	/* printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	while (1) { }
}
#endif
