#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <QObject>
#include <QThread>
#include "icaptureopr.h"
#include "videorender.h"
#include "imageprocesser.h"
#include "datamanager.h"

class VideoCapture : public QObject
{
    Q_OBJECT
public:
    explicit VideoCapture(QObject *parent = 0);
    explicit VideoCapture(VideoRender*render,ImageProcesser* processer, QObject *parent = 0);
    void  installVideoRender(VideoRender*render);
    void  installImageProcesser(ImageProcesser* processer);
    void  installCaptureOpr(ICaptureOpr* opr);
    void setCameraMode(Camera::CameraMode mode);
    bool  isSupportSaveCfg(){if(this->capOpr){return this->capOpr->bSupportSaveCfg;}}
    virtual ~VideoCapture();

private slots:
    void            putImage();
protected:
    VideoRender*            videoRender;
    ImageProcesser*         processer;
    BITMAPINFOHEADER        mheader;
    QByteArray              picHeader;
signals:
    void capOneFrame();
public slots:
#ifdef _WIN32
    void dealRenderAppEvent(int);
#endif
    void trigger();
private:
    void initBase();
private:
    QThread                 thr;
    bool                    bStop;
    Camera::CameraMode      curMode;
public:
    ICaptureOpr*            capOpr;


};

#endif // VIDEOCAPTURE_H
