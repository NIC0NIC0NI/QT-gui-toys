#ifndef GENERATORS_H_INCLUDED
#define GENERATORS_H_INCLUDED 1

#include <QPixmap>

struct GeneratedResult {
    QPixmap picture;
    int ops, levels, width;
};

void generate_network(GeneratedResult *result, int index, int n, int width, int height);

#endif