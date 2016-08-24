#ifndef DEMOHANDLER2_H
#define DEMOHANDLER2_H
#include "ioboardservice.h"
#include "IHandler.h"
#include "appconfig.h"

class DemoHandler2 : public IHandler
{
public:
    bool handlerInit();
    bool handlerUninit();

    bool handlerExec(void* buf,const int& length);
    bool hasUIItem(){return bUIData;}
    char matchOutPut(int& width,int& redX,int& blackCnt);
    DemoHandler2(QString name);
    ~DemoHandler2(){if(ioService){delete ioService; ioService = NULL;}}

private:
    IOBoardService* ioService;
    bool      bUIData;

};

#endif // DEMOHANDLER2_H
