Multi-threaded game of life with 8 generation history (current + 7 generations back).

## Use instructions

### Overview
TODO

### Controls
Mouse : draw living cells.<br />
Enter/Return : toggle life at ~60 generation per second.<br />
Right Arrow : step forward one generation.<br />
Left Arrow : step backward one generation.<br />
\+ : zoom in<br />
\- : zoom out<br />
~ : fill the screen at random<br />
Backspace : clear the screen<br />
Shift+R : start/stop recording, when recording is stopped, it'll take some time to render it to a file, frames being played back at the rate of video encoding<br />
Escape : stop the rendering process (video until that point still saved)<br />


### Special secret dangerous command line parameters
Positional:
1. filename for recording, required to enable the recording functionality

## Build instructions
Below are build instructions for GNU systems (including cygwin on Windows), primarily using `make`. If that does not suit your needs it should not be hard to set this project up with your favorite build system/IDE. See the dependencies in the next section. <br />
[Basic setup](docs/1_basic_setup.md) <br />
[Compile and run](docs/2_compile_and_run.md) <br />
[More on some of these makeshift tools](docs/3_more_on_tools.md)

## Dependencies
[libsdl2](https://libsdl.org) <br />
[libsimple_graphical](https://notabug.org/namark/libsimple_graphical) <br />
[libsimple_interactive](https://notabug.org/namark/libsimple_interactive) <br />
[libsimple_musical](https://notabug.org/namark/libsimple_musical) <br />
[libsimple_geom](https://notabug.org/namark/libsimple_geom) <br />
[libsimple_support](https://notabug.org/namark/libsimple_support) <br />
[libsimple_file](https://notabug.org/namark/libsimple_file) <br />
[libsimple_sdlcore](https://notabug.org/namark/libsimple_sdlcore) <br />
[cpp_tools](https://notabug.org/namark/cpp_tools) <br />
Optional: <br />
[libtheora](https://theora.org) <br />
[libogg](https://xiph.org/ogg/) <br />

The SDL2 headers are expected to be in a directory named SDL2. libsimple are all static libraries. cpp_tools is only necessary for building with GNU make as outlined in the previous section.<br />
There are also source archives in releases, which have all the simple libraries included, so using that you won't have to individually setup those.

## Licensing
COPYRIGHT and LICENSE apply to all the files in this repository unless otherwise noted in the files themselves.
