#ifndef NOTHREAD_ALLOCATE_INCL
#define NOTHREAD_ALLOCATE_INCL

#include <algorithm>
#include <stdexcept>
#include <memory>
#include <new>

#include "nothread/platform.hpp"

namespace nothread {
    namespace detail {
        class stack_common {
        public:
            stack_common() = default;
            
            stack_common(const stack_common& other) = delete;
            stack_common& operator=(const stack_common& other) = delete;
                        
            void swap(stack_common& other) {
                std::swap(sz_, other.sz_);
                std::swap(pmem_, other.pmem_);
            }
            
            void* begin() const noexcept {
                return pmem_;
            }
            
            size_t size() const noexcept {
                return sz_;
            }
            
        protected:
            void* pmem_;
            size_t sz_;
        };
    }

}

#include "nothread/allocate/mmap.hpp"
#include "nothread/allocate/heap.hpp"

#ifdef NOTHREAD_PLATFORM_MMAP_SUPPORTED
namespace nothread {
    class default_stack: public mmap_stack {
    };
}
#endif

#endif
