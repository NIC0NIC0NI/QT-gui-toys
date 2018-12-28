#ifndef CPP17_SUPPORT_H_INCLUDED
#define CPP17_SUPPORT_H_INCLUDED 1
/* for pre-C++17 versions  */

#include <type_traits>
#include <new>
#include <memory>
#include <iterator>
#include <QtGlobal>
// use Q_ASSERT instead of assert, to keep consistent with the whole application

namespace memory {
    template<class InputIt, class ForwardIt>
    inline ForwardIt uninitialized_move(InputIt first, InputIt last, ForwardIt d_first) {
        typedef typename ::std::iterator_traits<ForwardIt>::value_type Value;
        ForwardIt current = d_first;
        for (; first != last; ++first, ++current) {
            ::new (static_cast<void*>(::std::addressof(*current))) Value(std::move(*first));
        }
        return current;
    }

    template<typename Type>
    inline void destroy_at(Type *p) {
        p->~Type();
    }

    template<typename ForwardIt>
    inline void destroy(ForwardIt first, ForwardIt last) {
        for (; first != last; ++first) {
            destroy_at(::std::addressof(*first));
        }
    }
}

namespace utility {
    template<typename ValueType>
    class optional {
    public:
        typedef ValueType value_type;

        // only implemented used methods

        optional() : presented(false) { }
        ~optional() {
            reset();
        }

        value_type* get_ptr() {
            Q_ASSERT(presented);
            return reinterpret_cast<value_type*>(&storage);
        }
        
        value_type* operator->(){
            return get_ptr();
        }

        value_type& value(){
            return *get_ptr();
        }

        bool has_value() const noexcept {
            return presented;
        }

        template<typename ... Args>
        value_type& emplace(Args&& ... args) { 
            reset();
            auto p = ::new (&storage) value_type(::std::forward<Args>(args)...);
            presented = true;
            return *p;
        }

        void reset() noexcept {
            if(presented) {
                ::memory::destroy_at(get_ptr());
                presented = false;
            }
        }
    private:
        typedef typename ::std::aligned_storage<sizeof(ValueType), alignof(ValueType)>::type Storage;
        Storage storage;
        bool presented;
    };
}
#endif