#include <FastLED.h>
#include "Receiver.h"
#include "Controller.h"
#include "Color.h"

Receiver receiver;
Controller controller;

boolean gameRunning;

void setup() {
	
	Serial.begin(9600);

	receiver = Receiver();
	controller = Controller();
	
	gameRunning = false;
}


void loop() {
	
	while (Serial.available()) {
		receiver.processIncomingByte(Serial.read());
	}
	
	if (receiver.messageReady) {
		processMessage(receiver.getMessage());
		receiver.reset();
	}
	controller.loop();
}

void advanceGame() {
	controller.showCurrentGame();
}

void processMessage(Message message) {
	
	if (message.messageType == Message::COLOR_CHANGE) {
		controller.changeBulb(message.bulbIndex, message.color);
	}
	
	if (message.messageType == Message::MODE_CHANGE) {
		controller.changeMode(message.mode);
	}
	
	if (message.messageType == Message::FADE_SETTINGS) {
		controller.changeFadeSettings(message.fadeType);
	}
	
	if (message.messageType == Message::GAME_SEQUENCE) {
		controller.reset();
		for (int i = 0; i < 100; i++) {
			controller.addGameSequenceItem(i, message.gameSequence[i]);
		}
		Serial.print('#');
	}
	if (message.messageType == Message::ADVANCE_GAME) {
		//Serial.println("Message advance");
		advanceGame();
	}
	if (message.messageType == Message::GAME_OVER) {
		controller.gameOver();
	}
}

