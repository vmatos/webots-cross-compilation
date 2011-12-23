#include "Sample.hpp"
#include <webots/Servo.hpp>
#include <webots/LED.hpp>
#include <webots/Camera.hpp>
#include <managers/DARwInOPMotionManager.hpp>
#include <managers/DARwInOPGaitManager.hpp>

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>

using namespace webots;
using namespace managers;
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
  ArmLowerR /*ID5 */, ArmLowerL /*ID6 */, PelvYR /*ID7 */, PelvYL /*ID8 */,
  PelvR /*ID9 */, PelvL /*ID10*/, LegUpperR /*ID11*/, LegUpperL /*ID12*/,
  LegLowerR /*ID13*/, LegLowerL /*ID14*/, AnkleR /*ID15*/, AnkleL /*ID16*/,
  FootR /*ID17*/, FootL /*ID18*/, Neck /*ID19*/, Head, /*ID20*/
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
  int  image_width=mCamera->getWidth();
  int  image_height=mCamera->getHeight();
  while (true) {
	
    if(getTime()>3 && getTime()<=(3+mTimeStep/1000.0)) {
			const unsigned char *image = mCamera->getImage();
			
			for (int x = 0; x < image_width; x++) {
				printf("[");
				for (int y = 0; y < image_height; y++) {
					int r = mCamera->imageGetRed(image, image_width, x, y);
//					int g = mCamera->imageGetGreen(image, image_width, x, y);
//					int b = mCamera->imageGetBlue(image,	image_width, x, y);
//					printf(" r=%d,g=%d,b=%d", r, g, b);
					printf(" ,%d", r);
				} 
				printf(" ]\n");
			}
    }
    // step
    myStep();
  }
}
