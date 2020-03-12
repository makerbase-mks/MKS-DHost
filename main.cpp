#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QFontDatabase>
#include <QCoreApplication>
#include <QTranslator>
#include <QSettings>
//#include <Windows.h>

//LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException){//程式异常捕获
//    /*
//      ***保存数据代码***
//    */
//    //这里弹出一个错误对话框并退出程序
//    EXCEPTION_RECORD* record = pException->ExceptionRecord;
//    QString errCode(QString::number(record->ExceptionCode,16)),errAdr(QString::number((uint)record->ExceptionAddress,16)),errMod;
//    qDebug() << "异常退出---" << errCode << errAdr;
//    QMessageBox::critical(NULL,"程式崩溃","<FONT size=4><div><b>对于发生的错误，表示诚挚的歉意</b><br/></div>"+
//        QString("<div>错误代码：%1</div><div>错误地址：%2</div></FONT>").arg(errCode).arg(errAdr),
//        QMessageBox::Ok);
//    return EXCEPTION_EXECUTE_HANDLER;
//}

void myMessageOutput(QtMsgType type,const char *msg)
{
//    QDateTime current_date_time = QDateTime::currentDateTime();
//    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    QString txt;
    switch (type) {
    //调试信息提示
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;

        //一般的warning提示
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
        //严重错误提示
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
        //致命错误提示
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }
    QFile outFile("mksdhost.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
//    ts << endl<< current_date << txt << endl;
    ts << endl << txt << endl;
}

int main(int argc, char *argv[])
{
//    qInstallMsgHandler(myMessageOutput);
    QApplication a(argc, argv);
//    int frontid = QFontDatabase::addApplicationFont(QCoreApplication::applicationDirPath()+"/font/NotoSansHans-Regular.otf");
//    QString fn = QFontDatabase::applicationFontFamilies(frontid).at(0);
//    qDebug()<< fn;
//    frontid = QFontDatabase::addApplicationFont(QCoreApplication::applicationDirPath()+"/font/NotoSansHans-Medium.otf");
//    fn = QFontDatabase::applicationFontFamilies(frontid).at(0);
//    qDebug()<<fn;
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
    QString bd = "";
    QSettings settings("makerbase", "mksdlp");
    bd = settings.value("reset_data", "-1").toString();
    QString Version = "201901012";
    if(bd != Version)
    {
        settings.clear();
        settings.setValue("reset_data", Version);
    }
    bd = settings.value("mksdlp_language").toString();
    if(bd == "")
    {
        bd = ":/resource/language/cn.qm";
    }
    QSettings settings1("HKEY_CLASSES_ROOT\\*\\shell\\MKSDHOST",QSettings::NativeFormat);
    settings1.setValue(".","使用MKSDHOST转为mdlp");
    QSettings settings2("HKEY_CLASSES_ROOT\\*\\shell\\MKSDHOST\\command",QSettings::NativeFormat);
    settings2.setValue(".", "\""+QApplication::applicationFilePath().replace("/", "\\")+"\" change \"%1\"");

//    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);//注冊异常捕获函数

    QTranslator trans;
    trans.load(bd);
    a.installTranslator(&trans);
    MainWindow w;

    //    // 获取命令行参数
    QStringList arguments = QCoreApplication::arguments();
    if (arguments.count() > 2) {
        QString str = arguments.at(1);
        if (str == "change") {
            QString str2 = arguments.at(2);
             w.changeFile(str2);
        }
    }else {
        w.show();
    }
    return a.exec();
}

