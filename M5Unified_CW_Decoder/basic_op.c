/*==============================================================================
* @brief	Basic Operators compatible with ITU-T G.191 Software tools STL.
*
*	@author		Sho Ikeda (JJ1LFO@jarl.com)
*	@Date		Sep. 5th 2017
*
*	@copyright
*		Copyright (c) 2017 Sho Ikeda.
*
*		MIT License
*
*		Permission is hereby granted, free of charge, to any person obtaining a copy
*		of this software and associated documentation files (the "Software"), to deal
*		in the Software without restriction, including without limitation the rights
*		to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*		copies of the Software, and to permit persons to whom the Software is
*		furnished to do so, subject to the following conditions:
*
*		The above copyright notice and this permission notice shall be included in all
*		copies or substantial portions of the Software.
*
*		THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*		IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*		FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*		AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*		LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*		OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*		SOFTWARE.
*
*===============================================================================*/
#include <assert.h>

#include "basic_op.h"


bool Overflow = false;
int Carry = 0;

/**
* @brief	Limit the 32 bit input to the range of a 16 bit word. 
*
* @param[in] L_v1		32 bit input.
*						32 bit long signed integer(int32_t) whose value
*						falls in the range : 0x8000 0000 <= L_v1 <= 0x7fff ffff. 
*
* @return	16 bit short signed integer(int16_t) whose value falls
*			in the range : 0xffff 8000 <= return value <= 0x0000 7fff.
*/
int16_t saturate(int32_t L_v1)
{
	if(INT16_MAX < L_v1) {
		Overflow = true;
		return INT16_MAX;
	} else if(L_v1 < INT16_MIN) {
		Overflow = true;
		return INT16_MIN;
	} else {
		return extract_l(L_v1);
	}
}


/**
* @brief	Performs the addition (v1+v2) with overflow control and
*			saturation; the 16 bit result is set at +32767 when overflow
*			occurs or at -32768 when underflow occurs.
*
* @param[in] v1	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param[in] v2	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0xffff 8000 <= return value <= 0x0000 7fff.
*/
int16_t add(int16_t v1, int16_t v2)
{
	return saturate((int32_t)v1 + v2);
}


/**
* @brief	Performs the subtraction (v1-v2) with overflow control and
*			saturation; the 16 bit result is set at +32767 when overflow
*			occurs or at -32768 when underflow occurs.
*
* @param[in] v1	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param[in] v2	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0xffff 8000 <= return value <= 0x0000 7fff.
*/
int16_t sub(int16_t v1, int16_t v2)
{
	return saturate((int32_t)v1 - v2);
}


/**
* @brief	Arithmetically shift the 16 bit input v1 left v2 positions.
*			Zero fill the v2 LSB of the result. If v2 is negative,
*			arithmetically shift v1 right by -v2 with sign extension.
*			Saturate the result in case of underflows or overflows.
*
* @param[in] v1		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param[in] v2		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0xffff 8000 <= var_out <= 0x0000 7fff.
*/
int16_t shl(int16_t v1, int16_t v2)
{
	if(v2 < 0) {
		if(v2 < -16) v2 = -16;
		v2 = -v2;
		return shr(v1, v2);
	} else {
		int32_t result;
		result = (int32_t)v1 *((int32_t)1<< v2);

		if((v2 > 15 && v1 != 0) || (result != (int32_t)((int16_t)result))) {
			Overflow = true;
			return (v1 > 0)? INT16_MAX : INT16_MIN;
		} else {
			return extract_l(result);
		}
	}
}


/**
* @brief	Arithmetically shift the 16 bit input v1 right v2 positions
*			with sign extension. If v2 is negative, arithmetically shift 
*			v1 left by -v2 with sign extension.
*			Saturate the result in case of underflows or overflows.
*
* @param[in] v1		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param[in] v2		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0xffff 8000 <= var_out <= 0x0000 7fff.
*/
int16_t shr(int16_t v1, int16_t v2)
{
	if(v2 < 0) {
		if(v2 < -16) v2 = -16;
		v2 = -v2;
		return  shl(v1, v2);
	} else {
		if(v2 >= 15) {
			return (v1 < 0)? -1:0;
		} else {
			/*-----
			if(v1 < 0) {
				return ~((~v1)>>v2);
			} else {
				return v1>>v2;
			}
			----*/
			return v1>>v2;
		}
	}
}


/**
* @brief	Produces the number of left shift needed to normalize the
*			16-bit variable v1 for positive values on the interval with 
*			minimum of 16384 and maximum of 32767, and for negative
*			values on the interval with minimum of -32768 and 
*			maximum of -16384; in order to normalize the result, the
*			following operation must be done :
*				norm_v1 = shl(v1,norm_s(v1)).
*
* @param[in] v1	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0xffff 8000 <= return value <= 0x0000 7fff.
*/
int16_t norm_s(int16_t v1)
{
	int16_t var_out;

	if(v1 == 0) {
		var_out = 0;
	} else {
		if(v1 == (int16_t) 0xffff) {
			var_out = 15;
		} else {
			if(v1 < 0) {
				v1 = ~v1;
			}
			for(var_out = 0; v1 < 0x4000; var_out++) {
				v1 <<= 1;
			}
		}
	}
	return var_out;
}


/**
* @brief	32 bits addition of the two 32 bits variables (L_v1+L_v2) 
*			with overflow control and saturation; the result is set 
*			at +2147483647 when overflow occurs or at -2147483648 when underflow occurs.
*
* @param[in] L_v1		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v1 <= 0x7fff ffff.
*
* @param[in] L_v2		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v2 <= 0x7fff ffff.
*
* @return	32 bit long signed integer (int32_t) whose value falls
*			in the range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
*/
int32_t L_add(int32_t L_v1, int32_t L_v2)
{
	int32_t L_var_out;

	L_var_out = L_v1 + L_v2;

	if(0 == (0x80000000L & (L_v1 ^ L_v2))) {	// When two sign bits are the same.
		if(0x80000000L & (L_var_out ^ L_v1)) {	// But if the resulting sign bit is different. 
			L_var_out = (L_v1 < 0)? INT32_MIN:INT32_MAX;
			Overflow = true;
		}
	}
	return L_var_out;
}


/**
* @brief	32 bits subtraction of the two 32 bits variables (L_v1-L_v2) with
*			overflow control and saturation; the result is set at +2147483647 when
*			overflow occurs or at -2147483648 when underflow occurs.
*
* @param[in] L_v1		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v1 <= 0x7fff ffff.
*
* @param[in] L_v2		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v2 <= 0x7fff ffff.
*
* @return	32 bit long signed integer (int32_t) whose value falls
*			in the range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
*/
int32_t L_sub(int32_t L_v1, int32_t L_v2)
{
	int32_t L_var_out;

	L_var_out = L_v1 - L_v2;

	if(0 != (0x80000000L & (L_v1 ^ L_v2))) {	// When two sign bits are different.
		if(0x80000000L & (L_var_out ^ L_v1)) {	// But if the resulting sign bit is different. 
			L_var_out = (L_v1 < 0L)? INT32_MIN : INT32_MAX;
			Overflow = true;
		}
	}
	return L_var_out;
}


/**
* @brief	Arithmetically shift the 32 bit input L_v1 left v2 positions.
*			Zero fill the v2 LSB of the result. If v2 is negative,
*			arithmetically shift L_v1 right by -v2 with sign extension.
*			Saturate the result in case of underflows or overflows.
*
* @param[in] L_v1		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v1 <= 0x7fff ffff.
*
* @param[in] v2	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	32 bit long signed integer (int32_t) whose value falls
*			in the range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
*/
int32_t L_shl(int32_t L_v1, int16_t v2)
{
	int32_t L_var_out = 0L;

	if(v2 <= 0) {
		if(v2 < -32)
			v2 = -32;
		v2 = -v2;
		L_var_out = L_shr(L_v1, v2);
	} else {
		for(; v2 > 0; v2--) {
			if(L_v1 > (int32_t) 0X3fffffffL) {
				Overflow = true;
				L_var_out = INT32_MAX;
				break;
			} else {
				if(L_v1 < (int32_t) 0xc0000000L) {
					Overflow = true;
					L_var_out = INT32_MIN;
					break;
				}
			}
			L_v1 *= 2;
			L_var_out = L_v1;
		}
	}
	return	L_var_out;
}


/**
* @brief	Arithmetically shift the 32 bit input L_v1 right v2 positions
*			with sign extension.
*			If v2 is negative, arithmetically shift L_v1 left by 
*			-v2 and zero fill the -v2 LSB of the result.
*			Saturate the result in case of underflows or overflows.
*
* @param[in] L_v1		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v1 <= 0x7fff ffff.
*
* @param[in] v2	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	32 bit long signed integer (int32_t) whose value falls
*			in the range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
*/
int32_t L_shr(int32_t L_v1, int16_t v2)
{
	int32_t L_var_out;

	if(v2 < 0) {
		if(v2 < -32)	v2 = -32;
		v2 = -v2;
		L_var_out = L_shl(L_v1, v2);
	} else {
		if(v2 >= 31) {
			L_var_out = (L_v1 < 0L)? -1 : 0;
		} else {
			if(L_v1 < 0) {
				L_var_out = ~((~L_v1)>>v2);
			} else {
				L_var_out = L_v1>>v2;
			}
		}
	}
	return (L_var_out);
}

/**
* @brief	Produces the number of left shifts needed to normalize 
*			the 32-bit variable L_v1 for positive values on the interval 
*			with minimum of 1073741824 and maximum of 2147483647,
*			and for negative values on the interval with minimum of
*			-2147483648 and maximum of -1073741824; in order to normalize the result,
*			the following operation must be done :
*				norm_L_v1 = L_shl(L_v1,norm_l(L_v1)).
*
* @param[in] L_v1		32 bit long signed integer (int32_t) whose value falls in the
*						range : 0x8000 0000 <= v1 <= 0x7fff ffff.
*
* @return	16 bit short signed integer (int16_t) whose value falls in the
*			range : 0x0000 0000 <= var_out <= 0x0000 001f.
*/
int16_t norm_l(int32_t L_v1)
{
	if(L_v1 == 0) {
		return 0;
	} else {
		if(INT32_MIN == L_v1) {
			return 31;
		} else {
			int16_t var_out;

			if(L_v1 < 0) {
				L_v1 = ~L_v1;
			}
			for(var_out = 0; L_v1 < 0x40000000L; var_out++) {
				L_v1 <<= 1;
			}
			return var_out;
		}
	}
}


/**
* @brief	L_mult is the-32 bit result of the multiplication of
*			v1 times v2 with one shift left i.e.:
*				L_mult(v1,v2) = L_shl((v1 times v2),1) and
*				L_mult(-32768,-32768) = 2147483647.
*
* @param[in] v1	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param[in] v2	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0xffff 8000 <= return value <= 0x0000 7fff.
*/
int32_t L_mult(int16_t v1, int16_t v2)
{
	int32_t L_var_out;
	L_var_out = (int32_t)v1 *(int32_t)v2;

	if(L_var_out != 0x40000000L) {
		L_var_out <<= 1;
	} else {
		Overflow = true;
		L_var_out = INT32_MAX;
	}

	return L_var_out;
}


/**
* @brief	L_mult0 is the 32-bit result of the multiplication of
*			v1 times v2 without one left shift.
*
* @param v1	16 bit short signed integer (int16_t) whose value falls in the
*				range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param v2	16 bit short signed integer (int16_t) whose value falls in the
*				range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	32 bit long signed integer (int32_t) whose value falls in the
*			range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
*/
int32_t L_mult0(int16_t v1, int16_t v2)
{
	return	(int32_t)v1*(int32_t)v2;
}


/**
* @brief	Performs the multiplication of v1 by v2 and gives a 16-bit
*			result which is scaled i.e.:
*				mult(v1,v2) = extract_l(L_shr((v1 times v2),15)) and
*				mult(-32768,-32768) = 32767.
*
* @param[in] v1	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param[in] v2	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0xffff 8000 <= return value <= 0x0000 7fff.
*/
int16_t mult(int16_t v1, int16_t v2)
{
	int32_t acc;

	if((-32768 == v1) && (-32768 == v2)) {
		return INT16_MAX;
	}
	acc = (int32_t)v1*(int32_t)v2;
	acc <<= 1;

	return extract_h(acc);
}


/**
* @brief	Same as mult with rounding, i.e.:
*				mult_r(v1,v2) = extract_l(L_shr(((v1 * v2) + 16384),15)) and
*				mult_r(-32768,-32768) = 32767.
*
* @param[in] v1	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param[in] v2	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0xffff 8000 <= return value <= 0x0000 7fff.
*/
int16_t mult_r(int16_t v1, int16_t v2)
{
	int16_t var_out;
	int32_t L_product_arr;

	L_product_arr = (int32_t) v1 *(int32_t) v2; /* product */
	L_product_arr += 0x00004000L;					/* round */
	L_product_arr &= 0xffff8000L;
	L_product_arr >>= 15;							/* shift */

	if(L_product_arr & 0x00010000L) {   /* sign extend when necessary */
		L_product_arr |=  0xffff0000L;
	}

	return	 saturate(L_product_arr);
}


/**
* @brief	Multiply v1 by v2 and shift the result left by 1. Add the
*			32-bit result to L_v3 with saturation, return a 32 bit result:
*				L_mac(L_v3,v1,v2) = L_add(L_v3,L_mult(v1,v2)).
*
* @param[in] L_v3		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v3 <= 0x7fff ffff.
*
* @param[in] v1		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param[in] v2		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	32 bit long signed integer (int32_t ) whose value falls
*			in the range : 0x8000 0000 <= return value <= 0x7fff ffff.
*/
int32_t L_mac(int32_t L_v3, int16_t v1, int16_t v2)
{
	return L_add(L_v3, L_mult(v1, v2));
}


/**
* @brief	Multiply v1 by v2 (without left shift) and add the 32 bit result
*			to L_v3 with saturation, return a 32 bit result:
*				L_mac0(L_v3,v1,v2) = L_add(L_v3,(L_mult0(v1,v2)).
*
* @param L_v3		32 bit long signed integer (int32_t) whose value falls in the
*					range : 0x8000 0000 <= L_v3 <= 0x7fff ffff.
*
* @param v1		16 bit short signed integer (int16_t) whose value falls in the
*					range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param v2		16 bit short signed integer (int16_t) whose value falls in the
*					range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	32 bit long signed integer (int32_t) whose value falls in the
*			range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
*/
int32_t L_mac0(int32_t L_v3, int16_t v1, int16_t v2)
{
	return	L_add(L_v3, L_mult0(v1, v2));
}


/**
* @brief	Multiply v1 by v2 and shift the result left by 1. Add the 32-bit
*			result to L_v3 without saturation, return a 32 bit result. Generate
*			carry and overflow values :
*				L_macNs(L_v3,v1,v2) = L_add_c(L_v3,L_mult(v1,v2)).
*
* @param[in] L_v3		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v3 <= 0x7fff ffff.
*
* @param[in] v1		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param[in] v2		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	32 bit long signed integer (int32_t ) whose value falls
*			in the range : 0x8000 0000 <= return value <= 0x7fff ffff.
*/
int32_t L_macNs(int32_t L_v3, int16_t v1, int16_t v2)
{
	return L_add_c(L_v3, L_mult(v1, v2));
}


/**
* @brief	Multiply v1 by v2 and shift the result left by 1.  Add the
*			32 bit result to L_v3 with saturation. Round the LS 16 bits 
*			of the result into the MS 16 bits with saturation and shift 
*			the result right by 16. Return a 16 bit result.
*				mac_r(L_v3,v1,v2) = round_fx(L_mac(L_v3,v1,v2))
*
* @param[in] L_v3		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v3 <= 0x7fff ffff.
*
* @param[in] v1		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param[in] v2		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0xffff 8000 <= return value <= 0x0000 7fff.
*/
int16_t mac_r(int32_t L_v3, int16_t v1, int16_t v2)
{
	return	round_fx(L_mac(L_v3, v1, v2));
}


/**
* @brief	Multiply v1 by v2 and shift the result left by 1.  Subtract
*			the 32-bit result from L_v3 with saturation, return a 32 bit result:
*				L_msu(L_v3,v1,v2) = L_sub(L_v3,L_mult(v1,v2)).
*
* @param[in] L_v3		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v3 <= 0x7fff ffff.
*
* @param[in] v1		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param[in] v2		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	32 bit long signed integer (int32_t ) whose value falls
*			in the range : 0x8000 0000 <= return value <= 0x7fff ffff.
*/
int32_t L_msu(int32_t L_v3, int16_t v1, int16_t v2)
{
	return L_sub(L_v3, L_mult(v1, v2));
}


/**
* @brief	Multiply v1 by v2 (without left shift) and subtract the 32-bit
*			result to L_v3 with saturation, return a 32 bit result:
*				L_msu0(L_v3,v1,v2) = L_sub(L_v3,(L_mult0(v1,v2)).
*
* @param L_v3		32 bit long signed integer (int32_t) whose value falls in the
*					range : 0x8000 0000 <= L_v3 <= 0x7fff ffff.
*
* @param v1		16 bit short signed integer (int16_t) whose value falls in the
*					range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param v2		16 bit short signed integer (int16_t) whose value falls in the
*					range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	32 bit long signed integer (int32_t) whose value falls in the
*			range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
*/
int32_t L_msu0(int32_t L_v3, int16_t v1, int16_t v2)
{
	return	L_sub(L_v3, L_mult0(v1, v2));
}


/**
* @brief	Multiply v1 by v2 and shift the result left by 1. Subtract
*			the 32-bit result from L_v3 without saturation, return a
*			32-bit result. Generate carry and overflow values :
*				L_msuNs(L_v3,v1,v2) = L_sub_c(L_v3,L_mult(v1,v2)).
*
* @param[in] L_v3		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v3 <= 0x7fff ffff.
*
* @param[in] v1		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param[in] v2		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	32 bit long signed integer (int32_t ) whose value falls
*			in the range : 0x8000 0000 <= return value <= 0x7fff ffff.
*/
int32_t L_msuNs(int32_t L_v3, int16_t v1, int16_t v2) {
  return L_sub_c(L_v3, L_mult(v1, v2));
}


/**
* @brief	Multiply v1 by v2 and shift the result left by 1. Subtracts
*			the 32-bit result from L_v3 with saturation.
*			Round the LS 16 bits of the result into the MS 16 bits 
*			with saturation and shift the result right by 16. Return a 16 bit result.
#				msu_r(L_v3,v1,v2) = round_fx(L_msu(L_v3,v1,v2))    |
*
* @param[in] L_v3		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v3 <= 0x7fff ffff.
*
* @param[in] v1		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param[in] v2		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0xffff 8000 <= return value <= 0x0000 7fff.
*/
int16_t msu_r(int32_t L_v3, int16_t v1, int16_t v2)
{
	return round_fx(L_msu(L_v3, v1, v2));
}


/**
* @brief	Round the lower 16 bits of the 32 bit input number into the MS 16 bits with saturation.
*			Shift the resulting bits right by 16 and return the 16 bit number:
*			round_fx(L_v1) = extract_h(L_add(L_v1,32768))
*
* @param[in] L_v1		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v1 <= 0x7fff ffff.
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0xffff 8000 <= return value <= 0x0000 7fff.
*/
int16_t round_fx(int32_t L_v1)
{
	return extract_h(L_add(L_v1, 0x00008000L));
}


/**
* @brief	Performs 32 bits addition of the two 32 bits variables (L_v1+L_v2+C)
*			with carry. No saturation. Generate carry and Overflow values. 
*			The carry and overflow values are binary variables which can be 
*			tested and assigned values.                                                          
*
* @param[in] L_v1		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v1 <= 0x7fff ffff.
*
* @param[in] L_v2		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v2 <= 0x7fff ffff.
*
* @return	32 bit long signed integer (int32_t) whose value falls
*			in the range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
*
*
* @attention	In some cases the Carry flag has to be cleared or set before using
*				operators which take into account its value.
*/
int32_t L_add_c(int32_t L_v1, int32_t L_v2)
{
	int32_t L_var_out;
	int32_t L_test;
	int carry_int = 0;

	L_var_out = L_v1 + L_v2 + Carry;

	L_test = L_v1 + L_v2;

	if((L_v1 > 0) && (L_v2 > 0) && (L_test < 0)) {
		Overflow = true;
		carry_int = 0;
	} else {
		if((L_v1 < 0) && (L_v2 < 0)) {
			if(L_test >= 0) {
				Overflow = true;
				carry_int = 1;
			} else {
				Overflow = false;
				carry_int = 1;
			}
		} else {
			if(((L_v1 ^ L_v2) < 0) && (L_test >= 0)) {
				Overflow = false;
				carry_int = 1;
			} else {
				Overflow = false;
				carry_int = 0;
			}
		}
	}

	if(Carry) {
		if(L_test == INT32_MAX) {
			Overflow = true;
			Carry = carry_int;
		} else {
			if(L_test == (int32_t) 0xFFFFFFFFL) {
				Carry = 1;
			} else {
				Carry = carry_int;
			}
		}
	} else {
		Carry = carry_int;
	}

	return	L_var_out;
}


/**
* @brief	Performs 32 bits subtraction of the two 32 bits variables with carry 
*			(borrow) : L_v1-L_v2-C. No saturation. Generate carry and 
*			Overflow values. 
*			The carry and overflow values are binary variables which can be 
*			tested and assigned values.
*
* @param[in] L_v1		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v1 <= 0x7fff ffff.
*
* @param[in] L_v2		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v2 <= 0x7fff ffff.
*
* @return	32 bit long signed integer (int32_t) whose value falls
*			in the range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
*
*
* @attention	In some cases the Carry flag has to be cleared or set before using
*				operators which take into account its value.
*/
int32_t L_sub_c(int32_t L_v1, int32_t L_v2)
{
	int32_t L_var_out;
	int32_t L_test;
	int carry_int = 0;

	if(Carry) {
		Carry = 0;
		if(L_v2 != INT32_MIN) {
			L_var_out = L_add_c(L_v1, -L_v2);
		} else {
			L_var_out = L_v1 - L_v2;
			if(L_v1 > 0L) {
				Overflow = true;
				Carry = 0;
			}
		}
	} else {
		L_var_out = L_v1 - L_v2 - 0x00000001L;
		L_test = L_v1 - L_v2;

		if((L_test < 0) && (L_v1 > 0) && (L_v2 < 0)) {
			Overflow = true;
			carry_int = 0;
		} else if((L_test > 0) && (L_v1 < 0) && (L_v2 > 0)) {
			Overflow = true;
			carry_int = 1;
		} else if((L_test > 0) && ((L_v1 ^ L_v2) > 0)) {
			Overflow = false;
			carry_int = 1;
		}
		if(L_test == INT32_MIN) {
			Overflow = true;
			Carry = carry_int;
		} else {
			Carry = carry_int;
		}
	}

	return	L_var_out;
}


/**
* @brief	Same as shr(v1,v2) but with rounding. Saturate the result in case of
*			underflows or overflows :
*			- If v2 is greater than zero :
*				if(sub(shl(shr(v1,v2),1),shr(v1,sub(v2,1)))) is equal to zero
*				then
*					shr_r(v1,v2) = shr(v1,v2)
*				else
*					shr_r(v1,v2) = add(shr(v1,v2),1)
*			- If v2 is less than or equal to zero :
*				shr_r(v1,v2) = shr(v1,v2).
*
* @param[in] v1		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @param[in] v2		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0xffff 8000 <= var_out <= 0x0000 7fff.
*/
int16_t shr_r(int16_t v1, int16_t v2)
{
	int16_t var_out;

	if(v2 > 15) {
		var_out = 0;
	} else {
		var_out = shr(v1, v2);

		if(v2 > 0) {
			if((v1 & ((int16_t)1<<(v2 - 1))) != 0) {
				var_out++;
			}
		}
	}

	return	var_out;
}


/**
* @brief	Same as L_shr(L_v1,v2) but with rounding. Saturate the
*			result in case of underflows or overflows :
*				- If v2 is greater than zero :
*				if(L_sub(L_shl(L_shr(L_v1,v2),1),L_shr(L_v1,sub(v2,1))))
*				is equal to zero
*				then
*					L_shr_r(L_v1,v2) = L_shr(L_v1,v2)
*				else
*					L_shr_r(L_v1,v2) = L_add(L_shr(L_v1,v2),1)
*				- If v2 is less than or equal to zero :
*					L_shr_r(L_v1,v2) = L_shr(L_v1,v2).
*
* @param[in] L_v1		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v1 <= 0x7fff ffff.
*
* @param[in] v2	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v2 <= 0x0000 7fff.
*
* @return	32 bit long signed integer (int32_t) whose value falls
*			in the range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
*/
int32_t L_shr_r(int32_t L_v1, int16_t v2)
{
	int32_t L_var_out;

	if(v2 > 31) {
		L_var_out = 0;
	} else {
		L_var_out = L_shr(L_v1, v2);

		if(v2 > 0) {
			if((L_v1 & ((int32_t)1<<(v2 - 1))) != 0) {
				L_var_out++;
			}
		}
	}

	return L_var_out;
}


/**
* @brief	Integer 16-bit multiplication with overflow control.
*			No overflow protection is performed if ORIGINAL_G7231 is defined.
*
* @param[in] a	16 bit short signed integer (int16_t).
* @param[in] b	16 bit short signed integer (int16_t).
*
* @return	16 bit short signed integer (int16_t). No overflow checks
*			are performed if ORIGINAL_G7231 is defined.
*/
int16_t i_mult(int16_t a, int16_t b)
{
#ifdef ORIGINAL_G7231
	return a * b;
#else
	int32_t register c = a * b;
	return saturate(c);
#endif
}


/**
* @brief	32 bit L_v1 is set to 2147483647 if an overflow occured or to
*			-2147483648 if an underflow occured on the most recent L_add_c,
*			L_sub_c, L_macNs or L_msuNs operations.
*			The carry and overflow values are binary values which can be 
*			tested and assigned values.
*
* @param[in] L_v1		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v1 <= 0x7fff ffff.
*
* @return	32 bit long signed integer (int32_t) whose value falls
*			in the range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
*/
int32_t L_sat(int32_t L_v1)
{
	int32_t L_var_out;
	L_var_out = L_v1;

	if(Overflow) {
		if(Carry) {
			L_var_out = INT32_MIN;
		} else {
			L_var_out = INT32_MAX;
		}
		Carry = 0;
		Overflow = false;
	}
	return L_var_out;
}


/**
* @brief	Multiplies a 16 bit word v by a 32 bit word Lv and returns a 32 bit
*			word (multiplying 16 by 32 bit words gives 48 bit word; the function
*			extracts the 32 MSB and shift the result to the left by 1).
*
*			A 32 bit word can be written as
*				Lv = a  +  b * 2^16
*			where a= unsigned 16 LSBs and b= signed 16 MSBs.
*			The function returns v * Lv  /  2^15  which is equivalent to
*				a*v / 2^15 + b*v*2
*
* @param[in] Lv		32 bit long signed integer (int32_t) whose value falls in the
*					range : 0x8000 0000 <= v1 <= 0x7fff ffff.
*
* @param[in] v		16 bit short signed integer (int16_t) whose value falls in the
*					range : 0x8000 <= v1 <= 0x7fff.
*
* @return	32 bit long signed integer (int32_t) whose value falls in the
*			range : 0x8000 0000 <= var_out <= 0x7fff ffff.
*/
int32_t L_mls(int32_t Lv, int16_t v)
{
	int32_t acc;

	acc = Lv & 0x0000ffffL;
	acc = acc * (int32_t)v;
	acc = L_shr(acc, 15);
	acc = L_mac(acc, v, extract_h(Lv));

	return acc;
}


/**
* @brief	Produces a result which is the fractional integer division of v1  by v2; 
*			v1 and v2 must be positive and v2 must be greater or equal to v1;
*			the result is positive (leading bit equal to 0) and truncated to 16 bits.
*			If v1 = v2 then div(v1,v2) = 32767.
*
* @param[in] v1	16 bit short signed integer (int16_t) whose value falls in 
*					the range : 0x0000 0000 <= v1 <= v2 and v2 != 0. 
*
* @param[in] v2	16 bit short signed integer (int16_t) whose value falls in 
*					the range : v1 <= v2 <= 0x0000 7fff and v2 != 0.
*
* @return	16 bit short signed integer (int16_t) whose value falls in the 
*			range : 0x0000 0000 <= var_out <= 0x0000 7fff.
*			It's a Q15 value (point between b15 and b14).
*/
int16_t div_s(int16_t v1, int16_t v2)
{
	int16_t var_out = 0;
	int32_t L_num;
	int32_t L_denom;

	assert(v1 <= v2);
	assert(0 <= v1);
	assert(0 <  v2);

	if(0 == v1) {
		var_out = 0;
	} else {
		if(v1 == v2) {
			var_out = INT16_MAX;
		} else {
			L_num = L_deposit_l(v1);
			L_denom = L_deposit_l(v2);

			int16_t iteration;
			for(iteration = 0; iteration < 15; iteration++) {
				var_out <<= 1;
				L_num <<= 1;

				if(L_num >= L_denom) {
					L_num = L_sub(L_num, L_denom);
					var_out = add(var_out, 1);
				}
			}
		}
	}
	return var_out;
}


/**
* @brief	Produces a result which is the fractional integer division of a
*			positive 32-bit variable L_num by a positive 16-bit variable den.
*			The result is positive (leading bit equal to 0) and truncated to
*			16 bits.	
*
* @param[in] L_num	32 bit long signed integer (int32_t) whose value falls in the
*					range : 0x0000 0000 <= L_num <= (den << 16)  and den != 0.
*					L_v1 must be considered as a Q.31 value
*
* @param[in] den	16 bit short signed integer (int16_t) whose value falls in the
*					range : L_num <= (den<< 16) <= 0x7fff0000 and den != 0.
*					den must be considered as a Q.15 value
*
* @return	16 bit short signed integer (int16_t) whose value falls in the
*			range : 0x0000 0000 <= var_out <= 0x0000 7fff.
*			It's a Q15 value (point between b15 and b14).
*/
int16_t div_l(int32_t L_num, int16_t den)
{
	int32_t L_den;
	int16_t iteration;

	// assert(0 != den);
	assert(0 <  den);
	assert(0 <= L_num);

	L_den = L_deposit_h(den);

	if(L_num >= L_den) {
		return INT16_MAX;
	} else {
		int16_t var_out = 0;
		L_num = L_shr(L_num, (int16_t) 1);
		L_den = L_shr(L_den, (int16_t) 1);
		for(iteration = (int16_t) 0; iteration < (int16_t) 15; iteration++) {
			var_out = shl(var_out, (int16_t) 1);
			L_num = L_shl(L_num, (int16_t) 1);
			if(L_num >= L_den) {
				L_num = L_sub(L_num, L_den);
				var_out = add(var_out, (int16_t) 1);
			}
		}
		return var_out;
	}
}


/*-------------------------------------------------------------------------------
*	Module Debug 
-------------------------------------------------------------------------------*/
#ifdef MODULE_DEBUG

#include <stdio.h>

int main(int argc, char* argv[])
{
	int32_t var[5];

	for(int i = 0; i < argc; i++) {
		if(0 < i) {
			sscanf(argv[i], "%x\n", &var[i - 1]); 
		}
	}

	for(int i = 0; i < argc - 1; i++) {
		//	printf("%x\n", var[i]);
	}

	printf("L_mls=%x\n", L_mls(var[0], var[1]));
	
	return 0;
}
#endif
/*==============================================================================
*	End
*===============================================================================*/
