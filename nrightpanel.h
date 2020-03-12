#ifndef NRIGHTPANEL_H
#define NRIGHTPANEL_H

#include <QWidget>
#include <QDialog>
#include <m3dviewer.h>
#include <qtconcurrentrun.h>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QCheckBox>
#include <QSettings>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QSignalMapper>
#include <QtGui/QComboBox>
#include <mainwindow.h>
#include <rightlabel.h>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QRect>
#include <QDir>
#include <QScrollArea>
#include <QFont>
#include <msbar.h>
#include <mdialog.h>

class MDialog;
class MainWindow;
class ModelData;
class NRightPanel : public QWidget
{
    Q_OBJECT
public:
    NRightPanel(MainWindow *mparent, QWidget *parent);
    ~NRightPanel();
    void SyncSupportData();
public slots:
    void OnModelSelected(int id);
    void OnMachineChange(bool isoff);
    void OnSliceChange(bool isoff);
    void OnSupportChange(bool isoff);
    void OnModeChange();
    void OnBtnPress();
    void OnBtnRelease();
    void OnBtnClick();
    void toggleBase();
    void anifinished();
    void judgeText();
    void SliceTypeChange(int st);
    void SupportTypeChange(int st);
    void SupportShapeChange(int st);
    void UpdateLanguage();
    void rateSelect();
    void sizeSelect();
    void ShowOtherDialog();
    void UpdateData();
    void saveCombobox();
    void saveLineEdit();
    void ShowDelAllSupport();
    void closedialog();
    void closedelall();
signals:
    void ChangeMode(QString mode);
    void ChangeType(QString suptype);
    void ChangeShape(QString shape);
    void DelAllSupport();
private:
    MainWindow* mparent;
    QIntValidator *intvalidator;
    QDoubleValidator *floatvalidator;
    int selectid;
    QVBoxLayout *mainlayout;
    ModelData *smd;
    QHBoxLayout *machinelayout, *supporttypelayout, *supportshapelayout, *slicetypelayout, *supportratelayout
    , *supportsizelayout, *supportuplayout, *basesizelayout;
    QGridLayout *machineglayout, *sliceglayout, *supportglayout;
    QWidget *mainwidget, *machinewidget, *slicewidget, *supportwidget;
    QScrollArea *scpanel;
    QLineEdit *xresolutionedit, *yresolutionedit, *xlenedit, *ylenedit, *zlenedit
    , *thicknessedit, *exposeedit, *offedit, *botexposeedit, *botcountedit, *supportupedit, *basesizeedit;
    QLineEdit *botliftdisedit,*botliftspeededit,*liftdisedit,*liftspeededit,*returnspeededit,*botbrightnessPVMedit,*brightnessPVMedit;
    QLabel *botliftdislabel,*botliftspeedlabel,*liftdislabel,*liftspeedlabel,*returnspeedlabel,*botbrightnessPVMlabel,*brightnessPVMlabel;
    QLabel *machinesetting, *xresolutionlabel, *yresolutionlabel, *xlenlabel, *ylenlabel, *zlenlabel
    , *thicknesslabel, *exposelabel, *offlabel, *botexposelabel, *botcountlabel
    , *supporttypelabel, *supportshapelabel, *slicetypelabel, *supportratelabel, *supportsizelabel, *supportuplabel, *basesizelabel;
    RightLabel *machinelabel, *slicelabel, *supportlabel;
    MsBar *b1,*b2,*b3;
    QCheckBox *keepxyresolution;
    QPushButton *addSupport, *delSupport, *autoSupport, *supportrateL, *supportrateM, *supportrateH, *supportsizeS, *supportsizeM, *supportsizeL
    , *addbase, *editSupport, *otherSettings, *delAllSupport;
    QComboBox *supporttype, *supportshape, *slicetype;
    QLabel *toptext,*midtext,*bottext,*toptype,*touchtype,*touchtypesize,*toplength,*topwidth,*midtype,*midlength,*bottomtype,*botdepth,*botwidth,*botconesize,*botballsize;
    QComboBox *toptypecb,*touchtypecb,*midtypecb,*bottomtypecb;
    int machinewidgetheight, supportwidgetheight, slicewidgetheight;
    QPropertyAnimation *animation;
    QParallelAnimationGroup *anigroup;
    QPropertyAnimation *machineanimation;
    QPropertyAnimation *sliceanimation;
    QPropertyAnimation *supportanimation;
    QParallelAnimationGroup *machineanigroup;
    QParallelAnimationGroup *sliceanigroup;
    QParallelAnimationGroup *supportanigroup;
    QFont regular,medium;
    QString str_addsup, str_delsup, str_autosup, str_finish, str_toast, str_add, str_remove, str_editsup,
    str_otherset,str_sup_top,str_sup_mid,str_sup_bot,str_toptype,str_cone,str_cube,str_touchtype,str_sphere,
    str_none,str_toplen,str_midtype,str_midwidth,str_bottype,str_botdepth,str_touchtypesize,str_topwidth,
    str_botwidth,str_botconesize,str_botballsize,str_delallsupport,str_errorzero;
    QString str_dialogcomfirm, str_cancel, str_comfirm;
    MDialog *otherDialog;
    bool isanifinish, settexting;
    bool machineanifinish,sliceanifinish,supportanifinish;
    bool clickmachine,clickslice,clicksupport;
    MDialog *delallsupportdialog;

    void initUI();
    void initData();
    void getData(QString datakey, QString &data, QString defaultvalue);
    void dealData(QString datakey, QString data);
    void getPP(QWidget *w, int &posx, int &poxy, int &width, int &height);
};

#endif // NRIGHTPANEL_H
