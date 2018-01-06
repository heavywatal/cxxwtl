## cxxwtl

Personal C++ header library


### Dependency

- C++14 compiler
- `bmath.hpp` uses [boost::math](http://www.boost.org/doc/libs/release/libs/math/doc/)
- `filesystem.hpp` uses [boost::filesystems](http://www.boost.org/doc/libs/release/libs/filesystem/doc/)
- `zfstream.hpp` uses [boost::iostreams](http://www.boost.org/doc/libs/release/libs/iostreams/doc/)
- `itertools.hpp` uses [boost::coroutine2](http://www.boost.org/doc/libs/release/libs/coroutine2/doc/html/)
- `getopt.hpp` uses [boost::program_options](http://www.boost.org/doc/libs/release/libs/program_options/doc/)
- `grn.hpp` uses [boost::graph](http://www.boost.org/doc/libs/release/libs/graph/doc/)
- `eigen.hpp` uses [Eigen](https://eigen.tuxfamily.org/)


### Installation

The easiest way is to use [Homebrew](https://brew.sh/)/[Linuxbrew](http://linuxbrew.sh/):
```sh
% brew tap heavywatal/tap
% brew install cxxwtl --HEAD
```

Alternatively, you can get the source code from GitHub manually:
```sh
% git clone https://github.com/heavywatal/cxxwtl.git
% cd cxxwtl/
% mkdir build && cd build/
% YOUR_PREFIX=${HOME}/local  # or /usr/local
% cmake -DCMAKE_INSTALL_PREFIX=$YOUR_PREFIX ..
% make install
```

You can also just copy the header files, or clone the repository directly into `${YOUR_PREFIX}/include/wtl`.

### Usage

```c++
// example.cpp
#include <iostream>
#include <wtl/math.hpp>

int main() {
    std::cout << wtl::factorial(5) << std::endl;
}
```

Additional include path can be specified with `-I` option or `CPATH` environment variable.

```sh
% clang++ -O2 -Wall -Wextra -std=c++14 -I${HOME}/local/include example.cpp
% ./a.out
120
```
