#!/bin/sh

if [ -f Makefile ]
then
   OSTYPE=$(uname -s)
   case $OSTYPE in
	Linux)   echo Target is: ${OSTYPE}; sed -i 's/CC = $(BSD_CC)/CC = $(LINUX_CC)/g' Makefile; sed -i 's/CFLAGS = $(BSD_CFLAGS)/CFLAGS = $(LINUX_CFLAGS)/g' Makefile;;
	FreeBSD) echo Target is: ${OSTYPE}; sed -i "" 's/CC = $(LINUX_CC)/CC = $(BSD_CC)/g' Makefile; sed -i "" 's/CFLAGS = $(LINUX_CFLAGS)/CFLAGS = $(BSD_CFLAGS)/g' Makefile;;
	*)       echo unsupported;;
   esac
else
  echo "Makefile not found"
fi




