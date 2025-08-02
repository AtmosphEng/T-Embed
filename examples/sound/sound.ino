
#include "Audio.h" // https://github.com/schreibfaul1/ESP32-audioI2S
//#include "FS.h"
//#include "SPIFFS.h"
#include "pin_config.h"

#define MYDEBUGON
#define MYSERIALCHAN Serial2

#ifdef MYDEBUGON
  #define MYDEBUGPRINT(x)     		MYSERIALCHAN.print (x)
  #define MYDEBUGPRINTLN(x)  			MYSERIALCHAN.println (x)
  #define MYDEBUGPRINTDEC(x)  		MYSERIALCHAN.print (x, DEC)
  #define MYDEBUGPRINTDECLN(x)  	MYSERIALCHAN.println (x, DEC)
  #define MYDEBUGPRINTSTRING(x)  	MYSERIALCHAN.print ("%s", x)
  #define MYDEBUGWRITE(x)     		MYSERIALCHAN.write (x)
  #define MYDEBUGWRITEBUF(x,y)   	MYSERIALCHAN.write (x,y)
#else
  #define MYDEBUGPRINT(x)
  #define MYDEBUGPRINTLN(x) 
  #define MYDEBUGPRINTDEC(x)
  #define MYDEBUGPRINTDECLN(x)
  #define MYDEBUGPRINTSTRING(x)
  #define MYDEBUGWRITE(x)
  #define MYDEBUGWRITEBUF(x,y)
#endif

//#define DEF_SPIFFS // else its SD_MMC

Audio *audio; // https://github.com/schreibfaul1/ESP32-audioI2S

void setup() {

	bool ret = false;

	pinMode(PIN_POWER_ON, OUTPUT);
	digitalWrite(PIN_POWER_ON, HIGH);

	Serial.begin(115200);
	Serial2.begin(115200, SERIAL_8N1, 44, 43); // t-embed grove connector.
	delay(2000);

	MYDEBUGPRINT("\r\n"); // alternative to macros ending with ...LN
	MYDEBUGPRINT("psram size : ");
	MYDEBUGPRINTDECLN(ESP.getPsramSize() / 1024);

	MYDEBUGPRINT("FLASH size : ");
	MYDEBUGPRINTDECLN(ESP.getFlashChipSize() / 1024);

	audio = new Audio(0, 3, 1);
	audio->setPinout(PIN_IIS_BCLK, PIN_IIS_WCLK, PIN_IIS_DOUT);

	audio->setVolume(10); // 0...21
												// audio->setVolume(21); // 0...21


#ifdef DEF_SPIFFS
	//  *     ArduinoIDE only 1.8.19 - upload SPIFFS data with ESP32 Sketch Data Upload:
	//  *     ESP32: https://github.com/lorol/arduino-esp32fs-plugin
	//  *     Platformio
	// *      ENV -> sound -> Platform -> Upload Filesystem Image
	ret = SPIFFS.begin();
	while (!ret) {
		MYDEBUGPRINTLN("SPIFFS not mount . please Upload mp3 file");
		delay(1000);
	}
	MYDEBUGPRINTLN("SPIFFS mounted OK");

	// play spiffs
		ret = audio->connecttoFS(SPIFFS, "/sound1.mp3");
		if (!ret) {
			MYDEBUGPRINTLN("No find sound1.mp3.");
			while (1)
				delay(1000);
		}
	
	
#else // SD_MMC card
	// play sd music
	pinMode(PIN_SD_CS, OUTPUT);
	digitalWrite(PIN_SD_CS, 1);
	SD_MMC.setPins(PIN_SD_SCK, PIN_SD_MOSI, PIN_SD_MISO);
	if (!SD_MMC.begin("/sdcard", true)) {
		MYDEBUGPRINTLN("Card Mount Failed");
		return;
	} else
		MYDEBUGPRINTLN("Card Mount OK");

	uint8_t cardType = SD_MMC.cardType();
	if (cardType == CARD_NONE) {
		MYDEBUGPRINTLN("No SD card attached");
		return;
	} else
		MYDEBUGPRINTLN("SD card attached OK");

		//ret = audio->connecttoFS(SD, "/sound1.mp3");
		ret = audio->connecttoFS(SD_MMC, "/sound1.mp3");
		if(!ret){
		MYDEBUGPRINTLN("connecttoFS FAILED");
		return;
	}
#endif

	MYDEBUGPRINTLN("Now play sound ...");

} // setup

void loop() {
	delay(1);
	audio->loop();
}

// END_OF_FILE
