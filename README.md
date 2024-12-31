# pong

## Description

Recreation of the Atari classic "Pong", with single-player and shared-screen modes.

## Controls

To move up and down:

- Player 1: Up and Down (also Q and A, respectively)

- Player 2: P and L

## Screenshots

![Demo](recordings/demo.gif)

## Build

### CMake

- Desktop:

    ```
    cmake -B build && cmake --build build
    ```

- Web (TODO):

    Compiling for the web requires the [Emscripten
    SDK](https://emscripten.org/docs/getting_started/downloads.html):

    ``` bash
    mkdir build
    cd build
    emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXECUTABLE_SUFFIX=".html"
    emmake make
    ```

## Developers

- Luis Torres

## License

This project sources are licensed under an unmodified zlib/libpng license,
which is an OSI-certified, BSD-like license that allows static linking with
closed source software. Check [LICENSE](LICENSE) for further details.

*Copyright (c) 2024  (luismtorresv)*
