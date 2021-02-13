#pragma once
#include <Arduino.h>
#ifndef wsa
#define wsa

//possible device states
const int8_t OFF		= 0;
const int8_t ON			= 1;
const int8_t COOLING	= 2;
const int8_t IGNITION	= 3;
const int8_t ERROR		= -1;

//possible device error states
const char * const errors[] = {"no error", \
							   "ignition failure", \
							   "defective suction", \
							   "insufficient air intake", \
							   "water temperature", \
							   "out of pellets", \
							   "defective pressure switch", \
							   "?", \
							   "no current", \
							   "exhaust motor failure", \
							   "card surge", \
							   "date expired", \
							   "?", \
							   "suction regulating sensor", \
							   "overheating"} 

class myStove
{
	public:	
		//initialize connection
		bool 			myStove(Stream& stream, int baudrate = 115200);
		
		//target temperature
		int 			getTemperature();
		bool 			setTemperature(int8_t temperature);
		
		//blower level
		unsigned int 	getBlowerLevel();
		bool 			setBlowerLevel(int8_t level);
		
		//turn on/off
		bool			turnOn();
		bool			turnOff();
		
		//get device state
		int8_t			getState();
		
		//get device error state
		uint8_t			getError();	
	
	private:
		
		//the serial interface
		Stream* connection;
		
		//saved last stage from stove...
		int8_t 			lastState = -1;
		
		//function to communicate
		String			sendCommand(String command);
	
}


#endif