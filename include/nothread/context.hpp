#ifndef NOTHREAD_CONTEXT_INCL
#define NOTHREAD_CONTEXT_INCL

#include <utility>
#include <tuple>
#include "nothread/exceptions.hpp"
#include "nothread/detail/apply.hpp"
#include "nothread/detail/asm.hpp"
#include "nothread/allocate.hpp"

namespace nothread {
    class execution_context;

    namespace detail {
        template <typename Alloc, typename Func, typename... Args>
        void bootstrap(Alloc* alloc, Func* func, std::tuple<Args...>* args, execution_context* ctx);
    }
    
    class execution_context {
    public:
        execution_context() noexcept {
            p_ret = nullptr;
            p_stack = nullptr;
            p_saved = nullptr;
            p_caller_context = nullptr;
        }

        ~execution_context() {
            if (*this) {
                try {
                    execution_context ctx = abort();
                    if (ctx)
                        std::terminate();
                } catch (...) {
                    std::terminate();
                }
            }
        }
        
        /* copying is prohibited */
        execution_context(const execution_context& other) = delete;
        execution_context& operator=(const execution_context& other) = delete;

        /* but move is allowed */
        execution_context(execution_context&& other) noexcept : execution_context() {
            this->swap(other);
        }
        execution_context& operator=(execution_context&& other) noexcept {
            this->swap(other);
            return *this;
        }

        void swap(execution_context& other) noexcept {
            std::swap(p_ret,            other.p_ret);
            std::swap(p_stack,          other.p_stack);
            std::swap(p_saved,          other.p_saved);
            std::swap(p_caller_context, other.p_caller_context);
        }
        
        operator bool() const noexcept {
            return p_ret != nullptr;
        }
        
        bool operator!() const noexcept {
            return not bool(*this);
        }
        
        bool has_value() const noexcept {
            return p_saved != nullptr;
        }

        template <typename T>
        T get_value() {
            return std::forward<T>(*(T*)(p_saved));
        }

        execution_context abort() {
            return resume_advanced(2);
        }
        
        execution_context resume() {
            return resume_advanced(0);
        }

        template <typename T>
        execution_context resume(T arg) {
            execution_context self;
            self.p_saved = &arg;
            return resume_advanced(&self, 0);
        }
        
private:
        /* Flags explanation:
         * 0 -- usual resume
         * 1 -- suicide resume (p_saved points to allocator, p_callercontext to destructor function)
         * 2 -- murder resume
         */
        execution_context resume_advanced(size_t flags) {
            execution_context self;
            return resume_advanced(&self, flags);
        }

        // implementation could be found below.
        execution_context resume_advanced(execution_context* self, size_t flags);
        
        void invalidate() {
            p_ret            = nullptr;
            p_stack          = nullptr;
            p_caller_context = nullptr;
        }
        
        void* p_ret;   /* address where you should jump for return */
        void* p_stack; /* stack pointer you must restore */
        void* p_saved; /* pointer to passed object (if any) */
        execution_context** p_caller_context; /* pointer to address where caller context should be saved */
                
        template <typename Alloc, typename Func, typename... Args>
        friend execution_context call_alloc(Alloc alloc, Func func, Args... args);

        template <typename Alloc, typename Func, typename... Args>
        friend void detail::bootstrap(Alloc* alloc, Func* func, std::tuple<Args...>* args, execution_context* ctx);
    };

    // this class intentionally not derived from any common stl class, so that it won't be caught accidently
    class execution_aborted {
    public:
        execution_context&& get() {
            return std::move(killer);
        }
    private:
        execution_aborted(execution_context ctx): killer(std::move(ctx)) {}
        
        execution_context killer;
        friend class execution_context;
    };
    
    inline execution_context execution_context::resume_advanced(execution_context* self, size_t flags) {
        if (not (*this))
            throw invalid_resume_error();
        
        execution_context* res = nullptr;
        
        if (flags != 1) // in case of flag=1 we suicide and here is destructor.
            self->p_caller_context = &res;
        
        size_t nflags = NOTHREAD_IMPL_switch_context(self, this, flags);
        
        this->invalidate();
        self->invalidate();
        
        if (nflags == 0)
            return std::move(*res);
        else if (nflags == 1) {
            void (*destr)(void*) = (void (*)(void*))res->p_caller_context;
            destr(res->p_saved);
            
            return execution_context();
        } else {
            throw execution_aborted(std::move(*res));
        }
    }
    
    namespace detail {
        template <typename Alloc>
        void deallocator(void* alloc) {
            Alloc copy = std::move(*reinterpret_cast<Alloc*>(alloc));
        }
        
        template <typename Alloc, typename Func, typename... Args>
        void bootstrap(Alloc* alloc, Func* func, std::tuple<Args...>* args, execution_context* ctx) {
            // All the data is placed at the top of our stack, and here we only take
            // "pointers" of them.

            try {
                execution_context res;
                Alloc alloc_copy = std::move(*alloc);
            
                try {
                    Func func_copy = *func;
                    std::tuple<Args...> args_copy = *args;
                    
                    res = nothread::detail::apply(func_copy, std::tuple_cat(std::forward_as_tuple(std::move(*ctx)), args_copy));
                } catch (execution_aborted& abrt) {
                    res = abrt.get();
                } catch (...) {
                    std::terminate();
                }

                execution_context self;
                self.p_caller_context = (execution_context**)&deallocator<Alloc>;
                self.p_saved          = &alloc_copy;
                res.resume_advanced(&self, 1);
            } catch (...) {
                std::terminate();
            }
        }   
    }
    
    template <typename Func, typename... Args>
    execution_context call(Func func, Args... args) {
        return call_alloc(default_stack(), func, std::forward<Args>(args)...);
    }

    template <typename Alloc, typename Func, typename... Args>
    execution_context call_alloc(Alloc alloc, Func func, Args... args) {
        execution_context self, target;
        
        std::tuple<Args...> tup(std::forward<Args>(args)...);
        void** mem = (void**)(alloc.begin()) + (alloc.size() / sizeof(void*));
        
        mem[-1] = (void*)(&detail::bootstrap<Alloc, Func, Args...>);
        mem[-2] = (void*)(&alloc);
        mem[-3] = (void*)(&func);
        mem[-4] = (void*)(&tup);
        
        target.p_ret            = (void*)NOTHREAD_IMPL_deep_bootstrap;
        target.p_stack          = mem - 5;
        target.p_caller_context = (execution_context**)&mem[-5];
        
        auto res = target.resume_advanced(&self, 0);
        return res;
    }
}

#endif // NOTHREAD_CONTEXT_INCL
