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
#ifndef _BASIC_OP_H
#define _BASIC_OP_H

#include <stdint.h>
#include <stdbool.h>

extern bool Overflow;
extern int Carry;

/**
* @brief	Return the 16 MSB of L_v1.
*
* @param[in] L_v1		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v1 <= 0x7fff ffff.
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0xffff 8000 <= return value <= 0x0000 7fff.
*/
static inline int16_t extract_h(int32_t L_v1)
{
	return L_v1>>16;
}


/**
* @brief	Return the 16 LSB of L_v1.
*
* @param[in] L_v1		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v1 <= 0x7fff ffff.
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0xffff 8000 <= return value <= 0x0000 7fff.
*/
static inline int16_t extract_l(int32_t L_v1)
{
	return (int16_t)L_v1;
}


/**
* @brief	Deposit the 16 bit v1 into the 16 MS bits of the 32 bit output.
*			The 16 LS bits of the output are zeroed.
*
* @param[in] v1		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @return	32 bit long signed integer (int32_t ) whose value falls
*			in the range : 0x8000 0000 <= return value <= 0x7fff 0000.
*/
static inline int32_t L_deposit_h(int16_t v1)
{
	return (int32_t)v1<<16;
}


/**
* @brief	Deposit the 16 bit v1 into the 16 LS bits of the 32 bit output. The
*			16 MS bits of the output are sign extended.
*
* @param[in] v1		16 bit short signed integer (int16_t) whose value falls
*						in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @return	32 bit long signed integer (int32_t ) whose value falls
*			in the range : 0xFFFF 8000 <= return value <= 0x0000 7fff.
*/
static inline int32_t L_deposit_l(int16_t v1)
{
	return (int32_t)v1;
}


/**
* @brief	Negate v1 with saturation, saturate in the case where input is -32768:
*				negate(v1) = sub(0,v1).
*
* @param[in] v1	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0xffff 8000 <= return value <= 0x0000 7fff.
*/
static inline int16_t negate(int16_t v1)
{
	return (v1 == INT16_MIN)? INT16_MAX : -v1;
}


/**
* @brief	Negate the 32-bit variable L_v1 with saturation; 
*			saturate in the case where input is -2147483648 (0x8000 0000).                               |
*
* @param[in] L_v1		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v1 <= 0x7fff ffff.
*
* @return	32 bit long signed integer (int32_t) whose value falls
*			in the range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
*/
static inline int32_t L_negate(int32_t L_v1)
{
	return (INT32_MIN == L_v1)? INT32_MAX : -L_v1;
}


/**
* @brief	Absolute value of v1; abs_s(-32768) = 32767.
*
* @param[in] v1	16 bit short signed integer (int16_t) whose value falls
*					in the range : 0xffff 8000 <= v1 <= 0x0000 7fff.
*
* @return	16 bit short signed integer (int16_t) whose value falls
*			in the range : 0x0000 0000 <= return value <= 0x0000 7fff.
*/
static inline int16_t abs_s(int16_t v1)
{
	if(INT16_MIN == v1) {
		return INT16_MAX;
	} else {
		return (v1 < 0)? -v1 : v1;
	}
}


/**
* @brief	Absolute value of L_v1; Saturate in case where the input is -214783648
*
* @param[in] L_v1		32 bit long signed integer (int32_t ) whose value falls
*						in the range : 0x8000 0000 <= L_v1 <= 0x7fff ffff.
*
* @return	32 bit long signed integer (int32_t) whose value falls
*			in the range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.
*/
static inline int32_t L_abs(int32_t L_v1)
{
	if(L_v1 == INT32_MIN) {
		return INT32_MAX;
	} else {
		return (L_v1 < 0)? -L_v1 : L_v1; 
	}
}


/**
* @brief	Compares two variables returns the maximum/minimum value.
*
* @param v1		Signed 16/32-bit
* @param v2		Signed 16/32-bit
*
* @return		Signed 16/32-bit maximum/minimum value.
*/
static inline int16_t s_max(int16_t v1, int16_t v2)
{
	return (v1 > v2)? v1 : v2;
}

static inline int16_t s_min(int16_t v1, int16_t v2)
{
	return (v1 < v2)? v1 : v2;
}

static inline int32_t L_max(int32_t L_v1, int32_t L_v2)
{
	return (L_v1 > L_v2)? L_v1 : L_v2;
}

static inline int32_t L_min(int32_t L_v1, int32_t L_v2)
{
	return (L_v1 < L_v2)? L_v1 : L_v2;
}



#ifdef	__cplusplus
	extern "C" {
#endif

int16_t saturate(int32_t L_v1);				// Saturation
int16_t add(int16_t v1, int16_t v2);		// Short add
int16_t sub(int16_t v1, int16_t v2);		// Short sub
// int16_t abs_s(int16_t v1);					// Short abs
int16_t shl(int16_t v1, int16_t v2);		// Short shift left
int16_t shr(int16_t v1, int16_t v2);		// Short shift right
// int16_t negate(int16_t v1);					// Short negate
int16_t norm_s(int16_t v1);					// Short norm
int32_t L_add(int32_t L_v1, int32_t L_v2);	// Long add
int32_t L_sub(int32_t L_v1, int32_t L_v2);	// Long sub
// int32_t L_abs(int32_t L_v1);				// Long abs
int32_t L_shl(int32_t L_v1, int16_t v2);	// Long shift left
int32_t L_shr(int32_t L_v1, int16_t v2);	// Long shift right
// int32_t L_negate(int32_t L_v1);				// Long negate
int16_t norm_l(int32_t L_v1);				// Long norm
int32_t L_mult(int16_t v1, int16_t v2);		// Long mult
int32_t L_mult0(int16_t v1, int16_t v2);	// 32-bit Multiply w/o shift 1
int16_t mult(int16_t v1, int16_t v2);		// Short mult
int16_t mult_r(int16_t v1, int16_t v2);		// Mult with round
int32_t L_mac(int32_t L_v3, int16_t v1, int16_t v2);	// Mac
int32_t L_mac0(int32_t L_v3, int16_t v1, int16_t v2);	// 32-bit Mac w/o shift 1
int32_t L_macNs(int32_t L_v3, int16_t v1, int16_t v2);	// Mac without sat
int16_t mac_r(int32_t L_v3, int16_t v1, int16_t v2);	// Mac with rounding
int32_t L_msu(int32_t L_v3, int16_t v1, int16_t v2);	// Msu
int32_t L_msu0(int32_t L_v3, int16_t v1, int16_t v2);	// 32-bit Msu w/o shift 1
int32_t L_msuNs(int32_t L_v3, int16_t v1, int16_t v2);	// Msu without sat
int16_t msu_r(int32_t L_v3, int16_t v1, int16_t v2);	// Msu with rounding
// int16_t extract_h(int32_t L_v1);		// Extract high
// int16_t extract_l(int32_t L_v1);		// Extract low
int16_t round_fx(int32_t L_v1);			// Round
// int32_t L_deposit_h(int16_t v1);		// 16 bits v1 -> 32 bits MSB
// int32_t L_deposit_l(int16_t v1);		// 16 bits v1 -> 32 bits LSB
int32_t L_add_c(int32_t L_v1, int32_t L_v2);	// Long add with c
int32_t L_sub_c(int32_t L_v1, int32_t L_v2);	// Long sub with c
int16_t shr_r(int16_t v1, int16_t v2);			// Shift right with round
int32_t L_shr_r(int32_t L_v1, int16_t v2);		// Long shift right with round
int16_t i_mult(int16_t a, int16_t b);		// Weight FFS; currently assigned 3 */
int32_t L_sat(int32_t L_v1);				// Long saturation
int32_t L_mls(int32_t, int16_t);			// Weight FFS; currently assigned 5 */
int16_t div_s(int16_t v1, int16_t v2);		// Short division
int16_t div_l(int32_t, int16_t);			// Weight FFS; currently assigned 32 */


#ifdef	__cplusplus
	}
#endif

#endif
/*==============================================================================
*	End
==============================================================================*/
