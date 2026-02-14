#include <wtl/eigen.hpp>
#include <wtl/exception.hpp>

void constructors() {
  std::vector<int> v{0, 1, 2};
  auto arr = wtl::eigen::ArrayX(v);
  auto vec = wtl::eigen::VectorX(v);
  auto row_vec = wtl::eigen::RowVectorX(v);
  WTL_ASSERT(arr.size() == 3);
  WTL_ASSERT(vec.size() == 3);
  WTL_ASSERT(row_vec.size() == 3);
}

int main() {
  constructors();
  return 0;
}
