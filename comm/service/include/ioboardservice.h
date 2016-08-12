#ifndef IOBOARDSERVICE_H
#define IOBOARDSERVICE_H

#include <QObject>
#include "ITransferOpr.h"

class IOBoardService : public QObject
{
    Q_OBJECT
public:
    static IOBoardService* getInstance();
    bool   sendWriteWindCmd(unsigned char status);

private:
    explicit IOBoardService(QObject *parent = 0);

signals:

public slots:
private:
    ITransferOpr* transferOpr;
    static IOBoardService* instance;

};

#endif // IOBOARDSERVICE_H
