#include "toupcaptureopr.h"
#ifdef _WIN32
#include "debugredirect.h"
ToupCaptureOpr::ToupCaptureOpr(const QString& name,QObject *parent) : ICaptureOpr(name,parent)
{
    cameraHandler = NULL;
    hwnd = NULL;
    nMsg = -1;
    //tCapbility.pImageSizeDesc = NULL;
    this->render = NULL;
}

bool ToupCaptureOpr::isSupport()
{
    return Toupcam_Enum(devices);
}
void ToupCaptureOpr::getCameraCapbility(PT_CameraCapbility pCapbility)
{
    memcpy(pCapbility,&tCapbility,sizeof(T_CameraCapbility));
}

void ToupCaptureOpr::installLister()
{
    if((hwnd != NULL) && (nMsg != -1) && cameraHandler && render){
        if (SUCCEEDED(Toupcam_StartPullModeWithWndMsg(cameraHandler, hwnd, nMsg)))
        {
            qDebug("install listener successful!");
        }
    }
}

bool ToupCaptureOpr::openCapture(void)
{
    bool ret = false;
    unsigned int min,max,def;
    int i = 0;
    QString desc;
    if(!cameraHandler){
        cameraHandler = Toupcam_Open(NULL);
        if(cameraHandler){
           Toupcam_put_Option(cameraHandler, TOUPCAM_OPTION_TRIGGER, 1);
           if (SUCCEEDED(Toupcam_get_Size(cameraHandler, (int*)&iWidth, (int*)&iHeight))){

           }

           T_CameraMediaType* fmt = new T_CameraMediaType;
           fmt->iIndex            = 0;
           fmt->iMediaType        = CAMERA_MEDIA_TYPE_RGB8;

           T_CameraImageResolution* res ;

           for(i = 0 ; i<TOUPCAM_MAX;i++){
               if((devices[0].model->res[i].width == 0) ||(devices[0].model->res[i].height == 0)){
                   break;
               }

               res = new T_CameraImageResolution;
               res->iIndex = i;
               res->iWidth = (devices[0].model->res[i].width);
               res->iHeight = (devices[0].model->res[i].height);
               desc.clear();
               desc.sprintf("%d x %d  index %d",res->iWidth,res->iHeight,i);
               memcpy(res->acDescription,desc.toLocal8Bit().data(),sizeof(res->acDescription));
               fmt->ImageSizeList.append(res);

              if((iWidth == devices[0].model->res[i].width) &&(iHeight == devices[0].model->res[i].height)){
                  //tCapbility.curResolutionIndex = i;
                  tCapbility.curResoultion = fmt->ImageSizeList.at(i);
              }

           }
           tCapbility.fmtsList.append(fmt);
           tCapbility.curFmt = fmt;
           //tCapbility.curFmtIndex = 0;



           if(SUCCEEDED(Toupcam_get_ExpTimeRange(cameraHandler,&min,&max,&def))){
               tCapbility.sExposeDesc.dfExpLineTime =  1.0;
               tCapbility.sExposeDesc.dfExposeMin   =  min;
               tCapbility.sExposeDesc.dfExposeMax   =  max;
               tCapbility.sExposeDesc.dfExposeDef   =  def;
               tCapbility.sExposeDesc.dfExposeSaved =  def;

           }
           if(SUCCEEDED(Toupcam_get_ExpoAGainRange(cameraHandler,(unsigned short*)&min,(unsigned short*)&max,(unsigned short*)&def))){
              tCapbility.sExposeDesc.dfGainMin = min;
              tCapbility.sExposeDesc.dfGainMax = max;
              tCapbility.sExposeDesc.dfGainSaved = def;
              tCapbility.sExposeDesc.dfGainDef = def;
           }

           tCapbility.sColorTemp.ibUsed     = 1;
           tCapbility.sColorTemp.iTempMax   = TOUPCAM_TEMP_MAX;
           tCapbility.sColorTemp.iTempMin   = TOUPCAM_TEMP_MIN;
           tCapbility.sColorTemp.iTempDef   = TOUPCAM_TEMP_DEF;
           tCapbility.sColorTemp.iTempSaved = TOUPCAM_TEMP_DEF;

           tCapbility.sColorTemp.iTintMax   = TOUPCAM_TINT_MAX;
           tCapbility.sColorTemp.iTintMin   = TOUPCAM_TINT_MIN;
           tCapbility.sColorTemp.iTintDef   = TOUPCAM_TINT_DEF;
           tCapbility.sColorTemp.iTintSaved = TOUPCAM_TINT_DEF;


           tCapbility.sShadesRange.ibUsed   = 1;
           tCapbility.sShadesRange.iMin     = TOUPCAM_HUE_MIN;
           tCapbility.sShadesRange.iMax     = TOUPCAM_HUE_MAX;
           tCapbility.sShadesRange.iDef     = TOUPCAM_HUE_DEF;
           tCapbility.sShadesRange.iSaved   = TOUPCAM_HUE_DEF;


           tCapbility.sSaturationRange.ibUsed   =   1;
           tCapbility.sSaturationRange.iMax     = TOUPCAM_SATURATION_MAX;
           tCapbility.sSaturationRange.iMin     = TOUPCAM_SATURATION_MIN;
           tCapbility.sSaturationRange.iDef     = TOUPCAM_SATURATION_DEF;
           tCapbility.sSaturationRange.iSaved   = TOUPCAM_SATURATION_DEF;

           tCapbility.sBrightnessRange.ibUsed   = 1;
           tCapbility.sBrightnessRange.iMax     = TOUPCAM_BRIGHTNESS_MAX;
           tCapbility.sBrightnessRange.iMin     = TOUPCAM_BRIGHTNESS_MIN;
           tCapbility.sBrightnessRange.iDef     = TOUPCAM_BRIGHTNESS_DEF;
           tCapbility.sBrightnessRange.iSaved   = TOUPCAM_BRIGHTNESS_DEF;


           tCapbility.sContrastRange.ibUsed     = 1;
           tCapbility.sContrastRange.iMax       = TOUPCAM_CONTRAST_MAX;
           tCapbility.sContrastRange.iMin       = TOUPCAM_CONTRAST_MIN;
           tCapbility.sContrastRange.iDef       = TOUPCAM_CONTRAST_DEF;
           tCapbility.sContrastRange.iSaved     = TOUPCAM_CONTRAST_DEF;

           tCapbility.sGammaRange.ibUsed        = 1;
           tCapbility.sGammaRange.iMax          = TOUPCAM_GAMMA_MAX;
           tCapbility.sGammaRange.iMin          = TOUPCAM_GAMMA_MIN;
           tCapbility.sGammaRange.iDef          = TOUPCAM_GAMMA_DEF;
           tCapbility.sGammaRange.iSaved        = TOUPCAM_GAMMA_DEF;






           installLister();
           ret = true;
        }
    }
    return ret;
}

bool ToupCaptureOpr::isOpened()
{
    bool ret = false;
    if(cameraHandler){
        ret = true;
    }
    return ret;
}
void ToupCaptureOpr::closeCapture()
{
    if(cameraHandler){
        Toupcam_Stop(cameraHandler);
        Toupcam_Close(cameraHandler);
        clearCameraCability(tCapbility);
        cameraHandler = NULL;
    }
}
bool ToupCaptureOpr::setCameraArgs(Camera::CameraArgs argId,const QVariant& variant)
{
    bool ret = false;
    int  nTint ,nTemp;

    switch(argId){
    case Camera::EXPORESUEID:
        ret=SUCCEEDED(Toupcam_put_ExpoTime(cameraHandler,variant.toUInt()));
        break;
    case Camera::GAINID:
        ret=SUCCEEDED(Toupcam_put_ExpoAGain(cameraHandler,(short)variant.toInt()));
        break;
    case Camera::COLORTMPID:
        ret = SUCCEEDED(Toupcam_get_TempTint(cameraHandler, &nTemp,&nTint));
        if(ret){
            nTemp = variant.toInt();
            ret = SUCCEEDED(Toupcam_put_TempTint(cameraHandler, nTemp, nTint));
        }
        break;
    case Camera::TINTID:
        ret = SUCCEEDED(Toupcam_get_TempTint(cameraHandler, &nTemp,&nTint));
        if(ret){
            nTint = variant.toInt();
            ret = SUCCEEDED(Toupcam_put_TempTint(cameraHandler, nTemp, nTint));
        }
        break;
     case Camera::SHADESID:
       ret = SUCCEEDED(Toupcam_put_Hue(cameraHandler, (int)variant.toInt()));
       break;
    case Camera::SATURATIONID:
       ret = SUCCEEDED(Toupcam_put_Saturation(cameraHandler,(int)variant.toInt()));
       break;
    case Camera::BRIGHTNESSID:
       ret = SUCCEEDED(Toupcam_put_Brightness(cameraHandler,(int)variant.toInt()));
       break;
    case Camera::CONTRASTID:
       ret = SUCCEEDED(Toupcam_put_Contrast(cameraHandler,(int)variant.toInt()));
       break;
    case Camera::GAMAID:
       ret = SUCCEEDED(Toupcam_put_Gamma(cameraHandler,(int)variant.toInt()));
       break;
    default:
        break;
    }
    return ret;
}
bool ToupCaptureOpr::getCameraArgs(Camera::CameraArgs argId,QVariant&value)
{
    bool ret = false;
    unsigned Time;
    unsigned short  gain;
    int  nTint ,nTemp;

    switch(argId){
    case Camera::EXPORESUEID:
        ret=SUCCEEDED(Toupcam_get_ExpoTime(cameraHandler,&Time));
        if(ret)
        value=QVariant::fromValue(Time);

        break;
    case Camera::GAINID:
        ret=SUCCEEDED(Toupcam_get_ExpoAGain(cameraHandler,&gain));
        value = QVariant::fromValue(gain);
        break;
    case Camera::COLORTMPID:
        ret = SUCCEEDED(Toupcam_get_TempTint(cameraHandler, &nTemp,&nTint));
        value = QVariant::fromValue(nTemp);
        break;
    case Camera::TINTID:
        ret = SUCCEEDED(Toupcam_get_TempTint(cameraHandler, &nTemp,&nTint));
        value = QVariant::fromValue(nTint);
        break;
    case Camera::SHADESID:
        ret = SUCCEEDED(Toupcam_get_Hue(cameraHandler, &nTemp));
        value = QVariant::fromValue(nTemp);
        break;
    case Camera::SATURATIONID:
        ret = SUCCEEDED(Toupcam_get_Saturation(cameraHandler, &nTemp));
        value = QVariant::fromValue(nTemp);
        break;
    case Camera::BRIGHTNESSID:
        ret = SUCCEEDED(Toupcam_get_Brightness(cameraHandler, &nTemp));
        value = QVariant::fromValue(nTemp);
        break;
    case Camera::CONTRASTID:
        ret = SUCCEEDED(Toupcam_get_Contrast(cameraHandler, &nTemp));
        value = QVariant::fromValue(nTemp);
        break;
    case Camera::GAMAID:
        ret = SUCCEEDED(Toupcam_get_Gamma(cameraHandler, &nTemp));
        value = QVariant::fromValue(nTemp);
        break;
    default:
        break;
    }
    return ret;
}
void ToupCaptureOpr::trigger2()
{
    if(cameraHandler){

        HRESULT hr = Toupcam_Trigger(cameraHandler, 1);
        if (E_INVALIDARG == hr)
        {
            qDebug()<<"E_INVALIDARG == hr";
        }
    }
}

void ToupCaptureOpr::dealAppEvent(int eventId)
{
    if(eventId == TOUPCAM_EVENT_EXPOSURE){
        //qDebug("message->wParam == TOUPCAM_EVENT_EXPOSURE");

    }else if(eventId == TOUPCAM_EVENT_TEMPTINT){
        qDebug("message->wParam == TOUPCAM_EVENT_TEMPTINT");
        //onEventTemptint();

    }else if(eventId == TOUPCAM_EVENT_CHROME){
        qDebug("message->wParam == TOUPCAM_EVENT_CHROME");

    }else if(eventId == TOUPCAM_EVENT_IMAGE){
        //qDebug("eventId == TOUPCAM_EVENT_IMAGE");
        emit imageComming();

    }else if(eventId == TOUPCAM_EVENT_STILLIMAGE){
        qDebug("eventId == TOUPCAM_EVENT_STILLIMAGE");
        //onSnapShot();

    }else if(eventId == TOUPCAM_EVENT_WBGAIN){
        qDebug("message->wParam == TOUPCAM_EVENT_WBGAIN");

    }else if(eventId == TOUPCAM_EVENT_ERROR){
        qDebug("message->wParam == TOUPCAM_EVENT_ERROR");
        //onEventUsbError();
        WARN_DEBUG(QString::fromLocal8Bit("读USB数据出错!").toUtf8().data());
        closeCapture();

    }else if(eventId == TOUPCAM_EVENT_DISCONNECTED){
        WARN_DEBUG(QString::fromLocal8Bit("USB 已拔出，请检查USB连接!").toUtf8().data());
        closeCapture();
    }
}

bool  ToupCaptureOpr::grabFrame()
{
    bool ret = false;
    if(cameraHandler){
        unsigned nWidth = 0, nHeight = 0;
        void* buf ;
        buf = picData.data();

        HRESULT hr = Toupcam_PullImage(cameraHandler, buf, 24, &nWidth, &nHeight);
        if (FAILED(hr)){
            qDebug()<<"FAILED(hr) failed!";
            return ret;

        }
        if ((nWidth != this->iWidth) || (nHeight != this->iHeight)){
            qDebug()<<"(nWidth != m_header.biWidth) || (nHeight != m_header.biHeight)";
            return ret ;
        }
        ret = true;

    }
    return ret;
}
ToupCaptureOpr::~ToupCaptureOpr()
{
    closeCapture();
}
#endif



