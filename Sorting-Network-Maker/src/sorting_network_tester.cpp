#include <algorithm>
#include <numeric>
#include <functional>
#include <QRandomGenerator>
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

template<typename GENERATOR, typename INT>
void generateTestData(QVector<INT>& data, INT equal_elements, INT low_bits) {
    auto b = data.begin(), e = data.end();
    std::iota(b, e, 0);
    std::random_shuffle(b, e, GENERATOR());
    if(low_bits > 0) {
        auto size = data.size(), shrink_size = ((size - 1) / equal_elements) + 1;
        QVector<INT> count(shrink_size, 0);
        for(auto i = b; i != e; ++i) {
            INT key = (*i) / equal_elements;
            INT rank = count.at(key);
            count.replace(key, rank + 1);
            *i = (key << low_bits) | rank;
        }
    }
}

SortingNetworkTester::SortingNetworkTester(int n, TestData equal_elements, bool reproducible) \
     : input(n), output(n), low_bits(ceil_pow_2(equal_elements)) {
    if(reproducible) {
        generateTestData<random::Reproducible<TestData>>(this->input, equal_elements, low_bits);
    } else {
        generateTestData<random::Undeterminated<TestData>>(this->input, equal_elements, low_bits);
    }
    std::copy(this->input.begin(), this->input.end(), this->output.begin());
}

bool SortingNetworkTester::testSorted() const {
    return std::is_sorted(this->output.begin(), this->output.end(), std::less<TestData>());
}

void SortingNetworkTester::compareAndSwap(int r1, int r2) {
    auto x = this->output.at(r1), y = this->output.at(r2), l = this->low_bits;
    if((y >> l) < (x >> l)) {
        this->output.replace(r1, y);
        this->output.replace(r2, x);
    }
}

static const char equalElementNames[] = "abcdefgh"; 

QString SortingNetworkTester::showData(TestData value, TestData low_bits) {
    TestData ih = value >> low_bits;
    QString result;
    result.setNum(ih);
    if(low_bits > 0) {
        result.append(equalElementNames[value - (ih << low_bits)]);
    }
    return result;
}
