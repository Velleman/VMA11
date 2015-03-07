#include <Wire.h>
#include "VMA11.h"

VMA11 radio;

void setup()
{
	Serial.begin(9600);
        radio.begin();
	radio.tuneFrequency(1021);
}

void loop()
{

  /* add main program code here */
	if (Serial.available())
	{
		switch (Serial.read())
		{
		case 'm':
//			radio.mute();
			break;
		case 'u':
	//		radio.unmute();
			break;
		default :
			break;
		}
	}
	delay(500);
}
