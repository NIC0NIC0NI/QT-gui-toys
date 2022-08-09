#ifndef SORTING_NETWORK_MAKER_H_INCLUDED
#define SORTING_NETWORK_MAKER_H_INCLUDED 1

#include <QColor>
#include <QFont>
#include <QPixmap>
#include <QPicture>
#include <QCloseEvent>
#include <QMainWindow>
#include "ui_sorting_network_maker.h"

class SortingNetworkMaker : public QMainWindow
{
    Q_OBJECT

public:
    SortingNetworkMaker(QWidget *parent = Q_NULLPTR);

public slots:
    void save();
    void generate();
    void about();
    void selectLineColor();
    void selectBackgroundColor();
    void selectBackgroundTransparency();
    void selectResolution();
    void adjustInputRange(int index);
    void selectExampleFont();
    void selectStabilityTestType();
    void whatsThis();

private:
    Ui::SortingNetworkMakerClass ui;
    QPicture picture;
    QPixmap pixmap;
    QColor lines, background;
    QFont exampleFont;
    int resolution;
    int equalElements;
    bool generated, saved;

protected:
    virtual void closeEvent(QCloseEvent *event);
    bool askSaveOrContinue();
    void refresh();
    void errorMessage(const QString &msg);
};

#endif
