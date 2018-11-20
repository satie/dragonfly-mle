# ------------------------------------------------------------------
# dragonfly build
# ------------------------------------------------------------------
CC = $(BSD_CC)
CFLAGS = $(BSD_CFLAGS)
GIT_VERSION != git describe --abbrev=4 --always --tags

# ------------------------------------------------------------------
# 
# ------------------------------------------------------------------
LINUX_CC = gcc
LINUX_CFLAGS = -O2 -Wall -march=native -Werror
BSD_CC = cc
BSD_CFLAGS = -O2 -Wall -march=native -Werror -Wno-error=tautological-pointer-compare

INCLUDES = -I. -I/usr/local/include -I./io -I./test -I./lib/msgqueue -I./resp \
	   -I./lib/lua-cjson -I./lib/lua-hiredis -I./lib/lua-cmsgpack

LIBS = -lpthread -lluajit-5.1 -lhiredis -lm -lcurl -lrt -lz -lmicrohttpd
LFLAGS = -L. -L/usr/local/lib


DFLAGS = -DMLE_VERSION=\"$(GIT_VERSION)\"

RM=/bin/rm
DRAGONFLY_ROOT=/usr/local/dragonfly-mle

# -----------------------------------------------------------------
# core files
# -----------------------------------------------------------------
MAIN_SRC = main.c
UNIT_SRC = unit-test.c

# -----------------------------------------------------------------
# library ore files
# -----------------------------------------------------------------
SRCS0 = dragonfly-lib.c dragonfly-io.c dragonfly-cmds.c config.c \
	webservice.c responder.c

# ------------------------------------------------------------------
# I/O modules
# ------------------------------------------------------------------
SRCS1 = io/io-file.c io/io-pipe.c io/io-tail.c io/io-kafka.c \
        io/io-syslog.c io/io-zfile.c io/io-ipfix.c

# ------------------------------------------------------------------
# Responder modules
# ------------------------------------------------------------------
SRCS2 = resp/resp-suricata.c

# ------------------------------------------------------------------
# third party open-source libraries
# ------------------------------------------------------------------
SRCS3 = lib/lua-hiredis/lua-hiredis.c lib/lua-cjson/lua-cjson.c \
        lib/lua-cjson/strbuf.c lib/lua-cjson/fpconv.c \
        lib/msgqueue/msgqueue.c lib/lua-cmsgpack/lua_cmsgpack.c

# ------------------------------------------------------------------
# unit test files
# ------------------------------------------------------------------
SRCS4 = test/test.c test/test0.c test/test1.c test/test2.c \
		test/test3.c test/test4.c test/test5.c test/test6.c \
		test/test7.c test/test8.c test/test9.c test/test10.c

MAIN_OBJS = $(MAIN_SRC:.c=.o) $(SRCS0:.c=.o) $(SRCS1:.c=.o) $(SRCS2:.c=.o) $(SRCS3:.c=.o) 
TEST_OBJS = $(UNIT_SRC:.c=.o) $(SRCS0:.c=.o) $(SRCS1:.c=.o) $(SRCS2:.c=.o) $(SRCS3:.c=.o) $(SRCS4:.c=.o) 

MAIN = dragonfly-mle
TEST_BIN = dragonfly-test

all: $(MAIN)
test: $(TEST_BIN)

# ------------------------------------------------------------------
# production build
# ------------------------------------------------------------------

$(MAIN): $(MAIN_OBJS)
	echo $(MLE_VERSION)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(MAIN_OBJS) $(DFLAGS) $(LFLAGS) $(LIBS) 

# ------------------------------------------------------------------
# test build
# ------------------------------------------------------------------
$(TEST_BIN): $(TEST_OBJS) 
	echo $(MLE_VERSION)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TEST_BIN) $(TEST_OBJS) $(DFLAGS) $(LFLAGS) $(LIBS)
#	valgrind --leak-check=yes ./$(TEST_BIN) -r /tmp

# ------------------------------------------------------------------
# install
# ------------------------------------------------------------------

install:  
	@echo installing files
	install -d $(DRAGONFLY_ROOT)/bin/ 
	install -d $(DRAGONFLY_ROOT)/config/
	install -d $(DRAGONFLY_ROOT)/filter/
	install -d $(DRAGONFLY_ROOT)/analyzer/
	install ./dragonfly-mle $(DRAGONFLY_ROOT)/bin/
	install ./analyzer/* $(DRAGONFLY_ROOT)/analyzer/
	install ./filter/* $(DRAGONFLY_ROOT)/filter/
	install ./config/* $(DRAGONFLY_ROOT)/config/

# ------------------------------------------------------------------
# reconfigure OS target, i.e. gmake for Linux and make for FreeBSD
# ------------------------------------------------------------------
target:
	@./make-target.sh

# ------------------------------------------------------------------
# misc
# ------------------------------------------------------------------
.c.o:
	$(CC) $(CFLAGS) $(DFLAGS) $(INCLUDES) -c $<  -o $@

.PHONY: test clean

clean:
	$(RM) -f $(MAIN_OBJS) $(TEST_OBJS) $(MAIN) $(DEBUG_BIN) $(TEST_BIN) 
