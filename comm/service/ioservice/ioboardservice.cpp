#include "ioboardservice.h"
#include "deviceoprmanager.h"
#include "mudbusdatadef.h"
#include "mudbusmaster.h"
#include "appconfig.h"

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
    connect(this,SIGNAL(startOprTimer(int)),&timer,SLOT(start(int)));
    connect(this,SIGNAL(stopOprTimer()),&timer,SLOT(stop()));
    connect(&timer,SIGNAL(timeout()),this,SLOT(dealTimerOut()));
}
void IOBoardService::dealTimerOut()
{
    emit stopOprTimer();
    sendWriteWindCmd(0);

}
void IOBoardService::setTransferOpr(ITransferOpr* opr)
{
    this->transferOpr = opr;
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

    ret =  MudbusMaster::send15Code(transferOpr,usrData);
    if(ret && status !=0){
        emit startOprTimer(Appconfig::outPutTimeOut);
    }
    return ret;


}
