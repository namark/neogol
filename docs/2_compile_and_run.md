# Compilation and start

## Common for all GNU systems
Recommended - turn on optimizations for your compiler. You can do that using the CXXFLAGS variable, for example:<br />
```bash
export CXXFLAGS="$CXXFLAGS -O3 -DNDEBUG -fno-rtti"
```

1. For the sake of completeness - obtain the project and change directory to its root:
```bash
git clone https://notabug.org/namark/neogol
cd neogol
```

2. If you had to manually install the c++ compiler it's likely not the default one, so will need to manually specify it for `make`:
```bash
export CXX=g++-7
```
Otherwise, skip this step.

3. Fetch and install dependencies:
```bash
./tools/setup/init.sh
```

4. Compile the project itself:
```bash
make
```
To enable video recording functionality specify ENABLE_THEORA_RECORDER
```bash
make ENABLE_THEORA_RECORDER=true
```

5. After compilation the executable file will be created in the `out` folder. You can run it like so:
```bash
./out/neogol
```
To enable video recording functionality specify the video file name/path
```bash
./out/neogol recording.ogv
```
It can be used as is, or installed system wide using `make install` (or `checkinstall` on Debian based systems). `make unisntall` will uninstall it (use package manager in case of `checkinstall`).

That should be about it.

## Cygwin MinGW specifics
You'll need to copy or clone the project into the home directory in cygwin. <br />
Cygwin root is the folder you specified during installation. <br />
All the commands below will need to be carried out in cygwin terminal that should have a shortcut on the desktop. <br />
The terminal will start in your home folder by default, so you can just clone the project right there (step 1 above).<br />

For some (probably very good) reason, MinGW would not be installed as default toolchain, so the compiler and the archiver will need to be specified manually before step 3:
```bash
export CXX=x86_64-w64-mingw32-g++ # compiler variable
export AR=x86_64-w64-mingw32-ar # archiver variable
```
Now, SDL does some shenanigans with the main function, that didn't work for me, so had do disable that for step 4:
```bash
export CXXFLAGS="$CXXFLAGS -DSDL_MAIN_HANDLED"
```
You can try to skip this and see if it works for you, or try some other solutions to the problem.<br />
Before proceeding with step 5 there is the timeless problem of dlls to solve. Fortunately it seems pretty easy with this setup, need to just copy over all the dlls from mingw bin:
```bash
cp /usr/x86_64-w64-mingw32/sys-root/bin/*.dll ./out/
```
That's it! Here is an overview:
```bash
git clone https://notabug.org/namark/neogol
cd neogol
export CXX=x86_64-w64-mingw32-g++
export AR=x86_64-w64-mingw32-ar
export CXXFLAGS="$CXXFLAGS -O3 -DNDEBUG -fno-rtti"
./tools/setup/init.sh
export CXXFLAGS="$CXXFLAGS -DSDL_MAIN_HANDLED"
make ENABLE_THEORA_RECORDER=true
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/*.dll ./out/
./out/neogol recording.ogv
```

<br />
<br />
<br />

[More on some of these makeshift tools](3_more_on_tools.md)
