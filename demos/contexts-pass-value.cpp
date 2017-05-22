#include "nothread/context.hpp"

#include <iostream>
#include <stddef.h> // todo

using namespace std;
using namespace nothread;

execution_context say_hello(execution_context root, int id) {
    std::cout << "Hello " << id << "\n";
    root = root.resume<int>(10);
    std::cout << "Hello " << root.get_value<int>() << "\n";
    root = root.resume<int>(20);
    std::cout << "Hello " << root.get_value<int>() << "\n";
    return root;
}

int main() {
    execution_context ctx = call(say_hello, 2);
    std::cout << "returned value " << ctx.get_value<int>() << "\n";
    ctx = ctx.resume<int>(30);
    std::cout << "returned value " << ctx.get_value<int>() << "\n";
    ctx = ctx.resume<int>(40);
    return 0;
}
