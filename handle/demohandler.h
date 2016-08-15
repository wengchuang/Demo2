#ifndef DEMOHANDLER_H
#define DEMOHANDLER_H
#include "IHandler.h"

class DemoHandler : public IHandler
{
public:
    bool handlerInit(){ return true;}
    bool handlerUninit() {return true;}
    bool handlerExec(void* buf,const int& length);
    DemoHandler(QString name);
};

#endif // DEMOHANDLER_H
