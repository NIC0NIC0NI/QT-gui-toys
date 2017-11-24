#ifndef STATIC_MAP_H_INCLUDED
#define STATIC_MAP_H_INCLUDED

#include <new>
#include <memory>
#include <cassert>
#include <utility>
#include <iterator>
#include <algorithm>
#include <initializer_list>

#include "common.h"

namespace memory {  // for pre-C++17 versions
	template<class InputIt, class ForwardIt>
	inline ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt d_first) {
		typedef typename std::iterator_traits<ForwardIt>::value_type Value;
		ForwardIt current = d_first;
		for (; first != last; ++first, ++current) {
			::new (static_cast<void*>(std::addressof(*current))) Value(*first);
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
			destroy_at(std::addressof(*first));
		}
	}
}

namespace algorithm{  // why does `std::binary_search` return a bool instead of an iterator?
	template<class ForwardIt, class Key, class Compare = std::less<Key>>
	inline ForwardIt binary_search(ForwardIt begin, ForwardIt end, const Key& key, Compare comp) {
		auto it = std::lower_bound(begin, end, key, comp);
		if (!(it == end) && !comp(key, *it)) {
			return it;
		}
		else {
			return end;
		}
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
	// Use ordered array and binary search 
	template<typename Key, typename T, size_t AllocSize>
	class static_map {
	public:
		typedef std::pair<const Key, T> value_type;
		typedef const value_type*       const_iterator;

		static_map(std::initializer_list<value_type> init) {
			assert(init.size() == AllocSize);

			memory::uninitialized_copy(init.begin(), init.end(), this->Begin());
			std::sort(this->Begin(), this->End());
		}

		~static_map() {
			memory::destroy(this->Begin(), this->End());
		}

		// only implemented used methods

		template<typename K>
		const_iterator find(const K & x) const {
			return algorithm::binary_search(this->begin(), this->end(), x, CompareKey<value_type, K>());
		}

		const_iterator begin() const noexcept {
			return reinterpret_cast<const_iterator>(&storage);
		}

		const_iterator end() const noexcept {
			return this->begin() + AllocSize;
		}

	private:

		typedef std::pair<Key, T>*      Iterator;  // no `const` for `Key`, for internal mutability
		typedef typename std::aligned_storage<sizeof(value_type) * AllocSize, sizeof(void *)>::type Storage;

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
