/**
* @brief	Automatic Gain Control. 
*
*	@author	Sho Ikeda (JJ1LFO@jarl.com)
*	@date		Sep. 15th 2024
*
*	@copyright
*		Copyright (c) 2024 Sho Ikeda.
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
#include <algorithm>
#include "basic_op.h"

#include "agc.hpp"


#define	AGC_GAIN_Qn		10

											//	#include <Arduino.h>
#define	AGC_DEBUG_PRINT(fmt, val)			//	Serial.printf(fmt, val)

void Agc::process(int16_t* out, const int16_t* in, size_t nSamples)
{
	for( ; 0 < nSamples; nSamples--) {
		*out = round_fx(L_shl(L_mult(*in, extract_h(gain)), 15 - AGC_GAIN_Qn));
		if(target < abs_s(*out)) {
			// gain(n) = attack*gain(n-1)
			gain = L_mult(round_fx(gain), attack);		
		} else {
			// gain(n) = ((1 - gain(n-1))/4)*release + gain(n-1)
			//				 = release/4 - (1 - release/4)*gain(n-1)
			int16_t acc = sub(INT16_MAX, round_fx(gain));
			acc = shr(acc, 2);
			gain = L_mac(gain, acc, release);
		}
		gain = L_max(L_deposit_h(minGain), L_min(L_deposit_h(maxGain), gain));

		in++;
		out++;
	}
}
		
float Agc::ms2CoefA1(float ms, float fs)
{
	float sec = std::max(ms/1000., 1e-6);
	return pow(exp(-1), 1.0/(sec*fs));		// = (1/e)^(T/release_time_sec)
}

void Agc::setAttackTime(float ms, float fs)
{
	attack = 32768.*ms2CoefA1(ms, fs) + 0.5;
	AGC_DEBUG_PRINT("Agc::attack = 0x%04X\n", attack);
}

void Agc::setReleaseTime(float ms, float fs)
{
	release = (1<<2)*32768.*(1.f - ms2CoefA1(ms, fs)) + 0.5;
	AGC_DEBUG_PRINT("Agc::release = 0x%04X\n", release);
}

void Agc::setTargetLevel(float amp)
{
	target = INT16_MAX*amp + 0.5;
	AGC_DEBUG_PRINT("Agc::target = 0x%04X\n", target);
}


void Agc::setMaxGain(float amp)
{
	maxGain = (1<<AGC_GAIN_Qn)*amp + 0.5;
	AGC_DEBUG_PRINT("Agc::maxGain = 0x%04X\n", maxGain);
}

void Agc::setMinGain(float amp)
{
	minGain = (1<<AGC_GAIN_Qn)*amp + 0.5;
	AGC_DEBUG_PRINT("Agc::minGain = 0x%04X\n", minGain);
}
/**
* End
*/
