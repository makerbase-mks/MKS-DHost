#include <slice.h>
#include <QThread>
#include <QPixmap>
#include <QPainter>
#include <QFile>
#include <string>
#include <QtGui/QApplication>
#include <hash_map>
#include <QMap>
Slice::Slice(MainWindow* mparent)
{
    this->mparent = mparent;
    isComplete = false;
    isOutputing = false;
    slicetype = 0;
    initSliceThread();
}
Slice::~Slice()
{
    slicethreads.clear();
}
void Slice::setResolution(QVector2D rs)
{
    this->resolution = rs;
    this->baseimg = QImage(resolution.x(), resolution.y(), QImage::Format_RGB32);
}

void Slice::setPlatform(QVector2D pf)
{
    this->platform = pf;
    pixscale.setX(platform.x()/resolution.x());
    pixscale.setY(platform.y()/resolution.y());
}

void Slice::setSliceType(int slicetype)
{
    this->slicetype = slicetype;
}

void Slice::initSliceThread()
{
//    slicethreads.clear();
    if(slicethreads.size() == 0)
    {
        int tn = 6;
        tn = QThread::idealThreadCount();
        if(tn == -1)
        {
            tn = 2;
        }
//        tn = 1; //调试使用
        if(tn > 2)
        {
            tn = tn-2;
        }
        for(int i = 0; i < tn; i++)
        {
            slicethreads.push_back(QFuture<void>());
        }
    }
}

void Slice::initSliceThreadCount(int count)
{
//    slicethreads.clear();
    while (slicethreads.size()>0) {
        slicethreads.erase(slicethreads.begin());
    }
    if(slicethreads.size() == 0)
    {
        int tn = 6;
        tn = QThread::idealThreadCount();
        if(tn == -1)
        {
            tn = 2;
        }
//        tn = 1; //调试使用
        tn = count;
        if(tn > 2)
        {
            tn = tn-2;
        }
        for(int i = 0; i < tn; i++)
        {
            slicethreads.push_back(QFuture<void>());
        }
    }
//     qDebug() << "slice---------" << slicethreads.size();
}

void Slice::addLayer(unsigned int id, std::vector<triangle> tl, double att)
{
    ld.id = id;
    ld.tl = tl;
    ld.att = att;
    layerlist.push(ld);
}
void Slice::addH(unsigned int id, double att)
{
    ld.id = id;
    ld.att = att;
    layerlist.push(ld);
}

void Slice::removeH()
{
   while(layerlist.size()>0){
       layerlist.pop();
   }
}

void Slice::setMaxSize(int max_size)
{
    while(resultlayer.size()>0)
    {
        resultlayer.erase(resultlayer.begin());
    }
    this->max_size = max_size;
    finishlayer.reserve(max_size);
    resultlayer.reserve(max_size);
    layerlooplist lll;
    layerresult lr;
    QImage img;
    for(int i = 0; i < max_size; i++)
    {
//        finishlayer.push_back(lll);
        resultlayer.push_back(lr);
    }
    mdlist = mparent->getModelInstance();
}
void Slice::CancelSlicing()
{
    cancelslicing = true;
}

void Slice::checkAllThread(bool &isfinish)
{
    isfinish = true;
    for(int i = 0; i < slicethreads.size(); i++)
    {
        if(slicethreads[i].isRunning())
        {
            isfinish = false;
            break;
        }
    }
}

void Slice::startslice(bool &slicecomplete, int &precent, bool fastmod)
{
//    qDebug() << "layerlist.size()---------" << layerlist.size() << slicethreads.size();
//    qDebug() << "slicethreads.size()---------" << slicethreads.size();
    if(layerlist.size() > 0)
    {
        for(int i = 0; i < slicethreads.size(); i++)
        {
            if(slicethreads[i].isRunning())
            {
                if(this->mparent->isCancel){
                    qDebug() << "this->mparent->isCancel---------" << i;
                    slicethreads[i].cancel();
                    while(layerlist.size() > 0){
                        layerlist.pop();
                    }
                }
//                qDebug() << "slicethreads[i].isRunning()---------" << i;
                continue;
            }
            if(layerlist.size() <= 0)
            {
                break;
            }
            layerdata ld;
            ld = layerlist.front();
            layerlist.pop();
//            qDebug() << "resolution.x()--1----" << resolution.x() << resolution.y();
            QImage floorimg = QImage(resolution.x(), resolution.y(), QImage::Format_RGB32);
//            qDebug() << "floorimg--1----" << floorimg.width() << floorimg.height();
//            int count = 0;
//            while (floorimg.width() == 0) {
//                floorimg = QImage(resolution.x(), resolution.y(), QImage::Format_RGB32);
//                count ++;
//                if (count > 30) {
//                    mparent->isCancel = true;
//                    mparent->showToast("Slice error,please try again.",5);
//                    return;
//                }
//            }
//            qDebug() << "floorimg--11----" << floorimg.width() << floorimg.height();
            if(!fastmod)
            {
                //老版本
                slicethreads[i] = QtConcurrent::run(this, &Slice::generateByH, ld.id, ld.att, floorimg);
            }else{
                //新版本
//                slicethreads[i] = QtConcurrent::run(this, &Slice::generateByPreSlice, ld.id, ld.att, floorimg);
                slicethreads[i] = QtConcurrent::run(this, &Slice::generateByPreSliceCopy, ld.id, ld.att, floorimg);
            }
            //20200220更改切片模式
//            slicethreads[i] = QtConcurrent::run(this, &Slice::newGenerateByPreSlice, ld.id, ld.att, floorimg);
        }
    }else{
        isComplete = true;
        for(int i = 0; i < slicethreads.size(); i++)
        {
            if(slicethreads[i].isRunning())
            {
                isComplete = false;
                break;
            }
        }
        if(isComplete)
        {
//            std::sort(resultlayer.begin(), resultlayer.end(), layerresultcompare);
//            mparent->UpdatePreViewDialog(resolution, resultlayer);
            mparent->UpdatePreViewDialogCopy(resolution,max_size);
            slicecomplete = true;
        }
    }
    int runingthread = 0;
    for(int i = 0; i < slicethreads.size(); i++)
    {
        if(slicethreads[i].isRunning())
        {
            runingthread += 1;
        }
    }    
    precent = 100 - (layerlist.size()+runingthread)*100/max_size;
//    qDebug() << "runingthread---------" << layerlist.size() << runingthread << max_size << precent;
}

void Slice::startsliceSave(bool &slicecomplete, int &precent, bool newSliceType)
{
//    qDebug() << "slice.size()---------" << slicethreads.size();
    if(layerlist.size() > 0)
    {
        for(int i = 0; i < slicethreads.size(); i++)
        {
            if(slicethreads[i].isRunning())
            {
                if(this->mparent->isCancel){
                    slicethreads[i].cancel();
                    while(layerlist.size() > 0){
                        layerlist.pop();
                    }
                }
                continue;
            }
            if(layerlist.size() <= 0)
            {
                break;
            }
            layerdata ld;
            ld = layerlist.front();
            layerlist.pop();
//            QImage floorimg = QImage(resolution.x(), resolution.y(), QImage::Format_RGB32);
            QImage floorimg = this->baseimg;
//            qDebug() << "floorimg--1----" << floorimg.width() << floorimg.height();
//            while (floorimg.width() == 0) {
//                floorimg = QImage(resolution.x(), resolution.y(), QImage::Format_RGB32);
//            }
//            qDebug() << "floorimg--11----" << floorimg.width() << floorimg.height();
            //20200220更改切片模式
            if (newSliceType) {
               slicethreads[i] = QtConcurrent::run(this, &Slice::newGenerateByPreSlice, ld.id, ld.att, floorimg);
            }else {
               slicethreads[i] = QtConcurrent::run(this, &Slice::newGenerateByPreSlicecopy, ld.id, ld.att, floorimg);
            }

            if (!iswriting && resultlayer.size() > 0 ) {
                if (newSliceType) {
                    writeFile();
                }else{
                    writeFilecopy();
                }

            }
        }
    }else{
        isComplete = true;
        for(int i = 0; i < slicethreads.size(); i++)
        {
            if(slicethreads[i].isRunning())
            {
                isComplete = false;
                break;
            }
        }
        if(isComplete)
        {
//            std::sort(resultlayer.begin(), resultlayer.end(), layerresultcompare);
//            mparent->UpdatePreViewDialog(resolution, resultlayer);
//            mparent->UpdatePreViewDialogCopy(resolution,max_size);
            slicecomplete = true;
            if (!iswriting && resultlayer.size() > 0 ) {
                if (newSliceType) {
                    writeFile();
                }else{
                    writeFilecopy();
                }
            }
        }
    }
    int runingthread = 0;
    for(int i = 0; i < slicethreads.size(); i++)
    {
        if(slicethreads[i].isRunning())
        {
            runingthread += 1;
        }
    }
    precent = 100 - (layerlist.size()+runingthread)*100/max_size;
}


void Slice::checkslice(bool &slicecomplete, int &precent)
{
    if(layerlist.size() > 0)
    {
        for(int i = 0; i < slicethreads.size(); i++)
        {
            if(slicethreads[i].isRunning())
            {
                continue;
            }
            if(layerlist.size() <= 0)
            {
                break;
            }
            layerdata ld;
            ld = layerlist.front();
            layerlist.pop();
            slicethreads[i] = QtConcurrent::run(this, &Slice::generateLoop, ld.id, ld.tl, ld.att);
        }
        precent = 50 - layerlist.size()*50/max_size;
    }
    else{
        if(!isOutputing)
        {
            for(int i = 0; i < slicethreads.size(); i++)
            {
                if(slicethreads[i].isRunning())
                {
                    return;
                }
            }
            isOutputing = true;
            std::sort(finishlayer.begin(), finishlayer.end(), layerloopcompare);
        }
        if(finishlayer.size() > 0)
        {
//            if(!isOutputing){
//                isOutputing = true;
//                std::sort(finishlayer.begin(), finishlayer.end(), layerloopcompare);
//            }
            for(int i = 0; i < slicethreads.size(); i++)
            {
                if(slicethreads[i].isRunning())
                {
                    continue;
                }
                if(finishlayer.size() <= 0)
                {
                    break;
                }
                layerlooplist lll = finishlayer.back();
                finishlayer.pop_back();
                QImage floorimg = QImage(resolution.x(), resolution.y(), QImage::Format_RGB32);
                slicethreads[i] = QtConcurrent::run(this, &Slice::generateData, lll.layerid, lll.layerloopdata, floorimg);
            }
        }
        else{
            isComplete = true;
            for(int i = 0; i < slicethreads.size(); i++)
            {
                if(slicethreads[i].isRunning())
                {
                    isComplete = false;
                    break;
                }
            }
        }
        precent = 100 - finishlayer.size()*50/max_size;
        if(isComplete){
            std::sort(resultlayer.begin(), resultlayer.end(), layerresultcompare);
            slicecomplete = true;
        }
    }
}

void Slice::initCWS(QString path,bool &error)
{
    error = false;
    while (filelist.size() > 0) {
        filelist.erase(filelist.begin());
    }
    isSaveZip = true;
    nowimgid = 1;
    max_size = 0;
    QFileInfo info(path);
    QuaZip mzip(path);
    bool isopen = mzip.open(QuaZip::mdUnzip);
//    qDebug() << "mzip.open(QuaZip::mdUnzip)" << isopen;
    if (!isopen) {
        error = true;
       return;
    }
    mzip.setCurrentFile(info.baseName()+".gcode");
    QuaZipFile mqafr(&mzip);

     bool isopenmqafr = mqafr.open(QIODevice::ReadOnly);
//    qDebug() << "mqafr.open(QIODevice::ReadOnly)" << isopenmqafr;
    if (!isopenmqafr) {
        error = true;
       return;
    }
    QString line = "";
    float permmx, permmy;
    int et, ol, be, bc;
    QRegExp rx("(\\d+\\.?\\d*)");
    while(!mqafr.atEnd())
    {
//        qDebug() << line;
        line = mqafr.readLine();
        rx.indexIn(line);
        if(line.startsWith(";(Pix per mm X"))
        {
            permmx = rx.cap(0).toFloat();
        }else if(line.startsWith(";(Pix per mm Y"))
        {
            permmy = rx.cap(0).toFloat();
        }else if(line.startsWith(";(X Resolution"))
        {
            resolution.setX((int)rx.cap(0).toFloat());
        }else if(line.startsWith(";(Y Resolution"))
        {
            resolution.setY((int)rx.cap(0).toFloat());
        }else if(line.startsWith(";(Layer Thickness"))
        {
            thickness = rx.cap(0).toFloat();
        }else if(line.startsWith(";(Layer Time"))
        {
            et = rx.cap(0).toFloat()/1000;
        }else if(line.startsWith(";(Bottom Layers Time"))
        {
            be = rx.cap(0).toFloat()/1000;
        }else if(line.startsWith(";Number of Slices"))
        {
            max_size = rx.cap(0).toInt();
        }else if(line.startsWith(";(Blanking Layer Time"))
        {
            ol = (int)rx.cap(0).toFloat()/1000;
        }else if(line.startsWith(";(Number of Bottom Layers"))
        {
            bc = (int)rx.cap(0).toInt();
        }else if(line.startsWith(";********** Header Start ********"))
        {
            break;
        }
    }
    platform.setX((float)resolution.x()/permmx);
    platform.setY((float)resolution.y()/permmy);
    setMaxSize(max_size);
    setPlatform(platform);
    mparent->UpdatePreViewDetail(thickness, et, ol, be, bc);
    mqafr.close();
    mzip.close();
    QuaZip mxmlzip(path);
    mxmlzip.open(QuaZip::mdUnzip);
    mxmlzip.setCurrentFile("manifest.xml");
    QuaZipFile mxmlzipfile(&mxmlzip);
    mxmlzipfile.open(QIODevice::ReadOnly);
    QXmlStreamReader xmlreader;
    xmlreader.addData(mxmlzipfile.readAll());
    mxmlzipfile.close();
    mxmlzip.close();
    while(!xmlreader.atEnd())
    {
//        qDebug() << xmlreader.name() << xmlreader.text();
        if(xmlreader.name() == "name")
        {
            xmlreader.readNext();
            if(xmlreader.text().toString().endsWith("png"))
            {
                filelist.push_back(xmlreader.text().toString());
//                qDebug() << filelist[filelist.size()-1];
            }
            xmlreader.readNext();
        }
        xmlreader.readNext();
    }

}

void Slice::initZip(QString path,bool &error)
{
    isSaveZip = true;
    nowimgid = 0;
    max_size = 0;
    QuaZip mzip(path);
    bool isopen = mzip.open(QuaZip::mdUnzip);
//    qDebug() << "initZip mzip.open(QuaZip::mdUnzip)" << isopen;
    if (!isopen) {
       error = true;
       return;
    }
    mzip.setCurrentFile("run.gcode");
    QuaZipFile mzipfile(&mzip);
    if(mzipfile.open(QIODevice::ReadOnly))
    {
        QString line = mzipfile.readLine();
        int et, ol, be, bc;
        while(!line.isEmpty())
        {
//            if(line.isEmpty() || !line.startsWith(";"))
//            {
//                break;
//            }
            if(line.startsWith(";LAYER_START:"))
            {
                max_size = (int)line.mid(line.lastIndexOf(":")+1).toFloat();
//                max_size = (int)line.mid(line.lastIndexOf(":")+1).toFloat();
            }else if(line.startsWith(";resolutionX:"))
            {
                resolution.setX((int)line.mid(line.lastIndexOf(":")+1).toFloat());
            }else if(line.startsWith(";resolutionY:"))
            {
                resolution.setY((int)line.mid(line.lastIndexOf(":")+1).toFloat());
            }else if(line.startsWith(";machineX:"))
            {
                platform.setX((float)line.mid(line.lastIndexOf(":")+1).toFloat());
            }else if(line.startsWith(";machineY:"))
            {
                platform.setY((float)line.mid(line.lastIndexOf(":")+1).toFloat());
            }else if(line.startsWith(";layerHeight:"))
            {
                thickness = (float)line.mid(line.lastIndexOf(":")+1).toFloat();
            }else if(line.startsWith(";normalExposureTime:"))
            {
                et = (int)line.mid(line.lastIndexOf(":")+1).toFloat();
            }else if(line.startsWith(";lightOffTime:"))
            {
                ol = (int)line.mid(line.lastIndexOf(":")+1).toFloat();
            }else if(line.startsWith(";bottomLayExposureTime:"))
            {
                be = (int)line.mid(line.lastIndexOf(":")+1).toFloat();
            }else if(line.startsWith(";bottomLayCount:"))
            {
                bc = (int)line.mid(line.lastIndexOf(":")+1).toFloat();
            }
            line = mzipfile.readLine();
        }
        max_size += 1;
        setMaxSize(max_size);
//        qDebug() << max_size;
        setPlatform(platform);
        mparent->UpdatePreViewDetail(thickness, et, ol, be, bc);
    }else {
        error = true;
        return;
    }
    mzipfile.close();
    mzip.close();
    QuaZip mimgzip(path);
    mimgzip.open(QuaZip::mdUnzip);
    mimgzip.setCurrentFile("preview.png");
    QuaZipFile mimgzipfile(&mimgzip);
    if(mimgzipfile.open(QIODevice::ReadOnly))
    {
        zippreview.loadFromData(mimgzipfile.readAll());
        int maxw = zippreview.width();
        if(zippreview.height() < maxw)
        {
            maxw = zippreview.height();
        }
        zippreview = zippreview.copy(0, 0, maxw, maxw);
        zippreview = zippreview.scaled(290, 290, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    mimgzipfile.close();
    mimgzip.close();
}

void Slice::loadCWS(QString path, bool &slicecomplete, int &precent)
{
//    qDebug() << "path" << path;
//    qDebug() << "loadCWS";
//    qDebug() << "max_size" << max_size;
//    qDebug() << "nowimgid" << nowimgid;
//    qDebug() << "slicethreads.size()" << slicethreads.size();
    if(nowimgid <= max_size)
    {
        for(int i = 0; i < slicethreads.size(); i++)
        {
//            qDebug() << "try nowimgid" << nowimgid;
            if(slicethreads[i].isRunning())
            {
                continue;
            }
            if(nowimgid <= max_size) {
                QImage floorimg = QImage(resolution.x(), resolution.y(), QImage::Format_RGB32);
                QuaZip mzip(path);
                try {
                    mzip.open(QuaZip::mdUnzip);
                    mzip.setCurrentFile(filelist[nowimgid-1]);
//                    qDebug() << "filelist[nowimgid-1]" << filelist[nowimgid-1];
                    QuaZipFile mzipfile(&mzip);
                    if(mzipfile.open(QIODevice::ReadOnly))
                    {
                        QByteArray imgdata = mzipfile.readAll();
                        floorimg.loadFromData(imgdata);
                        mzipfile.close();
                        if(nowimgid == 1)
                        {
                            zippreview.loadFromData(imgdata);
                            int maxw = zippreview.width();
                            if(zippreview.height() < maxw)
                            {
                                maxw = zippreview.height();
                            }
                            zippreview = zippreview.copy((zippreview.width()-maxw)/2, (zippreview.height()-maxw)/2, maxw, maxw);
                            zippreview = zippreview.scaled(290, 290, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                        }
                        slicethreads[i] = QtConcurrent::run(this, &Slice::generateByImage, nowimgid, floorimg);
                        nowimgid ++;
                    }
                    mzip.close();
                } catch (QString e) {
                    qDebug() << "catch" << e;
                }
            }
        }
    }else{
        isComplete = true;
        for(int i = 0; i < slicethreads.size(); i++)
        {
            if(slicethreads[i].isRunning())
            {
                isComplete = false;
                break;
            }
        }
        qDebug() << "isComplete" << isComplete;
        if(isComplete)
        {
            std::sort(resultlayer.begin(), resultlayer.end(), layerresultcompare);
            mparent->UpdatePreViewDialog(resolution, resultlayer);
            slicecomplete = true;
        }
    }
    int runingthread = 0;
    for(int i = 0; i < slicethreads.size(); i++)
    {
        if(slicethreads[i].isRunning())
        {
            runingthread += 1;
        }
    }
    precent = (nowimgid-runingthread)*100/max_size;
}

void Slice::loadZip(QString path,bool &slicecomplete, int &precent,bool &error)
{
//    qDebug() << "loadZip";
//    qDebug() << "max_size" << max_size;
//    qDebug() << "nowimgid" << nowimgid;
//    qDebug() << "slicethreads.size()" << slicethreads.size();
    if(nowimgid <= max_size)
    {
        for(int i = 0; i < slicethreads.size(); i++)
        {
            if(slicethreads[i].isRunning())
            {
                if(this->mparent->isCancel){
                    slicethreads[i].cancel();
                }
                continue;
            }
            QImage floorimg = QImage(resolution.x(), resolution.y(), QImage::Format_RGB32);
            QuaZip mzip(path);
            try {
                mzip.open(QuaZip::mdUnzip);
                mzip.setCurrentFile(QString::number(nowimgid+1)+".png");
                QuaZipFile mzipfile(&mzip);
                if(mzipfile.open(QIODevice::ReadOnly))
                {
                    QByteArray imgdata = mzipfile.readAll();
                    floorimg.loadFromData(imgdata);
                    mzipfile.close();
                    slicethreads[i] = QtConcurrent::run(this, &Slice::generateByImage, nowimgid, floorimg);
                }
                nowimgid ++;
                mzip.close();
            } catch (QString e) {
                qDebug() << "catch" << e;
            }

        }
    }else{
        isComplete = true;
        for(int i = 0; i < slicethreads.size(); i++)
        {
            if(slicethreads[i].isRunning())
            {
                isComplete = false;
                break;
            }
        }
        qDebug() << "isComplete" << isComplete;
        if(isComplete && resultlayer.size() > 0)
        {
            std::sort(resultlayer.begin(), resultlayer.end(), layerresultcompare);
            mparent->UpdatePreViewDialog(resolution, resultlayer);
            slicecomplete = true;
        }
        if(isComplete && resultlayer.size() < 1) {
            error = true;
        }
    }
    int runingthread = 0;
    for(int i = 0; i < slicethreads.size(); i++)
    {
        if(slicethreads[i].isRunning())
        {
            runingthread += 1;
        }
    }
    precent = (nowimgid-runingthread)*100/max_size;
//    deleteDir(path);
}

void Slice::loadMdlp(QString filename, bool &canread)
{
//    qDebug() << "loadMdlp-------------";
    QFileInfo info(filename);
    while(resultlayer.size()>0)
    {
        resultlayer.erase(resultlayer.begin());
    }
    QFile* pf = new QFile(filename);
    pf->open(QIODevice::ReadOnly);
    QDataStream in(pf);
    quint16 rx, ry, blankint;
    quint8 nextline;
    QString px, py;
    QString readdata;
    char *tag = new char[11];
    char *endtag = new char[12];
    char *tx = new char[15];
    int et, ol, be, bc;
    uint lentag, rng;
    rng = 290*290*2+116*116*2+4;
    char *previewimg = new char[rng];
    in.readBytes(tag, lentag);
    QString tagname;
    std::vector<whitedata> eachlayer;
    tagname = QString(QLatin1String(tag));
    if(tagname != "MKSDLP")
    {
        qDebug() << "tagname != MKSDLP start----------";
        canread = false;
        pf->close();
        delete pf;
        return;
    }
    in.readRawData(previewimg, rng);
    in >> rx;
    max_size = (int)rx;
    in >> rx;
    in >> ry;
    resolution.setX(rx);
    resolution.setY(ry);
    in >> px;
    platform.setX(px.toDouble());
    in >> py;
    platform.setY(py.toDouble());
    in >> readdata;
    thickness = readdata.toDouble();
    in >> blankint;
    et = (int)blankint;
    in >> blankint;
    ol = (int)blankint;
    in >> blankint;
    be = (int)blankint;
    in >> blankint;
    bc = (int)blankint;
    in >> blankint;
//    qDebug() << "in1=" << (int)blankint;
    in >> blankint;
//    qDebug() << "in2=" << (int)blankint;
    in >> blankint;
//    qDebug() << "in3=" << (int)blankint;
    in >> blankint;
//    qDebug() << "in4=" << (int)blankint;
    in >> blankint;
//    qDebug() << "in5=" << (int)blankint;
    in >> blankint;
//    qDebug() << "in6=" << (int)blankint;
    in >> blankint;
//    qDebug() << "in7=" << (int)blankint;
//    qDebug() << "max_size=" << max_size;
    for(int i = 0; i < max_size; i++)
    {
        quint16 spy, epx, epy;
        quint32 pointcount;
        quint8 gray;
//        qDebug() << "pointcount=" << pointcount;
        in >> pointcount;
//        qDebug() << "pointcount=" << i << pointcount;
        eachlayer.clear();
        for(int j = 0; j < pointcount; j++)
        {
            whitedata wd;
            in >> spy;
            in >> epy;
            in >> epx;
            in >> gray;
            wd.sp.setX(epx);
            wd.sp.setY(spy);
            wd.ep.setX(epx);
            wd.ep.setY(epy);
            wd.gray = gray;
            eachlayer.push_back(wd);
        }
        layerresult lr;
        lr.layerid = i;
        lr.layerimgdata = eachlayer;
        resultlayer.push_back(lr);
        in >> nextline;
        in >> nextline;
    }
    in.readBytes(endtag, lentag);
    tagname = QString(QLatin1String(endtag));
    if(tagname != "MKSDLP")
    {
        qDebug() << "tagname != MKSDLP end----------";
        canread = false;
        pf->close();
        delete pf;
        return;
    }
    pf->close();
    qDebug() << "openfinish=";
    mparent->UpdatePreViewDialog(resolution, resultlayer);
    mparent->UpdatePreViewDetail(thickness, et, ol, be, bc);
    canread = true;
    delete pf;
}

void Slice::loadMdlpOld(QString filename, bool &canread)
{
    //使用旧格式
//    qDebug() << "loadMdlp----old---------";
    QFileInfo info(filename);
    while(resultlayer.size()>0)
    {
        resultlayer.erase(resultlayer.begin());
    }
    QFile* pf = new QFile(filename);
    pf->open(QIODevice::ReadOnly);
    QDataStream in(pf);
    quint16 rx, ry, blankint;
    quint8 nextline;
    QString px, py;
    QString readdata;
    char *tag = new char[11];
    char *endtag = new char[12];
    char *tx = new char[15];
    int et, ol, be, bc;
    uint lentag, rng;
    rng = 290*290*2+116*116*2+4;
    char *previewimg = new char[rng];
    in.readBytes(tag, lentag);
    QString tagname;
    std::vector<whitedata> eachlayer;
    tagname = QString(QLatin1String(tag));
    if(tagname != "MKSDLP")
    {
        qDebug() << "tagname != MKSDLP start----------";
        canread = false;
        pf->close();
        delete pf;
        return;
    }
    in.readRawData(previewimg, rng);
    in >> rx;
    max_size = (int)rx;
    in >> rx;
    in >> ry;
    resolution.setX(rx);
    resolution.setY(ry);
    in >> px;
    platform.setX(px.toDouble());
    in >> py;
    platform.setY(py.toDouble());
    in >> readdata;
    thickness = readdata.toDouble();
    in >> blankint;
    et = (int)blankint;
    in >> blankint;
    ol = (int)blankint;
    in >> blankint;
    be = (int)blankint;
    in >> blankint;
    bc = (int)blankint;
//    qDebug() << "max_size---" << max_size;
    for(int i = 0; i < max_size; i++)
    {
        quint16 spy, epx, epy;
        quint32 pointcount;
        in >> pointcount;
        eachlayer.clear();
//        qDebug() << "pointcount---" << i << pointcount;
        for(int j = 0; j < pointcount; j++)
        {
            whitedata wd;
            in >> spy;
            in >> epy;
            in >> epx;
            wd.sp.setX(epx);
            wd.sp.setY(spy);
            wd.ep.setX(epx);
            wd.ep.setY(epy);
            eachlayer.push_back(wd);
        }
        layerresult lr;
        lr.layerid = i;
        lr.layerimgdata = eachlayer;
        resultlayer.push_back(lr);
        in >> nextline;
        in >> nextline;
    }
    in.readBytes(endtag, lentag);
    tagname = QString(QLatin1String(endtag));
    if(tagname != "MKSDLP")
    {
        qDebug() << "tagname != MKSDLP end----------";
        canread = false;
        pf->close();
        delete pf;
        return;
    }
    pf->close();
//    qDebug() << "openfinish=";
    mparent->UpdatePreViewDialog(resolution, resultlayer);
    mparent->UpdatePreViewDetail(thickness, et, ol, be, bc);
    canread = true;
    delete pf;
}

void Slice::newloadMdlp(QString filename, bool &canread)
{
//    qDebug() << "newloadMdlp-------------";
//    QDir dir;
//    if(!dir.exists("dlppng")){
//        dir.mkdir("dlppng");
//    }
//    dir="dlppng";
//    qDebug() << "path ===" <<  dir.filePath(QString::number(5)+".png");
//    QString path;
//    this->clearFiles(dir);
    QFileInfo info(filename);
    while(resultlayer.size()>0)
    {
        resultlayer.erase(resultlayer.begin());
    }
    QFile* pf = new QFile(filename);
    pf->open(QIODevice::ReadOnly);
    QDataStream in(pf);
    quint16 rx, ry, blankint;
    quint8 nextline;
    QString px, py;
    QString readdata;
    char *tag = new char[11];
    char *endtag = new char[12];
    char *tx = new char[15];
    int et, ol, be, bc;
    uint lentag, rng;
    rng = 290*290*2+116*116*2+4;
    char *previewimg = new char[rng];
    in.readBytes(tag, lentag);
    QString tagname;
    std::vector<whitedata> eachlayer;
    tagname = QString(QLatin1String(tag));
    if(tagname != "MKSDLP")
    {
        qDebug() << "tagname != MKSDLP start----------";
        canread = false;
        pf->close();
        delete pf;
        return;
    }
    in.readRawData(previewimg, rng);
    in >> rx;
    max_size = (int)rx;
    in >> rx;
    in >> ry;
    resolution.setX(rx);
    resolution.setY(ry);
    in >> px;
    platform.setX(px.toDouble());
    in >> py;
    platform.setY(py.toDouble());
    in >> readdata;
    thickness = readdata.toDouble();
    in >> blankint;
    et = (int)blankint;
    in >> blankint;
    ol = (int)blankint;
    in >> blankint;
    be = (int)blankint;
    in >> blankint;
    bc = (int)blankint;
    in >> blankint;
//    qDebug() << "in1=" << (int)blankint;
    in >> blankint;
//    qDebug() << "in2=" << (int)blankint;
    in >> blankint;
//    qDebug() << "in3=" << (int)blankint;
    in >> blankint;
//    qDebug() << "in4=" << (int)blankint;
    in >> blankint;
//    qDebug() << "in5=" << (int)blankint;
    in >> blankint;
//    qDebug() << "in6=" << (int)blankint;
    in >> blankint;
//    qDebug() << "in7=" << (int)blankint;
    qDebug() << "max_size=" << max_size;
    QImage nowimg = QImage(resolution.x(), resolution.y(), QImage::Format_RGB32);
    for(int i = 0; i < max_size; i++)
    {
        quint16 spy, epx, epy;
        quint32 pointcount;
        quint8 gray;
//        qDebug() << "pointcount=" << pointcount;
        in >> pointcount;
//        qDebug() << "pointcount=" << i << pointcount;
        eachlayer.clear();
        for(int j = 0; j < pointcount; j++)
        {
            whitedata wd;
            in >> spy;
            in >> epy;
            in >> epx;
            in >> gray;
            wd.sp.setX(epx);
            wd.sp.setY(spy);
            wd.ep.setX(epx);
            wd.ep.setY(epy);
            wd.gray = gray;
            eachlayer.push_back(wd);
        }
//        layerresult lr;
//        lr.layerid = i;
//        lr.layerimgdata = eachlayer;
//        resultlayer.push_back(lr);
        nowimg.fill(QColor(0, 0, 0));
        whitedata wd;
        QPainter qp;
        qp.begin(&nowimg);
        //    while (!qp.isActive()) {
        //        qDebug() << "Painter not active";
        //        qp.begin(&nowimg);
        //    }
        //    qp.setPen(QColor(255, 255, 255));
        for(int i = 0; i < eachlayer.size(); i++)
        {
            wd = eachlayer[i];
            int gray = mapNumbers(wd.gray,1);
//            qDebug() << "wd.gray===" << wd.gray;
//            qDebug() << "gray===" << gray;
            qp.setPen(QColor(gray, gray, gray));
            qp.drawLine(wd.sp.x(), wd.sp.y(), wd.ep.x(), wd.ep.y());
        }
//        path = dir.filePath(QString::number(i)+".png");
//        nowimg.save(path,"PNG");
        qp.end();
        in >> nextline;
        in >> nextline;
    }
    in.readBytes(endtag, lentag);
    tagname = QString(QLatin1String(endtag));
    if(tagname != "MKSDLP")
    {
        qDebug() << "tagname != MKSDLP end----------";
        canread = false;
        pf->close();
        delete pf;
        return;
    }
    pf->close();
//    qDebug() << "openfinish=";
    mparent->UpdatePreViewDialogCopy(resolution, max_size);
    mparent->UpdatePreViewDetail(thickness, et, ol, be, bc);
    canread = true;
    delete pf;
}


void Slice::setZipFilename(QString filename, QString zipfilename)
{
    QuaZip mzip(zipfilename);
    mzip.open(QuaZip::mdUnzip);
    mzip.setCurrentFile("preview.png");
    QuaZipFile mzipfile(&mzip);
    QImage screenshot;
    if(mzipfile.open(QIODevice::ReadOnly))
    {
        QByteArray imgdata = mzipfile.readAll();
        screenshot.loadFromData(imgdata);
        mzipfile.close();
    }
    mzip.close();
}

//void Slice::setFilename(QString filename,int &precent, bool &finish)
//{
//    QDateTime current_date_time0 =QDateTime::currentDateTime();
//    QString current_date0 =current_date_time0.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
//    qDebug() << "SAVE---STARTtime---" << current_date0;
//    int et, ol, be, bc;
//    QFile* pf = new QFile(filename);
//    pf->open(QIODevice::WriteOnly);
//    QDataStream out(pf);
//    out << "MKSDLP" ;
//    if(!isSaveZip)
//    {
//        mparent->OutputScreenShot(out);
//    }else{
//        int r, g, b, rgb, width, height;
//        QImage hendimg = zippreview.scaled(116, 116, Qt::KeepAspectRatio);
//        width = hendimg.width();
//        height = hendimg.height();
//        for(int h = 0; h < height; h++)
//        {
//            for(int w = 0; w < width; w++)
//            {
//                QColor pcolor(hendimg.pixel(w, h));
//                r = pcolor.red() >> 3;
//                g = pcolor.green() >> 2;
//                b = pcolor.blue() >> 3;
//                rgb = (r << 11) | (g << 5) | b;
//                out << (quint16) rgb;
//            }
//        }
//        out << (quint8)0x0D << (quint8)0x0A;
//        width = zippreview.width();
//        height = zippreview.height();
//        for(int h = 0; h < height; h++)
//        {
//            for(int w = 0; w < width; w++)
//            {
//                QColor pcolor(zippreview.pixel(w, h));
//                r = pcolor.red() >> 3;
//                g = pcolor.green() >> 2;
//                b = pcolor.blue() >> 3;
//                rgb = (r << 11) | (g << 5) | b;
//                out << (quint16) rgb;
//            }
//        }
//        out << (quint8)0x0D << (quint8)0x0A;
//    }
//    out << (quint16)max_size << (quint16)resolution.x() << (quint16)resolution.y();
//    out << QString::number(platform.x())
//        << QString::number(platform.y())
//        << QString::number(thickness);
//    QString bd;
//    int cd;
////    mparent->getData("mksdlp_expose", bd, "8");
////    et = bd.toInt();
//    out << (quint16)mparent->pvdialog->et;
////    mparent->getData("mksdlp_lftime", bd, "3");
////    ol = bd.toInt();
//    out << (quint16)mparent->pvdialog->ol;
////    mparent->getData("mksdlp_bexpose", bd, "3");
////    be = bd.toInt();
//    out << (quint16)mparent->pvdialog->be;
////    mparent->getData("mksdlp_blayer", bd, "3");
////    bc = bd.toInt();
//    out << (quint16)mparent->pvdialog->bc;
//    mparent->getData("mksdlp_botliftdis", bd, "5");
//    cd = bd.toInt();
//    out << (quint16)cd;
//    mparent->getData("mksdlp_botliftspeed", bd, "65");
//    cd = bd.toInt();
//    out << (quint16)cd;
//    mparent->getData("mksdlp_liftdis", bd, "5");
//    cd = bd.toInt();
//    out << (quint16)cd;
//    mparent->getData("mksdlp_liftspeed", bd, "65");
//    cd = bd.toInt();
//    out << (quint16)cd;
//    mparent->getData("mksdlp_returnspeed", bd, "150");
//    cd = bd.toInt();
//    out << (quint16)cd;
//    mparent->getData("mksdlp_botbrightnessPVM", bd, "255");
//    cd = bd.toInt();
//    out << (quint16)cd;
//    mparent->getData("mksdlp_brightnessPVM", bd, "255");
//    cd = bd.toInt();
//    out << (quint16)cd;
//    //temp part
////    out << (quint16)5;
////    out << (quint16)5;
////    out << (quint16)65;
////    out << (quint16)65;
////    out << (quint16)150;
////    out << (quint16)88;
////    out << (quint16)88;
////    out << (quint16)88;
//    //end
////    std::vector<whitedata> eachlayer;
//    whitedata eachline;
////    qDebug() << "resultlayer.size()" << resultlayer.size();
////    for(int i = 0; i < resultlayer.size(); i++)
////    {
////        precent = i/resultlayer.size();
////        eachlayer = resultlayer[i].layerimgdata;
////        out << (quint32)eachlayer.size();
//////        qDebug() << "eachlayer.size()" << eachlayer.size();
////        int size = eachlayer.size();
////        int gray = 0;
////        for(int j = 0; j < eachlayer.size(); j++)
////        {
////            eachline = eachlayer[j];
//////            qDebug() << "eachline.gray" << eachline.gray;
////            out << (quint16)eachline.sp.y() << (quint16)eachline.ep.y() << (quint16)eachline.ep.x() << (quint8)eachline.gray;
////        }
////        out << (quint8)0x0D << (quint8)0x0A;
////    }
//    //20200225修改切片方式读取图片
////    MDialog *md;
//    //切片，读取本地图片
//    for (int i=0;i<max_size;i++) {
//        std::vector<QImage> imglist;
////        QDateTime current_date_time0 =QDateTime::currentDateTime();
////        QString current_date0 =current_date_time0.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
////        qDebug() << "imagePaths---starttime---" << current_date0;
//        mparent->pvdialog->getImg(i,imglist);
////        QDateTime current_date_time1 =QDateTime::currentDateTime();
////        QString current_date1 =current_date_time1.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
////        qDebug() << "imagePaths---ENDtime---" << current_date1;
//        std::vector<whitedata> eachlayer;
//        whitedata wd;
//        mparent->pvdialog->imgToLayer(imglist,eachlayer);
////        QDateTime current_date_time2 =QDateTime::currentDateTime();
////        QString current_date2 =current_date_time2.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
////        qDebug() << "imgToLayer---ENDtime---" << current_date2;
////        QApplication::processEvents();
//        precent = i * 100/max_size;
//        if (i < max_size - 1 ) {
//            finish = false;
//        }else {
//            finish = true;
//        }
//        mparent->saveprocess(precent,finish);
//        out << (quint32)eachlayer.size();
//        for(int j = 0; j < eachlayer.size(); j++)
//        {
//            eachline = eachlayer[j];
//            out << (quint16)eachline.sp.y() << (quint16)eachline.ep.y() << (quint16)eachline.ep.x() << (quint8)eachline.gray;
//        }
//        out << (quint8)0x0D << (quint8)0x0A;
////        QDateTime current_date_time3 =QDateTime::currentDateTime();
////        QString current_date3 =current_date_time3.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
////        qDebug() << "layer---ENDtime---" << current_date3;
//    }

//    out << "MKSDLP";
//    pf->close();
//    delete pf;
//    QDateTime current_date_time3 =QDateTime::currentDateTime();
//    QString current_date3 =current_date_time3.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
//    qDebug() << "SAVE---ENDtime---" << current_date3;
//    if (mparent->isChange) {
////        qApp->quit();
////        this->mparent->close();
//        QTimer::singleShot(100,mparent,SLOT(close()));
//    }else {
//        mparent->UpdatePreViewDetail(thickness, et, ol, be, bc);
//    }


//}

void Slice::setFilename(QString filename,int &precent, bool &finish)
{
    QDateTime current_date_time0 =QDateTime::currentDateTime();
    QString current_date0 =current_date_time0.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    qDebug() << "SAVE---STARTtime---" << current_date0;
    int et, ol, be, bc;
    QFile* pf = new QFile(filename);
    pf->open(QIODevice::WriteOnly);
    QDataStream out(pf);
    out << "MKSDLP" ;
    if(!isSaveZip)
    {
        mparent->OutputScreenShot(out);
    }else{
        int r, g, b, rgb, width, height;
        QImage hendimg = zippreview.scaled(116, 116, Qt::KeepAspectRatio);
        width = hendimg.width();
        height = hendimg.height();
        for(int h = 0; h < height; h++)
        {
            for(int w = 0; w < width; w++)
            {
                QColor pcolor(hendimg.pixel(w, h));
                r = pcolor.red() >> 3;
                g = pcolor.green() >> 2;
                b = pcolor.blue() >> 3;
                rgb = (r << 11) | (g << 5) | b;
                out << (quint16) rgb;
            }
        }
        out << (quint8)0x0D << (quint8)0x0A;
        width = zippreview.width();
        height = zippreview.height();
        for(int h = 0; h < height; h++)
        {
            for(int w = 0; w < width; w++)
            {
                QColor pcolor(zippreview.pixel(w, h));
                r = pcolor.red() >> 3;
                g = pcolor.green() >> 2;
                b = pcolor.blue() >> 3;
                rgb = (r << 11) | (g << 5) | b;
                out << (quint16) rgb;
            }
        }
        out << (quint8)0x0D << (quint8)0x0A;
    }
    out << (quint16)max_size << (quint16)resolution.x() << (quint16)resolution.y();
    out << QString::number(platform.x())
        << QString::number(platform.y())
        << QString::number(thickness);
    QString bd;
    int cd;
//    mparent->getData("mksdlp_expose", bd, "8");
//    et = bd.toInt();
    out << (quint16)mparent->pvdialog->et;
//    mparent->getData("mksdlp_lftime", bd, "3");
//    ol = bd.toInt();
    out << (quint16)mparent->pvdialog->ol;
//    mparent->getData("mksdlp_bexpose", bd, "3");
//    be = bd.toInt();
    out << (quint16)mparent->pvdialog->be;
//    mparent->getData("mksdlp_blayer", bd, "3");
//    bc = bd.toInt();
    out << (quint16)mparent->pvdialog->bc;
    if (mparent->newSliceType){
        mparent->getData("mksdlp_botliftdis", bd, "5");
        cd = bd.toInt();
        out << (quint16)cd;
        mparent->getData("mksdlp_botliftspeed", bd, "65");
        cd = bd.toInt();
        out << (quint16)cd;
        mparent->getData("mksdlp_liftdis", bd, "5");
        cd = bd.toInt();
        out << (quint16)cd;
        mparent->getData("mksdlp_liftspeed", bd, "65");
        cd = bd.toInt();
        out << (quint16)cd;
        mparent->getData("mksdlp_returnspeed", bd, "150");
        cd = bd.toInt();
        out << (quint16)cd;
        mparent->getData("mksdlp_botbrightnessPVM", bd, "255");
        cd = bd.toInt();
        out << (quint16)cd;
        mparent->getData("mksdlp_brightnessPVM", bd, "255");
        cd = bd.toInt();
        out << (quint16)cd;
    }
    //temp part
//    out << (quint16)5;
//    out << (quint16)5;
//    out << (quint16)65;
//    out << (quint16)65;
//    out << (quint16)150;
//    out << (quint16)88;
//    out << (quint16)88;
//    out << (quint16)88;
    //end
    std::vector<whitedata> eachlayer;
    whitedata eachline;
    qDebug() << "resultlayer.size()" << resultlayer.size();
    for(int i = 0; i < resultlayer.size(); i++)
    {
//        precent = i/resultlayer.size();
        eachlayer = resultlayer[i].layerimgdata;
        out << (quint32)eachlayer.size();
        precent = i * 100/max_size;
        if (i < max_size - 1 ) {
            finish = false;
        }else {
            finish = true;
        }
        mparent->saveprocess(precent,finish);

        for(int j = 0; j < eachlayer.size(); j++)
        {
            eachline = eachlayer[j];
//            qDebug() << "eachline.gray" << eachline.gray;
//            out << (quint16)eachline.sp.y() << (quint16)eachline.ep.y() << (quint16)eachline.ep.x() << (quint8)eachline.gray;
            out << (quint16)eachline.sp.y() << (quint16)eachline.ep.y() << (quint16)eachline.ep.x();

        }
        out << (quint8)0x0D << (quint8)0x0A;
    }
    out << "MKSDLP";
    pf->close();
    delete pf;
    QDateTime current_date_time3 =QDateTime::currentDateTime();
    QString current_date3 =current_date_time3.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    qDebug() << "SAVE---ENDtime---" << current_date3;
    if (mparent->isChange) {
//        qApp->quit();
//        this->mparent->close();
        QTimer::singleShot(100,mparent,SLOT(close()));
    }else {
        mparent->UpdatePreViewDetail(thickness, et, ol, be, bc);
    }


}

void Slice::setFilenameSave(QString filename)
{
    qDebug() << "slice---start---" << filename;
    //slice and save
    saveFileName = filename;
    int et, ol, be, bc;
    QFile* pf = new QFile(filename);
    pf->open(QIODevice::WriteOnly);
    QDataStream out(pf);
    out << "MKSDLP" ;
    if(!isSaveZip)
    {
        mparent->OutputScreenShot(out);
    }else{
        int r, g, b, rgb, width, height;
        QImage hendimg = zippreview.scaled(116, 116, Qt::KeepAspectRatio);
        width = hendimg.width();
        height = hendimg.height();
        for(int h = 0; h < height; h++)
        {
            for(int w = 0; w < width; w++)
            {
                QColor pcolor(hendimg.pixel(w, h));
                r = pcolor.red() >> 3;
                g = pcolor.green() >> 2;
                b = pcolor.blue() >> 3;
                rgb = (r << 11) | (g << 5) | b;
                out << (quint16) rgb;
            }
        }
        out << (quint8)0x0D << (quint8)0x0A;
        width = zippreview.width();
        height = zippreview.height();
        for(int h = 0; h < height; h++)
        {
            for(int w = 0; w < width; w++)
            {
                QColor pcolor(zippreview.pixel(w, h));
                r = pcolor.red() >> 3;
                g = pcolor.green() >> 2;
                b = pcolor.blue() >> 3;
                rgb = (r << 11) | (g << 5) | b;
                out << (quint16) rgb;
            }
        }
        out << (quint8)0x0D << (quint8)0x0A;
    }
    out << (quint16)max_size << (quint16)resolution.x() << (quint16)resolution.y();
    out << QString::number(platform.x())
        << QString::number(platform.y())
        << QString::number(thickness);
    QString bd;
    int cd;
    mparent->getData("mksdlp_expose", bd, "8");
    et = bd.toInt();
    out << (quint16)et;
    mparent->getData("mksdlp_lftime", bd, "3");
    ol = bd.toInt();
    out << (quint16)ol;
    mparent->getData("mksdlp_bexpose", bd, "3");
    be = bd.toInt();
    out << (quint16)be;
    mparent->getData("mksdlp_blayer", bd, "3");
    bc = bd.toInt();
    out << (quint16)bc;
    if (mparent->newSliceType){
        mparent->getData("mksdlp_botliftdis", bd, "5");
        cd = bd.toInt();
        out << (quint16)cd;
        mparent->getData("mksdlp_botliftspeed", bd, "65");
        cd = bd.toInt();
        out << (quint16)cd;
        mparent->getData("mksdlp_liftdis", bd, "5");
        cd = bd.toInt();
        out << (quint16)cd;
        mparent->getData("mksdlp_liftspeed", bd, "65");
        cd = bd.toInt();
        out << (quint16)cd;
        mparent->getData("mksdlp_returnspeed", bd, "150");
        cd = bd.toInt();
        out << (quint16)cd;
        mparent->getData("mksdlp_botbrightnessPVM", bd, "255");
        cd = bd.toInt();
        out << (quint16)cd;
        mparent->getData("mksdlp_brightnessPVM", bd, "255");
        cd = bd.toInt();
        out << (quint16)cd;
    }
    pf->close();
    delete pf;
}

void Slice::generateByImage(unsigned int id, QImage &floorimg)
{
    qDebug() << "generateByImage" << id;
    whitedata wd;
    QVector2D sp(-1, -1), ep(-1, -1);
    std::vector<whitedata> resultlist;
    int x, y, width;
    int star_x = 0, star_y = 0, end_x = resolution.x(), end_y = resolution.y();
    width = resolution.x();
    bool bCurColorIsWhite = false;
    int imagesize = end_x+end_y*width;
    unsigned currentpos = star_x + star_y*width;
//    if(star_x > end_x || star_y > end_y)
//    {
//        currentpos = imagesize -1;
//        star_x = end_x;
//        star_y = end_y;
//    }
//    uchar* imagedata = floorimg.bits();

    for(int h = star_x; h < end_x; h++)
    {
        for(int v = star_y; v<end_y; v++)
        {
            if(this->mparent->isCancel){
                break;
            }
            x = h;
            y = v;
            QColor mcolor(floorimg.pixel(x, y));
            int r = (mcolor.red()+mcolor.blue()+mcolor.green())/3;
//            int r = imagedata[(int)((y*width+x)*4)];
            if(r > 30)
            {
                bCurColorIsWhite = true;
            }else{
                bCurColorIsWhite = false;
            }
            if(bCurColorIsWhite)
            {
//                if(slicetype == 1)
//                {
//                    x = resolution.x() - x;
//                }
                if(sp.x() == -1 || sp.y() == -1)
                {
                    sp.setX(x);
                    sp.setY(y);
                }
                if(ep.x() == -1 || ep.y() == -1)
                {
                    ep.setX(x);
                    ep.setY(y);
                }else if(y - ep.y() == 1 && ep.x() == x){
                    ep.setY(y);
                }else{
                    wd.sp = sp;
                    wd.ep = ep;
                    resultlist.push_back(wd);
                    sp.setX(x);
                    sp.setY(y);
                    ep.setX(x);
                    ep.setY(y);
                }
            }
        }
        currentpos = x + y*width;
        if(currentpos > imagesize)
        {
            break;
        }
    }
    wd.sp = sp;
    wd.ep = ep;
    resultlist.push_back(wd);
    layerresult lr;
//    id = id-1;
    qDebug() << "generateByImage--layer" << id;
    lr.layerid = id;
    lr.layerimgdata = resultlist;
    resultlayer[id] = lr;
//    std::vector<struct whitedata>().swap(resultlist);
    while(resultlist.size()>0)
    {
        resultlist.erase(resultlist.begin());
    }
}

void Slice::generateByPreSlice(unsigned int id, double att, QImage &floorimg)
{
    QDateTime current_date_time0 =QDateTime::currentDateTime();
    QString current_date0 =current_date_time0.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    qDebug() << "resultlist---starttime---" << id << current_date0;
    ModelData* md;
    std::vector<triangle> tl;
    std::vector<QImage> imglist;
    std::vector<MSupport> suplist;
    int star_x = resolution.x(), star_y = resolution.y(), end_x = 0, end_y = 0;
//    int totaltri = 0;
    QVector3D thisP, thatP, mdoffset, mdposition, mdsize;
    float mdz = 0;
    int cmpcount = 0;
    double xdisp, ydisp, zdisp, planefraction;
    std::vector<segment> segments, segments1;
    QVector<segment> qsegments;
    QList<segment> qlsegments;
    triangle t;
    segment sg;
    segment bsg, nsg;
    QVector2D points[2];
    double potentialDist;
    int finalid, mpid;
    double mindist = 100000.0;
    int waitid, nid;
    QVector2D pointp1, pointp2;
//    hash_map<int, std::string> hmlist;
    QMap<int, std::string> hmlist;
    int j = 0;
    int nextid = 0;
    for(int i = 0; i < mdlist.size(); i++)
    {
        md = mdlist[i];
        md->initGetNexy();
//        int totaltri = 0;
//        totaltri = 0;
//        totaltri = md->getZTriCount(att);
        suplist = md->supportlist;
//        QVector3D thisP, thatP, mdoffset, mdposition, mdsize;
        thisP = QVector3D(0, 0, 0);
        thatP = QVector3D(0, 0, 0);
        mdoffset = QVector3D(0, 0, 0);
        mdposition = QVector3D(0, 0, 0);
        mdsize = QVector3D(0, 0, 0);
//        float mdz = md->getOffset().z();
        mdz = md->getOffset().z();
        mdoffset = md->getOffset();
        mdposition = md->getPosition();
        mdsize = md->getSize();
//        int cmpcount = 0;
        cmpcount = 0;
//        for(int j = 0; j < suplist.size(); j++)
//        {
//            totaltri += suplist[j].getZTriCount(att);
//        }
//        double xdisp, ydisp, zdisp, planefraction;
//        std::vector<segment> segments, segments1;
//        QVector<segment> qsegments;
//        QList<segment> qlsegments;
//        triangle t;
//        segment sg;
//        segment bsg, nsg;
//        QVector2D points[2];
//        double potentialDist;
//        int finalid, mpid;
//        double mindist = 100000.0;
//        int waitid, nid;
//        QVector2D pointp1, pointp2;
//        hash_map<int, std::string> hmlist;
//        int j = 0;
//        int nextid = 0;
        xdisp=0, ydisp=0, zdisp=0, planefraction=0;
//        segments.~vector(), segments1.~vector();
//        qsegments.~QVector();
//        qlsegments.~QList();
//        t.~triangle();
//        sg.~segment();
//        bsg.~segment(), nsg.~segment();
        QVector2D points[2];
        potentialDist=0;
        finalid=0, mpid=0;
        mindist = 100000.0;
        waitid =0;
        nid=0;
        pointp1 = QVector2D(0,0);
        pointp2 = QVector2D(0,0);
        hmlist.clear();
        j = 0;
        nextid = 0;
        QDateTime current_date_time1 =QDateTime::currentDateTime();
        QString current_date1 =current_date_time1.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
        qDebug() << "getNextTri---starttime---" << id << current_date1;
        while(md->getNextTri(t, att, nextid) && !this->mparent->isCancel && !canstop)
        {
            if (canstop){
                qDebug() << "while(md->isComplete";
                break;
            }
//            qDebug() << "while(md->getNextTri" << t.id << att;
            cmpcount = 0;
            for(int v1 = 0; v1 < 3; v1++)
            {
                thisP = t.vertex[v1];

                if(thisP.z() <= att)
                {
                    continue;
                }
                for(int v2 = 0; v2 < 3; v2++){
                    if(v2 == v1)
                    {
                        continue;
                    }
                    thatP = t.vertex[v2];
                    if(thatP.z() > att)
                    {
                        continue;
                    }
                    //根据空间直线的对称式方程和参数方程算出新的顶点
                    xdisp = thatP.x() - thisP.x();
                    ydisp = thatP.y() - thisP.y();
                    zdisp = thatP.z() - thisP.z();
                    planefraction = (thisP.z() - att)/(fabs(zdisp));
                    points[cmpcount].setX(thisP.x() + xdisp*planefraction);
                    points[cmpcount].setY(thisP.y() + ydisp*planefraction);
                    cmpcount++;
                }
            }
            points[0].setX((points[0].x()+mdposition.x())/pixscale.x());//根据平台大小缩放
            points[0].setY((points[0].y()+mdposition.y())/pixscale.y());
            points[1].setX((points[1].x()+mdposition.x())/pixscale.x());
            points[1].setY((points[1].y()+mdposition.y())/pixscale.y());
            sg.p1 = points[0] + resolution/2; //resolution像素值，移到平台中间
            sg.p2 = points[1] + resolution/2;
            if(sg.p2.x() > end_x)  //限制不能超过平台xy
            {
                end_x = ceil(sg.p2.x())+1;
            }
            if(sg.p2.y() > end_y)
            {
                end_y = ceil(sg.p2.y())+1;
            }
            if(sg.p2.x() < star_x)
            {
                star_x = floor(sg.p2.x())-1;
            }
            if(sg.p2.y() < star_y)
            {
                star_y = floor(sg.p2.y())-1;
            }
            sg.normal.setX(t.normal.x());
            sg.normal.setY(t.normal.y());
            sg.normal.normalize();
            sg.trinormal.setX(t.normal.x());
            sg.trinormal.setY(t.normal.y());
            sg.trinormal.setZ(t.normal.z());
            sg.trinormal.normalize();
            sg.inloop = false;
            sg.linepoint = false;
            sg.frontid = -1;
            sg.backid = -1;
            sg.nowid = j;
            CorrectPointOrder(sg); //排序两点，由左到右
            segments.push_back(sg);
            mpid = floor((sg.p1.x()*100+0.5));
            std::ostringstream ss;
            ss << j;
//            hmlist[mpid] += "|"+ss.str();
            if (hmlist.contains(mpid)) {
                std::string list = "|" + ss.str();
                std::istringstream fa(hmlist[mpid]);
                std::string sa;
                while(std::getline(fa, sa, '|'))
                {
                    if(sa.empty())
                    {
                        continue;
                    }
                    list += "|" + sa;
                }
                hmlist.insert(mpid,list);
            }else {
                hmlist.insert(mpid,"|" + ss.str());
            }
            j++;
        }
        QDateTime current_date_time2 =QDateTime::currentDateTime();
        QString current_date2 =current_date_time2.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
        qDebug() << "getNextTri---endtime---" << id << current_date2;
        for(int supid=0;supid<suplist.size();supid++)
        {
//            qDebug() << "for(int supid" << supid;
            suplist[supid].setOP(mdoffset, mdposition);
            suplist[supid].initGetNexy();
            nextid = 0;
            while(suplist[supid].getNextTri(t, att, nextid))
            {
                cmpcount = 0;
                for(int v1 = 0; v1 < 3; v1++)
                {
                    thisP = t.vertex[v1];

                    if(thisP.z() <= att)
                    {
                        continue;
                    }
                    for(int v2 = 0; v2 < 3; v2++){
                        if(v2 == v1)
                        {
                            continue;
                        }
                        thatP = t.vertex[v2];
                        if(thatP.z() > att)
                        {
                            continue;
                        }
                        xdisp = thatP.x() - thisP.x();
                        ydisp = thatP.y() - thisP.y();
                        zdisp = thatP.z() - thisP.z();
                        planefraction = (thisP.z() - att)/(fabs(zdisp));
                        points[cmpcount].setX(thisP.x() + xdisp*planefraction);
                        points[cmpcount].setY(thisP.y() + ydisp*planefraction);
                        cmpcount++;
                    }
                }
                points[0].setX((points[0].x()+mdposition.x())/pixscale.x());
                points[0].setY((points[0].y()+mdposition.y())/pixscale.y());
                points[1].setX((points[1].x()+mdposition.x())/pixscale.x());
                points[1].setY((points[1].y()+mdposition.y())/pixscale.y());
                sg.p1 = points[0] + resolution/2;
                sg.p2 = points[1] + resolution/2;
                if(sg.p2.x() > end_x)
                {
                    end_x = ceil(sg.p2.x())+1;
                }
                if(sg.p2.y() > end_y)
                {
                    end_y = ceil(sg.p2.y())+1;
                }
                if(sg.p2.x() < star_x)
                {
                    star_x = floor(sg.p2.x())-1;
                }
                if(sg.p2.y() < star_y)
                {
                    star_y = floor(sg.p2.y())-1;
                }
                sg.normal.setX(t.normal.x());
                sg.normal.setY(t.normal.y());
                sg.normal.normalize();
                sg.trinormal.setX(t.normal.x());
                sg.trinormal.setY(t.normal.y());
                sg.trinormal.setZ(t.normal.z());
                sg.trinormal.normalize();
                sg.inloop = false;
                sg.linepoint = false;
                sg.frontid = -1;
                sg.backid = -1;
                sg.nowid = j;
                CorrectPointOrder(sg);
                segments.push_back(sg);
                mpid = floor((sg.p1.x()*100+0.5));
                std::ostringstream ss;
                ss << j;
//                hmlist[mpid] += "|"+ss.str();
                if (hmlist.contains(mpid)) {
                    std::string list = "|" + ss.str();
                    std::istringstream fa(hmlist[mpid]);
                    std::string sa;
                    while(std::getline(fa, sa, '|'))
                    {
                        if(sa.empty())
                        {
                            continue;
                        }
                        list += "|" + sa;
                    }
                    hmlist.insert(mpid,list);
                }else {
                    hmlist.insert(mpid,"|" + ss.str());
                }
                j++;
            }
        }
        QDateTime current_date_time3 =QDateTime::currentDateTime();
        QString current_date3 =current_date_time3.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
        qDebug() << "for(int supid---endtime---" << id << current_date3;
        for(int sgi = 0; sgi < segments.size(); sgi++)
        {//把线段相近点连接
            if(this->mparent->isCancel){
                qDebug() << "for(int sgi this->mparent->isCancel";
                break;
            }
//            qDebug() << "for(int sgi" << sgi;
            if(segments[sgi].frontid != -1 || segments[sgi].linepoint)
            {
                continue;
            }
            mpid = floor((segments[sgi].p2.x()*100+0.5));
            mpid -= 3;
            mindist = 100000.0;
            finalid = -1;
            for(int nearpoint = 0; nearpoint < 7; nearpoint++)
            {
                if (hmlist[mpid+nearpoint].empty())
                {
                    continue;
                }
                std::istringstream f(hmlist[mpid+nearpoint]);
                std::string s;
                while(std::getline(f, s, '|'))
                {
                    if(s.empty())
                    {
                        continue;
                    }
                    nid = atoi(s.c_str());
                    if(nid == sgi || segments[nid].backid != -1)
                    {
                        continue;
                    }
                    potentialDist = distance2D(segments[sgi].p2, segments[nid].p1);
                    if(potentialDist < mindist && potentialDist <= 0.03)
                    {
                        mindist = potentialDist;
                        finalid = nid;
                    }
                }
            }
            if(finalid != -1)
            {
                segments[sgi].frontid = finalid;
                segments[finalid].backid = sgi;
                segments[sgi].p2 = segments[finalid].p1;
                pointp1 = segments[sgi].p1;
                pointp2 = segments[sgi].p2;
                double dx = pointp2.x() - pointp1.x();
                double dy = pointp2.y() - pointp1.y();
                segments[sgi].normal.setX(-dy);
                segments[sgi].normal.setY(dx);
                segments[sgi].normal.normalize();
            }
        }
        QDateTime current_date_time4 =QDateTime::currentDateTime();
        QString current_date4 =current_date_time4.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
        qDebug() << "for(int sgi---endtime---" << id << current_date4;
        std::vector<loop> looplist;
        OutPutData op = OutPutData(segments, id);
        looplist = op.looplist;
        QDateTime current_date_time6 =QDateTime::currentDateTime();
        QString current_date6 =current_date_time6.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
        qDebug() << "OutPutData---endtime---" << id << current_date6;
//        std::sort(looplist.begin(), looplist.end(), colorcompare);
        floorimg.fill(QColor(0, 0, 0));
        QPainter fp;//填充颜色，根据向量方向判断，
        qDebug() << "tl.size()------" << id << tl.size();
        qDebug() << "segments.size()------" << id << segments.size();
        while(tl.size()>0)
        {
            tl.erase(tl.begin()); //erase使用此方法清空可释放内存
        }
//        while(segments.size()>0)
//        {
//            segments.erase(segments.begin());
//        }
        segments.clear();
        QDateTime current_date_time7 =QDateTime::currentDateTime();
        QString current_date7 =current_date_time7.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
        qDebug() << "QPainter---starttime---" << id << current_date7;
        fp.begin(&floorimg);
        fp.setRenderHint(QPainter::Antialiasing, true);
        fp.setPen(QPen(QColor(255, 255, 255), 0));
        qDebug() << "looplist.size()==" << looplist.size();
//        for(int i = 0; i < looplist.size(); i++)
//        {
////            qDebug() << "looplist[i].fillcolor==" << looplist[i].fillcolor;
//            if(looplist[i].fillcolor.red() == 255)
//            {
//                fp.setBrush(looplist[i].fillcolor);
//                fp.drawPath(looplist[i].mpath);
//            }
//        }
        QRgb q1, q2;
        for(int i = 0; i < looplist.size(); i++)
        {
            if(looplist[i].fillcolor.red() != 255)
            {
                q1 = floorimg.pixel(looplist[i].maxsize.x(), looplist[i].maxsize.y());
                if(qRed(q1) == 255)
                {
                    fp.setBrush(looplist[i].fillcolor);
                    fp.drawPath(looplist[i].mpath);
                }else{
                    fp.setBrush(QColor(255, 255, 255));
                    fp.drawPath(looplist[i].mpath);
                }
            }else{
                fp.setBrush(looplist[i].fillcolor);
                fp.drawPath(looplist[i].mpath);
            }
        }
//        for (int i=0; i<floorimg.width(); i++) {
//            for (int j=0; j<floorimg.height(); j++) {
//                QRgb rgb(floorimg.pixel(i, j));
//                QColor color(rgb);
//                if (color.red() != 255 && color.red() != 0) {
//                    qDebug() << "r" << color.red() << "g" << color.green() << "b" << color.blue();
//                }
//            }
//        }
//        testimage(floorimg);
//        floorimg.save("H:floorimgtest001.jpg","JPG");
        imglist.push_back(floorimg); //floorimg每一层的图片
        fp.end();
        QDateTime current_date_time5 =QDateTime::currentDateTime();
        QString current_date5 =current_date_time5.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
        qDebug() << "QPainter---endtime---" << id << current_date5;
        while(looplist.size()>0)
        {
            looplist.erase(looplist.begin());
        }
        while(segments.size()>0)
        {
            segments.erase(segments.begin());
        }
        while(segments1.size()>0)
        {
            segments1.erase(segments1.begin());
        }
        while(qsegments.size()>0)
        {
            qsegments.erase(qsegments.begin());
        }
        while(segments.size()>0)
        {
            qlsegments.erase(qlsegments.begin());
        }

//        segments.clear();
//        segments1.clear();
//        qsegments.clear();
//        qlsegments.clear();
//        t.~triangle();
//        segments.~vector(), segments1.~vector();
//        qsegments.~QVector();
//        qlsegments.~QList();
        t.~triangle();
        sg.~segment();
        bsg.~segment(), nsg.~segment();
    }

   //把图片转为坐标点
    if(star_x < 0)
    {
        star_x = 0;
    }
    if(star_y < 0)
    {
        star_y = 0;
    }
    if(end_x < 0)
    {
        end_x = 0;
    }
    if(end_y < 0)
    {
        end_y = 0;
    }
    if(end_x > resolution.x())
    {
        end_x = resolution.x();
    }
    if(end_y > resolution.y())
    {
        end_y = resolution.y();
    }
//    floorimg.fill(QColor(0, 0, 0));
    whitedata wd;
    QVector2D sp(-1, -1), ep(-1, -1);
    int gray = 0;
    int gray2 = 0;
    std::vector<whitedata> resultlist;
    int x, y, width;
    width = resolution.x();
    bool bCurColorIsWhite = false;
    int imagesize = end_x+end_y*width;
    unsigned currentpos = star_x + star_y*width;
    if(star_x > end_x || star_y > end_y)
    {
        currentpos = imagesize -1;
        star_x = end_x;
        star_y = end_y;
    }
    std::vector<uchar*> datalist;
//    for(int i =0;i<imglist.size();i++)
//    {
//        datalist.push_back(imglist[i].bits());
//    }

    //此时已经是镜像状态
    if(slicetype == 1){
        //x正方向
        for(int h = end_x-1; h > star_x-1; h--)
        {
            for(int v = star_y; v<end_y; v++)
            {
                x = h;
                y = v;
                for(int i = 0; i < imglist.size(); i++)
                {
                    if(x > end_x || y>end_y)
                    {
                        break;
                    }
                    QRgb rgb(imglist[i].pixel(x, y));
                    QColor color(rgb);
    //                int r = datalist[i][(int)((y*resolution.x()+x)*4)];
                    gray = (color.red()*19595 + color.green()*38469 + color.blue()*7472) >> 16;
                    gray = mapNumbers(gray,0);
                    if(gray != 0)
                    {
//                        qDebug() << "gray" << gray;
                        bCurColorIsWhite = true;
                        break;
                    }else{
                        bCurColorIsWhite = false;
                    }
                }
                if(bCurColorIsWhite)
                {
                    if(slicetype == 1)
                    {
                        x = resolution.x() - x;
                    }
                    //20191108 现在的切片数据格式（stary.endy,endx）连线方式
                    if(sp.x() == -1 || sp.y() == -1)
                    {
                        sp.setX(x);
                        sp.setY(y);
                        gray2 = gray;
                    }
                    if(ep.x() == -1 || ep.y() == -1)
                    {
                        ep.setX(x);
                        ep.setY(y);
                        gray2 = gray;
                    }else if(y - ep.y() == 1 && ep.x() == x && gray == gray2){
//                        qDebug() << "ep.setx==" << x;
//                        qDebug() << "ep.setY==" << y;
                        ep.setY(y);
                    }else{
//                        qDebug() << "end------------";
                        wd.sp = sp;
                        wd.ep = ep;
                        if (wd.ep.y() - wd.sp.y() > 1) {
                            wd.gray = 15;
                        }else {
                            wd.gray = gray;
                        }
                        gray2 = gray;
                        resultlist.push_back(wd);
                        sp.setX(x);
                        sp.setY(y);
                        ep.setX(x);
                        ep.setY(y);
                    }
                    // 20191108如果需要改为（x,y,灰度）这样的格式，
                    //只需要在此计算灰度值，不为0即符合
                    //但此格式会使数据量更大

                }
            }
            currentpos = x + y*width;
            if(currentpos > imagesize)
            {
                break;
            }
        }
    }else{
        //x镜像
        for(int h = star_x; h < end_x; h++)
        {
            for(int v = star_y; v<end_y; v++)
            {
                x = h;
                y = v;
                for(int i = 0; i < imglist.size(); i++)
                {
                    if(x > end_x || y>end_y)
                    {
                        break;
                    }
                    QRgb rgb(imglist[i].pixel(x, y));
                    QColor color(rgb);
    //                int r = datalist[i][(int)((y*resolution.x()+x)*4)];
                    if(color.red() > 30)
                    {
                        gray = (color.red()*19595 + color.green()*38469 + color.blue()*7472) >> 16;
//                        qDebug() << "gray" << gray;
                        bCurColorIsWhite = true;
                        break;
                    }else{
                        bCurColorIsWhite = false;
                    }
                }
                if(bCurColorIsWhite)
                {
                    if(sp.x() == -1 || sp.y() == -1)
                    {
                        sp.setX(x);
                        sp.setY(y);
                        gray2 = gray;
                    }
                    if(ep.x() == -1 || ep.y() == -1)
                    {
                        ep.setX(x);
                        ep.setY(y);
                    }else if(y - ep.y() == 1 && ep.x() == x && gray2 == gray){
                        ep.setY(y);
                    }else{
                        wd.sp = sp;
                        wd.ep = ep;
                        wd.gray = gray2;
                        resultlist.push_back(wd);
                        sp.setX(x);
                        sp.setY(y);
                        ep.setX(x);
                        ep.setY(y);
                        gray = 0;
                    }
                }
            }
            currentpos = x + y*width;
            if(currentpos > imagesize)
            {
                break;
            }
        }
    }
    wd.sp = sp;
    wd.ep = ep;
    wd.gray = 255;
    resultlist.push_back(wd);
    layerresult lr;
    lr.layerid = id;
    lr.layerimgdata = resultlist;
    resultlayer[id] = lr;
//    std::vector<struct whitedata>().swap(resultlist);
    qDebug() << "imglist.size" << imglist.size();
    qDebug() << "resultlayer.size" << resultlayer.size();
    while(resultlist.size()>0)
    {
        resultlist.erase(resultlist.begin());
    }
    while(imglist.size()>0)
    {
        imglist.erase(imglist.begin());
    }
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    qDebug() << "resultlist---endtime---" << id << current_date;
}

void Slice::generateByPreSliceCopy(unsigned int id, double att, QImage &floorimg)
{
    ModelData* md;
    std::vector<triangle> tl;
    std::vector<QImage> imglist;
    std::vector<MSupport> suplist;
    int star_x = resolution.x(), star_y = resolution.y(), end_x = 0, end_y = 0;

    for(int i = 0; i < mdlist.size(); i++)
    {
        md = mdlist[i];
        md->initGetNexy();
        suplist = md->supportlist;
        QVector3D thisP, thatP, mdoffset, mdposition, mdsize;
        float mdz = md->getOffset().z();
        mdoffset = md->getOffset();
        mdposition = md->getPosition();
        mdsize = md->getSize();
        int cmpcount = 0;
        cmpcount = 0;
        double xdisp, ydisp, zdisp, planefraction;
        std::vector<segment> segments, segments1;
        QVector<segment> qsegments;
        QList<segment> qlsegments;
        triangle t;
        segment sg;
        segment bsg, nsg;
        QVector2D points[2];
        double potentialDist;
        int finalid, mpid;
        double mindist = 100000.0;
        int waitid, nid;
        QVector2D pointp1, pointp2;
//        hash_map<int, std::string> hmlist;
        QMap<int, std::string> hmlist;
        int j = 0;
        int nextid = 0;
        while(md->getNextTri(t, att, nextid) && !this->mparent->isCancel && !canstop)
        {
            if (canstop){
                qDebug() << "while(md->isComplete";
                break;
            }
            cmpcount = 0;
            for(int v1 = 0; v1 < 3; v1++)
            {
                thisP = t.vertex[v1];

                if(thisP.z() <= att)
                {
                    continue;
                }
                for(int v2 = 0; v2 < 3; v2++){
                    if(v2 == v1)
                    {
                        continue;
                    }
                    thatP = t.vertex[v2];
                    if(thatP.z() > att)
                    {
                        continue;
                    }
                    //根据空间直线的对称式方程和参数方程算出新的顶点
                    xdisp = thatP.x() - thisP.x();
                    ydisp = thatP.y() - thisP.y();
                    zdisp = thatP.z() - thisP.z();
                    planefraction = (thisP.z() - att)/(fabs(zdisp));
                    points[cmpcount].setX(thisP.x() + xdisp*planefraction);
                    points[cmpcount].setY(thisP.y() + ydisp*planefraction);
                    cmpcount++;
                }
            }
            points[0].setX((points[0].x()+mdposition.x())/pixscale.x());//根据平台大小缩放
            points[0].setY((points[0].y()+mdposition.y())/pixscale.y());
            points[1].setX((points[1].x()+mdposition.x())/pixscale.x());
            points[1].setY((points[1].y()+mdposition.y())/pixscale.y());
            sg.p1 = points[0] + resolution/2; //resolution像素值，移到平台中间
            sg.p2 = points[1] + resolution/2;
            if(sg.p2.x() > end_x)  //限制不能超过平台xy
            {
                end_x = ceil(sg.p2.x())+1;
            }
            if(sg.p2.y() > end_y)
            {
                end_y = ceil(sg.p2.y())+1;
            }
            if(sg.p2.x() < star_x)
            {
                star_x = floor(sg.p2.x())-1;
            }
            if(sg.p2.y() < star_y)
            {
                star_y = floor(sg.p2.y())-1;
            }
            sg.normal.setX(t.normal.x());
            sg.normal.setY(t.normal.y());
            sg.normal.normalize();
            sg.trinormal.setX(t.normal.x());
            sg.trinormal.setY(t.normal.y());
            sg.trinormal.setZ(t.normal.z());
            sg.trinormal.normalize();
            sg.inloop = false;
            sg.linepoint = false;
            sg.frontid = -1;
            sg.backid = -1;
            sg.nowid = j;
            CorrectPointOrder(sg); //排序两点，由左到右
            segments.push_back(sg);
            mpid = floor((sg.p1.x()*100+0.5));
            std::ostringstream ss;
            ss << j;
//            hmlist[mpid] += "|"+ss.str();
            if (hmlist.contains(mpid)) {
                std::string list = "|" + ss.str();
                std::istringstream fa(hmlist[mpid]);
                std::string sa;
                while(std::getline(fa, sa, '|'))
                {
                    if(sa.empty())
                    {
                        continue;
                    }
                    list += "|" + sa;
                }
                hmlist.insert(mpid,list);
            }else {
                hmlist.insert(mpid,"|" + ss.str());
            }
            j++;
        }
        for(int supid=0;supid<suplist.size();supid++)
        {
            suplist[supid].setOP(mdoffset, mdposition);
            suplist[supid].initGetNexy();
            nextid = 0;
            while(suplist[supid].getNextTri(t, att, nextid))
            {
                cmpcount = 0;
                for(int v1 = 0; v1 < 3; v1++)
                {
                    thisP = t.vertex[v1];

                    if(thisP.z() <= att)
                    {
                        continue;
                    }
                    for(int v2 = 0; v2 < 3; v2++){
                        if(v2 == v1)
                        {
                            continue;
                        }
                        thatP = t.vertex[v2];
                        if(thatP.z() > att)
                        {
                            continue;
                        }
                        xdisp = thatP.x() - thisP.x();
                        ydisp = thatP.y() - thisP.y();
                        zdisp = thatP.z() - thisP.z();
                        planefraction = (thisP.z() - att)/(fabs(zdisp));
                        points[cmpcount].setX(thisP.x() + xdisp*planefraction);
                        points[cmpcount].setY(thisP.y() + ydisp*planefraction);
                        cmpcount++;
                    }
                }
                points[0].setX((points[0].x()+mdposition.x())/pixscale.x());
                points[0].setY((points[0].y()+mdposition.y())/pixscale.y());
                points[1].setX((points[1].x()+mdposition.x())/pixscale.x());
                points[1].setY((points[1].y()+mdposition.y())/pixscale.y());
                sg.p1 = points[0] + resolution/2;
                sg.p2 = points[1] + resolution/2;
                if(sg.p2.x() > end_x)
                {
                    end_x = ceil(sg.p2.x())+1;
                }
                if(sg.p2.y() > end_y)
                {
                    end_y = ceil(sg.p2.y())+1;
                }
                if(sg.p2.x() < star_x)
                {
                    star_x = floor(sg.p2.x())-1;
                }
                if(sg.p2.y() < star_y)
                {
                    star_y = floor(sg.p2.y())-1;
                }
                sg.normal.setX(t.normal.x());
                sg.normal.setY(t.normal.y());
                sg.normal.normalize();
                sg.trinormal.setX(t.normal.x());
                sg.trinormal.setY(t.normal.y());
                sg.trinormal.setZ(t.normal.z());
                sg.trinormal.normalize();
                sg.inloop = false;
                sg.linepoint = false;
                sg.frontid = -1;
                sg.backid = -1;
                sg.nowid = j;
                CorrectPointOrder(sg);
                segments.push_back(sg);
                mpid = floor((sg.p1.x()*100+0.5));
                std::ostringstream ss;
                ss << j;
//                hmlist[mpid] += "|"+ss.str();
                if (hmlist.contains(mpid)) {
                    std::string list = "|" + ss.str();
                    std::istringstream fa(hmlist[mpid]);
                    std::string sa;
                    while(std::getline(fa, sa, '|'))
                    {
                        if(sa.empty())
                        {
                            continue;
                        }
                        list += "|" + sa;
                    }
                    hmlist.insert(mpid,list);
                }else {
                    hmlist.insert(mpid,"|" + ss.str());
                }
                j++;
            }
        }
        for(int sgi = 0; sgi < segments.size(); sgi++)
        {//把线段相近点连接
            if(this->mparent->isCancel){
                qDebug() << "for(int sgi this->mparent->isCancel";
                break;
            }
            if(segments[sgi].frontid != -1 || segments[sgi].linepoint)
            {
                continue;
            }
            mpid = floor((segments[sgi].p2.x()*100+0.5));
            mpid -= 3;
            mindist = 100000.0;
            finalid = -1;
            for(int nearpoint = 0; nearpoint < 7; nearpoint++)
            {
                if (hmlist[mpid+nearpoint].empty())
                {
                    continue;
                }
                std::istringstream f(hmlist[mpid+nearpoint]);
                std::string s;
                while(std::getline(f, s, '|'))
                {
                    if(s.empty())
                    {
                        continue;
                    }
                    nid = atoi(s.c_str());
                    if(nid == sgi || segments[nid].backid != -1)
                    {
                        continue;
                    }
                    potentialDist = distance2D(segments[sgi].p2, segments[nid].p1);
                    if(potentialDist < mindist && potentialDist <= 0.03)
                    {
                        mindist = potentialDist;
                        finalid = nid;
                    }
                }
            }
            if(finalid != -1)
            {
                segments[sgi].frontid = finalid;
                segments[finalid].backid = sgi;
                segments[sgi].p2 = segments[finalid].p1;
                pointp1 = segments[sgi].p1;
                pointp2 = segments[sgi].p2;
                double dx = pointp2.x() - pointp1.x();
                double dy = pointp2.y() - pointp1.y();
                segments[sgi].normal.setX(-dy);
                segments[sgi].normal.setY(dx);
                segments[sgi].normal.normalize();
            }
        }
        std::vector<loop> looplist;
        OutPutData op = OutPutData(segments, id);
        looplist = op.looplist;
//        std::sort(looplist.begin(), looplist.end(), colorcompare);
        floorimg.fill(QColor(0, 0, 0));
        QPainter fp;//填充颜色，根据向量方向判断，
//        while(tl.size()>0)
//        {
//            tl.erase(tl.begin()); //erase使用此方法清空可释放内存
//        }
//        while(segments.size()>0)
//        {
//            segments.erase(segments.begin());
//        }
//        segments.clear();
        fp.begin(&floorimg);
        fp.setRenderHint(QPainter::Antialiasing, true);
        fp.setPen(QPen(QColor(255, 255, 255), 0));
        QRgb q1, q2;
        for(int i = 0; i < looplist.size(); i++)
        {
            if(looplist[i].fillcolor.red() != 255)
            {
                q1 = floorimg.pixel(looplist[i].maxsize.x(), looplist[i].maxsize.y());
                if(qRed(q1) == 255)
                {
                    fp.setBrush(looplist[i].fillcolor);
                    fp.drawPath(looplist[i].mpath);
                }else{
                    fp.setBrush(QColor(255, 255, 255));
                    fp.drawPath(looplist[i].mpath);
                }
            }else{
                fp.setBrush(looplist[i].fillcolor);
                fp.drawPath(looplist[i].mpath);
            }
        }
//        floorimg.save("H:floorimgtest001.jpg","JPG");
//        count++;
//        floorimg.save("H:/dlppng/img"+QString::number(count)+".jpg","JPG");
//        qDebug() << "imglist()" << id << i;
        floorimg.save("H:/dlppng/"+QString::number(id)+"_"+QString::number(i)+".png","PNG");
//        imglist.push_back(floorimg); //floorimg每一层的图片
        fp.end();
//        while(looplist.size()>0)
//        {
//            looplist.erase(looplist.begin());
//        }
    }
//    count++;
//    qDebug() << "imglist.size()" << imglist.size() << id;
//    for (int i=0;i<imglist.size();i++){
//        qDebug() << "imglist.size()" << id << i;
//        imglist[i].save("H:/dlppng/"+QString::number(id)+"_"+QString::number(i)+".jpg","JPG");
//        QApplication::processEvents();
//    }


//   //把图片转为坐标点
//    if(star_x < 0)
//    {
//        star_x = 0;
//    }
//    if(star_y < 0)
//    {
//        star_y = 0;
//    }
//    if(end_x < 0)
//    {
//        end_x = 0;
//    }
//    if(end_y < 0)
//    {
//        end_y = 0;
//    }
//    if(end_x > resolution.x())
//    {
//        end_x = resolution.x();
//    }
//    if(end_y > resolution.y())
//    {
//        end_y = resolution.y();
//    }
//    whitedata wd;
//    QVector2D sp(-1, -1), ep(-1, -1);
//    int gray = 0;
//    int gray2 = 0;
//    std::vector<whitedata> resultlist;
//    int x, y, width;
//    width = resolution.x();
//    bool bCurColorIsWhite = false;
//    int imagesize = end_x+end_y*width;
//    unsigned currentpos = star_x + star_y*width;
//    if(star_x > end_x || star_y > end_y)
//    {
//        currentpos = imagesize -1;
//        star_x = end_x;
//        star_y = end_y;
//    }
//    std::vector<uchar*> datalist;
////    for(int i =0;i<imglist.size();i++)
////    {
////        datalist.push_back(imglist[i].bits());
////    }

//    //此时已经是镜像状态
//    if(slicetype == 1){
//        //x正方向
//        for(int h = end_x-1; h > star_x-1; h--)
//        {
//            for(int v = star_y; v<end_y; v++)
//            {
//                x = h;
//                y = v;
//                for(int i = 0; i < imglist.size(); i++)
//                {
//                    if(x > end_x || y>end_y)
//                    {
//                        break;
//                    }
//                    QRgb rgb(imglist[i].pixel(x, y));
//                    QColor color(rgb);
//    //                int r = datalist[i][(int)((y*resolution.x()+x)*4)];
//                    gray = (color.red()*19595 + color.green()*38469 + color.blue()*7472) >> 16;
//                    gray = mapNumbers(gray,0);
//                    if(gray != 0)
//                    {
////                        qDebug() << "gray" << gray;
//                        bCurColorIsWhite = true;
//                        break;
//                    }else{
//                        bCurColorIsWhite = false;
//                    }
//                }
//                if(bCurColorIsWhite)
//                {
//                    if(slicetype == 1)
//                    {
//                        x = resolution.x() - x;
//                    }
//                    //20191108 现在的切片数据格式（stary.endy,endx）连线方式
//                    if(sp.x() == -1 || sp.y() == -1)
//                    {
//                        sp.setX(x);
//                        sp.setY(y);
//                        gray2 = gray;
//                    }
//                    if(ep.x() == -1 || ep.y() == -1)
//                    {
//                        ep.setX(x);
//                        ep.setY(y);
//                        gray2 = gray;
//                    }else if(y - ep.y() == 1 && ep.x() == x && gray == gray2){
////                        qDebug() << "ep.setx==" << x;
////                        qDebug() << "ep.setY==" << y;
//                        ep.setY(y);
//                    }else{
////                        qDebug() << "end------------";
//                        wd.sp = sp;
//                        wd.ep = ep;
//                        if (wd.ep.y() - wd.sp.y() > 1) {
//                            wd.gray = 15;
//                        }else {
//                            wd.gray = gray;
//                        }
//                        gray2 = gray;
//                        resultlist.push_back(wd);
//                        sp.setX(x);
//                        sp.setY(y);
//                        ep.setX(x);
//                        ep.setY(y);
//                    }
//                    // 20191108如果需要改为（x,y,灰度）这样的格式，
//                    //只需要在此计算灰度值，不为0即符合
//                    //但此格式会使数据量更大

//                }
//            }
//            currentpos = x + y*width;
//            if(currentpos > imagesize)
//            {
//                break;
//            }
//        }
//    }else{
//        //x镜像
//        for(int h = star_x; h < end_x; h++)
//        {
//            for(int v = star_y; v<end_y; v++)
//            {
//                x = h;
//                y = v;
//                for(int i = 0; i < imglist.size(); i++)
//                {
//                    if(x > end_x || y>end_y)
//                    {
//                        break;
//                    }
//                    QRgb rgb(imglist[i].pixel(x, y));
//                    QColor color(rgb);
//    //                int r = datalist[i][(int)((y*resolution.x()+x)*4)];
//                    if(color.red() > 30)
//                    {
//                        gray = (color.red()*19595 + color.green()*38469 + color.blue()*7472) >> 16;
////                        qDebug() << "gray" << gray;
//                        bCurColorIsWhite = true;
//                        break;
//                    }else{
//                        bCurColorIsWhite = false;
//                    }
//                }
//                if(bCurColorIsWhite)
//                {
//                    if(sp.x() == -1 || sp.y() == -1)
//                    {
//                        sp.setX(x);
//                        sp.setY(y);
//                        gray2 = gray;
//                    }
//                    if(ep.x() == -1 || ep.y() == -1)
//                    {
//                        ep.setX(x);
//                        ep.setY(y);
//                    }else if(y - ep.y() == 1 && ep.x() == x && gray2 == gray){
//                        ep.setY(y);
//                    }else{
//                        wd.sp = sp;
//                        wd.ep = ep;
//                        wd.gray = gray2;
//                        resultlist.push_back(wd);
//                        sp.setX(x);
//                        sp.setY(y);
//                        ep.setX(x);
//                        ep.setY(y);
//                        gray = 0;
//                    }
//                }
//            }
//            currentpos = x + y*width;
//            if(currentpos > imagesize)
//            {
//                break;
//            }
//        }
//    }
//    wd.sp = sp;
//    wd.ep = ep;
//    wd.gray = 255;
//    resultlist.push_back(wd);
//    layerresult lr;
//    lr.layerid = id;
//    lr.layerimgdata = resultlist;
//    resultlayer[id] = lr;
////    std::vector<struct whitedata>().swap(resultlist);
////    qDebug() << "imglist.size" << imglist.size();
////    qDebug() << "resultlist.size" << resultlist.size();
////    while(resultlist.size()>0)
////    {
////        resultlist.erase(resultlist.begin());
////    }
////    while(imglist.size()>0)
////    {
////        imglist.erase(imglist.begin());
////    }
////    QDateTime current_date_time =QDateTime::currentDateTime();
////    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
////    qDebug() << "resultlist---endtime---" << id << current_date;
}


void Slice::newGenerateByPreSlice(unsigned int id, double att, QImage &floorimg)
{
    //边切片边保存
    ModelData* md;
    std::vector<triangle> tl;
    std::vector<QImage> imglist;
    imglist.reserve(mdlist.size());
    std::vector<MSupport> suplist;
    int star_x = resolution.x(), star_y = resolution.y(), end_x = 0, end_y = 0;
    QImage img = this->baseimg;
//    qDebug() << "resolution--1----" << resolution.x() << resolution.y();
//    qDebug() << "img--1----" << img.width() << img.height() << id;
//    img.scaled(resolution.x(),resolution.y(),Qt::KeepAspectRatio);
//    qDebug() << "img--2----" << img.width() << img.height();
    for(int i = 0; i < mdlist.size(); i++)
    {
        md = mdlist[i];
        md->initGetNexy();
        suplist = md->supportlist;
        QVector3D thisP, thatP, mdoffset, mdposition, mdsize;
        float mdz = md->getOffset().z();
        mdoffset = md->getOffset();
        mdposition = md->getPosition();
        mdsize = md->getSize();
        int cmpcount = 0;
        cmpcount = 0;
        double xdisp, ydisp, zdisp, planefraction;
        std::vector<segment> segments, segments1;
        QVector<segment> qsegments;
        QList<segment> qlsegments;
        triangle t;
        segment sg;
        segment bsg, nsg;
        QVector2D points[2];
        double potentialDist;
        int finalid, mpid;
        double mindist = 100000.0;
        int waitid, nid;
        QVector2D pointp1, pointp2;
//        hash_map<int, std::string> hmlist;
        QMap<int, std::string> hmlist;
        int j = 0;
        int nextid = 0;
        while(md->getNextTri(t, att, nextid) && !this->mparent->isCancel && !canstop)
        {
//            qDebug() << "nextid ==0 " << nextid;
            if (canstop){
                qDebug() << "while(md->isComplete";
                break;
            }
            cmpcount = 0;
            for(int v1 = 0; v1 < 3; v1++)
            {
                thisP = t.vertex[v1];

                if(thisP.z() <= att)
                {
                    continue;
                }
                for(int v2 = 0; v2 < 3; v2++){
                    if(v2 == v1)
                    {
                        continue;
                    }
                    thatP = t.vertex[v2];
                    if(thatP.z() > att)
                    {
                        continue;
                    }
                    //根据空间直线的对称式方程和参数方程算出新的顶点
                    xdisp = thatP.x() - thisP.x();
                    ydisp = thatP.y() - thisP.y();
                    zdisp = thatP.z() - thisP.z();
                    planefraction = (thisP.z() - att)/(fabs(zdisp));
                    points[cmpcount].setX(thisP.x() + xdisp*planefraction);
                    points[cmpcount].setY(thisP.y() + ydisp*planefraction);
                    cmpcount++;
                }
            }
//            qDebug() << "nextid ==1 " << nextid;
            points[0].setX((points[0].x()+mdposition.x())/pixscale.x());//根据平台大小缩放
            points[0].setY((points[0].y()+mdposition.y())/pixscale.y());
            points[1].setX((points[1].x()+mdposition.x())/pixscale.x());
            points[1].setY((points[1].y()+mdposition.y())/pixscale.y());
            sg.p1 = points[0] + resolution/2; //resolution像素值，移到平台中间
            sg.p2 = points[1] + resolution/2;
            if(sg.p2.x() > end_x)  //限制不能超过平台xy
            {
                end_x = ceil(sg.p2.x())+1;
            }
            if(sg.p2.y() > end_y)
            {
                end_y = ceil(sg.p2.y())+1;
            }
            if(sg.p2.x() < star_x)
            {
                star_x = floor(sg.p2.x())-1;
            }
            if(sg.p2.y() < star_y)
            {
                star_y = floor(sg.p2.y())-1;
            }
            sg.normal.setX(t.normal.x());
            sg.normal.setY(t.normal.y());
            sg.normal.normalize();
            sg.trinormal.setX(t.normal.x());
            sg.trinormal.setY(t.normal.y());
            sg.trinormal.setZ(t.normal.z());
            sg.trinormal.normalize();
            sg.inloop = false;
            sg.linepoint = false;
            sg.frontid = -1;
            sg.backid = -1;
            sg.nowid = j;
//            qDebug() << "nextid ==2 " << nextid;
            CorrectPointOrder(sg); //排序两点，由左到右
//            qDebug() << "nextid ==3 " << nextid;
            segments.push_back(sg);
            mpid = floor((sg.p1.x()*100+0.5));
            std::ostringstream ss;
            ss << j;
//            hmlist[mpid] += "|"+ss.str();
            if (hmlist.contains(mpid)) {
                std::string list = "|" + ss.str();
                std::istringstream fa(hmlist[mpid]);
                std::string sa;
                while(std::getline(fa, sa, '|'))
                {
                    if(sa.empty())
                    {
                        continue;
                    }
                    list += "|" + sa;
                }
                hmlist.insert(mpid,list);
            }else {
                hmlist.insert(mpid,"|" + ss.str());
            }
            j++;
        }

        for(int supid=0;supid<suplist.size();supid++)
        {
            suplist[supid].setOP(mdoffset, mdposition);
            suplist[supid].initGetNexy();
            nextid = 0;
            while(suplist[supid].getNextTri(t, att, nextid))
            {
                cmpcount = 0;
                for(int v1 = 0; v1 < 3; v1++)
                {
                    thisP = t.vertex[v1];

                    if(thisP.z() <= att)
                    {
                        continue;
                    }
                    for(int v2 = 0; v2 < 3; v2++){
                        if(v2 == v1)
                        {
                            continue;
                        }
                        thatP = t.vertex[v2];
                        if(thatP.z() > att)
                        {
                            continue;
                        }
                        xdisp = thatP.x() - thisP.x();
                        ydisp = thatP.y() - thisP.y();
                        zdisp = thatP.z() - thisP.z();
                        planefraction = (thisP.z() - att)/(fabs(zdisp));
                        points[cmpcount].setX(thisP.x() + xdisp*planefraction);
                        points[cmpcount].setY(thisP.y() + ydisp*planefraction);
                        cmpcount++;
                    }
                }
                points[0].setX((points[0].x()+mdposition.x())/pixscale.x());
                points[0].setY((points[0].y()+mdposition.y())/pixscale.y());
                points[1].setX((points[1].x()+mdposition.x())/pixscale.x());
                points[1].setY((points[1].y()+mdposition.y())/pixscale.y());
                sg.p1 = points[0] + resolution/2;
                sg.p2 = points[1] + resolution/2;
                if(sg.p2.x() > end_x)
                {
                    end_x = ceil(sg.p2.x())+1;
                }
                if(sg.p2.y() > end_y)
                {
                    end_y = ceil(sg.p2.y())+1;
                }
                if(sg.p2.x() < star_x)
                {
                    star_x = floor(sg.p2.x())-1;
                }
                if(sg.p2.y() < star_y)
                {
                    star_y = floor(sg.p2.y())-1;
                }
                sg.normal.setX(t.normal.x());
                sg.normal.setY(t.normal.y());
                sg.normal.normalize();
                sg.trinormal.setX(t.normal.x());
                sg.trinormal.setY(t.normal.y());
                sg.trinormal.setZ(t.normal.z());
                sg.trinormal.normalize();
                sg.inloop = false;
                sg.linepoint = false;
                sg.frontid = -1;
                sg.backid = -1;
                sg.nowid = j;
                CorrectPointOrder(sg);
                segments.push_back(sg);
                mpid = floor((sg.p1.x()*100+0.5));
                std::ostringstream ss;
                ss << j;
//                hmlist[mpid] += "|"+ss.str();
                if (hmlist.contains(mpid)) {
                    std::string list = "|" + ss.str();
                    std::istringstream fa(hmlist[mpid]);
                    std::string sa;
                    while(std::getline(fa, sa, '|'))
                    {
                        if(sa.empty())
                        {
                            continue;
                        }
                        list += "|" + sa;
                    }
                    hmlist.insert(mpid,list);
                }else {
                    hmlist.insert(mpid,"|" + ss.str());
                }
                j++;
            }
        }

        for(int sgi = 0; sgi < segments.size(); sgi++)
        {//把线段相近点连接
            if(this->mparent->isCancel){
                qDebug() << "for(int sgi this->mparent->isCancel";
                break;
            }
            if(segments[sgi].frontid != -1 || segments[sgi].linepoint)
            {
                continue;
            }
            mpid = floor((segments[sgi].p2.x()*100+0.5));
            mpid -= 3;
            mindist = 100000.0;
            finalid = -1;
            for(int nearpoint = 0; nearpoint < 7; nearpoint++)
            {
                if (hmlist[mpid+nearpoint].empty())
                {
                    continue;
                }
                std::istringstream f(hmlist[mpid+nearpoint]);
                std::string s;
                while(std::getline(f, s, '|'))
                {
                    if(s.empty())
                    {
                        continue;
                    }
                    nid = atoi(s.c_str());
                    if(nid == sgi || segments[nid].backid != -1)
                    {
                        continue;
                    }
                    potentialDist = distance2D(segments[sgi].p2, segments[nid].p1);
                    if(potentialDist < mindist && potentialDist <= 0.03)
                    {
                        mindist = potentialDist;
                        finalid = nid;
                    }
                }
            }
            if(finalid != -1)
            {
                segments[sgi].frontid = finalid;
                segments[finalid].backid = sgi;
                segments[sgi].p2 = segments[finalid].p1;
                pointp1 = segments[sgi].p1;
                pointp2 = segments[sgi].p2;
                double dx = pointp2.x() - pointp1.x();
                double dy = pointp2.y() - pointp1.y();
                segments[sgi].normal.setX(-dy);
                segments[sgi].normal.setY(dx);
                segments[sgi].normal.normalize();
            }
        }

        std::vector<loop> looplist;
        OutPutData op = OutPutData(segments, id);
        looplist = op.looplist;
//        std::sort(looplist.begin(), looplist.end(), colorcompare);
//        floorimg.fill(QColor(0, 0, 0));
        img.fill(QColor(0, 0, 0));

        QPainter fp;//填充颜色，根据向量方向判断，
//        while(tl.size()>0)
//        {
//            tl.erase(tl.begin()); //erase使用此方法清空可释放内存
//        }
//        while(segments.size()>0)
//        {
//            segments.erase(segments.begin());
//        }
//        segments.clear();
//        fp.begin(&floorimg);
//        img.save("H:/dlppng/000.png","PNG");
//        qDebug() << "img.width=begin====" << img.width() << img.height() << id;
        fp.begin(&img);
        fp.setRenderHint(QPainter::Antialiasing, true);
        fp.setPen(QPen(QColor(255, 255, 255), 0));
        QRgb q1, q2;
//        qDebug() << looplist.size() << looplist.size();
        for(int i = 0; i < looplist.size(); i++)
        {
            if(looplist[i].fillcolor.red() != 255)
            {
                q1 = img.pixel(looplist[i].maxsize.x(), looplist[i].maxsize.y());
                if(qRed(q1) == 255)
                {
                    fp.setBrush(looplist[i].fillcolor);
                    fp.drawPath(looplist[i].mpath);
                }else{
                    fp.setBrush(QColor(255, 255, 255));
                    fp.drawPath(looplist[i].mpath);
                }
            }else{
                fp.setBrush(looplist[i].fillcolor);
                fp.drawPath(looplist[i].mpath);
            }
        }
//        floorimg.save("H:floorimgtest001.jpg","JPG");
//        count++;
//        floorimg.save("H:/dlppng/img"+QString::number(count)+".jpg","JPG");
//        qDebug() << "imglist()" << id << i;
//        floorimg.save("H:/dlppng/"+QString::number(id)+"_"+QString::number(i)+".jpg","JPG");
//        qDebug() << "imglist.push_back(img)-------" << img.width() << img.height() << id;
//        img.save("H:/dlppng/"+QString::number(id)+".png","PNG");
//        imglist.push_back(floorimg); //floorimg每一层的图片
        imglist.push_back(img); //floorimg每一层的图片
//        qDebug() << "imglist.push_back(img)-------" << img.width() << img.height() << id;
        fp.end();
//        while(looplist.size()>0)
//        {
//            looplist.erase(looplist.begin());
//        }
//        qDebug() << "fp == ok";
    }

   //把图片转为坐标点
    if(star_x < 0)
    {
        star_x = 0;
    }
    if(star_y < 0)
    {
        star_y = 0;
    }
    if(end_x < 0)
    {
        end_x = 0;
    }
    if(end_y < 0)
    {
        end_y = 0;
    }
    if(end_x > resolution.x())
    {
        end_x = resolution.x();
    }
    if(end_y > resolution.y())
    {
        end_y = resolution.y();
    }
    whitedata wd;
    QVector2D sp(-1, -1), ep(-1, -1);
    int gray = 0;
    int gray2 = 0;
    std::vector<whitedata> resultlist;
    int x, y, width;
    width = resolution.x();
    bool bCurColorIsWhite = false;
    int imagesize = end_x+end_y*width;
    unsigned currentpos = star_x + star_y*width;
    if(star_x > end_x || star_y > end_y)
    {
        currentpos = imagesize -1;
        star_x = end_x;
        star_y = end_y;
    }
    std::vector<uchar*> datalist;
//    for(int i =0;i<imglist.size();i++)
//    {
//        datalist.push_back(imglist[i].bits());
//    }
//    qDebug() << "change == resultlist";

    //此时已经是镜像状态
    if(slicetype == 1){
        //x正方向
        for(int h = end_x-1; h > star_x-1; h--)
        {
            for(int v = star_y; v<end_y; v++)
            {
                x = h;
                y = v;
                for(int i = 0; i < imglist.size(); i++)
                {
                    if(x > end_x || y>end_y)
                    {
                        break;
                    }
                    if (imglist[i].width() < x || imglist[i].height() < y) {
                        gray = 0;
                    }else {
                        QRgb rgb(imglist[i].pixel(x, y));
                        QColor color(rgb);
                        gray = (color.red()*19595 + color.green()*38469 + color.blue()*7472) >> 16;
                    }
                    gray = mapNumbers(gray,0);
                    if(gray != 0)
                    {
                        bCurColorIsWhite = true;
                        break;
                    }else{
                        bCurColorIsWhite = false;
                    }
                }
                if(bCurColorIsWhite)
                {
                    if(slicetype == 1)
                    {
                        x = resolution.x() - x;
                    }
                    //20191108 现在的切片数据格式（stary.endy,endx）连线方式
                    if(sp.x() == -1 || sp.y() == -1)
                    {
                        sp.setX(x);
                        sp.setY(y);
                        gray2 = gray;
                    }
                    if(ep.x() == -1 || ep.y() == -1)
                    {
                        ep.setX(x);
                        ep.setY(y);
                        gray2 = gray;
                    }else if(y - ep.y() == 1 && ep.x() == x && gray == gray2){
//                        qDebug() << "ep.setx==" << x;
//                        qDebug() << "ep.setY==" << y;
                        ep.setY(y);
                    }else{
//                        qDebug() << "end------------";
                        wd.sp = sp;
                        wd.ep = ep;
                        if (wd.ep.y() - wd.sp.y() > 1) {
                            wd.gray = 15;
                        }else {
                            wd.gray = gray;
                        }
                        gray2 = gray;
                        resultlist.push_back(wd);
                        sp.setX(x);
                        sp.setY(y);
                        ep.setX(x);
                        ep.setY(y);
                    }
                    // 20191108如果需要改为（x,y,灰度）这样的格式，
                    //只需要在此计算灰度值，不为0即符合
                    //但此格式会使数据量更大

                }
            }
            currentpos = x + y*width;
            if(currentpos > imagesize)
            {
                break;
            }
        }
    }else{
        //x镜像
        for(int h = star_x; h < end_x; h++)
        {
            for(int v = star_y; v<end_y; v++)
            {
                x = h;
                y = v;
                for(int i = 0; i < imglist.size(); i++)
                {
                    if(x > end_x || y>end_y)
                    {
                        break;
                    }
                    QRgb rgb(imglist[i].pixel(x, y));
                    QColor color(rgb);
    //                int r = datalist[i][(int)((y*resolution.x()+x)*4)];
                    if(color.red() > 30)
                    {
                        gray = (color.red()*19595 + color.green()*38469 + color.blue()*7472) >> 16;
//                        qDebug() << "gray" << gray;
                        bCurColorIsWhite = true;
                        break;
                    }else{
                        bCurColorIsWhite = false;
                    }
                }
                if(bCurColorIsWhite)
                {
                    if(sp.x() == -1 || sp.y() == -1)
                    {
                        sp.setX(x);
                        sp.setY(y);
                        gray2 = gray;
                    }
                    if(ep.x() == -1 || ep.y() == -1)
                    {
                        ep.setX(x);
                        ep.setY(y);
                    }else if(y - ep.y() == 1 && ep.x() == x && gray2 == gray){
                        ep.setY(y);
                    }else{
                        wd.sp = sp;
                        wd.ep = ep;
                        wd.gray = gray2;
                        resultlist.push_back(wd);
                        sp.setX(x);
                        sp.setY(y);
                        ep.setX(x);
                        ep.setY(y);
                        gray = 0;
                    }
                }
            }
            currentpos = x + y*width;
            if(currentpos > imagesize)
            {
                break;
            }
        }
    }
    wd.sp = sp;
    wd.ep = ep;
    wd.gray = 15;
    resultlist.push_back(wd);
    layerresult lr;
    lr.layerid = id;
    lr.layerimgdata = resultlist;
    resultlayer[id] = lr;
    while(imglist.size()>0)
    {
        imglist.erase(imglist.begin());
    }
}

void Slice:: writeFile(){
    iswriting = true;
    QFile* pf = new QFile(saveFileName);
    pf->open(QIODevice::WriteOnly|QIODevice::Append);
    QDataStream out(pf);
    std::vector<whitedata> eachlayer;
    whitedata eachline;
    if (nowLayerid >= max_size) {
        return;
    }
    while(resultlayer[nowLayerid].layerimgdata.size() > 0){
        eachlayer = resultlayer[nowLayerid].layerimgdata;
        out << (quint32)eachlayer.size();
        for(int j = 0; j < eachlayer.size(); j++)
        {
            eachline = eachlayer[j];
            out << (quint16)eachline.sp.y() << (quint16)eachline.ep.y() << (quint16)eachline.ep.x() << (quint8)eachline.gray;
        }
        out << (quint8)0x0D << (quint8)0x0A;
        layerresult lr;
        resultlayer[nowLayerid] = lr;
        nowLayerid++;
        if (nowLayerid == max_size) {
            out << "MKSDLP" ;
        }
        if (nowLayerid >= max_size) {
            break;
        }
    }
    pf->close();
    delete pf;
    iswriting = false;
}

void Slice::newGenerateByPreSlicecopy(unsigned int id, double att, QImage &floorimg)
{
    //使用旧格式
    //边切片边保存
    ModelData* md;
    std::vector<triangle> tl;
    std::vector<QImage> imglist;
    imglist.reserve(mdlist.size());
    std::vector<MSupport> suplist;
    int star_x = resolution.x(), star_y = resolution.y(), end_x = 0, end_y = 0;
    QImage img = this->baseimg;
    for(int i = 0; i < mdlist.size(); i++)
    {
        md = mdlist[i];
        md->initGetNexy();
        suplist = md->supportlist;
        QVector3D thisP, thatP, mdoffset, mdposition, mdsize;
        float mdz = md->getOffset().z();
        mdoffset = md->getOffset();
        mdposition = md->getPosition();
        mdsize = md->getSize();
        int cmpcount = 0;
        cmpcount = 0;
        double xdisp, ydisp, zdisp, planefraction;
        std::vector<segment> segments, segments1;
        QVector<segment> qsegments;
        QList<segment> qlsegments;
        triangle t;
        segment sg;
        segment bsg, nsg;
        QVector2D points[2];
        double potentialDist;
        int finalid, mpid;
        double mindist = 100000.0;
        int waitid, nid;
        QVector2D pointp1, pointp2;
//        hash_map<int, std::string> hmlist;
        QMap<int, std::string> hmlist;
        int j = 0;
        int nextid = 0;
        while(md->getNextTri(t, att, nextid) && !this->mparent->isCancel && !canstop)
        {
            if (canstop){
                break;
            }
            if (mparent->isCancel) {
                break;
            }
            cmpcount = 0;
            for(int v1 = 0; v1 < 3; v1++)
            {
                thisP = t.vertex[v1];

                if(thisP.z() <= att)
                {
                    continue;
                }
                for(int v2 = 0; v2 < 3; v2++){
                    if(v2 == v1)
                    {
                        continue;
                    }
                    thatP = t.vertex[v2];
                    if(thatP.z() > att)
                    {
                        continue;
                    }
                    //根据空间直线的对称式方程和参数方程算出新的顶点
                    xdisp = thatP.x() - thisP.x();
                    ydisp = thatP.y() - thisP.y();
                    zdisp = thatP.z() - thisP.z();
                    planefraction = (thisP.z() - att)/(fabs(zdisp));
                    points[cmpcount].setX(thisP.x() + xdisp*planefraction);
                    points[cmpcount].setY(thisP.y() + ydisp*planefraction);
                    cmpcount++;
                }
            }
            points[0].setX((points[0].x()+mdposition.x())/pixscale.x());//根据平台大小缩放
            points[0].setY((points[0].y()+mdposition.y())/pixscale.y());
            points[1].setX((points[1].x()+mdposition.x())/pixscale.x());
            points[1].setY((points[1].y()+mdposition.y())/pixscale.y());
            sg.p1 = points[0] + resolution/2; //resolution像素值，移到平台中间
            sg.p2 = points[1] + resolution/2;
            if(sg.p2.x() > end_x)  //限制不能超过平台xy
            {
                end_x = ceil(sg.p2.x())+1;
            }
            if(sg.p2.y() > end_y)
            {
                end_y = ceil(sg.p2.y())+1;
            }
            if(sg.p2.x() < star_x)
            {
                star_x = floor(sg.p2.x())-1;
            }
            if(sg.p2.y() < star_y)
            {
                star_y = floor(sg.p2.y())-1;
            }
            sg.normal.setX(t.normal.x());
            sg.normal.setY(t.normal.y());
            sg.normal.normalize();
            sg.trinormal.setX(t.normal.x());
            sg.trinormal.setY(t.normal.y());
            sg.trinormal.setZ(t.normal.z());
            sg.trinormal.normalize();
            sg.inloop = false;
            sg.linepoint = false;
            sg.frontid = -1;
            sg.backid = -1;
            sg.nowid = j;
            CorrectPointOrder(sg); //排序两点，由左到右
            segments.push_back(sg);
            mpid = floor((sg.p1.x()*100+0.5));
            std::ostringstream ss;
            ss << j;
            if (hmlist.contains(mpid)) {
                std::string list = "|" + ss.str();
                std::istringstream fa(hmlist[mpid]);
                std::string sa;
                while(std::getline(fa, sa, '|'))
                {
                    if(sa.empty())
                    {
                        continue;
                    }
                    list += "|" + sa;
                }
                hmlist.insert(mpid,list);
            }else {
                hmlist.insert(mpid,"|" + ss.str());
            }
            j++;
        }
        for(int supid=0;supid<suplist.size();supid++)
        {
            suplist[supid].setOP(mdoffset, mdposition);
            suplist[supid].initGetNexy();
            nextid = 0;
            while(suplist[supid].getNextTri(t, att, nextid))
            {
                cmpcount = 0;
                for(int v1 = 0; v1 < 3; v1++)
                {
                    thisP = t.vertex[v1];

                    if(thisP.z() <= att)
                    {
                        continue;
                    }
                    for(int v2 = 0; v2 < 3; v2++){
                        if(v2 == v1)
                        {
                            continue;
                        }
                        thatP = t.vertex[v2];
                        if(thatP.z() > att)
                        {
                            continue;
                        }
                        xdisp = thatP.x() - thisP.x();
                        ydisp = thatP.y() - thisP.y();
                        zdisp = thatP.z() - thisP.z();
                        planefraction = (thisP.z() - att)/(fabs(zdisp));
                        points[cmpcount].setX(thisP.x() + xdisp*planefraction);
                        points[cmpcount].setY(thisP.y() + ydisp*planefraction);
                        cmpcount++;
                    }
                }
                points[0].setX((points[0].x()+mdposition.x())/pixscale.x());
                points[0].setY((points[0].y()+mdposition.y())/pixscale.y());
                points[1].setX((points[1].x()+mdposition.x())/pixscale.x());
                points[1].setY((points[1].y()+mdposition.y())/pixscale.y());
                sg.p1 = points[0] + resolution/2;
                sg.p2 = points[1] + resolution/2;
                if(sg.p2.x() > end_x)
                {
                    end_x = ceil(sg.p2.x())+1;
                }
                if(sg.p2.y() > end_y)
                {
                    end_y = ceil(sg.p2.y())+1;
                }
                if(sg.p2.x() < star_x)
                {
                    star_x = floor(sg.p2.x())-1;
                }
                if(sg.p2.y() < star_y)
                {
                    star_y = floor(sg.p2.y())-1;
                }
                sg.normal.setX(t.normal.x());
                sg.normal.setY(t.normal.y());
                sg.normal.normalize();
                sg.trinormal.setX(t.normal.x());
                sg.trinormal.setY(t.normal.y());
                sg.trinormal.setZ(t.normal.z());
                sg.trinormal.normalize();
                sg.inloop = false;
                sg.linepoint = false;
                sg.frontid = -1;
                sg.backid = -1;
                sg.nowid = j;
                CorrectPointOrder(sg);
                segments.push_back(sg);
                mpid = floor((sg.p1.x()*100+0.5));
                std::ostringstream ss;
                ss << j;
//                   hmlist[mpid] += "|" + ss.str();
                if (hmlist.contains(mpid)) {
                    std::string list = "|" + ss.str();
                    std::istringstream fa(hmlist[mpid]);
                    std::string sa;
                    while(std::getline(fa, sa, '|'))
                    {
                        if(sa.empty())
                        {
                            continue;
                        }
                        list += "|" + sa;
                    }
                    hmlist.insert(mpid,list);
                }else {
                    hmlist.insert(mpid,"|" + ss.str());
                }
                j++;
            }
        }
        for(int sgi = 0; sgi < segments.size(); sgi++)
        {//把线段相近点连接
            if(this->mparent->isCancel){
                break;
            }
            if(segments[sgi].frontid != -1 || segments[sgi].linepoint)
            {
                continue;
            }
            mpid = floor((segments[sgi].p2.x()*100+0.5));
            mpid -= 3;
            mindist = 100000.0;
            finalid = -1;
            for(int nearpoint = 0; nearpoint < 7; nearpoint++)
            {
//                if (hmlist[mpid+nearpoint].empty())
//                {
//                    continue;
//                }
                if (!hmlist.contains(mpid+nearpoint)) {
                    continue;
                }
                std::istringstream f(hmlist[mpid+nearpoint]);
                std::string s;
                while(std::getline(f, s, '|'))
                {
                    if(s.empty())
                    {
                        continue;
                    }
                    nid = atoi(s.c_str());
                    if(nid == sgi || segments[nid].backid != -1)
                    {
                        continue;
                    }
                    potentialDist = distance2D(segments[sgi].p2, segments[nid].p1);
                    if(potentialDist < mindist && potentialDist <= 0.03)
                    {
                        mindist = potentialDist;
                        finalid = nid;
                    }
                }
            }
            if(finalid != -1)
            {
                segments[sgi].frontid = finalid;
                segments[finalid].backid = sgi;
                segments[sgi].p2 = segments[finalid].p1;
                pointp1 = segments[sgi].p1;
                pointp2 = segments[sgi].p2;
                double dx = pointp2.x() - pointp1.x();
                double dy = pointp2.y() - pointp1.y();
                segments[sgi].normal.setX(-dy);
                segments[sgi].normal.setY(dx);
                segments[sgi].normal.normalize();
            }
        }
        hmlist.clear();
        std::vector<loop> looplist;
        OutPutData op = OutPutData(segments, id);
        looplist = op.looplist;
//        std::sort(looplist.begin(), looplist.end(), colorcompare);
//        floorimg.fill(QColor(0, 0, 0));
        img.fill(QColor(0, 0, 0));

        QPainter fp;//填充颜色，根据向量方向判断，
//        while(tl.size()>0)
//        {
//            tl.erase(tl.begin()); //erase使用此方法清空可释放内存
//        }
//        while(segments.size()>0)
//        {
//            segments.erase(segments.begin());
//        }
//        segments.clear();
//        fp.begin(&floorimg);
//        img.save("H:/dlppng/000.png","PNG");
//        qDebug() << "img.width=begin====" << img.width() << img.height() << id;
        fp.begin(&img);
//        fp.setRenderHint(QPainter::Antialiasing, true);
        fp.setPen(QPen(QColor(255, 255, 255), 0));
        QRgb q1, q2;
//        qDebug() << "looplist.size()--" << looplist.size();
        for(int i = 0; i < looplist.size(); i++)
        {
            if (mparent->isCancel) {
                break;
            }
            if(looplist[i].fillcolor.red() != 255)
            {
                q1 = img.pixel(looplist[i].maxsize.x(), looplist[i].maxsize.y());
                if(qRed(q1) == 255)
                {
                    fp.setBrush(looplist[i].fillcolor);
                    fp.drawPath(looplist[i].mpath);
                }else{
                    fp.setBrush(QColor(255, 255, 255));
                    fp.drawPath(looplist[i].mpath);
                }
            }else{
                fp.setBrush(looplist[i].fillcolor);
                fp.drawPath(looplist[i].mpath);
            }
        }
//        floorimg.save("H:floorimgtest001.jpg","JPG");
//        count++;
//        floorimg.save("H:/dlppng/img"+QString::number(count)+".jpg","JPG");
//        qDebug() << "imglist()" << id << i;
//        floorimg.save("H:/dlppng/"+QString::number(id)+"_"+QString::number(i)+".jpg","JPG");
//        qDebug() << "imglist.push_back(img)-------" << img.width() << img.height() << id;
//        img.save("H:/dlppng/"+QString::number(id)+".png","PNG");
//        imglist.push_back(floorimg); //floorimg每一层的图片
        imglist.push_back(img); //floorimg每一层的图片
//        qDebug() << "imglist.push_back(img)-------" << img.width() << img.height() << id;
        fp.end();
//        while(looplist.size()>0)
//        {
//            looplist.erase(looplist.begin());
//        }
//        qDebug() << "fp == ok";
    }

   //把图片转为坐标点
    if(star_x < 0)
    {
        star_x = 0;
    }
    if(star_y < 0)
    {
        star_y = 0;
    }
    if(end_x < 0)
    {
        end_x = 0;
    }
    if(end_y < 0)
    {
        end_y = 0;
    }
    if(end_x > resolution.x())
    {
        end_x = resolution.x();
    }
    if(end_y > resolution.y())
    {
        end_y = resolution.y();
    }
    whitedata wd;
    QVector2D sp(-1, -1), ep(-1, -1);
    int gray = 0;
    int gray2 = 0;
    std::vector<whitedata> resultlist;
    int x, y, width;
    width = resolution.x();
    bool bCurColorIsWhite = false;
    int imagesize = end_x+end_y*width;
    unsigned currentpos = star_x + star_y*width;
    if(star_x > end_x || star_y > end_y)
    {
        currentpos = imagesize -1;
        star_x = end_x;
        star_y = end_y;
    }
    std::vector<uchar*> datalist;
//    for(int i =0;i<imglist.size();i++)
//    {
//        datalist.push_back(imglist[i].bits());
//    }
//    qDebug() << "change == resultlist";

//    qDebug() << "imglist.size()" << imglist.size() << imglist.capacity() << id;

    //此时已经是镜像状态
//    for(int i = 0; i < imglist.size(); i++)
//    {
//        imglist[i].save("H:/dlppng/"+QString::number(id)+"_"+ QString::number(i) + ".png","PNG");
//    }

    if(slicetype == 1){
        //x正方向
        for(int h = end_x-1; h > star_x-1; h--)
        {
            if (mparent->isCancel) {
                break;
            }
            for(int v = star_y; v<end_y; v++)
            {
                if (mparent->isCancel) {
                    break;
                }
                x = h;
                y = v;
                for(int i = 0; i < imglist.size(); i++)
                {
                    if(x > end_x || y>end_y)
                    {
                        break;
                    }
                    QRgb rgb(imglist[i].pixel(x, y));
                    QColor color(rgb);
                    if(color.red() > 0)
                    {
                        bCurColorIsWhite = true;
                        break;
                    }else{
                        bCurColorIsWhite = false;
                    }
                }
                if(bCurColorIsWhite)
                {
                    if(slicetype == 1)
                    {
                        x = resolution.x() - x;
                    }
                    //20191108 现在的切片数据格式（stary.endy,endx）连线方式
                    if(sp.x() == -1 || sp.y() == -1)
                    {
                        sp.setX(x);
                        sp.setY(y);
                        gray2 = gray;
                    }
                    if(ep.x() == -1 || ep.y() == -1)
                    {
                        ep.setX(x);
                        ep.setY(y);
                        gray2 = gray;
                    }else if(y - ep.y() == 1 && ep.x() == x){
                        ep.setY(y);
                    }else{
                        wd.sp = sp;
                        wd.ep = ep;
                        resultlist.push_back(wd);
                        sp.setX(x);
                        sp.setY(y);
                        ep.setX(x);
                        ep.setY(y);
                    }
                    // 20191108如果需要改为（x,y,灰度）这样的格式，
                    //只需要在此计算灰度值，不为0即符合
                    //但此格式会使数据量更大

                }
            }
            currentpos = x + y*width;
            if(currentpos > imagesize)
            {
                break;
            }
        }
    }else{
        //x镜像
        for(int h = star_x; h < end_x; h++)
        {
            if (mparent->isCancel) {
                break;
            }
            for(int v = star_y; v<end_y; v++)
            {
                if (mparent->isCancel) {
                    break;
                }
                x = h;
                y = v;
                for(int i = 0; i < imglist.size(); i++)
                {
                    if(x > end_x || y>end_y)
                    {
                        break;
                    }
                    QRgb rgb(imglist[i].pixel(x, y));
                    QColor color(rgb);
    //                int r = datalist[i][(int)((y*resolution.x()+x)*4)];
                    if(color.red() > 30)
                    {
                        bCurColorIsWhite = true;
                        break;
                    }else{
                        bCurColorIsWhite = false;
                    }
                }
                if(bCurColorIsWhite)
                {
                    if(sp.x() == -1 || sp.y() == -1)
                    {
                        sp.setX(x);
                        sp.setY(y);
                        gray2 = gray;
                    }
                    if(ep.x() == -1 || ep.y() == -1)
                    {
                        ep.setX(x);
                        ep.setY(y);
                    }else if(y - ep.y() == 1 && ep.x() == x){
                        ep.setY(y);
                    }else{
                        wd.sp = sp;
                        wd.ep = ep;
                        resultlist.push_back(wd);
                        sp.setX(x);
                        sp.setY(y);
                        ep.setX(x);
                        ep.setY(y);
                    }
                }
            }
            currentpos = x + y*width;
            if(currentpos > imagesize)
            {
                break;
            }
        }
    }
    wd.sp = sp;
    wd.ep = ep;
    resultlist.push_back(wd);
    layerresult lr;
    lr.layerid = id;
    lr.layerimgdata = resultlist;
    resultlayer[id] = lr;
//    qDebug() << "layer--ok---" << id;
}

void Slice:: writeFilecopy(){
    //使用旧格式
    iswriting = true;
    QFile* pf = new QFile(saveFileName);
    pf->open(QIODevice::WriteOnly|QIODevice::Append);
    QDataStream out(pf);
    std::vector<whitedata> eachlayer;
    whitedata eachline;
//    qDebug() << "write===" << nowLayerid << max_size;
    if (nowLayerid >= max_size) {
        return;
    }
    while(resultlayer[nowLayerid].layerimgdata.size() > 0){
        eachlayer = resultlayer[nowLayerid].layerimgdata;
//        qDebug() << "eachlayer.size" << eachlayer.size() << nowLayerid;
        out << (quint32)eachlayer.size();
        for(int j = 0; j < eachlayer.size(); j++)
        {
            eachline = eachlayer[j];
            out << (quint16)eachline.sp.y() << (quint16)eachline.ep.y() << (quint16)eachline.ep.x();
        }
        out << (quint8)0x0D << (quint8)0x0A;
        layerresult lr;
        resultlayer[nowLayerid] = lr;
        nowLayerid++;
        if (nowLayerid == max_size) {
            out << "MKSDLP" ;
        }
        if (nowLayerid >= max_size) {
            break;
        }
    }
    pf->close();
    delete pf;
    iswriting = false;
}

////    painter.begin(&img);
////    painter.drawImage(0,0,img);
////    img.scaled(2, 2);
//////    painter.scale(5120, 2880);
//////    qp.drawRect(0, 0, 100,100);
////    painter.setPen(QPen(QColor(255, 255, 255), 0));
//////    painter.drawImage(target, img, source);
////    qDebug() << "width----2-------" << img.width() << img.height();
////    painter.drawImage(0,0,img);
////    img.save("H:/dlppng/img01.jpg","JPG");
////    painter.end();

void Slice::clearFiles(QDir pathdir)
{
    QDir dir = pathdir;
    dir.setFilter(QDir::Files);
    int fileCount = dir.count();
    for (int i = 0; i < fileCount; i++){
        dir.remove(dir[i]);
    }
}

void Slice::setSaveFileName(QString name){
    saveFileName = name;
}

double Slice::functionFromBook(double x, double y) {
    return ((.5)*(1 + sin(x*x*y*y)));
}
double Slice::startAntialiasingMethod1(int numPixels, double x, double y, double dx, double dy) {
    double darkness = 0;
    for (int i = 0; i < numPixels; i++) {
        double r = ((double)rand() / (RAND_MAX+1));
        double sampleX = x + r * (dx*8/9);
        r = ((double)rand() / (RAND_MAX+1));
        double sampleY = y + r * (dy*8/9);
        darkness += functionFromBook(sampleX, sampleY);
    }
    return (darkness / numPixels);
}
QImage Slice::calculatePixels(double minX, double maxX, double minY, double maxY,QImage img, bool antialiasingEnabled) {
    double pixelWidth = img.width();
    double pixelHeight = img.height();
    for (double i = minX; i < maxX; i++) {
        for (double j = minY; j < maxY; j--) {
            QRgb rgb(img.pixel(i, j));
            QColor color(rgb);
            if (color.red() > 30) {
                double darkness;
                darkness = startAntialiasingMethod1(1, j, i, 1, 1);
                QColor color(darkness*255);
                int pixelXVal = (j / maxX) * pixelWidth;
                int pixelYVal = (i / maxY) * pixelHeight;
                img.setPixel(pixelXVal, pixelYVal, color.rgba());
            }
        }
    }
    return img;
}

void Slice::generateByH(unsigned int id, double att, QImage &floorimg)
{
    ModelData* md;
    std::vector<triangle> tl;
    std::vector<QImage> imglist;
    std::vector<MSupport> suplist;
    int star_x = resolution.x(), star_y = resolution.y(), end_x = 0, end_y = 0;
//    QDateTime current_date_time;
//    QString current_date, current_time;
//    current_date_time = QDateTime::currentDateTime();
//    current_date = current_date_time.toString("yyyy-MM-dd");
//    current_time = current_date_time.toString("hh:mm:ss.zzz ");
//    qDebug() << current_time;
    for(int i = 0; i < mdlist.size(); i++)
    {
        md = mdlist[i];
        tl = md->getZTri(att);
        suplist = md->supportlist;
        for(int j = 0; j < suplist.size(); j++)
        {
            suplist[j].getZTri(tl, att);
        }
        double xdisp, ydisp, zdisp, planefraction;
        QVector3D thisP, thatP, mdoffset, mdposition;
        float mdz = md->getOffset().z();
        mdoffset = md->getOffset();
        mdposition = md->getPosition();
        int cmpcount = 0;
        std::vector<segment> segments;
        triangle t;
        segment sg;
        segment bsg, nsg;
        QVector2D points[2];
        double potentialDist;
        int finalid;
        double mindist = 100000.0;
        int waitid, nid;
        QVector2D pointp1, pointp2;
//        att -= mdz;
//        QPainter pfp;
//        floorimg.fill(QColor(0, 0, 0));
//        pfp.begin(&floorimg);
//        pfp.setPen(QPen(QColor(255, 255, 255), 0.1));
//        std::sort(tl.begin(), tl.end(), bstri);
        for(int j = 0; j<tl.size(); j++)
        {
            t = tl[j];
            cmpcount = 0;
            for(int v1 = 0; v1 < 3; v1++)
            {
                thisP = t.vertex[v1];
                if(thisP.z() <= att)
                {
                    continue;
                }
                for(int v2 = 0; v2 < 3; v2++){
                    if(v2 == v1)
                    {
                        continue;
                    }
                    thatP = t.vertex[v2];
                    if(thatP.z() > att)
                    {
                        continue;
                    }
                    xdisp = thatP.x() - thisP.x();
                    ydisp = thatP.y() - thisP.y();
                    zdisp = thatP.z() - thisP.z();
                    planefraction = (thisP.z() - att)/(fabs(zdisp));
                    points[cmpcount].setX(thisP.x() + xdisp*planefraction);
                    points[cmpcount].setY(thisP.y() + ydisp*planefraction);
                    cmpcount++;
                }
            }
            points[0].setX((points[0].x()+mdposition.x())/pixscale.x());
            points[0].setY((points[0].y()+mdposition.y())/pixscale.y());
            points[1].setX((points[1].x()+mdposition.x())/pixscale.x());
            points[1].setY((points[1].y()+mdposition.y())/pixscale.y());
            sg.p1 = points[0] + resolution/2;
            sg.p2 = points[1] + resolution/2;
            if(sg.p2.x() > end_x)
            {
                end_x = ceil(sg.p2.x())+1;
            }
            if(sg.p2.y() > end_y)
            {
                end_y = ceil(sg.p2.y())+1;
            }
            if(sg.p2.x() < star_x)
            {
                star_x = floor(sg.p2.x())-1;
            }
            if(sg.p2.y() < star_y)
            {
                star_y = floor(sg.p2.y())-1;
            }
            sg.normal.setX(t.normal.x());
            sg.normal.setY(t.normal.y());
            sg.normal.normalize();
            sg.inloop = false;
            sg.linepoint = false;
            sg.frontid = -1;
            sg.backid = -1;
            sg.nowid = j;
            CorrectPointOrder(sg);
            segments.push_back(sg);
        }
        std::sort(segments.begin(), segments.end(), bs);

//        current_date_time = QDateTime::currentDateTime();
//        current_date = current_date_time.toString("yyyy-MM-dd");
//        current_time = current_date_time.toString("hh:mm:ss.zzz ");
//        qDebug() << current_time;

        segment thisSeg;
        std::vector<sid> outline, potentialLeadSegs;
        double potentialangle;
        for(int i = 0; i < segments.size(); i++)
        {
            segments[i].nowid = i;
            if(segments[i].frontid != -1 || segments[i].linepoint)
            {
                continue;
            }
            potentialLeadSegs.clear();
            getSegmentAroundX(segments, outline, segments[i].p2.x());
            for(int j = 0; j < outline.size(); j++)
            {
                if(outline[j].id == i)
                {
                    continue;
                }
                if(outline[j].sg.backid != -1 || outline[j].sg.linepoint)
                {
                    continue;
                }
                if(distance2D(segments[i].p2, segments[outline[j].id].p1) <= 0.03)
                {
                    potentialLeadSegs.push_back(outline[j]);
                }
            }
            mindist = 100000.0;
            finalid = -1;
            potentialangle = 0.0;
            for(int potential = 0; potential < potentialLeadSegs.size(); potential++)
            {
                potentialDist = distance2D(segments[i].p2, potentialLeadSegs[potential].sg.p1);
                if(potentialDist < mindist)
                {
                    mindist = potentialDist;
                    finalid = potentialLeadSegs[potential].id;
                }
            }
            if(finalid != -1)
            {
                segments[i].frontid = finalid;
                segments[finalid].backid = i;
                segments[i].p2 = segments[finalid].p1;
                pointp1 = segments[i].p1;
                pointp2 = segments[i].p2;
                double dx = pointp2.x() - pointp1.x();
                double dy = pointp2.y() - pointp1.y();
                segments[i].normal.setX(-dy);
                segments[i].normal.setY(dx);
                segments[i].normal.normalize();
            }
        }
        std::vector<loop> looplist;
        OutPutData op = OutPutData(segments, id);
        looplist = op.looplist;
        std::sort(looplist.begin(), looplist.end(), loopcompare);

//        current_date_time = QDateTime::currentDateTime();
//        current_date = current_date_time.toString("yyyy-MM-dd");
//        current_time = current_date_time.toString("hh:mm:ss.zzz ");
//        qDebug() << current_time;
        floorimg.fill(QColor(0, 0, 0));
        QPainter fp;
        fp.begin(&floorimg);
        fp.setPen(QPen(QColor(255, 255, 255), 0));
        for(int i = 0; i < looplist.size(); i++)
        {
            fp.setBrush(looplist[i].fillcolor);
            fp.drawPath(looplist[i].mpath);
        }
        fp.end();
        if(slicetype == 1)
        {
            floorimg = floorimg.mirrored(true, false);
        }
        imglist.push_back(floorimg);
//        current_date_time = QDateTime::currentDateTime();
//        current_date = current_date_time.toString("yyyy-MM-dd");
//        current_time = current_date_time.toString("hh:mm:ss.zzz ");
//        qDebug() << current_time;
        std::vector<struct loop>().swap(looplist);
        std::vector<struct segment>().swap(segments);
        std::vector<struct triangle>().swap(tl);
    }
    if(star_x < 0)
    {
        star_x = 0;
    }
    if(star_y < 0)
    {
        star_y = 0;
    }
    if(end_x > resolution.x())
    {
        end_x = resolution.x();
    }
    if(end_y > resolution.y())
    {
        end_y = resolution.y();
    }
    if(slicetype == 1)
    {
        star_x = resolution.x()-end_x;
        end_x = resolution.x()-star_x;
    }
    floorimg.fill(QColor(0, 0, 0));
    whitedata wd;
    QVector2D sp(-1, -1), ep(-1, -1);
    std::vector<whitedata> resultlist;
    int x, y, width;
    width = resolution.x();
    bool bCurColorIsWhite = false;
    int imagesize = end_x+end_y*width;
    unsigned currentpos = star_x + star_y*width;
    if(star_x > end_x || star_y > end_y)
    {
        currentpos = imagesize -1;
        star_x = end_x;
        star_y = end_y;
    }

//    current_date_time = QDateTime::currentDateTime();
//    current_date = current_date_time.toString("yyyy-MM-dd");
//    current_time = current_date_time.toString("hh:mm:ss.zzz ");
//    qDebug() << current_time;
    std::vector<uchar*> datalist;
    for(int i =0;i<imglist.size();i++)
    {
        datalist.push_back(imglist[i].bits());
    }

    for(int h = star_x; h < end_x; h++)
    {
        for(int v = star_y; v<end_y; v++)
        {
            x = h;
            y = v;
            for(int i = 0; i < imglist.size(); i++)
            {
//                bCurColorIsWhite = isWhitePixel(imglist[i], x, y);
//                if(bCurColorIsWhite){
//                    break;
//                }
                int r = datalist[i][(int)((y*resolution.x()+x)*4)];
                if(r > 30)
                {
                    bCurColorIsWhite = true;
                    break;
                }else{
                    bCurColorIsWhite = false;
                }
            }
            if(bCurColorIsWhite)
            {
                if(sp.x() == -1 || sp.y() == -1)
                {
                    sp.setX(x);
                    sp.setY(y);
                }
                if(ep.x() == -1 || ep.y() == -1)
                {
                    ep.setX(x);
                    ep.setY(y);
                }else if(y - ep.y() == 1 && ep.x() == x){
                    ep.setY(y);
                }else{
                    wd.sp = sp;
                    wd.ep = ep;
                    resultlist.push_back(wd);
                    sp.setX(x);
                    sp.setY(y);
                    ep.setX(x);
                    ep.setY(y);
                }
            }
        }
        currentpos = x + y*width;
        if(currentpos > imagesize)
        {
            break;
        }
    }

//    current_date_time = QDateTime::currentDateTime();
//    current_date = current_date_time.toString("yyyy-MM-dd");
//    current_time = current_date_time.toString("hh:mm:ss.zzz ");
//    qDebug() << current_time;

    wd.sp = sp;
    wd.ep = ep;
    resultlist.push_back(wd);
    layerresult lr;
    lr.layerid = id;
    lr.layerimgdata = resultlist;
    resultlayer[id] = lr;
    std::vector<struct whitedata>().swap(resultlist);
}

void Slice::getSegmentAroundX(std::vector<segment> normallist,std::vector<sid> &outlist, double x)
{
    const double buffer = 0.001;
    const double cbuffer = 0.001;
    int mid,high,low;
    segment curSeg;
    outlist.clear();
    sid sd;
    high = normallist.size()-1;
    low = 0;
    while(high >= low)
    {
        mid = (high+low)/2;
        curSeg = normallist[mid];
        if (curSeg.p1.x() < (x - buffer))
        {
            low = mid + 1;
        }
        else if (curSeg.p1.x() > (x + buffer))
        {
            high = mid - 1;
        }else
        {
            while(mid > 0 && normallist[mid].p1.x() > (x-buffer))
            {
                mid--;
            }
            while((mid <= normallist.size()-1) && (normallist[mid].p1.x() < (x+buffer)))
            {
                sd.id = mid;
                sd.sg = normallist[mid];
                outlist.push_back(sd);
                mid++;
            }
            return;
        }
    }
}

double Slice::distance2D(QVector2D point1, QVector2D point2)
{
    return sqrt( pow((point2.x()-point1.x()),2) + pow((point2.y()-point1.y()),2));
}

void Slice::generateLoop(unsigned int id, std::vector<triangle> tl, double att)
{
    if(id > max_id)
    {
        max_id = id;
    }
    double xdisp, ydisp, zdisp, planefraction;
    QVector3D thisP, thatP;
    int cmpcount = 0;
    std::vector<segment> segments;
    triangle t;
    QPainter fp;
    segment sg;
    QVector2D points[2];
    for(int i = 0; i<tl.size(); i++)
    {
        t = tl[i];
        cmpcount = 0;
        for(int v1 = 0; v1 < 3; v1++)
        {
            thisP = t.vertex[v1];
            if(thisP.z() <= att)
            {
                continue;
            }
            for(int v2 = 0; v2 < 3; v2++){
                if(v2 == v1)
                {
                    continue;
                }
                thatP = t.vertex[v2];
                if(thatP.z() > att)
                {
                    continue;
                }
                xdisp = thatP.x() - thisP.x();
                ydisp = thatP.y() - thisP.y();
                zdisp = thatP.z() - thisP.z();
                planefraction = (thisP.z() - att)/(fabs(zdisp));
                points[cmpcount].setX(thisP.x() + xdisp*planefraction);
                points[cmpcount].setY(thisP.y() + ydisp*planefraction);
                cmpcount++;
            }
        }
//        sg.p1.setX(round((points[0].x()/thickness+resolution.x()/2)*10)/10);
//        sg.p1.setY(round((points[0].y()/thickness+resolution.y()/2)*10)/10);
//        sg.p2.setX(round((points[1].x()/thickness+resolution.x()/2)*10)/10);
//        sg.p2.setY(round((points[1].y()/thickness+resolution.y()/2)*10)/10);
        sg.p1 = points[0]/thickness+resolution/2;
        sg.p2 = points[1]/thickness+resolution/2;
//        sg.p1.setX(ceil(points[0].x()/0.1+resolution/2));
        sg.normal.setX(t.normal.x());
        sg.normal.setY(t.normal.y());
        sg.normal.normalize();
        sg.inloop = false;
        sg.frontid = -1;
        sg.backid = -1;
        CorrectPointOrder(sg);
        segments.push_back(sg);
//        QPointF pf1, pf2;
//        pf1.setX(sg.p1.x());
//        pf1.setY(sg.p1.y());
//        pf2.setX(sg.p2.x());
//        pf2.setY(sg.p2.y());
//        fp.drawLine(pf1, pf2);
    }
//    fp.end();
    std::sort(segments.begin(), segments.end(), bs);
    segment thisSeg;
    for(int i = 0; i < segments.size(); i++)
    {
        if(segments[i].frontid != -1 && segments[i].backid != -1)
        {
            continue;
        }
        for(int j = 0; j < segments.size(); j++)
        {
            if(i == j)
            {
                continue;
            }
            if(segments[i].p2 == segments[j].p1 && segments[i].backid == -1)
            {
//                segments[i].backP.push_back(thatSeg);
                segments[i].backid = j;
//                segments[j].frontP.push_back(thisSeg);
                segments[j].frontid = i;
                continue;
            }
            if(segments[i].p1 == segments[j].p2 && segments[i].frontid == -1)
            {
//                segments[i].frontP.push_back(thatSeg);
                segments[i].frontid = j;
//                segments[j].backP.push_back(thisSeg);
                segments[j].backid = i;
                continue;
            }
        }
    }
    std::vector<loop> looplist;
    for(int i = 0; i < segments.size(); i++)
    {
        thisSeg = segments[i];
        if(segments[i].inloop)
        {
            continue;
        }else{
            if(segments[i].backid == -1)
            {
                continue;
            }
            segment max_x[2], max_y[2], min_x[2], min_y[2];
            QVector2D firstPoint, lastPoint;
            int nowid;
            loop newloop;
            firstPoint = segments[i].p1;
            nowid = i;
            newloop.mpath.moveTo(round(segments[i].p1.x()*10)/10, round(segments[i].p1.y()*10)/10);
            segments[i].inloop = true;
            max_x[0] = segments[i];
            max_y[0] = segments[i];
            min_x[0] = segments[i];
            min_y[0] = segments[i];
            do{
                if(max_x[0].p1.x() < thisSeg.p1.x())
                {
                    max_x[0] = thisSeg;
                }
                if(max_y[0].p1.y() < thisSeg.p1.y())
                {
                    max_y[0] = thisSeg;
                }
                if(min_x[0].p1.x() > thisSeg.p1.x())
                {
                    min_x[0] = thisSeg;
                }
                if(min_y[0].p1.y() > thisSeg.p1.y())
                {
                    min_y[0] = thisSeg;
                }
                newloop.mpath.lineTo(round(segments[thisSeg.backid].p1.x()*10)/10, round(segments[thisSeg.backid].p1.y()*10)/10);
                newloop.mpath.lineTo(round(segments[thisSeg.backid].p2.x()*10)/10, round(segments[thisSeg.backid].p2.y()*10)/10);
                lastPoint = segments[thisSeg.backid].p2;
                segments[thisSeg.backid].inloop = true;
                thisSeg = segments[thisSeg.backid];
                if(thisSeg.backid == -1){
                    newloop.mpath.lineTo(firstPoint.x(), firstPoint.y());
                    lastPoint = firstPoint;
                    segments[nowid].backid = i;
                    break;
                }
                nowid = thisSeg.backid;
            }while(!segments[thisSeg.backid].inloop);
            if(lastPoint != firstPoint)
            {
                newloop.mpath.lineTo(firstPoint.x(), firstPoint.y());
            }
//            newloop.msize.setX(fabs(max_x[0].p1.x() - min_x[0].p1.x()));
//            newloop.msize.setY(fabs(max_y[0].p1.y() - min_y[0].p1.y()));
            max_x[1] = segments[max_x[0].backid];
            max_y[1] = segments[max_y[0].backid];
            min_x[1] = segments[min_x[0].backid];
            min_y[1] = segments[min_y[0].backid];
            getColor(max_x, max_y, min_x, min_y, newloop.fillcolor);
//            int r = newloop.fillcolor.red(), g = newloop.fillcolor.green(), b = newloop.fillcolor.blue();
            looplist.push_back(newloop);
        }
    }
    layerlooplist lll = finishlayer[id];
    lll.layerid = id;
    for(int i = 0; i < looplist.size(); i++)
    {
        lll.layerloopdata.push_back(looplist[i]);
    }
    finishlayer[id] = lll;
//    finishlayer.push_back(lll);
//    floorimg.fill(QColor(0, 0, 0));
//    fp.begin(&floorimg);
//    fp.setPen(QPen(QColor(255, 255, 255), 1));
//    loop nowloop;
//    for(int i = 0; i < looplist.size(); i++)
//    {
//        nowloop = looplist[i];
//        fp.setBrush(nowloop.fillcolor);
//        fp.drawPath(nowloop.mpath);
//    }
//    fp.end();
    std::vector<struct loop>().swap(looplist);
    std::vector<struct segment>().swap(segments);
    std::vector<struct triangle>().swap(tl);

//    finishlayer[id] = lr;
//    QFile file("F:\\qtest.png");
//    file.open(QIODevice::WriteOnly);
}
bool Slice::isWhitePixel(QImage imgdata, int x, int y)
{
    int r, g, b;
    QColor c(imgdata.pixel(x, y));
    r = c.red();
    g = c.green();
    b = c.blue();
    if(r < 32 && g < 32 && b < 32)
    {
        return false;
    }
    return true;
}
bool Slice::isWhitePixel(std::vector<QImage> imgdata, unsigned uCurPos)
{
    int x, y, r, g, b;
    y = uCurPos / imgdata[0].width();
    x = uCurPos - y*imgdata[0].width();
    QColor c;
    for(int i = 0; i < imgdata.size(); i++)
    {
        c = QColor(imgdata[i].pixel(x, y));
        r = c.red();
        g = c.green();
        b = c.blue();
        if(r > 50 && g > 50 && b > 50)
        {
            return true;
        }
    }
    return false;
}

void Slice::generateData(unsigned int id, std::vector<loop> nlooplist, QImage &floorimg)
{
    std::sort(nlooplist.begin(), nlooplist.end(), loopcompare);
    QPainter fp;
    floorimg.fill(QColor(0, 0, 0));
    fp.begin(&floorimg);
    fp.setPen(QPen(QColor(255, 255, 255), 1));
    loop nowloop;
    for(int i = 0; i < nlooplist.size(); i++)
    {
        nowloop = nlooplist[i];
        fp.setBrush(nowloop.fillcolor);
        fp.drawPath(nowloop.mpath);
    }
    fp.end();
//    bool b = floorimg.save("F:\\qtest.png", "PNG");
    int imagesize = floorimg.width()*floorimg.height();
    unsigned currentpos = 0;
    whitedata wd;
    QVector2D sp(-1, -1), ep(-1, -1);
    std::vector<whitedata> resultlist;
    int x, y;
    bool bCurColorIsWhite = false;
    do{
        bCurColorIsWhite = isWhitePixel(floorimg, x, y);
//        while((currentpos < imagesize) && (bCurColorIsWhite == isWhitePixel(floorimg, currentpos))){
//        }
        if(bCurColorIsWhite)
        {
            y = currentpos/floorimg.width();
            x = currentpos - y*floorimg.width();
            if(sp.x() == -1 || sp.y() == -1)
            {
                sp.setX(x);
                sp.setY(y);
            }
            if(ep.x() == -1 || ep.y() == -1)
            {
                ep.setX(x);
                ep.setY(y);
            }else if(x - ep.x() == 1 && ep.y() == y){
                ep.setX(x);
            }else{
                wd.sp = sp;
                wd.ep = ep;
                resultlist.push_back(wd);
                sp.setX(x);
                sp.setY(y);
                ep.setX(x);
                ep.setY(y);
            }
        }
        currentpos++;
    }while(currentpos < imagesize);
    wd.sp = sp;
    wd.ep = ep;
    resultlist.push_back(wd);
    layerresult lr;
    lr.layerid = id;
    lr.layerimgdata = resultlist;
    resultlayer[id] = lr;
    std::vector<struct whitedata>().swap(resultlist);
}

void Slice::getColor(segment max_x[2], segment max_y[2], segment min_x[2], segment min_y[2], QColor &fcolor)
{
    int vote = 0;
    QVector3D cross;
    if(max_x[0].normal.x() > 0 && max_x[1].normal.x() > 0)
    {
        vote++;
    }
    else if(max_x[0].normal.x() < 0 && max_x[1].normal.x() < 0)
    {
        vote--;
    }
    else{
        cross = QVector3D::crossProduct(QVector3D(max_x[0].normal), QVector3D(max_x[1].normal));
        if(cross.z() > 0)
        {
            vote--;
        }else{
            vote++;
        }
    }

    if(min_x[0].normal.x() < 0 && min_x[1].normal.x() < 0)
    {
        vote++;
    }
    else if(min_x[0].normal.x() > 0 && min_x[1].normal.x() > 0)
    {
        vote--;
    }
    else{
        cross = QVector3D::crossProduct(QVector3D(min_x[0].normal), QVector3D(min_x[1].normal));
        if(cross.z() > 0)
        {
            vote--;
        }else{
            vote++;
        }
    }

    if(max_y[0].normal.y() > 0 && max_y[1].normal.y() > 0)
    {
        vote++;
    }
    else if(max_y[0].normal.y() < 0 && max_y[1].normal.y() < 0)
    {
        vote--;
    }else{
        cross = QVector3D::crossProduct(QVector3D(max_y[0].normal), QVector3D(max_y[1].normal));
        if(cross.z() > 0)
        {
            vote--;
        }else{
            vote++;
        }
    }

    if(min_y[0].normal.y() < 0 && min_y[1].normal.y() < 0)
    {
        vote++;
    }
    else if(min_y[0].normal.y() > 0 && min_y[1].normal.y() > 0)
    {
        vote--;
    }else{
        cross = QVector3D::crossProduct(QVector3D(min_y[0].normal), QVector3D(min_y[1].normal));
        if(cross.z() > 0)
        {
            vote--;
        }else{
            vote++;
        }
    }

    if(vote > 0){
        fcolor = QColor(255, 255, 255);
    }else{
        fcolor = QColor(0, 0, 0);
    }
}

void Slice::fillImage(QImage &img, int x, int y)
{
    if(x < 0 || x >= img.width())
    {
        return;
    }
    if(y < 0 || y >= img.height())
    {
        return;
    }
    QRgb nowpixel;
    nowpixel = img.pixel(x, y);
    int r = qRed(nowpixel);
    int g = qGreen(nowpixel);
    int b = qBlue(nowpixel);
    if(r == 0 && g == 0 && b == 0)
    {
        img.setPixel(x, y, qRgb(135, 135, 135));
    }
    else{
        return;
    }
    fillImage(img, x+1, y);
    fillImage(img, x, y+1);
    fillImage(img, x-1, y);
    fillImage(img, x, y-1);
}

void Slice::ConnectSegmentNeighbors(std::vector<segment> &segments)
{
}
int Slice::PointLineCompare(QVector2D pointm, QVector2D dir, QVector2D quarrypoint)
{
    double position = (dir.x()*(quarrypoint.y() - pointm.y())) - (dir.y()*(quarrypoint.x() - pointm.x()));
    return -int(ceil(position));
}
bool Slice::CorrectPointOrder(segment &sg)
{
    double p2x;
    double p2y;


    QVector2D center((sg.p2.x() + sg.p1.x())/2.0,(sg.p2.y() + sg.p1.y())/2.0);
    int side = PointLineCompare(center,sg.normal, sg.p1);//returns 1 if point is on right, -1 if point is on left

    if(side < 0)
    {
        return 0; //we want the point on the left
    }
    else
    {
        //swap point data!
        p2x = sg.p2.x();
        p2y = sg.p2.y();

        sg.p2.setX(sg.p1.x());
        sg.p2.setY(sg.p1.y());

        sg.p1.setX(p2x);
        sg.p1.setY(p2y);

        return 1;
    }
}

//inline int idx(int w, int h, int i, int j) {
//    return i + ((h-1)-j)*w;
//}

//void writeImg(const int *red, const int *blue, const int *green, const int w, const int h, const char* fname) {
//    // Create and save a .bmp image file using red, blue, green channels
//    int filesize = 54 + 3*w*h;
//    unsigned char *img = new unsigned char[3*w*h];
//    int ind = 0;

//    for (int i=0; i<w; i++) {
//        for (int j=0; j<h; j++) {
//            ind = idx(w,h,i,j);
//            img[ind*3+2] = static_cast<unsigned char>(red[ind]  ); // r
//            img[ind*3+1] = static_cast<unsigned char>(blue[ind] ); // g
//            img[ind*3+0] = static_cast<unsigned char>(green[ind]); // b
//        }
//    }

//    // Magic header mumbo-jumbo
//    unsigned char bmpfileheader[14] = {'B','M',0,0,0,0,0,0,0,0,54,0,0,0};
//    unsigned char bmpinfoheader[40] = {40,0,0,0,0,0,0,0,0,0,0,0,1,0,24,0};
//    unsigned char bmppad[3] = {0,0,0};

//    bmpfileheader[ 2] = static_cast<unsigned char>(filesize    );
//    bmpfileheader[ 3] = static_cast<unsigned char>(filesize>> 8);
//    bmpfileheader[ 4] = static_cast<unsigned char>(filesize>>16);
//    bmpfileheader[ 5] = static_cast<unsigned char>(filesize>>24);

//    bmpinfoheader[ 4] = static_cast<unsigned char>(       w    );
//    bmpinfoheader[ 5] = static_cast<unsigned char>(       w>> 8);
//    bmpinfoheader[ 6] = static_cast<unsigned char>(       w>>16);
//    bmpinfoheader[ 7] = static_cast<unsigned char>(       w>>24);
//    bmpinfoheader[ 8] = static_cast<unsigned char>(       h    );
//    bmpinfoheader[ 9] = static_cast<unsigned char>(       h>> 8);
//    bmpinfoheader[10] = static_cast<unsigned char>(       h>>16);
//    bmpinfoheader[11] = static_cast<unsigned char>(       h>>24);

//    FILE *f;

//    f = fopen(fname,"wb");
//    fwrite(bmpfileheader,1,14,f);
//    fwrite(bmpinfoheader,1,40,f);
//    for (int i=0; i<h; i++) {
//        fwrite(img+(w*(h-i-1)*3),3,static_cast<unsigned int>(w),f);
//        fwrite(bmppad,1,(4-(w*3)%4)%4,f);
//    }

//    free(img);
//    fclose(f);
//}

//void writeImg(const int *grey, const int w, const int h, const char* fname) {
//    // Create and save a .bmp file in greyscale calling the rgb-version.
//    writeImg(grey, grey, grey, w, h, fname);
//}

//void edgeDetect(const int w, const int h, const int *pix, int *edge) {
//    const int LIMIT = 16;

//    int ind, ind_r, ind_b;

//    enum {l_edge=1, r_edge=2, b_edge=4, t_edge=8};

//    // Find vertical edges. Scan horizontal.
//    for (int i=0; i<w-1; i++) {
//        for (int j=0; j<h; j++) {
//            ind   = idx(w,h,i  ,j  );
//            ind_r = idx(w,h,i+1,j  );
//            if (abs(pix[ind] - pix[ind_r]) > LIMIT) {
//                // Store in pixel with the lowest (darkest) value
//                edge[((pix[ind]<pix[ind_r])?(ind):(ind_r))] += (pix[ind]>pix[ind_r])?(l_edge):(r_edge);
//            }
//        }
//    }

//    // Find horizontal edges. Scan vertical.
//    for (int i=0; i<w; i++) {
//        for (int j=0; j<h-1; j++) {
//            ind   = idx(w,h,i  ,j  );
//            ind_b = idx(w,h,i  ,j+1);
//            if (abs(pix[ind] - pix[ind_b]) > LIMIT) {
//                // Store in pixel with the lowest (darkest) value
//                edge[(pix[ind]<pix[ind_b])?(ind):(ind_b)] += (pix[ind]>pix[ind_b])?(t_edge):(b_edge);
//            }
//        }
//    }
//}

//class Shape {
//public:
//    void setStart(int _x, int _y) {start_x = _x; start_y = _y;}
//    void setEnd(int _x, int _y) {end_x = _x; end_y = _y;}

//    void setType(int _t, bool column) {
//        if (_t < 0) {
//            type = abs(_t);
//        }
//        else {
//            if (column) {
//                if      (start_y < end_y) type =  2;
//                else if (start_y > end_y) type =  4;
//                else if (end_y == _t)     type =  8;
//                else                      type = 16;
//            }
//            else {
//                if      (start_x < end_x) type =  3;
//                else if (start_x > end_x) type =  5;
//                else if (end_x == _t)     type =  9;
//                else                      type = 17;
//            }
//        }
//    }

//    int getStartX(){return start_x;}
//    int getStartY(){return start_y;}

//    int getEndX(){return end_x;}
//    int getEndY(){return end_y;}

//    int getType(){return type;}

//    float getLength(){
//        return sqrt((start_x-end_x)*(start_x-end_x) + (start_y-end_y)*(start_y-end_y));
//    }

//    void reset(){
//        start_x = -1;
//        start_y = -1;
//        end_x   = -1;
//        end_y   = -1;
//        type    = -1;
//    }

//private:
//    int start_x = -1;
//    int start_y = -1;
//    int end_x = -1;
//    int end_y = -1;
//    int type = -1; // 1 = \, 2 = /, 4 = U, 8 = ^
//};

//void findShapesRow(const int w, const int h, int *edge, std::vector<Shape> &shapes) {
//    enum {l_edge=1, r_edge=2, b_edge=4, t_edge=8};

//    int v_edge, h_edge;

//    int ind,ind_b;
//    Shape shape;

//    bool found_right;
//    bool found_left;
//    bool found_hor ;

//    int *ort = new int[2];

//    for (int j=0; j<h-1; j++) {
//        for (int i=0; i<w; i++) {
//            ind   = idx(w,h,i  ,j  );
//            ind_b = idx(w,h,i  ,j+1);

//            // Create slices that are the collective of the upper and lower pixel being checked.
//            v_edge = ((edge[ind] | edge[ind_b]) & (l_edge | r_edge));
//            h_edge = ((edge[ind] & b_edge) || (edge[ind_b] & t_edge));

//            // Reset flags for each line
//            if (i==0) {
//                found_right = false;
//                found_left  = false;
//                found_hor   = false;
//                ort[0] = -1;
//                ort[1] = -1;
//                // If we're at the edge we might find L shapes
//                if (h_edge) {
//                    found_hor = true;
//                    shape.setStart(i,j);
//                }
//            }

//            // True if we have a vertical edge and haven't found a left edge yet.
//            if (v_edge && !found_left && !found_hor) {
//                ort[0] = (v_edge    & l_edge)?(i):(i+1);
//                ort[1] = (edge[ind] & v_edge)?(j):(j+1);
//                found_left = true;
//            }
//            // We've found a horizontal edge which will be the start of our shape
//            if (h_edge && !found_hor) {
//                shape.setStart(ort[0],ort[1]);
//                found_hor = true;
//            }
//            // We've found a possible right edge to keep until we run out of horizontal edge.
//            if (v_edge && found_hor) {
//                ort[0] = (v_edge    & r_edge)?(i):(i-1);
//                ort[1] = (edge[ind] & v_edge)?(j):(j+1);
//                found_right = true;
//            }

//            // We've run out of horizontal edge and will end the shape, or we're at the end
//            if ((!h_edge && found_hor) || ((i==w-1) && h_edge)) {
//                if (!found_left){       // We've found an L-start shape
//                    shape.setEnd(ort[0],ort[1]);
//                    shape.setType((j==ort[1])?(-32):(-64),true);
//                }
//                else if (!found_right){ // We've found an L-end shape
//                    shape.setEnd(i     ,ort[1]);
//                    shape.setType((j==ort[1])?(-128):(-256),true);
//                }
//                else{  // We've found a Z or U shape
//                    shape.setEnd(ort[0],ort[1]);
//                    shape.setType(j,true);
//                }

//                shapes.push_back(shape);

//                // Reset flags
//                found_right = false;
//                found_left  = false;
//                found_hor   = false;
//                shape.reset();
//            }
//            // Reset if we don't have any edges
//            if (!h_edge && !v_edge) {
//                found_left = false;
//            }
//        }
//    }
//}

//void findShapesCol(const int w, const int h, int *edge, std::vector<Shape> &shapes) {
//    enum {l_edge=1, r_edge=2, b_edge=4, t_edge=8};

//    int v_edge, h_edge;

//    int ind,ind_r;
//    Shape shape;

//    bool found_top;
//    bool found_bot;
//    bool found_ver;

//    int *ort = new int[2];

//    for (int i=0; i<w-1; i++) {
//        for (int j=0; j<h; j++) {
//            ind   = idx(w,h,i  ,j  );
//            ind_r = idx(w,h,i+1,j  );

//            // Create slices that are the collective of the upper and lower pixel being checked.
//            v_edge = ((edge[ind] & r_edge) || (edge[ind_r] & l_edge));
//            h_edge = ((edge[ind] | edge[ind_r]) & (b_edge | t_edge));

//            // Reset flags at start of each row
//            if (j==0) {
//                found_bot = false;
//                found_top = false;
//                found_ver = false;
//                ort[0] = -1;
//                ort[1] = -1;
//                // If we're at the edge we might find L shapes
//                if (v_edge) {
//                    found_ver = true;
//                    shape.setStart(i,j);
//                }
//            }

//            // True if we have a horizontal edge and haven't found a top edge yet.
//            if (h_edge && !found_top && !found_ver) {
//                ort[0] = (edge[ind] & h_edge)?(i):(i+1);
//                ort[1] = (h_edge    & t_edge)?(j):(j+1);
//                found_top = true;
//            }
//            // We've found a vertical edge which will be the start of our shape
//            if (v_edge && !found_ver) {
//                shape.setStart(ort[0],ort[1]);
//                found_ver = true;
//            }
//            // We've found a possible bottom edge to keep until we run out of vertical edge.
//            if (h_edge && found_ver) {
//                ort[0] = (edge[ind] & h_edge)?(i):(i+1);
//                ort[1] = (h_edge    & b_edge)?(j):(j-1);
//                found_bot = true;
//            }

//            // We've run out of horizontal edge and will end the shape, or we're at the end
//            if ((!v_edge && found_ver) || ((j==h-1) && v_edge)) {
//                if (!found_top){      // We've found an L-start shape
//                    shape.setEnd(ort[0],ort[1]);
//                    shape.setType((i==ort[0])?(-33):(-65),false);
//                }
//                else if (!found_bot){ // We've found an L-end shape
//                    shape.setEnd(ort[0],j);
//                    shape.setType((i==ort[0])?(-129):(-257),false);
//                }
//                else{  // We've found a Z or U shape
//                    shape.setEnd(ort[0],ort[1]);
//                    shape.setType(i,false);
//                }

//                shapes.push_back(shape);

//                // Reset flags
//                found_top = false;
//                found_bot = false;
//                found_ver = false;
//                shape.reset();
//            }
//            // Reset if we don't have any edges
//            if (!v_edge && !h_edge) {
//                found_top = false;
//            }
//        }
//    }
//}

//void blend(const int w, const int h, int *pix, std::vector<Shape> &shapes) {
//    int c_old, c_opp;
//    float b1, b2;
//    float A;

//    int x_len, y_len;
//    int x_start, y_start;
//    int x_end, y_end;

//    float eps = 1e-8;
//    const int MAX_LEN = 15;

//    int type;

//    Shape shape;

//    int *row = new int[w*h]; // For row shapes
//    int *col = new int[w*h]; // For column shapes
//    for (int i=0; i<w*h; i++) {
//        row[i] = pix[i];
//        col[i] = pix[i];
//    }

//    for (std::vector<Shape>::iterator it = shapes.begin(); it != shapes.end(); it++) { // Go through all shapes.

//        x_start = it->getStartX();
//        y_start = it->getStartY();
//        x_end   = it->getEndX();
//        y_end   = it->getEndY();

//        type    = it->getType();

//        x_len = abs(x_start - x_end)+1;
//        y_len = abs(y_start - y_end)+1;

//        if ((x_len > MAX_LEN) || (y_len) > MAX_LEN) {
//            //Avoid processing too long shapes
//            continue;
//        }

//        if (!(type%2)) { // We have a row shape by design.
//            for (int i=0; i<x_len; i++) {
//                b1 = 0.5 - (i    )/(1.0*x_len);
//                b2 = 0.5 - (i+1.0)/(1.0*x_len);
//                // If we're in a split cell only calculate the triangle area,
//                // else the area will be zero.
//                A = 0.5*fabs((fabs(b1+b2)>eps)?(b1+b2):(b1));

//                if ((type ==   2) || (type ==   4)) { // Z-shapes
//                    if (b1 > 0) {
//                        c_old = pix[idx(w,h,x_start+i,y_start)];
//                        c_opp = pix[idx(w,h,x_start+i,y_end  )];
//                        row[idx(w,h,x_start+i,y_start)] = (1-A)*c_old + A*c_opp;
//                    }
//                    if (b2 < 0) {
//                        c_old = pix[idx(w,h,x_start+i,y_end  )];
//                        c_opp = pix[idx(w,h,x_start+i,y_start)];
//                        row[idx(w,h,x_start+i,y_end  )] = (1-A)*c_old + A*c_opp;
//                    }

//                }
//                if (type ==   8) { // U-shape
//                    c_old = pix[idx(w,h,x_start+i,y_start  )];
//                    c_opp = pix[idx(w,h,x_start+i,y_start+1)];
//                    //row[idx(w,h,x_start+i,y_start)] = (1-A)*c_old + A*c_opp;

//                }
//                if (type ==  16) { // U-shape
//                    c_old = pix[idx(w,h,x_start+i,y_start  )];
//                    c_opp = pix[idx(w,h,x_start+i,y_start-1)];
//                    //row[idx(w,h,x_start+i,y_start)] = (1-A)*c_old + A*c_opp;

//                }
//                if (type ==  32) { // L-shape
//                    // TODO
//                }
//                if (type ==  64) { // L-shape
//                    // TODO
//                }
//                if (type == 128) { // L-shape
//                    // TODO
//                }
//                if (type == 256) { // L-shape
//                    // TODO
//                }
//            }
//        }
//        else {
//            for (int j=0; j<y_len; j++) {
//                b1 = 0.5 - (j    )/(1.0*y_len);
//                b2 = 0.5 - (j+1.0)/(1.0*y_len);
//                // If we're in a split cell only calculate the triangle area,
//                // else the area will be zero.
//                A = 0.5*fabs((fabs(b1+b2)>eps)?(b1+b2):(b1));

//                if ((type ==   3) || (type ==   5)) { // Z-shapes
//                    if (b1 > 0) {
//                        c_old = pix[idx(w,h,x_start  ,y_start+j)];
//                        c_opp = pix[idx(w,h,x_end    ,y_start+j)];
//                        col[idx(w,h,x_start  ,y_start+j)] = (1-A)*c_old + A*c_opp;
//                    }
//                    if (b2 < 0) {
//                        c_old = pix[idx(w,h,x_end    ,y_start+j)];
//                        c_opp = pix[idx(w,h,x_start  ,y_start+j)];
//                        col[idx(w,h,x_end    ,y_start+j)] = (1-A)*c_old + A*c_opp;
//                    }

//                }
//                if (type ==   9) { // U-shape
//                    c_old = pix[idx(w,h,x_start  ,y_start-j)];
//                    c_opp = pix[idx(w,h,x_start+1,y_start-j)];
//                    //col[idx(w,h,x_start  ,y_start-j)] = (1-A)*c_old + A*c_opp;

//                }
//                if (type ==  17) { // U-shape
//                    c_old = pix[idx(w,h,x_start  ,y_start-j)];
//                    c_opp = pix[idx(w,h,x_start-1,y_start-j)];
//                    //col[idx(w,h,x_start  ,y_start-j)] = (1-A)*c_old + A*c_opp;

//                }
//                if (type ==  33) {
//                    // TODO
//                }
//                if (type ==  65) {
//                    // TODO
//                }
//                if (type == 129) {
//                    // TODO
//                }
//                if (type == 257) {
//                    // TODO
//                }
//            }
//        }
//    }

//    for (int i=0; i<w*h; i++) {
//        // Update the picture (pix) with the biggest change
//        pix[i] = (abs(pix[i]-row[i])>abs(pix[i]-col[i]))?(row[i]):(col[i]);
//    }

//    delete[] row;
//    delete[] col;
//}

//void Slice::testimage(QImage &img)
//{
//    int w = img.width();
//    int h = img.height();
//    int ind =   0;

//    int *fill = new int[w*h];

//    for (int i=0; i<w*h; i++) fill[i] = 255;

//    int *pix = new int[w*h];

//    // Create an image with some features
//    for (int i=0; i<w; i++) {
//        for (int j=0; j<h; j++) {
//            ind = idx(w,h,i,j);
//            QRgb rgb(img.pixel(i, j));
//            QColor color(rgb);
//            int gray = (color.red()*19595 + color.green()*38469 + color.blue()*7472) >> 16;
//            pix[ind] = gray;
//            //            pix[ind] = 255; // Make sure we fill a color;
//            //            if (   i         < w/4) pix[ind] = 127;
//            //            if (     j       < h/4) pix[ind] =  63;
//            //            if (   i+j - h/4 < h  ) pix[ind] =  31;
//            //            if (-2*i+j + 4*h < w  ) pix[ind] =   0;
//        }
//    }
//    //    writeImg(pix,w,h,"raw.bmp");

//    int *edge = new int[w*h];
//    for (int i=0; i<w*h; i++) edge[i] = 0;

//    edgeDetect(w, h, pix, edge);

//    int *edgepix = new int[w*h];
//    for (int i=0; i<w; i++) {
//        for (int j=0; j<h; j++) {
//            ind = idx(w,h,i,j);
//            edgepix[ind] = 255;
//            if (edge[ind]) edgepix[ind] = edge[ind];
//        }
//    }
//    //        writeImg(edgepix,w,h,"edge.bmp");

//    std::vector<Shape> shapes;
//    findShapesCol(w, h, edge, shapes);
//    findShapesRow(w, h, edge, shapes);

//    int *shapepix = new int[w*h];
//    for (int i=0; i<w*h; i++) {shapepix[i] = 255;}
//    blend(w,h,pix,shapes);
//    for (int i=0; i<w; i++) {
//        for (int j=0; j<h; j++) {
//            ind = idx(w,h,i,j);
//            img.setPixel(i,j,qRgb(pix[ind],pix[ind],pix[ind]));
//        }
//    }
//    img.mirrored(false,true);
////    img.save("H:img.jpg","JPG");

////    writeImg(pix,w,h,"mlaa.bmp");
////    FILE*stream;
////    if((stream=fopen("mlaa.bmp","r")) != NULL)
////    {
////        QImage image("mlaa.bmp");
////        img = image;
////    }
////    .mirrored(false, true)
////    for (int i=0; i<256; i++) {
////        double grey = mapNumbers(i);
////        qDebug() << "grey==" << grey;
////    }
//}

double Slice:: mapNumbers(double x, int type)
{
    double gray;
    if (type == 0) {//255->15
        gray = floor(x * 15/255);
        if (gray > 15) {
            gray = 15;
        }else if (gray < 0) {
            gray = 0;
        }
        return gray;
    }else if (type == 1) {//15->255
        gray = floor(x * 255/15);
        if (gray > 255) {
            gray = 255;
        }else if (gray < 0) {
            gray = 0;
        }
        return gray;
    }else{
        gray = floor(x * 15/255);
        if (gray > 15) {
            gray = 15;
        }else if (gray < 0) {
            gray = 0;
        }
        return gray;
    }
}
//double Slice:: mapNumbers(double x, long xMin, long xMax, long yMin, long yMax)
//{
//    return ((double)(x - xMin) * (yMax - yMin))/((double)(xMax - xMin)) + yMin;
//}
