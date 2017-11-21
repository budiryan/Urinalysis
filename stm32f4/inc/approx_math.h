#ifndef __APPROX_MATH_H
#define __APPROX_MATH_H

#include "stm32f4xx.h"

extern s16 cos_val[91];

struct Cartesian {
	s32 x, y;
};

s32 int_sin(s32 a);
s32 int_cos(s32 a);
s16 int_tan(s16 a);

s16 int_arc_sin(s16 sin_val);
s16 int_arc_cos(s16 cos_val);
s16 int_arc_tan(s32 tan_val);
s16 int_arc_tan2(s32 y, s32 x);

void xy_rotate(s32 *x, s32 *y, s32 w);

s32 p_mod(s32 dividor, s32 divisor);
s32 Abs(s32 v);
s32 s_Abs (s32 v);
s32 Sqr (s32 x);
u32 Sqrt(u32 num);
float largest_value(float numbers[], int size);
int largest_arg(float numbers[], int size);
float smallest_value(float numbers[], int size);
int smallest_arg(float numbers[], int size);
#endif		/*  __APPROX_MATH_H */
