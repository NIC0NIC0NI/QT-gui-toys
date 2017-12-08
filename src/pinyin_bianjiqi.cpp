#include <QMessageBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QTextCursor>
#include <QFile>

#include "common.h"
#include "pinyin_bianjiqi.h"

PinyinBianjiqi::PinyinBianjiqi(const char* startFileName, QWidget *parent)
    : QMainWindow(parent), updateTimer(this), currentFile() {
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

    if(startFileName != Q_NULLPTR) {
        if (!this->openFileByName(startFileName)) {
            this->errorMessage(tr("Cannot read file \"%1\"").arg(startFileName));
        }
    }
}

void PinyinBianjiqi::onTextChanged() {
    this->updateTimer.start();
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
    }
}

void PinyinBianjiqi::openFile() {
    if (this->askSaveOrContinue()) {
        auto filename = QFileDialog::getOpenFileName(
            this, tr("Select a file to open"), 
            ".", 
            tr("HTML (*.html);;Text file (*.txt)")
        );
        if (!filename.isNull()) {
            if (!this->openFileByName(filename)) {
                this->errorMessage(tr("Cannot read file \"%1\"").arg(filename));
            }
        }
    }
}

bool PinyinBianjiqi::openFileByName(const QString &filename) {
    QFile reader(filename, this); 
    reader.open(QIODevice::ReadOnly | QIODevice::Text);
    if (reader.isReadable()) {
        auto text = QString::fromUtf8(reader.readAll());
        reader.close();
        this->ui.textEdit->setHtml(text);

        return this->currentFile.emplace(filename, this)
                .open(QIODevice::WriteOnly | QIODevice::Text);
    }
    else {
        return false;
    }
}

void PinyinBianjiqi::saveFile() {
    if (this->currentFile.has_value()) {
        auto status = this->saveTextEditToFile(
            *this->ui.textEdit, 
            this->currentFile.get_ptr()
        );
        if (status == Success) {
            this->ui.textEdit->document()->setModified(false);
        } else if (status == Failure) {
            this->errorMessage(
                tr("Failed to save \"%1\"").arg(this->currentFile->fileName()));
        }
    }
    else {
        this->saveAsFile();
    }
}

void PinyinBianjiqi::saveAsFile() {
    auto filename = QFileDialog::getSaveFileName(
        this, 
        tr("Save as file"), 
        tr("untitled.html"), 
        tr("HTML (*.html);;Text file (*.txt)")
    );
    if (!filename.isNull()) {
        this->currentFile.emplace(filename, this)
            .open(QIODevice::WriteOnly | QIODevice::Text);
        auto status = this->saveTextEditToFile(
            *this->ui.textEdit, 
            this->currentFile.get_ptr()
        );
        if (status == Success) {
            this->ui.textEdit->document()->setModified(false);
        } else if (status == Failure) {
            this->errorMessage(tr("Failed to save as \"%1\"").arg(filename));
        }
    }
}

void PinyinBianjiqi::saveOutput() {
    auto filename = QFileDialog::getSaveFileName(
        this, 
        tr("Save generated file"), 
        tr("generated.html"), 
        tr("HTML (*.html);;Text file (*.txt)")
    );
    if (!filename.isNull()) {
        QSaveFile file(filename, this);
        if (this->saveTextEditToFile(*this->ui.textShow, &file) == Failure) {
            this->errorMessage(tr("Failed to save generated file as \"%1\"").arg(filename));
        }
    }
}

PinyinBianjiqi::SaveFileStatus PinyinBianjiqi::saveTextEditToFile(
        const QTextEdit &textEdit, QSaveFile *file) {
    QByteArray data;
    if (file->fileName().endsWith(".txt")) {
        if (QMessageBox::question(
                this, 
                tr("Save as text file"), 
                tr("Text format will be discarded in text file, continue to save?"),
                QMessageBox::Ok | QMessageBox::Cancel
            ) == QMessageBox::Ok) {
            data = textEdit.toPlainText().toUtf8();
        }
        else {
            return Canceled;
        }
    }
    else {
        data = textEdit.toHtml().toUtf8();
    }
    if (file->write(data) != data.size()){
        return Failure;
    }
    if (! file->commit()) {
        return Failure;
    }
    return Success;
}


bool PinyinBianjiqi::askSaveOrContinue() {
    if (this->ui.textEdit->document()->isModified())  {
        auto button = QMessageBox::question(
            this, 
            tr("Quit"), 
            tr("Save current file?"),
            QMessageBox::Save | QMessageBox::No | QMessageBox::Cancel
        );
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
    else {
        return true;
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
    auto color = QColorDialog::getColor(textEdit->textColor(), this, tr("Select color"));
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


void PinyinBianjiqi::errorMessage(const QString &msg){
    QMessageBox::warning(this, tr("Error"), msg);
}

void PinyinBianjiqi::about() {
    QMessageBox msg_box(this);
    msg_box.setWindowTitle(tr("About Pīnyīn Biānjíqì"));
    msg_box.setTextFormat(Qt::RichText);   //this is what makes the links clickable
    msg_box.setText(
        tr("<p>This is an opensource software.</p>"
        "<p>Author: <a href='https://github.com/NIC0NIC0NI'>NIC0NIC0NI</a></p>"
        "<p>License: <a href='http://doc.qt.io/qt-5/lgpl.html'>GNU LGPL version 3</a></p>"
        "<p>View source code: "
        "<a href='https://github.com/NIC0NIC0NI/Pinyin-Bianjiqi.git'>"
        "Pinyin-Bianjiqi.git</a></p>")
    );
    msg_box.exec();
}
