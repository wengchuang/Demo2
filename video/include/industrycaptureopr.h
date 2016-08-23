#ifndef INDUSTRYCAPTUREOPR_H
#define INDUSTRYCAPTUREOPR_H


#include "icaptureopr.h"
#if defined(Q_OS_WIN32)
    #include "win/CameraApi.h"
#elif defined(Q_OS_LINUX)
    #include "unix/CameraApi.h"
#endif


class IndustryCaptureOpr : public ICaptureOpr
{
public:
    explicit IndustryCaptureOpr(const QString& name,QObject *parent = 0);
    bool isSupport() ;
    bool openCapture(void) ;
    bool isOpened() ;
    void closeCapture();
    void getCameraCapbility(PT_CameraCapbility pCapbility);
    void setCameraMode(Camera::CameraMode mode);


    bool setCameraArgs(Camera::CameraArgs argId,const QVariant& variant);
    bool getCameraArgs(Camera::CameraArgs argId,QVariant&value);
    bool snapPic(const QString& filePath);
    virtual ~IndustryCaptureOpr();

protected:
    bool    grabFrame();
    void    trigger2();
    void   chanageReslution();
private:
    tSdkCameraDevInfo       tCameraEnumList[4];
    tSdkFrameHead           g_tFrameHead;
    tSdkCameraCapbility     g_tCapability;


    tSdkImageResolution   *curImageSizeDesc;
    unsigned char*          rawData;
    int                     g_hCamera;
    bool                    bSupport;
    bool                    bCloseCapture;
    bool                    ispause;
    bool                    bChangeRes;


};

#endif // INDUSTRYCAPTURE_H
