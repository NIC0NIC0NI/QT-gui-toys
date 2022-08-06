#ifndef GENERATORS_H_INCLUDED
#define GENERATORS_H_INCLUDED 1

#include <QColor>
#include <QVector>
#include <QPixmap>
#include <QPainter>
#include "tester.h"

class SortingNetworkBuilder {
public:
    /* r1 < r2 */
    virtual void addComparator(int r1, int r2) = 0;
    virtual void addSynchronizer(int begin, int width) = 0;
    virtual ~SortingNetworkBuilder() {}
};

void generate_network(int index, int n, SortingNetworkBuilder *builder);
int estimate_columns(int index, int n);

class SortingNetworkPainter : public SortingNetworkBuilder {
protected:
    QPixmap pic;
    QPainter painter;
    QColor color;
    QVector<int> latency;
    QVector<int> l_column;
    QVector<bool> holes;
    int ops, n, width, height, lw_vert, lw_hori;
    void drawComparator(int i, int j);
    int pictureWidth() const;
    QPixmap finishPicture(int xwidth);
public:
    SortingNetworkPainter(int n, int m, int width, int height, \
                const QColor& lines, const QColor& background);
    void addComparator(int, int) override;
    void addSynchronizer(int, int) override {}
    int operations() const { return ops; }
    int levels() const;
    QPixmap finishPicture() {
        return finishPicture(pictureWidth());
    }
};

class LeveledSortingNetworkPainter : public SortingNetworkPainter {
public:
    LeveledSortingNetworkPainter(int n, int m, int width, int height, \
                const QColor& lines, const QColor& background): \
                SortingNetworkPainter(n, m, width, height, lines, background) { }
    void addSynchronizer(int, int) override;
};

template<typename BasePainter>
class TestedSortingNetworkPainter : public BasePainter {
protected:
    SortingNetworkTester tester;
    QPixmap finishPicture(int xwidth, const QFont& testFont);
public:
    TestedSortingNetworkPainter(int n, int m, int width, int height, \
                const QColor& lines, const QColor& background, \
                int equal_elements, bool reproducible_random): \
                BasePainter(n, m, width, height, lines, background), \
                tester(n, equal_elements, reproducible_random) { }
    
    void addComparator(int i, int j) override;
    bool checkTestResult() const {
        return tester.testSorted();
    }
    QPixmap finishPicture(const QFont& testFont) {
        return finishPicture(pictureWidth(), testFont);
    }
    QPixmap finishPicture() {
        return BasePainter::finishPicture(pictureWidth());
    }
};

#endif