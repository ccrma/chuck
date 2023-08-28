# ChucK [![macOS - Build and Unit Tests](https://github.com/ccrma/chuck/actions/workflows/macos-build-unit-tests.yml/badge.svg)](https://github.com/ccrma/chuck/actions/workflows/macos-build-unit-tests.yml) [![Linux - Build and Unit Tests](https://github.com/ccrma/chuck/actions/workflows/linux-build-unit-tests.yml/badge.svg)](https://github.com/ccrma/chuck/actions/workflows/linux-build-unit-tests.yml)

## Strongly-timed Music Programming Language  
**[ChucK](https://chuck.stanford.edu/)** is a programming language for real-time sound synthesis and music creation. It is open-source and freely available on macOS, Windows, and Linux. ChucK presents a unique time-based, concurrent programming model that is precise and expressive (we call this _strongly-timed_), with dynamic control rates and the ability to add and modify code on-the-fly. In addition, ChucK supports MIDI, OpenSoundControl, HID devices, and multi-channel audio. It's fun and easy to learn, and offers composers, researchers, and performers a powerful programming tool for building and experimenting with complex audio synthesis/analysis programs, and real-time interactive music.

Welcome to ChucK!

## Installing ChucK
To download and install ChucK, visit the **[official ChucK release page](https://chuck.stanford.edu/release/)**.

For more information, including documentation, examples, research publications, and community resources, vist the **[ChucK homepage](https://chuck.stanford.edu/)** or its **[Princeton mirror](https://chuck.cs.princeton.edu/)**.

### Homebrew
```
brew install chuck
```

## Building ChucK
To build the latest ChucK from source, clone the `chuck` repo from github:
```
git clone https://github.com/ccrma/chuck.git
```

### macOS
navigate to the `chuck/src` directory, and run `make`:
```
cd chuck/src
make mac
```
OR to build a universal binary (intel + apple sillicon):
```
make mac-ub
```

This should build a `chuck` executable in `chuck/src`.

### Linux
Dependencies: gcc, g++, make, bison, flex, libsndfile, ALSA (for linux-alsa builds), PulseAudio (for linux-pulse builds), JACK (for linux-jack builds)

To set up a build environment for **chuck** on Debian or Ubuntu:
```
sudo apt install build-essential bison flex libsndfile1-dev \
  libasound2-dev libpulse-dev libjack-jackd2-dev
```
For other Linux distributions, the setup should be similar although the package install tools and package names may be slightly different. (_NOTE: setups that do not need JACK or PulseAudio can omit either or both of these packages. ALSA is needed for MIDI support on Linux._)

To build **chuck** (with all suppported drivers: ALSA, PulseAudio, JACK), navigate to the `chuck/src` directory and run `make`:
```
cd chuck/src
make linux-all
```

FYI `make linux-all` is equivalent in outcome to combining individual drivers:
```
make linux-alsa linux-pulse linux-jack
```

To build **chuck** for a subset of ALSA, PulseAudio, or JACK, run `make` with the desired driver(s). For example, to build for ALSA and PulseAudio only:
```
make linux-alsa linux-pulse
```

Or, to build for ALSA only:
```
make linux-alsa
```

This process should build a `chuck` executable in `chuck/src`.

### Windows
To build chuck using Visual Studio (2019 or later recommended), navigate to `chuck\src\visual-studio`, open `chuck.sln`, and build.


## Contributing to ChucK
Here are the source repositories maintained by the ChucK Team. _Developers wanted!_

[**chuck**](https://github.com/ccrma/chuck) | core language, virtual machine, and synthesis engine

[**chugins**](https://github.com/ccrma/chugins) | extend chuck with plugins! a great place to start contributing

[**miniAudicle**](https://github.com/ccrma/miniAudicle) | an IDE for chuck

[**webchuck**](https://github.com/ccrma/webchuck) | chuck running in web browsers (works on desktops and phones!)

[**webchuck IDE**](https://github.com/ccrma/webchuck-ide) | a browser-based IDE for chuck

[**chunity**](https://github.com/ccrma/chunity) | ChucK in Unity

[**chunreal**](https://github.com/ccrma/chunreal) | ChucK in Unreal Engine (new!)


## Integrating ChucK as Component in Other C++ Hosts
It is possible to incorporate ChucK **core** (compiler, virtual machine, synthesis engine) as a component/library within other c++ software **hosts**. This can useful for adding ChucK functionalities to your software systems or to create new plugins. FYI the various tools listed in the previous section all incorporate ChucK in this core/host model: including command-line, miniAudicle, Unity, Unreal Engine, WebAssembly; additionally ChucK has been integrated with openFrameworks, iOS/Android apps, embedded systems, and in hybrid language systems, e.g., [FaucK](https://ccrma.stanford.edu/~rmichon/fauck/) (FAUST + ChucK), [ChucKDesigner](https://github.com/DBraun/ChucKDesigner) (ChucK in TouchDesigner), [chuck~](https://github.com/shakfu/pd-chuck/) (ChucK in Pure Data), [chuck~](https://github.com/shakfu/chuck-max) (ChucK in Max/MSP).

To show how this integration can be done, we have created a series of examples in C++ to show how to integrate ChucK into any C++ host. The simplest of these examples, [example-1-minimal.cpp](https://github.com/ccrma/chuck/blob/main/src/host-examples/example-1-minimal.cpp), creates a minimal ChucK host in C++ in about 20 lines of code. The subsequent examples show adding real-time audio, C++/ChucK communication using ChucK globals, and ChucK shred control from C++, respectively. These host examples can be found in the [src/host-examples/](https://github.com/ccrma/chuck/tree/main/src/host-examples) folder of this repository. For more advanced usage, checkout the various tools in the ChucK ecosystem as listed above in the previous section. To get started building these host examples:
```
cd src/host-examples
make
```
### Building ChucK Core in "Vanilla" Mode
It is also possible to build _only_ ChucK **core** (compiler, virtual machine, and synthesis engine), without a host and without any platform-specific real-time audio I/O. This can be a helpful starting point for integrating ChucK core into existing host systems that already have audio I/O. To build ChucK core in "vanilla" mode:
```
cd src/core
make vanilla
```

## ChucK History
ChucK was created in the early 2000s at Princeton University by [Ge Wang](https://ccrma.stanford.edu/~ge/) and [Perry R. Cook](https://www.cs.princeton.edu/~prc/), while Ge was a Ph.D. student advised by Perry in the Computer Science Department. The first version of ChucK was released under a GPL license in 2003. Many researchers, teachers, and artists have contributed to ChucK's evolution over the years. [Spencer Salazar](https://ccrma.stanford.edu/~spencer/) created [miniAudicle](https://github.com/ccrma/miniAudicle), a GUI-based integrated development environment for ChucK in 2004 (this IDE, in addition to the command line version of ChucK, remains largely how ChucK is distributed and used today). The [Princeton Laptop Orchestra](https://plork.princeton.edu/) (PLOrk), founded by [Dan Trueman](https://manyarrowsmusic.com/) and Perry Cook in 2005, began using ChucK for teaching as well as instrument and sound design. In 2006, [Rebecca Fiebrink](https://researchers.arts.ac.uk/1594-rebecca-fiebrink) and Ge Wang created ChucK's audio analysis framework, expressed through unit analyzers--the analysis counterpart to unit generators. Ge join the faculty at Stanford University's CCRMA in 2007, and ChucK research and development became distributed, with developers at Princeton, Stanford, and elsewhere. The [Stanford Laptop Orchestra](https://slork.stanford.edu/) (SLOrk) was founded in 2008 at CCRMA, where ChucK continued to be a tool for instrument design and teaching. In that same year, the mobile music startup Smule was co-founded, which used ChucK on the iPhone (codenamed "ChiP") as a real-time audio engine for its early apps: [Ocarina](https://artful.design/ocarina/), Sonic Lighter, Zephyr, and Leaf Trombone: World Stage. Meanwhile, ChucK continued to find its way into computer music curricula, including at Stanford, Princeton, CalArts. In 2015, the book [_Programming for Musicians and Digital Artists: Creating music with ChucK_](https://www.amazon.com/Programming-Musicians-Digital-Artists-Creating/dp/1617291706/) was published, authored by Ajay Kapur, Perry Cook, Spencer Salazar, and Ge Wang. Around the same time, Kadenze introduced the online course [Introduction to Real-Time Audio Programming in ChucK](https://www.kadenze.com/courses/introduction-to-programming-for-musicians-and-digital-artists/info). Romain Michon and Ge Wang integrated FAUST and ChucK to create [FaucK](https://ccrma.stanford.edu/~rmichon/fauck/). In 2017, Jack Atherton created [Chunity](https://chuck.stanford.edu/chunity/), which enables one to program ChucK inside the Unity game development framework. In 2018, Ge write about ChucK in [_Artful Design: Technology in Search of the Sublime_](https://artful.design/), a photocomic book about the importance of cultural awareness in the shaping of technology. ChucK now runs natively in web browsers ([WebChucK](https://chuck.stanford.edu/webchuck/)) and can be programmed directly in the [WebChucK IDE](https://chuck.stanford.edu/ide/).

ChucK has been extensively documented in published articles and books ([see list](https://ccrma.stanford.edu/~ge/publish/)). For an overview, check out:

“[ChucK: A Strongly-Timed Music Programming Language](https://artful.design/stuff/samples/chuck.pdf)“ comic book excerpt from [_Artful Design: Techonlogy in Search of the Sublime_](https://artful.design/)

Wang, G., P. R., Cook, and S. Salazar. 2015. "[ChucK: A Strongly Timed Computer Music Language](https://ccrma.stanford.edu/~ge/publish/files/2015-cmj-chuck.pdf)" _Computer Music Journal_ 39:4. doi:10.1162/COMJ_a_00324

## ChucK Media
Non-source code documents (papers, logos, soundbites, comics) have been migrated to the [chuck-media](https://github.com/ccrma/chuck-media) repository.

## ChucK Community
Join us! [ChucK Community Discord](https://discord.gg/ENr3nurrx8) | [ChucK-users Mailing list](https://lists.cs.princeton.edu/mailman/listinfo/chuck-users)

_Happy ChucKing!_
