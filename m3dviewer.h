#ifndef M3DVIEWER_H
#define M3DVIEWER_H

#include <QGLWidget>
#include <QVector3D>
#include <QVector2D>
#include <mainwindow.h>
#include <modelloader.h>
#include <msupport.h>
#include <QLabel>
#include <QtGui/QHBoxLayout>
#include <iconlabel.h>
#include <msbar.h>
#include <numberedit.h>
#include <QWidget>
#include <QTimer>
#include <QDataStream>
#include <mprogressbar.h>
#include <updatethread.h>
#include <mbutton.h>
#include <supportthread.h>
#include <dlplog.h>
struct operaData{
    QVector3D rotation;
    QVector3D scale;
    QVector3D position;
    int id;
};

struct triangle;
class MainWindow;
class ModelData;
class MSupport;
class MProgressBar;
class updateThread;
class mButton;
class supportThread;
class DLPlog;
class M3DViewer : public QGLWidget
{
    Q_OBJECT

public:
    M3DViewer(MainWindow* mparent,QWidget *parent);
    ~M3DViewer();
    QTimer* pDrawTimer; //refreshes the 3d scene
    bool shiftdown; //public so that the mainwindow can alter these values easy.
    bool controldown;
    bool CancelAdd;
    bool ViewChangeing;
    void saveOperaData(int id, QVector3D  mscale, QVector3D mrotation, QVector3D mpos);
public slots:
    void UpdateTick();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void setXRotation(float angle);
    void setZRotation(float angle);
    void updateMSize();
    void setMod(QString toolmod);
    void setSType(QString stype);
    void setShape(QString shape);
    void OnFileOpen();
    void OnFileSave();
    void OnStlSave();
    void togglepanel();
    void viewChange();
    void OnBtnPress();
    void OnBtnRelease();
    void OnAnimFinish();
    void OnRotateChange(QString result);
    void OnPositionChange(QString result);
    void OnSizeChange(QString result);
    void OnResetData();
    void OnLanguageChange();
    void showToast(QString text, int tm);
    void UpdateLanguage();
    void getScreenShot();
    void outputScreenShot(QDataStream &out);
    void hideWidget();
    void CancelAdding();
    void updateProgress(int progress);
    void updateModel(ModelData*);
    void deleteModel();
    void copyModel();
    void getPlatformPos(QPoint mousePos);
    void autoSupportFinish();
    void autoaddSupport(QVector3D mpos, int id);
    void updateSupport(float x, float y);
    void undoModel();
    void redoModel();
    void delAllSupport();
    void loadfileerrors();
signals:
    void OnModelSelected(int id);
    void OnTriSelected(int id);
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void resetCamera(bool xraygon); //改变视角
    void drawFloor(); // 画平台
    void drawGUI();  //画左边绿色的背景
    void drawModelInstance();
    void drawSupport();
    void update3DPos(float x, float y);

private:
    int selectid;
    int supportfocusid;
    int focusid;
    float xRot;
    float yRot;
    float zRot;
    float xRotTarget;
    float yRotTarget;
    float zRotTarget;
    QString currViewAngle;
    float camdist;
    float camdistTarget;
    MainWindow* mparent;
    bool getObj;
    ModelData *smd;
    MProgressBar *mpb;
    bool supportmod;
    bool autosize;
    int triid; //  三角面 id
    triangle* tri;
    int bottriid; //  相对底部的三角面ID
    triangle* bottri;
    double waylen;
    double dscale;

    QVector3D pan;
    QVector3D panTarget;
    QVector3D revolvePoint;
    QVector3D revolvePointTarget;

    float deltaTime;//frame rate and delta time vars
    float lastFrameTime;

    bool perspective;
    bool hideNonActiveSupports;

    QVector3D cursorPos3D;//true global position of cursor
    QVector3D cursorNormal3D;//
    QVector2D cursorPosOnTrackCanvas;//similar to pixel cords, but in real units in world.
    QVector3D cursorPreDragPos3D;
    QVector3D cursorPostDragPos3D;//TODO not yet implemented
    QVector3D PreDragInstanceOffset;
    QVector3D PreDragRotationOffsetData;//not really a cordinate just info.

    //tools/keys
    QString currtool;
    QString supporttype;
    QString supportshape;
    QString midsupportshape; //支撑中部形状
    QVector3D supportpoint;
    QVector3D botpoint, fhead, shead, thead, turnpoint, fdist, sdist, tdist;
    QVector3D bottsize,middsize,topsize;
    bool pandown;
    bool dragdown;
    bool haveturn;
    QString picksupportpart;

    //visual build size only - use project settings as actual size
    float buildsizex;
    float buildsizey;
    float buildsizez;

    //visual fence stuff
    bool fencesOn[5];
    float fenceAlpha;
    float supportAlpha;
    bool updatingModel;

    //mouse cords
    QVector2D mouseDeltaPos;
    QPoint mouseDownInitialPos;
    QPoint mouseLastPos;
    void getMouseRay(int x, int y, QVector3D &p0, QVector3D &p1);
    void getData(QString datakey, QString &data, QString defaultvalue);
    void getMouseHoverTri(int x, int y);
    void getMinZP(QVector3D mtri[3], QVector3D &minpoint);
    void getMinMaxP(QVector3D mtri[3], QVector3D &mminp, QVector3D &mmaxp);
    void getFreePoint(QVector3D &mfreepoint);
    void rePaintModel();
    void rePaintSupport(int x, int y);
    void getSupportPart(int x, int y);
    void appendSupport(bool needrotate = true);
    void deleteSupport();
    void autoSupport();
    void getUnderPoint(QVector3D mpoint, QVector3D &underpoint);
    void getBottomPoint();
    void getTriCenter();
    QVector3D getPointTri(triangle* bottri);
    QVector3D getPointOnTri(triangle* bottri, int x, int y);
    //gui part
    QLabel *tt;
    MsBar *mmb;
    IconLabel *openfile, *saveas, *selectlanguage, *selectview, *rotatemodel, *scaledmodel, *positionmodel, *slicemodel
    , *topview, *bottomview, *frontview;
    QHBoxLayout *mainlayout;
    QWidget *viewpanel, *rotatepanel, *scaledpanel, *positionpanel, *languagepanel;
    NumberEdit *px, *py, *pz, *rx, *ry, *rz, *sx, *sy, *sz;
    QPushButton *preset, *rreset, *sreset, *language_cn, *language_en;
    QCheckBox *keepxyz;
    QTimer *tm;
    bool takeScreenShot;
    QImage screenshot;
    bool updatingtext;
    QVector3D mscale, mposition, mrotation;
    bool animfinish;
    QVector3D screenpos, mdmaxpos, mdminpos;
    QString string_selectmodel, string_finishopera, str_wait, loadfileerror;
    updateThread *mThread;
    supportThread *supThread;
    mButton *delbutton, *copybutton;
    QVector3D mousePlatformPos;
    void initIconLabel(IconLabel *&widget, QString url, QString text, int lpos);
    void initPanelData();
    std::vector<operaData *> operalist,redooperalist;
    bool saveopera;
    int currentoperaindex;
    int originlen;
//    bool hasuplift;
    void glDrawString(unsigned char *str);
};

#endif // 3DVIEWER_H
