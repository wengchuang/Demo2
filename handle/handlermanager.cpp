#include "handlermanager.h"
#include "demohandler.h"
#include "demohandler2.h"
#include "debugredirect.h"
#include <QDebug>
#include <QMutexLocker>

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
    unInit = false;
    mutex.lock();
    this->start();
}

bool  HandlerManager::handlerManagerInit()
{
    items = new HandleMsg[MAXHANLEMSGCNT];
    for(int i =0; i< MAXHANLEMSGCNT; i++){
        items[i].context = new char[MAXCONTEXLEN];
        items[i].action = NONE;
        emptyItemList.append(&items[i]);
    }
    return constructHandlers();
}
bool HandlerManager::constructHandlers()
{
    IHandler* tmpHandler = new DemoHandler("demo",IHandler::HANDLE_UI,1);
    registerHandler(tmpHandler);
    tmpHandler = new DemoHandler2("any");
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
        handlerMap.insert(handler->getHandleType(),tmpList);
        ret = handler->handlerInit();
    }
    return ret;
}
bool  HandlerManager::constructHandleMsg(const HandleMsg& msg ,IAlgorithm* pAlgo)
{
    bool ret = false;
    HandleMsg* item;
    if(itemList.count() == MAXHANLEMSGCNT){
       INFO_DEBUG(QString("itemList.count() =%1").arg(itemList.count()).toLocal8Bit().data());
    }


    if((msg.type == IHandler::HANDLE_ALGORET) && (pAlgo)){

        if((emptyItemList.count())){

            item = emptyItemList.takeFirst();
            item->type = msg.type;
            item->handlerId = msg.handlerId;
            item->name = pAlgo->objectName();
            pAlgo->constructRetMsg(item->context,item->contextLen);
            itemList.append(item);
            mutex.unlock();
            waitCon.wakeAll();
            ret = true;

        }

    }else{
        if((msg.contextLen < MAXCONTEXLEN ) && (emptyItemList.count())){
           item = emptyItemList.takeFirst();
           item->type = msg.type;
           item->handlerId = msg.handlerId;
           item->contextLen = msg.contextLen;
           memcpy(item->context,msg.context,msg.contextLen);
           itemList.append(item);
           mutex.unlock();
           waitCon.wakeAll();
           ret =true;
        }

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
    bool execRet = false;
    int i;
    while(!unInit){
      waitCon.wait(&mutex);

      while(!unInit){
          while(itemList.count()){

              item = itemList.at(0);

              handlers = handlerMap.value(item->type);
              foreach (handler, handlers) {
                  if(handler->getHandleType() == IHandler::HANDLE_ALGORET){
                      if(handler->getName() == item->name || handler->getName() == "any"){
                          execRet = handler->handlerExec(item->context,item->contextLen);
                          if(execRet && handler->hasUIItem()){
                             item->action = WAITFORUI;
                             forUIItemList.append(itemList.takeAt(0));
                             emit hasDataForUI(item);
                          }else{
                             emptyItemList.append(itemList.takeAt(0));
                          }
                      }

                  }else if(handler->handleId() == item->handlerId){

                      handler->handlerExec(item->context,item->contextLen);
                      if(handler->hasUIItem()){
                         item->action = WAITFORUI;
                         forUIItemList.append(itemList.takeAt(0));
                         emit hasDataForUI(item);
                      }else{
                         emptyItemList.append(itemList.takeAt(0));
                      }
                  }
              }


              for (i=0;i<forUIItemList.count();) {
                 item = forUIItemList.at(i);
                 if(item->action == UIDEALDONE){
                     item->action = NONE;
                     forUIItemList.takeAt(i);
                     emptyItemList.append(item);
                  }else{
                     i++;
                 }

              }

          }


      }
    }

    this->quit();
}

HandlerManager::~HandlerManager()
{
    QList<IHandler*> handlers;
    IHandler* handler;
    this->handlerManagerUninit();
    while(HandlerManager::getInstance()->isRunning());
    QList< QList<IHandler*> > handlerlist = handlerMap.values();
    foreach (handlers, handlerlist) {
        while(handlers.count()){
            handler = handlers.takeFirst();
            handler->handlerUninit();
            delete handler;
            handler = NULL;
        }

    }
    delete [] items;

}
