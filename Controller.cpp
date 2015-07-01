#include <FastLED.h>
#include "Controller.h"
#include "Message.h"
#include "Color.h"

static const int MODE_SOLID = 0;
static const int MODE_FADE = 1;
static const int MODE_GAME = 2;

int currentMode = MODE_SOLID;

unsigned long lastFadeMillis;

volatile int NUM_FADE_STEPS = 128;
unsigned long FADE_DELAY = 100; // ms
int fadeStep;

boolean fadingBack = false;

CRGB bulbColor[NUM_BULBS];
CRGB fadeTarget[NUM_BULBS];

CRGB leds[NUM_LEDS]; // leds to be displayed
int sequence[100];
int gameIndex = 0;

// default constructor
Controller::Controller() {
	FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, GRB>(leds, NUM_LEDS);
	FastLED.setBrightness(100);
} //Controller

// default destructor
Controller::~Controller() {
} //~Controller

CRGB argbToCrgb(long color) {
	/* converts ARGB long to CRGB struct */
	return  CRGB( ((color >> 16) & 0xFF), ((color >> 8) & 0xFF), (color & 0xFF));
}

CRGB getComplementaryColor(CRGB inColor) {
	/* returns "opposite" of given color by inverting each component */
	byte r = 255 - inColor.r;
	byte g = 255 - inColor.g;
	byte b = 255 - inColor.b;
	return CRGB(r, g, b);
}

CRGB getFadeColor(CRGB inColor, CRGB outColor, int i) {
	/* returns the color at step i in a fade from inColor to outColor */
	int redDiff = outColor.r - inColor.r;
	int greenDiff = outColor.g - inColor.g;
	int blueDiff = outColor.b - inColor.b;

	int r = inColor.r + i * redDiff / NUM_FADE_STEPS;
	int g = inColor.g + i * greenDiff / NUM_FADE_STEPS;
	int b = inColor.b + i * blueDiff / NUM_FADE_STEPS;
	CRGB output = CRGB(r, g, b);

	return output;
}

boolean fadeWaiting() {
	unsigned long currentMillis = millis();

	if (currentMillis - lastFadeMillis >= FADE_DELAY) {
		lastFadeMillis = currentMillis;
		return false;
	}	
	return true;
}

void incrementFade() {
	if (fadingBack) {
		fadeStep--;
		if (fadeStep <= 0) {
			fadeStep = 0;
			fadingBack = false;
		}
	} else {
		fadeStep++;	
		if (fadeStep >= NUM_FADE_STEPS) {
			fadeStep = NUM_FADE_STEPS;
			fadingBack = true;
		}
	}
}

void fade() {
	if (fadeWaiting()) {
		return;
	}
	for (int i = 0; i < NUM_BULBS; i++) {
		int startIndex = i * LEDS_PER_BULB;
		for (int j = startIndex; j < startIndex + LEDS_PER_BULB; j++) {
			leds[j] = getFadeColor(bulbColor[i], fadeTarget[i], fadeStep);
		}
	}
	incrementFade();
	leds[15] = CRGB::Black;
	FastLED.show();	
}

void startFade() {
	long currentMillis = millis();
	
	for (int i = 0; i < NUM_BULBS; i++) {
		fadeTarget[i] = getComplementaryColor(bulbColor[i]);
	}
	lastFadeMillis = currentMillis;
	fadeStep = 0;
	fade();
}

void Controller::loop() {
	if (currentMode == MODE_FADE)	 {
		fade();
	}
}

void Controller::reset() {
	fadingBack = false;
	gameIndex = 0;
}

void Controller::changeBulb(int index, long color) {
	
	bulbColor[index] = argbToCrgb(color);
	fadeTarget[index] = getComplementaryColor(argbToCrgb(color));
	
	int start = index * LEDS_PER_BULB;
	
	for (int i = start; i < start + LEDS_PER_BULB; i++) {
		if (currentMode == MODE_FADE) {
			leds[i] = getFadeColor(bulbColor[index], fadeTarget[index], fadeStep);
		} else {
		leds[i] = argbToCrgb(color);
		}
	}
	leds[15] = CRGB::Black;
	FastLED.show();		
}

void Controller::changeMode(int mode) {
	if (currentMode == mode) {
		/* Android has probably sent a modeChange message after recreating the screen,
		nothing needs to be done here */
		return;
	}
	
	currentMode = mode;	
	if (currentMode == MODE_FADE) {
		startFade();
	}
}

const int BLINK_STEPS = 1;
const int FAST_STEPS = 32;
const int MED_STEPS = 128;
const int SLOW_STEPS = 255;

const int FADE_BLINK = 0;
const int FADE_FAST = 1;
const int FADE_MED = 2;
const int FADE_SLOW = 3;

void Controller::changeFadeSettings(int fadeType) {
	switch (fadeType) {
		case FADE_BLINK:
			NUM_FADE_STEPS = BLINK_STEPS;
			return;	
		
		case FADE_FAST:
			NUM_FADE_STEPS = FAST_STEPS;
			return;
		
		case FADE_MED:
			NUM_FADE_STEPS = MED_STEPS;
			return;
		
		case FADE_SLOW:
			NUM_FADE_STEPS = SLOW_STEPS;
			return;
		default:
			NUM_FADE_STEPS = MED_STEPS;
			return;
	}
}

void Controller::addGameSequenceItem(int index, int value) {
	sequence[index] = value;
}

void Controller::flashBulb(int index) {
	int start = index * LEDS_PER_BULB;
	
	for (int i = start; i < start + LEDS_PER_BULB; i++) {
		leds[i] = CRGB::Black;
	}
	leds[15] = CRGB::Black;
	FastLED.show();	
	delay(500);
	for (int i = start; i < start + LEDS_PER_BULB; i++) {
		leds[i] = bulbColor[index];
	}	
	leds[15] = CRGB::Black;
	FastLED.show();
	delay(500);

}

void Controller::showCurrentGame() {
	for (int i = 0; i <= gameIndex; i++) {
		flashBulb(sequence[i]);
	}
	Serial.print('+');
	gameIndex++;
	if (gameIndex >= 100) {
		gameIndex = 0;
	}
}
void setBulb(int index) {
	int start = index * LEDS_PER_BULB;
	for (int i = start; i < start + LEDS_PER_BULB; i++) {
		leds[i] = bulbColor[index];
		if  (i == 15) {
			leds[i] = CRGB::Black;
		}
	}
}

void Controller::gameOver() {
	for (int i = 0; i < NUM_LEDS; i++) {
		leds[i] = CRGB::Red;
	}
	leds[15] = CRGB::Black;
	FastLED.show();
	delay(1000);
		
	for (int i = 0; i < NUM_BULBS; i++) {
		setBulb(i)	;
	}
	leds[15] = CRGB::Black;
	FastLED.show();
}