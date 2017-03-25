## cxxwtl

Personal C++ header library


### Dependency

- C++14 compiler
- `bmath.hpp` uses [boost::math](http://www.boost.org/doc/libs/release/libs/math/doc/)
- `itertools.hpp` uses [boost::coroutine2](http://www.boost.org/doc/libs/release/libs/coroutine2/doc/html/)
- `getopt.hpp` uses [boost::program_options](http://www.boost.org/doc/libs/release/libs/program_options/doc/)
- `grn.hpp` uses [boost::graph](http://www.boost.org/doc/libs/release/libs/graph/doc/)
- `eigen.hpp` uses [Eigen](https://eigen.tuxfamily.org/)
- `prandom.hpp` uses [sfmt-class](https://github.com/heavywatal/sfmt-class)
- `zfstream.hpp` uses [boost::iostreams](http://www.boost.org/doc/libs/release/libs/iostreams/doc/)


### Installation and Usage

You can put the header files anywhere you want.
For examle, I usualy clone them into `~/local/include/wtl`.
It is consistent with the namespace `wtl` in which most of functions and classes are defined.

```sh
% cd ~/local/include/
% git clone https://github.com/heavywatal/cxxwtl.git wtl
```

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
