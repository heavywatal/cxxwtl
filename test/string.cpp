#include <wtl/string.hpp>
#include <wtl/exception.hpp>

#include <iostream>

void test_sto() {
    WTL_ASSERT(!wtl::sto<bool>("0"));
    WTL_ASSERT(wtl::sto<bool>("1"));
    WTL_ASSERT(wtl::sto<int>("42") == 42);
    WTL_ASSERT(wtl::sto<long>("1729") == 1729l);
    WTL_ASSERT(wtl::sto<long long>("24601") == 24601ll);
    WTL_ASSERT(wtl::sto<unsigned>("42") == 42u);
    WTL_ASSERT(wtl::sto<unsigned long>("1729") == 1729ul);
    WTL_ASSERT(wtl::sto<unsigned long long>("24601") == 24601ull);
    WTL_ASSERT(wtl::sto<double>("3.14") - 3.14 < 1e-9);
}

int main() {
    std::cout << "__cplusplus: " << __cplusplus << std::endl;
    test_sto();
    WTL_ASSERT((wtl::split("a b\tc") == std::vector<std::string>{"a", "b", "c"}));
    WTL_ASSERT((wtl::split<int>("1 2\t3") == std::vector<int>{1, 2, 3}));
    WTL_ASSERT(wtl::strip("  str  ") == "str");
    WTL_ASSERT(wtl::startswith("abracadabra", "abr"));
    WTL_ASSERT(!wtl::startswith("abracadabra", "bra"));
    WTL_ASSERT(wtl::endswith("abracadabra", "bra"));
    WTL_ASSERT(!wtl::endswith("abracadabra", "abr"));
    WTL_ASSERT(wtl::replace("abracadabra", "br", "_") == "a_acada_a");
    return 0;
}
