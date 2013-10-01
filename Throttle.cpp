/*
 * Throttle.cpp
 *
 * Parent class for all throttle controllers

 Copyright (c) 2013 Collin Kidder, Michael Neuweiler, Charles Galpin

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */ 
 
#include "Throttle.h"
 
Throttle::Throttle() : Device() 
{
	prefsHandler = new PrefHandler(EE_THROTTLE_START);
    throttleDetector = NULL;
	level = 0;
}

Throttle::~Throttle() 
{
  if ( throttleDetector != NULL ) 
  {
    delete throttleDetector;
    throttleDetector = NULL;
  }
}

void Throttle::handleTick() 
{
	Device::handleTick();
	if ( throttleDetector != NULL ) 
	{
	    throttleDetector->handleTick();
	}
}

DeviceType Throttle::getType()
{
	return DEVICE_THROTTLE;
}

int Throttle::getLevel() 
{
	return level;
}

void Throttle::setNumThrottlePots(uint8_t num) 
{
	// Currently only valid values are 1  and 2
	if (num < 1) num = 1;
	if (num > 2) num = 2;

	numThrottlePots = num;
}

uint8_t Throttle::getNumThrottlePots() 
{
	return numThrottlePots;
}

void Throttle::setSubtype(uint8_t num)
{
	// Currently only valid values are 1  and 2
	if (num < 1) num = 1;
	if (num > 2) num = 2;

	throttleSubType = num;
}

uint8_t Throttle::getSubtype()
{
	return throttleSubType;
}


//Give default versions that return 0. Override in a child class if you implement the throttle
int Throttle::getRawThrottle1() {return 0;}
int Throttle::getRawThrottle2() {return 0;}

// Return the tick interval for this throttle. Override in a child class
// if you use a different tick interval
uint32_t Throttle::getTickInterval() 
{
	return CFG_TICK_INTERVAL_POT_THROTTLE;
}

//a common function to all throttles that takes as input the throttle position percentage
//and outputs an output throttle percentage which is calculated based on the throttle and
//brake mapping parameters. 
void Throttle::mapThrottle(signed int inVal) 
{
	signed int range, temp;

	//Logger::debug("Entering mapThrottle: %i", inVal);

	if (inVal > 0) {
		if (throttleRegen != 0) {
			if (inVal <= throttleRegen) {
				range = throttleRegen;
				temp = range - inVal;
				level = (signed long) ((signed long) (-10) * throttleMaxRegen * temp / range);
			}
		}

		if (inVal >= throttleFwd) {
			if (inVal <= throttleMap) { //bottom 50% forward
				range = throttleMap - throttleFwd;
				temp = (inVal - throttleFwd);
				level = (signed long) ((signed long) (500) * temp / range);
			}
			else { //more than throttleMap
				range = 1000 - throttleMap;
				temp = (inVal - throttleMap);
				level = 500 + (signed int) ((signed long) (500) * temp / range);
			}
		}
	}
	else {
		if (brakeMaxRegen != 0) { //is the brake regen even enabled?
			int range = brakeMaxRegen - throttleMaxRegen; //we start the brake at ThrottleMaxRegen so the range is reduced by that amount
			if (range < 1) { //detect stupidity and abort
				level = 0;
				return;
			}
			level = (signed int) ((signed int) 10 * range * inVal) / (signed int) 1000;
			level -= 10 * throttleMaxRegen;
		}
	}
	//Logger::debug("level: %d", level);
}

void Throttle::detectThrottle() 
{
  if ( throttleDetector == NULL ) {
    throttleDetector = new ThrottleDetector(this);
  }
  throttleDetector->detect();
}

uint16_t Throttle::getT1Min()
{
	return throttleMin1;
}
void Throttle::setT1Min(uint16_t min) 
{
	throttleMin1 = min;
}

uint16_t Throttle::getT2Min()
{
	return throttleMin2;
}
void Throttle::setT2Min(uint16_t min) 
{
	throttleMin2 = min;
}

uint16_t Throttle::getT1Max()
{
	return throttleMax1;
}
void Throttle::setT1Max(uint16_t max) 
{
	throttleMax1 = max;
}

uint16_t Throttle::getT2Max()
{
	return throttleMax2;
}
void Throttle::setT2Max(uint16_t max) 
{
	throttleMax2 = max;
}

uint16_t Throttle::getRegenEnd()
{
	return throttleRegen;
}
void Throttle::setRegenEnd(uint16_t regen) 
{
	throttleRegen = regen;
}

uint16_t Throttle::getFWDStart()
{
	return throttleFwd;
}
void Throttle::setFWDStart(uint16_t fwd) 
{
	throttleFwd = fwd;
}

uint16_t Throttle::getMAP()
{
	return throttleMap;
}
void Throttle::setMAP(uint16_t map) 
{
	throttleMap = map;
}

uint16_t Throttle::getMaxRegen()
{
	return throttleMaxRegen;
}
void Throttle::setMaxRegen(uint16_t regen) 
{
	throttleMaxRegen = regen;
}

uint16_t Throttle::getMinRegen()
{
	return throttleMinRegen;
}
void Throttle::setMinRegen(uint16_t regen) 
{
	throttleMinRegen = regen;
}

void Throttle::saveConfiguration()
{
}

void Throttle::saveEEPROM()
{
}


//TODO: need to plant this in here somehow..

//if (!runStatic)
//	throttleval++;
//if (throttleval > 80)
//	throttleval = 0;
//if (!runThrottle) { //ramping test
//	if (!runRamp) {
//		throttleval = 0;
//	}
//	motorController->setThrottle(throttleval * (int) 12); //with throttle 0-80 this sets throttle to 0 - 960
//}
//else { //use the installed throttle
//}



