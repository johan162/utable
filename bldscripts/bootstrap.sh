#!/bin/sh
# ------------------------------------------------------------------------------------------------------------
# Boostrap script for Ubuntu/Debian/Linux Mint. Only needs to run once after
# the respoitory has been downloaded from the server and not the installation
# file.
#
# This script will first make sure that all needed develipment packages are
# installed and then run the bootstrap for the autotools 
# ------------------------------------------------------------------------------------------------------------

if [ ! -f configure ];
then

WORKING_DIR=`pwd`

sudo apt-get -y -qq install autoconf
sudo apt-get -y -qq install automake
sudo apt-get -y -qq install libtool

touch ChangeLog 
autoreconf --install --symlink

if [ "$?" = 0 ]; then 

echo "--------------------------------------------------------------"
echo " DONE. Build environment is ready. "
echo " "
echo " You can now run \"./stdbuild.sh\" to build the test program "
echo "--------------------------------------------------------------"

else

echo "ERROR: Cannot setup build environment. Sorry."
echo "       Try running \"autoreconf --install --symlink\" manually."

fi

else
echo "Ignored. Bootstrap is only supposed to be run once."
echo "Try running ./stdbuild.sh to build the daemon."
fi

