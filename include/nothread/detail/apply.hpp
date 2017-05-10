#ifndef NOTHREAD_DETAIL_APPLY_INCL
#define NOTHREAD_DETAIL_APPLY_INCL

#include <tuple>
#include <utility>

namespace nothread::detail {
#if __cplusplus >= 201703L
    // if compiler conforms to C++17 simply use STL version
    
    using std::apply;
#else
    template <class Func, class Tuple, std::size_t... I>
    constexpr decltype(auto) apply_impl(Func&& func, Tuple&& tuple, std::index_sequence<I...>) {
        return std::forward<Func>(func)(std::get<I>(std::forward<Tuple>(tuple))...);
    } 
    
    template <class Func, class Tuple>
    constexpr decltype(auto) apply(Func&& func, Tuple&& tuple) {
        return apply_impl(std::forward<Func>(func), std::forward<Tuple>(tuple),
                          std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
    }
#endif
}
#endif // NOTHREAD_DETAIL_APPLY_INCL
