#ifndef IOBOARDSERVICE_H
#define IOBOARDSERVICE_H

#include <QObject>
#include <QTimer>
#include "ITransferOpr.h"

class IOBoardService : public QObject
{
    Q_OBJECT
public:
    static IOBoardService* getInstance();
    void   setTransferOpr(ITransferOpr* opr);
    bool   sendWriteWindCmd(unsigned char status);

private:
    explicit IOBoardService(QObject *parent = 0);

signals:
    void startOprTimer(int);
    void stopOprTimer(void);

private slots:
    void dealTimerOut();
private:
    ITransferOpr* transferOpr;
    static IOBoardService* instance;
    QTimer timer;

};

#endif // IOBOARDSERVICE_H
