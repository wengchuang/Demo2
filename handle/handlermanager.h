#ifndef HANDLERMANAGER_H
#define HANDLERMANAGER_H

#include <QThread>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include "IHandler.h"

struct HandleMsg{
    IHandler::IHandlerType type;
    int                    handlerId;
    int                    contextLen;
    void*                  context;
};

#define MAXHANLEMSGCNT       20
#define MAXCONTEXLEN         1024




class HandlerManager : public QThread
{
    Q_OBJECT
public:
    static HandlerManager* getInstance();
    bool  handlerManagerInit();
    bool  registerHandler( IHandler* handler);
    bool  constructHandleMsg(const HandleMsg& msg);
    bool  handlerManagerUninit();
    ~HandlerManager();

protected:
    void run();

signals:
private:
    bool constructHandlers();
public slots:
private:
    explicit HandlerManager(QObject *parent = 0);
    static HandlerManager* instance;
    QMutex              mutex;
    QWaitCondition      waitCon;
    QQueue<HandleMsg*>  itemQueue;
    QQueue<HandleMsg*>  emptyItemQueue;
    QMap<IHandler::IHandlerType, QList<IHandler*>> handlerMap;
    bool  unInit;
    HandleMsg* items;

};

#endif // HANDLERMANAGER_H
