#ifndef MUDBUSDATADEF_H
#define MUDBUSDATADEF_H


typedef struct _mUsrData{
    unsigned char  slaveId;
    unsigned char  funCode;
    unsigned short startAddr;
    unsigned short count;//线圈数量
    unsigned char  transCnt;//要传送的字节数
    unsigned char* data;    //传送的数据
}T_UsrData,*PT_UsrData;



#endif // MUDBUSDATADEF_H
