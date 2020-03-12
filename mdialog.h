#ifndef MDIALOG_H
#define MDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QWidget>
#include <mtitlebar.h>
#include <mprogressbar.h>
#include <QtGui>
#include <QPainter>
#include <QLineEdit>
#include <mcanvas.h>
#include <QVector2D>
#include <QIntValidator>
#include <mainwindow.h>
#include <updatethread.h>

struct layerresult;
struct whitedata;
class updateThread;
class MainWindow;
class MDialog : public QDialog
{
    Q_OBJECT
public:
    MDialog(MainWindow *mparent);
    void setDType(QString type);
    void setPanel(QWidget *md);
    void setTitle(QString tit);
    void setCancel();
    void initErrorType(QString text);
    void initPanel();
    void initData(QVector2D solution, std::vector<layerresult> resultlayer);
    void initDataCopy(QVector2D solution, int max_size);
    void getImg(int value,std::vector<QImage> &imagePaths);
    int et, ol, be, bc;
    double mapNumbers(double x, int type);
    void imgToLayer(std::vector<QImage> imglist,std::vector<whitedata> &resultlist);
    void getData(QString datakey, QString &data, QString defaultvalue);
    QPushButton *progressbtn;
    bool slicing;
public slots:
    void tbClose();
    void tbMinimize();
    void tbMove(QPoint p);
    void btnPress();
    void btnRelease();
    void setProgress(int progress);
    void setDragText(float thickness, int exposetime, int offlight, int botexpose, int botcount);
    void paintEvent(QPaintEvent *event);
    void MSliderChange(int value);
    void MSliderChangeOld(int value);
    void MSliderChangeCopy(int value);
    void setLayer(QString layer);
    void setLabelText(QString text);
signals:
    void OnCancel();
private:
    QVBoxLayout *mainLayout, *progresslayout, *draglayout, *guidelayout;
    QLineEdit *dragedit;
    MCanvas *dragcanvas;
    QWidget *mainPanel, *dragwidget,*guidewidget;
    MTitleBar *titlebar;
    QString type;
    MProgressBar *pb;
    QLabel *progresslabel, *draglabel, *guidelabel;
    QString string_slicing;    
    QPainter *mp;
    std::vector<layerresult> resultlayer;
    QVector2D resolution;
    QImage nowimg;
    QIntValidator *intvalidator;
    QVector<QImage> imagelist;
    MainWindow *mparent;
    updateThread *mThread;
//    QPainter *guideimage;
    int slicetype;
};

#endif // MDIALOG_H
