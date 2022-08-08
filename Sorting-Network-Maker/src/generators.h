#ifndef GENERATORS_H_INCLUDED
#define GENERATORS_H_INCLUDED 1

#include <cstddef>
#include <cstring>
#include <type_traits>
#include <QScopedArrayPointer>
#include <QVector>
#include <QString>
#include <QColor>
#include <QPicture>

#define SORTING_NETWORK_MAX_INPUT 128

namespace sorting_network {
    typedef std::integral_constant<int, 0> zero_type;
    template<typename T>
    class ScopedArray : public QScopedArrayPointer<T> {
    public:
        void setZero(std::size_t n) { std::memset(this->data(), 0, sizeof(T) * n); }
        ScopedArray(std::size_t n) : QScopedArrayPointer<T>(new T[n]) {}
        ScopedArray(std::size_t n, std::integral_constant<T, 0>) : ScopedArray(n) { setZero(n); }
        using QScopedArrayPointer<T>::data;
        using QScopedArrayPointer<T>::operator[];
    };

    struct Comparator {
        int where, low, high;
        Comparator(){}
        Comparator(int lv, int ll, int hh) : where(lv), low(ll), high(hh) {}
    };
    
    class Layout {
    public:
        Layout(int n) : latency(n, zero_type()), ops(0), input_n(n) {}
        void addComparator(int i, int j);
        void addSynchronizer(int i, int j);
        void layout(bool split_levels, bool compact);
        int getNumberOfComparator() const { return ops; }
        int getLatency() const;
        int getLayoutWidth() const { return column_m; }
    protected:
        QVector<Comparator> comparators;
        ScopedArray<int> latency;
        int ops, input_n, column_m;
        int preprocessLayout(bool split_levels, bool compact);
    };

    class Painter : public Layout {
    public:
        Painter(int n) : Layout(n) {}
        QPicture paint(int width, int height, const QColor& lines, const QColor& background);
    };

    class Tester {
    public:
        typedef int TestData;
        Tester(int n, TestData equal_elements, bool reproducible);
        bool checkSorted() const;
        void compareAndSwap(int i, int j);
        QString showInputAt(int i) const { return showData(input[i], this->low_bits); }
        QString showOutputAt(int i) const { return showData(output[i], this->low_bits); }
    private:
        QScopedArrayPointer<TestData> input, output;
        int input_n;
        TestData low_bits;
        static QString showData(TestData value, TestData low_bits_);
    };

    class TestedPainter : public Painter {
        Tester tester;
    public:
        typedef Tester::TestData TestData;
        TestedPainter(int n, TestData equal_elements, bool reproducible) : \
            Painter(n), tester(n, equal_elements, reproducible) {}
        bool checkSorted() const { return tester.checkSorted(); }
        void addComparator(int i, int j) {
            this->Layout::addComparator(i, j);
            tester.compareAndSwap(i, j);
        }
        using Painter::paint;
        QPicture paint(int width, int height, const QColor& lines, \
            const QColor& background, const QFont& font);
    };

    typedef TestedPainter Builder;

    namespace algorithm {
        void generate_network(int index, int n, Builder *builder);
        int estimate_columns(int index, int n);
        int maximum_n(int index);
    }

    QPixmap picture_to_pixmap(const QPicture& picture);
    bool show_picture(const QPicture& picture, QPaintDevice* device);

    inline int ceil_pow_2(int x) {
        int i;
        for(i = 0; (1 << i) < x; ++i);
        return i;
    }
}


#endif