#include "demohandler.h"
#include <QDebug>
#include "algargs.h"


DemoHandler::DemoHandler(const IHandlerType& type,const int& id):IHandler(type,id)
{

}
bool DemoHandler::handlerExec(void* buf,const int& length)
{
    T_AttachData* pDdata = (T_AttachData*)buf;

    qDebug("data:");
    qDebug("idention: 0x%02x",pDdata->recordItem.idention);
    qDebug("status: 0x%02x",pDdata->recordItem.status);
    pDdata->recordItem.idention += 0x10;
    pDdata->recordItem.status  += 0x3045;
    return true;
}

