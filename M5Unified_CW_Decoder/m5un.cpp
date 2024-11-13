#include <M5Unified.h>
	

#include "m5un.hpp"

#define	WPM_TEXT_WIDTH	50

IIRFilter2* bpf;
Agc* agc;
Goertzel* goertzel;	
Smoother* smoother;

class Plot : public M5Canvas {
public:
	Plot(M5GFX* canvas = nullptr) : M5Canvas(canvas)	{ }

	void setup()
	{
		createSprite(_parent->width() - WPM_TEXT_WIDTH, 54);
		clear(TFT_NAVY);
	}

	void write(bool state, int magnitude, int magnitudelimit)
	{
		scroll(-1, 0);

		static int y0 = height() - 1;
		int x = width() - 1;

		int y = mapping(0, 12000, height() - 1, 0, magnitude);
		writeFastVLine(x, 0, height(), TFT_NAVY);
		writeFastVLine(x, (y < y0)? y:y0, abs(y - y0) + 1, TFT_GREEN);
		y0 = y;

		writePixel(x, mapping(0, 12000, height() - 1, 0, magnitudelimit), TFT_DARKGRAY);

		if(state) {
			writeFastVLine(x, 2, 2, TFT_YELLOW);		// TFT_OLIVE);
		}
	}
private:
	float mapping(float x0, float x1, float y0, float y1, float x)
	{
		return (x - x0)/(x1 - x0)*(y1 - y0) + y0;
	}
};

Plot plot(&M5.Display);

// M5Canvas text_wpm;
struct : public M5Canvas {
	void setup()
	{
		createSprite(WPM_TEXT_WIDTH, plot.height());
		setTextColor(TFT_LIGHTGRAY);
	}

	void loop(M5GFX* display, int32_t x, int32_t y, int wpm)
	{
		clear(TFT_NAVY);

		setFont(&fonts::Font4);
		setTextSize(0.5, 0.5);
		setCursor(16, 0);
		print("WPM");

		setFont(&fonts::Font7);
		setTextSize(0.75, 0.75);
		setCursor(0, 16);
		printf("%02d", wpm);
		pushSprite(display, x, y);
	}
} text_wpm;

struct SideTone {
	SideTone(float freq = 600, uint32_t di_msec = 50, uint8_t vol = 128) : freq(freq), di_msec(di_msec)
	{
		M5.Speaker.setVolume(vol);
	}

	void di(bool isDi = true) 
	{
		float sample_rate = M5.Speaker.config().sample_rate;
		size_t n_samples = di_msec/1000.0*((isDi)? 1:3)*sample_rate;

		int16_t* wav = new int16_t[n_samples];

		int16_t delta = F2Q15(2*freq/sample_rate);
		int16_t tri = 0; 
		for(int sample = 0; sample < n_samples; sample++) {
			wav[sample] = tri>>4;
			tri += delta;
		}
		
		M5.Speaker.playRaw(wav, n_samples, sample_rate);
		while(M5.Speaker.isPlaying()); 
		
		delete wav;

		delay(di_msec);
	}

	void dah() { di(false); }
	void space()	{ delay(di_msec*3); }

	void play(const char* str)
	{
		int num = strlen(str);

		for( ; 0 < num; num--) {
			switch(*str++) {
			case '.': di(); break;
			case '-': dah(); break;
			case ' ': space(); break;
			default:	break;	
			}
		}
	}

	float freq;
	int32_t di_msec;
	uint8_t  vol;

} side_tone;


void Smoother::smooth(int16_t* out, const int16_t* in, int num)
{
	for(; 0 < num; num--) {
		int16_t dat = (*in++ - round_fx(buf));
		buf = L_mac(buf, dat, (0 <= dat)? up_coef : down_coef);
		*out++ = round_fx(buf);
	}
}

static void Splash(void)
{
	M5.Display.clear(TFT_WHITE);

	M5Canvas splash(&M5.Display);
	splash.createSprite(M5.Display.width() - 2*20, M5.Display.height() - 40);

	splash.clear(TFT_WHITE);

	splash.setFont(&fonts::FreeSansBold18pt7b);
	splash.setCursor(32, splash.height()/3);
	splash.setTextColor(TFT_NAVY, TFT_WHITE);
	splash.print("CW Decoder");

	splash.setTextColor(TFT_BLACK, TFT_WHITE);
	splash.setFont(&fonts::FreeSans9pt7b);
	splash.setCursor(32, splash.getCursorY() + 2*splash.fontHeight(&fonts::FreeSans9pt7b));
	splash.println("Copyright(C) 2024. JJ1LFO");
	splash.println("");
	
	splash.setCursor(0, splash.getCursorY());
	splash.setFont(&fonts::Font2);
	splash.print("This program is a modified version of \"OZ1JHM CW Decoder VER 1.01\", "
																"originally developed by Hjalmar Skovholm Hansen.");
	
	splash.pushSprite(20, 0);

	constexpr char tx_str[] = "-.. .  .--- .--- .---- .-.. ..-. --- ";	/// "...- ...- ...-";
	side_tone.play(tx_str);

	for(int i = 0; i < splash.height(); i++) {
		splash.scroll(0, -1);
		splash.pushSprite(20, 0);
	}

	splash.deleteSprite();
}

void m5un_setup(float target_freq, float sampling_freq, int numof_testdata, int16_t smoothing_up, int16_t smoothing_down)
{
	bpf = new IIRFilter2(target_freq, sampling_freq, FILTER_TYPE_BPF, 0.7071);
	agc = new Agc(0.7, 20.0, 3, 5000, sampling_freq);

	smoother = new Smoother(smoothing_up, smoothing_down);

	goertzel = new Goertzel(target_freq, sampling_freq, numof_testdata, false);

	M5.begin();

	Splash();
	
	plot.setup();
	text_wpm.setup();

	M5.Display.clear(TFT_BLACK);
	M5.Display.setFont(&fonts::Font4);		/// FreeSans12pt7b);
	M5.Display.setTextScroll(true);
	M5.Display.setScrollRect(0, 0, M5.Display.width(), M5.Display.height() - plot.height());
}

void m5un_loop(int wpm, int state, int16_t magnitude, int16_t magnitudelimit)
{
	//	M5.Power.setLed((state)? 255:0);

	plot.write(state, magnitude, magnitudelimit);
	
	int32_t x, y, w, h;
	M5.Display.getScrollRect(&x, &y, &w, &h);
	plot.pushSprite(WPM_TEXT_WIDTH, y + h);
	
	text_wpm.loop(&M5.Display, 0, M5.Display.height() - plot.height(), wpm);

	// Serial.printf("%d %d ", -1000, 16384);
	// Serial.printf("%d %d\n", magnitude, magnitudelimit);

	M5.update();

	if(M5.BtnB.wasDoubleClicked()) {
		// Clear Text scroll.
		int32_t x, y, w, h;
		M5.Display.getScrollRect(&x, &y, &w, &h);
		M5.Display.fillRect(x, y, w, h, TFT_BLACK);
		M5.Display.setCursor(0, 0);

		// Serial.println("BntB double Clicked");
	}
}


void m5un_printascii(char ascii)
{
	M5.Display.print(ascii);
}

void m5un_docode(const char* code)
{
	static constexpr struct {
		char* code;
		char ascii;
	} table[] = {
		{ ".-",			'A' },
		{ "-...",		'B' },
		{ "-.-.",		'C' },
		{ "-..",		'D' },
		{ ".",			'E' },
		{ "..-.",		'F' },
		{ "--.",		'G' },
		{ "....",		'H' },
		{ "..",			'I' },
		{ ".---",		'J' },
		{ "-.-",		'K' },
		{ ".-..",		'L' },
		{ "--",			'M' },
		{ "-.",			'N' },
		{ "---",		'O' },
		{ ".--.",		'P' },
		{ "--.-",		'Q' },
		{ ".-.",		'R' },
		{ "...",		'S' },
		{ "-",			'T' },
		{ "..-",		'U' },
		{ "...-",		'V' },
		{ ".--",		'W' },
		{ "-..-",		'X' },
		{ "-.--",		'Y' },
		{ "--..",		'Z' },
		
		{ ".----",	'1' },
		{ "..---",	'2' },
		{ "...--",	'3' },
		{ "....-",	'4' },
		{ ".....",	'5' },
		{ "-....",	'6' },
		{ "--...",	'7' },
		{ "---..",	'8' },
		{ "----.",	'9' },
		{ "-----",	'0' },

		{ "..--..",	'?' },
		{ ".-.-.-",	'.' },
		{ "--..--",	',' },	
		{ "-.-.--",	'!' },
		{ ".--.-.",	'@' },
		{ "---...",	':' },
		{ "-....-",	'-' },
		{ "-..-.",	'/' },

		{ "-.--.",	'(' },
		{ "-.--.-",	')' },
		{ ".-...",	'_' },
		{ "...-..-",'$' },
		{ "...-.-",	'>' },
		{ ".-.-.",	'<' },
		{ "...-.",	'~' },
		
		{ "-...-",	'=' },
		{ ".-..-.",	'"' },
	};

	for(int idx = 0; idx < sizeof(table)/sizeof(table[0]); idx++) {
		if(0 == strcmp(code, table[idx].code)) {
			m5un_printascii(table[idx].ascii);
			break;
		}
	}
}
