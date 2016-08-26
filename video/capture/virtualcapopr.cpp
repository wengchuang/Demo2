#include "virtualcapopr.h"
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QDebug>
#include "fileopr.h"

VirtualCapOpr::VirtualCapOpr(const QString& name,QObject *parent):ICaptureOpr(name,parent)
{
    bOpened = false;
}
bool VirtualCapOpr::isSupport()
{
    bool ret = false;
    QDir dir(loadPath);
    dir.setFilter(QDir::Files);
    QStringList filters;
    filters <<"*.jpg"<<"*.png"<<"*.bmp";
    QFileInfoList fileList = dir.entryInfoList();
    if(fileList.count()){
        ret = true;
    }
    return ret;
}
bool VirtualCapOpr::openCapture(void)
{
    bool ret = false;

    if(isSupport()){
        clearCameraCability(tCapbility);


        QDir dir(loadPath);
        dir.setFilter(QDir::Files);
        QStringList filters;
        filters <<"*.jpg"<<"*.png"<<"*.bmp";
        QFileInfoList fileList = dir.entryInfoList();

        fileImgDesc.files.clear();
        fileImgDesc.imageSizes.clear();
        foreach (QFileInfo info, fileList) {
           fileImgDesc.files << info.absoluteFilePath();
        }


        T_CameraMediaType* fmt = new T_CameraMediaType;
        fmt->iIndex            = 0;
        fmt->iMediaType        = CAMERA_MEDIA_TYPE_RGB8;
        memcpy(fmt->acDescription,"RGB",sizeof("RGB"));
        T_CameraImageResolution* res ;
        QImage image(fileImgDesc.files.at(0));
        res = new T_CameraImageResolution;
        res->iIndex = 0;
        res->iWidth = image.width();
        res->iHeight = image.height();
        char descBuf[50];
        memset(descBuf,0,sizeof(descBuf));
        sprintf(descBuf,"%d * %d",image.width(),image.height());

        memcpy(res->acDescription,descBuf,strlen(descBuf));
        fmt->ImageSizeList.append(res);

        tCapbility.fmtsList.append(fmt);
        tCapbility.curFmt = tCapbility.fmtsList.at(0);

        this->tCapbility.sColorTemp.ibUsed       = 0;


        tCapbility.curResoultion = tCapbility.fmtsList.at(0)->ImageSizeList.at(0);

        tCapbility.sColorTemp.ibUsed  = 0;
        tCapbility.sSaturationRange.ibUsed = 0;
        tCapbility.sGammaRange.ibUsed = 0;
        tCapbility.sShadesRange.ibUsed = 0;
        tCapbility.sBrightnessRange.ibUsed = 0;
        tCapbility.sContrastRange.ibUsed = 0;


        this->iHeight = res->iHeight;
        this->iWidth  = res->iWidth;
        bOpened = true;
        curIndex = 0;

        ret = true;
    }
    return ret;
}
bool VirtualCapOpr::isOpened()
{
    return bOpened;
}
void VirtualCapOpr::closeCapture()
{
    if(bOpened){
        clearCameraCability(tCapbility);
        bOpened = false;
    }
}
void VirtualCapOpr::getCameraCapbility(PT_CameraCapbility pCapbility)
{
    memcpy(pCapbility,&tCapbility,sizeof(T_CameraCapbility));

}


bool VirtualCapOpr::setCameraArgs(Camera::CameraArgs argId,const QVariant& variant)
{
        return true;
}
bool VirtualCapOpr::getCameraArgs(Camera::CameraArgs argId,QVariant&value)
{
        return true;
}
void VirtualCapOpr::setLoadPath(QString path)
{
    this->loadPath = path;
}
void VirtualCapOpr::chanageReslution()
{
    if(fileImgDesc.imageSizes.count() == fileImgDesc.files.count()){

        this->iWidth = fileImgDesc.imageSizes.at(curIndex).iWidth;
        this->iHeight = fileImgDesc.imageSizes.at(curIndex).iHeight;

    }else{
        if(FileOpr::isFileExists(fileImgDesc.files.at(curIndex))){
            ImgSize size;
            QImage image(fileImgDesc.files.at(curIndex));
            size.iWidth = image.width();
            size.iHeight = image.height();
            fileImgDesc.imageSizes.append(size);
            this->iHeight = size.iHeight;
            this->iWidth =  size.iWidth;
        }
    }

}
bool VirtualCapOpr::grabFrame()
{
    bool ret = false;
    if(FileOpr::isFileExists(fileImgDesc.files.at(curIndex))){
        QImage image(fileImgDesc.files.at(curIndex));
        QImage img = image.convertToFormat(QImage::Format_RGB888);

        memcpy(this->picData.data(),img.bits(),this->picData.size());
        this->reverseRGB = false;
        ret = true;
    }
    curIndex++;
    if(curIndex == fileImgDesc.files.count()){
        curIndex = 0;
    }
    return ret;
}
void VirtualCapOpr::trigger2()
{
    if(bOpened  && (m_mode == Camera::MODE_AUTO))
        emit imageComming();

}
VirtualCapOpr::~VirtualCapOpr()
{
    qDebug()<<"~VirtualCapOpr()";
    this->closeCapture();

}
