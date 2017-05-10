#ifndef NOTHREAD_EXCEPTIONS_INCL
#define NOTHREAD_EXCEPTIONS_INCL

#include <stdexcept>

namespace nothread {
    class invalid_resume_error: public std::runtime_error {
    public:
        invalid_resume_error(): std::runtime_error("Attempted to resume non-existing or expired execution_context") {}
    };
}

#endif // NOTHREAD_EXCEPTIONS_INCL
