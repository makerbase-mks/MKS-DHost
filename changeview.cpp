#include "changeview.h"

Changeview::Changeview(MainWindow *mparent,QWidget *parent) : QWidget(parent)
{
    this->mparent = mparent;
    this->setMaximumWidth(230);
    this->setMinimumWidth(230);
    initUI();
}
Changeview::~Changeview()
{
}
void Changeview::initUI()
{
    mainwidget = new QWidget();
    mainwidget->setMinimumWidth(230);
    mainwidget->setMaximumWidth(230);
    mainlayout = new QVBoxLayout();
    mainlayout->setMargin(0);
    mainlayout->setSpacing(0);
    progresslabel = new QLabel();
    progresslabel->setText(mparent->dialog_slicing);
    progresslabel->setStyleSheet("margin-left:10px;font:14px;");
    progresslayout = new QVBoxLayout();
    progresslayout->setMargin(2);
    progresslayout->setSpacing(0);
    pb = new MProgressBar();
    pb->setMinimumSize(210, 15);
    pb->setMaximumSize(210, 15);
    progressbtn = new QPushButton();
    progressbtn->setText(mparent->dialog_cancel);
    progressbtn->setObjectName("cancel");
    progressbtn->setMaximumSize(50, 20);
    progressbtn->setMinimumSize(50, 20);
    connect(progressbtn, SIGNAL(pressed()), this, SLOT(btnPress()));
    connect(progressbtn, SIGNAL(released()), this, SLOT(btnRelease()));
    progressbtn->setStyleSheet("background-color:#126e58;font:14px;color:#ffffff;border:0px;");
    progresslayout->addWidget(progresslabel);
    progresslayout->addWidget(pb);
    progresslayout->addWidget(progressbtn);
    progresslayout->setAlignment(pb, Qt::AlignCenter);
    progresslayout->setAlignment(progressbtn, Qt::AlignCenter);
    mainlayout->addLayout(progresslayout, 1);
    mainlayout->layout();

    this->setLayout(mainlayout);
}
