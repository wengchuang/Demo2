#include "imageprocesser.h"
#include "datamanager.h"
#include "debugredirect.h"

ImageProcesser::ImageProcesser(QObject *parent) :
    QObject(parent)
{
      isSnap = false;
      isBusy = false;
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
    isBusy = true;
    DataItem* item;

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
            DataManager::getInstance()->putMattedDataItem(item);

            emit processImageOver();
        }
    //}while(item);
    isBusy = false;
}

ImageProcesser::~ImageProcesser()
{
    while(isBusy);
    qDebug()<<"111111111111111";
    if(thr.isRunning()){
        thr.quit();
    }
    thr.wait();
    INFO_DEBUG("~ImageProcesser...");
}
