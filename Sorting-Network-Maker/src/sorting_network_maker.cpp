#include <cstddef>
#include <QSize>
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QInputDialog>

#include "sorting_network_maker.h"
#include "generators.h"

SortingNetworkMaker::SortingNetworkMaker(QWidget *parent)
    : QMainWindow(parent), lines(Qt::black), background(Qt::white), \
    resolution(32), generated(false), saved(false) {
    this->ui.setupUi(this);
    connect(this->ui.actionQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(this->ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(this->ui.actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(this->ui.buttonSave, SIGNAL(clicked()), this, SLOT(save()));
    connect(this->ui.buttonGenerate, SIGNAL(clicked()), this, SLOT(generate()));
    connect(this->ui.actionLineColor, SIGNAL(triggered()), this, SLOT(selectLineColor()));
    connect(this->ui.actionBackgroundColor, SIGNAL(triggered()), this, SLOT(selectBackgroundColor()));
    connect(this->ui.actionResolution, SIGNAL(triggered()), this, SLOT(selectResolution()));
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
        this->picture.save(filename, nullptr, 95);
        this->saved = true;
    }
}

void SortingNetworkMaker::selectLineColor() {
    auto color = QColorDialog::getColor(this->lines, this, tr("Select line color"));
    if (color.isValid()) {
        this->lines = color;
    }
}

void SortingNetworkMaker::selectResolution() {
    bool ok;
    auto resolution = QInputDialog::getInt(this, tr("Select resolution"), tr("scale"), 
        this->resolution, 2, 256, 1, &ok);
    if (ok) {
        this->resolution = resolution;
    }
}

void SortingNetworkMaker::selectBackgroundColor() {
    auto color = QColorDialog::getColor(this->background, this, tr("Select background color"));
    if (color.isValid()) {
        this->background = color;
    }
}

void SortingNetworkMaker::refresh() {
    auto size = this->ui.scrollArea->size() - QSize(32,32);
    this->ui.showPicture->resize(size);
    this->ui.showPicture->setPixmap(this->picture.scaled(size, \
            Qt::IgnoreAspectRatio, Qt::FastTransformation));
}

template<typename Builder>
void SortingNetworkMaker::generateWith() {
    auto n = this->ui.selectSize->value();
    auto index = this->ui.selectAlgorithm->currentIndex();
    auto width = this->resolution;
    auto height = this->resolution;
    auto est = estimate_columns(index, n);
    if(est < 0) {
        QMessageBox::warning(this, tr("Warning"), tr("The size is not supported."));
    } else {
        Builder builder(n, estimate_columns(index, n), \
                        width, height, this->lines, this->background, \
                        this->ui.actionShowTestExample->isChecked(), \
                        this->ui.actionReproducibleRandom->isChecked());
        generate_network(index, n, &builder);
        this->picture = builder.picture();
        this->ui.opValueLabel->setText(QString().setNum(builder.operations()));
        this->ui.latencyValueLabel->setText(QString().setNum(builder.levels()));
        this->ui.actionSave->setEnabled(true);
        this->ui.buttonSave->setEnabled(true);
        this->saved = false;
        this->generated = true;
        this->refresh();
        if(!builder.checkTestResult()) {
            QMessageBox::warning(this, tr("Warning"), tr("This network fails the test."));
        }
    }
}

void SortingNetworkMaker::generate() {
    if(this->ui.actionSplitLevels->isChecked()){
        this->generateWith<LeveledSortingNetworkPainter>();
    }
    else {
        this->generateWith<SortingNetworkPainter>();
    }
}

bool SortingNetworkMaker::askSaveOrContinue() {
    if (this->generated && ! this->saved)  {
        auto button = QMessageBox::question(
            this, 
            tr("Quit"), 
            tr("Save figure?"),
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

void SortingNetworkMaker::errorMessage(const QString &msg) {
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
