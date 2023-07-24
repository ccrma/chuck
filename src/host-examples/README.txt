---------------------------
host-examples (sample code)
---------------------------
author: Ge Wang
date: Summer 2023
---------------------------

So, you want to incorporate ChucK as a component/library within another 
software? These examples can show you how.

========
Examples
========
host-1-minimal.cpp -- (no real-time audio) shows the basics of 
initializing a ChucK instance, and using it to compile and run some ChucK 
code.

host-2-audio.cpp -- shows integrating the ChucK instance in a host with 
real-time audio, and shows using the ChucK instance to compile and run 
ChucK program at runtime to generate audio. The example plays audio (on 
your default output audio device)

host-3-globals.cpp -- shows communication between C++ and ChucK using
ChucK global variables

host-4-shreds.cpp -- shows high-level shred operations from c++, including
adding, removing last, and getting the current VM status including info
about shreds

========
Building
========
The `makefile` in this directory shows how to build the hosts as well as 
ChucK core from source. Depending on the host software to integrate ChucK 
into (e.g., command-line chuck, miniAudicle, web chuck. Chunity, Chunreal, 
or what have you), the build could be drastically different; the makefile 
is supplied just as a reference for these examples.

type `make` to get started.

NOTE much of the complexities of the makefiles here are due to the need
to support real-time on various operating systems. If you are incorporating
ChucK core as a component into a system that already handles real-time
audio, the build for ChucK can be much simpler. For reference see the
'vanilla' makefile, located in ../src/core/makefile

Good luck and keep on ChucKin'

