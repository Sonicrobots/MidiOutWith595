/*
 * TriggerManager.cpp
 *
 *  Created on: 11.03.2015
 *      Author: user
 */


#include "TriggerManager.h"
#include "portManipulations.h"



void TriggerManager::init(uint8_t numberChannels, uint8_t* preDelay, uint8_t* holdTime) {
  shiftRegFast::setup();

  for (uint8_t index=0; index<min(numberChannels,number595*8); index++) {
    setPreDelay(index,preDelay[index]);
    setHoldTime(index,holdTime[index]);
  }

  setAllOff();

  //setup timer
  TCCR0A = (1<<WGM01);
  TCCR0B = (1<<CS02) | (1<<CS00);
  TIMSK0 = (1<<OCIE0A);
  OCR0A  = F_CPU/1024/interruptFreq;
}

void TriggerManager::setPreDelay(uint8_t channel, uint8_t time) {
  preDelays[channel] = time;
}
void TriggerManager::setHoldTime(uint8_t channel, uint8_t time) {
  holdTimes[channel] = time;
}

void TriggerManager::update() {
  for (uint8_t index=0; index<number595; index++) {
    shiftRegFast::write_8bit(states[index]);
  }
  shiftRegFast::enableOutput();
}

void TriggerManager::setOn(uint8_t channel) {

  // do nothing if channel number is out of bounds
  if (channel > number595*8) return;

  // do nothing if channel is active
  if (toggleTimes[channel] > 0) return;

  toggleTimes[channel] = preDelays[channel];
}

bool TriggerManager::isChannelOn(uint8_t channel) {
  return states[channel/8] & (1<<(channel%8));
}


void TriggerManager::checkForToggle() {
  for (uint8_t index=0; index<number595*8; index++) {
    if (toggleTimes[index]>0){
      toggleTimes[index]--;
      if (toggleTimes[index] == 0) toggleChannel(index);
    }
  }

  // always update
  // check for nescessity doesn't make sense because
  // we need to set interrupt time for maximum
  // execution time of this function anyways
  update();
}

void TriggerManager::toggleChannel(uint8_t channel) {

  // if this was just the pre-delay, we need to load next trigger for switch off
  if (!isChannelOn(channel)) toggleTimes[channel] = holdTimes[channel];

  // invert state
  states[channel/8] = states[channel/8] ^ (1<<(channel%8));

}

void TriggerManager::setAllOff() {
  for (uint8_t index=0; index<number595; index++) {
    states[index] = 0;
  }

  for (uint8_t index=0; index<number595*3; index++) {
    toggleTimes[index] = 0;
  }
}

TriggerManager triggers;

//..check maximum execution time with oscilloscope maximum interrupt frequency
ISR(TIMER0_COMPA_vect) {
  triggers.checkForToggle();
}
