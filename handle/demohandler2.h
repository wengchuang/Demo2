#ifndef DEMOHANDLER2_H
#define DEMOHANDLER2_H
#include "IHandler.h"

class DemoHandler2 : public IHandler
{
public:
    bool handlerInit();
    bool handlerUninit();


    bool handlerExec(void* buf,const int& length);
    DemoHandler2(QString name);
};

#endif // DEMOHANDLER2_H
