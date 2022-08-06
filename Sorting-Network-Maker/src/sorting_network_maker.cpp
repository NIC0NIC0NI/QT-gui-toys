#include <cstddef>
#include <QSize>
#include <QMessageBox>
#include <QFontDialog>
#include <QFileDialog>
#include <QColorDialog>
#include <QInputDialog>

#include "sorting_network_maker.h"
#include "generators.h"

SortingNetworkMaker::SortingNetworkMaker(QWidget *parent) \
    : QMainWindow(parent), lines(Qt::black), background(Qt::white), \
    exampleFont(this->font()), resolution(32), equalElements(1), \
    generated(false), saved(false) {
    this->ui.setupUi(this);
    connect(this->ui.actionQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(this->ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(this->ui.actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(this->ui.buttonSave, SIGNAL(clicked()), this, SLOT(save()));
    connect(this->ui.buttonGenerate, SIGNAL(clicked()), this, SLOT(generate()));
    connect(this->ui.actionLineColor, SIGNAL(triggered()), this, SLOT(selectLineColor()));
    connect(this->ui.actionBackgroundColor, SIGNAL(triggered()), this, SLOT(selectBackgroundColor()));
    connect(this->ui.actionResolution, SIGNAL(triggered()), this, SLOT(selectResolution()));
    connect(this->ui.actionExampleFont, SIGNAL(triggered()), this, SLOT(selectExampleFont()));
    connect(this->ui.actionTestStability, SIGNAL(triggered()), this, SLOT(selectStabilityTestType()));
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
        auto ok = this->picture.save(filename, nullptr, 95);
        if(ok) {
            this->saved = true;
        }
    }
}

void SortingNetworkMaker::selectLineColor() {
    auto color = QColorDialog::getColor(this->lines, this, tr("Select line color"));
    if (color.isValid()) {
        this->lines = color;
    }
}

void SortingNetworkMaker::selectBackgroundColor() {
    auto color = QColorDialog::getColor(this->background, this, tr("Select background color"));
    if (color.isValid()) {
        this->background = color;
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


void SortingNetworkMaker::selectExampleFont() {
    bool ok;
    auto font = QFontDialog::getFont(&ok, this->exampleFont, this);
    if (ok) {
        this->exampleFont = font;
    }
}

void SortingNetworkMaker::selectStabilityTestType() {
    bool ok;
    auto eq = QInputDialog::getInt(this, tr("Test sorting stability"), tr("number of equal elements"), 
        this->equalElements, 1, 8, 1, &ok);
    if (ok) {
        this->equalElements = eq;
    }
}

void SortingNetworkMaker::refresh() {
    auto size = this->ui.scrollArea->size() - QSize(50,50);
    auto keepAspectRatio = this->ui.actionKeepAspectRatio->isChecked();
    auto aspectRatio = keepAspectRatio ? Qt::KeepAspectRatioByExpanding : Qt::IgnoreAspectRatio;
    this->ui.scrollArea->setHorizontalScrollBarPolicy(keepAspectRatio ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAsNeeded);
    this->ui.showPicture->resize(size);
    this->ui.showPicture->setPixmap(this->picture.scaled(size, aspectRatio, Qt::SmoothTransformation));
}

template<typename Builder>
void SortingNetworkMaker::generateWith(int n, int index, int col_est) {
    auto width = this->resolution;
    auto height = this->resolution;
    int equal = this->equalElements;
    bool reproducible = this->ui.actionTestReproducible->isChecked();
    Builder builder(n, col_est, width, height, this->lines, this->background, equal, reproducible);
    generate_network(index, n, &builder);
    this->ui.opValueLabel->setText(QString().setNum(builder.operations()));
    this->ui.latencyValueLabel->setText(QString().setNum(builder.levels()));
    this->ui.actionSave->setEnabled(true);
    this->ui.buttonSave->setEnabled(true);
    this->saved = false;
    this->generated = true;
    if(this->ui.actionShowTestExample->isChecked()) {
        this->picture = builder.finishPicture(this->exampleFont);
    } else {
        this->picture = builder.finishPicture();
    }
    this->refresh();
    if(!builder.checkTestResult()) {
        const char* msg = (equal > 1) ? "This network fails the stability test." : "This network fails the test.";
        QMessageBox::warning(this, tr("Warning"), tr(msg));
    }
}

void SortingNetworkMaker::generate() {
    typedef TestedSortingNetworkPainter<LeveledSortingNetworkPainter> Leveled;
    typedef TestedSortingNetworkPainter<SortingNetworkPainter> Unleveled;
    auto n = this->ui.selectSize->value();
    auto index = this->ui.selectAlgorithm->currentIndex();
    auto col_est = estimate_columns(index, n);
    if(col_est < 0) {
        QMessageBox::warning(this, tr("Warning"), tr("The size is not supported."));
    } else if(this->ui.actionSplitLevels->isChecked()) {
        this->generateWith<Leveled>(n, index, col_est);
    } else {
        this->generateWith<Unleveled>(n, index, col_est);
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
