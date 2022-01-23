# mqueue-mac
Implementation of POSIX mqueue using System V messages.

23-January-2022
James P. Parziale

## Description

I got frustrated not being able to test code that uses mqueue.h on the Mac. So I attempted to kludge an mqueue that will work on the Mac to satisfy simple programs that use mqueue's.

I make no guarantee that it will work exactly like mqueue, if it will work for you at all. But it works well enough for me to debug some Linux programs on my Mac.

