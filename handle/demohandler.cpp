#include "demohandler.h"
#include <QDebug>


DemoHandler::DemoHandler(QString name):IHandler(name)
{

}
bool DemoHandler::handlerExec(void* buf,const int& length)
{
    qDebug("demo2");
    return true;
}

