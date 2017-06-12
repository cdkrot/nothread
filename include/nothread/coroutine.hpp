#ifndef NOTHREAD_COROUTINE_INCL
#define NOTHREAD_COROUTINE_INCL

#include <iterator>
#include "nothread/context.hpp"

namespace nothread {
    namespace impl {
        template <typename T>
        class call_as_coroutine;
    }
    
    template <typename T>
    class yield_t {
    public:
        void operator()(T res) {
            ctx = ctx.resume(res);
        }
        
    private:
        yield_t(execution_context ctx): ctx(std::move(ctx)) {}
        
        execution_context ctx;

        friend class impl::call_as_coroutine<T>;
    };

    namespace impl {
        template <typename T>
        class call_as_coroutine {
        public:
            template <typename Func, typename... Args>
            execution_context operator()(execution_context&& ctx, Func func, Args... args) {
                yield_t<T> yield(std::move(ctx));
                func(yield, args...);
                return std::move(yield.ctx);
            }
        };
    }
    
    template<typename T>
    class coroutine {
    private:
        class iterator_impl;
        coroutine() = default;

        template <typename Alloc, typename Func, typename... Args>
        void setup(Alloc alloc, Func func, Args... args) {
            ctx = call_alloc(std::move(alloc), impl::call_as_coroutine<T>(), func, args...);
            
            if (ctx.has_value()) {
                value = ctx.get_value<T>();
                eof = false;
            } else {
                eof = true;
            }
        }
        
    public:
        template <typename Func, typename... Args>
        coroutine(Func func, Args... args) {
            setup(default_stack(), func, std::forward<Args>(args)...);
        }

        template <typename Alloc, typename Func, typename... Args>
        static coroutine<T> with_alloc(Alloc alloc, Func func, Args... args) {
            coroutine<T> res;
            res.setup(std::move(alloc), func, std::forward<Args>(args)...);
            return res;
        }

        
        template <typename U>
        coroutine(const coroutine<U>& other) = delete;

        template <typename U>
        coroutine<T>& operator=(const coroutine<U>& other) = delete;
        
        typedef T value_type;
        typedef iterator_impl iterator;

        iterator begin() {
            return iterator(*this, false);
        }

        iterator end() {
            return iterator(*this, true);
        }

        
    private:
        void fetch_next() {
            ctx = ctx.resume();
            
            if (ctx.has_value())
                value = ctx.get_value<T>();
            else
                eof   = true;
        }

        class iterator_impl {
        public:
            typedef T        value_type;
            typedef void     difference_type;
            typedef const T* pointer;
            typedef T&       reference;
            typedef std::input_iterator_tag iterator_category;

            bool operator==(const iterator_impl& other) const {
                return end == other.end or cor.eof;
            }

            bool operator!=(const iterator_impl& other) const {
                return !(*this == other);
            }

            reference operator*() {
                return cor.value;
            }

            pointer operator->() {
                return &(operator*());
            }
            
            iterator_impl& operator++() {
                cor.fetch_next();
                return *this;
            }

            // since increment invalidates previous iterator
            // no need for this to be different from above.
            iterator_impl& operator++(int) {
                return operator++();
            }

            
        private:
            iterator_impl(coroutine<T>& cor, bool end): cor(cor), end(end) {}

            coroutine<T>& cor;
            bool end;
            friend class coroutine<T>;
        };
        
        bool eof;
        T value;
        execution_context ctx;
    };
}

#endif
