/*******************************************************************************************************/
/* File:         Gyro.hpp                                                                              */
/* Date:         Sep 11                                                                                */
/* Description:  Wrapper of the Gyro Webots API for the DARwIn-OP real robot                           */
/* Author:       fabien.rohrer@cyberbotics.com                                                         */
/* Copyright (c) 2011 Cyberbotics - www.cyberbotics.com                                                */
/*******************************************************************************************************/

#ifndef GYRO_HPP
#define GYRO_HPP

#include <webots/Device.hpp>
#include <webots/Robot.hpp>

namespace webots {
	class Robot;
  class Gyro: public Device  {
    public:
                    Gyro(const std::string &name, const Robot *robot); //Use Robot::getGyro() instead
      virtual      ~Gyro();
      virtual void  enable(int ms);
      virtual void  disable();
      const double *getValues() const;
      
     private:
			double				mValues[3];
			void 					setValues(const int *integerValues);
			
			friend int Robot::step(int milisec);
  };
}

#endif // GYRO_HPP
