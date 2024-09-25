/*==============================================================================
 *
 * @brief	Goertzel Algorithm
 *
 * @remark	Gerald Goertzel. An Algorithm for the Evaluation of Finite Trigonometric Series. 
 *			The American Mathematical Monthly, Vol. 65, No. 1, p. 34, jan 1958.
 *
 * @author		Sho Ikeda (JJ1LFO@jarl.com)
 * @copyright
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
 *
 *============================================================================*/
#ifndef	_GOERTZEL_HPP
#define	_GOERTZEL_HPP

#include "basic_op.h"
#include "f2q.h"

class Goertzel {
public:
	/**
	* @brief Constructor
	*
	* @param[in] freq							Target frequency (Hz).
	* @param[in] sampling_freq		Sampling frequency (Hz).
	* @param[in] N								The number of samples.
	*/
	Goertzel(float freq = 1000, float sampling_freq = 8000, int N = 128) {
		setFreq(freq, sampling_freq, N);
	}

	/**
	* @brief Calc and set coefficients.
	*
	* @param[in] freq							Target frequency (Hz).
	* @param[in] sampling_freq		Sampling frequency (Hz).
	* @param[in] N								The number of samples.
	*/
	void setFreq(float freq, float sampling_freq, int num)
	{
		N = num;
		int k = N*freq/sampling_freq + 0.5;
		coef = F2Q14(2*cos((2*M_PI*k)/N));

		att = F2Q15(1.0f/N);
	}

	/**
	* @brief	Calc squared magnitude. 
	*
	* @param[in] in			The pointer to inputsamples. in[N].
	*
	* @return			Squared magnitude. Q31 format. 
	*							= y0^2 + y1^2 - (y0*y1)*coef.
	*/
	int32_t getSquaredMagnitude(const int16_t* in /* , int N */)
	{
		compute(in);

		int16_t y0 = round_fx(y[0]);
		int16_t y1 = round_fx(y[1]);

		int32_t magSq = mult(y0, y1);
		magSq = L_shl(L_mult(magSq, coef), 15 - 14);
		magSq = L_negate(magSq);
		magSq = L_mac(magSq, y1, y1);
		magSq = L_mac(magSq, y0, y0);

		return magSq;
	}

	/**
	* @brief	Calc magnitude. 
	*
	* @param[in] in			The pointer to inputsamples. in[N].
	*
	* @return		Magnitude. Q16 format. 
	*							= sqrt(y0^2 + y1^2 - (y0*y1)*coef).
	*/
	int16_t getMagnitude(const int16_t* in /* , int N */)
	{
		int32_t magSq = L_max(getSquaredMagnitude(in), 1);
		float magf = sqrt(magSq/((1LL<<31)*1.0f));
		return F2Q15(magf);
	}

private:
	int N;
	int32_t y[2];

	int16_t coef;		// Q14
	int16_t	att;		// Q15
	
	/**
	* @brief Goertzel algorithm feedback calc.
	*
	* @param[in] in			The pointer to input data. in[N]
	*
	* @remark			H(z) = (1/N)/{1 - ((z^-1)*coef - z^-2)} : N times.
	*/
	void compute(const int16_t* in)
	{
		y[0] = y[1] = 0;

		for (int i = 0 ; i < N; i++) {
			int32_t acc;
			acc = L_shl(L_mult(round_fx(y[0]), coef), 15 - 14);
			acc = L_sub(acc, y[1]);
			y[1] = y[0];
			y[0] = L_mac(acc, att, *in++);
		}
	}
};

#endif
/*==============================================================================
 *	End
 *============================================================================*/
