# APX for C++

This is the new [APX](https://cogu.github.io/apx/) for C++ implementation.

## Implementation Status

This is an alpha-stage project and currently has limited functionality.
Run-time tests and feature improvements are currently ongoing.

## Building with CMake

CMake build has been tested for Windows (with Visual Studio) as well as Linux (with GCC).
Unit tests should also build and run in Cygwin but it's not recomended to use as production environment (due to issues with Cygwin socket support).

Your compiler needs to support at least C++17 in order to build.

### Tested build environments

| Supported | Architcture | OS      | Compiler           |
|-----------|-------------|---------|--------------------|
| No        | Any         | Linux   | GCC < 9.x          |
| Yes       | Intel x64   | Linux   | GCC 9.3            |
| Yes       | Intel x64   | Windows | Visual Studio 2019 |

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
