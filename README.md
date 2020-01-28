Multi-threaded game of life with 8 generation history (current + 7 generations back).

## Use instructions
### Overview
Mouse - draw living cells.
Enter/Return - toggle life at ~60 generation per second.
Right Arrow - step forward one generation.
Left Arrow - step backward one generation.
+ - zoom in
- - zoom out

### Special secret dangerous command line parameters
None so far

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
[libsimple_sdlcore](https://notabug.org/namark/libsimple_sdlcore) <br />
[cpp_tools](https://notabug.org/namark/cpp_tools) <br />

The SDL2 headers are expected to be in a directory named SDL2. libsimple are all static libraries. cpp_tools is only necessary for building with GNU make as outlined in the previous section.<br />
There are also source archives in releases, which have all the simple libraries included, so using that you won't have to individually setup those.

## Licensing
COPYRIGHT and LICENSE apply to all the files in this repository unless otherwise noted in the files themselves.
