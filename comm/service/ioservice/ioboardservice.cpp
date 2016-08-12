#include "ioboardservice.h"
#include "deviceoprmanager.h"
#include "mudbusdatadef.h"
#include "mudbusmaster.h"

IOBoardService* IOBoardService::instance = NULL;

IOBoardService* IOBoardService::getInstance()
{
    if(!instance){
        instance = new IOBoardService;
    }
    return instance;
}

IOBoardService::IOBoardService(QObject *parent) :
    QObject(parent)
{
    this->transferOpr = NULL;
}
bool IOBoardService::sendWriteWindCmd(unsigned char status)
{
    bool ret = false;
    if(!transferOpr){
#if defined(Q_OS_LINUX)
        transferOpr = DeviceOprManager::getInstance()->getTransferOprByName("Linux Uart");
#endif
    }
    if(!transferOpr){
        return ret;
    }
    unsigned char data[1]={status};
    T_UsrData usrData;
    usrData.count = 8;
    usrData.data  = data;
    usrData.funCode = 0x0f;
    usrData.slaveId = 0x01;
    usrData.startAddr = 0x0000;
    usrData.transCnt = 1;

    return MudbusMaster::send15Code(transferOpr,usrData);


}
