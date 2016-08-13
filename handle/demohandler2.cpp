#include "demohandler2.h"
#include "algargs.h"
#include <QDebug>
DemoHandler2::DemoHandler2(const IHandlerType& type,const int& id ):IHandler(type,id)
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
    T_AttachData data;
    memcpy(&data,buf,length);
    qDebug("data:");
    qDebug("idention: 0x%02x",data.recordItem.idention);
    qDebug("status: 0x%02x",data.recordItem.status);
    return true;
}


