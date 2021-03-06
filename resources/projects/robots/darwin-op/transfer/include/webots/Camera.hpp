/*******************************************************************************************************/
/* File:         Camera.hpp                                                                            */
/* Date:         Sep 11                                                                                */
/* Description:  Wrapper of the Camera Webots API for the DARwIn-OP real robot                         */
/* Author:       fabien.rohrer@cyberbotics.com                                                         */
/* Copyright (c) 2011 Cyberbotics - www.cyberbotics.com                                                */
/*******************************************************************************************************/

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <webots/Device.hpp>

namespace webots {
  class Camera: public Device  {
    public:
                   Camera(const std::string &name, const Robot *robot); //Use Robot::getCamera() instead
      virtual     ~Camera();

      virtual void enable(int ms);
      virtual void disable();
      const unsigned char *getImage() const;
      int getWidth() const;
      int getHeight() const;
      double getFov() const;
      int getType() const;
      double getNear() const;
      static unsigned char imageGetRed(const unsigned char *image, int width, int x,int y);
      static unsigned char imageGetGreen(const unsigned char *image, int width, int x,int y);
      static unsigned char imageGetBlue(const unsigned char *image, int width, int x,int y);
      static unsigned char imageGetGrey(const unsigned char *image, int width, int x,int y);
    
    private:
      unsigned char          *mImage;
  };
}

#endif // CAMERA_HPP
