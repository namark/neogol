# Basic system setup

Required software:
1. git, to fetch some dependencies
2. GNU stuff (make, coreutils, findutils, binutils)
3. C++ 17 compliant compiler
4. SDL 2.0 runtime and development libraries
5. Optionally theora and ogg libraries for video recording functionality.

## Specific instruction for Debian based distributions
(Something similar should work on your favorite system/package manager as well. Know your distro!)

1. Install git, GNU stuff, compiler collection, SDL2
```bash
sudo apt install git coreutils findutils build-essential libsdl2-2.0 libsdl2-dev
```

2. Optionally (not necessary on most modern systems) install a sufficiently recent C++ compiler (instructions specific to Ubuntu based distros - 14.04/16.04)
```bash
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install g++-7 -y
```

3. Optionally (for video recording) install theora and ogg libraries
```bash
sudo apt install libtheora-dev libogg-dev
```

## Specific instruction for Windows

Download [cygwin](https://www.cygwin.com).<br />
In the installer go "Next" until presented with package selection screen. There change view to full, using the search find packages `git, make, mingw64-x86_64-gcc-g++, mingw64-x86_64-SDL2` (or same with i686 if you want to build for 32bit systems) and mark them for installation by double clicking the entry in the New column, which will change from "Skip" to a specific version number. Proceed with the installation.

## Specific instruction for OSX

Throw all your money at it and pray to apple? ... erm, I mean all the same tools should be available, brew them or something, I don't know.
<br />
<br />
<br />

[Compile and run](2_compile_and_run.md)
