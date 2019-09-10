# CHIP8

This repository contains a toy implementation of an emulator
for the CHIP8 platform. It aims to be cross-platform for Windows, macOS, and Linux.
Graphics are implemented using the excellent [raylib](https://github.com/raysan5/raylib) library.

This project is still in early development and probably rather buggy.
It was mostly created for me to learn and experiment with writing emulators.

## Building

Dependencies

* raylib 2.5.0

On macOS, this can be installed from Homebrew.

`brew install raylib`

On Windows, you can get raylib from their [GitHub page](https://github.com/raysan5/raylib)

The MinGW compiler is currently the only compiler supported by this project.

Linux has not been tested yet, but probably similar to macOS.

The project uses the CMake build system. I personally use CLion for development on the project.

The project can be built with the following commands (macOS):

```shell script
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## References

This project has used these two articles/pages heavily for references and inspiration

* http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
* https://austinmorlan.com/posts/chip8_emulator/

## LICENSE

This project is licensed under the MIT license. The license can be found [here](LICENSE.md)