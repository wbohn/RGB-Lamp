#ifndef _RECEIVER_h
#define _RECEIVER_h

#include "Message.h"
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class Receiver
{
 private:


 public:
	boolean messageReady;
	void processIncomingByte(char ch);
	void saveByte(char ch);
	void saveColorByte(char ch);
	void createMessage();
	void reset();
	Message getMessage();
};

#endif

