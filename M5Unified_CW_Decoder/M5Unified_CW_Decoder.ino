/*******************************************************************************
*	This program is a modified version of "OZ1JHM CW Decoder VER 1.01", 
*	originally developed by Hjalmar Skovholm Hansen OZ1JHM.
*	These modifications were made by Sho Ikeda (JJ1LFO) on Sep. 25th 2024.
*	Modifications: 
*
* - Supports M5Unified (M5Core2).
*		- Convert Goertzel algorithm to fixed-point arithmetic operations.
*		- Added ability to visualize detection magnitude.
*		- Added Morse tape printing function.
*		- Added Automatic Gain Control for microphone.
*	  - Checked with Arduino IDE 2.3.2, M5Unified 0.1.17 and esp32 3.0.5
*
*	- Supports Grove - OLED Display 0.96" (SSD1315) 
*							https://wiki.seeedstudio.com/Grove-OLED-Display-0.96-SSD1315/
*
*					and Grove - I2C 16x2 RGB Backlight LCD.
*							https://www.seeedstudio.com/Grove-LCD-RGB-Backlight.html
*
*	- Supports Grove - Beginner Kit for Arduino.
*					https://www.seeedstudio.com/Grove-Beginner-Kit-for-Arduino-p-4549.html
*
*	- Changed some parameter values.
*
*	- Sampling frequency can be measured automatically at startup.
*
* - Adding some codes. '=', '"'.
*
* Copyright (C) 2024.  Sho Ikeda JJ1LFO@jarl.com
* This program is free software: you can redistribute it and/or modify 
* it under the terms of the GNU General Public License as published 
* by the Free Software Foundation.
*
*******************************************************************************/
///////////////////////////////////////////////////////////////////////
// CW Decoder made by Hjalmar Skovholm Hansen OZ1JHM  VER 1.01       //
// Feel free to change, copy or what ever you like but respect       //
// that license is http://www.gnu.org/copyleft/gpl.html              //
// Discuss and give great ideas on                                   //
// https://groups.yahoo.com/neo/groups/oz1jhm/conversations/messages //
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// Read more here http://en.wikipedia.org/wiki/Goertzel_algorithm        //
// if you want to know about FFT the http://www.dspguide.com/pdfbook.htm //
///////////////////////////////////////////////////////////////////////////

//******** Configurations ********************************************
//--------	Select one of USE_BOARD. ---------------------------------
// #define	USE_BOARD_OZ1JHM_ORIGINAL
// #define	USE_BOARD_GROVE_BEGINNER_KIT	// https://www.seeedstudio.com/Grove-Beginner-Kit-for-Arduino-p-4549.html
#define	USE_BOARD_M5UNIFIED

//--------	Select one of USE_PARAMETERS. ----------------------------
#define	USE_PARAMETERS_M5UNIFIED
// #define	USE_PARAMETERS_OZ1JHM_ORIGINAL
// #define	USE_PARAMETERS_FOR_GROVE_SOUND_SENSOR_MIC	// https://wiki.seeedstudio.com/Grove-Sound_Sensor/

//--------	Measure sampling frequency automatically -----------------
// #define	USE_MEASURE_SAMPLING_FREQ


#if defined(USE_BOARD_M5UNIFIED)

	#include <M5Unified.h>
	#include "m5un.hpp"

#else
	//--------	Select one of USE_LCD. ---0-------------------------------
	// #define USE_LCD_RS_4BIT_20X4		///	Orignal decoder11.ino by OZ1JHM.
	#define USE_LCD_SSD1306_128X64_HW_I2C		// Grove - OLED Display 0.96"
	// #define USE_LCD_GROVE_RGB_BACKLIGHT	// Grove - I2C 16x2 RGB Backlight LCD.

	//********************************************************************
	#if defined(USE_LCD_RS_4BIT_20X4)		///	Orignal decoder11.ino by OZ1JHM.

		#include <LiquidCrystal.h>

		///////////////////////////////////////////////
		// select the pins used on the LCD panel      /
		///////////////////////////////////////////////
		//  LiquidCrystal lcd(RS, E, D4, D5, D6, D7) //
		///////////////////////////////////////////////

		LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

		const int colums = 20; /// have to be 16 or 20
		const int rows = 4;  /// have to be 2 or 4
			
	#elif	defined(USE_LCD_SSD1306_128X64_HW_I2C)

		#include <U8x8lib.h>

		#ifdef U8X8_HAVE_HW_SPI
			#include <SPI.h>
		#endif
		
		U8X8_SSD1306_128X64_NONAME_HW_I2C lcd(/* reset=*/ U8X8_PIN_NONE);
		
		const int colums = 16;	/// have to be 16 or 20
		const int rows = 4;			/// have to be 2 or 4

	#elif	defined(USE_LCD_GROVE_RGB_BACKLIGHT)

		#include <Wire.h>				// I2C
		#include "rgb_lcd.h"
		
		rgb_lcd	lcd;
		
		const int colums = 16;	/// have to be 16 or 20
		const int rows = 2;			/// have to be 2 or 4

	#else
		#error	"Invalid USE_LCD_*****"
	#endif
#endif

//--------------------------------------------------------------------
#if	defined(USE_BOARD_M5UNIFIED)

#elif	defined(USE_BOARD_OZ1JHM_ORIGINAL)

	///	Orignal decoder11.ino by OZ1JHM.
	#define AUDIO_IN_PIN	A1
	#define	AUDIO_OUT_PIN	1
	#define	LED_PIN	13

#elif	defined(USE_BOARD_GROVE_BEGINNER_KIT)

	#define AUDIO_IN_PIN	A2		// Microphone input.
	#define	AUDIO_OUT_PIN	1			// Dummy. No beep.
	#define	LED_PIN	4

#else
	#error	"Invalid USE_BOARD_*****"
#endif

//--------------------------------------------------------------------
#if defined(USE_PARAMETERS_M5UNIFIED)

	#define	TARGET_FREQ						600
	#define	NUMOF_TESTDATA				53
	#define	NBTIME_MS							6
	#define	MAGNITUDELIMIT_LOW		F2Q15(0.10)
	#define	MAGNITUDE_SMOOTHING_UP			F2Q15(1.f/6)
	#define	MAGNITUDE_SMOOTHING_DOWN		F2Q15(1.f/5)
	#define	MAGNITUDE_THRESHOLD		0.7

#elif defined(USE_PARAMETERS_OZ1JHM_ORIGINAL)

	#define	TARGET_FREQ						558.0
	#define	NUMOF_TESTDATA				48
	#define	NBTIME_MS							6
	#define	MAGNITUDELIMIT_LOW		100
	#define	MAGNITUDE_FILTER_DIV	6
	#define	MAGNITUDE_THRESHOLD		0.6

#elif defined(USE_PARAMETERS_FOR_GROVE_SOUND_SENSOR_MIC)

	#define	TARGET_FREQ						600.0
	#define	NUMOF_TESTDATA				53
	#define	NBTIME_MS							6	
	#define	MAGNITUDELIMIT_LOW		2200
	#define	MAGNITUDE_FILTER_DIV	5
	#define	MAGNITUDE_THRESHOLD		0.707

#else
	#error	"Invalid USE_PARAMETERS_*****"
#endif
//--------------------------------------------------------------------

int lcdindex = 0;

#if !defined(USE_BOARD_M5UNIFIED)
	#if	defined(USE_LCD_SSD1306_128X64_HW_I2C)
		char line1[colums + 1];
		char line2[colums + 1];
	#else
		int line1[colums];
		int line2[colums];

		////////////////////////////////
		// Define 8 specials letters  //
		////////////////////////////////

		byte U_umlaut[8] =   {B01010,B00000,B10001,B10001,B10001,B10001,B01110,B00000}; // 'Ü'  
		byte O_umlaut[8] =   {B01010,B00000,B01110,B10001,B10001,B10001,B01110,B00000}; // 'Ö'  
		byte A_umlaut[8] =   {B01010,B00000,B01110,B10001,B11111,B10001,B10001,B00000}; // 'Ä'    
		byte AE_capital[8] = {B01111,B10100,B10100,B11110,B10100,B10100,B10111,B00000}; // 'Æ' 
		byte OE_capital[8] = {B00001,B01110,B10011,B10101,B11001,B01110,B10000,B00000}; // 'Ø' 
		byte fullblock[8] =  {B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};  
		byte AA_capital[8] = {B00100,B00000,B01110,B10001,B11111,B10001,B10001,B00000}; // 'Å'   
		byte emtyblock[8] =  {B00000,B00000,B00000,B00000,B00000,B00000,B00000,B00000};  
	#endif
#endif


#if	defined(USE_BOARD_M5UNIFIED)
	int16_t magnitude ;
	int16_t magnitudelimit = 100;
	int16_t magnitudelimit_low = MAGNITUDELIMIT_LOW;
#else
	int audioInPin = AUDIO_IN_PIN;	
	int audioOutPin = AUDIO_OUT_PIN;
	int ledPin = LED_PIN;

	float magnitude ;
	int magnitudelimit = 100;
	int magnitudelimit_low = MAGNITUDELIMIT_LOW;
#endif

int realstate = LOW;
int realstatebefore = LOW;
int filteredstate = LOW;
int filteredstatebefore = LOW;


///////////////////////////////////////////////////////////
// The sampling frq will be 8928 on a 16 mhz             //
// without any prescaler etc                             //
// because we need the tone in the center of the bins    //
// you can set the tone to 496, 558, 744 or 992          //
// then n the number of samples which give the bandwidth //
// can be (8928 / tone) * 1 or 2 or 3 or 4 etc           //
// init is 8928/558 = 16 *4 = 64 samples                 //
// try to take n = 96 or 128 ;o)                         //
// 48 will give you a bandwidth around 186 hz            //
// 64 will give you a bandwidth around 140 hz            //
// 96 will give you a bandwidth around 94 hz             //
// 128 will give you a bandwidth around 70 hz            //
// BUT remember that high n take a lot of time           //
// so you have to find the compromice - i use 48         //
///////////////////////////////////////////////////////////

float coeff;
float Q1 = 0;
float Q2 = 0;
float sine;
float cosine;  
float sampling_freq=8928.0;
float target_freq=TARGET_FREQ; /// adjust for your needs see above

#if defined(USE_BOARD_M5UNIFIED)
	int16_t 
#else
	int 
#endif
testData[NUMOF_TESTDATA];
int /* float */ n=sizeof(testData)/sizeof(testData[0]);

//////////////////////////////
// Noise Blanker time which //
// shall be computed so     //
// this is initial          //
//////////////////////////////
int nbtime = NBTIME_MS;  /// ms noise blanker         

long starttimehigh;
long highduration;
long lasthighduration;
long hightimesavg;
long lowtimesavg;
long startttimelow;
long lowduration;
long laststarttime = 0;

char code[20];
int stop = LOW;
int wpm=20;		/// int wpm; 

#ifdef	USE_MEASURE_SAMPLING_FREQ
static float get_sampling_freq(long times)
{
	unsigned long us = micros();
	for(long i = 0; i < times; i++) {
		analogRead(audioInPin);
	}
	us = micros() - us;

	float period = 1e-6*us/times;

	return 1.0f/period;
}
#endif

////////////////
// init setup //
////////////////
void setup() {

#if !defined(USE_BOARD_M5UNIFIED)
	#ifdef	USE_MEASURE_SAMPLING_FREQ
		sampling_freq = get_sampling_freq(1000);
	#endif
	
	////////////////////////////////////
	// The basic goertzel calculation //
	////////////////////////////////////
	int k;
	float omega;
	k = (int) (0.5 + ((n * target_freq) / sampling_freq));
	omega = (2.0 * PI * k) / n;
	sine = sin(omega);
	cosine = cos(omega);
	coeff = 2.0 * cosine;
#endif

#if	defined(USE_LCD_RS_4BIT_20X4) ///	Orignal decoder11.ino
	///////////////////////////////
	// define special characters //
	///////////////////////////////
	lcd.createChar(0, U_umlaut); //     German
	lcd.createChar(1, O_umlaut); //     German, Swedish
	lcd.createChar(2, A_umlaut); //     German, Swedish 
	lcd.createChar(3, AE_capital); //   Danish, Norwegian
	lcd.createChar(4, OE_capital); //   Danish, Norwegian
	lcd.createChar(5, fullblock);        
	lcd.createChar(6, AA_capital); //   Danish, Norwegian, Swedish
	lcd.createChar(7, emtyblock); 
	lcd.clear(); 
#endif


#if defined(USE_BOARD_M5UNIFIED)
	m5un_setup(target_freq, sampling_freq, NUMOF_TESTDATA, MAGNITUDE_SMOOTHING_UP, MAGNITUDE_SMOOTHING_DOWN);
#else
	Serial.begin(115200); 
	pinMode(ledPin, OUTPUT);
#endif

#if defined(USE_LCD_RS_4BIT_20X4)
	lcd.begin(colums, rows); 
#elif	defined(USE_LCD_GROVE_RGB_BACKLIGHT)	
	lcd.begin(colums, rows); 

	lcd.setRGB(64, 64, 64);		// R, G, B
	///////////////////////////////
	// define special characters //
	///////////////////////////////
	lcd.createChar(0, U_umlaut); //     German
	lcd.createChar(1, O_umlaut); //     German, Swedish
	lcd.createChar(2, A_umlaut); //     German, Swedish 
	lcd.createChar(3, AE_capital); //   Danish, Norwegian
	lcd.createChar(4, OE_capital); //   Danish, Norwegian
	lcd.createChar(5, fullblock);        
	lcd.createChar(6, AA_capital); //   Danish, Norwegian, Swedish
	lcd.createChar(7, emtyblock); 
	lcd.clear(); 
#elif defined(USE_LCD_SSD1306_128X64_HW_I2C)
	lcd.begin();
	lcd.setFlipMode(true);
  lcd.setPowerSave(0);
  lcd.setFont(u8x8_font_7x14_1x2_f);  // u8x8_font_chroma48medium8_r);
#endif

#if defined(USE_BOARD_M5UNIFIED)
#else
	for (int index = 0; index < colums; index++){
		line1[index] = 32;
		line2[index] = 32;
	}           
#endif

#ifdef	USE_MEASURE_SAMPLING_FREQ
	Serial.print("\nsampling_freq = ");
	Serial.print(sampling_freq);
	Serial.println(" (Hz)");
#endif

}

///////////////
// main loop //
///////////////
void loop() {

	///////////////////////////////////// 
	// The basic where we get the tone //
	/////////////////////////////////////
#if defined(USE_BOARD_M5UNIFIED)
	static int16_t recBuf[NUMOF_TESTDATA];
	M5.Mic.record(recBuf, sizeof(recBuf)/sizeof(recBuf[0]), sampling_freq);
	memcpy(testData, recBuf, sizeof(testData));

	bpf->filter(testData, testData, n);
	agc->process(testData, testData, n);

	magnitude = goertzel->getMagnitude(testData);
	
	/////////////////////////////////////////////////////////// 
	// here we will try to set the magnitude limit automatic //
	///////////////////////////////////////////////////////////

	smoother->smooth(&magnitudelimit, &magnitude, 1);

	magnitudelimit = max(magnitudelimit, magnitudelimit_low);
#else
	for (char index = 0; index < n; index++){
		testData[index] = analogRead(audioInPin);
	}
	
	for (char index = 0; index < n; index++){
		float Q0;
		Q0 = coeff * Q1 - Q2 + (float) testData[index];
		Q2 = Q1;
		Q1 = Q0;  
	}
	float magnitudeSquared = (Q1*Q1)+(Q2*Q2)-Q1*Q2*coeff;  // we do only need the real part //
	magnitude = sqrt(magnitudeSquared);
	Q2 = 0;
	Q1 = 0;

	//Serial.print(magnitude); Serial.println();  //// here you can measure magnitude for setup..
	
	/////////////////////////////////////////////////////////// 
	// here we will try to set the magnitude limit automatic //
	///////////////////////////////////////////////////////////
	
	if (magnitude > magnitudelimit_low){
		magnitudelimit = (magnitudelimit +((magnitude - magnitudelimit)/MAGNITUDE_FILTER_DIV));		/// moving average filter
	}
 
	if (magnitudelimit < magnitudelimit_low)
		magnitudelimit = magnitudelimit_low;
#endif
	
	////////////////////////////////////
	// now we check for the magnitude //
	////////////////////////////////////

	if(magnitude > magnitudelimit*MAGNITUDE_THRESHOLD)	// just to have some space up 
		 realstate = HIGH; 
	else
		realstate = LOW; 
	
	///////////////////////////////////////////////////// 
	// here we clean up the state with a noise blanker //
	/////////////////////////////////////////////////////
 
	if (realstate != realstatebefore){
		laststarttime = millis();
	}
	if ((millis()-laststarttime)> nbtime){
		if (realstate != filteredstate){
			filteredstate = realstate;
		}
	}
 
	////////////////////////////////////////////////////////////
	// Then we do want to have some durations on high and low //
	////////////////////////////////////////////////////////////
 
	if (filteredstate != filteredstatebefore){
		if (filteredstate == HIGH){
			starttimehigh = millis();
			lowduration = (millis() - startttimelow);
		}

		if (filteredstate == LOW){
			startttimelow = millis();
			highduration = (millis() - starttimehigh);
			if (highduration < (2*hightimesavg) || hightimesavg == 0){
				hightimesavg = (highduration+hightimesavg+hightimesavg)/3;     // now we know avg dit time ( rolling 3 avg)
			}
			if (highduration > (5*hightimesavg) ){
				hightimesavg = highduration+hightimesavg;     // if speed decrease fast ..
			}
		}
	}

 ///////////////////////////////////////////////////////////////
 // now we will check which kind of baud we have - dit or dah //
 // and what kind of pause we do have 1 - 3 or 7 pause        //
 // we think that hightimeavg = 1 bit                         //
 ///////////////////////////////////////////////////////////////
 
	if (filteredstate != filteredstatebefore){
		stop = LOW;
		if (filteredstate == LOW){  //// we did end a HIGH
			if (highduration < (hightimesavg*2) && highduration > (hightimesavg*0.6)){ /// 0.6 filter out false dits
				strcat(code,".");
				Serial.print(".");
			}
			if (highduration > (hightimesavg*2) && highduration < (hightimesavg*6)){ 
				strcat(code,"-");
				Serial.print("-");
				wpm = (wpm + (1200/((highduration)/3)))/2;  //// the most precise we can do ;o)
				wpm = min(max(wpm, 8), 40);		/// limit
			}
		}
 
		if (filteredstate == HIGH){  //// we did end a LOW
		 
			float lacktime = 1;
			if(wpm > 25)lacktime=1.0; ///  when high speeds we have to have a little more pause before new letter or new word 
			if(wpm > 30)lacktime=1.2;
			if(wpm > 35)lacktime=1.5;

			if (lowduration > (hightimesavg*(2*lacktime)) && lowduration < hightimesavg*(5*lacktime)){ // letter space
				docode();
				code[0] = '\0';
				Serial.print("/");
			}
			if (lowduration >= hightimesavg*(5*lacktime)){ // word space
				docode();
				code[0] = '\0';
				printascii(32);
				Serial.println();
			}
		}
	}
 
	//////////////////////////////
	// write if no more letters //
	//////////////////////////////

	if ((millis() - startttimelow) > (highduration * 6) && stop == LOW){
		docode();
		code[0] = '\0';
		stop = HIGH;
	}

#if !defined(USE_BOARD_M5UNIFIED)
	/////////////////////////////////////
	// we will turn on and off the LED //
	// and the speaker                 //
	/////////////////////////////////////

	if(filteredstate == HIGH){ 
		digitalWrite(ledPin, HIGH);
		tone(audioOutPin,target_freq);
	}
	else{
		digitalWrite(ledPin, LOW);
		noTone(audioOutPin);
	}
#endif

	//////////////////////////////////
	// the end of main loop clean up//
	/////////////////////////////////
	updateinfolinelcd();
	realstatebefore = realstate;
	lasthighduration = highduration;
	filteredstatebefore = filteredstate;
}


////////////////////////////////
// translate cw code to ascii //
////////////////////////////////

void docode(){
#if defined(USE_BOARD_M5UNIFIED)
	m5un_docode(code);
#else
	if (strcmp(code,".-") == 0) printascii(65);
	if (strcmp(code,"-...") == 0) printascii(66);
	if (strcmp(code,"-.-.") == 0) printascii(67);
	if (strcmp(code,"-..") == 0) printascii(68);
	if (strcmp(code,".") == 0) printascii(69);
	if (strcmp(code,"..-.") == 0) printascii(70);
	if (strcmp(code,"--.") == 0) printascii(71);
	if (strcmp(code,"....") == 0) printascii(72);
	if (strcmp(code,"..") == 0) printascii(73);
	if (strcmp(code,".---") == 0) printascii(74);
	if (strcmp(code,"-.-") == 0) printascii(75);
	if (strcmp(code,".-..") == 0) printascii(76);
	if (strcmp(code,"--") == 0) printascii(77);
	if (strcmp(code,"-.") == 0) printascii(78);
	if (strcmp(code,"---") == 0) printascii(79);
	if (strcmp(code,".--.") == 0) printascii(80);
	if (strcmp(code,"--.-") == 0) printascii(81);
	if (strcmp(code,".-.") == 0) printascii(82);
	if (strcmp(code,"...") == 0) printascii(83);
	if (strcmp(code,"-") == 0) printascii(84);
	if (strcmp(code,"..-") == 0) printascii(85);
	if (strcmp(code,"...-") == 0) printascii(86);
	if (strcmp(code,".--") == 0) printascii(87);
	if (strcmp(code,"-..-") == 0) printascii(88);
	if (strcmp(code,"-.--") == 0) printascii(89);
	if (strcmp(code,"--..") == 0) printascii(90);

	if (strcmp(code,".----") == 0) printascii(49);
	if (strcmp(code,"..---") == 0) printascii(50);
	if (strcmp(code,"...--") == 0) printascii(51);
	if (strcmp(code,"....-") == 0) printascii(52);
	if (strcmp(code,".....") == 0) printascii(53);
	if (strcmp(code,"-....") == 0) printascii(54);
	if (strcmp(code,"--...") == 0) printascii(55);
	if (strcmp(code,"---..") == 0) printascii(56);
	if (strcmp(code,"----.") == 0) printascii(57);
	if (strcmp(code,"-----") == 0) printascii(48);

	if (strcmp(code,"..--..") == 0) printascii(63);
	if (strcmp(code,".-.-.-") == 0) printascii(46);
	if (strcmp(code,"--..--") == 0) printascii(44);
	if (strcmp(code,"-.-.--") == 0) printascii(33);
	if (strcmp(code,".--.-.") == 0) printascii(64);
	if (strcmp(code,"---...") == 0) printascii(58);
	if (strcmp(code,"-....-") == 0) printascii(45);
	if (strcmp(code,"-..-.") == 0) printascii(47);

	if (strcmp(code,"-.--.") == 0) printascii(40);
	if (strcmp(code,"-.--.-") == 0) printascii(41);
	if (strcmp(code,".-...") == 0) printascii(95);
	if (strcmp(code,"...-..-") == 0) printascii(36);
	if (strcmp(code,"...-.-") == 0) printascii(62);
	if (strcmp(code,".-.-.") == 0) printascii(60);
	if (strcmp(code,"...-.") == 0) printascii(126);
	
	//////////////////
	// The specials //
	//////////////////
	if (strcmp(code,".-.-") == 0) printascii(3);
	if (strcmp(code,"---.") == 0) printascii(4);
	if (strcmp(code,".--.-") == 0) printascii(6);

	/// Added by JJ1LFO
	if (strcmp(code,"-...-") == 0) printascii('=');
	if (strcmp(code,".-..-.") == 0) printascii('"');	
#endif
}

/////////////////////////////////////
// print the ascii code to the lcd //
// one a time so we can generate   //
// special letters                 //
/////////////////////////////////////
void printascii(int asciinumber){

#if defined(USE_BOARD_M5UNIFIED)
	m5un_printascii(asciinumber);
#else
	#if	defined(USE_LCD_RS_4BIT_20X4) || defined(USE_LCD_GROVE_RGB_BACKLIGHT)
		int fail = 0;
		if (rows == 4 and colums == 16)fail = -4; /// to fix the library problem with 4*16 display http://forum.arduino.cc/index.php/topic,14604.0.html

		if (lcdindex > colums-1){
			lcdindex = 0;
			if (rows==4){
				for (int i = 0; i <= colums-1 ; i++){
					lcd.setCursor(i,rows-3);
					lcd.write(line2[i]);
					line2[i]=line1[i];
				}
			}
			for (int i = 0; i <= colums-1 ; i++){
				lcd.setCursor(i+fail,rows-2);
				lcd.write(line1[i]);
				lcd.setCursor(i+fail,rows-1);
				lcd.write(32);
			}
		}
		line1[lcdindex]=asciinumber;
		lcd.setCursor(lcdindex+fail,rows-1);
		lcd.write(asciinumber);
		lcdindex += 1;
	#elif defined(USE_LCD_SSD1306_128X64_HW_I2C)
		///	USE_LCD_SSD1306_128X64_HW_I2C
		const int TEXT_HEIGHT = 2;
		static int text_row = 1;
		if(colums <= lcdindex) {
			line1[lcdindex] = '\0';
			lcdindex = 0;
			if(text_row < (rows - 1)) {
				text_row++;
			} else {
				lcd.setCursor(0, TEXT_HEIGHT*text_row);
				lcd.print("                ");	// 16 colums space
			}

			for(int src = 2; src <= text_row; src++) {
				if(text_row == src) {
					memcpy(line2, line1, sizeof(line2));
				}
				lcd.setCursor(0, TEXT_HEIGHT*(src - 1));
				lcd.print(line2);
			}
		}
		line1[lcdindex]=asciinumber;
		lcd.setCursor(lcdindex, TEXT_HEIGHT*text_row);
		lcd.print(line1[lcdindex]);
		lcdindex++;
	#endif
#endif
}

void updateinfolinelcd(){
	/////////////////////////////////////
	// here we update the upper line   //
	// with the speed.                 //
	/////////////////////////////////////

#if defined(USE_BOARD_M5UNIFIED)
	m5un_loop(wpm, filteredstate, magnitude, MAGNITUDE_THRESHOLD*magnitudelimit);
#else
	int place;
	if (rows == 4){
		place = colums/2;}
	else{
		place = 2;
	}
	if (wpm<10){
		lcd.setCursor((place)-2,0);
		lcd.print("0");
		lcd.setCursor((place)-1,0);
		lcd.print(wpm);
		lcd.setCursor((place),0);
		lcd.print(" WPM");
	}
	else{
		lcd.setCursor((place)-2,0);
		lcd.print(wpm);
		lcd.setCursor((place),0);
		lcd.print(" WPM ");
	}
#endif
}
