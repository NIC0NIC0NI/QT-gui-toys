#include <QApplication>
#include "pinyin_bianjiqi.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    PinyinBianjiqi w((argc > 1) ? argv[1] : Q_NULLPTR);
    w.show();
    return a.exec();
}
