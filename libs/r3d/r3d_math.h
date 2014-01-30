/**
 * r3d -- 3D rendering library
 * author: Andreas Mantler (ands)
 */

#ifndef R3D_MATH_H
#define R3D_MATH_H

#if defined (ARM_MATH_CM4) || (ARM_MATH_CM3) || (ARM_MATH_CM0)
#define R3D_ARM_MATH
#include "arm_math.h"
#else
#include <math.h>
#endif

#define float_pi		3.1415926535897932f
#define float_pi_over180	0.017453293f

typedef struct {
	union {
		float v[2];
		struct {
			float x, y;
		};
		struct {
			float r, g;
		};
	};
} vec2_t;

typedef struct {
	union {
		float v[3];
		struct {
			float x, y, z;
		};
		struct {
			float r, g, b;
		};
		vec2_t xy;
	};
} vec3_t;

typedef struct {
	union {
		float v[4];
		struct {
			float x, y, z, w;
		};
		struct {
			float r, g, b, a;
		};
		vec2_t xy;
		vec3_t xyz;
		vec3_t rgb;
	};
} vec4_t;

typedef struct {
	union {
		float m[16];
		struct {
			float m00, m10, m20, m30,
			      m01, m11, m21, m31,
			      m02, m12, m22, m32,
			      m03, m13, m23, m33;
		};
		vec4_t c[4];
	};
} mat4_t;

static inline int int_min(int a, int b)
{
	return a < b ? a : b;
}
static inline int int_max(int a, int b)
{
	return a < b ? b : a;
}
static inline int int_clamp(int value, int min, int max)
{
	return value < min ? min : (value < max ? value : max);
}

static inline float float_min(float a, float b)
{
	return a < b ? a : b;
}
static inline float float_max(float a, float b)
{
	return a < b ? b : a;
}
static inline float float_clamp(float value, float min, float max)
{
	return value < min ? min : (value < max ? value : max);
}

static inline vec2_t vec2(float x, float y)
{
	vec2_t r = { x, y };
	return r;
}
static inline vec2_t vec2_add(vec2_t v0, vec2_t v1)
{
	vec2_t r = { v0.x + v1.x, v0.y + v1.y };
	return r;
}
static inline vec2_t vec2_sub(vec2_t v0, vec2_t v1)
{
	vec2_t r = { v0.x - v1.x, v0.y - v1.y };
	return r;
}
static inline vec2_t vec2_mul(vec2_t v, float f)
{
	vec2_t r = { v.x * f, v.y * f };
	return r;
}
static inline vec2_t vec2_div(vec2_t v, float f)
{
	float fi = 1.0f / f;
	vec2_t r = { v.x * fi, v.y * fi };
	return r;
}
static inline float vec2_dot(vec2_t v0, vec2_t v1)
{
	return v0.x * v1.x + v0.y * v1.y;
}
static inline float vec2_cross(vec2_t v0, vec2_t v1)
{
	return v0.x * v1.y - v0.y * v1.x;
}
static inline float vec2_length(vec2_t v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}
static inline vec2_t vec2_normalize(vec2_t v)
{
	float fi = 1.0f / sqrtf(v.x * v.x + v.y * v.y);
	vec2_t r = { v.x * fi, v.y * fi };
	return r;
}

static inline vec3_t vec3(float x, float y, float z)
{
	vec3_t r = { x, y, z };
	return r;
}
static inline vec3_t vec3_2(vec2_t xy, float z)
{
	vec3_t r = { xy.x, xy.y, z };
	return r;
}
static inline vec3_t vec3_add(vec3_t v0, vec3_t v1)
{
	vec3_t r = { v0.x + v1.x, v0.y + v1.y, v0.z + v1.z };
	return r;
}
static inline vec3_t vec3_sub(vec3_t v0, vec3_t v1)
{
	vec3_t r = { v0.x - v1.x, v0.y - v1.y, v0.z - v1.z };
	return r;
}
static inline vec3_t vec3_mul(vec3_t v, float f)
{
	vec3_t r = { v.x * f, v.y * f, v.z * f };
	return r;
}
static inline vec3_t vec3_div(vec3_t v, float f)
{
	float fi = 1.0f / f;
	vec3_t r = { v.x * fi, v.y * fi, v.z * fi };
	return r;
}
static inline float vec3_dot(vec3_t v0, vec3_t v1)
{
	return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}
static inline vec3_t vec3_cross(vec3_t v0, vec3_t v1)
{
	vec3_t r = { v0.y * v1.z - v0.z * v1.y, v0.z * v1.x - v0.x * v1.z, v0.x * v1.y - v0.y * v1.x };
	return r;
}
static inline float vec3_length(vec3_t v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}
static inline vec3_t vec3_normalize(vec3_t v)
{
	float fi = 1.0f / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	vec3_t r = { v.x * fi, v.y * fi, v.z * fi };
	return r;
}

static inline vec4_t vec4(float x, float y, float z, float w)
{
	vec4_t r = { x, y, z, w };
	return r;
}
static inline vec4_t vec4_2(vec2_t xy, float z, float w)
{
	vec4_t r = { xy.x, xy.y, z, w };
	return r;
}
static inline vec4_t vec4_3(vec3_t xyz, float w)
{
	vec4_t r = { xyz.x, xyz.y, xyz.z, w };
	return r;
}
static inline vec4_t vec4_add(vec4_t v0, vec4_t v1)
{
	vec4_t r = { v0.x + v1.x, v0.y + v1.y, v0.z + v1.z, v0.w + v1.w };
	return r;
}
static inline vec4_t vec4_sub(vec4_t v0, vec4_t v1)
{
	vec4_t r = { v0.x - v1.x, v0.y - v1.y, v0.z - v1.z, v0.w - v1.w };
	return r;
}
static inline vec4_t vec4_mul(vec4_t v, float f)
{
	vec4_t r = { v.x * f, v.y * f, v.z * f, v.w * f };
	return r;
}
static inline vec4_t vec4_div(vec4_t v, float f)
{
	float fi = 1.0f / f;
	vec4_t r = { v.x * fi, v.y * fi, v.z * fi, v.w * fi };
	return r;
}
static inline float vec4_dot(vec4_t v0, vec4_t v1)
{
	return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z + v0.w * v1.w;
}
static inline vec4_t vec4_cross(vec4_t v0, vec4_t v1)
{
	return vec4_3(vec3_cross(v0.xyz, v1.xyz), (v0.w + v1.w) * 0.5f);    // USUALLY FAIL
}
static inline float vec4_length(vec4_t v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}
static inline vec4_t vec4_normalize(vec4_t v)
{
	float fi = 1.0f / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	vec4_t r = { v.x * fi, v.y * fi, v.z * fi, v.w * fi };
	return r;
}

static inline mat4_t mat4(float m00, float m10, float m20, float m30, float m01, float m11, float m21, float m31, float m02, float m12, float m22, float m32, float m03, float m13, float m23, float m33)
{
	mat4_t m = {
		m00, m10, m20, m30,
		m01, m11, m21, m31,
		m02, m12, m22, m32,
		m03, m13, m23, m33
	};
	return m;
}

static inline mat4_t mat4_identity()
{
	mat4_t m = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	return m;
}

static inline mat4_t mat4_ortho(float left, float right,
                                float bottom, float top,
                                float near, float far)
{
	float rl = right - left;
	float tx = - (right + left) / rl;
	float tb = top - bottom;
	float ty = - (top + bottom) / tb;
	float fn = far - near;
	float tz = - (far + near) / fn;
	mat4_t m = {
		2.0f / rl, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / tb, 0.0f, 0.0f,
		0.0f, 0.0f, -2.0f / fn, 0.0f,
		tx, ty, tz, 1.0f
	};
	return m;
}

static inline mat4_t mat4_perspective(float fovy, float aspect,
                                      float near, float far)
{
	float a = fovy * float_pi_over180 * 0.5f;
#ifdef R3D_ARM_MATH
	float f = arm_cos_f32(a) / arm_sin_f32(a);
#else
	float f = 1.0f / tanf(a);
#endif
	float nfi = 1.0f / (near - far);
	mat4_t m = {
		f / aspect, 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, (far + near) * nfi, -1.0f,
		0.0f, 0.0f, 2.0f * far * near * nfi, 0.0f
	};
	return m;
}

static inline mat4_t mat4_mul(mat4_t m0, mat4_t m1)
{
	mat4_t m = {
		m0.m00*m1.m00 + m0.m01*m1.m10 + m0.m02*m1.m20 + m0.m03 * m1.m30,
		m0.m10*m1.m00 + m0.m11*m1.m10 + m0.m12*m1.m20 + m0.m13 * m1.m30,
		m0.m20*m1.m00 + m0.m21*m1.m10 + m0.m22*m1.m20 + m0.m23 * m1.m30,
		m0.m30*m1.m00 + m0.m31*m1.m10 + m0.m32*m1.m20 + m0.m33 * m1.m30,

		m0.m00*m1.m01 + m0.m01*m1.m11 + m0.m02*m1.m21 + m0.m03 * m1.m31,
		m0.m10*m1.m01 + m0.m11*m1.m11 + m0.m12*m1.m21 + m0.m13 * m1.m31,
		m0.m20*m1.m01 + m0.m21*m1.m11 + m0.m22*m1.m21 + m0.m23 * m1.m31,
		m0.m30*m1.m01 + m0.m31*m1.m11 + m0.m32*m1.m21 + m0.m33 * m1.m31,

		m0.m00*m1.m02 + m0.m01*m1.m12 + m0.m02*m1.m22 + m0.m03 * m1.m32,
		m0.m10*m1.m02 + m0.m11*m1.m12 + m0.m12*m1.m22 + m0.m13 * m1.m32,
		m0.m20*m1.m02 + m0.m21*m1.m12 + m0.m22*m1.m22 + m0.m23 * m1.m32,
		m0.m30*m1.m02 + m0.m31*m1.m12 + m0.m32*m1.m22 + m0.m33 * m1.m32,

		m0.m00*m1.m03 + m0.m01*m1.m13 + m0.m02*m1.m23 + m0.m03 * m1.m33,
		m0.m10*m1.m03 + m0.m11*m1.m13 + m0.m12*m1.m23 + m0.m13 * m1.m33,
		m0.m20*m1.m03 + m0.m21*m1.m13 + m0.m22*m1.m23 + m0.m23 * m1.m33,
		m0.m30*m1.m03 + m0.m31*m1.m13 + m0.m32*m1.m23 + m0.m33*m1.m33
	};
	return m;
}

static inline mat4_t mat4_translation(vec3_t v)
{
	mat4_t m = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		v.x, v.y, v.z, 1.0f
	};
	return m;
}

static inline mat4_t mat4_lookat(vec3_t eye, vec3_t center, vec3_t up)
{
	vec3_t f = vec3_normalize(vec3_sub(center, eye));
	vec3_t s = vec3_cross(f, vec3_normalize(up));
	vec3_t u = vec3_cross(vec3_normalize(s), f);

	mat4_t m = {
		s.x, u.x, -f.x, 0.0f,
		s.y, u.y, -f.y, 0.0f,
		s.z, u.z, -f.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	return mat4_mul(m, mat4_translation(vec3_mul(eye, -1.0f)));
}

static inline mat4_t mat4_scaling(vec3_t v)
{
	mat4_t m = {
		v.x, 0.0f, 0.0f, 0.0f,
		0.0f, v.y, 0.0f, 0.0f,
		0.0f, 0.0f, v.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	return m;
}

static inline mat4_t mat4_rotation(float angle, vec3_t axis)
{
#ifdef R3D_ARM_MATH
	float c, s;
	const float i360 = 1.0f / 360.0f;
	int a = (int)(angle * i360);
	if (angle < 0.0f) a--;
	arm_sin_cos_f32(angle - (a * 360.0f) - 180.0f, &c, &s); // accepts [-180, 180]
	// = arm_sin_cos_f32(fmod(angle, 360) - 180, &c, &s)
	c *= -1.0f;
	s *= -1.0f; // we're 180° off. correct this
#else
	angle *= float_pi_over180;
	float c = cosf(angle);
	float s = sinf(angle);
#endif
	float c2 = 1.0f - c;
	axis = vec3_normalize(axis);
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;

	mat4_t m = {
		x*x*c2 + c, y*x*c2 + z * s, x*z*c2 - y * s, 0.0f,
		x*y*c2 - z * s, y*y*c2 + c, y*z*c2 + x * s, 0.0f,
		x*z*c2 + y * s, y*z*c2 - x * s, z*z*c2 + c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	return m;
}

static inline vec4_t mat4_transform(mat4_t m, vec4_t v)
{
	vec4_t r = {
		m.m00*v.x + m.m01*v.y + m.m02*v.z + m.m03 * v.w,
		m.m10*v.x + m.m11*v.y + m.m12*v.z + m.m13 * v.w,
		m.m20*v.x + m.m21*v.y + m.m22*v.z + m.m23 * v.w,
		m.m30*v.x + m.m31*v.y + m.m32*v.z + m.m33*v.w
	};
	return r;
}

static inline vec3_t mat4_transform_position(mat4_t m, vec3_t v)
{
	float fi = 1.0f / (m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33);
	vec3_t r = {
		(m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03) * fi,
		(m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13) * fi,
		(m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23) * fi
	};
	return r;
}

static inline vec3_t mat4_transform_vector(mat4_t m, vec3_t v)
{
	vec3_t r = {
		m.m00*v.x + m.m01*v.y + m.m02 * v.z,
		m.m10*v.x + m.m11*v.y + m.m12 * v.z,
		m.m20*v.x + m.m21*v.y + m.m22*v.z
	};
	return r;
}

static inline mat4_t mat4_invert(mat4_t m)
{
	mat4_t mi = {
		m.m11*m.m22*m.m33 + m.m12*m.m23*m.m31 + m.m13*m.m21*m.m32 - m.m11*m.m23*m.m32 - m.m12*m.m21*m.m33 - m.m13*m.m22 * m.m31,
		m.m10*m.m23*m.m32 + m.m12*m.m20*m.m33 + m.m13*m.m22*m.m30 - m.m10*m.m22*m.m33 - m.m12*m.m23*m.m30 - m.m13*m.m20 * m.m32,
		m.m10*m.m21*m.m33 + m.m11*m.m23*m.m30 + m.m13*m.m20*m.m31 - m.m10*m.m23*m.m31 - m.m11*m.m20*m.m33 - m.m13*m.m21 * m.m30,
		m.m10*m.m22*m.m31 + m.m11*m.m20*m.m32 + m.m12*m.m21*m.m30 - m.m10*m.m21*m.m32 - m.m11*m.m22*m.m30 - m.m12*m.m20 * m.m31,
		m.m01*m.m23*m.m32 + m.m02*m.m21*m.m33 + m.m03*m.m22*m.m31 - m.m01*m.m22*m.m33 - m.m02*m.m23*m.m31 - m.m03*m.m21 * m.m32,
		m.m00*m.m22*m.m33 + m.m02*m.m23*m.m30 + m.m03*m.m20*m.m32 - m.m00*m.m23*m.m32 - m.m02*m.m20*m.m33 - m.m03*m.m22 * m.m30,
		m.m00*m.m23*m.m31 + m.m01*m.m20*m.m33 + m.m03*m.m31*m.m30 - m.m00*m.m21*m.m33 - m.m01*m.m23*m.m30 - m.m03*m.m20 * m.m31,
		m.m00*m.m21*m.m32 + m.m01*m.m22*m.m30 + m.m02*m.m20*m.m31 - m.m00*m.m22*m.m31 - m.m01*m.m20*m.m32 - m.m02*m.m21 * m.m30,
		m.m01*m.m12*m.m33 + m.m02*m.m13*m.m31 + m.m03*m.m11*m.m32 - m.m01*m.m13*m.m32 - m.m02*m.m11*m.m33 - m.m03*m.m12 * m.m31,
		m.m00*m.m13*m.m32 + m.m02*m.m10*m.m33 + m.m03*m.m12*m.m30 - m.m00*m.m12*m.m33 - m.m02*m.m13*m.m30 - m.m03*m.m10 * m.m32,
		m.m00*m.m11*m.m33 + m.m01*m.m13*m.m30 + m.m03*m.m10*m.m31 - m.m00*m.m13*m.m31 - m.m01*m.m10*m.m33 - m.m03*m.m11 * m.m30,
		m.m00*m.m12*m.m31 + m.m01*m.m10*m.m32 + m.m02*m.m11*m.m30 - m.m00*m.m11*m.m32 - m.m01*m.m12*m.m30 - m.m02*m.m10 * m.m31,
		m.m01*m.m13*m.m22 + m.m02*m.m11*m.m23 + m.m03*m.m12*m.m21 - m.m01*m.m12*m.m23 - m.m02*m.m13*m.m21 - m.m03*m.m11 * m.m22,
		m.m00*m.m12*m.m23 + m.m02*m.m13*m.m20 + m.m03*m.m10*m.m22 - m.m00*m.m13*m.m22 - m.m02*m.m10*m.m23 - m.m03*m.m12 * m.m20,
		m.m00*m.m13*m.m21 + m.m01*m.m10*m.m23 + m.m03*m.m11*m.m20 - m.m00*m.m11*m.m23 - m.m01*m.m13*m.m20 - m.m03*m.m10 * m.m21,
		m.m00*m.m11*m.m22 + m.m01*m.m12*m.m20 + m.m02*m.m10*m.m21 - m.m00*m.m12*m.m21 - m.m01*m.m10*m.m22 - m.m02*m.m11 * m.m20,
	};
	return mi;
}

static inline mat4_t mat4_transpose(mat4_t m)
{
	mat4_t mt = {
		m.m00, m.m01, m.m02, m.m03,
		m.m10, m.m11, m.m12, m.m13,
		m.m20, m.m21, m.m22, m.m23,
		m.m30, m.m31, m.m32, m.m33
	};
	return mt;
}

#endif
