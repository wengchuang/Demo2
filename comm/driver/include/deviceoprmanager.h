#ifndef DEVICEOPRMANAGER_H
#define DEVICEOPRMANAGER_H
#include <QList>
#include <ITransferOpr.h>

class DeviceOprManager
{
public:
    static DeviceOprManager* getInstance();
public:
    bool managerOprInit();
    bool managerOprUninit();
    ITransferOpr* getTransferOprByName(const QString& name);
    ~DeviceOprManager();
private:
    bool    constructTransfers();
    bool    registerOpr(ITransferOpr* opr);

private:
    DeviceOprManager();
    QList<ITransferOpr*> oprs;
    static DeviceOprManager* instance;
};

#endif // DEVICEOPRMANAGER_H
