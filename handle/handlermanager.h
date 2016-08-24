#ifndef HANDLERMANAGER_H
#define HANDLERMANAGER_H

#include <QThread>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QWaitCondition>
#include <QList>
#include <QString>
#include "IHandler.h"
#include "ialgorithm.h"

enum MsgAction{
    WAITFORUI,
    UIDEALDONE,
    NONE
};

struct HandleMsg{
    IHandler::IHandlerType type;
    QString                name;
    MsgAction              action;
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
    bool  constructHandleMsg(const HandleMsg& msg,IAlgorithm* pAlgo = NULL);
    bool  handlerManagerUninit();
    ~HandlerManager();
protected:
    void run();

signals:
    void hasDataForUI(HandleMsg* msg);
private:
    bool constructHandlers();
public slots:
private:
    explicit HandlerManager(QObject *parent = 0);
    static HandlerManager* instance;
    QMutex              mutex;
    QWaitCondition      waitCon;
    QList<HandleMsg*>   itemList;
    QList<HandleMsg*>   forUIItemList;
    QList<HandleMsg*>   emptyItemList;
    QMap<IHandler::IHandlerType, QList<IHandler*> > handlerMap;
    bool  unInit;
    HandleMsg* items;

};

#endif // HANDLERMANAGER_H
