#ifndef MESSAGE_H_
#define MESSAGE_H_

struct Message {
	
	enum MessageType {
		MODE_CHANGE, COLOR_CHANGE, FADE_SETTINGS, GAME_SEQUENCE, 
		ADVANCE_GAME, GAME_OVER
	};
	
	MessageType messageType;
	
	int mode;
	int bulbIndex;
	long color;
	int fadeType;
	int flashBulb;
	int sequenceIndex;
	int gameSequenceValue;
	int gameSequence[100];

};

#endif /* MESSAGE_H_ */