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
#ifndef	_FILTER_HPP
#define	_FILTER_HPP

typedef enum {
	FILTER_TYPE_LPF = 0,
	FILTER_TYPE_BPF,
	FILTER_TYPE_HPF,
	FILTER_TYPE_APF,
	FILTER_TYPE_BEF,
	NUMOF_FILTER_TYPE
} FILTER_TYPE;


class IIR1DirectFormI {
	public:
		/**
		* @brief Constructor
		*
		* @param b0		Q15
		* @param b1		Q15
		* @param a1		Q15
		*/
		IIR1DirectFormI(int16_t b0 = 0x7FFF, int16_t b1 = 0, int16_t a1 = 0) : 
									b0(b0), b1(b1), a1(a1), ff0(0), fb0(0)	{ ; }

		/**
		* @brief	Filtering n samples.
		*
		* @description		H(z) = (b0 + b1*z^-1)/(1 - a1*z^-1)
		*
		* @param out	The pointer to output. out[num].
		* @param in		The pointer to input. in[num].
		* @param num	The number of output/input.
		*/
		void filter(int16_t* out, const int16_t* in, int num);	// Q15 outputs.
		void filter(int32_t* out, const int16_t* in, int num);	// Q31 outputs.

	protected:
		// Coefficients
		int16_t b0, b1;
		int16_t a1;
		
		// Delay line buffer
		int16_t	ff0;
		int32_t fb0;

	private:
		virtual int32_t convol(int16_t in);
};

class IIRFilter1 : public IIR1DirectFormI {
	public:
		IIRFilter1(float cutoff = 500, float sample_freq = 8000, FILTER_TYPE type = FILTER_TYPE_LPF) {
			setFreq(cutoff, sample_freq, type);
		}

		void setFreq(float cutoff, float sample_freq, FILTER_TYPE type);
};


class IIR2DirectFormI : public IIR1DirectFormI {
	public:
		/**
		* @brief Constructor
		*
		* @param b0		Q14
		* @param b1		Q14
		* @param b2		Q14
		* @param a1		Q14
		* @param a2		Q14
		*/
		IIR2DirectFormI(int16_t b0 = 0x7FFF, 
						int16_t b1 = 0,
						int16_t b2 = 0,
						int16_t a1 = 0,
						int16_t a2 = 0) : IIR1DirectFormI(b0, b1, a1), b2(b2), a2(a2), ff1(0), fb1(0) { ; }

	protected:
		// Coefficients
		int16_t b2;
		int16_t a2;

		// Delay line buffer
		int16_t ff1;
		int32_t fb1;

	private:
		virtual int32_t convol(int16_t in);
};

class IIRFilter2 : public IIR2DirectFormI {
	public:
		IIRFilter2(float cutoff = 500, float sample_freq = 8000, FILTER_TYPE type = FILTER_TYPE_LPF, float Q = 0.707) {
			setFreq(cutoff, sample_freq, type, Q);
		}

		void setFreq(float cutoff, float sample_freq, FILTER_TYPE type, float Q);
};

#endif /* _FILTER_HPP */
/**
* End
*/
