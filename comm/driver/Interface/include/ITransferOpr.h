#ifndef ITRANSFEROPR_H
#define ITRANSFEROPR_H
#include <QString>
class ITransferOpr{
public:
    virtual const QString& getOprName() = 0;
    virtual bool  transferInit()=0;
    virtual int   sendData(void* buf, int length)=0;
    virtual int   readData(void* buf, int length)=0;
    virtual bool  transferUninit()=0;
    virtual ~ITransferOpr(){}
};

#endif // ITRANSFEROPR_H
