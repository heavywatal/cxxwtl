## cxxwtl

[![Build status](https://github.com/heavywatal/cxxwtl/workflows/build/badge.svg)](https://github.com/heavywatal/cxxwtl/actions)

Personal C++ header library


### Dependency

- C++17 compiler
- CMake
- `itertools.hpp` uses [boost::coroutine2](http://www.boost.org/doc/libs/release/libs/coroutine2/doc/html/)
- `grn.hpp` uses [boost::graph](http://www.boost.org/doc/libs/release/libs/graph/doc/)
- `eigen.hpp` uses [Eigen](https://eigen.tuxfamily.org/)
- `zlib.hpp` uses [zlib](https://github.com/madler/zlib)


### Installation

The easiest way is to use [Homebrew](https://brew.sh/):
```sh
brew install heavywatal/tap/cxxwtl
```

Alternatively, you can get the source code from GitHub, and install it with CMake:
```sh
git clone https://github.com/heavywatal/cxxwtl.git
cd cxxwtl/
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=${HOME}/local
cmake --build build
cmake --install build
```


### Usage

Header files are installed to `${CMAKE_INSTALL_PREFIX}/include/wtl/`.
This library can be imported from other CMake projects:
```cmake
find_package(wtl COMPONENTS zlib)
target_link_libraries(${YOUR_TARGET} PRIVATE wtl::wtl wtl::zlib)
```

```c++
// example.cpp
#include <iostream>
#include <wtl/math.hpp>

int main() {
    std::cout << wtl::factorial(5) << std::endl;
    return 0;
}
```
