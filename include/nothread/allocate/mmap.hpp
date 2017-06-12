#ifndef NOTHREAD_ALLOCATE_MMAP_INCL
#define NOTHREAD_ALLOCATE_MMAP_INCL

#ifdef NOTHREAD_PLATFORM_MMAP_SUPPORTED

#include "sys/mman.h"

#include <iostream>

namespace nothread {
    class mmap_stack: public detail::stack_common {
    public:
        mmap_stack(): mmap_stack(std::max(size_t(1 << 20), minimal())) {}
        mmap_stack(size_t sz) {
            // rounding up to the nearest page
            sz = sz + (sz % pagesize() != 0);
            sz_ = sz;
            
            pmem_ = mmap((void*)0, sz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            if (pmem_ == MAP_FAILED)
                throw std::bad_alloc();
           int err = 0;
#if NOTHREAD_PLATFORM_STACKGROWTH == DOWN
           err = mprotect((char*)pmem_, pagesize(), PROT_NONE);
#else
           err = mprotect((char*)pmem_ + sz - pagesize(), pagesize(), PROT_NONE);
#endif
           if (err != 0) {
               munmap(pmem_, sz_);
               pmem_ = 0;
               throw std::bad_alloc();
           }
        }
        
        ~mmap_stack() {
            if (pmem_)
                munmap(pmem_, sz_);
        }

        mmap_stack(mmap_stack&& other) noexcept {
            pmem_ = (void*)0;
            sz_ = 0;
            
            swap(other);
        }
        
        mmap_stack& operator=(mmap_stack&& other) noexcept {
            swap(other);
            return *this;
        }

        
        static size_t minimal() noexcept {
            return 2 * pagesize();
        }
        
    private:
        static size_t pagesize() noexcept {
            static size_t res = sysconf(_SC_PAGESIZE);
            return res;
        }
    };
}

#endif
#endif
