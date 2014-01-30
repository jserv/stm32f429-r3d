/**
 * r3d -- 3D rendering library
 * author: Andreas Mantler (ands)
 */

#include <string.h>
#include <r3d.h>

typedef void (*r3d_primitive_rasterizer_func)(const float *in);
static void r3d_points_rasterizer(const float *in);
static void r3d_lines_rasterizer(const float *in);
static void r3d_line_strip_rasterizer(const float *in);
static void r3d_line_fan_rasterizer(const float *in);
static void r3d_triangles_rasterizer(const float *in);
static void r3d_triangle_strip_rasterizer(const float *in);
static void r3d_triangle_fan_rasterizer(const float *in);
//static void r3d_quads_rasterizer(const float *in);
//static void r3d_quad_strip_rasterizer(const float *in);

// public variables
r3d_switch_t r3d_backface_culling = R3D_DISABLE;
r3d_primitive_winding_t r3d_primitive_winding = R3D_PRIMITIVE_WINDING_CCW;
r3d_shader_t r3d_shader = {0};

// private variables
static vec2_t r3d_viewport_position = {0};
static vec2_t r3d_viewport_half_size = {0};
static int r3d_viewport_width = 0;
static int r3d_viewport_height = 0;
static float *r3d_primitive_vertex_buffer;
static uint8_t r3d_primitive_vertex_index = 0;
static r3d_primitive_rasterizer_func r3d_primitive_rasterizers[R3D_PRIMITIVE_TYPE_NUM] = {
	r3d_points_rasterizer,
	r3d_lines_rasterizer,
	r3d_line_strip_rasterizer, // line_loop: differentiation happens in r3d_draw.
	r3d_line_strip_rasterizer,
	r3d_line_fan_rasterizer,
	r3d_triangles_rasterizer,
	r3d_triangle_strip_rasterizer,
	r3d_triangle_fan_rasterizer,
	//r3d_quads_rasterizer,
	//r3d_quad_strip_rasterizer
};

void r3d_viewport(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	r3d_viewport_position.x = x0;
	r3d_viewport_position.y = y0;
	r3d_viewport_half_size.x = (float)(x1 - x0) * 0.5f;
	r3d_viewport_half_size.y = (float)(y1 - y0) * 0.5f;
	r3d_viewport_width = x1 - x0;
	r3d_viewport_height = y1 - y0;
}

void r3d_draw(const r3d_drawcall_t *drawcall)
{
	const void *vs_in;
	float vs_out[R3D_VERTEX_ELEMENTS_MAX];

	// initialize rasterizer
	float primitive_buffer[R3D_PRIMITIVE_VERTEX_BUFFER * R3D_VERTEX_ELEMENTS_MAX];
	r3d_primitive_rasterizer_func rasterizer = r3d_primitive_rasterizers[drawcall->primitive_type];
	r3d_primitive_vertex_buffer = primitive_buffer;
	r3d_primitive_vertex_index = 0;

	if (drawcall->indices == 0) {
		// rasterize vertex arrays
		vs_in = drawcall->vertices;
		const void *vs_end = vs_in + drawcall->count * drawcall->stride;
		if (drawcall->primitive_type == R3D_PRIMITIVE_TYPE_LINE_LOOP) {
			r3d_shader.vertexshader(vs_end - drawcall->stride, vs_out);
			rasterizer(vs_out);
		}
		while (vs_in != vs_end) {
			r3d_shader.vertexshader(vs_in, vs_out);
			rasterizer(vs_out);
			vs_in += drawcall->stride;
		}
	} else {
		// rasterize indexed arrays
		if (drawcall->primitive_type == R3D_PRIMITIVE_TYPE_LINE_LOOP) {
			vs_in = drawcall->vertices + drawcall->indices[drawcall->count - 1] * drawcall->stride;
			r3d_shader.vertexshader(vs_in, vs_out);
			rasterizer(vs_out);
		}
		for (uint32_t i = 0; i < drawcall->count; i++) {
			vs_in = drawcall->vertices + drawcall->indices[i] * drawcall->stride;
			r3d_shader.vertexshader(vs_in, vs_out);
			rasterizer(vs_out);
		}
	}
}

// interpolators
static inline void r3d_primitive_linear_interpolate(const float *in0, const float *in1, float *out, float x)
{
	float xr = 1.0f - x;
	for (int i = 0; i < r3d_shader.vertex_out_elements; i++)
		out[i] = in0[i] * xr + in1[i] * x;
}

static inline void r3d_primitive_barycentric_interpolate(const float *in0, const float *in1, const float *in2, float *out, float t0, float t1, float t2)
{
	for (int i = 0; i < r3d_shader.vertex_out_elements; i++)
		out[i] = in0[i] * t0 + in1[i] * t1 + in2[i] * t2;
}

// temporary buffer access
#define r3d_primitive_vertex_buffer(i) (r3d_primitive_vertex_buffer + (i) * r3d_shader.vertex_out_elements)
#define r3d_primitive_vertex_buffer_put(i, in) memcpy(r3d_primitive_vertex_buffer(i), in, r3d_shader.vertex_out_elements * sizeof(float));

static inline void r3d_fragment_rasterizer(const float *in, uint16_t x, uint16_t y)
{
	// TODO: alpha test
	float z = (in[2] - 1.0f) * -0.5f;
	if (z > r3d_get_depth(x, y)) {
		vec4_t color = r3d_shader.fragmentshader(in);
		color.r = float_clamp(color.r, 0.0f, 1.0f);
		color.g = float_clamp(color.g, 0.0f, 1.0f);
		color.b = float_clamp(color.b, 0.0f, 1.0f);
		r3d_set_pixel(x, y, z, color.rgb);
	}
}

static void r3d_points_rasterizer(const float *in)
{
	if (in[0] < -1.0f || in[0] > 1.0f || in[1] < -1.0f || in[1] > 1.0f || in[2] < -1.0f || in[2] > 1.0f)
		return;
	uint16_t x = (uint16_t)((in[0] + 1.0f) * r3d_viewport_half_size.x + r3d_viewport_position.x);
	uint16_t y = (uint16_t)((in[1] - 1.0f) * -r3d_viewport_half_size.y + r3d_viewport_position.y);
	r3d_fragment_rasterizer(in, x, y);
}

static void r3d_line_rasterizer(const float *v0, const float *v1)
{
	// bresenham
	int x0 = (int)((v0[0] + 1.0f) * r3d_viewport_half_size.x + r3d_viewport_position.x);
	int y0 = (int)((v0[1] - 1.0f) * -r3d_viewport_half_size.y + r3d_viewport_position.y);
	int x1 = (int)((v1[0] + 1.0f) * r3d_viewport_half_size.x + r3d_viewport_position.x);
	int y1 = (int)((v1[1] - 1.0f) * -r3d_viewport_half_size.y + r3d_viewport_position.y);

	int dx = x1 - x0, sx = x0 < x1 ? 1 : -1;
	int dy = y1 - y0, sy = y0 < y1 ? 1 : -1;
	if (dx < 0) dx = -dx;
	if (dy < 0) dy = -dy;
	int cur = 0, len = dx < dy ? dy : dx;
	int err = (dx > dy ? dx : -dy) / 2, e2;
	float t;
	float *vi = r3d_primitive_vertex_buffer(1);

	for (;;) {
		t = (float)(cur++) / (float)len; // TODO: incremental?
		r3d_primitive_linear_interpolate(v0, v1, vi, t);
		r3d_fragment_rasterizer(vi, x0, y0);

		if (x0 == x1 && y0 == y1) break;
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
}

static void r3d_lines_rasterizer(const float *in)
{
	if (r3d_primitive_vertex_index == 1) {
		r3d_line_rasterizer(r3d_primitive_vertex_buffer(0), in);
		r3d_primitive_vertex_index = 0;
	} else {
		r3d_primitive_vertex_buffer_put(0, in);
		r3d_primitive_vertex_index = 1;
	}
}

static void r3d_line_strip_rasterizer(const float *in)
{
	if (r3d_primitive_vertex_index == 1) {
		r3d_line_rasterizer(r3d_primitive_vertex_buffer(0), in);
		r3d_primitive_vertex_buffer_put(0, in);
	} else {
		r3d_primitive_vertex_buffer_put(0, in);
		r3d_primitive_vertex_index = 1;
	}
}

static void r3d_line_fan_rasterizer(const float *in)
{
	if (r3d_primitive_vertex_index == 1) {
		r3d_line_rasterizer(r3d_primitive_vertex_buffer(0), in);
	} else {
		r3d_primitive_vertex_buffer_put(0, in);
		r3d_primitive_vertex_index = 1;
	}
}

// screen orientations: cw vs ccw
static inline float r3d_orientation2f(const float *i0, const float *i1, const float *i2)
{
	return (i1[0] - i0[0]) * (i2[1] - i0[1]) - (i1[1] - i0[1]) * (i2[0] - i0[0]);
}

static inline int r3d_orientation2i(const int *i0, const int *i1, const int *i2)
{
	return (i1[0] - i0[0]) * (i2[1] - i0[1]) - (i1[1] - i0[1]) * (i2[0] - i0[0]);
}

// triangle front face rasterizer
static void r3d_triangle_front_rasterizer(const float *v0, const float *v1, const float *v2)
{
	int i0[2], i1[2], i2[2];
	i0[0] = (int)((v0[0] + 1.0f) * r3d_viewport_half_size.x + r3d_viewport_position.x);
	i0[1] = (int)((v0[1] - 1.0f) * -r3d_viewport_half_size.y + r3d_viewport_position.y);
	i1[0] = (int)((v1[0] + 1.0f) * r3d_viewport_half_size.x + r3d_viewport_position.x);
	i1[1] = (int)((v1[1] - 1.0f) * -r3d_viewport_half_size.y + r3d_viewport_position.y);
	i2[0] = (int)((v2[0] + 1.0f) * r3d_viewport_half_size.x + r3d_viewport_position.x);
	i2[1] = (int)((v2[1] - 1.0f) * -r3d_viewport_half_size.y + r3d_viewport_position.y);

	int minX = int_max(int_min(i0[0], int_min(i1[0], i2[0])), (int)r3d_viewport_position.x); // bounding box
	int minY = int_max(int_min(i0[1], int_min(i1[1], i2[1])), (int)r3d_viewport_position.y);
	int maxX = int_min(int_max(i0[0], int_max(i1[0], i2[0])), (int)r3d_viewport_position.x + r3d_viewport_width - 1);
	int maxY = int_min(int_max(i0[1], int_max(i1[1], i2[1])), (int)r3d_viewport_position.y + r3d_viewport_height - 1);

	int A01 = i0[1] - i1[1], B01 = i1[0] - i0[0]; // triangle setup
	int A12 = i1[1] - i2[1], B12 = i2[0] - i1[0];
	int A20 = i2[1] - i0[1], B20 = i0[0] - i2[0];

	int p[2] = { minX, minY }; // barycentric coordinates at minX/minY corner
	int w0_row = r3d_orientation2i(i1, i2, p);
	int w1_row = r3d_orientation2i(i2, i0, p);
	int w2_row = r3d_orientation2i(i0, i1, p);

	float vi[R3D_VERTEX_ELEMENTS_MAX]; // interpolated vertex

	for (p[1] = minY; p[1] <= maxY; p[1]++) {
		int w0 = w0_row; // barycentric coordinates at start of row
		int w1 = w1_row;
		int w2 = w2_row;

		for (p[0] = minX; p[0] <= maxX; p[0]++) {
			if ((w0 | w1 | w2) >= 0) { // if p is on or inside all edges, render pixel.
				float wai = 1.0f / (float)(w0 + w1 + w2);
				r3d_primitive_barycentric_interpolate(v0, v1, v2, vi, w0 * wai, w1 * wai, w2 * wai);
				r3d_fragment_rasterizer(vi, p[0], p[1]);
			}
			w0 += A12; // one step to the right
			w1 += A20;
			w2 += A01;
		}
		w0_row += B12; // one row step
		w1_row += B20;
		w2_row += B01;
	}
}

static void r3d_triangle_rasterizer(const float *v0, const float *v1, const float *v2)
{
	if (r3d_primitive_winding == R3D_PRIMITIVE_WINDING_CCW) {
		if (r3d_orientation2f(v0, v1, v2) > 0.0f)
			r3d_triangle_front_rasterizer(v0, v2, v1); // ccw front face
		else if (!r3d_backface_culling)
			r3d_triangle_front_rasterizer(v0, v1, v2); // ccw back face
	} else {
		if (r3d_orientation2f(v0, v1, v2) < 0.0f)
			r3d_triangle_front_rasterizer(v0, v1, v2); // cw front face
		else if (!r3d_backface_culling)
			r3d_triangle_front_rasterizer(v0, v2, v1); // cw back face
	}
}

static void r3d_triangles_rasterizer(const float *in)
{
	if (r3d_primitive_vertex_index == 2) {
		r3d_triangle_rasterizer(r3d_primitive_vertex_buffer(0), r3d_primitive_vertex_buffer(1), in);
		r3d_primitive_vertex_index = 0;
	} else {
		r3d_primitive_vertex_buffer_put(r3d_primitive_vertex_index, in);
		r3d_primitive_vertex_index++;
	}
}

static void r3d_triangle_strip_rasterizer(const float *in)
{
	if (r3d_primitive_vertex_index == 2) {
		r3d_triangle_rasterizer(r3d_primitive_vertex_buffer(0), r3d_primitive_vertex_buffer(1), in);
		r3d_primitive_vertex_buffer_put(r3d_primitive_vertex_index, in);
		r3d_primitive_vertex_index++;
	} else if (r3d_primitive_vertex_index == 3) {
		r3d_triangle_rasterizer(r3d_primitive_vertex_buffer(2), r3d_primitive_vertex_buffer(1), in);
		r3d_primitive_vertex_buffer_put(0, r3d_primitive_vertex_buffer(2));
		r3d_primitive_vertex_buffer_put(1, in);
		r3d_primitive_vertex_index = 2;
	} else {
		r3d_primitive_vertex_buffer_put(r3d_primitive_vertex_index, in);
		r3d_primitive_vertex_index++;
	}
}

static void r3d_triangle_fan_rasterizer(const float *in)
{
	if (r3d_primitive_vertex_index == 2) {
		r3d_triangle_rasterizer(r3d_primitive_vertex_buffer(0), r3d_primitive_vertex_buffer(1), in);
		r3d_primitive_vertex_buffer_put(1, in);
	} else {
		r3d_primitive_vertex_buffer_put(r3d_primitive_vertex_index, in);
		r3d_primitive_vertex_index++;
	}
}

/*static void r3d_quads_rasterizer(const float *in)
{

}

static void r3d_quad_strip_rasterizer(const float *in)
{

}*/


