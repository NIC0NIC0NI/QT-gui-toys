#include <cstddef>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QFile>

#include "sorting_network_maker.h"

SortingNetworkMaker::SortingNetworkMaker(QWidget *parent)
    : QMainWindow(parent), generated(false), saved(false) {
    this->ui.setupUi(this);
    connect(this->ui.actionQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(this->ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(this->ui.actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(this->ui.buttonSave, SIGNAL(clicked()), this, SLOT(save()));
    connect(this->ui.buttonGenerate, SIGNAL(clicked()), this, SLOT(generate()));
}

void SortingNetworkMaker::save() {
    auto filename = QFileDialog::getSaveFileName(
        this, 
        tr("Save figure"), 
        tr("untitled.png"), 
        tr("Portable Network Graphics (*.png);;"
        "Joint Photographic Experts Group (*.jpg);;"
        "Portable Pixmap (*.ppm);;"
        "Windows Bitmap (*.bmp);;"
        "X11 Bitmap (*.xbm);;"
        "X11 Pixmap (*.xpm)")
    );
    if (!filename.isNull()) {
        this->result.picture.save(filename, nullptr, 100);
        this->saved = true;
    }
}

void SortingNetworkMaker::generate() {
    auto n = this->ui.selectSize->value();
    auto width = this->ui.selectWidth->value();
    auto height = this->ui.selectHeight->value();
    auto index = this->ui.selectAlgorithm->currentIndex();
    generate_network(&this->result, index, n, width, height);
    this->ui.label->setPixmap(this->result.picture);
    this->generated = true;
    this->saved = false;
}

bool SortingNetworkMaker::askSaveOrContinue() {
    if (this->generated && ! this->saved)  {
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
            this->save();
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

void SortingNetworkMaker::closeEvent(QCloseEvent *event) {
    if (this->askSaveOrContinue()) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

void SortingNetworkMaker::errorMessage(const QString &msg){
    QMessageBox::warning(this, tr("Error"), msg);
}

void SortingNetworkMaker::about() {
    QMessageBox msg_box(this);
    msg_box.setWindowTitle(tr("About Sorting Network Maker"));
    msg_box.setTextFormat(Qt::RichText);   //this is what makes the links clickable
    msg_box.setText(
        tr("<p>This is an opensource software.</p>"
        "<p>Author: <a href='https://github.com/NIC0NIC0NI'>NIC0NIC0NI</a></p>"
        "<p>License: <a href='http://doc.qt.io/qt-5/lgpl.html'>GNU LGPL version 3</a></p>"
        "<p>View source code: "
        "<a href='https://github.com/NIC0NIC0NI/QT-gui-toys.git'>"
        "QT-gui-toys.git</a></p>")
    );
    msg_box.exec();
}
