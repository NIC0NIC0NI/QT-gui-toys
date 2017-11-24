#ifndef PINYIN_EDITOR_H_INCLUDED
#define PINYIN_EDITOR_H_INCLUDED

#include <QMainWindow>
#include <QCloseEvent>
#include <QSaveFile>
#include <QScopedPointer>
#include <QTimer>

#include "ui_pinyin_bianjiqi.h"

static const int UPDATE_DELAY = 500; // 0.5s

class PinyinBianjiqi : public QMainWindow
{
    Q_OBJECT

public:
    PinyinBianjiqi(const char* startFileName, QWidget *parent = Q_NULLPTR);

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
	QScopedPointer<QSaveFile> currentFile;

protected:
	virtual void closeEvent(QCloseEvent *event);
	bool askSaveOrContinue();
	void openFileByName(const QString &filename);
	void saveFileTo(QSaveFile *file);
};

#endif
