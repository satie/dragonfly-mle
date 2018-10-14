#!/bin/sh

if [ -f Makefile ]
then
   OSTYPE=$(uname -s)
   case $OSTYPE in
	Linux)  sed -i 's/^CC =.*/CC = $(LINUX_CC)/g' Makefile
          sed -i 's/^CFLAGS =.*/CFLAGS = $(LINUX_CFLAGS)/g' Makefile
          sed -i 's/^GIT_VERSION.*/GIT_VERSION = $(shell git describe --abbrev=4 --dirty --always --tags)/g' Makefile
           ;;
	FreeBSD) sed -i "" 's/^CC =.*/CC = $(LINUX_CC)/g' Makefile
           sed -i "" 's/^CFLAGS =.*/CFLAGS = $(LINUX_CFLAGS)/g' Makefile
           sed -i "" 's/^GIT_VERSION.*/GIT_VERSION = $(shell git describe --abbrev=4 --dirty --always --tags)/g' Makefile
           ;;
	*)       echo unsupported;;
   esac
else
  echo "Makefile not found"
fi







