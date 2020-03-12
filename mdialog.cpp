#include "mdialog.h"

MDialog::MDialog(MainWindow *mparent) : QDialog(NULL)
{
    this->mparent = mparent;
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);
    mainLayout = new QVBoxLayout();
    titlebar = new MTitleBar();
    mainLayout->addWidget(titlebar);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    this->setLayout(mainLayout);
    connect(titlebar, SIGNAL(OnClose()), this, SLOT(tbClose()));
    connect(titlebar, SIGNAL(OnMinimize()), this, SLOT(tbMinimize()));
    connect(titlebar, SIGNAL(OnMove(QPoint)), this, SLOT(tbMove(QPoint)));
    type = "None";
    this->setStyleSheet("border-width:1px;border-color:#126e58;");
    mp = new QPainter();
    intvalidator = new QIntValidator();
    guidelayout = new QVBoxLayout();
    guidelabel = new QLabel();
}

void MDialog::setDType(QString type)
{
    this->type = type;
    initPanel();
}

void MDialog::setCancel()
{
    if(type == "Progress")
    {
        progresslabel->setText(mparent->dialog_cancel);
    }
}

void MDialog::initPanel()
{
    if(type == "Progress")
    {
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
        mainLayout->addLayout(progresslayout, 1);
        mainLayout->layout();
    }else if(type == "slicefinish")
    {
    }else if(type == "drag")
    {
        setTitle(mparent->dialog_preview);
        dragwidget = new QWidget();
        draglayout = new QVBoxLayout();
        draglayout->setMargin(0);
        draglayout->setSpacing(0);
        draglabel = new QLabel();
        draglabel->setMinimumHeight(24);
        draglabel->setStyleSheet("background-color:#e6e6e6;font:12px;margin-left:1px;margin-right:1px;");
        dragedit = new QLineEdit();
        dragedit->setMaximumWidth(50);
        dragedit->setMinimumHeight(20);
        dragedit->setStyleSheet("margin-top:10px;margin-bottom:10px;font:14px;");
        dragedit->setValidator(intvalidator);
        connect(dragedit, SIGNAL(textChanged(const QString &)), this, SLOT(setLayer(QString)));
        dragcanvas = new MCanvas();
        pb = new MProgressBar();
        pb->setMinimumSize(750, 20);
        pb->setType(type);
        connect(pb, SIGNAL(sliderChange(QString)), dragedit, SLOT(setText(QString)));
        draglayout->addWidget(draglabel);
        draglayout->addWidget(dragcanvas, 1);
        draglayout->addWidget(pb);
        draglayout->addWidget(dragedit);
        dragwidget->setLayout(draglayout);
        draglayout->setAlignment(dragedit, Qt::AlignCenter);
        setPanel(dragwidget);
    }else if (type == "guide") {
//        setTitle("使用指引");
//        guidelayout = new QVBoxLayout();
//        guidelayout->setMargin(0);
//        guidelabel = new QLabel();
//        guidelabel->setText("一");
//        guideimage = new QPainter();
////        guideimage->load(":/resource/icon/direction.png");
//        guideimage->begin(this);
//        guideimage->drawImage(100,100,QImage(":/resource/icon/direction.png"));
//        guideimage->end();
//        QImage image;
//        QPalette palette;
//        image.load("picture_path/name.png"); // 指定图片所在位置及图片名
//        palette.setBrush(this->backgroundRole(),QBrush(image));
//        guidewidget->setPalette(palette);
//        guidelayout->addWidget(guidelabel);
//        guidewidget->setLayout(guidelayout);
//        setPanel(guidewidget);
    }
}
void MDialog::initErrorType(QString text){
//    guidewidget = new QWidget();
    guidelabel->setText(text);
    guidelabel->setAlignment(Qt::AlignCenter);
    guidelayout->addWidget(guidelabel);
//    guidewidget->setLayout(guidelayout);
    mainLayout->addLayout(guidelayout, 1);
    mainLayout->layout();
//    setPanel(guidewidget);
}
void MDialog::setPanel(QWidget *md)
{
    mainLayout->addWidget(md, 1);
    mainLayout->layout();
}

void MDialog::setTitle(QString tit)
{
    titlebar->setTitle(tit);
}

void MDialog::tbClose()
{    
//    while (resultlayer.size()>0) {
//        resultlayer.erase(resultlayer.begin());
//    }
    this->close();
    if (mparent->isChange) {
//        qApp->activeWindow()->hide();
        mparent->close();
    }
}

void MDialog::tbMove(QPoint p)
{
    QPoint np = this->pos();
    this->move(p+np);
}

void MDialog::tbMinimize()
{
    this->setWindowState(Qt::WindowMinimized);
}

void MDialog::btnPress()
{
    QPushButton *mb = (QPushButton*) sender();
    if(type == "Progress" || type == "None")
    {
        mb->setStyleSheet("background-color:#0b4739;font:14px;color:#cacaca;border:0px;");
    }
}

void MDialog::btnRelease()
{
    QPushButton *mb = (QPushButton*) sender();
    if(type == "Progress" || type == "None")
    {
        mb->setStyleSheet("background-color:#126e58;font:14px;color:#ffffff;border:0px;");
        if(mb->objectName() == "cancel")
        {
            emit OnCancel();
        }
    }
}

void MDialog::setProgress(int progress)
{
    if(type == "Progress")
    {
        pb->setProgress(progress);
    }
}

void MDialog::setDragText(float thickness, int exposetime, int offlight, int botexpose, int botcount)
{
    if(type == "drag")
    {
        setTitle(mparent->dialog_preview);
        et = exposetime;
        ol = offlight;
        be = botexpose;
        bc = botcount;
        draglabel->setText(mparent->dialog_dragtext
                           .arg(QString::number(thickness)).arg(QString::number(exposetime))
                           .arg(QString::number(offlight)).arg(QString::number(botexpose))
                           .arg(QString::number(botcount)));
    }
}

void MDialog::paintEvent(QPaintEvent *event)
{
    int width,height;
    width = this->width();
    height = this->height();
    mp->begin(this);
    mp->setPen(QPen(QColor(18, 110, 88), 1));
    mp->drawRect(0, 0, width-1, height-1);
    mp->end();
}

void MDialog::initData(QVector2D solution, std::vector<layerresult> resultlayer)
{
    if(type == "drag")
    {
        this->resolution = solution;
        this->resultlayer = resultlayer;
        this->slicing = false;
        nowimg = QImage(resolution.x(), resolution.y(), QImage::Format_RGB32);
        pb->setProgress(0);
        pb->setMaxs(resultlayer.size()-1);
        dragedit->setText("0");
    }
}

void MDialog::initDataCopy(QVector2D solution,int max_size)
{
    if(type == "drag")
    {
        QString bd;
        getData("mksdlp_slicetype", bd, "0");
        slicetype = bd.toInt();
//        slicetype = 1;
        this->resolution = solution;
        this->slicing = true;
        nowimg = QImage(resolution.x(), resolution.y(), QImage::Format_RGB32);
        pb->setProgress(0);
        pb->setMaxs(max_size);
        dragedit->setText("0");
    }
}
void MDialog::getData(QString datakey, QString &data, QString defaultvalue)
{
    QSettings settings("makerbase", "mksdlp");
    data = settings.value(datakey).toString();
    if(data == "")
    {
        settings.setValue(datakey, defaultvalue);
        data = defaultvalue;
    }
}

void MDialog::getImg(int value,std::vector<QImage> &imagePaths)
{
    //判断路径是否存在
    QDir dir("H:/dlppng");
    if(!dir.exists()){
        return;
    }
    //查看路径中后缀为.jpg格式的文件
    QStringList filters;
    filters<<QString("*.png");
    dir.setFilter(QDir::Files | QDir::NoSymLinks); //设置类型过滤器，只为文件格式
    dir.setNameFilters(filters);  //设置文件名称过滤器，只为filters格式
    //统计jpg格式的文件个数
    int dir_count = dir.count();
    if(dir_count <= 0)
        return;
    //存储文件名称
    QImage img;
    for(int i=0; i<dir_count; i++)
    {
//        qDebug() << "dir[i]---" << dir[i];
        QString file_name = dir[i];  //文件名称
        QStringList list = file_name.split("_");        
        if (list[0] == QString::number(value)){
            img = QImage("H:/dlppng/"+file_name);
            imagePaths.push_back(img);
        }
    }
}

void MDialog::setLayer(QString layer)
{
//    qDebug() << "layer---" << layer;
    int ll;
    bool cantrans = false;
    ll = layer.toInt(&cantrans);
    if(cantrans)
    {
        pb->setProgress(ll);
//        if (slicing) {
//            //20200224
//            MSliderChangeCopy(ll);
//        }else {
//            MSliderChange(ll);
//        }
        if (mparent->newSliceType) {
            MSliderChange(ll);
        }else {
            MSliderChangeOld(ll);
        }
    }
}

void MDialog::setLabelText(QString text)
{
    progresslabel->setText(text);
}

void MDialog::MSliderChangeCopy(int value)
{
    //切片，读取本地图片
    std::vector<QImage> imglist;
    getImg(value,imglist);
    QPainter qp;
    nowimg.fill(QColor(0, 0, 0));
    std::vector<whitedata> eachlayer;
    whitedata wd;
    imgToLayer(imglist,eachlayer);
    qp.begin(&nowimg);
    qDebug() << "eachlayer.size()" << eachlayer.size();
    for(int i = 0; i < eachlayer.size(); i++)
    {
        wd = eachlayer[i];
        int gray = mapNumbers(wd.gray,1);
        qp.setPen(QColor(gray, gray, gray));
        qp.drawLine(wd.sp.x(), wd.sp.y(), wd.ep.x(), wd.ep.y());
    }
    qp.end();
    dragcanvas->setImage(nowimg);
}

void MDialog::MSliderChange(int value)
{
    if(resultlayer.size() < value)
    {
        return;
    }
//    qDebug() << "nowimg.width()--1-----------" << nowimg.width();
//    int count = 0;
//    while (nowimg.width() == 0) {
//        nowimg = QImage(resolution.x(), resolution.y(), QImage::Format_RGB32);
//        count++;
//        if (count > 30) {
//            mparent->showToast("error，请重试",5);
//            this->tbClose();
//            break;
//        }
//    }
//    qDebug() << "nowimg.width()--2------------" << nowimg.width();
    nowimg.fill(QColor(0, 0, 0));
    layerresult lr = resultlayer[value];
    whitedata wd;
    QPainter qp;
    qp.begin(&nowimg);
//    while (!qp.isActive()) {
//        qDebug() << "Painter not active";
//        qp.begin(&nowimg);
//    }
//    qp.setPen(QColor(255, 255, 255));
    for(int i = 0; i < lr.layerimgdata.size(); i++)
    {
        wd = lr.layerimgdata[i];
//        qDebug() << "pre_gray=" << wd.gray;
        int gray = mapNumbers(wd.gray,1);
        qp.setPen(QColor(gray, gray, gray));
        qp.drawLine(wd.sp.x(), wd.sp.y(), wd.ep.x(), wd.ep.y());
    }
    qp.end();
//    QDir dir;
//    if(!dir.exists("dlppng")){
//        this->mparent->showToast("error",5);
//        return;
//    }
//    nowimg = QImage("/dlppng/" + QString::number(value) + ".png");
    dragcanvas->setImage(nowimg);
}

void MDialog::MSliderChangeOld(int value)
{
//    qDebug() << "resultlayer.size()---MSliderChangeOld--------" << resultlayer.size();
    if(resultlayer.size() < value)
    {
        return;
    }
//    qDebug() << "nowimg.width()--1---MSliderChangeOld--------" << nowimg.width();
    int count = 0;
    while (nowimg.width() == 0) {
        nowimg = QImage(resolution.x(), resolution.y(), QImage::Format_RGB32);
        count++;
        if (count > 30) {
            mparent->showToast(tr("Error, please try again later"),5);
            this->tbClose();
            break;
        }
    }
//    qDebug() << "nowimg.width()--2------------" << nowimg.width();
    nowimg.fill(QColor(0, 0, 0));
    layerresult lr = resultlayer[value];
    whitedata wd;
    QPainter qp;
    qp.begin(&nowimg);
//    while (!qp.isActive()) {
//        qDebug() << "Painter not active";
//        qp.begin(&nowimg);
//    }

    for(int i = 0; i < lr.layerimgdata.size(); i++)
    {
        wd = lr.layerimgdata[i];
        qp.setPen(QColor(255, 255, 255));
        qp.drawLine(wd.sp.x(), wd.sp.y(), wd.ep.x(), wd.ep.y());
    }
    qp.end();
    dragcanvas->setImage(nowimg);
}


double MDialog:: mapNumbers(double x, int type)
{
    if (type == 0) {//255->15
        return floor(x * 15/255);
    }else if (type == 1) {//15->255
        return floor(x * 255/15);
    }else{
        return floor(x * 15/255);
    }
}

void MDialog::imgToLayer(std::vector<QImage> imglist,std::vector<whitedata> &resultlist){
    int star_x = 0, star_y = 0, end_x = resolution.x(), end_y = resolution.y();
    //把图片转为坐标点
    whitedata wd;
    QVector2D sp(-1, -1), ep(-1, -1);
    int gray = 0;
    int gray2 = 0;
//    std::vector<whitedata> resultlist;
    int x, y, width;
    width = resolution.x();
    bool bCurColorIsWhite = false;
    int imagesize = end_x+end_y*width;
    unsigned currentpos = star_x + star_y*width;
    std::vector<uchar*> datalist;

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
//    resultlayer.layerid = 0;
//    resultlayer.layerimgdata = resultlist;
}
