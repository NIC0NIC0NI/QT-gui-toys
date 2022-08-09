#ifndef PINYIN_EDITOR_H_INCLUDED
#define PINYIN_EDITOR_H_INCLUDED 1

#include <QMainWindow>
#include <QCloseEvent>
#include <QSaveFile>
#include <QTimer>

#include "ui_pinyin_bianjiqi.h"

static const int UPDATE_DELAY = 150; // 0.15s

class PinyinBianjiqi : public QMainWindow
{
    Q_OBJECT

public:
    PinyinBianjiqi(const QString& startFileName, QWidget *parent = Q_NULLPTR);
    enum SaveFileStatus {Success, Failure, Canceled};

public slots:
    void updateText();
    void onTextChanged();
    void deleteSelectedText();
    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();
    void saveOutput();
    void selectFont();
    void selectColor();
    void about();

private:
    Ui::PinyinBianjiqiClass ui;
    QTimer updateTimer;
    QString currentFileName;

protected:
    virtual void closeEvent(QCloseEvent *event);
    bool askSaveOrContinue();
    bool openFileByName(const QString &filename);
    SaveFileStatus saveTextEditToFile(const QTextEdit &textEdit, const QString &filename);
    void errorMessage(const QString &msg);
    QString fileFormat();
};

#endif
