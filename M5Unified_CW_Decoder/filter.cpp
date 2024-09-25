/**
* @brief	Digital filter class
*
*	@author	Sho Ikeda (JJ1LFO@jarl.com)
*	@date		Sep. 15th 2024
*
*	@copyright
*		Copyright (c) 1998-2024 Sho Ikeda.
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
*/
#include <cmath>
#include <assert.h>
	
#include "basic_op.h"
#include "bilinear.h"
#include "f2q.h"

#include "filter.hpp"
	

#ifdef	MODULE_DEBUG
	#include <stdio.h>
#endif

/**
* @brief	Set coefficients from cutoff frequency in Hz. 
*
* @param[in] cutoff			Cutoff frequency in Hz.
* @param[in] sample_freq	Sampling frequency in Hz.
* @param[in] type
*/
void IIRFilter1::setFreq(float cutoff, float sample_freq, FILTER_TYPE type)
{
	float numa[2], dena[2], numd[2], dend[2];

	float T = 1.0/sample_freq;
	float wp = bilinear_prewarp(2*M_PI*cutoff, T);

	switch(type) {
	case FILTER_TYPE_LPF:	// H(s) = wp/(wp + s)
		numa[0] = wp;	numa[1] = 0.0;
		break;
	case FILTER_TYPE_HPF:	// H(s) = s/(wp + s)
		numa[0] = 0.0;	numa[1] = 1.0;
		break;
	default:
		assert(false);
		break;
	}
	dena[0] = wp;	dena[1] = 1.0;

	bilinear(numd, dend, numa, dena, sizeof(dend)/sizeof(dend[0]), T);

	b0 = F2Q15( numd[0]);
	b1 = F2Q15( numd[1]);
	a1 = F2Q15(-dend[1]);

#ifdef	MODULE_DEBUG
	printf("\nfp=%f\n", wp/2.0/M_PI);

	printf("b0=%+f\n", b0/32768.f);
	printf("b1=%+f\n", b1/32768.f);
	printf("a1=%+f\n", a1/32768.f);
#endif
}

/**
* @brief	Filtering n samples.
*
* @param[out] out		Teh pointer to Q15 or Q31 output buffer. out[num].
* @param[in] in			The pointer to Q15 input buffer. in[num].
* @param[in] num		The number of samples.
*/
void IIR1DirectFormI::filter(int16_t* out, const int16_t* in, int num)
{
	for( ; 0 < num; num--) {
		*out++ = round_fx(convol(*in++));
	}
}

void IIR1DirectFormI::filter(int32_t* out, const int16_t* in, int num)
{
	for( ; 0 < num; num--) {
		*out++ = convol(*in++);
	}
}

/**
* @brief	Convolution.
*
* @param[in] in		Q15 input.
*
* @return	Q31 output.
*/
int32_t IIR1DirectFormI::convol(int16_t in)
{
	int32_t acc = L_mult(round_fx(fb0),	a1);
	acc = L_mac(acc,	ff0,			b1);
	acc = L_mac(acc,	ff0 = in,		b0);

	return (fb0 = acc);
}


/**
* @brief	Set coefficients from cutoff frequency in Hz. 
*
* @param[in] cutoff			Cutoff frequency in Hz.
* @param[in] sample_freq	Sampling frequency in Hz.
* @param[in] type
* @param[in] Q				Filter Q
*/
void IIRFilter2::setFreq(float cutoff, float sample_freq, FILTER_TYPE type, float Q)
{
	float numa[3], dena[3], numd[3], dend[3];

	float T = 1.0/sample_freq;
	float wp = bilinear_prewarp(2*M_PI*cutoff, T);

	switch(type) {
	case FILTER_TYPE_LPF:	// H(s) = wp^2/(wp^2 + 1/Q*wp*s + s^2)
		numa[0] = wp*wp;	numa[1] = 0.0;		numa[2] = 0.0;
		break;
	case FILTER_TYPE_BPF:	// H(s) = (1/Q*wp*s)/(wp^2 + 1/Q*wp*s + s^2)
		numa[0] = 0.0;		numa[1] = 1/Q*wp;	numa[2] = 0.0;
		break;
	case FILTER_TYPE_HPF:	// H(s) = s^2/(wp^2 + 1/Q*wp*s + s^2)
		numa[0] = 0.0;		numa[1] = 0.0;		numa[2] = 1.0;
		break;
	case FILTER_TYPE_BEF:	// H(s) = (wp*wp + w^2)/(wp^2 + 1/Q*wp*s + s^2)
		numa[0] = wp*wp;	numa[1] = 0.0;		numa[2] = 1.0;
	default:
		assert(false);
		break;
	}
	dena[0] = wp*wp;	dena[1] = 1/Q*wp;	dena[2] = 1.0;

	bilinear(numd, dend, numa, dena, sizeof(dend)/sizeof(dend[0]), T);

	b0 = F2Q14( numd[0]);
	b1 = F2Q14( numd[1]);
	b2 = F2Q14( numd[2]);
	a1 = F2Q14(-dend[1]);
	a2 = F2Q14(-dend[2]);

#ifdef	MODULE_DEBUG
	printf("\nfp=%f\n", wp/2.0/M_PI);

	printf("b0=%+f\n", b0/16384.f);
	printf("b1=%+f\n", b1/16384.f);
	printf("b2=%+f\n", b2/16384.f);
	printf("a1=%+f\n", a1/16384.f);
	printf("a2=%+f\n", a2/16384.f);
#endif
}

/**
* @brief	Convolution.
*
* @param[in] in		Q15 input.
*
* @return	Q31 output.
*/
int32_t IIR2DirectFormI::convol(int16_t in)
{
	int32_t acc =	 L_shl(L_mult(round_fx(fb1),	a2), 15 - 14);
	acc = L_add(acc, L_shl(L_mult(round_fx(fb0),	a1), 15 - 14));
	acc = L_add(acc, L_shl(L_mult(ff1,				b2), 15 - 14));
	acc = L_add(acc, L_shl(L_mult(ff0,				b1), 15 - 14));
	ff1 = ff0;
	acc = L_add(acc, L_shl(L_mult(ff0 = in,			b0), 15 - 14));
	fb1 = fb0;

	return (fb0 = acc);
}

//----------------------------------------------------------------------------------
#ifdef	MODULE_DEBUG

#include <stdio.h>

int main(int argc, char* argv[])
{
	IIRFilter1	lpf1(1000, 8000, FILTER_TYPE_LPF);
	IIRFilter1	hpf1(1000, 8000, FILTER_TYPE_HPF);

	IIRFilter2	lpf2(1000., 8000., FILTER_TYPE_LPF, 4);
	IIRFilter2	hpf2(1000., 8000., FILTER_TYPE_HPF, 4);
	IIRFilter2	bpf2(1000., 8000., FILTER_TYPE_BPF, 4);
	IIRFilter2	bef2(1000., 8000., FILTER_TYPE_BEF, 4);

	int16_t in[16], out[16];

	lpf1.filter(out, in, 16);
	bpf2.filter(out, in, 16);

	return 0;
}

#endif

/**
* End
*/
