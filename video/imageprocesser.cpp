#include "imageprocesser.h"
#include "datamanager.h"
#include "debugredirect.h"
#include "handle/handlermanager.h"
#include "cameraparameterdef.h"
#include "fileopr.h"
#include <QDate>
#include<highgui.h>



#define  RECORDFILEPATH  "./RecordFiles"

ImageProcesser::ImageProcesser(QObject *parent) :
    QObject(parent),recordFilePath(RECORDFILEPATH)
{
      isSnap = false;
      isBusy = true;
      algoItem = NULL;
      isAlgoInit = false;
      picIndex = 0;
      FileOpr::fileOprMkdir(recordFilePath);
}
void ImageProcesser::installAlgo(IAlgorithm* algo)
{
    if(algo){
        this->algoItem = algo;
    }
}

void ImageProcesser::saveSanptoFile(DataItem* item)
{
    static int index = 0;
    if(item->reverseRGB){
        cvtColor(item->mat,item->mat,CV_BGR2RGB);
    }
    QString fileName = filePath + QString("/") + "processersnap" + QString::number(index++) + ".jpg";
    qDebug()<<fileName;
    item->image.save(fileName);
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
    QString dirName = QString(RECORDFILEPATH) + QString("/") + QDate::currentDate().toString("yyyy_MM_dd");
    FileOpr::fileOprMkdir(dirName);
    QString fileName = dirName + "/" + "record" + QString::number(picIndex++) +".jpg";
    if(item->reverseRGB){
        cvtColor(item->orgMat,item->orgMat,CV_BGR2RGB);
    }
    imwrite(fileName.toStdString(),item->orgMat);
}

void ImageProcesser::processImage()
{
    DataItem* item;
    HandleMsg msg;
    msg.type = IHandler::HANDLE_ALGORET;
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
                cvtColor(item->mat,item->orgMat,CV_BGR2RGB);
                //memcpy(item->orgData.data()+item->offset,item->data.data()+item->offset,item->data.size()-item->offset);
                if(isAlgoInit){
                     algoItem->algoExec(item->mat);
                }
                if(item->reverseRGB){
                    cvtColor(item->mat,item->mat,CV_BGR2RGB);
                }
                if(item->videoMode == Camera::MODE_EXTERN){
                    recordResault(item);
                }


                HandlerManager::getInstance()->constructHandleMsg(msg,algoItem);

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
