#ifndef NOTHREAD_DETAIL_ASM_INCL
#define NOTHREAD_DETAIL_ASM_INCL

#include <tuple>
#include <utility>

namespace nothread {
    class execution_context;
};

/* Every arch should implement these two functions (probably in assembler)
 *
 * deep_bootstrap is a function, which is only for simplicity
 *
 * It allows to implement the call itself and following context switches in a common way
 *
 * switch_context context takes pointer tor current context and to the new context and
 * does the work.
 */
extern "C" {
    void NOTHREAD_IMPL_deep_bootstrap(void);
    void NOTHREAD_IMPL_switch_context(nothread::execution_context* self,
                                      nothread::execution_context* target);
};

#endif // NOTHREAD_DETAIL_ASM_INCL
