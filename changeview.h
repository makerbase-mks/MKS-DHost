#ifndef CHANGEVIEW_H
#define CHANGEVIEW_H

#include <QWidget>
#include <mainwindow.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <msbar.h>
#include <mdialog.h>
class MDialog;
class MainWindow;
class ModelData;
class Changeview : public QWidget
{
    Q_OBJECT
public:
    Changeview(MainWindow *mparent, QWidget *parent);
    ~Changeview();
public slots:
signals:
private:
    MainWindow* mparent;
    QVBoxLayout *mainlayout,*progresslayout;
    QWidget *mainwidget;
    MProgressBar *pb;
    QLabel *progresslabel;
    QPushButton *progressbtn;
    void initUI();
};

#endif // CHANGEVIEW_H
