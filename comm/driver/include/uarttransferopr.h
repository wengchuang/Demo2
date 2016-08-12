#ifndef UARTTRANSFEROPR_H
#define UARTTRANSFEROPR_H
#include <ITransferOpr.h>
#if defined(Q_OS_LINUX)
#include <uart.h>
#elif defined(Q_OS_WIN32)
#include <win_qextserialport.h>
#endif


class UartTransferOpr : public ITransferOpr
{
public:
    UartTransferOpr();
    const QString& getOprName();

    bool  transferInit();
    int   sendData(void* buf, int length);
    int   readData(void* buf, int length);
    bool  transferUninit();

    ~UartTransferOpr(){transferUninit();}
private:
#if defined(Q_OS_LINUX)
    int fd;
#elif defined(Q_OS_WIN32)
    Win_QextSerialPort          *port;
#endif
    QString      oprName;
};

#endif // UARTTRANSFEROPR_H
