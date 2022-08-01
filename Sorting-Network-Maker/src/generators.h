#ifndef GENERATORS_H_INCLUDED
#define GENERATORS_H_INCLUDED 1

#include <algorithm>
#include <QColor>
#include <QVector>
#include <QPixmap>
#include <QPainter>

class SortingNetworkBuilder {
public:
    /* r1 < r2 */
    virtual void addComparator(int r1, int r2) = 0;
    virtual void addSynchronizer(int begin, int width) = 0;
    virtual ~SortingNetworkBuilder() {}
};

class SortingNetworkPainter : public SortingNetworkBuilder {
public:
    typedef int                 TestData;
    typedef std::less<TestData> TestCompare;
    SortingNetworkPainter(int n, int m, int width, int height, \
                const QColor& lines, const QColor& background, bool show_test, bool reproducible_random);
    void addComparator(int i, int j) override;
    void addSynchronizer(int, int) override {}
    int operations() const { return ops; }
    int levels() const;
    QPixmap finishPicture();
    bool checkTestResult() const;
protected:
    QPixmap pic;
    QPainter painter;
    QColor color;
    QVector<int> latency;
    QVector<int> l_column;
    QVector<bool> holes;
    QVector<TestData> test_array;
    int ops, n, width, height, lw_vert, lw_hori;
    bool show_test_example;
    void showTestExample(int x);
    void addComparatorImpl(int i, int j);
    void testCompareAndSwap(int i, int j);
};

class LeveledSortingNetworkPainter : public SortingNetworkPainter {
public:
    LeveledSortingNetworkPainter(int n, int m, int width, int height, \
                const QColor& lines, const QColor& background, bool show_test, bool reproducible_random): \
                SortingNetworkPainter(n, m, width, height, lines, background, show_test, reproducible_random){}
    void addSynchronizer(int left, int right) override;
};

void generate_network(int index, int n, SortingNetworkBuilder *builder);
int estimate_columns(int index, int n);

#endif