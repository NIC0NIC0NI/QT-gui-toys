#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QTranslator>
#include "common.h"
#include "pinyin_bianjiqi.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    
    QStringList translationFileName = {QSTR("l"), QSTR("language")};
    QStringList inputFileName = {QSTR("o"), QSTR("open")};
    QCommandLineParser parser;
    QCommandLineOption translationFileOpt(translationFileName, 
        QSTR("Load the translation file to translate the UI."), QSTR("translation"));
    QCommandLineOption inputFileOpt(inputFileName, 
        QSTR("The program starts with the initial input file opened."), QSTR("input"));

    QCoreApplication::setApplicationName(QSTR("Pinyin-Bianjiqi"));
    parser.addOption(translationFileOpt);
    parser.addOption(inputFileOpt);
    parser.process(a);
    QString translationFileValue = parser.value(translationFileOpt);

    if(!translationFileValue.isEmpty() && translator.load(translationFileValue)) {
        a.installTranslator(&translator);
    }
    PinyinBianjiqi w(parser.value(inputFileOpt));
    w.show();
    return a.exec();
}
