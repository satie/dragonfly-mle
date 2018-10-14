#!/bin/sh

if [ -f Makefile ]
then
   OSTYPE=$(uname -s)
   case $OSTYPE in
	Linux)  sed -i 's/^CC =.*/CC = $(BSD_CC)/g' Makefile
          sed -i 's/^CFLAGS =.*/CFLAGS = $(BSD_CFLAGS)/g' Makefile
          sed -i 's/^GIT_VERSION.*/GIT_VERSION != git describe --abbrev=4 --always --tags/g' Makefile
           ;;
	FreeBSD) sed -i "" 's/^CC =.*/CC = $(BSD_CC)/g' Makefile
           sed -i "" 's/^CFLAGS =.*/CFLAGS = $(BSD_CFLAGS)/g' Makefile
           sed -i "" 's/^GIT_VERSION.*/GIT_VERSION != git describe --abbrev=4 --always --tags/g' Makefile
           ;;
	*)       echo unsupported;;
   esac
else
  echo "Makefile not found"
fi







