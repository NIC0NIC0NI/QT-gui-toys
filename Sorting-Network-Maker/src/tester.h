#ifndef TESTS_H_INCLUDED
#define TESTS_H_INCLUDED 1

#include <QVector>
#include <QString>

inline int ceil_pow_2(int x) {
    int i;
    for(i = 0; (1 << i) < x; ++i);
    return i;
}

class SortingNetworkTester {
public:
    typedef int TestData;
    SortingNetworkTester(int n, TestData equal_elements, bool reproducible);
    bool testSorted() const;
    void compareAndSwap(int i, int j);
    QString showInputAt(int i) const { return showData(input.at(i), this->low_bits); }
    QString showOutputAt(int i) const { return showData(output.at(i), this->low_bits); }
private:
    QVector<TestData> input, output;
    TestData low_bits;
    static QString showData(TestData value, TestData low_bits_);
};

#endif