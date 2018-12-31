#ifndef SORTING_NETWORK_MAKER_H_INCLUDED
#define SORTING_NETWORK_MAKER_H_INCLUDED 1

#include <QString>
#include <QColor>
#include <QMainWindow>
#include <QCloseEvent>
#include <QPixmap>
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

private:
    Ui::SortingNetworkMakerClass ui;
    QPixmap picture;
    QColor lines, background;
    int resolution;
    bool generated, saved;

protected:
    virtual void closeEvent(QCloseEvent *event);
    bool askSaveOrContinue();
    void errorMessage(const QString &msg);
    void refresh();
    template<typename Builder>
    void generateWith();
};

#endif
