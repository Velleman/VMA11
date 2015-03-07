// Velleman VMA11 Code by BN
// Released to the public domain! Enjoy!
#ifndef VMA11_H
#define VMA11_H

#include <5B700.h>

//Define the SPI Pin Numbers
//#define SDA 18		//MOSI
//#define SCL  19		//MISO 
//#define ADDRESS 100	//Chips address

#define ON	true
#define OFF	false

// Main Class for VMA11 FM radio shield
class VMA11 {

#pragma region Public Methods
public:
	//This is just a constructor.
	VMA11();
	/*
	* Description:
	*	Initializes the 5B700, powers up the radio.
	* 	This function must be called before any other radio command.
	*/
	void begin();
	/*
	* Description:
	*	Used to send an ascii command string to the radio.
	* Parameters:
	*	myCommand - A null terminated ascii string limited to hexidecimal characters
	*				to be sent to the radio module. Instructions for building commands can be found
	*				in the Si4735 Programmers Guide.
	*/
	void sendCommand(char * myCommand);
	/*
	* Description:
	*	Used to to tune the radio to a desired frequency. The library uses the mode indicated in the
	* 	begin() function to determine how to set the frequency.
	* Parameters:
	*	frequency - The frequency to tune to, in kHz (or in 10kHz if using FM mode).
	* Returns:
	*	True
	* TODO:
	* 	Make the function return true if the tune was successful, else return false.
	*/
	bool tuneFrequency(int frequency);
	/*
	* Description:
	*	This function currently does not work!
	* TODO:
	*	Make this function work.
	*/
	int getFrequency(void);
	/*
	* Description:
	*	Commands the radio to seek up to the next valid channel. If the top of the band is reached, the seek
	*	will continue from the bottom of the band.
	* Returns:
	*	True
	* TODO:
	*	Make the function return true if a valid channel was found, else return false.
	*/
	bool seekUp(void);
	/*
	* Description:
	*	Commands the radio to seek down to the next valid channel. If the bottom of the band is reached, the seek
	*	will continue from the top of the band.
	* Returns:
	*	True
	* TODO:
	*	Make the function return true if a valid channel was found, else return false.
	*/
	bool seekDown(void);
	/*
	* Description:
	*	Increasese the volume by 1. If the maximum volume has been reached, no increase will take place.
	*/
	void volumeUp(void);
	/*
	* Description:
	*	Decreases the volume by 1. If the minimum volume has been reached, no decrease will take place.
	*/
	void volumeDown(void);
	/*
	* Description:
	*	Mutes the audio output
	*/
	void mute(void);
	/*
	* Description:
	*	Disables the mute.
	*/
	void unmute(void);
	/*
	* Description:
	*	Gets the current status of the radio. Learn more about the status in the Si4735 datasheet.
	* Returns:
	*	The status of the radio.
	*/
	char getStatus(void);
	/*
	* Description:
	*	Gets the long response (16 characters) from the radio. Learn more about the long response in the Si4735 datasheet.
	* Parameters:
	*	response - A string for the response from the radio to be stored in.
	*/
	void getResponse(char * response);
	/*
	* Description:
	*	Powers down the radio
	*/
	void end(void);
#pragma endregion

};
#endif
