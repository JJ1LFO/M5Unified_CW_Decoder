#ifndef	_M5UN_HPP
#define	_M5UN_HPP

#include "filter.hpp"
#include "agc.hpp"
#include "goertzel.hpp"


extern IIRFilter2* bpf;
extern Agc* agc;
extern Goertzel* goertzel;	

extern class Smoother {
	public:
		Smoother(int16_t up_coef, int16_t down_coef) : up_coef(up_coef), down_coef(down_coef), buf(0) { ; }
		
		void smooth(int16_t* out, const int16_t* in, int num);	// Q15 outputs.
		
		int16_t up_coef;
		int16_t down_coef;
	private:
		int32_t buf;
} *smoother;

extern void m5un_setup(float target_freq, float sampling_freq, int numof_testdata, int16_t smoothing_up, int16_t smoothing_down);
extern void m5un_loop(int wpm, int state, int16_t magnitude, int16_t magnitudelimit);

extern void m5un_printascii(char ascii);
extern void m5un_docode(const char* code);

#endif
