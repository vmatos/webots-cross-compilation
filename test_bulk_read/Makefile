###############################################################
#
# Purpose: Makefile for "test_set_position_timed"
# Author.: Vitor Matos
# Version: 0.1
# License: Give me a beer
#
###############################################################

TARGET = test_set_position_timed

INCLUDE_DIRS = -I../../include -I../../../Framework/include

CXX = g++
CXXFLAGS += -O2 -m32 -DLINUX -Wall $(INCLUDE_DIRS)
LFLAGS += -m32 -pthread -ljpeg -lrt

OBJECTS = ./main.cpp ./vsProfileLib.cpp

all: $(TARGET)

clean:
	rm -f *.a *.o $(TARGET) core *~ *.so *.lo

libclean:
	make -C ../../build clean

distclean: clean libclean

darwin.a:
	make -C ../../build

$(TARGET): darwin.a $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(OBJECTS) ../../lib/darwin.a -o $(TARGET)
	chmod 755 $(TARGET)
	

# useful to make a backup "make tgz"
tgz: clean
	mkdir -p backups
	tar czvf ./backups/DARwIn_demo_`date +"%Y_%m_%d_%H.%M.%S"`.tgz --exclude backups *
