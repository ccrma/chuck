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

This build process should build a `chuck` executable in `chuck/src`.

### Windows
To build chuck using Visual Studio (2019 or later recommended), navigate to `chuck\src\visual-studio`, open `chuck.sln`, and build.

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
