#include "deviceoprmanager.h"
#include <uarttransferopr.h>

DeviceOprManager* DeviceOprManager::instance = NULL;
DeviceOprManager* DeviceOprManager::getInstance()
{
    if(!instance){
        instance = new DeviceOprManager;
    }
    return instance;
}
DeviceOprManager::DeviceOprManager()
{
}

bool DeviceOprManager::managerOprInit()
{
    bool ret = false;
    ITransferOpr* tmpOpr;
    ret = constructTransfers();
    if(ret){
        foreach (tmpOpr, oprs) {
           ret = tmpOpr->transferInit();
           if(!ret){
               break;
           }
        }
    }
    return ret;
}
bool DeviceOprManager::constructTransfers()
{
    bool ret = false;
    ITransferOpr* opr = new UartTransferOpr;
    ret = registerOpr(opr);
    return ret;
}
bool DeviceOprManager::managerOprUninit()
{
    bool ret = false;
    ITransferOpr* tmpOpr;
    foreach (tmpOpr, oprs) {
       ret = tmpOpr->transferUninit();
       if(!ret){
           break;
       }
    }

    return ret;
}
ITransferOpr* DeviceOprManager::getTransferOprByName(const QString& name)
{
    ITransferOpr*ret  = NULL;
    ITransferOpr* tmpOpr;
    foreach (tmpOpr, oprs) {
        if(tmpOpr->getOprName() == name)
        {
            ret = tmpOpr;
            break;
        }
    }
    return ret;

}
bool DeviceOprManager::registerOpr(ITransferOpr* opr)
{
    bool ret = true;
    ITransferOpr* tmpOpr;
    foreach (tmpOpr, oprs) {
        if(opr->getOprName() == tmpOpr->getOprName())
        {
            ret = false;
            break;
        }
    }

    if(ret){
        oprs.append(opr);
    }
    return ret;
}

DeviceOprManager::~DeviceOprManager()
{
    ITransferOpr* tmpOpr;
    foreach (tmpOpr, oprs) {
        delete tmpOpr;
    }
}
