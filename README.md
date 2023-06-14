# ChucK [![macOS - Build and Unit Tests](https://github.com/ccrma/chuck/actions/workflows/macos-build-unit-tests.yml/badge.svg)](https://github.com/ccrma/chuck/actions/workflows/macos-build-unit-tests.yml) [![Linux - Build and Unit Tests](https://github.com/ccrma/chuck/actions/workflows/linux-build-unit-tests.yml/badge.svg)](https://github.com/ccrma/chuck/actions/workflows/linux-build-unit-tests.yml)

Please note that the `master` branch has been renamed to `main.`

> The Internet Engineering Task Force (IETF) points out that "Master-slave is an oppressive metaphor that will and should never become fully detached from history" as well as "In addition to being inappropriate and arcane, the master-slave metaphor is both technically and historically inaccurate." 
[https://www.hanselman.com/blog/EasilyRenameYourGitDefaultBranchFromMasterToMain.aspx](https://www.hanselman.com/blog/EasilyRenameYourGitDefaultBranchFromMasterToMain.aspx)


## Strongly-timed, Concurrent, and On-the-fly Music Programming Language  

Chuck is a programming language for real-time sound synthesis and music creation. It is open-source and freely available on macOS, Windows, and Linux. Chuck presents a unique time-based, concurrent programming model that's precise and expressive (we call this strongly-timed), with dynamic control rates and the ability to add and modify code on-the-fly. In addition, ChucK supports MIDI, OpenSoundControl, HID device, and multi-channel audio. It's fun and easy to learn, and offers composers, researchers, and performers a powerful programming tool for building and experimenting with complex audio synthesis/analysis programs, and real-time interactive music.

Welcome to ChucK! 

## Downloading ChucK

To download and install ChucK, visit https://chuck.stanford.edu/release/

For more information, including documentation, examples, research publications, and community resources, please check out the ChucK website:
https://chuck.stanford.edu/

## Building ChucK
### macOS
To build the latest chuck from source, clone the `chuck` repo from github, navigate to the `chuck/src` directory, and run `make`.

```
git clone https://github.com/ccrma/chuck.git
cd chuck/src
make mac
```

### Linux
To build the latest chuck from source, clone the `chuck` repo from github, navigate to the `chuck/src` directory, and run `make`.

```
git clone https://github.com/ccrma/chuck.git
cd chuck/src
make linux-alsa linux-pulse linux-jack
```

### Windows
To build the latest chuck on Windows using Visual Studio (2019 or later recommended), clone the `chuck` repo from github, navigate to `chuck\src\visual-studio`, open `chuck.sln`, and build.
