#include <iterator>
#include <algorithm>
#include <numeric>
#include <functional>
#include <QRandomGenerator>
#include <iostream>
#include "generators.h"

namespace random {

template<typename INT>
struct Undeterminated {
    INT operator()(INT i) const {
        return QRandomGenerator::global()->bounded(i);
    }
};

template<typename INT>
struct Reproducible {
    QRandomGenerator gen;
    Reproducible() : gen(123456789u) {}
    INT operator()(INT i) {
        return gen.bounded(i);
    }
};

}

namespace sorting_network {

template<typename GENERATOR, typename INT>
void generate_test_data(int* data, int size, INT equal_elements, INT low_bits) {
    auto e = data + size;
    std::iota(data, e, 0);
    std::random_shuffle(data, e, GENERATOR());
    if(low_bits > 0) {
        auto shrink_size = ((size - 1) / equal_elements) + 1;
        ScopedArray<INT> count(shrink_size, zero_type());
        for(auto i = data; i != e; ++i) {
            INT key = (*i) / equal_elements;
            INT rank = count[key];
            count[key] = rank + 1;
            *i = (key << low_bits) | rank;
        }
    }
}

Tester::Tester(int n, TestData equal_elements, bool reproducible) \
     : input(new int[n]), output(new int[n]), input_n(n), low_bits(ceil_pow_2(equal_elements)) {
    if(reproducible) {
        generate_test_data<random::Reproducible<TestData>>(this->input.data(), n, equal_elements, low_bits);
    } else {
        generate_test_data<random::Undeterminated<TestData>>(this->input.data(), n, equal_elements, low_bits);
    }
    std::memcpy(this->output.data(), this->input.data(), sizeof(int) * n);
}

bool Tester::checkSorted() const {
    return std::is_sorted(this->output.data(), this->output.data() + this->input_n, std::less<TestData>());
}

void Tester::compareAndSwap(int r1, int r2) {
    auto x = this->output[r1], y = this->output[r2], l = this->low_bits;
    if((y >> l) < (x >> l)) {
        this->output[r1] = y;
        this->output[r2] = x;
    }
}

static const char equal_element_names[] = "ABCDEFGH"; 

QString Tester::showData(TestData value, TestData low_bits) {
    TestData ih = value >> low_bits;
    QString result;
    result.setNum(ih);
    if(low_bits > 0) {
        result.append(equal_element_names[value - (ih << low_bits)]);
    }
    return result;
}

template<typename Iterator>
bool not_any(Iterator begin, Iterator end) {
    for(Iterator i = begin; i != end; ++i) {
        if(*i) {
            return false;
        }
    }
    return true;
}

// different from std::max_element, this function handles empty input
template<typename Iterator>
typename std::iterator_traits<Iterator>::value_type 
max_value(Iterator begin, Iterator end) {
    typedef typename std::iterator_traits<Iterator>::value_type Value;
    return std::accumulate(begin, end, 0, 
        [](const Value& x, const Value& y) { return std::max(x, y); }
    );
}

template<typename Iterator>
typename std::iterator_traits<Iterator>::value_type
update_as_max(Iterator begin, Iterator end, \
    typename std::iterator_traits<Iterator>::value_type step = 0) {
    typename std::iterator_traits<Iterator>::value_type 
        new_value = max_value(begin, end);
    std::fill(begin, end, new_value + step);
    return new_value;
}

template<typename T, typename Key>
void counting_sort(QVector<T>& vec, int max_value, Key key) { // stable
    ScopedArray<int> counter(max_value + 1, zero_type());
    QVector<T> cpy(vec.size());
    for(auto& c : vec) {
        ++ counter[key(c)];
    }
    for(int prefix = 0, i = 0; i <= max_value; ++i) {
        int temp = counter[i];
        counter[i] = prefix;
        prefix += temp;
    }
    for(auto& c : vec) {
        int index = counter[key(c)] ++;
        cpy.replace(index, c);
    }
    vec.swap(cpy);
}

void Layout::addComparator(int r1, int r2) {
    auto new_latency = std::max(this->latency[r1], this->latency[r2]) + 1;
    this->latency[r1] = this->latency[r2] = new_latency;
    this->comparators.push_back(Comparator(new_latency, r1, r2));
    this->ops += 1;
}

void Layout::addSynchronizer(int i, int j) {
    auto i1 = this->latency.data() + i, i2 = this->latency.data() + j;
    update_as_max(i1, i2);
}

inline int layout_comparator(int r1, int r2, int* latency, int* position) {
    auto new_latency = std::max(latency[r1], latency[r2]) + 1;
    latency[r1] = latency[r2] = new_latency;
    return update_as_max(position + r1, position + (r2 + 1), 1);
}

int layout_comparator_compact(int n, int r1, int r2, int* position, bool* holes) {
    int pos = std::max(position[r1], position[r2]);
    for(int col = pos; ; ++col) {
        auto i1 = holes + col * n + r1;
        auto i2 = holes + col * n + (r2 + 1);

        if(not_any(i1, i2)) {
            std::fill(i1, i2, true);
            position[r1] = position[r2] = col + 1;
            return col;
        }
    }
}

int Layout::preprocessLayout(bool split_levels, bool compact) {
    const auto n = this->input_n;
    ScopedArray<int> position(n, zero_type());
    const auto lb = this->latency.data(), le = lb + n;
    const auto pb = position.data(), pe = pb + n;

    if(split_levels) {
        int last_latency = 0;
        counting_sort(this->comparators,
            max_value(lb, le), [](const Comparator& c) { return c.where; }
        );
        // recompute latency without synchronizers
        this->latency.setZero(n);
        for(auto& c : this->comparators) {
            if(last_latency != c.where) {
                last_latency = c.where;
                update_as_max(pb, pe);
            }
            auto pos = layout_comparator(c.low, c.high, lb, pb);
            if(!compact) {
                c.where = pos;
            }
        }
    } else {
        this->latency.setZero(n);
        for(auto& c : this->comparators) {
            auto pos = layout_comparator(c.low, c.high, lb, pb);
            if(!compact) {
                c.where = pos;
            }
        }
    }
    column_m = max_value(pb, pe);
    return column_m;
}


void Layout::layout(bool split_levels, bool compact) {
    auto n = this->input_n;
    ScopedArray<int> position(n, zero_type());
    const auto pb = position.data(), pe = pb + n;
    auto est_column = this->preprocessLayout(split_levels, compact);
    if(compact) {  // recompute a compact layout
        ScopedArray<bool> holes(n * est_column, std::false_type());
        const auto h = holes.data();
        if(split_levels) {
            int last_latency = 0;
            for(auto& c : this->comparators) {
                if(last_latency != c.where) {
                    last_latency = c.where;
                    update_as_max(pb, pe);
                }
                c.where = layout_comparator_compact(n, c.low, c.high, pb, h);
            }
        } else {
            for(auto& c : this->comparators) {
                c.where = layout_comparator_compact(n, c.low, c.high, pb, h);
            }
        }
        column_m = max_value(pb, pe);
    } 
}

int Layout::getLatency() const {
    return max_value(this->latency.data(), this->latency.data() + this->input_n);
}

}
