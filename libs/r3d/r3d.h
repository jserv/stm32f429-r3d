/**
 * r3d -- 3D rendering library
 * author: Andreas Mantler (ands)
 */

#ifndef R3D_H
#define R3D_H

#include <stdint.h>
#include <r3d_math.h>

/*****************************************************************************/
// to configure by user:

// number of float elements per vertex passed to/between shaders (at least 3)
#define R3D_VERTEX_ELEMENTS_MAX 16

// sets the maximum number for *_FAN vertices.
// should be at least:
// 6 if quads are used, ?
// 4 if triangles are used,
// 2 if lines are used or
// 0 if only points are used
#define R3D_PRIMITIVE_VERTEX_BUFFER 4

// to implement:
void r3d_set_pixel(uint16_t x, uint16_t y, float z, vec3_t color);
float r3d_get_depth(uint16_t x, uint16_t y);

// end of configuration section
/*****************************************************************************/

// types
typedef uint8_t r3d_switch_t;
#define R3D_DISABLE 0
#define R3D_ENABLE  1

typedef uint8_t r3d_primitive_winding_t;
#define R3D_PRIMITIVE_WINDING_CW  0
#define R3D_PRIMITIVE_WINDING_CCW 1

typedef uint8_t r3d_primitive_type_t;
#define R3D_PRIMITIVE_TYPE_POINTS         0x00
#define R3D_PRIMITIVE_TYPE_LINES          0x01
#define R3D_PRIMITIVE_TYPE_LINE_LOOP      0x02
#define R3D_PRIMITIVE_TYPE_LINE_STRIP     0x03
#define R3D_PRIMITIVE_TYPE_LINE_FAN       0x04
#define R3D_PRIMITIVE_TYPE_TRIANGLES      0x05
#define R3D_PRIMITIVE_TYPE_TRIANGLE_STRIP 0x06
#define R3D_PRIMITIVE_TYPE_TRIANGLE_FAN   0x07
//#define R3D_PRIMITIVE_TYPE_QUADS          0x08
//#define R3D_PRIMITIVE_TYPE_QUAD_STRIP     0x09
#define R3D_PRIMITIVE_TYPE_NUM            0x08 //0x0a

typedef void (*r3d_vertexshader_func)(const void *in, float *out);
typedef vec4_t (*r3d_fragmentshader_func)(const float *in);
typedef struct {
	r3d_vertexshader_func vertexshader;
	r3d_fragmentshader_func fragmentshader;
	uint8_t vertex_out_elements; // number of floats passed from vs to fs
} r3d_shader_t;

typedef struct {
	r3d_primitive_type_t primitive_type;
	const void *vertices; // vertex buffer
	uint8_t stride; // vertex stride
	uint32_t count; // number of vertices/indices
	const uint16_t *indices; // index buffer
} r3d_drawcall_t;

typedef struct {
	uint16_t width;
	uint16_t height;
	uint16_t data[];
} r3d_texture_t;

// variables
extern r3d_switch_t r3d_backface_culling;
extern r3d_primitive_winding_t r3d_primitive_winding;
extern r3d_shader_t r3d_shader;

// functions
void r3d_viewport(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void r3d_draw(const r3d_drawcall_t *drawcall);

// texturing (inlined for speed)
static inline vec3_t r3d_texture_nearest(const r3d_texture_t *texture, vec2_t uv)
{
	int ui = (int)uv.x;
	int vi = (int)uv.y;
	if (uv.x < 0) ui--;
	if (uv.y < 0) vi--;
	uv.x -= ui;
	uv.y -= vi;
	uv.x *= texture->width - 1;
	uv.y *= texture->height - 1;
	ui = uv.x;
	vi = uv.y;
	uint16_t c = texture->data[vi * texture->width + ui];
	const float ri = 1.0f / 63488.0f, gi = 1.0f / 2016.0f, bi = 1.0f / 31.0f;
	return vec3((c & 63488) * ri, (c & 2016) * gi, (c & 31) * bi);
}

#endif
