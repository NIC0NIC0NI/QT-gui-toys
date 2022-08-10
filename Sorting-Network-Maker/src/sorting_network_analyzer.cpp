#include <limits>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <functional>
#include <QRandomGenerator>
#include "generators.h"

namespace sorting_network {

template<typename GENERATOR, typename INT>
void generate_test_data(INT* data, int size, INT equal_elements, INT low_bits, GENERATOR&& gen) {
    auto e = data + size;
    std::iota(data, e, 0);
    std::shuffle(data, e, gen);
    if(low_bits > 0) {
        auto shrink_size = ((size - 1) / equal_elements) + 1;
        IntegralArray<INT> count(shrink_size, std::integral_constant<INT, 0>());
        for(auto i = data; i != e; ++i) {
            INT key = (*i) / equal_elements;
            INT rank = count[key];
            count[key] = rank + 1;
            *i = (key << low_bits) | rank;
        }
    }
}

Tester::Tester(int n, TestData equal_elements, bool reproducible) \
     : input(n), output(n), input_n(n), low_bits(ceil_log_2(equal_elements)) {
    if(reproducible) {
        QRandomGenerator64 g({123456789, 987654321, 0xabcd1234, 0x5678dcba});
        generate_test_data(this->input.data(), n, equal_elements, low_bits, g);
    } else {
        generate_test_data(this->input.data(), n, equal_elements, low_bits, *QRandomGenerator64::global());
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
    QString result(QString::number(ih));
    if(low_bits > 0) {
        result.append(equal_element_names[value - (ih << low_bits)]);
    }
    return result;
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
    IntegralArray<int> counter(max_value + 1, zero_type());
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

int layout_comparator_compact(int n, int r1, int r2, int* position, bool* occupied) {
    int pos = std::max(position[r1], position[r2]);
    for(int col = pos; ; ++col) {
        auto i1 = occupied + col * n + r1;
        auto i2 = occupied + col * n + (r2 + 1);

        if(std::none_of(i1, i2, [](bool p) { return p; })) {
            std::fill(i1, i2, true);
            position[r1] = position[r2] = col + 1;
            return col;
        }
    }
}

int Layout::preprocessLayout(bool split_levels, bool compact) {
    const auto n = this->input_n;
    IntegralArray<int> position(n, zero_type());
    const auto lb = this->latency.data(), le = lb + n;
    const auto pb = position.data(), pe = pb + n;
    int last_latency = 0;

    if(split_levels) {
        counting_sort(this->comparators,
            max_value(lb, le), [](const Comparator& c) { return c.where; }
        );
    }
    // recompute latency without synchronizers
    this->latency.fill(n, zero_type());
    for(auto& c : this->comparators) {
        if(split_levels && last_latency != c.where) {
            last_latency = c.where;
            update_as_max(pb, pe);
        }
        auto pos = layout_comparator(c.low, c.high, lb, pb);
        if(!compact) {
            c.where = pos;
        }
    }
    return max_value(pb, pe);
}


void Layout::layout(bool split_levels, bool compact) {
    auto n = this->input_n;
    IntegralArray<int> position(n, zero_type());
    const auto pb = position.data(), pe = pb + n;
    auto column_m_loose = this->preprocessLayout(split_levels, compact);
    if(compact) {  // recompute a compact layout
        IntegralArray<bool> occupied(n * column_m_loose, false_type());
        const auto o = occupied.data();
        int last_latency = 0;
        for(auto& c : this->comparators) {
            if(split_levels && last_latency != c.where) {
                last_latency = c.where;
                update_as_max(pb, pe);
            }
            c.where = layout_comparator_compact(n, c.low, c.high, pb, o);
        }
        column_m = max_value(pb, pe);
    } else {
        column_m = column_m_loose;
    }
}

int Layout::getLatency() const {
    return max_value(this->latency.data(), this->latency.data() + this->input_n);
}

}
