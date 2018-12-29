#ifndef GENERATORS_H_INCLUDED
#define GENERATORS_H_INCLUDED 1

#include <QPixmap>
#include <QPainter>
#include <QColor>

class SortingNetworkBuilder {
public:
    /* r1 < r2 */
    virtual void addComparator(int r1, int r2) = 0;
    virtual void addLevel() {};
};

class SortingNetworkPainter : public SortingNetworkBuilder {
protected:
    QPixmap pic;
    QPainter painter;
    QColor color;
    int ops, n, width, height, lw_vert, lw_hori;
    QVector<bool> holes;
    QVector<int> l_column;
    void addComparatorImpl(int i, int j, int column_base);
public:
    SortingNetworkPainter(int n, int m, int width, int height, \
                const QColor& lines, const QColor& background);
    void addComparator(int i, int j) override;
    QPixmap picture() const;
    int operations() const { return ops; }
};

class LevelSortingNetworkPainter : public SortingNetworkPainter {
    QVector<int> latency;
    int column_base;
public:
    LevelSortingNetworkPainter(int n, int m, int width, int height, \
                const QColor& lines, const QColor& background);
    void addComparator(int i, int j) override;
    void addLevel() override;
    int levels() const;
};

void generate_network(int index, int n, SortingNetworkBuilder *builder);
int estimate_columns(int index, int n);

#endif