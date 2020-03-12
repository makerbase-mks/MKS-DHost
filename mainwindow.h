#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <m3dviewer.h>
#include <modelloader.h>
#include <modeldata.h>
#include <previewdlg.h>
#include <slice.h>
#include <rightpanel.h>
#include <nrightpanel.h>
#include <QSettings>
#include <mdialog.h>
#include <QTranslator>
#include <QApplication>
#include <updatethread.h>
#include <changeview.h>
struct layerresult;
class PreViewDialog;
class MDialog;
class Slice;

namespace Ui {
class MainWindow;
}
class M3DViewer;
class ModelLoader;
class ModelData;
class RightPanel;
class NRightPanel;
class updateThread;
class Changeview;
struct triangle;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void UpdatePreViewDialog(QVector2D solution, std::vector<layerresult> resultlayer);
    void UpdatePreViewDialogCopy(QVector2D solution,int max_size);
    void ShowErrorDialog(QString text);
    void UpdatePreViewDetail(float thickness, int exposetime, int offlight, int botexpose, int botcount);
    double thickness;
    void getData(QString datakey, QString &data, QString defaultvalue);
    void setData(QString datakey, QString data);
    void deleteModel(int key);
    QString toast_selectmodel, toast_fileerro, toast_mpdtext, toast_printtime, toast_preview, toast_ok,
    dialog_slicing, dialog_cancel, dialog_preview, dialog_dragtext, add_supporting,onestep,twostep,threestep,fourstep,fivestep,sixstep,
    Usage,Previous,Next,stlsaved,pleasesetfilename,filesaved,filesave,fileerror,Notremind;
    MDialog *pvdialog;
    MDialog *guidedialog;
    MDialog *errordialog;
    MDialog *saveprocessdialog;
    QLabel *tt, *piclabel;
   std::vector<QString> text_arr;
   std::vector<QString> pic_arr;
   int currentIndex;
   bool isChange;
   void changeFile(QString filename); //转换格式
   bool isCancel;
   void saveprocess(double value,bool finish);
   bool newSliceType,antialiasing;
   void setadding(bool isadd);

public slots:
    void loadscene();
    void savedlp();
    void savedlpfile();
    void CancelSlicing();
    void showPreView();
    void showPreViewcopy();
    void savestl();
    void loadZip(QString filename);
    void loadCWS(QString filename);
    void showToast(QString t, int tm);
    void showSliceFinish();
    void UpdateLanguage();
    void OutputScreenShot(QDataStream &out);
    void addmodels();
    void overloadmodels();
    void copyModel(int key, QVector3D position);
    void closedialog();
    void filedialog();
//    void modelDataChange(int id);
    std::vector<ModelData*> getModelInstance();
    void shownextguide();
    void closeguide();
    void showGuide();
signals:

private:
    Ui::MainWindow *ui;
    M3DViewer* mview;
    ModelLoader* loader;
    Slice *sliceresult;
    ModelData *smd;
    RightPanel* rightPanel;
    NRightPanel* nrightPanel;
    QString settingpath;
    std::vector<ModelData*> md_list;
    PreViewDialog* pd;
    MDialog *finishdialog;
    updateThread *mThread;
    int hour, minute;
    bool adding;
    void updateActionSave();
    void SyncData();
    void loadFile(QString filename);    
    void initUI();
    void initChangeUI();
    Changeview *changeview;    
protected:
};

#endif // MAINWINDOW_H
