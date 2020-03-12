#ifndef MODELDATA_H
#define MODELDATA_H
#include <modelloader.h>
#include <QObject>
#include <vector>
#include <QVector3D>
#include <QVector2D>
#include <msupport.h>
#include <slice.h>
#include <qtconcurrentrun.h>
#include <QDebug>
#include <mdialog.h>
struct triangle
{
    QVector3D normal;
    QVector3D vertex[3];
    QVector3D minp;
    QVector3D maxp;
    int id;
};
struct segment;
struct sid;
class MSupport;
class MDialog;
static bool bstri(const triangle &a, const triangle &b)
{
    double minxa = a.vertex[2].x(), minxb = b.vertex[2].x();
    for(int i = 0; i < 2; i++)
    {
        if(minxa < a.vertex[i].x())
        {
            minxa = a.vertex[i].x();
        }
    }
    for(int i = 0; i < 2; i++)
    {
        if(minxb < b.vertex[i].x())
        {
            minxb = b.vertex[i].x();
        }
    }
    return minxa < minxb;
//    return a.vertex[2].x() < b.vertex[2].x();
}

static bool bssy(const triangle &a, const triangle &b)
{
    return a.maxp.y() < b.maxp.y();
}
static bool bssx(const triangle &a, const triangle &b)
{
    return a.maxp.x() < b.maxp.x();
}
class ModelData : public QObject
{
    Q_OBJECT
public:
    QString filename;
    bool hasBase;
    std::vector<triangle> baselist;
    ModelData(std::vector<triangle> trilist, QString filename);
    ModelData(QString filename);
    ~ModelData();
    void load(bool &error);
    void dealBIN(bool &error);
    void dealASCII(bool &error);
    void FromModel();
    QVector3D getScale();
    void setScale(QVector3D scale);
    QVector3D getOffset();
    QVector3D getmoffset();
    void setOffset(QVector3D scale);
    QVector3D getRotation();
    void setRotation(QVector3D rotation);
    QVector3D getPosition();
    QString getFilename();
    QVector3D getSize();
    void addSupport(MSupport msup);
    void setPosition(QVector3D position);
    std::vector<unsigned int> normDispLists;
    std::vector<unsigned int> supportLists;
    std::vector<unsigned int> basenormDispLists;
    std::vector<MSupport> supportlist;
    std::vector<triangle> getZTri(float zdistance);
    void getStri(float xangle, std::vector<triangle> &trilist, float supportlen, QVector3D &maxp, QVector3D &minp, MDialog *mpd);
    bool PointinTri(triangle tri, QVector3D p);
    float sign(QVector3D p1, QVector3D p2, QVector3D p3);
    void getZonXY(triangle tri, float x, float y, float &z);
    QVector3D getNormal(triangle tri);
    std::vector<triangle> getOutputList();
    void PreGenerateLoop(float thickness);
    void getSegments(std::vector< std::vector<segment> > &segments, int id);
    QVector3D mmax; //模型最高点
    QVector3D mmin; // 模型最低点
    QVector3D origonSize;
    QVector3D mbasesize;
    void updateOutput();
    void getTri(triangle* &result, int index, bool &succed);
    void getRotateTri(triangle* &result);
    void resetAll();
    void outputmodel(std::vector<triangle> &modeloutputlist);
    bool supportlastcheck(float x, float y, float z);
    std::vector<triangle> getTrilist();
    int getOutputListSize();
    bool hasuplift; //记录模型自动支撑z轴被抬起
    void correctTri();
    std::vector<triangle> getTriByID(int id);
public slots:
    bool getNextTri(triangle &tri, float zdistance, int &nextid);
    void initGetNexy();
    int getZTriCount(float zdistance);
    void rotatPoint(QVector3D &vec, double angledeg, QVector3D axis);
    void getHead(QVector3D botpoint, QVector3D &fhead, QVector3D &shead, QVector3D &thead, QVector3D &fdist, QVector3D &sdist, QVector3D &tdist);
    void generateBase();
    void setHasBase(bool hasBase);
signals:
    void updateProgress(int);
    void loadfileerror();
private:
    std::vector<triangle> modellist;
    std::vector<triangle> outputlist;
    std::vector<triangle> resultlist;
    std::vector< QFuture<void> > slicethreads;
    std::vector<std::vector<segment> > preslicedata;
    QVector3D rotation;
    QVector3D scale;
    QVector3D offset;
    QVector3D moffset;
    QVector3D position;
    int nextid;
    bool aborded;
    bool abordedfinish;
    int totallayer;
    float thickness;
    QVector2D pixscale;
    void CenterModel();
    bool FromModelList();
    bool FromBaseList();
    bool FromSupportList();
    void generateloop();
    int GetGLError();
    bool CorrectPointOrder(segment &sg);
    int PointLineCompare(QVector2D pointm, QVector2D dir, QVector2D quarrypoint);
    double distance2D(QVector2D point1, QVector2D point2);
    void getSegmentAroundX(std::vector<segment> normallist,std::vector<sid> &outlist, double x);
};
#endif // MODELDATA_H
