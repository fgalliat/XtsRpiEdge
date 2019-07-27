/***
	XtsRpiEdge - bitlash integration
	by Xtase - fgalliat @ Jul2019

	Copyright (C) 2008-2012 Bill Roy

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:
	
	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.

***/

#include "Arduino.h"

#include "bitlash.h"

#ifdef CORE_EDGE
  #define mp3Serial Serial2
#else
  #include "SoftwareSerial.h"
  SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
  #define mp3Serial softSerial
#endif


// @@@@@@@@@@@@@@@@@@@@@@@@@@
#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini myDFPlayer;

bool mp3Ok = false;
bool mp3Pause = false;

numvar playMp3(void) { 
	int trackNum = arg[-1];
	myDFPlayer.stop();
	myDFPlayer.play(trackNum);
	return 1;
}

numvar stopMp3(void) { 
	myDFPlayer.stop();
	return 1;
}

numvar pauseMp3(void) { 
	if ( !pauseMp3 ) {
		myDFPlayer.pause();
		mp3Pause = true;
	} else {
		myDFPlayer.start();
		mp3Pause = false;
	}
	return 1;
}

numvar volumeMp3(void) { 
	int volume = arg[-1];
	myDFPlayer.volume(volume);
	return 1;
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@



int virtTimer = 0;

// Declare a user function named "timer1" returning a numeric Bitlash value
//
numvar timer1(void) { 
	// return TCNT1; 	// return the value of Timer 1
	return virtTimer++;
}

bool local_echo = true;
numvar setEcho(void) { 
	local_echo = arg[-1] != 0;
	return local_echo ? 1 : 0;
}

void setup(void) {
  mp3Serial.begin(9600);
  if (!myDFPlayer.begin(mp3Serial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    mp3Ok = false;
  } else {
	Serial.println(F("DFPlayer Mini online."));
	myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
	myDFPlayer.volume(15);
	mp3Ok = true;
  }



	initBitlash(115200);		// must be first to initialize serial port

	// Register the extension function with Bitlash
	//		"timer1" is the Bitlash name for the function
	//		0 is the argument signature: takes 0 arguments
	//		(bitlash_function) timer1 tells Bitlash where our handler lives
	//
	addBitlashFunction("timer1", (bitlash_function) timer1);

	addBitlashFunction("echo", (bitlash_function) setEcho);
}

void loop(void) {
	runBitlash();
}

