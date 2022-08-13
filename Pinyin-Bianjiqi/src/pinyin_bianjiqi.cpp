#include <QMessageBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QTextCursor>
#include <QFile>

#include "common.h"
#include "pinyin_bianjiqi.h"

PinyinBianjiqi::PinyinBianjiqi(const QString& startFileName, QWidget *parent)
    : QMainWindow(parent), updateTimer(this), currentFileName() {
    this->ui.setupUi(this);
    this->updateTimer.setSingleShot(true);
    this->updateTimer.setInterval(UPDATE_DELAY);

    connect(this->ui.actionQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(this->ui.actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(this->ui.actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAsFile()));
    connect(&this->updateTimer, SIGNAL(timeout()), this, SLOT(updateText()));

    if(!startFileName.isEmpty()) {
        this->openAsCurrentFile(startFileName);
    }
}

void PinyinBianjiqi::on_textEdit_textChanged() {
    this->updateTimer.start();
}

void PinyinBianjiqi::updateText() {
    auto doc = this->ui.textEdit->document()->clone(this->ui.textShow);
    convertPinyin(doc);
    this->ui.textShow->setDocument(doc);
}

void PinyinBianjiqi::on_actionDelete_triggered() {
    this->ui.textEdit->textCursor().removeSelectedText();
}

void PinyinBianjiqi::on_actionNew_triggered() {
    if (this->askSaveOrContinue()) {
        this->currentFileName.clear();
        this->ui.textEdit->clear();
    }
}

QString PinyinBianjiqi::fileFormat() {
    QString format;
    format.append(tr("HTML")).append(CSTR("(*.html);;"));
    format.append(tr("Text file")).append(CSTR("(*.txt)"));
    return format;
}

void PinyinBianjiqi::on_actionOpen_triggered() {
    if (this->askSaveOrContinue()) {
        auto filename = QFileDialog::getOpenFileName(
            this, tr("Select an input file to open"), 
            QSTR("."), fileFormat()
        );
        if (!filename.isNull()) {
            this->openAsCurrentFile(filename);
        }
    }
}

void PinyinBianjiqi::openAsCurrentFile(const QString& filename) {
    QFile file(filename, this); 
    if(file.exists()) {
        if (file.open(QIODevice::ReadOnly | QIODevice::Text) && file.isReadable()) {
            auto text = QString::fromUtf8(file.readAll());
            file.close();
            this->ui.textEdit->setHtml(text);
            this->currentFileName = filename;
        }
        this->errorMessage(tr("Cannot open file \"%1\".").arg(filename));
    } else {
        if (QMessageBox::question(
                this, 
                tr("File \"%1\" not exist.").arg(filename), 
                tr("Create new?"),
                QMessageBox::Ok | QMessageBox::Cancel
            ) == QMessageBox::Ok) {
            if(file.open(QIODevice::NewOnly | QIODevice::Text) && file.isWritable()) {
                this->currentFileName = filename;
            }
            this->errorMessage(tr("Cannot create file \"%1\".").arg(filename));
        }
    }
}

void PinyinBianjiqi::saveFile() {
    if (this->currentFileName.isNull()) {
        this->saveAsFile();
    } else {
        auto status = this->saveTextEditToFile(
            *this->ui.textEdit, 
            this->currentFileName
        );
        if (status == Success) {
            this->ui.textEdit->document()->setModified(false);
        } else if (status == Failure) {
            this->errorMessage(tr("Failed to save input file \"%1\".").arg(this->currentFileName));
        }
    }
}

void PinyinBianjiqi::saveAsFile() {
    auto filename = QFileDialog::getSaveFileName(
        this, 
        tr("Save input as file"), 
        tr("untitled input.html"), 
        fileFormat()
    );
    if (!filename.isNull()) {
        auto status = this->saveTextEditToFile(*this->ui.textEdit, filename);
        if (status == Success) {
            this->ui.textEdit->document()->setModified(false);
            this->currentFileName.swap(filename);
        } else if (status == Failure) {
            this->errorMessage(tr("Failed to save input as file \"%1\".").arg(filename));
        }
    }
}

void PinyinBianjiqi::on_actionSaveOutput_triggered() {
    auto filename = QFileDialog::getSaveFileName(
        this, 
        tr("Save pīnyīn as file"), 
        tr("untitled pīnyīn.html"),
        fileFormat()
    );
    if (!filename.isNull()) {
        if (this->saveTextEditToFile(*this->ui.textShow, filename) == Failure) {
            this->errorMessage(tr("Failed to save pīnyīn as \"%1\".").arg(filename));
        }
    }
}

PinyinBianjiqi::SaveFileStatus PinyinBianjiqi::saveTextEditToFile(
        const QTextEdit &textEdit, const QString& filename) {
    QByteArray data;
    QSaveFile writer(filename, this);
    if (!writer.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return Failure;
    }
    if (filename.endsWith(CSTR(".txt"))) {
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
    
    if (writer.write(data) != data.size()){
        return Failure;
    }
    if (! writer.commit()) {
        return Failure;
    }
    return Success;
}

// return value: true for continue (no matter saved or not), false for cancel
bool PinyinBianjiqi::askSaveOrContinue() {
    if (this->ui.textEdit->document()->isModified())  {
        auto button = QMessageBox::question(
            this, 
            tr("Quit"), 
            tr("Save the current input file?"),
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

void PinyinBianjiqi::on_actionFont_triggered() {
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

void PinyinBianjiqi::on_actionColor_triggered() {
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

void PinyinBianjiqi::on_actionAbout_triggered() {
    QLatin1String p("<p>"), q("</p>");
    QLatin1String ref_author("<a href='https://github.com/NIC0NIC0NI'>NIC0NIC0NI</a>");
    QLatin1String ref_license("<a href='http://doc.qt.io/qt-5/lgpl.html'>GNU LGPL version 3</a>");
    QLatin1String ref_code("<a href='https://github.com/NIC0NIC0NI/QT-gui-toys.git'>QT-gui-toys.git</a>");
    QString text;
    text.append(p).append((tr("This is an opensource software."))).append(q);
    text.append(p).append(tr("Author: ")).append(ref_author).append(q);
    text.append(p).append(tr("License: ")).append(ref_license).append(q);
    text.append(p).append(tr("View source code: ")).append(ref_code).append(q);
    QMessageBox::about(this, tr("About Pīnyīn Biānjíqì"), text);
}
