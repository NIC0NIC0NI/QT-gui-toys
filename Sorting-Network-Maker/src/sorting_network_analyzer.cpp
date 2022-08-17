#include <limits>
#include <algorithm>
#include <numeric>
#include <functional>
#include <QChar>
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

static const QChar equal_element_names[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

QString Tester::showData(TestData value, TestData low_bits) {
    TestData ih = value >> low_bits;
    QString result(QString::number(ih));
    if(low_bits > 0) {
        result.append(equal_element_names[value - (ih << low_bits)]);
    }
    return result;
}

// different from std::max_element, this function handles empty input
template<typename Iter>
itv_t<Iter> max_value(Iter begin, Iter end) {
    return std::accumulate(begin, end, static_cast<itv_t<Iter>>(0), 
        [](itv_t<Iter> x, itv_t<Iter> y) { return std::max(x, y); }
    );
}

template<typename Iter>
itv_t<Iter> update_as_max(Iter begin, Iter end, itv_t<Iter> step = 0) {
    auto new_value = max_value(begin, end);
    std::fill(begin, end, new_value + step);
    return new_value;
}

template<typename Iter>
inline itv_t<Iter> update_as_max(Iter it, itd_t<Iter> i, itd_t<Iter> j, itv_t<Iter> step = 0) {
    return update_as_max(it + i, it + j, step);
}

template<typename T, typename Key>
void counting_sort(QVector<T>& vec, int max_val, Key key) { // stable
    IntegralArray<int> counter(max_val + 1, zero_type());
    QVector<T> cpy(vec.size());
    for(auto& c : vec) {
        ++ counter[key(c)];
    }
    for(int prefix = 0, i = 0; i <= max_val; ++i) {
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
    this->comparators.push_back(Comparator(r1, r2));
    this->ops += 1;
}

void Layout::addSynchronizer(int i, int j) {
    this->synchronizers.push_back(Comparator(i, j, static_cast<int>(this->comparators.size())));
}

inline int update_latency(int r1, int r2, int* latency) {
    auto new_latency = std::max(latency[r1], latency[r2]) + 1;
    latency[r1] = latency[r2] = new_latency;
    return new_latency;
}

inline int layout_comparator_loose(int r1, int r2, int* position) {
    return update_as_max(position, r1, r2 + 1, 1);
}

int layout_comparator_compact(int r1, int r2, int* position, bool* occupied, int ld) {
    int pos = std::max(position[r1], position[r2]);
    for(int col = pos; ; ++col) { // search from the left to find a compact layout
        auto i1 = occupied + col * ld + r1;
        auto i2 = occupied + col * ld + (r2 + 1);

        if(std::none_of(i1, i2, [](bool p) { return p; })) {
            std::fill(i1, i2, true);
            position[r1] = position[r2] = col + 1;
            return col;
        }
    }
}

inline int split_levels(bool split_parallel, int last_latency, int this_latency, int *tb, int *te) {
    if(split_parallel && last_latency != this_latency) {
        update_as_max(tb, te);
    }
    return this_latency;
}

void Layout::layout(Options options) {
    const int n = this->input_n, nsync = this->synchronizers.size();
    IntegralArray<int> temp(n, zero_type());
    const auto tb = temp.data(), te = tb + n;
    int last_latency = 0, counting_max;
    bool split_parallel = options.testFlag(SplitParallel);

    // reorder comparators if split parallel levels
    if( options.testFlag(SplitRecursive) && split_parallel && nsync > 0 ) {
        IntegralArray<int> temp2(n, zero_type());
        const auto real_latency = temp2.data();
        auto comp = this->comparators.begin();
        int ib = 0, ie;
        this->addSynchronizer(0, n);
        for(int j = 0; j <= nsync; ++j, ib = ie) {
            const auto& sync = this->synchronizers.at(j);
            ie = sync.where;
            for(int i = ib; i < ie; ++i) {
                update_latency(comp[i].low, comp[i].high, real_latency);
                comp[i].where = update_latency(comp[i].low, comp[i].high, tb);
            }
            update_as_max(tb, sync.low, sync.high);
        }
        counting_max = max_value(tb, te);
        this->latency = max_value(real_latency, real_latency + n);
    } else {
        for(auto& c : this->comparators) {
            c.where = update_latency(c.low, c.high, tb);
        }
        this->latency = counting_max = max_value(tb, te);
    }
    temp.fill(n, zero_type());
    if(split_parallel) {
        counting_sort(this->comparators, counting_max, [](const Comparator& c) { return c.where; });
    }
    
    // compute layout
    if(options.testFlag(Compact)) {
        for(auto& c : static_cast<const QVector<Comparator>&>(this->comparators)) {
            last_latency = split_levels(split_parallel, last_latency, c.where, tb, te);
            layout_comparator_loose(c.low, c.high, tb);
        }

        // recompute compact layout
        IntegralArray<bool> occupied(n * max_value(tb, te), false_type());
        const auto o = occupied.data();

        temp.fill(n, zero_type());
        last_latency = 0;
        for(auto& c : this->comparators) {
            last_latency = split_levels(split_parallel, last_latency, c.where, tb, te);
            c.where = layout_comparator_compact(c.low, c.high, tb, o, n);
        }
    } else {
        for(auto& c : this->comparators) {
            last_latency = split_levels(split_parallel, last_latency, c.where, tb, te);
            c.where = layout_comparator_loose(c.low, c.high, tb);
        }
    }
    this->column_m = max_value(tb, te);
}

}
