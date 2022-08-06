#ifndef SORTING_NETWORK_MAKER_H_INCLUDED
#define SORTING_NETWORK_MAKER_H_INCLUDED 1

#include <QFlags>
#include <QColor>
#include <QFont>
#include <QString>
#include <QPixmap>
#include <QCloseEvent>
#include <QDialog>
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
    void selectResolution();
    void selectExampleFont();
    void selectStabilityTestType();

private:
    Ui::SortingNetworkMakerClass ui;
    QPixmap picture;
    QColor lines, background;
    QFont exampleFont;
    int resolution;
    int equalElements;
    bool generated, saved;

protected:
    virtual void closeEvent(QCloseEvent *event);
    bool askSaveOrContinue();
    void refresh();
    template<typename Builder>
    void generateWith(int n, int index, int col_est);
};

#endif
