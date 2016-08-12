#ifndef TOUPCAPTUREOPR_H
#define TOUPCAPTUREOPR_H


#ifdef _WIN32
#include <Windows.h>

#include "videocapture.h"
#include "videorender.h"
#include "toupcam.h"

class ToupCaptureOpr : public ICaptureOpr
{
public:
    explicit ToupCaptureOpr(const QString& name,QObject *parent = 0);
    bool isSupport() ;
    bool openCapture(void) ;
    bool isOpened() ;
    void closeCapture();
    bool setCameraArgs(Camera::CameraArgs argId,const QVariant& variant);
    bool getCameraArgs(Camera::CameraArgs argId,QVariant& value);
    void dealAppEvent(int);
    void getCameraCapbility(PT_CameraCapbility pCapbility);

    inline void setVideoRender(void* render){this->render = (VideoRender*)render;}
    inline void setWinIDAndMsgId(HWND&hwnd,UINT& nMsg){this->hwnd = hwnd;this->nMsg = nMsg;}
    virtual ~ToupCaptureOpr();
private:
    void  installLister();

protected:
    virtual bool    grabFrame();
    virtual void    trigger2();
private:
    HToupCam            cameraHandler;
    ToupcamInst         devices[TOUPCAM_MAX];
    HWND                hwnd;
    UINT                nMsg;
    VideoRender*        render;


};
#endif

#endif // TOUPCAPTURE_H
