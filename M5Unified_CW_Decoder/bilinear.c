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
#include <stdlib.h>
#include <math.h>

#include "bilinear.h"

//------------------------------------------------------------------------------
static BILINEAR_TYPE*  a2c(const BILINEAR_TYPE* a, BILINEAR_TYPE* c, int N, BILINEAR_TYPE T) 
{
	BILINEAR_TYPE* buf = malloc(N*sizeof(BILINEAR_TYPE));
	int i, j, k, l, m;

	for(i = 0; i < N; i++)     c[i] = 0.0;

	for(k = 0; k < N; k++) {
		buf[0] = a[k];
		for(j = 1; j < N; j++)     buf[j] = 0.0;
		
		for(l = 1; l <= k; l++) { 
			for(m = N - 1; m >= 1; m--) {
				buf[m] += -2.f* buf[m-1];  
				buf[m-1] *= 2.f;  
			}
		}
		for(l = k+1; l < N; l++) { 
			for(m = N - 1; m >= 1; m--) {
				buf[m] += (T* buf[m-1]);
				buf[m-1] *= T;
			}
		}
		for(i = 0; i < N; i++)     c[i] += buf[i];     
	}

	free(buf);
	return c;
}

static void normalize(BILINEAR_TYPE* num, BILINEAR_TYPE* den, int N)
{
	int i;

	for(i = N - 1; 0 <= i; i--) {
		num[i] /= den[0];
		den[i] /= den[0];
	}
}


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
void bilinear(BILINEAR_TYPE* numd, BILINEAR_TYPE* dend, const BILINEAR_TYPE* numa, const BILINEAR_TYPE* dena, int N, BILINEAR_TYPE T)
{
	int n;

	for(n = N - 1; (0.0 == numa[n]) && (0.0 == dena[n]) && (0 < n); n--) {
		numd[n] = 0.0;
		dend[n] = 0.0;
	}
	n++;

	a2c(numa, numd, n, T); 
	a2c(dena, dend, n, T); 
	normalize(numd, dend, n);
}


/**
* @brief	Prewarping for bilinear tranformation method. 
*
* @param[in] wa		Analog-domain angular frequency. = 2*pi*frequency(Hz).
* @param[in] T			Sampling Period (sec.). = 1/Fs. Fs is Sampling Frequency (Hz)
*
* @return 
*/
BILINEAR_TYPE bilinear_prewarp(BILINEAR_TYPE wa, BILINEAR_TYPE T)
{
	return 	(BILINEAR_TYPE)(2.0*tan(wa*T/2.0)/T);
}

//------------------------------------------------------------------------------
//  Debug
//------------------------------------------------------------------------------
#ifdef  MODULE_DEBUG

#include <stdio.h>
#include <stdlib.h>



void NewError()
{
    fprintf(stderr, "Memory not enough\n");
    exit(-1);
}

int main()
{
	int	i;

	BILINEAR_TYPE	Fs;
    printf("Fs=");
    scanf("%f", &Fs);

    BILINEAR_TYPE T = 1.0/Fs;
    
	int	N;
    while(printf("demension ="), EOF != scanf("%d", &N)) {
		N++;
    	BILINEAR_TYPE* numa = malloc(N*sizeof(BILINEAR_TYPE));
        for(i = 1; i < N; i++)     numa[i] = 0.0;

        for(i = 0; i < N; i++) {
            printf("numa[%d]=", i);
			scanf("%f", &numa[i]);
        }

    	BILINEAR_TYPE* dena = malloc(N*sizeof(BILINEAR_TYPE));
        for(i = 1; i < N; i++)     dena[i] = 0.0;

        for(i = 0; i < N; i++) {
            printf("dena[%d]=", i);
			scanf("%f", &dena[i]);
        }
        
    	BILINEAR_TYPE* numd = malloc(N*sizeof(BILINEAR_TYPE));
        for(i = 0; i < N; i++)         numd[i] = 0.0;
            
    	BILINEAR_TYPE* dend = malloc(N*sizeof(BILINEAR_TYPE));
        for(i = 0; i < N; i++)         dend[i] = 0.0;

        bilinear(numd, dend, numa, dena, N, T);
        
        for(i = 0; i < N; i++)     printf("%f\n", numd[i]);
        
        for(i = 0; i < N; i++)     printf("%f\n", dend[i]);
    
		free(dend);    
		free(numd);    
		free(dena);    
		free(numa);    
    }
    
    return 0;
}
#endif  /* MODULE_DEBUG */
/*==============================================================================
 *	End 
==============================================================================*/    
