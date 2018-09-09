## cxxwtl

[![Build Status](https://travis-ci.org/heavywatal/cxxwtl.svg?branch=master)](https://travis-ci.org/heavywatal/cxxwtl)

Personal C++ header library


### Dependency

- C++14 compiler
- CMake
- `filesystem.hpp` uses [boost::filesystem](http://www.boost.org/doc/libs/release/libs/filesystem/doc/)
- `zfstream.hpp` uses [boost::iostreams](http://www.boost.org/doc/libs/release/libs/iostreams/doc/)
- `itertools.hpp` uses [boost::coroutine2](http://www.boost.org/doc/libs/release/libs/coroutine2/doc/html/)
- `getopt.hpp` uses [boost::program_options](http://www.boost.org/doc/libs/release/libs/program_options/doc/)
- `grn.hpp` uses [boost::graph](http://www.boost.org/doc/libs/release/libs/graph/doc/)
- `eigen.hpp` uses [Eigen](https://eigen.tuxfamily.org/)
- `zlib.hpp` uses [zlib](https://github.com/madler/zlib)
- `clipp.hpp` uses
  [muellan/clipp](https://github.com/muellan/clipp) and
  [nlohmann/json](https://github.com/nlohmann/json)


### Installation

The easiest way is to use [Homebrew](https://brew.sh/)/[Linuxbrew](http://linuxbrew.sh/):
```sh
% brew install heavywatal/tap/cxxwtl
```

Alternatively, you can get the source code from GitHub, and install it with CMake:
```sh
% git clone https://github.com/heavywatal/cxxwtl.git
% cd cxxwtl/
% mkdir build
% cd build/
% cmake -DCMAKE_INSTALL_PREFIX=${HOME}/local ..
% make install
```
Header files are installed to `${CMAKE_INSTALL_PREFIX}/include/wtl/`.
This library can be used with `find_package()` from other CMake projects.


### Usage

```c++
// example.cpp
#include <iostream>
#include <wtl/math.hpp>

int main() {
    std::cout << wtl::factorial(5) << std::endl;
    return 0;
}
```

Additional include path can be specified with `-I` option or `CPATH` environment variable.

```sh
% clang++ -std=c++14 -O2 -Wall -Wextra -Wpedantic -I${HOME}/local/include example.cpp
% ./a.out
120
```
