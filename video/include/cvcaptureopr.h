#ifndef CVCAPTUREOPR_H
#define CVCAPTUREOPR_H

#ifdef _WIN32
#include "icaptureopr.h"
#if defined(Q_OS_WIN32)
   #include <highgui.h>
#elif defined(Q_OS_LINUX)
   #include <opencv/highgui.h>
#endif


using namespace cv;

class CVCaptureOpr : public ICaptureOpr
{
public:
    explicit CVCaptureOpr(const QString& name,QObject *parent = 0);
    bool isSupport() ;
    bool openCapture(void) ;
    bool isOpened() ;
    void closeCapture();
    void getCameraCapbility(PT_CameraCapbility pCapbility){Q_UNUSED(pCapbility);}

    bool setCameraArgs(Camera::CameraArgs argId,const QVariant& variant);
    bool getCameraArgs(Camera::CameraArgs argId,QVariant&value);
    virtual ~CVCaptureOpr();
protected:
     void    trigger2();
     bool    grabFrame();
private:
    CvCapture*      pCapture;
    IplImage*       pFrame;
    bool            isGrabFrame;
};
#endif
#endif // CVCAPTURE_H
