#include "handlermanager.h"
#include "demohandler.h"
#include <QDebug>

HandlerManager* HandlerManager::instance = NULL;

HandlerManager* HandlerManager::getInstance()
{
    if(!instance){
        instance = new HandlerManager;
    }
    return instance;
}


HandlerManager::HandlerManager(QObject *parent) :
    QThread(parent)
{
    bRun = true;
    unInit = false;
    mutex.lock();
    this->start();
}
bool HandlerManager::isStopRun(){
    return !bRun;
}
bool  HandlerManager::handlerManagerInit()
{
    items = new HandleMsg[MAXHANLEMSGCNT];
    for(int i =0; i< MAXHANLEMSGCNT; i++){
        items[i].context = new char[MAXCONTEXLEN];
        emptyItemQueue.enqueue(&items[i]);
    }
    return constructHandlers();
}
bool HandlerManager::constructHandlers()
{
    IHandler* tmpHandler = new DemoHandler;
    registerHandler(tmpHandler);
    return true;
}
bool HandlerManager::registerHandler( IHandler* handler)
{
    bool ret = false;
    QList<IHandler*> tmpList;
    if((handler->getHandleType() > IHandler::HANDLE_MIN) && (handler->getHandleType() < IHandler::HANDLE_MAX)){
        tmpList=handlerMap.value(handler->getHandleType());
        tmpList.append(handler);
        if(tmpList.count() == 1){
            handlerMap.insert(handler->getHandleType(),tmpList);
        }
        ret = handler->handlerInit();
    }
    return ret;
}
bool  HandlerManager::constructHandleMsg(const HandleMsg& msg)
{
    bool ret = false;
    HandleMsg* item;
    qDebug()<<"last items:"<<itemQueue.count();

    if(emptyItemQueue.count() && msg.contextLen < MAXCONTEXLEN){
       item = emptyItemQueue.dequeue();
       item->type = msg.type;
       item->handlerId = msg.handlerId;
       item->contextLen = msg.contextLen;
       memcpy(item->context,msg.context,msg.contextLen);
       itemQueue.enqueue(item);
       mutex.unlock();
       waitCon.wakeAll();
       ret =true;
    }
    return ret;
}
bool  HandlerManager::handlerManagerUninit()
{
    unInit = true;
    mutex.unlock();
    waitCon.wakeAll();
    return true;
}
void  HandlerManager::run()
{
    HandleMsg* item;
    QList<IHandler*> handlers;
    IHandler* handler;
    while(!unInit){
      waitCon.wait(&mutex);
      while(!unInit){
          while(itemQueue.count()){
              item = itemQueue.dequeue();
              handlers = handlerMap.value(item->type);
              foreach (handler, handlers) {
                  if(handler->handleId() == item->handlerId){
                      handler->handlerExec(item->context,item->contextLen);
                  }
              }
              emptyItemQueue.enqueue(item);
          }

      }
    }

qDebug("quit 1111111111111111111111");
#if 1
    QList<QList<IHandler*>> handlerlist = handlerMap.values();
    foreach (handlers, handlerlist) {
        while(handlers.count()){
            handler = handlers.takeFirst();
            handler->handlerUninit();
            delete handler;
            handler = NULL;
        }

    }
 #endif
    qDebug("quit 22222222222222222222222");
    delete [] items;
    qDebug("quit 4444444444444444444444");
    bRun = false;
}
