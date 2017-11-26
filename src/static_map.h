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
    template<class Pair, class Key>
    struct CompareKey {
        CompareKey() {}
        bool operator()(const Pair & a, const Key & b) {
            return a.first < b;
        }
        bool operator()(const Key & a, const Pair & b) {
            return a < b.first;
        }
    };


    // Unable to insert / remove / modify key, keys are determined once constructed
    // Use sorted array and binary search 
    template<typename Key, typename T, size_t AllocSize>
    class static_map {
    public:
        typedef ::std::pair<const Key, T> value_type;
        typedef const value_type*       const_iterator;

        static_map(::std::initializer_list<value_type> init) {
            Q_ASSERT(init.size() == AllocSize);

            ::memory::uninitialized_move(init.begin(), init.end(), this->Begin());
            ::std::sort(this->Begin(), this->End());
        }

        ~static_map() {
            ::memory::destroy(this->Begin(), this->End());
        }

        // only implemented used methods

        template<typename K>
        const_iterator find(const K & x) const {
            return ::algorithm::binary_search(this->begin(), this->end(), x, CompareKey<value_type, K>());
        }

        const_iterator begin() const noexcept {
            return reinterpret_cast<const_iterator>(&storage);
        }

        const_iterator end() const noexcept {
            return this->begin() + AllocSize;
        }

    private:

        typedef ::std::pair<Key, T>*      Iterator;  // no `const` for `Key`, for internal mutability
        typedef typename ::std::aligned_storage<sizeof(value_type) * AllocSize, alignof(value_type)>::type Storage;

        Storage storage;

        Iterator Begin() noexcept {
            return reinterpret_cast<Iterator>(&storage);
        }

        Iterator End() noexcept {
            return this->Begin() + AllocSize;
        }
    };
}
#endif
