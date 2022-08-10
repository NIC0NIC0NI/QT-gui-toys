#ifndef GENERATORS_H_INCLUDED
#define GENERATORS_H_INCLUDED 1

#include <QVector>
#include <QString>
#include <QColor>
#include <QPicture>
#include "common.h"

namespace sorting_network {
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
        IntegralArray<int> latency;
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
        IntegralArray<TestData> input, output;
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
}


#endif