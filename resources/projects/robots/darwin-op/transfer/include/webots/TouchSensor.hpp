/*******************************************************************************************************/
/* File:         TouchSensor.hpp                                                                       */
/* Date:         Jun 12                                                                                */
/* Description:  Wrapper of the TouchSensor Webots API for the DARwIn-OP real robot                    */
/* Author:       Vitor Matos                                                                           */
/*******************************************************************************************************/


#ifndef TOUCH_HPP
#define TOUCH_HPP

#include <webots/Robot.hpp>
#include <webots/Device.hpp>


namespace webots {
  class TouchSensor: public Device {
    public:
      TouchSensor(const std::string &name, const Robot *robot); //Use Robot::getTouchSensor() instead
      virtual ~TouchSensor() {}
      virtual void enable(int ms);
      virtual void disable();
      
      double getValue() const;
      
    private:
      double m_Value;
      void setValue(int intengerValue);

      friend int Robot::step(int milisec);
  };
}

#endif //TOUCH_HPP
