# mqueue-mac
Implementation of POSIX mqueue using System V messages.

23-January-2022  
James P. Parziale

## Description

I got frustrated not being able to test code that uses mqueue.h on the Mac. So I attempted to kludge an mqueue that will work on the Mac to satisfy simple programs that use mqueue's.

I make no guarantee that it will work exactly like mqueue, if it will work for you at all. But it works well enough for me to debug some Linux programs on my Mac.

## Build

To build, just use `make` in the main directory.
This will create mqueue.o, and then go into the test directory
and create mq_client and mq_server.

## Run a test

You can run the test programs using `make run`

Both mq_client and mq_server open a receive queue for themselves.
mq_client opens a send queue to the server,
then sends its queue name to the server.
mq_server receives the client's queue name and uses it to
open a send queue back to the client.

The client prompts for text to be entered from stdin.
It sends the text to the server, which prints it and sends
it back to the client.

If the user enters ^D to the client's prompt, the client
sends a special string to the server which causes it to
exit its loop and both the client and server terminate.

### @TODO
It would also be possible to add signal handlers to catch
^C and gracefully terminate the programs as well.