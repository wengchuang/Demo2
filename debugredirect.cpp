#include <QFile>
#include <QDateTime>
#include <QTextEdit>
//#include "centerwidget.h"
#include "debugredirect.h"
//#define LOGFILENAME     "debuglog.txt"
static QFile outFile;
static QTextStream ts;

void messageHandler(QtMsgType type,const char*msg){
    #if 1
    QString txt;
    switch(type){
    case QtDebugMsg:
        break;
    case QtWarningMsg:
        txt = QString("Warning:");
        break;
    case QtCriticalMsg:
        txt = QString("Critical:");
        break;
    case QtFatalMsg:
        txt = QString("Fatal:");
        abort();
    }

    txt += QString::fromLocal8Bit(msg);



    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
#if 0
    if(!outFile.isOpen()){
        outFile.setFileName(LOGFILENAME);
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        ts.setDevice(&outFile);
    }
    ts<<dateTime<<endl<<txt<<endl;
#endif
    dateTime +="\n";

   // QString logTxt = dateTime+"    "+txt;
   // CenterWidget::updateLog(type,logTxt);
    qDebug()<<txt;
        #endif
}
