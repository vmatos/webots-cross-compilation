#include <webots/Robot.hpp>
#include <webots/Accelerometer.hpp>
#include <webots/Camera.hpp>
#include <webots/Gyro.hpp>
#include <webots/LED.hpp>
#include <webots/Servo.hpp>
#include <webots/TouchSensor.hpp>
#include <webots/Device.hpp>

#include <LinuxDARwIn.h>
#include <MX28.h>
#include <CM730.h>

#include <cmath>

#include <unistd.h>
#include <errno.h>
#include <libgen.h>
#include <time.h>

webots::Robot::Robot() {
  initDarwinOP();
  initDevices();
  gettimeofday(&mStart, NULL);
  mPreviousStepTime = 0.0;
}

webots::Robot::~Robot() {
}


int webots::Robot::step(int milisec) {
// -------- Actual time stepping --------
  // at which time did the loop end?
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &mEndLoop);
  // how much time is left to wait to perform the required milisec?

  // Calculate difference
  mDiffLoop.tv_sec = mEndLoop.tv_sec - mStartLoop.tv_sec; 
  mDiffLoop.tv_nsec = mEndLoop.tv_nsec - mStartLoop.tv_nsec; 
  
  /* Eliminate overflows. */ 
  while (mDiffLoop.tv_nsec > 1e9L) { 
    mDiffLoop.tv_nsec -= 1e9L; 
    mDiffLoop.tv_sec++; 
  } 
  /* Eliminate underflows. */ 
  while (mDiffLoop.tv_nsec < 0L) { 
    mDiffLoop.tv_nsec += 1e9L; 
    mDiffLoop.tv_sec--; 
  }

  mDiffLoop.tv_nsec = milisec*1000000L  - mDiffLoop.tv_nsec;
  if(mDiffLoop.tv_nsec <= 0) return 1; // no time to wait, execution is lagging
  
  // wait the remaining time
  while(nanosleep(&mDiffLoop,&mDiffLoop)==-1 && errno == EINTR); // repeat with the remaining time if interrupted by signal
  
  // save the starting time of the next loop
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &mStartLoop);
  
   
// -------- BulkRead --------
  mCM730->BulkRead();
  // get gyro from bulk read table from CM730 device
  int values[3];
  values[0] = mCM730->m_BulkReadData[::Robot::CM730::ID_CM].ReadWord(::Robot::CM730::P_GYRO_Z_L);
  values[1] = mCM730->m_BulkReadData[::Robot::CM730::ID_CM].ReadWord(::Robot::CM730::P_GYRO_Y_L);
  values[2] = mCM730->m_BulkReadData[::Robot::CM730::ID_CM].ReadWord(::Robot::CM730::P_GYRO_X_L);
  ((Gyro *)mDevices["Gyro"])->setValues(values);
  // get accelerometer from bulk read table from CM730 device
  values[0] = mCM730->m_BulkReadData[::Robot::CM730::ID_CM].ReadWord(::Robot::CM730::P_ACCEL_X_L);
  values[1] = mCM730->m_BulkReadData[::Robot::CM730::ID_CM].ReadWord(::Robot::CM730::P_ACCEL_Y_L);
  values[2] = mCM730->m_BulkReadData[::Robot::CM730::ID_CM].ReadWord(::Robot::CM730::P_ACCEL_Z_L);
  ((Accelerometer *)mDevices["Accelerometer"])->setValues(values);
  
  // get FSR force values from bulk read table from CM730 device  
  int fsr_value;
  fsr_value = mCM730->m_BulkReadData[::Robot::FSR::ID_L_FSR].ReadWord(::Robot::FSR::P_FSR1_L);
  ((TouchSensor *)mDevices["FootL_FL_FSR"])->setValue(fsr_value);
  fsr_value = mCM730->m_BulkReadData[::Robot::FSR::ID_L_FSR].ReadWord(::Robot::FSR::P_FSR2_L);
  ((TouchSensor *)mDevices["FootL_FR_FSR"])->setValue(fsr_value);
  fsr_value = mCM730->m_BulkReadData[::Robot::FSR::ID_L_FSR].ReadWord(::Robot::FSR::P_FSR3_L);
  ((TouchSensor *)mDevices["FootL_BL_FSR"])->setValue(fsr_value);
  fsr_value = mCM730->m_BulkReadData[::Robot::FSR::ID_L_FSR].ReadWord(::Robot::FSR::P_FSR4_L);
  ((TouchSensor *)mDevices["FootL_BR_FSR"])->setValue(fsr_value);
  
  fsr_value = mCM730->m_BulkReadData[::Robot::FSR::ID_R_FSR].ReadWord(::Robot::FSR::P_FSR1_L);
  ((TouchSensor *)mDevices["FootR_FL_FSR"])->setValue(fsr_value);
  fsr_value = mCM730->m_BulkReadData[::Robot::FSR::ID_R_FSR].ReadWord(::Robot::FSR::P_FSR2_L);
  ((TouchSensor *)mDevices["FootR_FR_FSR"])->setValue(fsr_value);
  fsr_value = mCM730->m_BulkReadData[::Robot::FSR::ID_R_FSR].ReadWord(::Robot::FSR::P_FSR3_L);
  ((TouchSensor *)mDevices["FootR_BL_FSR"])->setValue(fsr_value);
  fsr_value = mCM730->m_BulkReadData[::Robot::FSR::ID_R_FSR].ReadWord(::Robot::FSR::P_FSR4_L);
  ((TouchSensor *)mDevices["FootR_BR_FSR"])->setValue(fsr_value);
  
  
  // Get joint positions from bulk read table of each MX28 device
  std::map<const std::string, int>::iterator servo_it;
  
  for(servo_it = Servo::mNamesToIDs.begin() ; servo_it != Servo::mNamesToIDs.end(); servo_it++  ) {
    ((Servo *) mDevices[(*servo_it).first])->setCurrentPosition( ::Robot::MX28::Value2Angle( 
        mCM730->m_BulkReadData[(*servo_it).second].ReadWord(::Robot::MX28::P_PRESENT_POSITION_L) 
      )*M_PI/180 );
  }

// -------- Sync Write to actuators --------
	const int msgLength = 3; // id + low byte + hight byte
	// latter we could probably include PID / maxForce / compliance parameters. 
	int param[20*msgLength]; 
	int n=0;
	int changed_servos=0;
	int value;
	
	for(servo_it = Servo::mNamesToIDs.begin() ; servo_it != Servo::mNamesToIDs.end(); servo_it++  ) {
		if( ((Servo *) mDevices[(*servo_it).first])->mTargetChanged == true ) {
			value = ::Robot::MX28::Angle2Value( ((Servo *) mDevices[(*servo_it).first])->getTargetPosition()*180.0/M_PI );
			if(value >= 0 && value <= ::Robot::MX28::MAX_VALUE) {
				param[n++] = (*servo_it).second;
				param[n++] = ::Robot::CM730::GetLowByte(value);
				param[n++] = ::Robot::CM730::GetHighByte(value);
				changed_servos++;
			}
		}
	}
	getCM730()->SyncWrite(::Robot::MX28::P_GOAL_POSITION_L, msgLength, changed_servos , param);	
	
  return 0;
}

std::string webots::Robot::getName() const {
  return "darwin-op";
}

double webots::Robot::getTime() const {
  struct timeval end;
  
  gettimeofday(&end, NULL);

  long seconds  = end.tv_sec  - mStart.tv_sec;
  long useconds = end.tv_usec - mStart.tv_usec;
  long mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

  return (double) mtime/1000.0;
}

int webots::Robot::getMode() const {
  return 1;
}

double webots::Robot::getBasicTimeStep() const {
  return 8.0;
}

webots::Device *webots::Robot::getDevice(const std::string &name) const {
  std::map<const std::string, webots::Device *>::const_iterator it = mDevices.find(name);
  if (it != mDevices.end())
    return (*it).second;
  return NULL;

}

webots::Accelerometer *webots::Robot::getAccelerometer(const std::string &name) const {
  webots::Device *device = getDevice(name);
  if (device) {
    webots::Accelerometer *accelerometer = dynamic_cast<webots::Accelerometer *> (device);
    if (accelerometer)
      return accelerometer;
  }
  return NULL;
}

webots::Camera *webots::Robot::getCamera(const std::string &name) const {
  webots::Device *device = getDevice(name);
  if (device) {
    webots::Camera *camera = dynamic_cast<webots::Camera *> (device);
    if (camera)
      return camera;
  }
  return NULL;
}

webots::Gyro *webots::Robot::getGyro(const std::string &name) const {
  webots::Device *device = getDevice(name);
  if (device) {
    webots::Gyro *gyro = dynamic_cast<webots::Gyro *> (device);
    if (gyro)
      return gyro;
  }
  return NULL;
}

webots::Servo *webots::Robot::getServo(const std::string &name) const {
  webots::Device *device = getDevice(name);
  if (device) {
    webots::Servo *servo = dynamic_cast<webots::Servo *> (device);
    if (servo)
      return servo;
  }
  return NULL;
}

webots::LED *webots::Robot::getLED(const std::string &name) const {
  webots::Device *device = getDevice(name);
  if (device) {
    webots::LED *led = dynamic_cast<webots::LED *> (device);
    if (led)
      return led;
  }
  return NULL;
}

webots::TouchSensor *webots::Robot::getTouchSensor(const std::string &name) const {
  webots::Device *device = getDevice(name);
  if (device) {
    webots::TouchSensor *touch = dynamic_cast<webots::TouchSensor *> (device);
    if (touch)
      return touch;
  }
  return NULL;
}

void webots::Robot::initDevices() {
  mDevices["Accelerometer"] = new webots::Accelerometer("Accelerometer", this);
  mDevices["Camera"]        = new webots::Camera       ("Camera",        this);
  mDevices["Gyro"]          = new webots::Gyro         ("Gyro",          this);
  mDevices["EyeLed"]        = new webots::LED          ("EyeLed",        this);
  mDevices["HeadLed"]       = new webots::LED          ("HeadLed",       this);
  mDevices["ShoulderR"]     = new webots::Servo        ("ShoulderR",     this);
  mDevices["ShoulderL"]     = new webots::Servo        ("ShoulderL",     this);
  mDevices["ArmUpperR"]     = new webots::Servo        ("ArmUpperR",     this);
  mDevices["ArmUpperL"]     = new webots::Servo        ("ArmUpperL",     this);
  mDevices["ArmLowerR"]     = new webots::Servo        ("ArmLowerR",     this);
  mDevices["ArmLowerL"]     = new webots::Servo        ("ArmLowerL",     this);
  mDevices["PelvYR"]        = new webots::Servo        ("PelvYR",        this);
  mDevices["PelvYL"]        = new webots::Servo        ("PelvYL",        this);
  mDevices["PelvR"]         = new webots::Servo        ("PelvR",         this);
  mDevices["PelvL"]         = new webots::Servo        ("PelvL",         this);
  mDevices["LegUpperR"]     = new webots::Servo        ("LegUpperR",     this);
  mDevices["LegUpperL"]     = new webots::Servo        ("LegUpperL",     this);
  mDevices["LegLowerR"]     = new webots::Servo        ("LegLowerR",     this);
  mDevices["LegLowerL"]     = new webots::Servo        ("LegLowerL",     this);
  mDevices["AnkleR"]        = new webots::Servo        ("AnkleR",        this);
  mDevices["AnkleL"]        = new webots::Servo        ("AnkleL",        this);
  mDevices["FootR"]         = new webots::Servo        ("FootR",         this);
  mDevices["FootL"]         = new webots::Servo        ("FootL",         this);
  mDevices["Neck"]          = new webots::Servo        ("Neck",          this);
  mDevices["Head"]          = new webots::Servo        ("Head",          this);
  mDevices["FootL_FL_FSR"]  = new webots::TouchSensor  ("FootL_FL_FSR",  this);
  mDevices["FootL_FR_FSR"]  = new webots::TouchSensor  ("FootL_FR_FSR",  this);
  mDevices["FootL_BL_FSR"]  = new webots::TouchSensor  ("FootL_BL_FSR",  this);
  mDevices["FootL_BR_FSR"]  = new webots::TouchSensor  ("FootL_BR_FSR",  this);
  mDevices["FootR_FL_FSR"]  = new webots::TouchSensor  ("FootR_FL_FSR",  this);
  mDevices["FootR_FR_FSR"]  = new webots::TouchSensor  ("FootR_FR_FSR",  this);
  mDevices["FootR_BL_FSR"]  = new webots::TouchSensor  ("FootR_BL_FSR",  this);
  mDevices["FootR_BR_FSR"]  = new webots::TouchSensor  ("FootR_BR_FSR",  this);
}

void webots::Robot::initDarwinOP() {
	char exepath[1024] = {0};
  if(readlink("/proc/self/exe", exepath, sizeof(exepath)) != -1)  {
      if(chdir(dirname(exepath)))
          fprintf(stderr, "chdir error!! \n");
  }
  
  mLinuxCM730 = new ::Robot::LinuxCM730("/dev/ttyUSB0");
  mCM730 = new ::Robot::CM730(mLinuxCM730);
  
  if(mCM730->Connect() == false) {
    printf("Fail to connect CM-730!\n");
    exit(EXIT_FAILURE);
  }

  mCM730->WriteByte(::Robot::CM730::P_LED_PANNEL, 0x01|0x02|0x04, NULL);
  
  mCM730->MakeBulkReadPacket();
//  ::Robot::MotionManager::GetInstance()->Initialize(mCM730);
//  ::Robot::LinuxMotionTimer::Initialize(::Robot::MotionManager::GetInstance());
//  ::Robot::LinuxActionScript::PlayMP3("../../../Data/mp3/Thank you.mp3");

}
