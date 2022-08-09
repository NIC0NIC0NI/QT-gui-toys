#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QTranslator>
#include "pinyin_bianjiqi.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    QStringList translationFileName = {"l", "language"};
    QStringList inputFileName = {"o", "open"};
    QCommandLineParser parser;
    QCommandLineOption translationFileOpt(translationFileName, "Load the translation file to translate the UI.", "translation");
    QCommandLineOption inputFileOpt(inputFileName, "The program starts with the initial input file opened.", "input");

    QCoreApplication::setApplicationName("Pinyin-Bianjiqi");
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
