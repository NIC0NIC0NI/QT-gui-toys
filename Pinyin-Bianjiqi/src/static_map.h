#ifndef STATIC_MAP_H_INCLUDED
#define STATIC_MAP_H_INCLUDED 1

#include <utility>
#include <new>
#include <memory>
#include <type_traits>
#include <iterator>
#include <initializer_list>
#include <algorithm>
#include <functional>
#include <QtGlobal>
// use Q_ASSERT instead of assert, to keep consistent with the whole application

#include "common.h"

namespace memory {  // for pre-c++17 compilers
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

namespace container {
    template<typename Key, typename Value>
    class key_value_pair {
    private:
        typedef  key_value_pair<Key, Value> This;
        Key k;
        Value v;
    public:
        template<typename ArgKey, typename ArgValue>
        constexpr key_value_pair(ArgKey && kk, ArgValue && vv): 
            k(::std::forward<ArgKey>(kk)), v(::std::forward<ArgValue>(vv)) { }
        constexpr key_value_pair(const This & another) = default;
        constexpr key_value_pair(This && another) = default;
        This& operator=(const This & another) = default;
        This& operator=(This && another) = default;

        constexpr const Key & key() const {  // key is immutable
            return this->k;
        }
        constexpr const Value & value() const {
            return this->v;
        }
        Value & value() {
            return this->v;
        }
    };

    template<typename KeyValuePair>
    struct key_compare {
        constexpr key_compare() = default;
        constexpr bool operator()(const KeyValuePair & a, const KeyValuePair & b) const {
            return a.key() < b.key();
        }
        template<typename Key>
        constexpr bool operator()(const Key & a, const KeyValuePair & b) const {
            return a < b.key();
        }
        template<typename Key>
        constexpr bool operator()(const KeyValuePair & a, const Key & b) const {
            return a.key() < b;
        }
    };

    // Unable to insert / remove / modify key, keys are determined once constructed
    // Use sorted array and binary search 
    template<typename Key, typename T, size_t AllocSize>
    class static_map {
    public:
        typedef key_value_pair<Key, T> value_type;
        typedef value_type* iterator;
        typedef const value_type* const_iterator;
    private:
        typedef key_compare<value_type> compare;
        typename ::std::aligned_storage<sizeof(value_type) * AllocSize, alignof(value_type)>::type storage;
    public:
        // only implemented used methods

        static_map(::std::initializer_list<value_type> init){
            Q_ASSERT(AllocSize == init.size());
            
            ::memory::uninitialized_move(init.begin(), init.end(), this->begin());
            ::std::sort(this->begin(), this->end(), compare());
        }

        ~static_map() {
            ::memory::destroy(this->begin(), this->end());
        }

        template<typename K>
        const_iterator lower_bound(const K & x) const {
            return ::std::lower_bound(this->begin(), this->end(), x, compare());
        }

        iterator begin() noexcept {
            return reinterpret_cast<iterator>(&storage);
        }

        iterator end() noexcept {
            return this->begin() + AllocSize;
        }

        const_iterator begin() const noexcept {
            return reinterpret_cast<const_iterator>(&storage);
        }

        const_iterator end() const noexcept {
            return this->begin() + AllocSize;
        }
    };
}
#endif
