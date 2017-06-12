#include "nothread/coroutine.hpp"

#include <iostream>
#include <vector>

using namespace std;
using namespace nothread;

void func(yield_t<int>& yield, vector<int> a) {
    for (int x: a) {
        yield(2 * x + 0);
        yield(2 * x + 1);
    }
}

int main() {
    vector<int> a = {1,2,3};
    
    for (int elem: coroutine<int>(func, a))
        cout << elem << " ";
    cout << "\n";
    return 0;
}
