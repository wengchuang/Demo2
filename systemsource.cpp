#include "systemsource.h"
#include "videomanager.h"
#include "deviceoprmanager.h"
#include "handle/handlermanager.h"
#include "appconfig.h"

SystemSource* SystemSource::instance = NULL;

SystemSource* SystemSource::getInstance(){
    if(!instance){
        instance = new SystemSource;
    }
    return instance;
}

SystemSource::SystemSource(QObject *parent) :
    QObject(parent)
{
    isInit = false;
}


bool SystemSource::sourceInit()
{
    bool ret = false;
    if(isInit == false){
        if(VideoManager::getInstance()->managerInit() == 0){

            DeviceOprManager::getInstance()->managerOprInit();

            if(HandlerManager::getInstance()->handlerManagerInit()){
                ret = true;
                isInit = true;
            }
        }
    }

    return ret;

}
SystemSource::~SystemSource()
{
    delete VideoManager::getInstance();
    delete DeviceOprManager::getInstance();
    delete HandlerManager::getInstance();
    delete Appconfig::getInstance();
}
