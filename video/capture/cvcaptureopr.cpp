#include "cvcaptureopr.h"
#include "debugredirect.h"
#ifdef _WIN32

CVCaptureOpr::CVCaptureOpr(const QString& name,QObject *parent) : ICaptureOpr(name,parent)
{
    pCapture = NULL;
    isGrabFrame = false;
}

bool CVCaptureOpr::isSupport()
{
    return openCapture();
}
bool CVCaptureOpr::openCapture(void)
{
   bool ret = false;
   if(!pCapture){
       pCapture = cvCreateCameraCapture(-1);
       if(pCapture){
          pFrame = cvQueryFrame(pCapture);
          if(pFrame){
              this->iHeight = pFrame->height;
              this->iWidth  = pFrame->width;
              ret = true;
          }
       }
   }
   return ret;
}
bool CVCaptureOpr::isOpened()
{
    if((this->iHeight != -1) && (this->iWidth != -1)){
        return true;
    }else{
        return false;
    }
}
void CVCaptureOpr::closeCapture()
{

}
void CVCaptureOpr::trigger2()
{
    emit imageComming();
}
bool CVCaptureOpr::grabFrame()
{
    isGrabFrame= true;
    if(pCapture){
        pFrame = cvQueryFrame(pCapture);
        if(pFrame){
            memcpy(this->picData.data(),pFrame->imageData,this->picData.size());
        }
    }
    isGrabFrame = false;
    return true;
}

bool CVCaptureOpr::setCameraArgs(Camera::CameraArgs argId,const QVariant& variant)
{
    Q_UNUSED(argId);
    Q_UNUSED(variant);
    return false;
}
bool CVCaptureOpr::getCameraArgs(Camera::CameraArgs argId,QVariant&value)
{
    Q_UNUSED(argId);
    Q_UNUSED(value);
    return true;
}
CVCaptureOpr::~CVCaptureOpr()
{
    qDebug("~CVCaptureOpr");
    while(isGrabFrame);
    if(pCapture){
        cvReleaseCapture(&pCapture);
    }
}
#endif
