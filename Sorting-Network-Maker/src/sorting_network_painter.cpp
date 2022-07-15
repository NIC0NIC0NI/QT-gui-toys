#include <cmath>
#include <limits>
#include <QRandomGenerator>
#include "generators.h"

inline int line_width(int resolution) {
    return std::max(1, resolution / 18);
}

SortingNetworkPainter::SortingNetworkPainter(int n, int m, int width, int height,\
                                            const QColor& lines, const QColor& background): \
        pic(m * width, n * height), painter(&pic), color(lines), \
        latency(n, 0), l_column(n, 2), holes(n*m, true), \
        ops(0),  n(n), width(width), height(height), \
        lw_vert(line_width(width)), lw_hori(line_width(height)), test_array(n) {
    QRandomGenerator gen;
    this->pic.fill(background);
    this->painter.setPen(lines);
    for(int i = 0; i < n; ++i) {
        int y = i * height + (height - lw_hori + 1) / 2;
        this->painter.fillRect(0, y, m * width - 1, lw_hori, lines);
        this->test_array[i] = gen.bounded(std::numeric_limits<TestData>::max());
    }
}

template<typename Iterator>
bool all_of(Iterator begin, Iterator end) {
    for(Iterator i = begin; i != end; ++i) {
        if(!*i) {
            return false;
        }
    }
    return true;
}

void SortingNetworkPainter::testCompareAndSwap(int r1, int r2) {
    auto x = this->test_array[r1], y = this->test_array[r2];
    TestCompare cmp;
    if(cmp(y, x)) {
        this->test_array[r1] = y;
        this->test_array[r2] = x;
    }
}

void SortingNetworkPainter::addComparatorImpl(int r1, int r2) {
    auto begin = this->holes.begin();
    int first = std::max(this->l_column[r1], this->l_column[r2]);
    for(int c = first; ; ++c) {
        auto i1 = begin + c * this->n + r1;
        auto i2 = begin + c * this->n + r2 + 1;

        if(all_of(i1, i2)) {
            const int lw_vert = this->lw_vert;
            const int lw_hori = this->lw_hori;
            const int width = this->width;
            const int height = this->height;
            const int dy = (r2 - r1) * height;
            const int radius = std::min(width, height) / 8;
            const int x = c * width;
            const int y1 = r1 * height;
            this->painter.fillRect(
                x + (width - lw_vert + 1) / 2, 
                y1 + (height - lw_hori + 1) / 2, 
                lw_vert, dy, this->color
            );
            for(int i = - radius; i <= radius; ++i) {
                int chord = (int)std::sqrt(radius * radius - i*i);
                for(int j = y1 - chord; j <= y1 + chord; ++j) {
                    this->painter.drawPoint(x + i + width / 2, j + height / 2);
                    this->painter.drawPoint(x + i + width / 2, dy + j + height / 2);
                }
            }
            std::fill(i1, i2, false);

            this->l_column[r1] = this->l_column[r2] = c + 1;

            return;
        }
    }
}


void SortingNetworkPainter::addComparator(int r1, int r2) {
    this->testCompareAndSwap(r1, r2);
    this->addComparatorImpl(r1, r2);
    this->ops += 1;
    this->latency[r1] = this->latency[r2] = \
                std::max(this->latency[r1],this->latency[r2]) + 1;
}

QPixmap SortingNetworkPainter::picture() const { 
    return pic.copy(0, 0, (*std::max_element(this->l_column.cbegin(), \
                this->l_column.cend()) + 2) * width, this->n * height); 
}

int SortingNetworkPainter::levels() const {
    return *std::max_element(this->latency.cbegin(), this->latency.cend());
}

void LeveledSortingNetworkPainter::addSynchronizer(int begin, int width) {
    auto ibegin = this->l_column.begin() + begin;
    auto iend = ibegin + width;
    int m = *std::max_element(ibegin, iend);
    std::fill(ibegin, iend, m);
}

bool SortingNetworkPainter::checkTestResult() const {
    return std::is_sorted(test_array.begin(), test_array.end(), TestCompare());
}