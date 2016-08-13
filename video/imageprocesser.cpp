#include "imageprocesser.h"
#include "datamanager.h"
#include "debugredirect.h"
#include "handle/handlermanager.h"

ImageProcesser::ImageProcesser(QObject *parent) :
    QObject(parent)
{
      isSnap = false;
      isBusy = true;
      algoItem = NULL;
      isAlgoInit = false;
}
void ImageProcesser::installAlgo(IAlgorithm* algo)
{
    if(algo){
        this->algoItem = algo;
    }
}

void ImageProcesser::saveSanptoFile(DataItem* item)
{
    if(item->reverseRGB){
        cvtColor(item->mat,item->mat,CV_BGR2RGB);
    }
    item->image.save("123.jpg");
    item->reverseRGB = false;
    isSnap = false;
}


bool ImageProcesser::algoExit()
{
    if(algoItem){
        if(algoItem->_algoExit()){
            return true;
        }
    }
    return false;
}
bool ImageProcesser::algoInit()
{
    isAlgoInit = false;
    if(algoItem){
        if(algoItem->_algoInit()){
            isAlgoInit = true;
        }
    }
    return isAlgoInit;
}
void ImageProcesser::recordResault(DataItem* item)
{
    Q_UNUSED(item);

}

void ImageProcesser::processImage()
{
    DataItem* item;
    HandleMsg msg;
    msg.type = IHandler::HANDLE_AFTERPROCESSIMAGE;
    msg.handlerId = 0;


    if(isBusy){

        if(this->thread() != &thr){
            this->moveToThread(&thr);
            if(!thr.isRunning()){
                thr.start();
            }
        }
        if(!isAlgoInit){
           if(!algoInit()){
               qDebug()<<"algo init failed !!! return ...";

           }
        }

        //do{
            item = DataManager::getInstance()->getMattedDataItem();
            if(item){
                if(isSnap){
                    saveSanptoFile(item);
                }
                memcpy(item->orgData.data()+item->offset,item->data.data()+item->offset,item->data.size()-item->offset);
                if(isAlgoInit){
                     algoItem->algoExec(item);
                }
                if(item->reverseRGB){
                    cvtColor(item->mat,item->mat,CV_BGR2RGB);
                }
                recordResault(item);

                item->attachData.recordItem.idention = 0xa;
                item->attachData.recordItem.status = 0xa050;

                msg.context =(void*) (&(item->attachData));
                msg.contextLen = sizeof(item->attachData);
                HandlerManager::getInstance()->constructHandleMsg(msg);

                DataManager::getInstance()->putMattedDataItem(item);

                emit processImageOver();
            }
    }else{
        if(thr.isRunning()){
            thr.quit();
        }
    }
}

ImageProcesser::~ImageProcesser()
{
    isBusy = false;
    if(thr.isRunning()){
        thr.quit();
    }
    while(thr.isRunning());
    INFO_DEBUG("~ImageProcesser...");
}
