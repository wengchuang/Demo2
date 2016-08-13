#include "demohandler.h"
#include <QDebug>
#include "algargs.h"


DemoHandler::DemoHandler(const IHandlerType& type,const int& id):IHandler(type,id)
{

}
bool DemoHandler::handlerExec(void* buf,const int& length)
{
    T_AttachData data;
    memcpy(&data,buf,length);
    qDebug("data:");
    qDebug("idention: 0x%02x",data.recordItem.idention);
    qDebug("status: 0x%02x",data.recordItem.status);
    return true;
}

