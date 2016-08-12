#include "uarttransferopr.h"
#include <QDebug>
#if defined(Q_OS_LINUX)
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#endif

UartTransferOpr::UartTransferOpr()
{
#if defined(Q_OS_LINUX)
    fd = -1;
    this->oprName = QString("Linux Uart");
#elif defined(Q_OS_WIN32)
    this->port = NULL;
    this->oprName = QString("Windows Uart");
#endif
}
const QString& UartTransferOpr::getOprName()
{
    return oprName;
}

bool UartTransferOpr::transferInit()
{
    bool ret = false;
#if defined(Q_OS_LINUX)
    this->fd = open("/dev/ttymxc1",O_RDWR);
    if(this->fd < 0){
        return ret;
    }
    set_speed(this->fd,115200);
    if(set_Parity(this->fd, 8, 1, 'n') == 0){
        ret = true;
    }
#elif defined(Q_OS_WIN32)
    if(port)
    {
        delete port;
    }
    port=new Win_QextSerialPort("com3",QextSerialBase::EventDriven);
    ret=this->port->open(QIODevice::ReadWrite);
    if(ret)
    {
        this->port->setBaudRate(BAUD115200);
        this->port->setDataBits(DATA_8);
        this->port->setFlowControl(FLOW_OFF);
        this->port->setParity(PAR_NONE);
        this->port->setStopBits(STOP_1);
        this->port->setTimeout(20);
    }else{
       qDebug("Init port error!");
    }
#endif
    return ret;
}
int  UartTransferOpr::sendData(void* buf, int length)
{
    int ret = -1;
    #if defined(Q_OS_LINUX)
    if(this->fd > 0){
        ret = write(this->fd,buf,length);
    }

    #elif defined(Q_OS_WIN32)
     if(port){
         ret = port->write((const char*)buf,length);
     }
    #endif
    return ret;

}
int  UartTransferOpr::readData(void* buf, int length)
{
    int ret = -1;
    #if defined(Q_OS_LINUX)

    fd_set fds;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 2000;
    FD_ZERO(&fds);
    FD_SET(this->fd,&fds);

    ret = select(fd+1,&fds,NULL,NULL,&timeout);
    if(ret > 0){
        ret = read(fd,buf,length);
    }

    #elif defined(Q_OS_WIN32)
    if(port){
     ret = port->read((char*)buf,length);
    }
    #endif
    return ret;
}
bool UartTransferOpr::transferUninit()
{
    bool ret = false;
 #if defined(Q_OS_LINUX)
    if(this->fd > 0){
        close(this->fd);
        this->fd = -1;
    }
    ret = true;
#elif defined(Q_OS_WIN32)
    if(port){
       delete port;
       port = NULL;
    }
#endif
    return ret;
}

