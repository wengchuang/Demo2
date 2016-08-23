#ifndef IHANDLER_H
#define IHANDLER_H
#include <QString>
#include <QObject>

class IHandler{

public:
    enum IHandlerType{
        HANDLE_MIN,
        HANDLE_ALGORET,
        HANDLE_UI,
        HANDLE_MAX
    };
    IHandler(QString name = "NULL", const IHandlerType& type = HANDLE_ALGORET,const int& id = 0):name(name),handlerType(type),handlerId(id){}
    virtual bool handlerInit() = 0;
    virtual bool handlerUninit() = 0;
    virtual bool hasUIItem(){return false;}
    virtual bool handlerExec(void* buf,const int& length) = 0;
    IHandlerType getHandleType() {return handlerType;}
    QString      getName(){return name;}
    int          handleId() {return handlerId;}
protected:
     const IHandlerType  handlerType;
     const int           handlerId;
     const QString       name;

};

#endif // IHANDLER_H
