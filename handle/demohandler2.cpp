#include "demohandler2.h"
#include "deviceoprmanager.h"
#include "debugredirect.h"
#include "SimpleAlgoArgs.h"
#include <QDebug>
DemoHandler2::DemoHandler2(QString name):IHandler(name)
{
    ioService = NULL;
    bUIData = false;

}
bool DemoHandler2::handlerInit()
{
    ioService = IOBoardService::getInstance();
#if defined(Q_OS_LINUX)
    ioService->setTransferOpr(DeviceOprManager::getInstance()->getTransferOprByName("Linux Uart"));
#elif defined(Q_OS_WIN32)
    ioService->setTransferOpr(DeviceOprManager::getInstance()->getTransferOprByName("Windows Uart"));
#endif
    return true;
}
bool DemoHandler2::handlerUninit()
{


    return true;
}
char DemoHandler2::matchOutPut(int& width,int& redX,int& blackCnt)
{
    int cnt   = Appconfig::lineCfg.redIndexs.count();
    int delta = width/cnt;
    int redRes = -1;
    char ret;
    if(redX != -1){
        qDebug()<<"redX" << redX ;
        qDebug()<<"max width"<<width;
        qDebug()<<"delta"<<delta;
        for(int i = 1;i < cnt; i++ ){
           if((i*delta < redX) && ((i+1)*delta > redX)){
              redRes = i;
              break;
           }

        }

    }else if(blackCnt != -1){
        qDebug()<< "blackLine cnt :"<<blackCnt;
    }

    if(redRes != -1){
        qDebug()<<"redRes:"<<redRes;
        ret = Appconfig::lineCfg.redIndexs.at(redRes);
    }else if(blackCnt != -1){
        ret = Appconfig::lineCfg.blackmap.value(blackCnt,-1);
        qDebug()<<"ret "<<(int)ret;
    }

    return ret;
}

bool DemoHandler2::handlerExec(void* buf,const int& length)
{

    AlgResData* pAlgData = (AlgResData*)buf;
    int redX = -1;
    int blackCnt = -1;
    qDebug()<<"pAlgData->redLines:"<< pAlgData->redLines;
    if(pAlgData->redLines  == 1){

       if(pAlgData->redLines == 1){
          redX = pAlgData->redRects[0].x + pAlgData->redRects[0].width/2 - pAlgData->roiRect.x;
       }
    }else if(pAlgData->blackLines){
        blackCnt = pAlgData->blackLines;
    }
    char data = matchOutPut(pAlgData->roiRect.width,redX,blackCnt);
    if(data >= 0 && data <= MAXCHANNELCNT ){
        ioService->sendWriteWindCmd(1<<(data -1 ));
        bUIData = true;
    }else{
        data = -1;
        bUIData = false;
    }
    memcpy(buf,&data,sizeof(data));

    return true;
}


