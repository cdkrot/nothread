#ifndef NOTHREAD_CONTEXT_INCL
#define NOTHREAD_CONTEXT_INCL

#include <utility>
#include <tuple>
#include "nothread/exceptions.hpp"
#include "nothread/detail/apply.hpp"
#include "nothread/detail/asm.hpp"

namespace nothread {
    class execution_context;

    namespace detail {
        template <typename Func, typename... Args>
        void bootstrap(Func* func, std::tuple<Args...>* args, execution_context* ctx);
    }
    
    class execution_context {
    public:
        execution_context() noexcept {
            p_ret = nullptr;
            p_stack = nullptr;
            p_saved = nullptr;
            p_caller_context = nullptr;
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
        
        bool operator()() const noexcept {
            return p_ret != nullptr;
        }
        
        bool operator!() const noexcept {
            return not operator()();
        }
        
        bool has_value() const noexcept {
            return p_saved != nullptr;
        }

        template <typename T>
        T get_value() {
            return std::forward<T>(*(T*)(p_saved));
        }
        
        execution_context resume() {
            return resume_advanced(false);
        }

        template <typename T>
        execution_context resume(T arg) {
            execution_context self;
            self.p_saved = &arg;
            return resume_advanced(&self);
        }
        
private:
        execution_context resume_advanced(bool kill_self) {
            execution_context self;
            return resume_advanced(&self);
        }

        execution_context resume_advanced(execution_context* self) {
            if (not operator()())
                throw invalid_resume_error();
            
            execution_context* res = nullptr;
            self->p_caller_context = &res;
            
            NOTHREAD_IMPL_switch_context(self, this);

            this->invalidate();
            self->invalidate();
            
            return std::move(*res);;
        }
        
        void invalidate() {
            p_ret            = nullptr;
            p_stack          = nullptr;
            p_caller_context = nullptr;
        }
        
        void* p_ret;   /* address where you should jump for return */
        void* p_stack; /* stack pointer you must restore */
        void* p_saved; /* pointer to passed object (if any) */
        execution_context** p_caller_context; /* pointer to address where caller context should be saved */
                
        template <typename Func, typename... Args>
        friend execution_context call(Func func, Args... args);

        template <typename Func, typename... Args>
        friend void detail::bootstrap(Func* func, std::tuple<Args...>* args, execution_context* ctx);
    };

    namespace detail {
        template <typename Func, typename... Args>
        void bootstrap(Func* func, std::tuple<Args...>* args, execution_context* ctx) {
            // All the data is placed at the top of our stack, and here we only take
            // "pointers" of them.
            
            try {
                execution_context ret = apply(std::forward<Func>(*func), std::tuple_cat(std::forward_as_tuple(std::move(*ctx)), *args));
                ret.resume_advanced(true);
            } catch (...) {
                std::terminate();
            }
            
            /* if low-level switching fails for some reason we want to abort */
            std::terminate();
        }   
    }
    
    template <typename Func, typename... Args>
    execution_context call(Func func, Args... args) {
        execution_context self, target;
        
        std::tuple<Args...> tup(std::forward<Args...>(args...));
        void** mem  = (void**)(new char[8192] + 8192);
        std::fill((char*)mem - 8192, (char*)mem, 0x01);
        
        mem[-1] = (void*)(&detail::bootstrap<Func, Args...>);
        mem[-2] = (void*)(&func);
        mem[-3] = (void*)(&tup);
        
        target.p_ret            = (void*)NOTHREAD_IMPL_deep_bootstrap;
        target.p_stack          = mem - 4;
        target.p_caller_context = (execution_context**)&mem[-4];
        
        auto res = target.resume_advanced(&self);
        return res;
    }
}

#endif // NOTHREAD_CONTEXT_INCL
