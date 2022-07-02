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

---

---

# I'm Using GitHub Under Protest

This project is currently hosted on GitHub.  This is not ideal; GitHub is a proprietary,  
trade-secret system that is not Free and Open Souce Software (FOSS).  
I am deeply concerned about using a proprietary system like GitHub to develop my FOSS project.  
I urge you to read about the
[Give up GitHub](https://GiveUpGitHub.org) campaign from
[the Software Freedom Conservancy](https://sfconservancy.org)  
to understand some of the reasons why GitHub is not a good place to host FOSS projects.

Any use of this project's code by GitHub Copilot, past or present, is done without my permission.  
I do not consent to GitHub's use of this project's code in Copilot.

I plan to move this project to another hosting site (TBD) and will leave a link to it here in this README file.

---

![Logo of the GiveUpGitHub campaign](https://sfconservancy.org/img/GiveUpGitHub.png)
