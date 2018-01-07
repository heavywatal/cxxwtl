#include "iostr.hpp"
#include "demangle.hpp"

int main(int argc, char* argv[]) {
    std::cout << "__cplusplus: " << __cplusplus << std::endl;
    try {
        std::cerr << wtl::str_join(argv, argv + argc, " ") << std::endl;
        std::cerr << "EXIT_SUCCESS" << std::endl;
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e) {
        std::cerr << "\n" << wtl::typestr(e) << ": " << e.what() << std::endl;
    }
    return EXIT_FAILURE;
}
