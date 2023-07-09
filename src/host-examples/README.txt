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


========
Building
========
The `makefile` in this directory shows how to build the hosts as well as 
ChucK core from source. Depending on the host software to integrate ChucK 
into (e.g., command-line chuck, miniAudicle, web chuck. Chunity, Chunreal, 
or what have you), the build could be drastically different; the makefile 
is supplied just as a reference for these examples.

type `make` to get started.
