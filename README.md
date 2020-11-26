# APX for C++

This is the new [APX](https://cogu.github.io/apx/) for C++ implementation.

## Implementation Status

This is an alpha-stage project and currently has limited functionality.
Run-time tests and feature improvements are currently ongoing.

## Building with CMake

CMake build has been tested for Windows (with Visual Studio) as well as Linux (with GCC).
Unit tests should also build and run in Cygwin but it's not recomended to use as production environment (due to issues with Cygwin socket support).

At least C++17 is required by your compiler, C++20 is recommended (this is the current pre-set in CMake files).

### Running unit tests

**Configure:**

```bash
cmake -S . -B build -DUNIT_TEST=ON
```

**Build:**

```bash
cmake --build build
```

**Run test cases:**

```bash
cd build
ctest
```
