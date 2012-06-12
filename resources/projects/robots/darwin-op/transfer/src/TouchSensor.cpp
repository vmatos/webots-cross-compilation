#include <webots/TouchSensor.hpp>

using namespace webots;

TouchSensor::TouchSensor(const std::string &name, const Robot *robot) :
  Device(name, robot)
{ }

void TouchSensor::enable(int ms) { }

void TouchSensor::disable() { }

double TouchSensor::getValue() const {
  return m_Value;
}

void TouchSensor::setValue(int intengerValue) {
  m_Value = intengerValue;
}
