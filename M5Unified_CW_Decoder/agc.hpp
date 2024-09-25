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
#ifndef	_AGC_HPP
#define	_AGC_HPP


class Agc {
	public:
		int16_t target;
		int16_t	maxGain;	// Q10
		
		Agc(float target	=			0.707,
				float maxGain =			10.0,
				float attackTime =	5,
				float releaseTime =	200,
				float sample_rate =	8000) : gain(0)
		{ 
			setTargetLevel(target);
			setMaxGain(maxGain);
			setAttackTime(attackTime, sample_rate);
			setReleaseTime(releaseTime, sample_rate);
		
			setMinGain(0.7);
		}	

		void process(int16_t* out, const int16_t* in, size_t nSamples);

		void setTargetLevel(float amp);
		void setMaxGain(float amp);
		void setMinGain(float amp);
		void setAttackTime(float ms, float fs);
		void setReleaseTime(float ms, float fs);

	private:
		int16_t	attack;
		int16_t release;

		int32_t gain;
		int16_t minGain;

		float ms2CoefA1(float ms, float fs);
};

#endif
/**
* End
*/
