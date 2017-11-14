#include "pinyin_editor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PinyinEditor w;
    w.show();
    return a.exec();
}
