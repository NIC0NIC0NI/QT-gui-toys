#include <algorithm>
#include <cmath>
#include "generators.h"

inline int line_width(int resolution) {
    return std::max(1, resolution / 18);
}

SortingNetworkPainter::SortingNetworkPainter(int n, int m, int width, int height,\
                                            const QColor& lines, const QColor& background): \
        pic(m * width, n * height), painter(&pic), color(lines), \
        ops(0),  n(n), width(width), height(height), \
        lw_vert(line_width(width)), lw_hori(line_width(height)), holes(n*m, true), l_column(n, 2) {
    const int pw = m * width;
    this->pic.fill(background);
    this->painter.setPen(lines);
    for(int i = 0; i < n; ++i) {
        int y = i * height + (height - lw_hori + 1) / 2;
        this->painter.fillRect(0, y, pw - 1, lw_hori, lines);
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


void SortingNetworkPainter::addComparator(int r1, int r2) {
    this->addComparatorImpl(r1, r2, 0);
}

void SortingNetworkPainter::addComparatorImpl(int r1, int r2, int column_base) {
    auto begin = this->holes.begin();
    int first = std::max(std::max(this->l_column[r1], this->l_column[r2]), column_base);
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
            for(int i = - radius; i < radius; ++i) {
                int chord = (int)std::sqrt(radius * radius - i*i);
                for(int j = y1 - chord; j < y1 + chord; ++j) {
                    this->painter.drawPoint(x + i + width / 2, j + height / 2);
                    this->painter.drawPoint(x + i + width / 2, dy + j + height / 2);
                }
            }
            std::fill(i1, i2, false);
            this->ops += 1;

            this->l_column[r1] = this->l_column[r2] = c + 1;

            return;
        }
    }
}


QPixmap SortingNetworkPainter::picture() const { 
    return pic.copy(0, 0, (*std::max_element(this->l_column.cbegin(), \
                this->l_column.cend()) + 2) * width, this->n * height); 
}

void LevelSortingNetworkPainter::addComparator(int r1, int r2) {
    this->addComparatorImpl(r1, r2, this->column_base);
    this->latency[r1] = this->latency[r2] = \
                std::max(this->latency[r1],this->latency[r2]) + 1;
}


LevelSortingNetworkPainter::LevelSortingNetworkPainter(int n, int m, int width, int height,\
                                            const QColor& lines, const QColor& background): \
        SortingNetworkPainter(n, m, width, height, lines, background), \
        latency(n, 0), column_base(2) {}

void LevelSortingNetworkPainter::addLevel() {
    this->column_base = *std::max_element(this->l_column.cbegin(), this->l_column.cend()) + 1;
}

int LevelSortingNetworkPainter::levels() const {
    return *std::max_element(this->latency.cbegin(), this->latency.cend());
}