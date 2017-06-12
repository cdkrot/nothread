#include "util.hpp"
#include "nothread/coroutine.hpp"

static void func(nothread::yield_t<int>& yield, std::vector<int> a) {
    for (int x: a) {
        yield(2 * x + 0);
        yield(2 * x + 1);
    }
}

TEST_CASE("coroutine basic", "[coroutine]") {
    std::vector<int> a = {1, 2, 3};
    std::vector<int> exp = {2, 3, 4, 5, 6, 7};
    
    std::vector<int> r;
    
    for (int elem: nothread::coroutine<int>(func, a))
        r.push_back(elem);

    REQUIRE(r == exp);
}

static void bar(nothread::yield_t<std::string>& yield, std::string a, std::string b, std::string c) {
    yield(a);
    yield(b);
    yield(c);
}

static void bar2(nothread::yield_t<std::string>& yield, const std::string& a, const std::string& b, const std::string& c) {
    yield(a);
    yield(b);
    yield(c);
}

TEST_CASE("coroutine complex 1", "[coroutine]") {
    std::vector<std::string> exp = {"123", "456", "7"};
    std::vector<std::string> r, r2;
    
    for (std::string elem: nothread::coroutine<std::string>(bar, "123", "456", "7"))
        r.push_back(elem);

    for (std::string elem: nothread::coroutine<std::string>(bar2, "123", "456", "7"))
        r2.push_back(elem);
    
    REQUIRE(r  == exp);
    REQUIRE(r2 == exp);
}
