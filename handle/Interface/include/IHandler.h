#ifndef IHANDLER_H
#define IHANDLER_H



class IHandler{
public:
    enum IHandlerType{
        HANDLE_MIN,
        HANDLE_BRFOREPROCESSIMAGE,
        HANDLE_AFTERPROCESSIMAGE,
        HANDLE_MAX
    };
    IHandler( const IHandlerType& type,const int& id):handlerType(type),handlerId(id){}
    virtual bool handlerInit() = 0;
    virtual bool handlerUninit() = 0;
    virtual bool handlerExec(void* buf,const int& length) = 0;
    IHandlerType getHandleType() {return handlerType;}
    int          handleId() {return handlerId;}
protected:
     const IHandlerType  handlerType;
     const int           handlerId;

};

#endif // IHANDLER_H
