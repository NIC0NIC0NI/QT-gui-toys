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
    enum OpenFileStatus {ReadSuccess, NewSuccess, ReadFailure, NewFailure, NewCanceled};

public slots:
    void updateText();
    void saveFile();
    void saveAsFile();

    // automatic connected by QMetaObject::connectSlotsByName
    void on_actionOpen_triggered();
    void on_actionNew_triggered();
    void on_textEdit_textChanged();
    void on_actionDelete_triggered();
    void on_actionSaveOutput_triggered();
    void on_actionFont_triggered();
    void on_actionColor_triggered();
    void on_actionAbout_triggered();

private:
    Ui::PinyinBianjiqiClass ui;
    QTimer updateTimer;
    QString currentFileName;

protected:
    virtual void closeEvent(QCloseEvent *event);
    bool askSaveOrContinue();
    void openAsCurrentFile(const QString& filename);
    SaveFileStatus saveTextEditToFile(const QTextEdit &textEdit, const QString &filename);
    void errorMessage(const QString &msg);
    QString fileFormat();
};

#endif
