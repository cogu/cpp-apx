# APX for C++

This is the new [APX](https://cogu.github.io/apx/) for C++ implementation.

## Implementation Status

This repo is in early stages of development and mostly contains a set of classes with unit tests.
The Visual Studio 2019 solution is used for development while the CMake build is used for production.

## Building with CMake

CMake build has been tested for Windows (with Visual Studio) as well as Linux (with GCC).
Unit tests should also build and run in Cygwin but it's not recomended to use as production environment (due to issues with Cygwin socket support).

At least C++17 is required by your compiler, C++20 is recommended (this is the current pre-set in CMake files).

### Running unit tests (Linux with GCC)

**Configure:**

```sh
cmake -S . -B build -DUNIT_TEST=ON
```

**Build:**

```sh
cmake --build build
```

**Run test cases:**

```cmd
cd build && ctest
```

### Running unit tests (Windows with Visual Studio 2019)

Launch "x64 Native Tools Command Prompt for Visual Studio 2019" in start menu.

**Configure:**

```cmd
cmake -S . -B build -DUNIT_TEST=ON
```

**Build:**

```cmd
cmake --build build
```

**Run test cases:**

```cmd
cd build
ctest
```
