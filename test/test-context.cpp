#include "util.hpp"
#include "nothread/context.hpp"

TEST_CASE("Context switching basic", "[context]") {;
    verificator<std::string> v;
    
    v.expect("Hello 2");
    v.expect("Hello 3");
    v.expect("Hello 5");
    v.expect("returned");
    v.expect("returned-2");
    v.depend_chain({"Hello 2", "returned", "Hello 3", "returned-2", "Hello 5"});
    
    auto say_hello = [&v](nothread::execution_context root, std::string a, std::string b, std::string c) {
        v.log("Hello " + a);
        root = root.resume();
        v.log("Hello " + b);
        root = root.resume();
        v.log("Hello " + c);
        return root;
    };

    nothread::execution_context ctx = nothread::call(say_hello, "2", "3", "5");
    v.log("returned");
    ctx = ctx.resume();
    v.log("returned-2");
    ctx = ctx.resume();
}

TEST_CASE("Context switching args", "[context]") {;
    auto value = [](nothread::execution_context& root) {
        if (root.has_value())
            return root.get_value<int>();
        return -1;
    };
    
    auto say_hello = [value](nothread::execution_context root) {
        root = root.resume(60);
        REQUIRE(value(root) == 228);
        root = root.resume(33);
        REQUIRE(value(root) == -1);
        auto ctx = root.resume();
        REQUIRE(value(ctx) == 600);
        REQUIRE(value(root) == -1);
        REQUIRE(not root);
        REQUIRE(ctx);
        return ctx;
    };
    
    nothread::execution_context ctx = nothread::call(say_hello);
    REQUIRE(value(ctx) == 60);
    ctx = ctx.resume(228);
    REQUIRE(ctx);
    REQUIRE(value(ctx) == 33);
    ctx = ctx.resume();
    REQUIRE(value(ctx) == -1);
    ctx = ctx.resume(600);
    REQUIRE(value(ctx) == -1);
    REQUIRE(not ctx);
}
