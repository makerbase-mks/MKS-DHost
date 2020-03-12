#ifndef MSUPPORT_H
#define MSUPPORT_H
#include <modelloader.h>
#include <QVector3D>
#include <mtools.h>

struct triangle;
struct segment;
struct sid;
class MSupport
{
public:
    MSupport(int fid,QString supporttype, QVector3D toppoint);
    MSupport();
    ~MSupport();
    int botfaceid;
    void setNormal(QVector3D normal);
    void setBotPos(QVector3D botpoint, int botfaceid);
    void setTri(triangle* toptri, triangle* bottri);
    void setOthers(int fid, QString supporttype, QVector3D toppoint);
    void setOP(QVector3D offset, QVector3D pos);
    void setWaylen(double waylen);
    void setScaled(QVector3D mscaled);
    void setShape(QString shape);
    void setMidSupportShape(QString shape);
    void setMidWidth(float width);
    void setTopSize(QString ttype,QString ttouchtype,float len,float ttouchsize,float wid);
    void setBotSupportShape(QString shape,float wid,float conesize,float ballsize);
    void setASize(QVector3D botsize, QVector3D midsize);
    void setNeedBot(bool needbot);
    void addfinish();
    void supMove(QVector3D toppoint, QVector3D botpoint);
    void getTriid(int &fid, int &bid);
    void setTopPos(QVector3D toppoint, int fid);
    void updatelist(QVector3D mscaled, QVector3D moffset, QVector3D rotation);
    void getZTri(std::vector<triangle> &tl, double att);
    void getSegments(std::vector<segment> &segments, double att);
    QVector3D getBotpoint();
    QVector3D getToppoint();
    std::vector<unsigned int> normDispLists;
    void outputmodel(std::vector<triangle> &modeloutputlist);
public slots:
    bool getNextTri(triangle &tri, float zdistance, int &nextid);
    void initGetNexy();
    int getZTriCount(float zdistance);
    void setTurnpoint(QVector3D trunpoint);
    void setThreePoint(QVector3D fhead, QVector3D shead, QVector3D thead, QVector3D fdist, QVector3D sdist, QVector3D tdist);
    void getSquare(std::vector<triangle> &mlist, QVector3D &mmx);
    void getCube(std::vector<triangle> &mlist, QVector3D &mmx);
    void getCylinder(std::vector<triangle> &mlist, QVector3D &mmx);
    void setCursorPoint(QVector3D cursorpoint);
    void updateHead(QString headname, QVector3D headpos, QVector3D headdist);
    QVector3D getMPos();
    QVector3D getHead(QString headname);
    QString getPickPart(QVector3D cursorpoint);
    QVector3D getCursorPoint();
    void getData(QString key, QString &data, QString defaultvalue);

private:
    QVector3D toppoint;
    int faceid;
    QString supporttype;
    QString supportshape;
    QString midsupportshape,botsupportshape,toptype,touchtype;
    double midwidth,toplen,touchtypesize,topwidth,botwidth,botconesize,botballsize;
    QVector3D normal;
    QVector3D botpoint;
    QVector3D turnpoint;
    QVector3D fhead, shead, thead, fdist, sdist, tdist;
    double waylen;
    int nextid;
    QVector3D moffset, mpos, botsize, middsize,nscaled, nrotation, origonscale, topsize;
    triangle* toptri;
    triangle* bottri;
    triangle* origontop,*origonbot;
    QVector3D origontoppoint, origonbotpoint, cursorpoint;
    bool needbot;
    bool haveTurn;
    std::vector<triangle> trilist;
    std::vector<triangle> squarelist;
    std::vector<triangle> outputlist;
    bool FormSupportList();
    void setupList();
    void getTopList(std::vector<triangle> &mlist, triangle nearpoint, triangle farpoint);
    void getConeTop(std::vector<triangle> &mlist, QVector3D &mmx);
    void getBallConnect(std::vector<triangle> &mlist, QVector3D &mmx);
    void getSupportShape(std::vector<triangle> &mlist, QVector3D &mmx);
    void ROStrilist(std::vector<triangle> &mlist, QVector3D rotate, QVector3D scale, QVector3D position);
    bool CorrectPointOrder(segment &sg);
    double distance2D(QVector2D point1, QVector2D point2);
    void getSegmentAroundX(std::vector<segment> normallist,std::vector<sid> &outlist, double x);
    int PointLineCompare(QVector2D pointm, QVector2D dir, QVector2D quarrypoint);
    void updateOutPutlist();
    void rotatPoint(QVector3D &vec, double angledeg, QVector3D axis);
};

#endif // MSUPPORT_H
