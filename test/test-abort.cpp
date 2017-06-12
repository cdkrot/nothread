#include "util.hpp"
#include "nothread/context.hpp"

TEST_CASE("Context abort basic", "[abort]") {;
    verificator<std::string> v;
    
    v.expect("Hello 2");
    v.expect("Hello 3");
    v.expect("returned");
    v.expect("returned-2");
    v.depend_chain({"Hello 2", "returned", "Hello 3", "returned-2"});
    
    auto say_hello = [&v](nothread::execution_context root, std::string a, std::string b, std::string c) {
        v.log("Hello " + a);
        root = root.resume();
        v.log("Hello " + b);
        root = root.resume();
        v.log("Hello " + c);
        root = root.resume();
        v.log("Hello " + a + b + c);
        return root;
    };

    nothread::execution_context ctx = nothread::call(say_hello, "2", "3", "5");
    v.log("returned");
    ctx = ctx.resume();
    v.log("returned-2");
    ctx = ctx.abort();
    REQUIRE(not ctx);
}
