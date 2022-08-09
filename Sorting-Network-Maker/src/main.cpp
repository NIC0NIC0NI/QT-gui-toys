#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QTranslator>
#include "sorting_network_maker.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    QStringList translationFileName = {"l", "language"};
    QCommandLineParser parser;
    QCommandLineOption translationFileOpt(translationFileName, "Load the translation file to translate the UI.", "translation");

    QCoreApplication::setApplicationName("Sorting Network Maker");
    parser.addOption(translationFileOpt);
    parser.process(a);
    QString translationFileValue = parser.value(translationFileOpt);
    
    if(!translationFileValue.isEmpty() && translator.load(translationFileValue)) {
        a.installTranslator(&translator);
    }
    SortingNetworkMaker w;
    w.show();
    return a.exec();
}
