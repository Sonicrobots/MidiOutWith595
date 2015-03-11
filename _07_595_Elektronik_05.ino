// 
// /~~\ /~~\ |\  |~|~/~~  |~~\ /~~\ |~~\ /~~\~~|~~/~~\
// `--.|    || \ | ||     |__/|    ||--<|    | |  `--.
// \__/ \__/ |  \|_|_\__  |  \ \__/ |__/ \__/  |  \__/
//
// Contributors: Moritz Simon Geist, LennartS
// 01/2015, v.05: Triggercontrol added
//
// === Recources: ===
// 1. 
// 2. MIDI Library https://github.com/FortySevenEffects/arduino_midi_library (used: c4.2.)
// 3. SimpleTimer Library http://playground.arduino.cc/Code/SimpleTimer
// 
// 

// does not get parsed when using Arduino IDE
#ifdef EXTERNAL_IDE
#include <Arduino.h>
int main(void) {
  init();
  setup();
  while(true) {
    loop();
  }
}
#endif


#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>
MIDI_CREATE_DEFAULT_INSTANCE();

// #include <SimpleTimer.h>
// SimpleTimer midichaos; // Send a midi OFF Note to all channels when no NoteOff has been send for X seconds

#include "TriggerManager.h"
extern TriggerManager triggers;



//  Set up dynamic behaviour for different 595 pins that are controlled by a TriggerManager
const uint8_t channels = 25;
 //             	595-PIN    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24
uint8_t preDelays[channels] = {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20  };
uint8_t holdTimes[channels] = {90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90  };



// Readout the Coded switch on PIN 2,4,5,7 on Startup. To set the MIDI Channel (1-16)
uint8_t readMidiChannel()
{
	pinMode(2, INPUT_PULLUP);pinMode(7, INPUT_PULLUP);pinMode(4, INPUT_PULLUP);pinMode(5, INPUT_PULLUP); // the pins for the coded channel switch with build in PULLUP resistors
	uint8_t i=0;
	bitWrite(i, 0, !digitalRead(5));
	bitWrite(i, 1, !digitalRead(7));
	bitWrite(i, 2, !digitalRead(2));
	bitWrite(i, 3, !digitalRead(4));

	return i+1; //+1 .. nullausgleich
}


void HandleNoteOn(byte channel, byte note, byte velocity) {
	//..should there be some mapping note --> 595-pin?
	triggers.setOn(note);
}

void HandleNoteOff(byte channel, byte note, byte velocity) {
	//needed or not?
}




void setup() {
        pinMode(13, OUTPUT); // what for?

        MIDI.setHandleNoteOn(HandleNoteOn); // 
        MIDI.setHandleNoteOff(HandleNoteOff); // 
        MIDI.begin(readMidiChannel());                     // listens on only channel wich is set up with the coded rotyr encoder

        triggers.init(channels,&preDelays[0],&holdTimes[0]);
}


void loop() {
	MIDI.read();
}


