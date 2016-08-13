#ifndef DEMOHANDLER2_H
#define DEMOHANDLER2_H
#include "IHandler.h"

class DemoHandler2 : public IHandler
{
public:
    bool handlerInit();
    bool handlerUninit();


    bool handlerExec(void* buf,const int& length);
    DemoHandler2(const IHandlerType& type = IHandler::HANDLE_AFTERPROCESSIMAGE,const int& id = 0);
};

#endif // DEMOHANDLER2_H
