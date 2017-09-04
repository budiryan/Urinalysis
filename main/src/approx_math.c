#include "approx_math.h"

s16 tan_val[91] = {		//scale 100
0,2,3,5,7,9,11,12,14,16,18,19,21,23,25,27,29,31,32,34,36,38,40,42,
45,47,49,51,53,55,58,60,62,65,67,70,73,75,78,81,84,87,90,93,97,100,104,107,
111,115,119,123,128,133,138,143,148,154,160,166,173,180,188,196,205,214,225,236,248,261,275,290,
308,327,349,373,401,433,470,514,567,631,712,814,951,1143,1430,1908,2864,5729};

s16 cos_val[91] = {
10000,9998,9994,9986,9976,9962,9945,9925,9903,9877,9848,9816,9781,9744,9703,9659,9613,9563,9511,
9455,9397,9336,9272,9205,9135,9063,8988,8910,8829,8746,8660,8572,8480,8387,8290,8192,8090,7986,
7880,7771,7660,7547,7431,7314,7193,7071,6947,6820,6691,6561,6428,6293,6157,6018,5878,5736,5592,
5446,5299,5150,5000,4848,4695,4540,4384,4226,4067,3907,3746,3584,3420,3256,3090,2924,2756,2588,
2419,2250,2079,1908,1736,1564,1392,1219,1045,872,698,523,349,175,0};





/**
  * @brief  Approximation of sin function
  * @param  a: angle in degree scaled by 10 (0-3600)
  * @retval sin(a) scaled by 10000 (0-10000)
  */
s32 int_sin(s32 a)
{
	s16 ta;
	while(a < 0)
		a += 3600;
	a = a % 3600;
	ta = a;
	if (a > 2700) {
		a = 3600-a;
	} else if (a > 1800) {
		a = a-1800;
	} else if (a > 900) {
		a = 1800-a;
	}
	a = cos_val[90-(a/10)] + (((a%10)*cos_val[a/10]+286)/573);
	return ta > 1800 ? -a : a;
}

/**
  * @brief  Approximation of cos function
  * @param  a: angle in degree scaled by 10 (0-3600)
  * @retval cos(a) scaled by 10000 (0-10000)
  */
s32 int_cos(s32 a)
{
	return int_sin(900-a);
}

/**
  * @brief  Approximation of tan function
  * @param  a: angle in degree WITHOUT scaling
  * @retval tan(a) scaled by 100
  */
s16 int_tan(s16 a)
{
	u8 neg = 0;
	if (a < 0) {
		neg = 1;
		a = -a;
	}
	while (a < 0)
		a += 180;
	a = a % 180;
	if (a > 90) {
		return neg ? tan_val[180-a] : -tan_val[180-a];
	} else if (a < 90) {
		return neg ? -tan_val[a] : tan_val[a];
	} else {
		return -1;
	}
}

/**
  * @brief  Quick asin function approximation with 
  * @param  sin_val: input scaled by 10000 (-10000-10000)
  * @retval asin(sin_val) in degree WITHOUT scaling (-90-90)
  */
s16 int_arc_sin(s16 sin_val)
{
	s32 search_angle = 0;

	s16 lower_bound = 0, upper_bound = 900;
	s16 interval = 450, index = 0;
	u8 neg_val = 0;


	if (sin_val < 0)	neg_val = 1;
	sin_val = Abs(sin_val);

	while (1)
	{
		index = lower_bound + interval;
		search_angle = int_sin(index);

		if (interval == 0 || sin_val == search_angle)
		{
			index = (index+4)/10;
			break;
		}
		else if (sin_val < search_angle)
			upper_bound = index;
		else//						(sin_val > search_angle)
			lower_bound = index;
		interval = (upper_bound - lower_bound) / 2;

	}

	if (neg_val)	return -index;
	else			return index;
}


/**
  * @brief  Approximation of acos function
  * @param  cos_val: input scaled by 10000 (0-10000)
  * @retval acos(cos_val) in degree WITHOUT scaling
  */
s16 int_arc_cos(s16 cos_val)
{
	return 90-int_arc_sin(cos_val);
}

/**
  * @brief  Approximation of atan function
  * @param  tan_val: input scaled by 100
  * @retval atan(y/x) in degree from -89 to 90 (ignore quarter)
  */
s16 int_arc_tan(s32 tan_val)
{
	s16 angle = 0;
	s16 pre_tan = 0;
	s16 cur_tan = 0;
	if (tan_val >= 0) {
		while (angle < 90) {
			pre_tan = cur_tan;
			cur_tan = int_tan(angle);
			if (tan_val <= cur_tan && tan_val >= pre_tan) break;
			angle++;
		}
	} else {
		while (angle >= -89) {
			pre_tan = cur_tan;
			cur_tan = int_tan(angle);
			if (tan_val >= cur_tan && tan_val <= pre_tan) break;
			angle--;
		}
	}
	return angle;
}

/**
  * @brief  Approximation of atan2 function
  * @param  y: input y
  * @param  x: input x
  * @retval atan(y/x) in degree from 0 to 359 WITHOUT scaling (correct quarter)
  */
s16 int_arc_tan2(s32 y, s32 x)
{
	if (x == 0) {
		if (y < 0)
			return -90;
		else if (y == 0)
			return 0;
		else
			return 90;
	} else if (y == 0) {
		return x < 0 ? 180 : 0;
	} else if (x < 0) {
		return 180+int_arc_tan((s32)(100*y+x/2)/x);
	} else {
		return int_arc_tan((s32)(100*y+x/2)/x);
	}
}

/**
  * @brief  Matrix rotation
  * @param  x-coor (Pass by REF), y-coor (Pass by REF), angle to rotate (anti-clockwise)
  * @retval none
  */
void xy_rotate(s32 *x, s32 *y, s32 w) {
	s32 new_x = int_cos(w) * *x - int_sin(w) * *y;
	s32 new_y = int_sin(w) * *x + int_cos(w) * *y;
	*x = new_x / 10000;
	*y = new_y / 10000;
}

/**
  * @brief A proper modulus (result is always positive, which ((-n) % p) =/= -(n % p)
  * @param dividor: n
  * @param divisor: p
  * @retval The modulus (n % p)
  * @example 24 % 7 return 3, -24 % 7 returns 4
  */
s32 p_mod(s32 dividor, s32 divisor) {
		while (dividor < 0) {dividor += divisor;}
		return dividor % divisor;
}

/**
  * @brief  Absolute value function
  * @param  v: input
  * @retval absolute value |v|
  */
s32 Abs(s32 v)
{
	return v < 0 ? -v : v;
}

/**
  * @brief Square of x
  * @param x: input
  * @retval x^2 
  */
s32 Sqr (s32 x) {
	return x * x;
}

/**
  * @brief  Sqrt calculation using binary search, run-time = O(log n)
  * @param  num: the integer inside the root
  * @retval square root of num
  */
u32 Sqrt(u32 num) {
	s32 upper_sqrt = 1, lower_sqrt, range, tmp_sqrt, tmp_sqr;
	//u8 i = 0;
	if (num == 0) return 0;

	while (Sqr(upper_sqrt) <= num) {
		if ((upper_sqrt <<= 1) < 0) {
			return 0;	// Overflow happened
		}
	}
  
	lower_sqrt = upper_sqrt >> 1;
	range = upper_sqrt - lower_sqrt;
	do {
		tmp_sqrt = lower_sqrt + (range >>= 1);
		tmp_sqr = Sqr(tmp_sqrt);
		if (tmp_sqr < num) {lower_sqrt = tmp_sqrt;}
		else if (tmp_sqr > num) {upper_sqrt = tmp_sqrt;}
		else {return tmp_sqrt;}
	} while (range >= 2);

  
	if (num < (Sqr(lower_sqrt) + Sqr(upper_sqrt)) >> 1) {
		return lower_sqrt;
	} else {
		return upper_sqrt;
	}
}

