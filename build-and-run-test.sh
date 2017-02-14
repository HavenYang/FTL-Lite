#!/bin/bash
# Config basic variables
BUILD_DIR=source
EXEC_BIN=lite
TEST_LOG=log.txt

# Color
RED='\033[0;31m'
GREEN='\033[0;32m'
NOCOLOR='\033[0m'

echo ""
echo "***********************************************"
echo "* Start compiling                             *"
echo "***********************************************"
cd $BUILD_DIR
make clean
rm -f $TEST_LOG
make

# Detect whether compile successful
if test -f $EXEC_BIN
then
	echo Compile success
else
	printf "${RED}Compile failed!${NOCOLOR}\n"
	exit 1
fi

echo ""
echo "***********************************************"
echo "* Start testing                               *"
echo "***********************************************"
./$EXEC_BIN | tee -a $TEST_LOG


echo ""
echo "***********************************************"
echo "* Checking test result                        *"
echo "***********************************************"
if grep -Fxq "ALL TESTS PASSED" $TEST_LOG
then
	printf "${GREEN}Build Successful, Congratulations!${NOCOLOR}\n"
else
	printf "${RED}Build Failure!${NOCOLOR}\n"
	exit 1
fi
