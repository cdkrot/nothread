#define private public // todo
#include "nothread/context.hpp"

#include <stddef.h> // todo

using namespace std;
using namespace nothread;

execution_context say_hello(execution_context root, int id) {
    std::cout << "Hello " << id << "\n";
    root = root.resume();
    std::cout << "Hello " << id + 1 << "\n";
    root = root.resume();
    std::cout << "Hello " << id << "\n";
    return std::move(root);
}

int main() {
    cout << offsetof(execution_context, p_ret) << " " << offsetof(execution_context, p_stack) <<
        " " << offsetof(execution_context, p_saved) << " " << offsetof(execution_context, p_caller_context) << "\n";

    cout << sizeof(execution_context) << "\n";

    execution_context ctx = call(say_hello, 2);
    std::cout << "returned\n";
    ctx = ctx.resume();
    std::cout << "returned-2\n";
    ctx = ctx.resume();
    return 0;
}
