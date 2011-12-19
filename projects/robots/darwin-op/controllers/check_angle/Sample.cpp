#include "Sample.hpp"
#include <webots/Servo.hpp>
#include <webots/LED.hpp>
#include <webots/Camera.hpp>

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>

using namespace webots;
using namespace std;

static const char *servoNames[NSERVOS] = {
  "ShoulderR" /*ID1 */, "ShoulderL" /*ID2 */, "ArmUpperR" /*ID3 */, "ArmUpperL" /*ID4 */,
  "ArmLowerR" /*ID5 */, "ArmLowerL" /*ID6 */, "PelvYR"    /*ID7 */, "PelvYL"    /*ID8 */,
  "PelvR"     /*ID9 */, "PelvL"     /*ID10*/, "LegUpperR" /*ID11*/, "LegUpperL" /*ID12*/,
  "LegLowerR" /*ID13*/, "LegLowerL" /*ID14*/, "AnkleR"    /*ID15*/, "AnkleL"    /*ID16*/,
  "FootR"     /*ID17*/, "FootL"     /*ID18*/, "Neck"      /*ID19*/, "Head"      /*ID20*/
};
enum {
	ShoulderR /*ID1 */, ShoulderL /*ID2 */, ArmUpperR /*ID3 */, ArmUpperL /*ID4 */,
  ArmLowerR /*ID5 */, ArmLowerL /*ID6 */, PelvYR    /*ID7 */, PelvYL    /*ID8 */,
  PelvR     /*ID9 */, PelvL     /*ID10*/, LegUpperR /*ID11*/, LegUpperL /*ID12*/,
  LegLowerR /*ID13*/, LegLowerL /*ID14*/, AnkleR    /*ID15*/, AnkleL    /*ID16*/,
  FootR     /*ID17*/, FootL     /*ID18*/, Neck      /*ID19*/, Head,      /*ID20*/
	NUM_SERVOS
};

Sample::Sample():
    Robot()
{
  mTimeStep = getBasicTimeStep();
  
  mEyeLED = getLED("EyeLed");
  mHeadLED = getLED("HeadLed");
  mHeadLED->set(0x10C040);
  mCamera = getCamera("Camera");
  mCamera->enable(mTimeStep);
  
  for (int i=0; i<NSERVOS; i++)
    mServos[i] = getServo(servoNames[i]);

}

Sample::~Sample() {
}

void Sample::myStep() {
  int ret = step(mTimeStep);
  if (ret == -1)
    exit(EXIT_SUCCESS);
}

void Sample::wait(int ms) {
  double startTime = getTime();
  double s = (double) ms / 1000.0;
  while (s + startTime >= getTime())
    myStep();
}

// function containing the main feedback loop
void Sample::run() {
  // set eye led to green
  mEyeLED->set(0x10C040);
  
  wait(200);
  for (int i=0; i<NSERVOS; i++) {
  	wait(100);
    mServos[i]->setPosition(0.0);
  }  
  printf("All servos at 0.\n");
  wait(1000);
  while (true) {
  	mServos[LegUpperL]->setPosition(0.25);
  	wait(100);
    mServos[LegUpperR]->setPosition(-0.25);
    wait(100);
    mServos[LegLowerL]->setPosition(-0.5);
    wait(100);
    mServos[LegLowerR]->setPosition(0.5);
    wait(100);
    mServos[AnkleL]->setPosition(-0.25);
    wait(100);
    mServos[AnkleR]->setPosition(0.25);
    
    wait(100);
    mServos[FootL]->setPosition(0.2);
    wait(100);
    mServos[FootR]->setPosition(0.2);
    wait(100);
    mServos[PelvL]->setPosition(0.2);
    wait(100);
    mServos[PelvR]->setPosition(0.2);
    
    wait(100);
    mServos[ShoulderL]->setPosition(-1.54);
    wait(100);
    mServos[ShoulderR]->setPosition(1.54);
    
    wait(100);
    mServos[ArmUpperL]->setPosition(-.7);
    wait(100);
    mServos[ArmUpperR]->setPosition(.7);
    
    wait(100);
    mServos[ArmLowerL]->setPosition(1.54);
    wait(100);
    mServos[ArmLowerR]->setPosition(-1.54);
    
    wait(100);
    mServos[Neck]->setPosition(1.5);
    wait(200);
    mServos[Head]->setPosition(.5);
    
    printf("Check posture of robot\n");
    wait(10000);
    
    
    // step
    myStep();
  }
}
