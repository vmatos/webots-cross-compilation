#!/bin/bash
#
# Vitor Matos - 2012.12.19
#
# script for copying and compiling the controllers
# 

CONTROLLER=check_angle
GIT_REPOSITORY=/home/hazard/Webots/webots-cross-compilation

while true; do
	echo 
	read -p " Copy webots project to Darwin's framework dir on local machine? (y/n/q) " yn
	case $yn in
		  [Yy]* ) 
		  				cp -R -v $GIT_REPOSITORY/projects/robots/darwin-op/controllers $DARWINOP/Linux/project/webots;
		  				break;;
		  [Nn]* ) break;;
		  [Qq]* ) exit;;
		  * ) echo " Please answer y/n/q.";;
	esac
done

# Compile the wrapper library
while true; do
	echo 
	read -p " Compile wrapper library on local machine? (y/n/q) " yn
	case $yn in
		  [Yy]* ) make -C $DARWINOP/Linux/project/webots/transfer/lib clean;
						  make -C $DARWINOP/Linux/project/webots/transfer/lib;
		  				break;;
		  [Nn]* ) break;;
		  [Qq]* ) exit;;
		  * ) echo " Please answer y/n/q.";;
	esac
done

# Compile the managers library
while true; do
	echo 
	read -p " Compile Darwin's managers library on local machine? (y/n/q) " yn
	case $yn in
		  [Yy]* ) make -C $DARWINOP/Linux/project/webots/lib -f Makefile.darwin-op clean;
						  make -C $DARWINOP/Linux/project/webots/lib clean -f Makefile.darwin-op;
		  				break;;
		  [Nn]* ) break;;
		  [Qq]* ) exit;;
		  * ) echo " Please answer y/n/q.";;
	esac
done

# Compile the controller
while true; do
	echo 
	read -p " Compile Darwin's controller on local machine? (y/n/q) " yn
	case $yn in
		  [Yy]* ) make -C $DARWINOP/Linux/project/webots/controllers/$CONTROLLER -f Makefile.darwin-op clean;
						  make -C $DARWINOP/Linux/project/webots/controllers/$CONTROLLER -f Makefile.darwin-op;
		  				break;;
		  [Nn]* ) break;;
		  [Qq]* ) exit;;
		  * ) echo " Please answer y/n/q.";;
	esac
done

