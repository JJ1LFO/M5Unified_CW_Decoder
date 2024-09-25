/*==============================================================================
    bilinear.[ch]: Bilinear transformation method.

	Modified from $ s2z.c  1.1  95/06/23 16:08:52  S.Ikeda  Exp $


	MIT License

	Copyright (c) 1995-2002 Sho Ikeda

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

==============================================================================*/    
#ifndef	_BILINEAR_H
#define	_BILINEAR_H


#define	BILINEAR_TYPE	float		// or double

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

/**
* @brief Bilinear transformation method for analog-to-digital filter conversion
*
* @param[out] numd		Digital-domain numerator coefficients. numd[N].	
* @param[out] dend		Digital-domain denominator coefficients. dend[N].	
* @param[in] numa		Analog-domain numerator coefficients. numa[N].
* @param[in] dena		Analog-domain denominator coefficients. dena[N].
* @param[in] N			Number of coefficients.
* @param[in] T			Sampling Period (sec.). = 1/Fs. Fs is Sampling Frequency (Hz)
*/
extern void bilinear(BILINEAR_TYPE* numd, BILINEAR_TYPE* dend, const BILINEAR_TYPE* numa, const BILINEAR_TYPE* dena, int N, BILINEAR_TYPE T);


/**
* @brief	Prewarping for bilinear tranformation method. 
*
* @param[in] wa		Analog-domain angular frequency. = 2*pi*frequency(Hz).
* @param[in] T		Sampling Period (sec.). = 1/Fs. Fs is Sampling Frequency (Hz).
*
* @return 
*/
extern BILINEAR_TYPE bilinear_prewarp(BILINEAR_TYPE wa, BILINEAR_TYPE T);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* _BILINEAR_H */
/*==============================================================================
 *	End 
==============================================================================*/	
