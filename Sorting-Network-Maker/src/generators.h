#ifndef GENERATORS_H_INCLUDED
#define GENERATORS_H_INCLUDED 1

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
protected:
    QPixmap pic;
    QPainter painter;
    QColor color;
    QVector<int> latency;
    QVector<int> l_column;
    QVector<bool> holes;
    int ops, n, width, height, lw_vert, lw_hori;
    void addComparatorImpl(int i, int j);
public:
    SortingNetworkPainter(int n, int m, int width, int height, \
                const QColor& lines, const QColor& background);
    void addComparator(int i, int j) override;
    void addSynchronizer(int, int) override {}
    int operations() const { return ops; }
    int levels() const;
    QPixmap picture() const;
};

class LeveledSortingNetworkPainter : public SortingNetworkPainter {
public:
    LeveledSortingNetworkPainter(int n, int m, int width, int height, \
                const QColor& lines, const QColor& background): \
                SortingNetworkPainter(n, m, width, height, lines, background){}
    void addSynchronizer(int left, int right) override;
};

void generate_network(int index, int n, SortingNetworkBuilder *builder);
int estimate_columns(int index, int n);

#endif