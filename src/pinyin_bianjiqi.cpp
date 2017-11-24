#include <QMessageBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QTextCursor>
#include <QFile>

#include "common.h"
#include "pinyin_bianjiqi.h"


PinyinBianjiqi::PinyinBianjiqi(QWidget *parent)
	: QMainWindow(parent), updateTimer(this), currentFile(nullptr), saved(true) {
	this->ui.setupUi(this);
	this->updateTimer.setSingleShot(true);
	this->updateTimer.setInterval(UPDATE_DELAY);

	connect(this->ui.actionQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(this->ui.actionFont, SIGNAL(triggered()), this, SLOT(selectFont()));
	connect(this->ui.actionColor, SIGNAL(triggered()), this, SLOT(selectColor()));
	connect(this->ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	connect(this->ui.actionNew, SIGNAL(triggered()), this, SLOT(newFile()));
	connect(this->ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(this->ui.actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(this->ui.actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAsFile()));
	connect(this->ui.actionSaveOutput, SIGNAL(triggered()), this, SLOT(saveOutput()));
	connect(this->ui.actionDelete, SIGNAL(triggered()), this, SLOT(deleteSelectedText()));
	connect(this->ui.textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
	connect(&this->updateTimer, SIGNAL(timeout()), this, SLOT(updateText()));
}

void PinyinBianjiqi::onTextChanged() {
	this->updateTimer.start();
	this->saved = false;
}

void PinyinBianjiqi::updateText() {
	auto doc = this->ui.textEdit->document()->clone(this->ui.textShow);
	convertPinyin(doc);
	this->ui.textShow->setDocument(doc);
}

void PinyinBianjiqi::deleteSelectedText() {
	this->ui.textEdit->textCursor().removeSelectedText();
}

void PinyinBianjiqi::newFile() {
	if (this->askSaveOrContinue()) {
		this->currentFile.reset();
		this->ui.textEdit->clear();
		this->saved = true;
	}
}

void PinyinBianjiqi::openFile() {
	if (this->askSaveOrContinue()) {
		auto filename = QFileDialog::getOpenFileName(this, tr("Select a file to open"), ".", tr("HTML (*.html);;Text file (*.txt)"));
		if (!filename.isNull()) {
			QFile reader(filename, this); 
			reader.open(QIODevice::ReadOnly | QIODevice::Text);
			if (reader.isReadable()) {
				auto text = QString::fromUtf8(reader.readAll());
				reader.close();
				this->ui.textEdit->setHtml(text);

				this->currentFile.reset(new QSaveFile(filename, this));
				this->currentFile->open(QIODevice::WriteOnly | QIODevice::Text);
				this->saved = true; 
			}
			else {
				QMessageBox::warning(this, tr("Error"), tr("Cannot read file \"%1\"").arg(filename));
			}
		}
	}
}

void PinyinBianjiqi::saveFile() {
	if (this->currentFile.isNull()) {
		this->saveAsFile();
	}
	else {
		this->currentFile->write(this->ui.textEdit->toHtml().toUtf8());
		this->currentFile->commit();
		this->saved = true;
	}
}

void PinyinBianjiqi::saveAsFile() {
	auto filename = QFileDialog::getSaveFileName(this, tr("Save as file"), tr("untitled.html"), tr("HTML (*.html);;Text file (*.txt)"));
	if (!filename.isNull()) {
		this->currentFile.reset(new QSaveFile(filename, this));
		this->currentFile->open(QIODevice::WriteOnly | QIODevice::Text);

		if (filename.endsWith(".txt")) {
			if (QMessageBox::question(this, tr("Save File"), 
				tr("Text format will be discarded in text file, continue to save?"),
				QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok) {

				this->currentFile->write(this->ui.textEdit->toPlainText().toUtf8());
			}
		}
		else {
			this->currentFile->write(this->ui.textEdit->toHtml().toUtf8());
		}
		this->currentFile->commit();
		this->saved = true;
	}
}

void PinyinBianjiqi::saveOutput() {
	auto filename = QFileDialog::getSaveFileName(this);
	if (!filename.isNull()) {
		QSaveFile file(filename, this);
		file.open(QIODevice::WriteOnly | QIODevice::Text);
		file.write(this->ui.textShow->toHtml().toUtf8());
		file.commit();
	}
}


void PinyinBianjiqi::selectFont() {
	bool ok;
	auto textEdit = this->ui.textEdit;
	auto font = QFontDialog::getFont(&ok, textEdit->currentFont(), this, tr("Select font"));
	if (ok) {
		auto cursor = textEdit->textCursor();
		if (cursor.hasSelection()) {
			QTextCharFormat format;
			format.setFont(font);
			cursor.mergeCharFormat(format);
		}
		else {
			textEdit->setCurrentFont(font);
		}
	}
}

void PinyinBianjiqi::selectColor() {
	auto textEdit = this->ui.textEdit;
	auto color = QColorDialog::getColor(this->ui.textEdit->textColor(), this, tr("Select color"));
	if (color.isValid()) {

		auto cursor = textEdit->textCursor();
		if (cursor.hasSelection()) {
			QTextCharFormat format;
			format.setForeground(color);
			cursor.mergeCharFormat(format);
		}
		else {
			textEdit->setTextColor(color);
		}
	}
}

bool PinyinBianjiqi::askSaveOrContinue() {
	if (this->saved) {
		return true;
	}
	else {
		auto button = QMessageBox::question(this, tr("Quit"), tr("Save before quit?"),
			QMessageBox::Save | QMessageBox::No | QMessageBox::Cancel);
		if (button == QMessageBox::No) {
			return true;
		}
		else if (button == QMessageBox::Save) {
			this->saveFile();
			return true;
		}
		else {
			return false;
		}
	}
}

void PinyinBianjiqi::closeEvent(QCloseEvent *event) {
	if (this->askSaveOrContinue()) {
		event->accept();
	}
	else {
		event->ignore();
	}
}


void PinyinBianjiqi::about() {
	QMessageBox msg_box(this);
	msg_box.setWindowTitle(tr("About Pinyin-Bianjiqi"));
	msg_box.setTextFormat(Qt::RichText);   //this is what makes the links clickable
	msg_box.setText(
		tr("<p>This is an opensource application.</p>"
		"<p>Author: <a href='https://github.com/NIC0NIC0NI'>NIC0NIC0NI</a></p>"
		"<p>License: <a href='http://doc.qt.io/qt-5/lgpl.html'>GNU LGPL version 3</a></p>"
		"<p>View source code: <a href='https://github.com/NIC0NIC0NI/Pinyin-Bianjiqi.git'>Pinyin-Bianjiqi.git</a></p>"));
	msg_box.exec();
}
