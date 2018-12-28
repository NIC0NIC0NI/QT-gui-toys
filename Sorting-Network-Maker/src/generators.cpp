#include <QPainter>
#include"generators.h"

inline int get_columns(int l) {
    return (1 << (l - 1)) * l + (l - 1) * l / 2 + l + 4;
}

inline int ceil_pow_2(int x) {
    int i;
    for(i = 0; (1 << i) < x; ++i);
    return i;
}

inline void init_picture(QPainter *painter, int n, int w, int height) {
    for(int i = 0; i < n; ++i) {
        int y = i * height + height / 2;
        painter->drawLine(0, y, w-1, y);
    }
}

inline void draw_compare(QPainter *painter, int r1, int r2, int c, int width, int height) {
    const int x = c * width + width / 2;
    const int y1 = r1 * height + height / 2, y2 = r2 * height + height / 2;
    painter->drawLine(x, y1, x, y2);
}

void generate_bitonic_bottom_up(GeneratedResult *result, int n, int width, int height) {
    const int l = ceil_pow_2(n);
    const int max_columns = (1 << (l + 1)) + (l - 1) * l / 2 + l;
    const int w = width * max_columns;
    const int h = n * height;
    int column, ops, levels;
    QPixmap picture (w, h);
    QPainter painter (&picture);
    picture.fill();
    init_picture(&painter, n, w, height);

    column = 2;
    ops = 0;
    levels = 0;
    for(int d_1 = 0; d_1 < l; ++d_1) {
        const int length_1 = 1 << d_1;
        for(int j = 0; j < length_1; ++j) {
            const int partner = length_1 * 2 - j - 1;
            int ops_column = 0;

            for(int i = 0; i < n - partner; i += length_1 * 2) {
                draw_compare(&painter, i + j, i + partner, column, width, height);
                ++ops_column;
            }
            if(ops_column > 0) {
                ops += ops_column;
                ++column;
            }
        }
        ++column;
        ++levels;
        for(int d_2 = d_1 - 1; d_2 >= 0; --d_2) {
            int length_2 = 1 << d_2;
            for(int j = 0; j < length_2; ++j) {
                const int partner = j + length_2;
                int ops_column = 0;
                for(int i = 0; i < n - partner; i += length_2 * 2) {
                    draw_compare(&painter, i + j, i + partner, column, width, height);
                    ++ops_column;
                }
                if(ops_column > 0) {
                    ops += ops_column;
                    ++column;
                }
            }
            ++column;
            ++levels;
        }
        ++column;
    }

    result->picture = picture.copy(0,0,column*width,h);
    result->width = column*width;
    result->ops = ops;
    result->levels = levels;
}

void generate_odd_even_bottom_up(GeneratedResult *result, int n, int width, int height) {
    const int l = ceil_pow_2(n);
    const int max_columns = (1 << (l + 1)) + (l - 1) * l / 2 + l;
    const int w = width * max_columns;
    const int h = n * height;
    int column, ops, levels;
    QPixmap picture (w, h);
    QPainter painter (&picture);
    picture.fill();
    init_picture(&painter, n, w, height);

    column = 2;
    ops = 0;
    levels = 0;
    for(int d_1 = 0; d_1 < l; ++d_1) {
        int length_1 = 1 << d_1;
        for(int j = 0; j < length_1; ++j) {
            const int partner = j + length_1;
            int ops_column = 0;
            for(int i = 0; i < n - partner; i += length_1 * 2) {
                draw_compare(&painter, i + j, i + partner, column, width, height);
                ++ops_column;
            }
            if(ops_column > 0) {
                ops += ops_column;
                ++column;
            }
        }
        ++column;
        ++levels;
        for(int d_2 = d_1 - 1; d_2 >= 0; --d_2) {
            int length_2 = 1 << d_2;
            for(int j = 0; j < length_2; ++j) {
                const int partner = j + length_2;
                int ops_column = 0;
                for(int i = length_2; i < n - partner; i += length_2 * 2) {
                    draw_compare(&painter, i + j, i + partner, column, width, height);
                    ++ops_column;
                }
                if(ops_column > 0) {
                    ops += ops_column;
                    ++column;
                }
            }
            ++column;
            ++levels;
        }
        ++column;
    }

    result->picture = picture.copy(0,0,column*width,h);
    result->width = column*width;
    result->ops = ops;
    result->levels = levels;
}

void generate_pairwise_bottom_up(GeneratedResult *result, int n, int width, int height) {
    const int l = ceil_pow_2(n);
    const int max_columns = (1 << (l - 1)) * l + (l - 1) * l / 2 + l + 4;
    const int w = width * max_columns;
    const int h = n * height;
    int column, ops, levels;
    QPixmap picture (w, h);
    QPainter painter (&picture);
    picture.fill();
    init_picture(&painter, n, w, height);

    column = 2;
    ops = 0;
    levels = 0;
    for(int d_1 = 0; d_1 < l; ++d_1) {
        int length_1 = 1 << d_1;
        for(int j = 0; j < length_1; ++j) {
            const int partner = j + length_1;
            int ops_column = 0;
            for(int i = 0; i < n - partner; i += length_1 * 2) {
                draw_compare(&painter, i + j, i + partner, column, width, height);
                ++ops_column;
            }
            if(ops_column > 0) {
                ops += ops_column;
                ++column;
            }
        }
        ++column;
        ++levels;
    }
    ++column;
        
    for(int d_1 = l - 2; d_1 >= 0; --d_1) {
        int length_1 = 1 << d_1;
        for(int d_2 = l - 1; d_2 > d_1; --d_2) {
            int length_2 = 1 << d_2;
            int leap = length_2 - length_1;

            for(int k = length_1; k < length_2; k += length_1 * 2) {
                for(int j = 0; j < length_1; ++j) {
                    int ops_column = 0;
                    for(int i = k + j; i < n - leap; i += length_2) {
                        draw_compare(&painter, i, i + leap, column, width, height);
                        ++ops_column;
                    }
                    if(ops_column > 0) {
                        ops += ops_column;
                        ++column;
                    }
                }
            }
            ++column;
            ++levels;
        }
    }

    result->picture = picture.copy(0,0,column*width,h);
    result->width = column*width;
    result->ops = ops;
    result->levels = levels;
}

typedef void (*Generator)(GeneratedResult *result, int n, int width, int height);
static const Generator generators[] = {
    generate_bitonic_bottom_up,
    generate_odd_even_bottom_up,
    generate_pairwise_bottom_up
};

void generate_network(GeneratedResult *result, int index, int n, int width, int height) {
    generators[index](result, n, width, height);
}