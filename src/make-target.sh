#!/bin/sh


if [ -f Makefile ]
then
   if [ -z "$1" ]
   then
      OSTYPE=$(uname -s)
   else
      OSTYPE=${1}
   fi
   case $OSTYPE in
        Linux)  echo Target is: ${OSTYPE}; sed -i  's/^CC .*$/CC = $(LINUX_CC)/' Makefile; \
                                           sed -i  's/^CFLAGS.*$/CFLAGS = $(LINUX_CFLAGS)/' Makefile;;
	FreeBSD) echo Target is: ${OSTYPE}; sed -i "" 's/CC = $(LINUX_CC)/CC = $(BSD_CC)/g' Makefile; \
                                           sed -i "" 's/CFLAGS = $(LINUX_CFLAGS)/CFLAGS = $(BSD_CFLAGS)/g' Makefile;;
	*)       echo $(OSTYPE) is unsupported; exit;
   esac
else
  echo "Makefile not found"
fi




