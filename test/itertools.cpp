#include <wtl/itertools.hpp>
#include <wtl/exception.hpp>

void product() {
  std::vector<int> x{0, 1, 2};
  std::vector<int> y{3, 4};
  auto p = wtl::itertools::product(std::vector<std::vector<int>>{x, y});
  WTL_ASSERT(p.count() == 0);
  WTL_ASSERT(p.max_count() == 6);
}

int main() {
  product();
  return 0;
}
