#include <webots/Gyro.hpp>
#include <webots/Robot.hpp>

using namespace webots;

Gyro::Gyro(const std::string &name, const Robot *robot) :
  Device(name, robot)
{ }

Gyro::~Gyro() { }

void Gyro::enable(int ms) { }

void Gyro::disable() { }

void Gyro::setValues(const int *integerValues) {
	for (int i=0; i<3; i++)
    mValues[i] = integerValues[i];
}

const double *Gyro::getValues() const {
  return mValues;
}
