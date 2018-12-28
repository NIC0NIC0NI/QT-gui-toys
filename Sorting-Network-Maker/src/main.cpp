#include <QApplication>
#include "sorting_network_maker.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    SortingNetworkMaker w;
    w.show();
    return a.exec();
}
