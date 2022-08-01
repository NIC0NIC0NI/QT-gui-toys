#include <cmath>
#include <limits>
#include <QRandomGenerator>
#include <fstream>
#include "generators.h"

inline int line_width(int resolution) {
    return std::max(1, resolution / 18);
}

void SortingNetworkPainter::showTestExample(int x) {
    auto width_125 = width + (width >> 2);
    this->painter.eraseRect(x, 0, width_125, n * height);
    for(int i = 0; i < n; ++i) {
        int y = i * height;
        this->painter.drawText(x, y, width_125, height, \
            Qt::AlignCenter, QString().setNum(this->test_array.at(i)));
    }
}

namespace random {

struct Undeterminated {
    int operator()(int i) const {
        return QRandomGenerator::global()->bounded(i);
    }
};

struct Reproducible {
    QRandomGenerator gen;
    Reproducible() : gen(123456789u) {}
    int operator()(int i) {
        return gen.bounded(i);
    }
};

}

SortingNetworkPainter::SortingNetworkPainter(int n, int m, int width, int height,\
                                            const QColor& lines, const QColor& background, \
                                            bool show_test, bool reproducible_test): \
        pic(m * width, n * height), painter(&pic), color(lines), \
        latency(n, 0), l_column(n, 2), holes(n*m, true), \
        ops(0),  n(n), width(width), height(height), show_test_example(show_test), \
        lw_vert(line_width(width)), lw_hori(line_width(height)), test_array(n) {
    this->pic.fill(background);
    this->painter.setPen(lines);
    int left = show_test ? width : 0, right = m * width - 1;
    for(int i = 0; i < n; ++i) {
        int y = i * height + (height - lw_hori + 1) / 2;
        this->painter.fillRect(left, y, right, lw_hori, lines);
        this->test_array.replace(i, i);
    }
    if(reproducible_test) {
        std::random_shuffle(this->test_array.begin(), this->test_array.end(), random::Reproducible());
    } else {
        std::random_shuffle(this->test_array.begin(), this->test_array.end(), random::Undeterminated());
    }
    if(show_test) {
        auto font = this->painter.font();
        font.setPixelSize(height * 2 / 3);
        this->painter.setFont(font);
        showTestExample(0);
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
    auto x = this->test_array.at(r1), y = this->test_array.at(r2);
    TestCompare cmp;
    if(cmp(y, x)) {
        this->test_array.replace(r1, y);
        this->test_array.replace(r2, x);
    }
}

void SortingNetworkPainter::addComparatorImpl(int r1, int r2) {
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
            for(int i = - radius; i <= radius; ++i) {
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
    this->testCompareAndSwap(r1, r2);
    this->addComparatorImpl(r1, r2);
    this->ops += 1;
    auto new_value = std::max(this->latency.at(r1),this->latency.at(r2)) + 1;
    this->latency.replace(r1, new_value);
    this->latency.replace(r2, new_value);
}

QPixmap SortingNetworkPainter::finishPicture() { 
    int xwidth = *std::max_element(this->l_column.cbegin(), this->l_column.cend()) * width;
    if(show_test_example) {
        showTestExample(xwidth + ((3 * width) >> 2));
    }
    this->painter.end();
    pic = pic.copy(0, 0, xwidth + (width << 1), this->n * height);
    return pic;
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