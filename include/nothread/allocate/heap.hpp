#ifndef NOTHREAD_ALLOCATE_HEAP_INCL
#define NOTHREAD_ALLOCATE_HEAP_INCL

namespace nothread {
    class heap_stack: public nothread::detail::stack_common {
    public:
        heap_stack(): heap_stack(std::max(size_t(1 << 20), minimal())) {}
        heap_stack(size_t sz) {
            sz_ = sz;
            pmem_ = new char[sz_];
        }
        
        ~heap_stack() {
            if (pmem_)
                delete[] reinterpret_cast<char*>(pmem_);
        }

        heap_stack(heap_stack&& other) noexcept {
            pmem_ = (void*)0;
            sz_ = 0;
            
            swap(other);
        }
            
        heap_stack& operator=(heap_stack&& other) noexcept {
            swap(other);
            return *this;
        }

        static size_t minimal() noexcept {
            return 1024;
        }
    };
}

#endif
