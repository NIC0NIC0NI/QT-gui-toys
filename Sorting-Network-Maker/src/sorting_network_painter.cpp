#include <cmath>
#include <limits>
#include "generators.h"

inline int line_width(int resolution) {
    return std::max(1, resolution / 18);
}

SortingNetworkPainter::SortingNetworkPainter(int n, int m, int width, int height,\
                                            const QColor& lines, const QColor& background): \
        pic(m * width, n * height), painter(&pic), color(lines), \
        latency(n, 0), l_column(n, 2), holes(n*m, true), \
        ops(0),  n(n), width(width), height(height), \
        lw_vert(line_width(width)), lw_hori(line_width(height)) {
    this->pic.fill(background);
    this->painter.setPen(lines);
    int left = 0, right = m * width - 1;
    for(int i = 0; i < n; ++i) {
        int y = i * height + (height - lw_hori + 1) / 2;
        this->painter.fillRect(left, y, right, lw_hori, lines);
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

void SortingNetworkPainter::drawComparator(int r1, int r2) {
    auto begin = this->holes.begin();
    int first = std::max(this->l_column.at(r1), this->l_column.at(r2));
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
            for(int i = - radius; i <= radius; ++i) { // fill circle
                int chord = (int)std::sqrt(radius * radius - i*i);
                for(int j = y1 - chord; j <= y1 + chord; ++j) {
                    this->painter.drawPoint(x + i + width / 2, j + height / 2);
                    this->painter.drawPoint(x + i + width / 2, dy + j + height / 2);
                }
            }
            std::fill(i1, i2, false);
            this->l_column.replace(r1, c + 1);
            this->l_column.replace(r2, c + 1);
            return;
        }
    }
}

void SortingNetworkPainter::addComparator(int r1, int r2) {
    this->drawComparator(r1, r2);
    this->ops += 1;
    auto new_value = std::max(this->latency.at(r1),this->latency.at(r2)) + 1;
    this->latency.replace(r1, new_value);
    this->latency.replace(r2, new_value);
}

void LeveledSortingNetworkPainter::addSynchronizer(int begin, int width) {
    auto ibegin = this->l_column.begin() + begin;
    auto iend = ibegin + width;
    int m = *std::max_element(ibegin, iend);
    std::fill(ibegin, iend, m);
}

int SortingNetworkPainter::levels() const {
    return *std::max_element(this->latency.cbegin(), this->latency.cend());
}

int SortingNetworkPainter::pictureWidth() const {
    return *std::max_element(this->l_column.cbegin(), this->l_column.cend()) * width;
}

QPixmap SortingNetworkPainter::finishPicture(int xwidth) { 
    this->painter.end();
    pic = pic.copy(0, 0, xwidth + (width << 1), this->n * height);
    return pic;
}

template<typename BasePainter>
QPixmap TestedSortingNetworkPainter<BasePainter>::finishPicture(int xwidth, const QFont& testFont) {
    auto out_left = xwidth + ((3 * width) >> 2);
    auto width_125 = width + (width >> 2);
    auto font = testFont;
    font.setPixelSize(height * 2 / 3);
    this->painter.setFont(font);
    this->painter.eraseRect(0,        0, width_125, n * height);
    this->painter.eraseRect(out_left, 0, width_125, n * height);
    for(int i = 0; i < n; ++i) {
        int y = i * height;
        this->painter.drawText(0,        y, width_125, height, \
            Qt::AlignCenter, this->tester.showInputAt(i));
        this->painter.drawText(out_left, y, width_125, height, \
            Qt::AlignCenter, this->tester.showOutputAt(i));
    }
    return BasePainter::finishPicture(xwidth);
}

template<typename BasePainter>
void TestedSortingNetworkPainter<BasePainter>::addComparator(int i, int j) {
    BasePainter::addComparator(i, j);
    tester.compareAndSwap(i, j);
}

template QPixmap TestedSortingNetworkPainter<SortingNetworkPainter>::finishPicture(int xwidth, const QFont& testFont);
template QPixmap TestedSortingNetworkPainter<LeveledSortingNetworkPainter>::finishPicture(int xwidth, const QFont& testFont);
template void TestedSortingNetworkPainter<SortingNetworkPainter>::addComparator(int i, int j);
template void TestedSortingNetworkPainter<LeveledSortingNetworkPainter>::addComparator(int i, int j);
