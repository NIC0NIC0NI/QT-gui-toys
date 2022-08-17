#include <QRectF>
#include <QPainterPath>
#include <QPainter>
#include "generators.h"

namespace sorting_network {

inline int line_width(int resolution) {
    return std::max(1, resolution / 18);
}

struct PainterTemporary {
    QPainter painter;
    QColor color;
    int width, height, lw_vert, lw_hori;

    PainterTemporary(int n, int m, int w, int h, int margin, \
            const QColor& lines, const QColor& background, QPaintDevice* picture);
    void drawComparator(int r1, int r2, int c);
    void setFont(const QFont& font, int size) {
        auto f = font;
        f.setPixelSize(size);
        this->painter.setFont(f);
    }
};

PainterTemporary::PainterTemporary(int n, int m, int w, int h, int margin, \
        const QColor& lines, const QColor& background, QPaintDevice* picture) \
    : color(lines), width(w), height(h), lw_vert(line_width(w)), lw_hori(line_width(h)) {
    constexpr qreal half = 0.5;
    painter.begin(picture);
    painter.setBrush(lines);
    painter.setPen(lines);
    painter.setBackground(background);
    painter.setBackgroundMode(Qt::TransparentMode);
    painter.eraseRect(0, 0, m * width, n * height);
    int right = m * width - 1 - 2 * margin;
    for(int i = 0; i < n; ++i) {
        QRectF line(margin, i * height + (height - lw_hori + 1) * half, right, lw_hori);
        painter.drawRect(line);
    }
}

void PainterTemporary::drawComparator(int r1, int r2, int c) {
    constexpr qreal half = 0.5;
    const int dy = (r2 - r1) * height;
    const int radius = std::min(width, height) / 8;
    const qreal x = c * width + width * half;
    const qreal y1 = r1 * height + height * half;
    QPainterPath circle1, circle2;
    QRectF line(x - (lw_vert - 1) * half, y1 - (lw_hori - 1) * half, lw_vert, dy);
    circle1.addEllipse(QPointF(x + half, y1 + half), radius, radius);
    circle2.addEllipse(QPointF(x + half, y1 + half + dy), radius, radius);
    painter.drawRect(line);
    painter.drawPath(circle1);
    painter.drawPath(circle2);
}

QPicture Painter::paint(int width, int height, const QColor& lines, \
                       const QColor& background) {
    int m = this->getLayoutWidth() + 4, n = this->input_n;
    QPicture picture;
    PainterTemporary painter(n, m, width, height, 0, lines, background, &picture);
    for(auto& comp : this->comparators) {
        painter.drawComparator(comp.low, comp.high, comp.where + 2);
    }
    picture.setBoundingRect(QRect(0, 0, m * width, n * height));
    return picture;
}

QPicture TestedPainter::paint(int width, int height, const QColor& lines, \
                       const QColor& background, const QFont& font) {
    const int m = this->getLayoutWidth() + 4, n = this->input_n;
    const int width_125 = width + (width >> 2);
    QPicture picture;
    PainterTemporary painter(n, m, width, height, width_125, lines, background, &picture);
    for(auto& comp : this->comparators) {
        painter.drawComparator(comp.low, comp.high, comp.where + 2);
    }
    painter.setFont(font, std::min(width, height) * 2 / 3);
    auto out_left = m * width - 1 - width_125;
    for(int i = 0; i < n; ++i) {
        int y = i * height;
        painter.painter.drawText(0,        y, width_125, height, \
            Qt::AlignCenter, this->tester.showInputAt(i));
        painter.painter.drawText(out_left, y, width_125, height, \
            Qt::AlignCenter, this->tester.showOutputAt(i));
    }
    picture.setBoundingRect(QRect(0, 0, m * width, n * height));
    return picture;
}

bool show_picture(const QPicture& picture, QPaintDevice* device) {
    QPainter painter;
    painter.begin(device);
    painter.drawPicture(0, 0, picture);
    return painter.end();
}

QPixmap picture_to_pixmap(const QPicture& picture) {
    auto rect = picture.boundingRect();
    QPixmap pixmap(rect.width(), rect.height());
    pixmap.fill(Qt::transparent);
    show_picture(picture, &pixmap);
    return pixmap;
}

}
