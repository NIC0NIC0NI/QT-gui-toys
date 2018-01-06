#ifndef STATIC_MAP_H_INCLUDED
#define STATIC_MAP_H_INCLUDED 1

#include <utility>
#include <type_traits>
#include <initializer_list>
#include <algorithm>
#include <functional>
#include <QtGlobal>
// use Q_ASSERT instead of assert, to keep consistent with the whole application

#include "common.h"
#include "cpp17_support.h"

namespace algorithm{  // why does `std::binary_search` return a bool instead of an iterator?
    template<class ForwardIt, class Key, class Compare = ::std::less<Key>>
    inline ForwardIt binary_search(ForwardIt begin, ForwardIt end, const Key& key, Compare comp) {
        auto it = ::std::lower_bound(begin, end, key, comp);
        return (!(it == end) && !comp(key, *it)) ? it : end;
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
        const_iterator find(const K & x) const {
            return ::algorithm::binary_search(this->begin(), this->end(), x, compare());
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
