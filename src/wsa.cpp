//initial

//possible return-states from "RD90005f&"
/*const char * const	COOLING1	= "0802002a";
const char * const	COOLING2	= "08010029";
const char * const	STATE_OFF	= "00000020";
const char * const	IGNITION	= "01010022";
const char * const	STATE_ON	= "02010023";*/

bool myStove::myStove(Stream& stream, int baudrate) {
	connection = &stream;
	if (!connection.begin(baudrate)) { 
		lastState = -1;
		return false; 
	}
	
	if (getState() > -1) {
		return true;
	} else {
		return false;
	}
	
	
}

int8_t myStove::getState() {
	
	String ret_str = sendCommand("RD90005f");
	
	/*
	//possible device states
	const int8_t OFF		= 0;
	const int8_t ON			= 1;
	const int8_t COOLING	= 2;
	const int8_t IGNITION	= 3;
	const int8_t ERROR		= -1;
	*/
	
	if (ret_str.indexOf("0802002a") > -1) { return 2; }
	if (ret_str.indexOf("08010029") > -1) { return 2; }
	if (ret_str.indexOf("00000020") > -1) { return 0; }
	if (ret_str.indexOf("01010022") > -1) { return 3; }
	if (ret_str.indexOf("02010023") > -1) { return 1; }	
	
	return t_state;
}

String sendCommand(String command) {
	String ret_str = "";
	String send_str = "\x1b" + command + "&";
	flushInputBuff();
	
	connection->print(send_str);
	
	ret_str = connection->readString();
	
	if ((ret_str != NULL) and (ret_str.length > 0)) {
		return ret_str;
	} else {	
		return NULL;
	}
}