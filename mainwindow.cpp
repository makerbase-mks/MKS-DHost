#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "m3dviewer.h"
#include <QFileDialog>
#include <QSettings>
#include <loadingbar.h>
#include <math.h>
#include <previewdlg.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    toast_selectmodel = tr("At lease import one model");
    toast_fileerro = tr("Please choose .stl or .mdlp file");
    toast_mpdtext = tr("Saving to stl file:");
    toast_printtime = tr("estimated time:%1hour%2min");
    toast_preview = tr("Preview");
    toast_ok = tr("Save");
    dialog_slicing = tr("Slicing");
    dialog_cancel = tr("Cancel");
    dialog_preview = tr("Preview");
    add_supporting = tr("Auto Add Support");
    dialog_dragtext = tr("thickness:%1mm expose time:%2s off:%3s bottom expose:%4s bottom layer:%5");
    isChange = false;
    onestep = tr("one");
    twostep = tr("two");
    threestep = tr("three");
    fourstep = tr("four");
    fivestep = tr("five");
    sixstep = tr("six");
    Usage = tr("Usage guidelines");
    Previous = tr("Previous");
    Next = tr("Next step");
    Notremind = tr("Not remind");
    fileerror = tr("File error");
    filesave = tr("MDLP File is saving");
    filesaved = tr("MDLP File has been saved.");
    pleasesetfilename = tr("Please set file name.");
    stlsaved = tr("stl file has been saved.");

    newSliceType = false; //新的切片方式，包含抗锯齿、文件格式改变、切片参数减少
    antialiasing = false; //抗锯齿

//    // 获取命令行参数
    QStringList arguments = QCoreApplication::arguments();
    if (arguments.count() > 2) {
        QString str = arguments.at(1);
        if (str == "change") {
            QString str2 = arguments.at(2);
            initChangeUI();
//            changeFile(str2);
        }
    }else if (arguments.count() > 1) {
        QString str = arguments.at(1);
        initUI();
        loadFile(str);
    }else {
        initUI();
        QString showguide;
        getData("mksdlp_showguide",showguide,"0");
        if (showguide == "0") {
            showGuide();
        }
    }
}

MainWindow::~MainWindow()
{
    delete mview;
//    delete rightPanel;
    delete nrightPanel;
    delete ui;
}

void MainWindow::initUI()
{
    ui->setupUi(this);
    this->setMinimumSize(1024, 768);
    centralWidget()->layout()->setMargin(0);
    statusBar()->hide();
    this->setWindowTitle("MKS DHOST");
//    QStringList arguments = QCoreApplication::arguments();
//    QString args;
//    for (int i=0; i<arguments.length();i++) {
//       args += " i = " + arguments[i];
//    }
//    this->setWindowTitle(args);
    ui->mainToolBar->hide();
    mview = new M3DViewer(this, NULL);
    nrightPanel = new NRightPanel(this, NULL);
    ui->ViewWindow->setMargin(0);
    ui->ViewWindow->setSpacing(0);
//    rightPanel = new RightPanel(this, NULL);
    ui->ViewWindow->addWidget(mview, 1);
//    ui->ViewWindow->addWidget(rightPanel);
    ui->ViewWindow->addWidget(nrightPanel);
    mview->show();
//    this->pd = new PreViewDialog();
    pvdialog = new MDialog(this);
    pvdialog->setDType("drag");
    pvdialog->setMaximumSize(750, 480);
    pvdialog->setMinimumSize(750, 480);
    ui->menu_File->setTitle(tr("File"));
    ui->actionSave->setEnabled(false);
    ui->actionOpen->setText(tr("Open"));
    ui->actionSave->setText(tr("Slice"));
    ui->actionExit->setText(tr("Exit"));
    ui->menuHelp->setTitle(tr("Help"));
    ui->menuAbout->setTitle(tr("About"));
    ui->actionTips->setText(tr("Usage guidelines"));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(loadscene()));
//    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(savedlp()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(savedlpfile()));

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(QApplication::quit()));
    connect(ui->actionTips, SIGNAL(triggered()), this, SLOT(showGuide()));
    connect(mview, SIGNAL(OnModelSelected(int)), nrightPanel, SLOT(OnModelSelected(int)));
    connect(nrightPanel, SIGNAL(ChangeMode(QString)), mview, SLOT(setMod(QString)));
    connect(nrightPanel, SIGNAL(ChangeType(QString)), mview, SLOT(setSType(QString)));
    connect(nrightPanel, SIGNAL(ChangeShape(QString)), mview, SLOT(setShape(QString)));
    connect(nrightPanel, SIGNAL(DelAllSupport()), mview, SLOT(delAllSupport()));
    SyncData();
    sliceresult = new Slice(this);
    mThread = new updateThread();
    adding = false;
    finishdialog = new MDialog(this);
    finishdialog->setMinimumSize(240, 128);
    connect(mThread, SIGNAL(addmodel()), this, SLOT(addmodels()));
    connect(mThread, SIGNAL(updateProgress(int)), mview, SLOT(updateProgress(int)));
    errordialog = new MDialog(this);
    errordialog->setMinimumSize(240, 128);
//    ui->centralWidget->hide();
    ui->label->hide();

    saveprocessdialog = new MDialog(this);
    saveprocessdialog->setDType("Progress");
    saveprocessdialog->setLabelText(filesave);
    saveprocessdialog->setProgress(0);
    saveprocessdialog->progressbtn->hide();
}

void MainWindow::initChangeUI()
{
    ui->setupUi(this);
    this->setMinimumSize(240,50);
    this->setMaximumSize(240,50);
    centralWidget()->layout()->setMargin(0);
    statusBar()->hide();
    this->setWindowTitle("MKS DHOST");
//    QStringList arguments = QCoreApplication::arguments();
//    QString args;
//    for (int i=0; i<arguments.length();i++) {
//       args += " i = " + arguments[i];
//    }
//    this->setWindowTitle(args);
    ui->mainToolBar->hide();
    ui->menuBar->hide();
    mview = new M3DViewer(this, NULL);
    nrightPanel = new NRightPanel(this, NULL);
    pvdialog = new MDialog(this);
    pvdialog->setDType("drag");
    pvdialog->setMaximumSize(750, 480);
    pvdialog->setMinimumSize(750, 480);
    sliceresult = new Slice(this);
    mThread = new updateThread();
    adding = false;
    finishdialog = new MDialog(this);
    finishdialog->setMinimumSize(240, 128);
    connect(mThread, SIGNAL(addmodel()), this, SLOT(addmodels()));
    connect(mThread, SIGNAL(overloadmodel()), this, SLOT(overloadmodels()));
    connect(mThread, SIGNAL(updateProgress(int)), mview, SLOT(updateProgress(int)));
    errordialog = new MDialog(this);
    errordialog->setMinimumSize(240, 128);
    mview->hide();
    nrightPanel->hide();
//    ui->centralWidget->hide();
//    changeview = new Changeview(this,NULL);
//    ui->ViewWindow->addWidget(changeview);
//    changeview->show();
/*    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    QLabel *label = new QLabel();
    label->setText("格式转换");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    widget->setLayout(layout)*/;
}

void MainWindow::SyncData()
{
//    rightPanel->SyncSupportData();
    nrightPanel->SyncSupportData();
}

void MainWindow::OutputScreenShot(QDataStream &out)
{
    mview->outputScreenShot(out);
}

void MainWindow::UpdatePreViewDialog(QVector2D solution, std::vector<layerresult> resultlayer)
{
//    this->pd->initData(solution, resultlayer);
    pvdialog->initData(solution, resultlayer);
}

void MainWindow::UpdatePreViewDialogCopy(QVector2D solution,int max_size)
{
    pvdialog->initDataCopy(solution,max_size);
}

void MainWindow::ShowErrorDialog(QString text)
{
    errordialog->close();
    errordialog->initErrorType(text);
    errordialog->show();
}

void MainWindow::UpdatePreViewDetail(float thickness, int exposetime, int offlight, int botexpose, int botcount)
{
    pvdialog->setDragText(thickness, exposetime, offlight, botexpose, botcount);
}

void MainWindow::loadFile(QString filename) {
    QFileInfo info(filename);
    if(info.suffix().toLower() == "stl")
    {
        adding = true;
        QString bd;
//        loader = new ModelLoader(filename);
//        mThread->initReadThread(loader);
//        ModelData* newdata = loader->getTriList();
        smd = new ModelData(filename);
        mThread->initLoadThread(smd);
        this->getData("mksdlp_thickness", bd, "0.1");
        thickness = bd.toFloat();
        mThread->start();
//        newdata->PreGenerateLoop(thickness);
//        md_list.push_back(newdata);
//        connect(newdata, SIGNAL(updateProgress(int)), mview, SLOT(updateProgress(int)));
    }else if(info.suffix().toLower() == "mdlp")
    {
        bool isMKSFile = false;
//        sliceresult = Slice(this);
//        sliceresult->loadMdlp(filename, isMKSFile);
        if (newSliceType) {
            sliceresult->loadMdlp(filename, isMKSFile);
        }else {
            sliceresult->loadMdlpOld(filename, isMKSFile);
        }
        if(isMKSFile)
        {
//            showPreView();
            showPreViewcopy();
        }
    }else if(info.suffix().toLower() == "zip")
    {
        loadZip(filename);
    }else if(info.suffix().toLower() == "cws")
    {
        loadCWS(filename);
    }else{
        mview->showToast(toast_fileerro,2);
    }
    updateActionSave();
}

void MainWindow::changeFile(QString filename) {
    isChange = true;
    QFileInfo info(filename);
    if(info.suffix().toLower() == "stl")
    {
        adding = true;
        QString bd;
        smd = new ModelData(filename);
        mThread->changeLoadThread(smd);
        this->getData("mksdlp_thickness", bd, "0.1");
        thickness = bd.toFloat();
        mThread->start();
    }else if(info.suffix().toLower() == "mdlp")
    {
        bool isMKSFile = false;
//        sliceresult->loadMdlp(filename, isMKSFile);
        if (newSliceType) {
            sliceresult->loadMdlp(filename, isMKSFile);
        }else {
            sliceresult->loadMdlpOld(filename, isMKSFile);
        }
        if(isMKSFile)
        {
//            qDebug() << "已经是mdlp格式，不需要转换end";
//            ui->label->setText(tr("nochange"));
            errordialog->close();
//            errordialog->initErrorType(tr("nochange"));
//            errordialog->show();
            ShowErrorDialog(tr("nochange"));
//            showPreView();
        }
    }else if(info.suffix().toLower() == "zip")
    {
        loadZip(filename);
    }else if(info.suffix().toLower() == "cws")
    {
        loadCWS(filename);
    }else{
        ShowErrorDialog(tr("limitfile"));
//        ui->label->setText(tr("limitfile"));/*仅支持stl/zip/cws文件*/
//        mview->showToast(toast_fileerro,2);
    }
//    updateActionSave();
}
void MainWindow::loadscene()
{
    if(adding)
    {
        return;
    }
    QSettings settings("makerbase", "mksdlp");
    QString filename = QFileDialog::getOpenFileName(this,
             tr("Open stl"), settings.value("loadpath").toString(), tr("stl|mdlp|zip|cws (*.stl;*.mdlp;*.zip;*.cws)"));
    if(filename.isEmpty())
    {
        return;
    }
    QFileInfo info(filename);
    settings.setValue("loadpath", info.path());
    if(info.suffix().toLower() == "stl")
    {
        qDebug() << "add model-----loadscene-------";
        adding = true;
        QString bd;
//        loader = new ModelLoader(filename);
//        mThread->initReadThread(loader);
//        ModelData* newdata = loader->getTriList();
        smd = new ModelData(filename);
        connect(smd, SIGNAL(loadfileerror()), mview, SLOT(loadfileerrors()));
        mThread->initLoadThread(smd);
        this->getData("mksdlp_thickness", bd, "0.1");
        thickness = bd.toFloat();
        mThread->start();
//        newdata->PreGenerateLoop(thickness);
//        md_list.push_back(newdata);
//        connect(newdata, SIGNAL(updateProgress(int)), mview, SLOT(updateProgress(int)));
    }else if(info.suffix().toLower() == "mdlp")
    {
        bool isMKSFile = false;
//        sliceresult = Slice(this);
//        sliceresult->loadMdlp(filename, isMKSFile);
        if (newSliceType) {
            sliceresult->loadMdlp(filename, isMKSFile);
        }else {
            sliceresult->loadMdlpOld(filename, isMKSFile);
        }
        if(isMKSFile)
        {
//            showPreView();
            showPreViewcopy();
        }
    }else if(info.suffix().toLower() == "zip")
    {
        loadZip(filename);
    }else if(info.suffix().toLower() == "cws")
    {
        loadCWS(filename);
    }else{
        mview->showToast(toast_fileerro,2);
    }
    updateActionSave();
}

void MainWindow::loadCWS(QString filename)
{
    MDialog *mpd = new MDialog(this);
    mpd->setDType("Progress");
    mpd->setProgress(0);
    mpd->show();
    connect(mpd, SIGNAL(OnCancel()), this, SLOT(CancelSlicing()));
    isCancel = false;
    bool error = false;
    sliceresult->initCWS(filename,error);
    if (error) {
        mpd->close();
        if (isChange) {
            ShowErrorDialog(fileerror);
//            ui->label->setText(fileerror);
        }else {
            mview->showToast(fileerror,2);
        }
        return;
    }
//    qDebug() << "initCWS";
    bool isFinish = false;
    int precent = 0;
    do{
        sliceresult->loadCWS(filename, isFinish, precent);
        mpd->setProgress(precent);
        QApplication::processEvents();
    }while(!isFinish && !isCancel);
    if(isCancel)
    {
        bool isfinish = false;
        mpd->setCancel();
        while(!isfinish)
        {
            sliceresult->checkAllThread(isfinish);
        }
        if (isChange) {
            this->close();
//            qApp->activeWindow()->hide();
        }
    }
    mpd->close();
    int totalsec = sliceresult->max_size*pvdialog->et + pvdialog->be*pvdialog->bc + 7*sliceresult->max_size;
    hour = totalsec/3600;
    minute = totalsec%3600/60;
    if(!isCancel)
    {
        if (isChange) {
//            qDebug() << "filedialog";
            filedialog();
        }else {
            showSliceFinish();
        }
    }
}

void MainWindow::loadZip(QString filename)
{
    MDialog *mpd = new MDialog(this);
    mpd->setDType("Progress");
    mpd->setProgress(0);
    mpd->show();
    connect(mpd, SIGNAL(OnCancel()), this, SLOT(CancelSlicing()));
    QString path = QApplication::applicationDirPath()+"/rf";
    bool errorfile = false;
    sliceresult->initZip(filename,errorfile);
    if (errorfile) {
        mpd->close();
        if (isChange) {
            ShowErrorDialog(fileerror);
        }else {
            mview->showToast(fileerror,2);
        }
        return;
    }
    bool isFinish = false;
    bool error = false;
    isCancel = false;
    int precent = 0;

    do{
        sliceresult->loadZip(filename, isFinish, precent,error);
        mpd->setProgress(precent);
        QApplication::processEvents();
    }while(!isFinish && !isCancel && !error);
    if(isCancel)
    {
        bool isfinish = false;
        mpd->setCancel();
        while(!isfinish)
        {
            sliceresult->checkAllThread(isfinish);
        }
        if (isChange) {
//            qApp->activeWindow()->hide();
            ui->label->setText(tr("Cancel"));
            mpd->close();
            this->close();
//            QTimer::singleShot(100,this,SLOT(close()));
        }
    }
    if(error) {
        ShowErrorDialog("文件错误");
    }
    mpd->close();
    int totalsec = sliceresult->max_size*pvdialog->et + pvdialog->be*pvdialog->bc + 7*sliceresult->max_size;
    hour = totalsec/3600;
    minute = totalsec%3600/60;
    if(!isCancel && !error)
    {
        if (isChange) {
            filedialog();
        }else {
            showSliceFinish();
        }
    }
}

void MainWindow::showPreView()
{
    //切片预览
//    this->pd->show();
    pvdialog->show();
//    pvdialog->MSliderChange(0);
//    pvdialog->MSliderChangeCopy(0);
    if (newSliceType) {
        pvdialog->MSliderChange(0);
    }else {
        pvdialog->MSliderChangeOld(0);
    }
}

void MainWindow::showPreViewcopy()
{
    //查看mdlp文件
//    this->pd->show();
    pvdialog->show();
    if (newSliceType) {
        pvdialog->MSliderChange(0);
    }else {
        pvdialog->MSliderChangeOld(0);
    }
}

std::vector<ModelData*> MainWindow::getModelInstance()
{
    return md_list;
}

void MainWindow::showToast(QString t, int tm)
{
    mview->showToast(t, tm);
}

void MainWindow::savedlp()
{
    if(md_list.size() < 1)
    {
        mview->showToast(toast_selectmodel, 2);
//        mview->showToast(QApplication::translate("MainWindow", "At lease import one model"),2);
        return;
    }
    mview->getScreenShot(); //模型预览图

    MDialog *mpd = new MDialog(this);
    mpd->setDType("Progress");
    mpd->setProgress(0);
    mpd->show();
    connect(mpd, SIGNAL(OnCancel()), this, SLOT(CancelSlicing()));
//    LoadingBar progressbar(0, 100);
//    QObject::connect(&progressbar,SIGNAL(rejected()),this,SLOT(CancelSlicing()));
//    progressbar.setDescription("准备切片");
//    progressbar.setValue(0);
    QApplication::processEvents();

//    sliceresult = Slice(this);
    QString bd;
    this->getData("mksdlp_pixelx", bd, "2560");
    int resolutionx = bd.toInt();
    this->getData("mksdlp_pixely", bd, "1440");
    int resolutiony = bd.toInt();
    sliceresult->setResolution(QVector2D(resolutionx, resolutiony));
    this->getData("mksdlp_thickness", bd, "0.1");
    thickness = bd.toFloat();
    sliceresult->thickness = thickness;
    this->getData("mksdlp_xsize", bd, "121");
    float platformx = bd.toFloat();
    this->getData("mksdlp_ysize", bd, "68.0625");
    float platformy = bd.toFloat();
    getData("mksdlp_slicetype", bd, "0");
    int slicetype = bd.toInt();
    sliceresult->setSliceType(slicetype);
    sliceresult->setPlatform(QVector2D(platformx, platformy));
    std::vector<triangle> trilist;
    double ff = thickness*0.5;
    int totallayer = 0;
    double zd;
    ModelData* md;
    int tarsize = 0;
    int max_size = 0;
    int max_result = 0;
    for(int i = 0; i < md_list.size(); i++)
    {
        tarsize = 0;
        md = md_list[i];
        if(md->hasBase)
        {
            tarsize = md->getScale().z();
        }
        zd = fabs(md->mmax.z() - md->mmin.z()+md->getPosition().z()+tarsize);
        totallayer = ceil(zd/thickness);
        max_size = max_size + totallayer;
        if(totallayer > max_result)
        {
            max_result = totallayer;
        }
//        for(unsigned int l = 0; l < totallayer; l++)
//        {
//            trilist = md->getZTri(l*thickness + ff);
//            sliceresult.addLayer(l, trilist, l*thickness + ff);
//        }
    }
    for(unsigned int l = 0; l < max_result; l++)
    {
        sliceresult->addH(l, l*thickness + ff); //第0层往上一点，避免拿不到第一层数据
    }
    sliceresult->setMaxSize(max_result);
    sliceresult->slicepos = 0;

    getData("mksdlp_expose", bd, "8");
    int layertime = bd.toInt(); //曝光时间
    getData("mksdlp_blayer", bd, "3");
    int baselayer = bd.toInt(); //底层数
    getData("mksdlp_bexpose", bd, "60");
    int baselayerexpose = bd.toInt(); //底层曝光时间
    getData("mksdlp_lftime", bd, "3");
    int lightofftime = bd.toInt(); //关灯时间（每层都会先移动->开灯->关灯）
    int lftimeormovetime = 7*totallayer;//7为每层默认打印时间
    if (7*totallayer < lightofftime*totallayer){
        lftimeormovetime = lightofftime*totallayer;
    }
    int totalsec = totallayer*layertime + baselayerexpose*baselayer + lftimeormovetime;
    hour = totalsec/3600;
    minute = totalsec%3600/60;
    int precent;
    isCancel = false;
    sliceresult->isSaveZip = false;
    bool slfinish = false;
    sliceresult->canstop = false;
    sliceresult->count = 0;
//    sliceresult->clearFiles("H:\dlppng");
//    QDir dir;
//    if(!dir.exists("dlppng")){
//        dir.mkdir("dlppng");
//    }
//    dir="dlppng";
//    sliceresult->clearFiles(dir);
//    qDebug() << "sliceresult->startslice";
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    qDebug() << "slice---start---" << current_date;
    do{
//        sliceresult.checkslice(slfinish, precent);
        sliceresult->startslice(slfinish, precent, true);
//        progressbar.setValue(precent);
        mpd->setProgress(precent);
        QApplication::processEvents();
//        sleep(5);
    }while(!slfinish && !isCancel);
    if(isCancel)
    {
        bool isfinish = false;
        mpd->setCancel();
        while(!isfinish)
        {
            sliceresult->checkAllThread(isfinish);
        }
    }
    sliceresult->canstop = true;
    QDateTime current_date_time1 =QDateTime::currentDateTime();
    QString current_date1 =current_date_time1.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    qDebug() << "slice---end---" << current_date1;
    this->UpdatePreViewDetail(thickness, layertime, lightofftime, baselayerexpose, baselayer);
    mpd->close();
    if(!isCancel)
    {
//        this->showPreView();
        if (isChange) {
//            ui->label->setText("ready filedialog");
            filedialog();
        }else {
            showSliceFinish();
        }
    }else {
        if (isChange) {
            this->close();
        }
    }
}

void MainWindow::savedlpfile()
{
    //边切片边保存
    if(md_list.size() < 1)
    {
        mview->showToast(toast_selectmodel, 2);
//        mview->showToast(QApplication::translate("MainWindow", "At lease import one model"),2);
        return;
    }

    QSettings settings("makerbase", "mksdlp");
    QString defaultname = settings.value("savepath").toString() + "/";
    if(md_list.size()>0)
    {
        smd = md_list[0];
        QFileInfo minfo(smd->filename);
        defaultname  = defaultname+minfo.baseName();
    }
    QString filename = QFileDialog::getSaveFileName(this, tr("Export Slices"),
                                                defaultname,
                                                tr("mdlp (*.mdlp)"));
    if(filename.isEmpty())
    {
        mview->showToast(pleasesetfilename, 5);
        return;
    }
    QFileInfo info(filename);
    settings.setValue("savepath", info.path());

//    qDebug()<< smd->getOutputListSize() << smd->baselist.size();
    int sizecount = 0;
    for(int i=0;i<md_list.size();i++) {
        sizecount += md_list[i]->getOutputListSize();
    }
    if (sizecount > 2500000) {
        sliceresult->initSliceThreadCount(1);
    }else{
        sliceresult->initSliceThreadCount(8);
    }

//    sliceresult->initSliceThreadCount(6);

    mview->getScreenShot(); //模型预览图

    MDialog *mpd = new MDialog(this);
    mpd->setDType("Progress");
    mpd->setProgress(0);
    mpd->show();
    connect(mpd, SIGNAL(OnCancel()), this, SLOT(CancelSlicing()));
//    LoadingBar progressbar(0, 100);
//    QObject::connect(&progressbar,SIGNAL(rejected()),this,SLOT(CancelSlicing()));
//    progressbar.setDescription("准备切片");
//    progressbar.setValue(0);
    QApplication::processEvents();

//    sliceresult = Slice(this);
    QString bd;
    this->getData("mksdlp_pixelx", bd, "2560");
    int resolutionx = bd.toInt();
    this->getData("mksdlp_pixely", bd, "1440");
    int resolutiony = bd.toInt();
    sliceresult->setResolution(QVector2D(resolutionx, resolutiony));
    this->getData("mksdlp_thickness", bd, "0.1");
    thickness = bd.toFloat();
    sliceresult->thickness = thickness;
    this->getData("mksdlp_xsize", bd, "121");
    float platformx = bd.toFloat();
    this->getData("mksdlp_ysize", bd, "68.0625");
    float platformy = bd.toFloat();
    getData("mksdlp_slicetype", bd, "0");
    int slicetype = bd.toInt();
    sliceresult->setSliceType(slicetype);
    sliceresult->setPlatform(QVector2D(platformx, platformy));
    std::vector<triangle> trilist;
    double ff = thickness*0.5;
    int totallayer = 0;
    double zd;
    ModelData* md;
    int tarsize = 0;
    int max_size = 0;
    int max_result = 0;
    for(int i = 0; i < md_list.size(); i++)
    {
        tarsize = 0;
        md = md_list[i];
        if(md->hasBase)
        {
            tarsize = md->getScale().z();
        }
        zd = fabs(md->mmax.z() - md->mmin.z()+md->getPosition().z()+tarsize);
        totallayer = ceil(zd/thickness);
        max_size = max_size + totallayer;
        if(totallayer > max_result)
        {
            max_result = totallayer;
        }
//        for(unsigned int l = 0; l < totallayer; l++)
//        {
//            trilist = md->getZTri(l*thickness + ff);
//            sliceresult.addLayer(l, trilist, l*thickness + ff);
//        }
    }
    sliceresult->removeH();//清空所有层数据，避免取消切片残留
    for(unsigned int l = 0; l < max_result; l++)
    {
        sliceresult->addH(l, l*thickness + ff); //第0层往上一点，避免拿不到第一层数据
    }
    sliceresult->setMaxSize(max_result);
    sliceresult->slicepos = 0;

    getData("mksdlp_expose", bd, "8");
    int layertime = bd.toInt(); //曝光时间
    getData("mksdlp_blayer", bd, "3");
    int baselayer = bd.toInt(); //底层数
    getData("mksdlp_bexpose", bd, "60");
    int baselayerexpose = bd.toInt(); //底层曝光时间
    getData("mksdlp_lftime", bd, "3");
    int lightofftime = bd.toInt(); //关灯时间（每层都会先移动->开灯->关灯）
    int lftimeormovetime = 7*totallayer;//7为每层默认打印时间
    if (7*totallayer < lightofftime*totallayer){
        lftimeormovetime = lightofftime*totallayer;
    }
    int totalsec = totallayer*layertime + baselayerexpose*baselayer + lftimeormovetime;
    hour = totalsec/3600;
    minute = totalsec%3600/60;
    int precent;
    isCancel = false;
    sliceresult->isSaveZip = false;
    bool slfinish = false;
    sliceresult->canstop = false;
    sliceresult->count = 0;
    sliceresult->iswriting = false;
    sliceresult->nowLayerid = 0;

//    QDir dir;
//    if(!dir.exists("dlppng")){
//        dir.mkdir("dlppng");
//    }
//    dir="dlppng";
//    sliceresult->clearFiles(dir);

//    QDateTime current_date_time =QDateTime::currentDateTime();
//    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
//    qDebug() << "slice---start---" << current_date;

    sliceresult->setFilenameSave(filename);//write head

    do{
        sliceresult->startsliceSave(slfinish, precent,newSliceType);
        mpd->setProgress(precent);
        QApplication::processEvents();
    }while(!slfinish && !isCancel);
    if(isCancel)
    {
        bool isfinish = false;
        mpd->setCancel();
        while(!isfinish)
        {
            sliceresult->checkAllThread(isfinish);
        }
    }
    sliceresult->canstop = true;
//    QDateTime current_date_time1 =QDateTime::currentDateTime();
//    QString current_date1 =current_date_time1.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
//    qDebug() << "slice---end---" << current_date1;
    this->UpdatePreViewDetail(thickness, layertime, lightofftime, baselayerexpose, baselayer);
    mpd->close();

    if(!isCancel)
    {
//        this->showPreView();
//        if (isChange) {
//            ui->label->setText("ready filedialog");
//            filedialog();
//            this->close();
//        }else {
//            showSliceFinish();
            bool isMKSFile = false;
//            sliceresult->newloadMdlp(filename, isMKSFile);
            if (newSliceType) {
                sliceresult->loadMdlp(filename, isMKSFile);
            }else {
                sliceresult->loadMdlpOld(filename, isMKSFile);
            }
            if(isMKSFile)
            {
//                qDebug() << "open";
                showPreViewcopy();
            }else{
                mview->showToast(fileerror,5);
            }
//        }
    }else {
        if (isChange) {
            this->close();
        }
    }
}


void MainWindow::CancelSlicing()
{
    qDebug() << "CancelSlicing";
    isCancel = true;
}

void MainWindow::deleteModel(int key)
{
    md_list[key]->~ModelData();
    md_list.erase(md_list.begin()+key);
    qDebug() << "md_list-------" << md_list.size();
    updateActionSave();
}

void MainWindow::copyModel(int key, QVector3D position)
{
    if(key < 0 || key>md_list.size()-1)
    {
        return;
    }
    ModelData *smd = md_list[key];
    std::vector<triangle> templist = smd->getTrilist();

    for(int i=0; i<smd->baselist.size(); i++)
    {
        templist.erase(templist.end());
    }
    ModelData *md = new ModelData(templist, smd->getFilename());
    position.setX(-position.x());
    position.setY(-position.y());
    md->setPosition(position);
    md->setRotation(smd->getRotation());
    md->setScale(smd->getScale());
//    if (smd->hasBase) {
        md->setHasBase(smd->hasBase);
//    }else {
//        position.setZ(smd->getPosition().z());
//        md->setPosition(position);
//    }
    md->updateOutput();
    md_list.push_back(md);
}

void MainWindow::updateActionSave()
{
    ui->actionSave->setEnabled(false);
    if(md_list.size() > 0)
    {
        ui->actionSave->setEnabled(true);
    }
}

void MainWindow::getData(QString datakey, QString &data, QString defaultvalue)
{
    QSettings settings("makerbase", "mksdlp");
    data = settings.value(datakey).toString();
    if(data == "")
    {
        settings.setValue(datakey, defaultvalue);
        data = defaultvalue;
    }
}


void MainWindow::setData(QString datakey, QString data)
{
    QSettings settings("makerbase", "mksdlp");
    settings.setValue(datakey, data);
}

void MainWindow::savestl()
{
    if(md_list.size() < 1)
    {
        mview->showToast(toast_selectmodel, 2);
        return;
    }
    QSettings settings("makerbase", "mksdlp");
    QString filename = QFileDialog::getSaveFileName(this, tr("Export STL"),
                                                settings.value("savepath").toString(),
                                                tr("stl (*.stl)"));
    if(filename.isEmpty())
    {
        return;
    }
    MDialog *mpd = new MDialog(this);
    mpd->setDType("Progress");
    mpd->setLabelText(toast_mpdtext);
    mpd->setProgress(0);
    mpd->show();
    connect(mpd, SIGNAL(OnCancel()), this, SLOT(CancelSlicing()));
    QApplication::processEvents();
    std::vector<triangle> mdoutputlist;
    mdoutputlist.clear();
    ModelData *md;
    QVector3D tempmax, tempmin;
    for(int i = 0; i < md_list.size(); i++)
    {
        md = md_list[i];
        md->outputmodel(mdoutputlist);
//        md->setPosition(QVector3D(0, 0, 5));
//        md->getStri(60, mdoutputlist, 3, tempmax, tempmin);
    }
    QFile *pf = new QFile(filename);
    isCancel = false;
    char* head = new char[80];
    char* mkshead = "MakerBaseOutPut";
    char* detal = new char[2];
    char* facecount = new char[4];
    int mfc = mdoutputlist.size();
    itoa(mfc, facecount, 4);
    int datalen = 80;
    strcpy(head, mkshead);
    pf->open(QIODevice::WriteOnly);
    QDataStream out(pf);
    out.writeRawData(head, datalen);
    triangle tri;
    float x, y, z;
    int precent;
    out.writeRawData((char*)&mfc, 4);
//    out << (UINT32)mdoutputlist.size();
//    out.writeRawData(mdoutputlist.size(), 4);
    qDebug() << "mdoutputlist.size()==" << mdoutputlist.size();
    for(int i = 0; i < mdoutputlist.size(); i++)
    {
//        qDebug() << i;
        tri = mdoutputlist[i];
        x = tri.normal.x();
        y = tri.normal.y();
        z = tri.normal.z();
        out.writeRawData((char*)&x, 4);
        out.writeRawData((char*)&y, 4);
        out.writeRawData((char*)&z, 4);
        for(int v = 0; v < 3; v++)
        {
            x = tri.vertex[v].x();
            y = tri.vertex[v].y();
            z = tri.vertex[v].z();
            out.writeRawData((char*)&x, 4);
            out.writeRawData((char*)&y, 4);
            out.writeRawData((char*)&z, 4);
        }
        out.writeRawData(detal, 2);
        if(isCancel)
        {
            break;
        }
        if (i%1000 == 0) {
            precent = i/(float)mdoutputlist.size()*100;
            mpd->setProgress(precent);
            QApplication::processEvents();
        }
    }
    pf->close();
    mpd->close();
    this->showToast(stlsaved,5);
    delete pf;
}

void MainWindow::showSliceFinish()
{
    qDebug() << "showSliceFinish";

//    md->setDType("drag");
    finishdialog = new MDialog(this);
    finishdialog->setMinimumSize(240, 128);
    QWidget *mwidget = new QWidget();
    QVBoxLayout *mlayout = new QVBoxLayout();
    QLabel *tt = new QLabel();
    tt->setText(toast_printtime.arg(hour).arg(minute));
    tt->setAlignment(Qt::AlignCenter);
    QHBoxLayout *botlayout = new QHBoxLayout();
    QPushButton *preview, *ok;
    preview = new QPushButton();
    preview->setText(toast_preview);
    preview->setMaximumSize(100, 20);
    preview->setMinimumSize(100, 20);
    preview->setStyleSheet("background-color:#126e58;font:14px;color:#ffffff;border:0px;");
    connect(preview, SIGNAL(clicked(bool)), this, SLOT(showPreView()));
    connect(preview, SIGNAL(pressed()), finishdialog, SLOT(btnPress()));
    connect(preview, SIGNAL(released()), finishdialog, SLOT(btnRelease()));
    ok = new QPushButton();
    ok->setText(toast_ok);
    ok->setMaximumSize(50, 20);
    ok->setMinimumSize(50, 20);
    ok->setStyleSheet("background-color:#126e58;font:14px;color:#ffffff;border:0px;");
    connect(ok, SIGNAL(clicked(bool)), this, SLOT(closedialog()));
    connect(ok, SIGNAL(pressed()), finishdialog, SLOT(btnPress()));
    connect(ok, SIGNAL(released()), finishdialog, SLOT(btnRelease()));
    botlayout->addWidget(preview);
    botlayout->addWidget(ok);
    mlayout->addWidget(tt, 1);
    mlayout->addLayout(botlayout);
    mwidget->setLayout(mlayout);
    finishdialog->setPanel(mwidget);
    finishdialog->show();
}

void MainWindow::showGuide()
{
    while(pic_arr.size() > 0){
        pic_arr.erase(pic_arr.begin());
    }
    while(text_arr.size() > 0){
        text_arr.erase(text_arr.begin());
    }
    QString language;
    getData("mksdlp_language",language,":/resource/language/cn.qm");
    if (language == ":/resource/language/cn.qm" ) {
        pic_arr.push_back(":/resource/icon/1.png");
        pic_arr.push_back(":/resource/icon/2.png");
        pic_arr.push_back(":/resource/icon/3.png");
        pic_arr.push_back(":/resource/icon/4.png");
        pic_arr.push_back(":/resource/icon/5.png");
        pic_arr.push_back(":/resource/icon/6.png");
    }else {
        pic_arr.push_back(":/resource/icon/1.1.png");
        pic_arr.push_back(":/resource/icon/2.1.png");
        pic_arr.push_back(":/resource/icon/3.1.png");
        pic_arr.push_back(":/resource/icon/4.1.png");
        pic_arr.push_back(":/resource/icon/5.png");
        pic_arr.push_back(":/resource/icon/6.1.png");
    }
    text_arr.push_back(onestep);
    text_arr.push_back(twostep);
    text_arr.push_back(threestep);
    text_arr.push_back(fourstep);
    text_arr.push_back(fivestep);
    text_arr.push_back(sixstep);
    currentIndex = 0;
    guidedialog = new MDialog(this);
    guidedialog->setMinimumSize(700, 500);
    guidedialog->setTitle(Usage);
    QWidget *mwidget = new QWidget();
    QVBoxLayout *mlayout = new QVBoxLayout();
    tt = new QLabel();
    tt->setText( text_arr[currentIndex]);
    tt->setAlignment(Qt::AlignVCenter);
    tt->setWordWrap(true);
    tt->setStyleSheet("font:14px;");
    piclabel=new QLabel();
    piclabel->resize(300,300);
    QPixmap pix(pic_arr[currentIndex]);
    QPixmap dest=pix.scaled(piclabel->size(),Qt::KeepAspectRatio);
    piclabel->setPixmap(dest);
    piclabel->setStyleSheet("background-color:#126e58;");
    piclabel->setAlignment(Qt::AlignCenter);
    QHBoxLayout *botlayout = new QHBoxLayout();
    QPushButton *nextstep, *ok, *prestep;
    prestep = new QPushButton();
    prestep->setObjectName("0");
    prestep->setText(Previous);
    prestep->setMaximumSize(100, 20);
    prestep->setMinimumSize(100, 20);
    prestep->setStyleSheet("background-color:#126e58;font:14px;color:#ffffff;border:0px;");
    connect(prestep, SIGNAL(clicked(bool)), this, SLOT(shownextguide()));
    connect(prestep, SIGNAL(pressed()), guidedialog, SLOT(btnPress()));
    connect(prestep, SIGNAL(released()), guidedialog, SLOT(btnRelease()));
    nextstep = new QPushButton();
    nextstep->setObjectName("1");
    nextstep->setText(Next);
    nextstep->setMaximumSize(100, 20);
    nextstep->setMinimumSize(100, 20);
    nextstep->setStyleSheet("background-color:#126e58;font:14px;color:#ffffff;border:0px;");
    connect(nextstep, SIGNAL(clicked(bool)), this, SLOT(shownextguide()));
    connect(nextstep, SIGNAL(pressed()), guidedialog, SLOT(btnPress()));
    connect(nextstep, SIGNAL(released()), guidedialog, SLOT(btnRelease()));
    ok = new QPushButton();
    ok->setText(Notremind);
    ok->setMaximumSize(100, 20);
    ok->setMinimumSize(100, 20);
    ok->setStyleSheet("background-color:#126e58;font:14px;color:#ffffff;border:0px;");
    connect(ok, SIGNAL(clicked(bool)), this, SLOT(closeguide()));
    connect(ok, SIGNAL(pressed()), guidedialog, SLOT(btnPress()));
    connect(ok, SIGNAL(released()), guidedialog, SLOT(btnRelease()));
    botlayout->addWidget(prestep);
    botlayout->addWidget(nextstep);
    botlayout->addWidget(ok);
    mlayout->addWidget(tt, 1);
     mlayout->addWidget(piclabel, 5);
    mlayout->addLayout(botlayout);
    mwidget->setLayout(mlayout);
    guidedialog->setPanel(mwidget);
    guidedialog->show();
}
void MainWindow::shownextguide() {
    QPushButton *pb = (QPushButton *)sender();
    if (pb->objectName() == "1") {
        if (currentIndex < text_arr.size() - 1) {
            currentIndex += 1;
        }
    }else {
        if (currentIndex > 0) {
            currentIndex -= 1;
        }
    }
    tt->setText( text_arr[currentIndex]);
    piclabel->resize(300,300);
    QPixmap pix(pic_arr[currentIndex]);
    QPixmap dest=pix.scaled(piclabel->size(),Qt::KeepAspectRatio);
    piclabel->setPixmap(dest);
    piclabel->setAlignment(Qt::AlignCenter);
}
void MainWindow::closeguide() {
    setData("mksdlp_showguide","1");
    guidedialog->close();
}
void MainWindow::closedialog()
{
    if (isChange) {
        isCancel = true;
        finishdialog->close();
        return;
    }
    QSettings settings("makerbase", "mksdlp");
    QString defaultname = settings.value("savepath").toString();
    if(md_list.size()>0)
    {
        smd = md_list[0];
        QFileInfo minfo(smd->filename);
        defaultname  = defaultname+minfo.baseName();
    }
    QString filename = QFileDialog::getSaveFileName(this, tr("Export Slices"),
                                                defaultname,
                                                tr("mdlp (*.mdlp)"));
    if(filename.isEmpty())
    {
        return;
    }
    QFileInfo info(filename);
    settings.setValue("savepath", info.path());
//    double precent;
////    MDialog *mpd = new MDialog(this);
////    mpd->setDType("Progress");
////    mpd->setLabelText(toast_mpdtext);
////    mpd->setProgress(0);
////    mpd->show();
////    connect(mpd, SIGNAL(OnCancel()), this, SLOT(CancelSlicing()));
////    QApplication::processEvents();
////    do{
////        mpd->setProgress(precent);
////        QApplication::processEvents();
////    }while(precent!=1);
//    sliceresult->setFilename(filename,precent);
////    if (precent == 1) {
////        mpd->close();
//        finishdialog->close();
////    }
        //20200225保存进度
        saveprocessdialog->show();
        int pro = 0;
        bool finish = false;
        sliceresult->setFilename(filename,pro,finish);
//        do {
//            qDebug() << "pro==" << pro;
//            saveprocessdialog->setProgress(pro);
//            QApplication::processEvents();
//        }while (!finish);
//        saveprocessdialog->close();
        finishdialog->close();
}
void MainWindow::saveprocess(double value,bool finish) {
    if (value == 100 || finish) {
        saveprocessdialog->close();
        mview->showToast(filesaved,5);
    }else {
        saveprocessdialog->setProgress(value);
        QApplication::processEvents();
    }
}
void MainWindow::filedialog()
{
//    ui->label->setText("filedialog");
    QSettings settings("makerbase", "mksdlp");
    QString defaultname = settings.value("savepath").toString();
    if(md_list.size()>0)
    {
        smd = md_list[0];
        QFileInfo minfo(smd->filename);
        defaultname  = defaultname+minfo.baseName();
    }
    QString filename = QFileDialog::getSaveFileName(this, tr("Export Slices"),
                                                defaultname,
                                                tr("mdlp (*.mdlp)"));
    if(filename.isEmpty())
    {
        return;
    }
    QFileInfo info(filename);
    settings.setValue("savepath", info.path());
    int precent;
    bool finish = false;
    sliceresult->setFilename(filename,precent,finish);
    finishdialog->close();
}

void MainWindow::UpdateLanguage()
{
    QString bd = "";
    QTranslator trans;
    getData("mksdlp_language", bd, ":/resource/language/cn.qm");
//    trans.load(":/resource/language/cn.qm");
//    QCoreApplication::removeTranslator(&trans);
    bool loadsucc = trans.load(bd);
//    qDebug()<<bd<<loadsucc;
//    QApplication::instance()->installTranslator(&trans);
    QCoreApplication::installTranslator(&trans);
    ui->retranslateUi(this);
    this->setWindowTitle("MKS DHOST");
    toast_selectmodel = tr("At lease import one model");
    toast_fileerro = tr("Please choose .stl or .mdlp file");
    toast_mpdtext = tr("Saving to stl file:");
    toast_printtime = tr("estimated time:%1hour%2min");
    toast_preview = tr("Preview");
    toast_ok = tr("Save");
    dialog_slicing = tr("Slicing");
    dialog_cancel = tr("Cancel");
    dialog_preview = tr("Preview");
    add_supporting = tr("Auto Add Support");
    dialog_dragtext = tr("thickness:%1mm expose time:%2s off:%3s bottom expose:%4s bottom layer:%5");
    mview->UpdateLanguage();
    nrightPanel->UpdateLanguage();
    onestep = tr("one");
    twostep = tr("two");
    threestep = tr("three");
    fourstep = tr("four");
    fivestep = tr("five");
    sixstep = tr("six");
    Usage = tr("Usage guidelines");
    Previous = tr("Previous");
    Next = tr("Next step");
    Notremind = tr("Not remind");
    fileerror = tr("File error");
    filesave = tr("MDLP File is saving");
    filesaved = tr("MDLP File has been saved.");
    pleasesetfilename = tr("Please set file name.");
    stlsaved = tr("stl file has been saved.");
    while(pic_arr.size() > 0){
        pic_arr.erase(pic_arr.begin());
    }
    while(text_arr.size() > 0){
        text_arr.erase(text_arr.begin());
    }
    if (bd == ":/resource/language/cn.qm" ) {
        pic_arr.push_back(":/resource/icon/1.png");
        pic_arr.push_back(":/resource/icon/2.png");
        pic_arr.push_back(":/resource/icon/3.png");
        pic_arr.push_back(":/resource/icon/4.png");
        pic_arr.push_back(":/resource/icon/5.png");
        pic_arr.push_back(":/resource/icon/6.png");
    }else {
        pic_arr.push_back(":/resource/icon/1.1.png");
        pic_arr.push_back(":/resource/icon/2.1.png");
        pic_arr.push_back(":/resource/icon/3.1.png");
        pic_arr.push_back(":/resource/icon/4.1.png");
        pic_arr.push_back(":/resource/icon/5.png");
        pic_arr.push_back(":/resource/icon/6.1.png");
    }
    text_arr.push_back(onestep);
    text_arr.push_back(twostep);
    text_arr.push_back(threestep);
    text_arr.push_back(fourstep);
    text_arr.push_back(fivestep);
    text_arr.push_back(sixstep);
}

void MainWindow::addmodels()
{
//    ModelData *md = loader->getMD();
    md_list.push_back(smd);
    smd->FromModel();
//    qDebug() << "from finish";
//    std::vector<triangle> trilist = md->getTrilist();
//    ModelData *nmd = new ModelData(trilist, md->filename);
//    md_list.push_back(nmd);
    QApplication::processEvents();
//    qDebug() << "from finish ---ok";
    adding = false;
//    delete loader;
}
void MainWindow::overloadmodels()
{
    md_list.push_back(smd);
    smd->FromModel();
//    qDebug() << "overloadmodels finish";
//    ui->label->setText("overloadmodels finish");
    adding = false;
//    savedlp();
    savedlpfile();
}

void MainWindow::setadding(bool isadd){
    adding = isadd;
}
