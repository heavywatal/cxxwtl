#include <wtl/grn.hpp>
#include <wtl/exception.hpp>

void product() {
  auto adj_mtrx = std::vector<std::vector<double>>{{0.0, 1.0}, {0.0, 0.0}};
  auto g = wtl::make_graph(adj_mtrx);
  WTL_ASSERT(boost::num_vertices(g) == 2);
  WTL_ASSERT(boost::num_edges(g) == 1);
}

int main() {
  product();
  return 0;
}
