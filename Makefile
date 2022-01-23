#-------------------------------------------------------------------------------
# Makefile for mqueue implementation for computers without mqueue.h
# but have System V messages.
#
# J. Parziale
# 23-Jan-2022
#-------------------------------------------------------------------------------

TARGETS = mqueue.o
INCLUDES = -I /usr/include
CFLAGS = -O2 -g -Wall  -fmessage-length=0 $(INCLUDES) $(LIBDIRS) $(LIBS)

#-------------------------------------------------------------------------------

all: $(TARGETS)
	$(MAKE) -C test

mqueue.o: mqueue.c mqueue.h
	$(CC) $(CFLAGS) -c $< -o $@

run:
	$(MAKE) -C test run

clean:
	@rm -vf $(TARGETS)
	$(MAKE) -C test clean

#-------------------------------------------------------------------------------
