#include "demohandler.h"
#include <QDebug>


DemoHandler::DemoHandler(QString name,const IHandlerType& type,const int& id):IHandler(name,type,id)
{

}
bool DemoHandler::handlerExec(void* buf,const int& length)
{
    return true;
}

