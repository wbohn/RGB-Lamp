/* 
* Controller.h
*
* Created: 6/1/2015 2:16:45 PM
* Author: William
*/


#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#define NUM_LEDS 30 // LED strip
#define DATA_PIN 6  //
#define CLOCK_PIN 7 //

class Controller
{
//variables
public:
	
protected:
private:

//functions
public:
	Controller();
	~Controller();
	void loop();
	void changeBulb(int index, long color);
	void changeMode(int mode);
	void changeFadeSettings(int fadeType);
	void checkFadeDelay();
	void addGameSequenceItem(int index, int item);
	void showCurrentGame();
	void flashBulb(int index);
	void reset();
	void gameOver();
protected:
private:

}; //Controller

#endif //__CONTROLLER_H__
