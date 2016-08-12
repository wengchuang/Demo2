#include "datamanager.h"

DataManager* DataManager::instance = NULL;

DataManager* DataManager::getInstance()
{
    if(!instance){
        instance = new DataManager;
    }
    return instance;
}

DataManager::DataManager(QObject *parent) :
    QObject(parent)
{
    items = new DataItem[MAXDATACNT];
    int i = 0;
    for(i=0 ; i< MAXDATACNT; i++){
        preMatList.append(&items[i]);
    }
    isInit = false;
}
void DataManager::initItems(const BITMAPINFOHEADER &infoHeader, const BITMAPFILEHEADER &fileHeader)
{
    const char* tmp;
    for(int i = 0; i<MAXDATACNT; i++){
        items[i].data.resize(fileHeader.bfSize);
        items[i].orgData.resize(fileHeader.bfSize);
        tmp = (char*)&fileHeader;
        memcpy(items[i].data.data(),tmp,sizeof(BITMAPFILEHEADER));
        memcpy(items[i].orgData.data(),tmp,sizeof(BITMAPFILEHEADER));
        tmp = (char*)&infoHeader;
        memcpy(items[i].data.data()+sizeof(BITMAPFILEHEADER),tmp,sizeof(BITMAPINFOHEADER));
        memcpy(items[i].orgData.data()+sizeof(BITMAPFILEHEADER),tmp,sizeof(BITMAPINFOHEADER));

        items[i].mat = Mat(infoHeader.biHeight,infoHeader.biWidth,CV_8UC3,(void*)(items[i].data.data()+fileHeader.bfOffBits));
        items[i].orgMat = Mat(infoHeader.biHeight,infoHeader.biWidth,CV_8UC3,(void*)(items[i].orgData.data()+fileHeader.bfOffBits));
        items[i].offset = fileHeader.bfOffBits;
        items[i].image = QImage(items[i].mat.data, items[i].mat.cols, items[i].mat.rows, items[i].mat.step, QImage::Format_RGB888);
    }
    isInit = true;
}
DataManager::~DataManager()
{
    this->preMatList.clear();
    this->mattedList.clear();
    this->imageList.clear();
    delete []items;
    INFO_DEBUG("~DataManager ...");
}
