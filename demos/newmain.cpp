#include "nothread/context.hpp"

#include <iostream>
#include <stddef.h> // todo

using namespace std;
using namespace nothread;

execution_context say_hello(execution_context root, int id) {
    std::cout << "Hello " << id << "\n";
    root = root.resume();
    std::cout << "Hello " << id + 1 << "\n";
    root = root.resume();
    std::cout << "Hello " << id + 2 << "\n";
    return root;
}

int main() {
    execution_context ctx = call(say_hello, 2);
    std::cout << "returned\n";
    ctx = ctx.resume();
    std::cout << "returned-2\n";
    ctx = ctx.resume();
    return 0;
}
