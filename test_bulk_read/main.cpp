/*
 * main.cpp
 * Fri 23 Dec 2011 04:37:14 PM WET  *  Created on: 2011. 1. 4.
 *      Author: Vitor Matos
 * 	Testing setting positions on the servos.
 * Based on demo code by Robotis
 */

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>

#include "CM730.h"
#include "LinuxDARwIn.h"
#include <MX28.h>

#include <iostream>

#include "vsProfileLib.h"

//#include "mjpg_streamer.h"

#define U2D_DEV_NAME0       "/dev/ttyUSB0"
#define U2D_DEV_NAME1       "/dev/ttyUSB1"

using namespace Robot;

LinuxCM730 linux_cm730(U2D_DEV_NAME0);
CM730 cm730(&linux_cm730);

void change_current_dir() {
    char exepath[1024] = {0};
    if(readlink("/proc/self/exe", exepath, sizeof(exepath)) != -1)
    {
        if(chdir(dirname(exepath)))
            fprintf(stderr, "chdir error!! \n");
    }
}

void sighandler(int sig) {
    exit(0);
}

/* 
* Imitating Webots wrappers
*/
CM730 *getCM730() {// getRobot()->getCM730()
	return &cm730;
}

void setPosition(int mID,double position) {
  CM730 *cm730 = getCM730();//getRobot()->getCM730();
  int value = MX28::Angle2Value(position*180.0/3.14);
	
  if(value >= 0 && value <= MX28::MAX_VALUE) {
    int error;
    cm730->WriteWord(mID, MX28::P_GOAL_POSITION_L, value, &error);
  }
}

/* 
* Imitating Loop
*/
int main(void) {
    signal(SIGABRT, &sighandler);
    signal(SIGTERM, &sighandler);
    signal(SIGQUIT, &sighandler);
    signal(SIGINT, &sighandler);

    change_current_dir();

    //////////////////// Framework Initialize ////////////////////////////

    if(MotionManager::GetInstance()->Initialize(&cm730) == false)  {
        linux_cm730.SetPortName(U2D_DEV_NAME1);
    }

    /////////////////////////////////////////////////////////////////////

    cm730.WriteByte(CM730::P_LED_PANNEL, 0x01|0x02|0x04, NULL);
	
	cm730.MakeBulkReadPacket();
	
    LinuxActionScript::PlayMP3("../../../Data/mp3/Thank you.mp3");

    // Wait for 2 seconds
    printf("Wait 2 seconds\n");
    sleep(1);
    printf("Wait 1 second\n");
    sleep(1);
    
    int joint_pos=0;
    while(1)  {
		{
		PROFILE("Loop");
		sleep(1);
		
			{
			PROFILE("bulk read");
			cm730.BulkRead();
			
				
			for(int i=1; i<JointData::NUMBER_OF_JOINTS ; i++) {
				joint_pos = cm730.m_BulkReadData[i].ReadWord(MX28::P_PRESENT_POSITION_L);
				printf("id: %d - pos: %d\n", i, joint_pos);
			}
			
			}
			
			{
		    PROFILE("DUMP_PRINT");
		    std::string s = VSProfileLib::DumpLevels();
		    std::cout << s << std::endl;
			}
		
		}
    }

    return 0;
}
