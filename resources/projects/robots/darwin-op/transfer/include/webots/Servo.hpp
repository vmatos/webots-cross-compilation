/*******************************************************************************************************/
/* File:         Servo.hpp                                                                             */
/* Date:         Sep 11                                                                                */
/* Description:  Wrapper of the Servo Webots API for the DARwIn-OP real robot                          */
/* Author:       fabien.rohrer@cyberbotics.com                                                         */
/* Copyright (c) 2011 Cyberbotics - www.cyberbotics.com                                                */
/*******************************************************************************************************/

#ifndef SERVO_HPP
#define SERVO_HPP

#include <webots/Robot.hpp>
#include <webots/Device.hpp>
#include <map>

namespace webots {
	class Robot;
  class Servo: public Device  {
    public:
                    Servo(const std::string &name, const Robot *robot); //Use Robot::getServo() instead
      virtual      ~Servo();
      virtual void  setPosition(double position);
      virtual double getPosition();
      virtual void	enablePosition(int ms);

    private:
			bool mTargetChanged;
			double mTargetPosition;
      double mCurrentPosition;
			
			double getTargetPosition();
      void setCurrentPosition(double current);
      
      static void   initStaticMap();
      static std::map<const std::string, int> mNamesToIDs;
      
      friend int Robot::step(int milisec);
  };
}

#endif //SERVO_HPP
