#include <QApplication>
#include <QTextCodec>
#include "systemsource.h"
#include "mainwindown.h"

int main(int argc,char*argv[])
{
    int ret;
    QApplication app(argc,argv);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));


    #if defined(Q_OS_WIN32)
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    #elif defined(Q_OS_LINUX)
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    #endif

    if(!SystemSource::getInstance()->sourceInit()){
        qDebug("SystemSource init failed!");
        return -1;
    }


    MainWindown win;
    win.showMaximized();

    ret = app.exec();

    delete  SystemSource::getInstance();

    return ret;


}
