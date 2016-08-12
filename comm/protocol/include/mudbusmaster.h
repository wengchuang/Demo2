#ifndef MUDBUSMASTER_H
#define MUDBUSMASTER_H
#include "ITransferOpr.h"
#include "mudbusdatadef.h"
#include "mudbusdef.h"

class MudbusMaster
{
public:
    MudbusMaster(){}
    static bool send15Code(ITransferOpr* opr,const T_UsrData& tUsrData);
private:
    static bool transCmd(ITransferOpr* opr,const T_UsrData& tUsrData);
private:
    static unsigned char    sendBuf[SENDBUFSIZE];
    static unsigned char    recvBuf[RECVBUFSIZE];



};

#endif // MUDBUSMASTER_H
