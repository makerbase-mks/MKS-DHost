#include "nrightpanel.h"

NRightPanel::NRightPanel(MainWindow *mparent, QWidget *parent) : QWidget(parent)
{
    this->mparent = mparent;
    this->setMaximumWidth(230);
    this->setMinimumWidth(230);
    intvalidator = new QIntValidator();
    floatvalidator = new QDoubleValidator();
    str_addsup = tr("Add Support");
    str_delsup = tr("Del Support");
    str_autosup = tr("Auto Support");
    str_finish = tr("Finish");
    str_toast = tr("please select a model");
    str_add = tr("Add Base");
    str_remove = tr("Del Base");
    str_editsup = tr("Edit Support");
    str_otherset = tr("Other Setting");
    str_sup_top = tr("Support Top");
    str_sup_mid = tr("Support Mid");
    str_sup_bot = tr("Support Bottom");
    str_toptype = tr("Top Type:");
    str_cone = tr("Cone");
    str_cube = tr("Cube");
    str_touchtype = tr("Touch Type:");
    str_sphere = tr("Sphere");
    str_none = tr("None");
    str_touchtypesize = tr("Touch Type Size:");
    str_toplen = tr("Top Length:");
     str_topwidth = tr("Top Width:");
    str_midtype = tr("Mid Type:");
    str_midwidth = tr("Mid Width:");
    str_bottype = tr("Bot Type:");
    str_botdepth = tr("Bot Depth:");
    str_botwidth = tr("Bot Width:");
    str_botconesize = tr("Bot Cone Size:");
    str_botballsize = tr("Bot Ball Size:");
    str_delallsupport = tr("Del All Support");
    str_dialogcomfirm = tr("Delete all support?");
    str_cancel = tr("Cancel");
    str_comfirm = tr("Confirm");
    str_errorzero = tr("can not less than zero.");

    initUI();
    machinewidgetheight = -1;
    slicewidgetheight = -1;
    supportwidgetheight = -1;
    anigroup = new QParallelAnimationGroup;
    machineanigroup = new QParallelAnimationGroup;
    machineanigroup->setObjectName("machineanigroup");
    sliceanigroup = new QParallelAnimationGroup;
     sliceanigroup->setObjectName("sliceanigroup");
    supportanigroup = new QParallelAnimationGroup;
     supportanigroup->setObjectName("supportanigroup");
    isanifinish = true;
    machineanifinish = true;
    sliceanifinish = true;
    supportanifinish = true;
    initData();
    connect(anigroup, SIGNAL(finished()), this, SLOT(anifinished()));
//    connect(machineanigroup, SIGNAL(finished()), this, SLOT(anifinished()));
//    connect(sliceanigroup, SIGNAL(finished()), this, SLOT(anifinished()));
//    connect(supportanigroup, SIGNAL(finished()), this, SLOT(anifinished()));
    selectid = -1;
//    int posx, posy, width, height;
//    getPP(supportwidget, posx, posy, width, height);
//    if(supportwidgetheight == -1 && height != 0)
//    {
//        supportwidgetheight = height;
//    }
//    getPP(machinewidget, posx, posy, width, height);
//    if(machinewidgetheight == -1 && height != 0)
//    {
//        machinewidgetheight = height;
//    }
//    getPP(slicewidget, posx, posy, width, height);
//    if(slicewidgetheight == -1 && height != 0)
//    {
//        slicewidgetheight = height;
//    }
//    clickmachine = false;
//    clickslice = false;
//    clicksupport = false;
    machinelabel->setIsOff(true);
    machinewidget->hide();
    slicelabel->setIsOff(true);
    slicewidget->hide();
    supportlabel->setIsOff(false);
    supportwidget->show();
}

NRightPanel::~NRightPanel()
{
}

void NRightPanel::OnModelSelected(int id)
{
    this->selectid = id;
    if(id >= 0 && id < mparent->getModelInstance().size())
    {
        smd = mparent->getModelInstance()[id];
        if(smd->hasBase)
        {
            addbase->setText(str_remove);
        }else{
            addbase->setText(str_add);
        }
    }else{
        addbase->setText(str_add);
    }
}

void NRightPanel::SyncSupportData()
{
    QString data;
    getData("mksdlp_supportshape", data, "0");
    if(supportshape->count() < data.toInt()+1)
    {
        supportshape->setCurrentIndex(0);
        dealData("mksdlp_supportshape", "0");
    }else{
        supportshape->setCurrentIndex(data.toInt());
    }
    getData("mksdlp_supporttype", data, "0");
    if(supporttype->count() < data.toInt()+1)
    {
        supporttype->setCurrentIndex(0);
        dealData("mksdlp_supporttype", "0");
    }else{
        supporttype->setCurrentIndex(data.toInt());
    }
//    qDebug()<< supportshape->currentIndex() << supporttype->currentIndex();
}

void NRightPanel::initUI()
{
//    scpanel = new QScrollArea(this);
//    scpanel->setMinimumSize(230, 768);
//    scpanel->setFrameShape(QFrame::NoFrame);
//    scpanel->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    scpanel->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//    scpanel->setWidgetResizable(true);
    mainwidget = new QWidget();
    mainwidget->setMinimumWidth(230);
    mainwidget->setMaximumWidth(230);
    mainlayout = new QVBoxLayout();
    mainlayout->setMargin(0);
    mainlayout->setSpacing(0);

//    machinelayout = new QHBoxLayout();
//    machinesetting = new QLabel();
//    machinesetting->setText(tr("机器参数设置"));
//    machinesetting->setStyleSheet("background-color:#009a79;padding-top:8px;padding-bottom:5px;color:#FFFFFF;font:11pt;font-weight:bold;font-family:Arial,Helvetica,sans-serif;");
//    machinesetting->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px;");
//    machinelayout->addWidget(machinesetting, 1);

//    b1 = new SpacingBar();
    b1 = new MsBar();
    b1->setMinimumHeight(5);
    machinelabel = new RightLabel();
    machinelabel->setText(tr("Machine Setting"));
    machinelabel->setMargin(0);
    machineglayout = new QGridLayout();
    machineglayout->setMargin(0);
    machineglayout->setSpacing(0);
    xresolutionlabel = new QLabel();
    xresolutionlabel->setText(tr("x resolution(pixel)"));
    xresolutionlabel->setStyleSheet("margin-left:10px;");
    yresolutionlabel = new QLabel();
    yresolutionlabel->setText(tr("y resolution(pixel)"));
    yresolutionlabel->setStyleSheet("margin-left:10px;");
    xlenlabel = new QLabel();
    xlenlabel->setText(tr("x size(mm)"));
    xlenlabel->setStyleSheet("margin-left:10px;");
    ylenlabel = new QLabel();
    ylenlabel->setText(tr("y size(mm)"));
    ylenlabel->setStyleSheet("margin-left:10px;");
    zlenlabel = new QLabel();
    zlenlabel->setText(tr("z size(mm)"));
    zlenlabel->setStyleSheet("margin-left:10px;");
    xresolutionedit = new QLineEdit();
    xresolutionedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    xresolutionedit->setAlignment(Qt::AlignRight);
    yresolutionedit = new QLineEdit();
    yresolutionedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    yresolutionedit->setAlignment(Qt::AlignRight);
    xlenedit = new QLineEdit();
    xlenedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    xlenedit->setAlignment(Qt::AlignRight);
    ylenedit = new QLineEdit();
    ylenedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    ylenedit->setAlignment(Qt::AlignRight);
    zlenedit = new QLineEdit();
    zlenedit->setStyleSheet("QWidget{background-color:#ffffff;padding:0;color:#000000;margin:5px;"
                            "margin-right:10px;}");
    zlenedit->setAlignment(Qt::AlignRight);
    keepxyresolution = new QCheckBox();
    keepxyresolution->setText(tr("lock ratio"));
    keepxyresolution->setStyleSheet("margin:5px;margin-bottom:10px;");
    keepxyresolution->setChecked(true);
    machinewidget = new QWidget();
    machinewidget->setStyleSheet("background-color:#12997a;color:#f2f2f2;"
                                 "font:14px;");
    machineglayout->addWidget(b1, 0, 0, 1, 2);
    machineglayout->addWidget(xresolutionlabel, 1, 0);
    machineglayout->addWidget(xresolutionedit, 1, 1);
    machineglayout->addWidget(yresolutionlabel, 2, 0);
    machineglayout->addWidget(yresolutionedit, 2, 1);
    machineglayout->addWidget(xlenlabel, 3, 0);
    machineglayout->addWidget(xlenedit, 3, 1);
    machineglayout->addWidget(ylenlabel, 4, 0);
    machineglayout->addWidget(ylenedit, 4, 1);
    machineglayout->addWidget(zlenlabel, 5, 0);
    machineglayout->addWidget(zlenedit, 5, 1);
    machineglayout->addWidget(keepxyresolution, 6, 0, 1, 2);
    machineglayout->setAlignment(keepxyresolution, Qt::AlignCenter);
    machinewidget->setLayout(machineglayout);
    connect(machinelabel, SIGNAL(OnClicked(bool)), this, SLOT(OnMachineChange(bool)));
//    connect(machinelabel, SIGNAL(machinelabelClicked(bool)), this, SLOT(OnMachineChange(bool)));
    b2 = new MsBar();
    b2->setMinimumHeight(5);
    slicelabel = new RightLabel();
    slicelabel->setText(tr("Slice Setting"));
    slicelabel->setMargin(5);
    sliceglayout = new QGridLayout();
    sliceglayout->setMargin(0);
    sliceglayout->setSpacing(0);
    thicknesslabel = new QLabel();
    thicknesslabel->setText(tr("thickness(mm)"));
    thicknesslabel->setStyleSheet("margin-left:10px;");
    exposelabel = new QLabel();
    exposelabel->setText(tr("expose time(s)"));
    exposelabel->setStyleSheet("margin-left:10px;");
    offlabel = new QLabel();
    offlabel->setText(tr("light off time(s)"));
    offlabel->setStyleSheet("margin-left:10px;");
    botexposelabel = new QLabel();
    botexposelabel->setText(tr("bottom expose(s)"));
    botexposelabel->setStyleSheet("margin-left:10px;");
    botcountlabel = new QLabel();
    botcountlabel->setText(tr("bottom layer"));
    botcountlabel->setStyleSheet("margin-left:10px;");
    thicknessedit = new QLineEdit();
    thicknessedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    thicknessedit->setAlignment(Qt::AlignRight);
    exposeedit = new QLineEdit();
    exposeedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    exposeedit->setAlignment(Qt::AlignRight);
    offedit = new QLineEdit();
    offedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    offedit->setAlignment(Qt::AlignRight);
    botexposeedit = new QLineEdit();
    botexposeedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    botexposeedit->setAlignment(Qt::AlignRight);
    botcountedit = new QLineEdit();
    botcountedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    botcountedit->setAlignment(Qt::AlignRight);
    slicetypelabel = new QLabel();
    slicetypelabel->setText(tr("light curing type"));
    slicetypelabel->setStyleSheet("margin-left:10px;");
    slicetype = new QComboBox();
    slicetype->setStyleSheet("QComboBox::drop-down {border:0px;}"
                             "QComboBox{background-color:#ffffff;padding:0px;margin-left:2px;margin-top:10px;margin-bottom:10px;margin-right:5px;}"
                             "QWidget{background-color:#ffffff;padding:0px;selection-background-color:#126e59;color:#000000;"
                             "border:0px;width:88px;height:20px;font:12px;}"
                             "QComboBox::down-arrow {image: url(:/resource/icon/drop_down.png);}");
    slicetype->addItem(tr("X forward"));
    slicetype->addItem(tr("X direction"));
    //灰度版本添加的参数20200113底层抬升距离、底层抬升速度、抬升距离、抬升速度、回程速度、底层光强PWM、光强PWM
    botliftdislabel = new QLabel();
    botliftdislabel->setText(tr("bottom lift dis"));
    botliftdislabel->setStyleSheet("margin-left:10px;");
    botliftdisedit = new QLineEdit();
    botliftdisedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    botliftdisedit->setAlignment(Qt::AlignRight);
    botliftspeedlabel = new QLabel();
    botliftspeedlabel->setText(tr("bottom lift speed"));
    botliftspeedlabel->setStyleSheet("margin-left:10px;");
    botliftspeededit = new QLineEdit();
    botliftspeededit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    botliftspeededit->setAlignment(Qt::AlignRight);
    liftdislabel = new QLabel();
    liftdislabel->setText(tr("lift dis"));
    liftdislabel->setStyleSheet("margin-left:10px;");
    liftdisedit = new QLineEdit();
    liftdisedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    liftdisedit->setAlignment(Qt::AlignRight);
    liftspeedlabel = new QLabel();
    liftspeedlabel->setText(tr("lift speed"));
    liftspeedlabel->setStyleSheet("margin-left:10px;");
    liftspeededit = new QLineEdit();
    liftspeededit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    liftspeededit->setAlignment(Qt::AlignRight);
    returnspeedlabel = new QLabel();
    returnspeedlabel->setText(tr("return speed"));
    returnspeedlabel->setStyleSheet("margin-left:10px;");
    returnspeededit = new QLineEdit();
    returnspeededit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    returnspeededit->setAlignment(Qt::AlignRight);
    botbrightnessPVMlabel = new QLabel();
    botbrightnessPVMlabel->setText(tr("bottom light PVM"));
    botbrightnessPVMlabel->setStyleSheet("margin-left:10px;");
    botbrightnessPVMedit = new QLineEdit();
    botbrightnessPVMedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    botbrightnessPVMedit->setAlignment(Qt::AlignRight);
    brightnessPVMlabel = new QLabel();
    brightnessPVMlabel->setText(tr("light PVM"));
    brightnessPVMlabel->setStyleSheet("margin-left:10px;");
    brightnessPVMedit = new QLineEdit();
    brightnessPVMedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    brightnessPVMedit->setAlignment(Qt::AlignRight);

    sliceglayout->addWidget(b2, 0, 0, 1, 2);
    sliceglayout->addWidget(thicknesslabel, 1, 0);
    sliceglayout->addWidget(thicknessedit, 1, 1);
    sliceglayout->addWidget(exposelabel, 2, 0);
    sliceglayout->addWidget(exposeedit, 2, 1);
    sliceglayout->addWidget(offlabel, 3, 0);
    sliceglayout->addWidget(offedit, 3, 1);
    sliceglayout->addWidget(botexposelabel, 4, 0);
    sliceglayout->addWidget(botexposeedit, 4, 1);
    sliceglayout->addWidget(botcountlabel, 5, 0);
    sliceglayout->addWidget(botcountedit, 5, 1);
    sliceglayout->addWidget(botliftdislabel, 6, 0);
    sliceglayout->addWidget(botliftdisedit, 6, 1);
    sliceglayout->addWidget(botliftspeedlabel, 7, 0);
    sliceglayout->addWidget(botliftspeededit, 7, 1);
    sliceglayout->addWidget(liftdislabel, 8, 0);
    sliceglayout->addWidget(liftdisedit, 8, 1);
    sliceglayout->addWidget(liftspeedlabel, 9, 0);
    sliceglayout->addWidget(liftspeededit, 9, 1);
    sliceglayout->addWidget(returnspeedlabel, 10, 0);
    sliceglayout->addWidget(returnspeededit, 10, 1);
    sliceglayout->addWidget(botbrightnessPVMlabel, 11, 0);
    sliceglayout->addWidget(botbrightnessPVMedit, 11, 1);
    sliceglayout->addWidget(brightnessPVMlabel, 12, 0);
    sliceglayout->addWidget(brightnessPVMedit, 12, 1);
    sliceglayout->addWidget(slicetypelabel, 13, 0);
    sliceglayout->addWidget(slicetype, 13, 1);
    if (!mparent->newSliceType) {
        botliftdislabel->hide();
        botliftdisedit->hide();
        botliftspeedlabel->hide();
        botliftspeededit->hide();
        liftdislabel->hide();
        liftdisedit->hide();
        liftspeedlabel->hide();
        liftspeededit->hide();
        returnspeedlabel->hide();
        returnspeededit->hide();
        botbrightnessPVMlabel->hide();
        botbrightnessPVMedit->hide();
        brightnessPVMlabel->hide();
        brightnessPVMedit->hide();
    }
    slicewidget = new QWidget();
    slicewidget->setStyleSheet("background-color:#12997a;color:#f2f2f2;"
                               "font:14px;");
    slicewidget->setLayout(sliceglayout);
    connect(slicelabel, SIGNAL(OnClicked(bool)), this, SLOT(OnSliceChange(bool)));
//    connect(slicelabel, SIGNAL(slicelabelClicked(bool)), this, SLOT(OnSliceChange(bool)));

    b3 = new MsBar();
    b3->setMinimumHeight(5);
    supportlabel = new RightLabel();
    supportlabel->setText(tr("Support Setting"));
    supportlabel->setMargin(5);
    supporttypelayout = new QHBoxLayout();
    supportshapelayout = new QHBoxLayout();
    supportuplayout = new QHBoxLayout();
    basesizelayout = new QHBoxLayout();
    supportratelayout = new QHBoxLayout();
    supportsizelayout = new QHBoxLayout();
    slicetypelayout = new QHBoxLayout();
    supportglayout = new QGridLayout();
    supportglayout->setMargin(0);
    supportglayout->setSpacing(0);
    supporttypelabel = new QLabel();
    supporttypelabel->setText(tr("Support type:"));
    supporttypelabel->setStyleSheet("padding-left:6px;margin-top:10px;");
    supportshapelabel = new QLabel();
    supportshapelabel->setText(tr("Support shape:"));
    supportshapelabel->setStyleSheet("padding-left:6px;margin-top:10px;");
    supporttype = new QComboBox();
    supporttype->setStyleSheet("QComboBox::drop-down {border:0px;}"
                               "QComboBox{background-color:#ffffff;padding:0px;margin-left:2px;margin-top:10px;}"
                               "QWidget{background-color:#ffffff;padding:0px;selection-background-color:#126e59;color:#000000;"
                               "border:0px;width:94px;height:20px;font:14px;}"
                               "QComboBox::down-arrow {image: url(:/resource/icon/drop_down.png);}");
//    supporttype->addItem(tr("point"));
    supporttype->addItem(tr("OutSide"));
    supporttype->addItem(tr("EveryWhere"));
    supportshape = new QComboBox();
    supportshape->setStyleSheet("QComboBox::drop-down {border:0px;}"
                                "QComboBox{background-color:#ffffff;padding:0px;margin-left:2px;margin-top:10px;}"
                                "QWidget{background-color:#ffffff;padding:0px;selection-background-color:#126e59;color:#000000;"
                                "border:0px;width:94px;height:20px;font:14px;}"
                                "QComboBox::down-arrow {image: url(:/resource/icon/drop_down.png);}");
//    supportshape->addItem(tr("cube"));
    supportshape->addItem(tr("cylinder"));
    connect(supporttype, SIGNAL(currentIndexChanged(const int)), this, SLOT(SupportTypeChange(int)));
    connect(supportshape, SIGNAL(currentIndexChanged(int)), this, SLOT(SupportShapeChange(int)));

    supportuplabel = new QLabel();
    supportuplabel->setText(tr("Uplift:"));
    supportuplabel->setStyleSheet("padding-left:6px;margin-top:10px;");
    supportupedit = new QLineEdit();
    supportupedit->setAlignment(Qt::AlignRight);
    supportupedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000; margin-top:10px;margin-left:13px;"
                                 "margin-right:39px;");

    basesizelabel = new QLabel();
    basesizelabel->setText(tr("Base size:"));
    basesizelabel->setStyleSheet("padding-left:6px;margin-top:10px;");
    basesizeedit = new QLineEdit();
    basesizeedit->setAlignment(Qt::AlignRight);
    basesizeedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000; margin-top:10px;margin-left:13px;"
                                 "margin-right:39px;");

    supportratelabel = new QLabel();
    supportratelabel->setText(tr("Support density:"));
    supportratelabel->setStyleSheet("padding-left:6px;margin-top:10px;");
    supportrateM = new QPushButton();
    supportrateM->setText(tr("M"));
    supportrateM->setObjectName("50");
    supportrateM->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:2px;border:0px;margin-top:10px;"
                              "margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    supportrateL = new QPushButton();
    supportrateL->setText(tr("L"));
    supportrateL->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:2px;border:0px;margin-top:10px;"
                                "margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    supportrateL->setObjectName("40");
    supportrateH = new QPushButton();
    supportrateH->setText(tr("H"));
    supportrateH->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:2px;border:0px;margin-top:10px;"
                                "margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    supportrateH->setObjectName("80");
    supportsizelabel = new QLabel();
    supportsizelabel->setText(tr("Support Size:"));
    supportsizelabel->setStyleSheet("padding-left:6px;margin-top:10px;");
    supportsizeS = new QPushButton();
    supportsizeS->setText(tr("sizeS"));
    supportsizeS->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:2px;border:0px;margin-top:10px;"
                                "margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    supportsizeS->setObjectName("S");
    supportsizeM = new QPushButton();
    supportsizeM->setText(tr("sizeM"));
    supportsizeM->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:2px;border:0px;margin-top:10px;"
                                "margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    supportsizeM->setObjectName("M");
    supportsizeL = new QPushButton();
    supportsizeL->setText(tr("sizeL"));
    supportsizeL->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:2px;border:0px;margin-top:10px;"
                                "margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    supportsizeL->setObjectName("L");
    addSupport = new QPushButton();
    addSupport->setText(tr("Add Support"));
    addSupport->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:10px;border:0px;margin-top:10px;"
                              "font:12px;width:170px;height:22px;margin-left:1px;margin-right:1px;");
    addSupport->setObjectName("add");
    delSupport = new QPushButton();
    delSupport->setText(tr("Del Support"));
    delSupport->setObjectName("del");
    delSupport->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:10px;border:0px;margin-top:10px;"
                              "font:12px;width:170px;height:22px;margin-left:1px;margin-right:1px;");
    autoSupport = new QPushButton();
    autoSupport->setText(tr("Auto Support"));
    autoSupport->setObjectName("auto");
    autoSupport->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:10px;border:0px;margin-top:10px;"
                               "font:12px;width:170px;height:22px;margin-left:1px;margin-right:1px;");
    addbase = new QPushButton();
    addbase->setText(tr("Add Base"));
    addbase->setObjectName("auto");
    addbase->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:10px;border:0px;margin-top:10px;"
                               "font:12px;width:170px;height:22px;margin-left:1px;margin-right:1px;");
    editSupport = new QPushButton();
    editSupport->setText(str_editsup);
    editSupport->setObjectName("edit");
    editSupport->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:10px;border:0px;margin-top:10px;"
                               "font:12px;width:170px;height:22px;margin-left:1px;margin-right:1px;");
    otherSettings = new QPushButton();
    otherSettings->setObjectName("other");
    otherSettings->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:10px;border:0px;margin-top:10px;"
                               "font:12px;width:170px;height:22px;margin-left:1px;margin-right:1px;");
    otherSettings->setText(str_otherset);
//    otherSettings->setVisible(false);

    delAllSupport = new QPushButton();
    delAllSupport->setObjectName("delall");
    delAllSupport->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:10px;border:0px;margin-top:10px;"
                                 "font:12px;width:170px;height:22px;margin-left:1px;margin-right:1px;");
    delAllSupport->setText(str_delallsupport);

    QWidget *dlgpanel = new QWidget();
    QString data = "";
    bool b = false;
    QGridLayout *dlglayout = new QGridLayout();
    dlglayout->setSpacing(5);
    dlglayout->setMargin(5);
    toptext = new QLabel();
    toptext->setStyleSheet("background-color:#ffffff;color:#0f8764;padding-left:5px;margin-top:10px;");
    toptext->setText(str_sup_top);
    midtext = new QLabel();
    midtext->setStyleSheet("background-color:#ffffff;color:#0f8764;padding-left:5px;margin-top:10px;");
    midtext->setText(str_sup_mid);
    bottext = new QLabel();
    bottext->setStyleSheet("background-color:#ffffff;color:#0f8764;padding-left:5px;margin-top:10px;");
    bottext->setText(str_sup_bot);
    toptype = new QLabel();
    toptype->setText(str_toptype);
    toptypecb = new QComboBox();
    toptypecb->setStyleSheet("QComboBox::drop-down {border:0px;}"
                                "QComboBox{background-color:#ffffff;padding:0px;margin-left:2px;}"
                                "QWidget{background-color:#ffffff;padding:0px;selection-background-color:#126e59;color:#000000;"
                                "border:0px;width:94px;height:20px;font:14px;}"
                                "QComboBox::down-arrow {image: url(:/resource/icon/drop_down.png);}");
    toptypecb->setObjectName("mksdlp_toptype");
    toptypecb->addItem(str_cone);
    toptypecb->addItem(str_cube);
    getData(toptypecb->objectName(), data, "0");
    toptypecb->setCurrentIndex(data.toInt());
    connect(toptypecb, SIGNAL(currentIndexChanged(int)),this,SLOT(saveCombobox()));
    touchtype = new QLabel();
    touchtype->setText(str_touchtype);
    touchtypecb = new QComboBox();
    touchtypecb->setStyleSheet("QComboBox::drop-down {border:0px;}"
                                "QComboBox{background-color:#ffffff;padding:0px;margin-left:2px;}"
                                "QWidget{background-color:#ffffff;padding:0px;selection-background-color:#126e59;color:#000000;"
                                "border:0px;width:94px;height:20px;font:14px;}"
                                "QComboBox::down-arrow {image: url(:/resource/icon/drop_down.png);}");
    touchtypecb->setObjectName("mksdlp_touchtype");
    touchtypecb->addItem(str_sphere);
    touchtypecb->addItem(str_none);
    getData(touchtypecb->objectName(), data, "0");
    touchtypecb->setCurrentIndex(data.toInt());
    connect(touchtypecb, SIGNAL(currentIndexChanged(int)),this,SLOT(saveCombobox()));
    touchtypesize = new QLabel();
    touchtypesize->setText(str_touchtypesize);
    QLineEdit *touchtypesizeedit = new QLineEdit();
    touchtypesizeedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin-left:2px;");
//    touchtypesizeedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    touchtypesizeedit->setObjectName("mksdlp_touchtypesize");
    touchtypesizeedit->setValidator(this->floatvalidator);
    getData(touchtypesizeedit->objectName(), data, "3");
    touchtypesizeedit->setText(data);
    connect(touchtypesizeedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    toplength = new QLabel();
    toplength->setText(str_toplen);
    QLineEdit *toplengthedit = new QLineEdit();
    toplengthedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin-left:2px;");
//    toplengthedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
//    toplengthedit->setAlignment(Qt::AlignRight);
    toplengthedit->setObjectName("mksdlp_toplength");
    toplengthedit->setValidator(this->floatvalidator);
    getData(toplengthedit->objectName(), data, "3");
    toplengthedit->setText(data);
    connect(toplengthedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    topwidth = new QLabel();
    topwidth->setText(str_topwidth);
    QLineEdit *topwidthedit = new QLineEdit();
    topwidthedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin-left:2px;");
//    topwidthedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
//    toplengthedit->setAlignment(Qt::AlignRight);
    topwidthedit->setObjectName("mksdlp_topwidth");
    topwidthedit->setValidator(this->floatvalidator);
    getData(topwidthedit->objectName(), data, "3");
    topwidthedit->setText(data);
    connect(topwidthedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    midtype = new QLabel();
    midtype->setText(str_midtype);
    midtypecb = new QComboBox();
    midtypecb->setStyleSheet("QComboBox::drop-down {border:0px;}"
                                "QComboBox{background-color:#ffffff;padding:0px;margin-left:2px;}"
                                "QWidget{background-color:#ffffff;padding:0px;selection-background-color:#126e59;color:#000000;"
                                "border:0px;width:94px;height:20px;font:14px;}"
                                "QComboBox::down-arrow {image: url(:/resource/icon/drop_down.png);}");
    touchtypecb->setObjectName("mksdlp_touchtype");
    midtypecb->setObjectName("mksdlp_midtype");
    midtypecb->addItem(str_cone);
    midtypecb->addItem(str_cube);
    getData(midtypecb->objectName(), data, "0");
    midtypecb->setCurrentIndex(data.toInt());
    connect(midtypecb, SIGNAL(currentIndexChanged(int)),this,SLOT(saveCombobox()));
    midlength = new QLabel();
    midlength->setText(str_midwidth);
    QLineEdit *midlengthedit = new QLineEdit();
    midlengthedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin-left:2px;");
    midlengthedit->setObjectName("mksdlp_supportmidsize");
    getData(midlengthedit->objectName(), data, "0.5");
    midlengthedit->setValidator(this->floatvalidator);
    midlengthedit->setText(data);
    connect(midlengthedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    bottomtype = new QLabel();
    bottomtype->setText(str_bottype);
    bottomtypecb = new QComboBox();
    bottomtypecb->setStyleSheet("QComboBox::drop-down {border:0px;}"
                                "QComboBox{background-color:#ffffff;padding:0px;margin-left:2px;}"
                                "QWidget{background-color:#ffffff;padding:0px;selection-background-color:#126e59;color:#000000;"
                                "border:0px;width:94px;height:20px;font:14px;}"
                                "QComboBox::down-arrow {image: url(:/resource/icon/drop_down.png);}");
    bottomtypecb->setObjectName("mksdlp_bottype");
    bottomtypecb->addItem(str_cone);
    bottomtypecb->addItem(str_cube);
    getData(bottomtypecb->objectName(), data, "0");
    bottomtypecb->setCurrentIndex(data.toInt());
    connect(bottomtypecb, SIGNAL(currentIndexChanged(int)),this,SLOT(saveCombobox()));
    botdepth = new QLabel();
    botdepth->setText(str_botdepth);
    QLineEdit *botdepthedit = new QLineEdit();
    botdepthedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin-left:2px;");
    botdepthedit->setObjectName("mksdlp_botdepth");
    botdepthedit->setValidator(this->floatvalidator);
    getData(botdepthedit->objectName(), data, "1");
    botdepthedit->setText(data);
    connect(botdepthedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    botwidth = new QLabel();
    botwidth->setText(str_botwidth);
    QLineEdit *botwidthedit = new QLineEdit();
    botwidthedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin-left:2px;");
    botwidthedit->setObjectName("mksdlp_botwidth");
    botwidthedit->setValidator(this->floatvalidator);
    getData(botwidthedit->objectName(), data, "1");
    botwidthedit->setText(data);
    connect(botwidthedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    botconesize = new QLabel();
    botconesize->setText(str_botconesize);
    QLineEdit *botconesizeedit = new QLineEdit();
    botconesizeedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin-left:2px;");
    botconesizeedit->setObjectName("mksdlp_botconesize");
    botconesizeedit->setValidator(this->floatvalidator);
    getData(botconesizeedit->objectName(), data, "1");
    botconesizeedit->setText(data);
    connect(botconesizeedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    botballsize = new QLabel();
    botballsize->setText(str_botballsize);
    QLineEdit *botballsizeedit = new QLineEdit();
    botballsizeedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin-left:2px;");
    botballsizeedit->setObjectName("mksdlp_botballsize");
    botballsizeedit->setValidator(this->floatvalidator);
    getData(botballsizeedit->objectName(), data, "1");
    botballsizeedit->setText(data);
    connect(botballsizeedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    dlglayout->addWidget(toptext, 0, 0, 1, 2);
    dlglayout->addWidget(toptype, 1, 0, 1, 1);
    dlglayout->addWidget(toptypecb, 1, 1, 1, 1);
    dlglayout->addWidget(touchtype, 2, 0, 1, 1);
    dlglayout->addWidget(touchtypecb, 2, 1, 1, 1);
    dlglayout->addWidget(touchtypesize, 3, 0, 1, 1);
    dlglayout->addWidget(touchtypesizeedit, 3, 1, 1, 1);
    dlglayout->addWidget(toplength, 4, 0, 1, 1);
    dlglayout->addWidget(toplengthedit, 4, 1, 1, 1);
    dlglayout->addWidget(topwidth, 5, 0, 1, 1);
    dlglayout->addWidget(topwidthedit, 5, 1, 1, 1);
    dlglayout->addWidget(midtext, 6, 0, 1, 2);
    dlglayout->addWidget(midtype, 7, 0, 1, 1);
    dlglayout->addWidget(midtypecb, 7, 1, 1, 1);
    dlglayout->addWidget(midlength, 8, 0, 1, 1);
    dlglayout->addWidget(midlengthedit, 8, 1, 1, 1);
    dlglayout->addWidget(bottext, 9, 0, 1, 2);
    dlglayout->addWidget(bottomtype, 10, 0, 1, 1);
    dlglayout->addWidget(bottomtypecb, 10, 1, 1, 1);
    dlglayout->addWidget(botdepth, 11, 0, 1, 1);
    dlglayout->addWidget(botdepthedit, 11, 1, 1, 1);
    dlglayout->addWidget(botwidth, 12, 0, 1, 1);
    dlglayout->addWidget(botwidthedit, 12, 1, 1, 1);
    dlglayout->addWidget(botconesize, 13, 0, 1, 1);
    dlglayout->addWidget(botconesizeedit, 13, 1, 1, 1);
    dlglayout->addWidget(botballsize, 14, 0, 1, 1);
    dlglayout->addWidget(botballsizeedit, 14, 1, 1, 1);
//    dlgpanel->setLayout(dlglayout);

    supporttypelayout->addWidget(supporttypelabel);
    supporttypelayout->addWidget(supporttype);
    supporttypelayout->addStretch(1);
    supportshapelayout->addWidget(supportshapelabel);
    supportshapelayout->addWidget(supportshape);
    supportshapelayout->addStretch(1);
    supportshapelabel->hide();
    supportshape->hide();
    supportuplayout->addWidget(supportuplabel);
    supportuplayout->addWidget(supportupedit);
    supportuplayout->addStretch(1);
    basesizelayout->addWidget(basesizelabel);
    basesizelayout->addWidget(basesizeedit);
    basesizelayout->addStretch(1);
    supportratelayout->addWidget(supportratelabel);
    supportratelayout->addWidget(supportrateL);
    supportratelayout->addWidget(supportrateM);
    supportratelayout->addWidget(supportrateH);
    supportratelayout->addStretch(1);
    supportsizelayout->addWidget(supportsizelabel);
    supportsizelayout->addWidget(supportsizeS);
    supportsizelayout->addWidget(supportsizeM);
    supportsizelayout->addWidget(supportsizeL);
    supportsizelayout->addStretch(1);
    supportsizelabel->hide();
    supportsizeM->hide();
    supportsizeL->hide();
    supportsizeS->hide();
    supportglayout->addWidget(b3, 0, 0);
    supportglayout->addLayout(supporttypelayout, 1, 0, 1, 3);
    supportglayout->addLayout(supportshapelayout, 2, 0, 1, 3);
    supportglayout->addLayout(supportuplayout, 3, 0, 1, 3);
    supportglayout->addLayout(basesizelayout, 4, 0, 1, 3);
    supportglayout->addLayout(supportsizelayout, 5, 0, 1, 3);
    supportglayout->addLayout(supportratelayout, 6, 0, 1, 3);
    supportglayout->addWidget(addSupport, 7, 0);
    supportglayout->addWidget(delSupport, 7, 1);
    supportglayout->addWidget(autoSupport, 8, 0);
    supportglayout->addWidget(addbase, 8, 1);
    supportglayout->addWidget(editSupport, 9, 0);
//    supportglayout->addWidget(otherSettings, 9, 1);
    supportglayout->addWidget(delAllSupport, 9, 1);
    supportglayout->addLayout(dlglayout, 10, 0, 1, 3);
//    supportglayout->addWidget(dlgpanel, 11, 1);
    supportglayout->setAlignment(addSupport, Qt::AlignCenter);
    supportglayout->setAlignment(delSupport, Qt::AlignCenter);
    supportglayout->setAlignment(autoSupport, Qt::AlignCenter);
    supportglayout->setAlignment(editSupport, Qt::AlignCenter);
    supportglayout->setAlignment(addbase, Qt::AlignCenter);
    supportwidget = new QWidget();
    supportwidget->setStyleSheet("background-color:#009a79;color:#FFFFFF;"
                                 "font:14px;");
    supportwidget->setLayout(supportglayout);
    connect(supportlabel, SIGNAL(OnClicked(bool)), this, SLOT(OnSupportChange(bool)));
//    connect(supportlabel, SIGNAL(supportlabelClicked(bool)), this, SLOT(OnSupportChange(bool)));
    connect(addSupport, SIGNAL(pressed()), this, SLOT(OnBtnPress()));
    connect(addSupport, SIGNAL(released()), this, SLOT(OnBtnRelease()));
    connect(addSupport, SIGNAL(clicked(bool)), this, SLOT(OnBtnClick()));
    connect(delSupport, SIGNAL(pressed()), this, SLOT(OnBtnPress()));
    connect(delSupport, SIGNAL(released()), this, SLOT(OnBtnRelease()));
    connect(delSupport, SIGNAL(clicked(bool)), this, SLOT(OnBtnClick()));
    connect(autoSupport, SIGNAL(pressed()), this, SLOT(OnBtnPress()));
    connect(autoSupport, SIGNAL(released()), this, SLOT(OnBtnRelease()));
    connect(autoSupport, SIGNAL(clicked(bool)), this, SLOT(OnBtnClick()));
    connect(editSupport, SIGNAL(pressed()), this, SLOT(OnBtnPress()));
    connect(editSupport, SIGNAL(released()), this, SLOT(OnBtnRelease()));
    connect(editSupport, SIGNAL(clicked(bool)), this, SLOT(OnBtnClick()));
    connect(otherSettings, SIGNAL(pressed()), this, SLOT(OnBtnPress()));
    connect(otherSettings, SIGNAL(released()), this, SLOT(OnBtnRelease()));
    connect(otherSettings, SIGNAL(clicked(bool)), this, SLOT(ShowOtherDialog()));
    connect(addbase, SIGNAL(pressed()), this, SLOT(OnBtnPress()));
    connect(addbase, SIGNAL(released()), this, SLOT(OnBtnRelease()));
    connect(addbase, SIGNAL(clicked(bool)), this, SLOT(toggleBase()));
    connect(supportrateL, SIGNAL(clicked(bool)), this, SLOT(rateSelect()));
    connect(supportrateM, SIGNAL(clicked(bool)), this, SLOT(rateSelect()));
    connect(supportrateH, SIGNAL(clicked(bool)), this, SLOT(rateSelect()));
    connect(supportsizeS, SIGNAL(clicked(bool)), this, SLOT(sizeSelect()));
    connect(supportsizeM, SIGNAL(clicked(bool)), this, SLOT(sizeSelect()));
    connect(supportsizeL, SIGNAL(clicked(bool)), this, SLOT(sizeSelect()));

    connect(delAllSupport, SIGNAL(pressed()), this, SLOT(OnBtnPress()));
    connect(delAllSupport, SIGNAL(released()), this, SLOT(OnBtnRelease()));
    connect(delAllSupport, SIGNAL(clicked(bool)), this, SLOT(ShowDelAllSupport()));

    mainlayout->addWidget(machinelabel);
    mainlayout->addWidget(machinewidget);
    mainlayout->addWidget(slicelabel);
    mainlayout->addWidget(slicewidget);
    mainlayout->addWidget(supportlabel);
    mainlayout->addWidget(supportwidget);
    mainlayout->addStretch(1);
//    mainwidget->setLayout(mainlayout);
//    scpanel->setWidget(mainwidget);

    this->setLayout(mainlayout);

    UpdateData();
}

void NRightPanel::initData()
{
    QString data;
    getData("mksdlp_pixelx", data, "2560");
    xresolutionedit->setObjectName("mksdlp_pixelx");
    xresolutionedit->setText(data);
    xresolutionedit->setValidator(intvalidator);
    connect(xresolutionedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));

    getData("mksdlp_pixely", data, "1440");
    yresolutionedit->setObjectName("mksdlp_pixely");
    yresolutionedit->setText(data);
    yresolutionedit->setValidator(intvalidator);
    connect(yresolutionedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));

    getData("mksdlp_xsize", data, "121");
    xlenedit->setObjectName("mksdlp_xsize");
    xlenedit->setText(data);
    xlenedit->setValidator(this->floatvalidator);
    connect(xlenedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));

    getData("mksdlp_ysize", data, "68.0625");
    ylenedit->setObjectName("mksdlp_ysize");
    ylenedit->setText(data);
    ylenedit->setValidator(this->floatvalidator);
    connect(ylenedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));

    getData("mksdlp_zsize", data, "150.0");
    zlenedit->setObjectName("mksdlp_zsize");
    zlenedit->setText(data);
    zlenedit->setValidator(this->floatvalidator);
    connect(zlenedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));

    getData("mksdlp_thickness", data, "0.1");
    thicknessedit->setObjectName("mksdlp_thickness");
    thicknessedit->setText(data);
    thicknessedit->setValidator(this->floatvalidator);
    connect(thicknessedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));

    getData("mksdlp_expose", data, "8");
    exposeedit->setObjectName("mksdlp_expose");
    exposeedit->setText(data);
    exposeedit->setValidator(this->intvalidator);
    connect(exposeedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));

    getData("mksdlp_lftime", data, "3");
    offedit->setObjectName("mksdlp_lftime");
    offedit->setText(data);
    offedit->setValidator(this->intvalidator);
    connect(offedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));

    getData("mksdlp_bexpose", data, "60");
    botexposeedit->setObjectName("mksdlp_bexpose");
    botexposeedit->setText(data);
    botexposeedit->setValidator(this->intvalidator);
    connect(botexposeedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));

    getData("mksdlp_blayer", data, "3");
    botcountedit->setObjectName("mksdlp_blayer");
    botcountedit->setText(data);
    botcountedit->setValidator(this->intvalidator);
    connect(botcountedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));

    getData("mksdlp_botliftdis", data, "5");
    botliftdisedit->setObjectName("mksdlp_botliftdis");
    botliftdisedit->setText(data);
    botliftdisedit->setValidator(this->intvalidator);
    connect(botliftdisedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));
    getData("mksdlp_botliftspeed", data, "65");
    botliftspeededit->setObjectName("mksdlp_botliftspeed");
    botliftspeededit->setText(data);
    botliftspeededit->setValidator(this->intvalidator);
    connect(botliftspeededit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));
    getData("mksdlp_liftdis", data, "5");
    liftdisedit->setObjectName("mksdlp_liftdis");
    liftdisedit->setText(data);
    liftdisedit->setValidator(this->intvalidator);
    connect(liftdisedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));
    getData("mksdlp_liftspeed", data, "65");
    liftspeededit->setObjectName("mksdlp_liftspeed");
    liftspeededit->setText(data);
    liftspeededit->setValidator(this->intvalidator);
    connect(liftspeededit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));
    getData("mksdlp_returnspeed", data, "150");
    returnspeededit->setObjectName("mksdlp_returnspeed");
    returnspeededit->setText(data);
    returnspeededit->setValidator(this->intvalidator);
    connect(returnspeededit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));
    getData("mksdlp_botbrightnessPVM", data, "255");
    botbrightnessPVMedit->setObjectName("mksdlp_botbrightnessPVM");
    botbrightnessPVMedit->setText(data);
    botbrightnessPVMedit->setValidator(this->intvalidator);
    connect(botbrightnessPVMedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));
    getData("mksdlp_brightnessPVM", data, "255");
    brightnessPVMedit->setObjectName("mksdlp_brightnessPVM");
    brightnessPVMedit->setText(data);
    brightnessPVMedit->setValidator(this->intvalidator);
    connect(brightnessPVMedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));

    getData("mksdlp_slicetype", data, "1");
    slicetype->setCurrentIndex(data.toInt());
    connect(slicetype, SIGNAL(currentIndexChanged(const int)), this, SLOT(SliceTypeChange(int)));

    getData("mksdlp_supportuplift", data, "5");
    supportupedit->setObjectName("mksdlp_supportuplift");
    supportupedit->setText(data);
    supportupedit->setValidator(this->floatvalidator);
    connect(supportupedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));

    getData("mksdlp_basesize", data, "5");
    basesizeedit->setObjectName("mksdlp_basesize");
    basesizeedit->setText(data);
    basesizeedit->setValidator(this->floatvalidator);
    connect(basesizeedit, SIGNAL(textChanged(const QString&)), this, SLOT(judgeText()));
}

void NRightPanel::getData(QString datakey, QString &data, QString defaultvalue)
{
    QSettings settings("makerbase", "mksdlp");
    data = settings.value(datakey).toString();
    if(data == "")
    {
        settings.setValue(datakey, defaultvalue);
        data = defaultvalue;
    }
}

void NRightPanel::dealData(QString datakey, QString data)
{
    QSettings settings("makerbase", "mksdlp");
    settings.setValue(datakey, data);
    if(keepxyresolution->isChecked() && !settexting)
    {
        settexting = true;
        float result;
        if(datakey == "mksdlp_xsize")
        {
            result = yresolutionedit->text().toFloat()*(xlenedit->text().toFloat()/xresolutionedit->text().toFloat());
            ylenedit->setText(QString::number(result));
        }else if(datakey == "mksdlp_ysize")
        {
            result = xresolutionedit->text().toFloat()*(ylenedit->text().toFloat()/yresolutionedit->text().toFloat());
            xlenedit->setText(QString::number(result));
        }
//        qDebug() << "changing";
        settexting = false;
    }
}

void NRightPanel::ShowOtherDialog()
{
    otherDialog = new MDialog(mparent);
    otherDialog->setTitle(str_otherset);
    QWidget *dlgpanel = new QWidget();
    QString data = "";
    bool b = false;
    QGridLayout *dlglayout = new QGridLayout();
    dlglayout->setSpacing(5);
//    dlglayout->setMargin(0);
    QLabel *toptext = new QLabel();
    toptext->setStyleSheet("background-color:#12997a;color:#f2f2f2;padding-left:5px;");
    toptext->setText(str_sup_top);
    QLabel *midtext = new QLabel();
    midtext->setStyleSheet("background-color:#12997a;color:#f2f2f2;padding-left:5px;");
    midtext->setText(str_sup_mid);
    QLabel *bottext = new QLabel();
    bottext->setStyleSheet("background-color:#12997a;color:#f2f2f2;padding-left:5px;");
    bottext->setText(str_sup_bot);
    QLabel *toptype = new QLabel();
    toptype->setText(str_toptype);
    QComboBox *toptypecb = new QComboBox();
    toptypecb->setObjectName("mksdlp_toptype");
    toptypecb->addItem(str_cone);
    toptypecb->addItem(str_cube);
    getData(toptypecb->objectName(), data, "0");
    toptypecb->setCurrentIndex(data.toInt());
    connect(toptypecb, SIGNAL(currentIndexChanged(int)),this,SLOT(saveCombobox()));
    QLabel *touchtype = new QLabel();
    touchtype->setText(str_touchtype);
    QComboBox *touchtypecb = new QComboBox();
    touchtypecb->setObjectName("mksdlp_touchtype");
    touchtypecb->addItem(str_sphere);
    touchtypecb->addItem(str_none);
    getData(touchtypecb->objectName(), data, "0");
    touchtypecb->setCurrentIndex(data.toInt());
    connect(touchtypecb, SIGNAL(currentIndexChanged(int)),this,SLOT(saveCombobox()));
    QLabel *touchtypesize = new QLabel();
    touchtypesize->setText(str_touchtypesize);
    QLineEdit *touchtypesizeedit = new QLineEdit();
//    touchtypesizeedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
    touchtypesizeedit->setObjectName("mksdlp_touchtypesize");
    touchtypesizeedit->setValidator(this->floatvalidator);
    getData(touchtypesizeedit->objectName(), data, "3");
    touchtypesizeedit->setText(data);
    connect(touchtypesizeedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    QLabel *toplength = new QLabel();
    toplength->setText(str_toplen);
    QLineEdit *toplengthedit = new QLineEdit();
//    toplengthedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
//    toplengthedit->setAlignment(Qt::AlignRight);
    toplengthedit->setObjectName("mksdlp_toplength");
    toplengthedit->setValidator(this->floatvalidator);
    getData(toplengthedit->objectName(), data, "3");
    toplengthedit->setText(data);
    connect(toplengthedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    QLabel *topwidth = new QLabel();
    topwidth->setText(str_topwidth);
    QLineEdit *topwidthedit = new QLineEdit();
//    topwidthedit->setStyleSheet("background-color:#ffffff;padding:0;color:#000000;margin:5px;margin-right:10px;");
//    toplengthedit->setAlignment(Qt::AlignRight);
    topwidthedit->setObjectName("mksdlp_topwidth");
    topwidthedit->setValidator(this->floatvalidator);
    getData(topwidthedit->objectName(), data, "3");
    topwidthedit->setText(data);
    connect(topwidthedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    QLabel *midtype = new QLabel();
    midtype->setText(str_midtype);
    QComboBox *midtypecb = new QComboBox();
    midtypecb->setObjectName("mksdlp_midtype");
    midtypecb->addItem(str_cone);
    midtypecb->addItem(str_cube);
    getData(midtypecb->objectName(), data, "0");
    midtypecb->setCurrentIndex(data.toInt());
    connect(midtypecb, SIGNAL(currentIndexChanged(int)),this,SLOT(saveCombobox()));
    QLabel *midlength = new QLabel();
    midlength->setText(str_midwidth);
    QLineEdit *midlengthedit = new QLineEdit();
    midlengthedit->setObjectName("mksdlp_supportmidsize");
    getData(midlengthedit->objectName(), data, "0.5");
    midlengthedit->setValidator(this->floatvalidator);
    midlengthedit->setText(data);
    connect(midlengthedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    QLabel *bottomtype = new QLabel();
    bottomtype->setText(str_bottype);
    QComboBox *bottomtypecb = new QComboBox();
    bottomtypecb->setObjectName("mksdlp_bottype");
    bottomtypecb->addItem(str_cone);
    bottomtypecb->addItem(str_cube);
    getData(bottomtypecb->objectName(), data, "0");
    bottomtypecb->setCurrentIndex(data.toInt());
    connect(bottomtypecb, SIGNAL(currentIndexChanged(int)),this,SLOT(saveCombobox()));
    QLabel *botdepth = new QLabel();
    botdepth->setText(str_botdepth);
    QLineEdit *botdepthedit = new QLineEdit();
    botdepthedit->setObjectName("mksdlp_botdepth");
    botdepthedit->setValidator(this->floatvalidator);
    getData(botdepthedit->objectName(), data, "1");
    botdepthedit->setText(data);
    connect(botdepthedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    QLabel *botwidth = new QLabel();
    botwidth->setText(str_botwidth);
    QLineEdit *botwidthedit = new QLineEdit();
    botwidthedit->setObjectName("mksdlp_botwidth");
    botwidthedit->setValidator(this->floatvalidator);
    getData(botwidthedit->objectName(), data, "1");
    botwidthedit->setText(data);
    connect(botwidthedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    QLabel *botconesize = new QLabel();
    botconesize->setText(str_botconesize);
    QLineEdit *botconesizeedit = new QLineEdit();
    botconesizeedit->setObjectName("mksdlp_botconesize");
    botconesizeedit->setValidator(this->floatvalidator);
    getData(botconesizeedit->objectName(), data, "1");
    botconesizeedit->setText(data);
    connect(botconesizeedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    QLabel *botballsize = new QLabel();
    botballsize->setText(str_botballsize);
    QLineEdit *botballsizeedit = new QLineEdit();
    botballsizeedit->setObjectName("mksdlp_botballsize");
    botballsizeedit->setValidator(this->floatvalidator);
    getData(botballsizeedit->objectName(), data, "1");
    botballsizeedit->setText(data);
    connect(botballsizeedit,SIGNAL(textChanged(QString)),this,SLOT(saveLineEdit()));
    dlglayout->addWidget(toptext, 0, 0, 1, 2);
    dlglayout->addWidget(toptype, 1, 0, 1, 1);
    dlglayout->addWidget(toptypecb, 1, 1, 1, 1);
    dlglayout->addWidget(touchtype, 2, 0, 1, 1);
    dlglayout->addWidget(touchtypecb, 2, 1, 1, 1);
    dlglayout->addWidget(touchtypesize, 3, 0, 1, 1);
    dlglayout->addWidget(touchtypesizeedit, 3, 1, 1, 1);
    dlglayout->addWidget(toplength, 4, 0, 1, 1);
    dlglayout->addWidget(toplengthedit, 4, 1, 1, 1);
    dlglayout->addWidget(topwidth, 5, 0, 1, 1);
    dlglayout->addWidget(topwidthedit, 5, 1, 1, 1);
    dlglayout->addWidget(midtext, 6, 0, 1, 2);
    dlglayout->addWidget(midtype, 7, 0, 1, 1);
    dlglayout->addWidget(midtypecb, 7, 1, 1, 1);
    dlglayout->addWidget(midlength, 8, 0, 1, 1);
    dlglayout->addWidget(midlengthedit, 8, 1, 1, 1);
    dlglayout->addWidget(bottext, 9, 0, 1, 2);
    dlglayout->addWidget(bottomtype, 10, 0, 1, 1);
    dlglayout->addWidget(bottomtypecb, 10, 1, 1, 1);
    dlglayout->addWidget(botdepth, 11, 0, 1, 1);
    dlglayout->addWidget(botdepthedit, 11, 1, 1, 1);
    dlglayout->addWidget(botwidth, 12, 0, 1, 1);
    dlglayout->addWidget(botwidthedit, 12, 1, 1, 1);
    dlglayout->addWidget(botconesize, 13, 0, 1, 1);
    dlglayout->addWidget(botconesizeedit, 13, 1, 1, 1);
    dlglayout->addWidget(botballsize, 14, 0, 1, 1);
    dlglayout->addWidget(botballsizeedit, 14, 1, 1, 1);
    dlgpanel->setLayout(dlglayout);
    otherDialog->setPanel(dlgpanel);
    otherDialog->exec();
}

void NRightPanel::saveCombobox()
{
    QComboBox *tempwidget = (QComboBox *)sender();
    dealData(tempwidget->objectName(),QString::number(tempwidget->currentIndex()));
}

void NRightPanel::saveLineEdit()
{
    QLineEdit *tempwidget = (QLineEdit *)sender();
    dealData(tempwidget->objectName(),tempwidget->text());
}

void NRightPanel::SupportShapeChange(int st)
{
    QString shape;
    switch(st){
    case 0:
        shape = "cylinder";
        break;
    case 1:
        shape = "square";
        break;
    default:
        shape = "square";
        break;
    }
    dealData("mksdlp_supportshape", QString::number(st));
    emit ChangeShape(shape);
}

void NRightPanel::SupportTypeChange(int st)
{
    QString stype;
    switch (st) {
    case 0:
        stype = "OutSide";
        break;
    case 1:
        stype = "EveryWhere";
        break;
    case 2:
        stype = "point";
        break;
    default:
        stype = "point";
        break;
    }
    dealData("mksdlp_supporttype", QString::number(st));
    emit ChangeType(stype);
}

void NRightPanel::judgeText()
{
    QLineEdit* mwidget = (QLineEdit*)sender();
    QString tt = mwidget->text();
    QString datakey = mwidget->objectName();
    if (datakey == "mksdlp_supportuplift" && tt.toFloat() < 0) {
        tt = QString::number(0);
        supportupedit->setText(0);
        mparent->showToast(str_errorzero,3);
    }
    dealData(datakey, tt);
}

void NRightPanel::OnMachineChange(bool isoff)
{
    machinelabel->setIsOff(isoff);
    if (isoff) {
        machinewidget->hide();
    }else {
        machinewidget->show();
        slicelabel->setIsOff(true);
        slicewidget->hide();
        supportlabel->setIsOff(true);
        supportwidget->hide();
    }
//    if(!isanifinish)
//    {
//        machinelabel->setIsOff(!isoff);
//        return;
//    }
//    isanifinish = false;
//    anigroup->clear();
//    int posx, posy, width, height;
//    getPP(machinewidget, posx, posy, width, height);
//    if(machinewidgetheight == -1 && height != 0)
//    {
//        machinewidgetheight = height;
//    }
//    int temp = machinewidgetheight;
//    animation = new QPropertyAnimation(machinewidget, "geometry", this);
//    animation->setDuration(150);
//    if(isoff)
//    {
//        temp = -temp;
//        animation->setStartValue(QRect(posx, posy, width, machinewidgetheight));
//        animation->setEndValue(QRect(posx, posy, width, 0));
//    }else{
//        animation->setStartValue(QRect(posx, posy, width, 0));
//        animation->setEndValue(QRect(posx, posy, width, machinewidgetheight));
//    }
//    anigroup->addAnimation(animation);

//    getPP(slicelabel, posx, posy, width, height);
//    animation = new QPropertyAnimation(slicelabel, "geometry", this);
//    animation->setDuration(150);
//    animation->setStartValue(QRect(posx, posy, width, height));
//    animation->setEndValue(QRect(posx, posy+temp, width, height));
//    anigroup->addAnimation(animation);

//    getPP(slicewidget, posx, posy, width, height);
//    animation = new QPropertyAnimation(slicewidget, "geometry", this);
//    animation->setDuration(150);
//    animation->setStartValue(QRect(posx, posy, width, height));
//    animation->setEndValue(QRect(posx, posy+temp, width, height));
//    anigroup->addAnimation(animation);

//    getPP(supportlabel, posx, posy, width, height);
//    animation = new QPropertyAnimation(supportlabel, "geometry", this);
//    animation->setDuration(150);
//    animation->setStartValue(QRect(posx, posy, width, height));
//    animation->setEndValue(QRect(posx, posy+temp, width, height));
//    anigroup->addAnimation(animation);

//    getPP(supportwidget, posx, posy, width, height);
//    animation = new QPropertyAnimation(supportwidget, "geometry", this);
//    animation->setDuration(150);
//    animation->setStartValue(QRect(posx, posy, width, height));
//    animation->setEndValue(QRect(posx, posy+temp, width, height));
//    anigroup->addAnimation(animation);

//    anigroup->start();
//    while(!isanifinish)
//    {
//        QApplication::processEvents();
//    }
//    OnSliceChange(true);
}

void NRightPanel::OnSliceChange(bool isoff)
{
    slicelabel->setIsOff(isoff);
    if (isoff) {
        slicewidget->hide();
    }else {
        slicewidget->show();
        machinelabel->setIsOff(true);
        machinewidget->hide();
        supportlabel->setIsOff(true);
        supportwidget->hide();
    }
//    if(!isanifinish)
//    {
//        slicelabel->setIsOff(!isoff);
//        return;
//    }
//    isanifinish = false;
//    anigroup->clear();
//    int posx, posy, width, height, temp;
//    getPP(slicewidget, posx, posy, width, height);
//    if(slicewidgetheight == -1 && height != 0)
//    {
//        slicewidgetheight = height;
//    }
//    temp = slicewidgetheight;
//    animation = new QPropertyAnimation(slicewidget, "geometry", this);
//    animation->setDuration(150);
//    if(isoff)
//    {
//        temp = -temp;
//        animation->setStartValue(QRect(posx, posy, width, slicewidgetheight));
//        animation->setEndValue(QRect(posx, posy, width, 0));
//    }else{
//        animation->setStartValue(QRect(posx, posy, width, 0));
//        animation->setEndValue(QRect(posx, posy,width, slicewidgetheight));
//    }
//    anigroup->addAnimation(animation);

//    getPP(supportlabel, posx, posy, width, height);
//    animation = new QPropertyAnimation(supportlabel, "geometry", this);
//    animation->setDuration(150);
//    animation->setStartValue(QRect(posx, posy, width, height));
//    animation->setEndValue(QRect(posx, posy+temp, width, height));
//    anigroup->addAnimation(animation);

//    getPP(supportwidget, posx, posy, width, height);
//    animation = new QPropertyAnimation(supportwidget, "geometry", this);
//    animation->setDuration(150);
//    animation->setStartValue(QRect(posx, posy, width, height));
//    animation->setEndValue(QRect(posx, posy+temp, width, height));
//    anigroup->addAnimation(animation);

//    anigroup->start();
}

void NRightPanel::OnSupportChange(bool isoff)
{
    supportlabel->setIsOff(isoff);
    if (isoff) {
        supportwidget->hide();
    }else {
        supportwidget->show();
        machinelabel->setIsOff(true);
        machinewidget->hide();
        slicelabel->setIsOff(true);
        slicewidget->hide();
    }
//    if(!isanifinish)
//    {
//        supportlabel->setIsOff(!isoff);
//        return;
//    }
//    isanifinish = false;
//    anigroup->clear();
//    int posx, posy, width, height, temp;
//    getPP(supportwidget, posx, posy, width, height);
//    if(supportwidgetheight == -1 && height != 0)
//    {
//        supportwidgetheight = height;
//    }
//    temp = supportwidgetheight;
//    animation = new QPropertyAnimation(supportwidget, "geometry", this);
//    animation->setDuration(150);
//    if(isoff)
//    {
//        animation->setStartValue(QRect(posx, posy, width, supportwidgetheight));
//        animation->setEndValue(QRect(posx, posy, width, 0));
//    }else{
//        animation->setStartValue(QRect(posx, posy, width, 0));
//        animation->setEndValue(QRect(posx, posy, width, supportwidgetheight));
//    }
//    anigroup->addAnimation(animation);
//    anigroup->start();
}
//void NRightPanel::OnMachineChange(bool isoff)
//{
//    qDebug() << "isoff" << isoff;
//    if(!isanifinish)
//    {
//        machinelabel->setIsOff(!isoff);
//        return;
//    }
//    while (isanifinish) {
//        isanifinish = false;
//        anigroup->clear();
//        int posx, posy, width, height;
//        getPP(machinewidget, posx, posy, width, height);
//        if(machinewidgetheight == -1 && height != 0)
//        {
//            machinewidgetheight = height;
//        }
//        int temp = machinewidgetheight;
//        qDebug() << "temp" << temp;
//        animation = new QPropertyAnimation(machinewidget, "geometry", this);
//        animation->setDuration(150);
//        if(isoff)
//        {
//            temp = -temp;
//            animation->setStartValue(QRect(posx, posy, width, machinewidgetheight));
//            animation->setEndValue(QRect(posx, posy, width, 0));
//            anigroup->addAnimation(animation);

//            getPP(slicelabel, posx, posy, width, height);
//            animation = new QPropertyAnimation(slicelabel, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, height));
//            animation->setEndValue(QRect(posx, posy+temp, width, height));
//            anigroup->addAnimation(animation);

//            getPP(slicewidget, posx, posy, width, height);
//            animation = new QPropertyAnimation(slicewidget, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, height));
//            animation->setEndValue(QRect(posx, posy+temp, width, height));
//            anigroup->addAnimation(animation);

//            getPP(supportlabel, posx, posy, width, height);
//            animation = new QPropertyAnimation(supportlabel, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, height));
//            animation->setEndValue(QRect(posx, posy+temp, width, height));
//            anigroup->addAnimation(animation);

//            getPP(supportwidget, posx, posy, width, height);
//            animation = new QPropertyAnimation(supportwidget, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, height));
//            animation->setEndValue(QRect(posx, posy+temp, width, height));
//            anigroup->addAnimation(animation);
//        }else{
//            animation->setStartValue(QRect(posx, posy, width, 0));
//            animation->setEndValue(QRect(posx, posy, width, machinewidgetheight));
//            anigroup->addAnimation(animation);

//            getPP(slicelabel, posx, posy, width, height);
//            animation = new QPropertyAnimation(slicelabel, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, height));
//            animation->setEndValue(QRect(posx, posy+temp, width, height));
//            anigroup->addAnimation(animation);

//            getPP(slicewidget, posx, posy, width, height);
//            animation = new QPropertyAnimation(slicewidget, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, height));
//            animation->setEndValue(QRect(posx, posy+temp, width, 0));
//            anigroup->addAnimation(animation);
//            temp = temp-height;

//            getPP(supportlabel, posx, posy, width, height);
//            animation = new QPropertyAnimation(supportlabel, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, height));
//            animation->setEndValue(QRect(posx, posy+temp, width, height));
//            anigroup->addAnimation(animation);

//            getPP(supportwidget, posx, posy, width, height);
//            animation = new QPropertyAnimation(supportwidget, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, height));
//            animation->setEndValue(QRect(posx, posy+temp, width, 0));
//            anigroup->addAnimation(animation);
//        }
//        anigroup->start();
//    }
//}

//void NRightPanel::OnSliceChange(bool isoff)
//{
//    if(!isanifinish)
//    {
//        slicelabel->setIsOff(!isoff);
//        return;
//    }
//    while (isanifinish) {
//        isanifinish = false;
//        anigroup->clear();
//        int posx, posy, width, height, temp;
//        getPP(slicewidget, posx, posy, width, height);
//        if(slicewidgetheight == -1 && height != 0)
//        {
//            slicewidgetheight = height;
//        }
//        temp = slicewidgetheight;
//        if(isoff)
//        {
//            temp = -temp;
//            animation = new QPropertyAnimation(slicewidget, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, slicewidgetheight));
//            animation->setEndValue(QRect(posx, posy, width, 0));
//            anigroup->addAnimation(animation);

//            getPP(supportlabel, posx, posy, width, height);
//            animation = new QPropertyAnimation(supportlabel, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, height));
//            animation->setEndValue(QRect(posx, posy+temp, width, height));
//            anigroup->addAnimation(animation);

//            getPP(supportwidget, posx, posy, width, height);
//            animation = new QPropertyAnimation(supportwidget, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, height));
//            animation->setEndValue(QRect(posx, posy+temp, width, height));
//            anigroup->addAnimation(animation);
//        }else{
//            getPP(machinewidget, posx, posy, width, height);
//            animation = new QPropertyAnimation(machinewidget, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, height));
//            qDebug() << "height" << height;
//            if (height > 0) {
//                animation->setEndValue(QRect(posx, posy-height, width, 0));
//            }else {
//                animation->setEndValue(QRect(posx, posy, width, 0));
//            }
//            anigroup->addAnimation(animation);
//            temp = height;

//            getPP(slicelabel, posx, posy, width, height);
//            animation = new QPropertyAnimation(slicelabel, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, height));
//            animation->setEndValue(QRect(posx, posy+temp, width, height));
//            anigroup->addAnimation(animation);

//            getPP(slicewidget, posx, posy, width, height);
//            animation = new QPropertyAnimation(slicewidget, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, 0));
//            animation->setEndValue(QRect(posx, posy,width, slicewidgetheight));
//            anigroup->addAnimation(animation);

//            temp = temp-slicewidgetheight;
//            getPP(supportwidget, posx, posy, width, height);
//            animation = new QPropertyAnimation(supportwidget, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, height));
//            animation->setEndValue(QRect(posx, posy+temp, width, height));
//            anigroup->addAnimation(animation);

//            getPP(supportlabel, posx, posy, width, height);
//            animation = new QPropertyAnimation(supportlabel, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, height));
//            animation->setEndValue(QRect(posx, posy+temp, width, height));
//            anigroup->addAnimation(animation);

//            getPP(supportwidget, posx, posy, width, height);
//            animation = new QPropertyAnimation(supportwidget, "geometry", this);
//            animation->setDuration(150);
//            animation->setStartValue(QRect(posx, posy, width, height));
//            if (height > 0) {
//                animation->setEndValue(QRect(posx, posy+temp-height, width, 0));
//            }else {
//                animation->setEndValue(QRect(posx, posy+temp, width, 0));
//            }
//            anigroup->addAnimation(animation);
//        }
//        anigroup->start();
//    }
//}

//void NRightPanel::OnSupportChange(bool isoff)
//{
//    if(!supportanifinish)
//    {
//        supportlabel->setIsOff(!isoff);
//        return;
//    }
//    while (supportanifinish) {
////        isanifinish = false;
//        supportanifinish = false;
//        supportanigroup->clear();
//        int posx, posy, width, height, temp;
//        getPP(supportwidget, posx, posy, width, height);
//        if(supportwidgetheight == -1 && height != 0)
//        {
//            supportwidgetheight = height;
//        }
//        temp = supportwidgetheight;
//        animation = new QPropertyAnimation(supportwidget, "geometry", this);
//        animation->setDuration(150);
//        if(isoff)
//        {
//            animation->setStartValue(QRect(posx, posy, width, supportwidgetheight));
//            animation->setEndValue(QRect(posx, posy, width, 0));
//        }else{
//            animation->setStartValue(QRect(posx, posy, width, 0));
//            animation->setEndValue(QRect(posx, posy, width, supportwidgetheight));
//        }
//        supportanigroup->addAnimation(animation);
//        supportanigroup->start();
//        if (!isoff) {
//            machinelabel->clickevent(true);
//            slicelabel->clickevent(true);
//        }
//    }
//}

void NRightPanel::getPP(QWidget *w, int &posx, int &posy, int &width, int &height)
{
    posx = w->pos().x();
    posy = w->pos().y();
    width = w->width();
    height = w->height();
}

void NRightPanel::anifinished()
{
    isanifinish = true;
    QParallelAnimationGroup *group = (QParallelAnimationGroup *)sender();
    QString name = group->objectName();
    qDebug() << "groupname" << name;
    if (name == "machineanigroup") {
        machineanifinish = true;
//        if (!machinelabel->getIsOff() && !slicelabel->getIsOff()) {
//            slicelabel->clickevent(true);
//        }else if(!slicelabel->getIsOff() && machinelabel->getIsOff() && !supportlabel->getIsOff()) {
//            supportlabel->clickevent(true);
//        }else if (!supportlabel->getIsOff() && machinelabel->getIsOff()) {
//            slicelabel->clickevent(true);
//        }
    }else if (name == "sliceanigroup") {
        sliceanifinish = true;
//        if (!machinelabel->getIsOff() && slicelabel->getIsOff() && !supportlabel->getIsOff()) {
//            supportlabel->clickevent(true);
//        }else if (!slicelabel->getIsOff() && !machinelabel->getIsOff()) {
//            machinelabel->clickevent(true);
//        }
    }else if (name == "supportanigroup") {
        supportanifinish = true;
//        if (!supportlabel->getIsOff() && !machinelabel->getIsOff()) {
//            machinelabel->clickevent(true);
//        }

    }
}

void NRightPanel::OnModeChange()
{
    QPushButton *mbtn = (QPushButton*)sender();
    QString mtype = mbtn->objectName();
}

void NRightPanel::OnBtnPress()
{
    QPushButton *mbtn = (QPushButton*)sender();
    QString tm = mbtn->objectName();
//    QString sheet = "background-color:#126e59;color:#14d39f;border-radius:10px;border:0px;margin-top:20px;"
//                    "font:18px;font-weight:bold;width:170px;height:22px;";
    QString sheet = mbtn->styleSheet();
    sheet.replace("background-color:#ffffff;color:#0f8764;", "background-color:#126e59;color:#14d39f;");

//    if(tm == "del")
//    {
//        sheet = "background-color:#126e59;color:#14d39f;border-radius:10px;border:0px;margin-top:10px;"
//                "font:18px;font-weight:bold;width:170px;height:22px;";
//    }else if(tm == "auto")
//    {
//        sheet = "background-color:#126e59;color:#14d39f;border-radius:10px;border:0px;margin-top:10px;margin-bottom:10px;"
//                "font:18px;font-weight:bold;width:170px;height:22px;";
//    }
    mbtn->setStyleSheet(sheet);
}

void NRightPanel::OnBtnRelease()
{
    QPushButton *mbtn = (QPushButton*)sender();
    QString tm = mbtn->objectName();
    QString sheet = mbtn->styleSheet();
    sheet.replace("background-color:#126e59;color:#14d39f;", "background-color:#ffffff;color:#0f8764;");

//    QString sheet = "background-color:#ffffff;color:#0f8764;border-radius:10px;border:0px;margin-top:20px;"
//                    "font:18px;font-weight:bold;width:170px;height:22px;";
//    if(tm == "del")
//    {
//        sheet = "background-color:#ffffff;color:#0f8764;border-radius:10px;border:0px;margin-top:10px;"
//                "font:18px;font-weight:bold;width:170px;height:22px;";
//    }else if(tm == "auto")
//    {
//        sheet = "background-color:#ffffff;color:#0f8764;border-radius:10px;border:0px;margin-top:10px;margin-bottom:10px;"
//                "font:18px;font-weight:bold;width:170px;height:22px;";
//    }
    mbtn->setStyleSheet(sheet);
}
void NRightPanel::ShowDelAllSupport() {
    if(selectid == -1)
    {
        mparent->showToast(str_toast, 2);
        return;
    }
    delallsupportdialog = new MDialog(mparent);
    delallsupportdialog->setMinimumSize(240, 128);
    QWidget *mwidget = new QWidget();
    QVBoxLayout *mlayout = new QVBoxLayout();
    QLabel *tt = new QLabel();
    tt->setText(str_dialogcomfirm);
    tt->setAlignment(Qt::AlignCenter);
    QHBoxLayout *botlayout = new QHBoxLayout();
    QPushButton *preview, *ok;
    preview = new QPushButton();
    preview->setText(str_cancel);
    preview->setMaximumSize(80, 20);
    preview->setMinimumSize(80, 20);
    preview->setStyleSheet("background-color:#126e58;font:14px;color:#ffffff;border:0px;");
    connect(preview, SIGNAL(clicked(bool)), this, SLOT(closedialog()));
    connect(preview, SIGNAL(pressed()), delallsupportdialog, SLOT(btnPress()));
    connect(preview, SIGNAL(released()), delallsupportdialog, SLOT(btnRelease()));
    ok = new QPushButton();
    ok->setText(str_comfirm);
    ok->setMaximumSize(80, 20);
    ok->setMinimumSize(80, 20);
    ok->setStyleSheet("background-color:#126e58;font:14px;color:#ffffff;border:0px;");
    connect(ok, SIGNAL(clicked(bool)), this, SLOT(closedelall()));
    connect(ok, SIGNAL(pressed()), delallsupportdialog, SLOT(btnPress()));
    connect(ok, SIGNAL(released()), delallsupportdialog, SLOT(btnRelease()));
    botlayout->addWidget(preview);
    botlayout->addWidget(ok);
    mlayout->addWidget(tt, 1);
    mlayout->addLayout(botlayout);
    mwidget->setLayout(mlayout);
    delallsupportdialog->setPanel(mwidget);
    delallsupportdialog->show();
}
void NRightPanel::closedialog() {
    delallsupportdialog->close();
}
void NRightPanel::closedelall() {
    delallsupportdialog->close();
    emit DelAllSupport();
}
void NRightPanel::OnBtnClick()
{
    if(selectid == -1)
    {
        mparent->showToast(str_toast, 2);
        return;
    }
    QPushButton *mbtn = (QPushButton*)sender();
    QString objn = mbtn->objectName();
    emit ChangeMode(objn);
    addSupport->setText(str_addsup);
    addSupport->setObjectName("add");
    delSupport->setText(str_delsup);
    delSupport->setObjectName("del");
    autoSupport->setText(str_autosup);
    autoSupport->setObjectName("auto");
    editSupport->setText(str_editsup);
    editSupport->setObjectName("edit");
    if(objn != "move")
    {
        mbtn->setText(str_finish);
        mbtn->setObjectName("move");
    }
}

void NRightPanel::toggleBase()
{
    if(selectid == -1)
    {
        mparent->showToast(str_toast, 2);
        return;
    }
    smd = mparent->getModelInstance()[selectid];
    if(smd->hasBase)
    {
        smd->setHasBase(false);
        addbase->setText(str_add);
    }else{
        smd->setHasBase(true);
        addbase->setText(str_remove);
    }
    if (smd->hasuplift) {
        emit ChangeMode("auto");
    }
}

void NRightPanel::SliceTypeChange(int st)
{
    dealData("mksdlp_slicetype", QString::number(st));
}

void NRightPanel::rateSelect()
{
    QPushButton *mbtn = (QPushButton*)sender();
    QString objn = mbtn->objectName();
    mparent->setData("mksdlp_supportrate", objn);
    UpdateData();
}

void NRightPanel::sizeSelect()
{
    QPushButton *mbtn = (QPushButton*)sender();
    QString objn = mbtn->objectName();
    if(objn == "S")
    {
        mparent->setData("mksdlp_supportbsize", "5");
        mparent->setData("mksdlp_supportmsize", "0.3");
    }else if(objn == "M")
    {
        mparent->setData("mksdlp_supportbsize", "10");
        mparent->setData("mksdlp_supportmsize", "0.5");
    }else if(objn == "L")
    {
        mparent->setData("mksdlp_supportbsize", "10");
        mparent->setData("mksdlp_supportmsize", "1.0");
    }else{
        mparent->setData("mksdlp_supportbsize", "10");
        mparent->setData("mksdlp_supportmsize", "0.5");
    }
    UpdateData();
}

void NRightPanel::UpdateData()
{
    QString data;
    mparent->getData("mksdlp_supportrate", data, "30");
    supportrateL->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:2px;border:0px;margin-top:10px;"
                                "margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    supportrateM->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:2px;border:0px;margin-top:10px;"
                                "margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    supportrateH->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:2px;border:0px;margin-top:10px;"
                                "margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    if(data == supportrateL->objectName())
    {
        supportrateL->setStyleSheet("background-color:#126e59;color:#14d39f;;border-radius:2px;border:0px;"
                                    "margin-top:10px;margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    }else if(data == supportrateM->objectName())
    {
        supportrateM->setStyleSheet("background-color:#126e59;color:#14d39f;;border-radius:2px;border:0px;"
                                    "margin-top:10px;margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    }else if(data == supportrateH->objectName())
    {
        supportrateH->setStyleSheet("background-color:#126e59;color:#14d39f;;border-radius:2px;border:0px;"
                                    "margin-top:10px;margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    }else{
        mparent->setData("mksdlp_supportrate", "30");
        supportrateM->setStyleSheet("background-color:#126e59;color:#14d39f;;border-radius:2px;border:0px;"
                                    "margin-top:10px;margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    }
    mparent->getData("mksdlp_supportuplift", data, "5");
    supportupedit->setText(data);
    mparent->getData("mksdlp_basesize", data, "5");
    basesizeedit->setText(data);
    mparent->getData("mksdlp_supportmsize", data, "0.5");
    supportsizeL->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:2px;border:0px;margin-top:10px;"
                                "margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    supportsizeM->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:2px;border:0px;margin-top:10px;"
                                "margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    supportsizeS->setStyleSheet("background-color:#ffffff;color:#0f8764;border-radius:2px;border:0px;margin-top:10px;"
                                "margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    if(data == "0.3")
    {
        supportsizeS->setStyleSheet("background-color:#126e59;color:#14d39f;;border-radius:2px;border:0px;"
                                    "margin-top:10px;margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    }else if(data == "0.5")
    {
        supportsizeM->setStyleSheet("background-color:#126e59;color:#14d39f;;border-radius:2px;border:0px;"
                                    "margin-top:10px;margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    }else if(data == "1.0")
    {
        supportsizeL->setStyleSheet("background-color:#126e59;color:#14d39f;;border-radius:2px;border:0px;"
                                    "margin-top:10px;margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    }else{
        mparent->setData("mksdlp_supportmsize", "0.5");
        mparent->setData("mksdlp_supportbsize", "10");
        supportsizeM->setStyleSheet("background-color:#126e59;color:#14d39f;;border-radius:2px;border:0px;"
                                    "margin-top:10px;margin-left:5px;font:14px;font-weight:bold;width:20px;height:20px;");
    }
}

void NRightPanel::UpdateLanguage()
{
    str_addsup = tr("Add Support");
    str_delsup = tr("Del Support");
    str_autosup = tr("Auto Support");
    str_add = tr("Add Base");
    str_remove = tr("Del Base");
    str_finish = tr("Finish");
    str_toast = tr("please select a model");
    str_otherset = tr("Other Setting");
    str_sup_top = tr("Support Top");
    str_sup_mid = tr("Support Mid");
    str_sup_bot = tr("Support Bottom");
    str_toptype = tr("Top Type:");
    str_cone = tr("Cone");
    str_cube = tr("Cube");
    str_touchtype = tr("Touch Type:");
    str_sphere = tr("Sphere");
    str_none = tr("None");
    str_toplen = tr("Top Length:");
    str_midtype = tr("Mid Type:");
    str_midwidth = tr("Mid Width:");
    str_bottype = tr("Bot Type:");
    str_botdepth = tr("Bot Depth:");
    str_botwidth = tr("Bot Width:");
    str_touchtypesize = tr("Touch Type Size:");
    str_topwidth = tr("Top Width:");
    str_botconesize = tr("Bot Cone Size:");
    str_botballsize = tr("Bot Ball Size:");
    machinelabel->setText(tr("Machine Setting"));
    xresolutionlabel->setText(tr("x resolution(pixel)"));
    yresolutionlabel->setText(tr("y resolution(pixel)"));
    xlenlabel->setText(tr("x size(mm)"));
    ylenlabel->setText(tr("y size(mm)"));
    zlenlabel->setText(tr("z size(mm)"));
    keepxyresolution->setText(tr("lock ratio"));
    slicelabel->setText(tr("Slice Setting"));
    thicknesslabel->setText(tr("thickness(mm)"));
    exposelabel->setText(tr("expose time(s)"));
    offlabel->setText(tr("light off time(s)"));
    botexposelabel->setText(tr("bottom expose(s)"));
    botcountlabel->setText(tr("bottom layer"));
    slicetypelabel->setText(tr("light curing type"));
    slicetype->setItemText(0, tr("X forward"));
    slicetype->setItemText(1, tr("X direction"));
    supportlabel->setText(tr("Support Setting"));
    supporttypelabel->setText(tr("Support type:"));
    supportshapelabel->setText(tr("Support shape:"));
    supporttype->setItemText(0,tr("OutSide"));
    supporttype->setItemText(1,tr("EveryWhere"));
//    supporttype->setItemText(2,tr("free"));
    supportshape->setItemText(0,tr("cube"));
    supportshape->setItemText(1,tr("cylinder"));
    addSupport->setText(tr("Add Support"));
    addSupport->setObjectName("add");
    delSupport->setText(tr("Del Support"));
    autoSupport->setText(tr("Auto Support"));
    delSupport->setObjectName("del");
    autoSupport->setObjectName("auto");
    str_editsup = tr("Edit Support");
    editSupport->setText(str_editsup);
    editSupport->setObjectName("edit");
    supportratelabel->setText(tr("Support density:"));
    supportrateM->setText(tr("M"));
    supportrateL->setText(tr("L"));
    supportrateH->setText(tr("H"));
    supportsizelabel->setText(tr("Support Size:"));
    supportsizeS->setText(tr("sizeS"));
    supportsizeM->setText(tr("sizeM"));
    supportsizeL->setText(tr("sizeL"));
    supportuplabel->setText(tr("Uplift:"));
    basesizelabel->setText(tr("Base size:"));
    otherSettings->setText(str_otherset);
    str_delallsupport = tr("Del All Support");
    delAllSupport->setText(str_delallsupport);
    str_dialogcomfirm = tr("Delete all support?");
    str_cancel = tr("Cancel");
    str_comfirm = tr("Confirm");
    returnspeedlabel->setText(tr("return speed"));
    brightnessPVMlabel->setText(tr("light PVM"));
    botbrightnessPVMlabel->setText(tr("bottom light PVM"));
    liftspeedlabel->setText(tr("lift speed"));
    liftdislabel->setText(tr("lift dis"));
    botliftdislabel->setText(tr("bottom lift dis"));
    botliftspeedlabel->setText(tr("bottom lift speed"));
    str_errorzero = tr("can not less than zero.");
    OnModelSelected(selectid);
    emit ChangeMode("move");

    toptext->setText(str_sup_top);
    midtext->setText(str_sup_mid);
    bottext->setText(str_sup_bot);
    toptype->setText(str_toptype);
    toptypecb->setItemText(0,str_cone);
    toptypecb->setItemText(1,str_cube);
    touchtype->setText(str_touchtype);
    touchtypecb->setItemText(0,str_sphere);
    touchtypecb->setItemText(1,str_none);
    touchtypesize->setText(str_touchtypesize);
    toplength->setText(str_toplen);
    topwidth->setText(str_topwidth);
    midtype->setText(str_midtype);
    midtypecb->setItemText(0,str_cone);
    midtypecb->setItemText(1,str_cube);
    midlength->setText(str_midwidth);
    bottomtype->setText(str_bottype);
    bottomtypecb->setItemText(0,str_cone);
    bottomtypecb->setItemText(1,str_cube);
    botdepth->setText(str_botdepth);
    botwidth->setText(str_botwidth);
    botconesize->setText(str_botconesize);
    botballsize->setText(str_botballsize);
}
