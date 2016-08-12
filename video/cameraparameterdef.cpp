#include "cameraparameterdef.h"
#include <QDebug>

void clearCameraCability(T_CameraCapbility& capbility)
{
    while(capbility.fmtsList.count()){
        while(capbility.fmtsList.at(0)->ImageSizeList.count()){
           delete capbility.fmtsList.at(0)->ImageSizeList.takeFirst();
        }
        delete capbility.fmtsList.takeFirst();

    }
}

void printCameraCapbilityInfo(const T_CameraCapbility& capbility)
{
    Q_UNUSED(capbility);
    qDebug()<<">>>>>>>>>>>>>>>>>>>触发模式<<<<<<<<<<<<<<<<<<<<<<<<<<";


    qDebug()<<">>>>>>>>>>>>>>>>>>>预设分辨率<<<<<<<<<<<<<<<<<<<<<<<<";

    qDebug(">>>>>>>>>>>>>>>>>>>当前辨率<<<<<<<<<<<<<<<<<<<<<<<<");


    qDebug(">>>>>>>>>>>>>>>>>>>>输出图像格式<<<<<<<<<<<<<<<<<<<<<<<<");


    qDebug(">>>>>>>>>>>>>>>>>>>>传输分包长度<<<<<<<<<<<<<<<<<<<<<<<<");

    qDebug(">>>>>>>>>>>>>>>>>>>>曝光的范围值<<<<<<<<<<<<<<<<<<<<<<<<");


    qDebug(">>>>>>>>>>>>>>>>>>>>分辨率范围描述<<<<<<<<<<<<<<<<<<<<<<<<");


    qDebug(">>>>>>>>>>>>>>>>>>>>图像数字增益范围<<<<<<<<<<<<<<<<<<<<<<<<");


    qDebug(">>>>>>>>>>>>>>>>>>>>饱和度范围<<<<<<<<<<<<<<<<<<<<<<<<");


    qDebug(">>>>>>>>>>>>>>>>>>>>伽马范围<<<<<<<<<<<<<<<<<<<<<<<<");

    qDebug(">>>>>>>>>>>>>>>>>>>>对比度范围<<<<<<<<<<<<<<<<<<<<<<<<");

    qDebug(">>>>>>>>>>>>>>>>>>>>锐化范围<<<<<<<<<<<<<<<<<<<<<<<<");


}

