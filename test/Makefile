#-------------------------------------------------------------------------------
# Makefile for mqueue test programs
#
# J. Parziale
# 23-Jan-2022
#-------------------------------------------------------------------------------

TARGETS = mq_client mq_server
INCLUDES = -I /usr/include -I..
OBJS = ../mqueue.o
HEADERS = ../mqueue.h
CFLAGS = -O2 -g -Wall -fmessage-length=0 $(INCLUDES) $(LIBDIRS) $(LIBS)

all: $(OBJS) $(TARGETS)

mq_client: mq_client.c $(HEADERS) $(OBJS)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@

mq_server: mq_server.c $(HEADERS) $(OBJS)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@

run:
	./mq_server & ./mq_client

clean:
	@rm -vf $(TARGETS)
	@rm -rvf *.dSYM

#-------------------------------------------------------------------------------
