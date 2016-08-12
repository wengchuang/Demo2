#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <opencv/cv.h>
#include <QImage>
#include "debugredirect.h"
#include "algargs.h"

#if defined(Q_OS_WIN32)
#include <Windows.h>
#elif defined(Q_OS_LINUX)

#pragma pack(push)
#pragma pack(1)

typedef struct tagBITMAPFILEHEADER { /* bmfh */
    unsigned short bfType;
    unsigned long  bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned long  bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER { /* bmih */
    unsigned long  biSize;
    unsigned long  biWidth;
    unsigned long  biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned long  biCompression;
    unsigned long  biSizeImage;
    unsigned long  biXPelsPerMeter;
    unsigned long  biYPelsPerMeter;
    unsigned long  biClrUsed;
    unsigned long  biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)

#endif
using namespace cv;



#define MAXDATACNT          3


class DataManager : public QObject
{
    Q_OBJECT
public:
    static DataManager* getInstance();
    inline bool   isMemInit(){return isInit;}
    void   initItems(const BITMAPINFOHEADER &infoHeader, const BITMAPFILEHEADER &fileHeader);
    inline DataItem*   getPreMatDataItem(){
           DataItem* ret=NULL;
           if(!preMatList.isEmpty()){
               ret = preMatList.takeFirst();
           }else{
               INFO_DEBUG("getPreMatDataItem empty ...");
           }
           return ret;
    }

    inline void putPreMatDataItem(DataItem* item){
        mattedList.append(item);
        emit readyToProcess();
    }
    inline DataItem* getMattedDataItem(){
        DataItem* item= NULL;
        if(!mattedList.isEmpty()){
            item = mattedList.takeFirst();
        }else{
            //INFO_DEBUG("getMattedDataItem empty ...");
        }
        return item;
    }
    inline void putMattedDataItem(DataItem* item){
        imageList.append(item);
    }
    inline DataItem* getImage(){
        DataItem* ret = NULL;
        if(!imageList.isEmpty()){
            ret = imageList.takeFirst();
            preMatList.append(ret);
        }else{
            //qDebug()<<"getImage empty ... ";
        }
        return ret;
    }
    ~DataManager();

private:
    explicit DataManager(QObject *parent = 0);

signals:
    void readyToProcess();

public slots:
private:
    static  DataManager*        instance;
    QList<DataItem*>            preMatList;
    QList<DataItem*>            mattedList;

    QList<DataItem*>            imageList;
    DataItem*                    items;
    bool                        isInit;

};

#endif // DATAMANAGER_H
