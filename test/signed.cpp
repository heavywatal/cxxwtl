#include <wtl/signed.hpp>
#include <wtl/exception.hpp>

#include <vector>
#include <deque>

template <class Container> inline
void test() {
    Container cont(6u);
    WTL_ASSERT(wtl::at(cont, 5) == 0);
    WTL_ASSERT(cont.size() == 6u);
    WTL_ASSERT(wtl::ssize(cont) == 6);
    wtl::resize(cont, 16);
    WTL_ASSERT(wtl::ssize(cont) == 16);
    if constexpr (std::is_same_v<Container, std::vector<int>>) {
        wtl::reserve(cont, 32);
        WTL_ASSERT(cont.capacity() >= 32u);
    }
}

int main() {
    test<std::vector<int>>();
    test<std::deque<int>>();
    return 0;
}
