/* Arduino Si4735 Library
* Written by Ryan Owens for SparkFun Electronics
* 5/17/11
*
* This library is for use with the SparkFun Si4735 Shield
* Released under the 'Buy Me a Beer' license
* (If we ever meet, you buy me a beer)
*
* See the header file for better function documentation.
*
* See the example sketches to learn how to use the library in your code.
*/

#include "VMA11.h"


//This is just a constructor.
VMA11::VMA11(){
	//Do Nothing
}


void VMA11::begin(){
	typFREQ freq;
	freq.word = 10210;
	StartingSequence(freq);
}

void VMA11::sendCommand(char * myCommand){
	
}

/*
* Description: Tunes the Si4735 to a frequency
*
* Params: int frequency - The desired frequency in kHz
*
* Returns: True if tune was successful
*			False if tune was unsuccessful
*/
bool VMA11::tuneFrequency(int frequency){
	
}

//This function does not work yet!
int VMA11::getFrequency(void){
	return 0;
}

bool VMA11::seekUp(void){
	
}

bool VMA11::seekDown(void){
	
}

void VMA11::volumeUp(void){
	
}

void VMA11::volumeDown(void){
	
}

void VMA11::mute(void){
	
}

void VMA11::unmute(void){
	
}

char VMA11::getStatus(void){
	
}
void VMA11::getResponse(char * response){
	
}

void VMA11::end(void){
	
}



