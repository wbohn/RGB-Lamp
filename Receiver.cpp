/*
 * MyClass.cpp
 *
 * Created: 5/31/2015 9:47:57 PM
 * Author: William
 */ 

#include "Receiver.h"
#include "Message.h"

static const char MODE_CHANGE = '@';
static const char FADE_TYPE_CHANGE = '#';
static const char START_LAMP_CHANGE = '<';
static const char END_LAMP_CHANGE = '>';
static const char START_COLOR = ':';
static const char START_GAME_SEQUENCE = '[';
static const char END_GAME_SEQUENCE = ']';
static const char GAME_SEQUENCE_DELIM = ',';
static const char ADVANCE_GAME = '+';
static const char GAME_OVER = '*';

static const char NEGATIVE_SIGN = '-';

boolean receivingMode;
boolean receivingColorChange;
boolean receivingFadeType;
boolean receivingGameSequence;
boolean receivingAdvance;
boolean receivingColor;
boolean receivingBulbIndex;
boolean receivingGameOver;

boolean negative = false;

long incomingColor = 0;
int incomingMode = 0;
int incomingBulbIndex = 0;
int incomingFadeType = 0;
int incomingFlashBulb = 0;

int incomingSequenceIndex = 0;
int incomingSequenceValue = 0;

int incomingSequence[100];
Message message;

void Receiver::processIncomingByte(char ch) {
	//Serial.print(ch);
	switch (ch)	 {
		
		case GAME_OVER:
			receivingGameOver = true;
			createMessage();
			return;
			
		case MODE_CHANGE:
			receivingMode = true;
			return;
		
		case FADE_TYPE_CHANGE:
			receivingFadeType = true;
			return;
			
		case START_LAMP_CHANGE:
			receivingColorChange = true;
			receivingBulbIndex = true;
			return;	
		case START_COLOR:
			incomingColor = 0;
			receivingColor = true;
			return;
		case END_LAMP_CHANGE:
			createMessage();		
			return;
		
		case START_GAME_SEQUENCE:
			receivingGameSequence = true;
			incomingSequenceIndex = 0;
			return;
			
		case GAME_SEQUENCE_DELIM:
			incomingSequenceIndex++;	
			return;
		case END_GAME_SEQUENCE:
			createMessage();
			return;		
		case ADVANCE_GAME:
			receivingAdvance = true;
			createMessage();
			return;
		default:
			saveByte(ch);
	}
}

void Receiver::saveByte(char ch) {
	if (ch == ' ') {
		return;
	}
	if (receivingMode) {
		incomingMode = (ch - '0');
		createMessage();
	}
	if (receivingFadeType) {
		incomingFadeType = (ch - '0');
		createMessage();
	}
	if (receivingBulbIndex) {
		incomingBulbIndex = (ch - '0');
		receivingBulbIndex = false;
	}
	if (receivingColor) {
		saveColorByte(ch);
	}
	if (receivingGameSequence) {
		message.gameSequence[incomingSequenceIndex] = ch - '0';
		incomingSequence[incomingSequenceIndex] = (ch - '0');
		incomingSequenceValue = (ch - '0');
	}
}

void Receiver::saveColorByte(char ch) {
	if (ch == NEGATIVE_SIGN) {
		negative = true;
		return;
	}
	incomingColor *= 10;
	incomingColor += (ch - '0');	
}

void Receiver::createMessage() {
	if (receivingGameOver) {
		message.messageType = Message::GAME_OVER;
	}
	if (receivingMode) {
		message.messageType = Message::MODE_CHANGE;
	} 
	if (receivingColorChange) {
		message.messageType = Message::COLOR_CHANGE;
	} 
	if (receivingFadeType) {
		message.messageType = Message::FADE_SETTINGS;
	}
	if (receivingGameSequence) {
		message.messageType = Message::GAME_SEQUENCE;
	}
	if (receivingAdvance) {
		message.messageType = Message::ADVANCE_GAME;
	}

	message.mode = incomingMode;
	message.bulbIndex = incomingBulbIndex;
	message.sequenceIndex = incomingSequenceIndex;
	message.gameSequenceValue = incomingSequenceValue;
	message.fadeType = incomingFadeType;
	
	if (negative) {
		message.color = (- incomingColor);
	} else {
		message.color = incomingColor;
	}
	
	messageReady = true;	
}

void Receiver::reset() {
	messageReady = false;
	
	receivingAdvance = false;
	receivingMode = false;
	receivingFadeType = false;
	receivingBulbIndex = false;
	receivingColor = false;
	receivingGameSequence = false;
	receivingColorChange = false;
	receivingGameOver = false;
}

Message Receiver::getMessage() {	
	return message;
}