#include "demohandler2.h"
#include <QDebug>
DemoHandler2::DemoHandler2(QString name):IHandler(name)
{
}
bool DemoHandler2::handlerInit()
{
    qDebug()<<"DemoHandler2::handlerInit()";
    return true;
}
bool DemoHandler2::handlerUninit()
{
    qDebug()<<"DemoHandler2::handlerUninit()";
    return true;
}
bool DemoHandler2::handlerExec(void* buf,const int& length)
{
    qDebug()<<"DemoHandler2::handlerExec ...";

    return true;
}


