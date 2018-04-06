#include "scope.hpp"

#include <iostream>

void rvalue() {
    auto atexit = wtl::scope_exit([]{std::cout << "rvalue exit\n";});
    std::cout << "rvalue body\n";
}

void lvalue() {
    auto func = []{std::cout << "lvalue exit\n";};
    auto atexit = wtl::scope_exit(std::move(func));
    std::cout << "lvalue body\n";
}

int main() {
    lvalue();
    rvalue();
    return 0;
}
