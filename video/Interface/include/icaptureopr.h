#ifndef ICAPTUREOPR_H
#define ICAPTUREOPR_H

#include <QObject>
#include "cameraparameterdef.h"

#ifdef _WIN32
#include <Windows.h>
#endif
class   VideoCapture;

class ICaptureOpr : public QObject
{
    Q_OBJECT
public:
    friend class VideoCapture;
    explicit ICaptureOpr(const QString& name,QObject *parent = 0):QObject(parent){
        bSupportSaveCfg = false;
        this->reverseRGB = true;

        //memset(&tCapbility,0,sizeof(T_CameraCapbility));
        this->setObjectName(name);
    }
    virtual ~ICaptureOpr(){}

    virtual bool isSupport() = 0;
    virtual bool openCapture(void) = 0;
    virtual bool isOpened() = 0;
    virtual void closeCapture() = 0;
    virtual bool setCameraArgs(Camera::CameraArgs argId,const QVariant& variant) = 0;
    virtual bool getCameraArgs(Camera::CameraArgs argId,QVariant& value) = 0;
    virtual void trigger2() = 0;
    virtual bool grabFrame() = 0;
    virtual void setCameraMode(Camera::CameraMode mode){Q_UNUSED(mode);}
    virtual void getCameraCapbility(PT_CameraCapbility pCapbility)=0;
    virtual bool snapPic(const QString& filePath){Q_UNUSED(filePath); return false;}


#ifdef _WIN32
    virtual void setVideoRender(void* render){Q_UNUSED(render);}
    virtual void dealAppEvent(int){}
    virtual void setWinIDAndMsgId(HWND &hwnd,UINT& nMsg){Q_UNUSED(hwnd);Q_UNUSED(nMsg);}
#endif



signals:
    void imageComming();

public slots:
#ifdef _WIN32

#endif
protected:
    virtual void chanageReslution(){return;}


protected:
    int                     iWidth;
    int                     iHeight;
    bool                    bSupportSaveCfg;
    QByteArray              picData;
    T_CameraCapbility       tCapbility;
    bool                    reverseRGB;
};



#endif // ICaptureOpr_H
