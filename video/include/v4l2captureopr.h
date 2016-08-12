#ifndef V4L2CAPTUREOPR_H
#define V4L2CAPTUREOPR_H

#ifdef __linux

#include "icaptureopr.h"
#define   MAXDEVNAMELEN         20
#define   MAXDEVNUM             10
#define   NB_BUFFER             4

class V4L2CaptureOpr : public ICaptureOpr
{
public:
    explicit V4L2CaptureOpr(const QString& name,QObject *parent = 0);
    bool isSupport() ;
    bool openCapture(void) ;
    bool isOpened() ;
    void closeCapture();
    void getCameraCapbility(PT_CameraCapbility pCapbility);

    bool setCameraArgs(Camera::CameraArgs argId,const QVariant& variant);
    bool getCameraArgs(Camera::CameraArgs argId,QVariant&value);
    virtual ~V4L2CaptureOpr();
private:
    bool isSupportThisFmt(const int& fmt);

    int getContrl(const int& contrl);
    int resetControl(const int &control);
    int isv4l2Contrl(const int& contrl,struct v4l2_queryctrl *queryctrl);
    int setContrl(const int& contrl,const int& value);



    int enumFrameFormats();
    int enumFrameIntervals(int pixfmt,int width, int height);
    int enumControls();
    int enumFrameSizes(int pixfmt,QList<T_CameraImageResolution*>& resList);
    int stopCapture();

protected:
    virtual void    trigger2();
    virtual bool    grabFrame();
private:
    char    strName[MAXDEVNAMELEN];
    int     iFd;
    bool    bOpened;
    bool    isStart;
    bool    isGrabImage;
    int     iVideoBufCnt;
    bool    bSupportReadIo;
    int 	iVideoBufMaxLen;
    int     iVideoBufCurIndex;
    int     iCurFmtIndex;
    int     iCurResIndex;
    unsigned int iByteUsed;
    static  int supportFmts[3];

    unsigned char *pucVideBuf[NB_BUFFER];

};
#endif
#endif // V4L2CaptureOpr_H
