#include <mudbusmaster.h>
#include <crcopr.h>
#include <QDebug>

unsigned char MudbusMaster::sendBuf[SENDBUFSIZE];
unsigned char MudbusMaster::recvBuf[RECVBUFSIZE];


bool MudbusMaster::send15Code(ITransferOpr* opr,const T_UsrData& tUsrData)
{
    bool ret = false;
    const int responLen = 8;
    int readRet = 0;
    int recvLen = 0;
    unsigned short* crcPtr;
    T_FunData* pFdata;
    if(tUsrData.funCode == 0xf){
        if(transCmd(opr,tUsrData)){
            do{
                recvLen += readRet;
                readRet = opr->readData(recvBuf + recvLen,responLen);
            }while(readRet > 0);
            if(recvLen == 8){
                pFdata = (PT_FunData) pFdata;
                if((pFdata->count == tUsrData.count) &&
                        (pFdata->funCode == tUsrData.funCode) &&
                        (pFdata->slaveId == tUsrData.slaveId) &&
                        (pFdata->startAddr == tUsrData.startAddr)){
                        crcPtr = (unsigned short*)&(recvBuf[6]);
                        if(*crcPtr == Util::CrcOpr::checkCRC(recvBuf,6)){
                            ret =  true;
                        }else{

                            qDebug("check crc error!\n");
                        }
                }
            }
        }
    }
    return ret;
}

bool MudbusMaster:: transCmd(ITransferOpr* opr,const T_UsrData& tUsrData)
{
    unsigned short crc = 0;
    int transLen = 0;
    int transRet = 0;
    T_FunData funData;
    funData.count = be16tole16(tUsrData.count);
    funData.startAddr = be16tole16(tUsrData.startAddr);
    funData.funCode = tUsrData.funCode;
    funData.slaveId = tUsrData.slaveId;
    memset(sendBuf,0,sizeof(sendBuf));
    memcpy(sendBuf,&funData,sizeof(funData));

    if(tUsrData.transCnt){
        memcpy(sendBuf+sizeof(funData),&(tUsrData.transCnt),sizeof(tUsrData.transCnt));
        memcpy(sendBuf+sizeof(funData)+sizeof(tUsrData.transCnt),tUsrData.data,tUsrData.transCnt);
        crc = Util::CrcOpr::checkCRC(sendBuf,sizeof(funData)+sizeof(tUsrData.transCnt)+tUsrData.transCnt);
        memcpy(sendBuf+sizeof(funData)+sizeof(tUsrData.transCnt)+tUsrData.transCnt,&crc,sizeof(crc));
        transLen = sizeof(funData)+sizeof(tUsrData.transCnt)+tUsrData.transCnt+sizeof(crc);

    }else{
        crc=Util::CrcOpr::checkCRC(sendBuf,sizeof(funData));
        memcpy(sendBuf+sizeof(funData),&crc,sizeof(crc));
        transLen = sizeof(funData)+sizeof(crc);
    }
    transRet = opr->sendData(sendBuf,transLen);
    memset(recvBuf,0,sizeof(recvBuf));
    return (transRet == transLen);

}
