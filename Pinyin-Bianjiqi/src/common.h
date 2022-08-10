#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED 1

#include <cstddef>
#include <QtGlobal>
#include <QTextDocument>

using std::size_t;

#define QSTR(str)  QStringLiteral(str)
#define CSTR(str)  QLatin1String(str)

void convertPinyin(QTextDocument * doc);

#endif
