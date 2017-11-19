#ifndef COMMON_H
#define COMMON_H

#include <cstddef>
#include <cstdint>
#include <QTextDocument>

typedef std::size_t size_t;
typedef std::uint8_t byte;

void convertPinyin(QTextDocument * doc);

#endif
