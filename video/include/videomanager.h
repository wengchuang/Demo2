#ifndef VIDEOMANAGER_H
#define VIDEOMANAGER_H

#include <QObject>
#include <QList>
#include <QLibrary>
#include "cameraparameterdef.h"
#include "videocapture.h"
#include "videorender.h"
#include "imageprocesser.h"
#include "icaptureopr.h"
#include "irenderopr.h"
#include "ialgorithm.h"

#include "libdatadef.h"

typedef int (*LibInit)();
typedef const PT_LibInfo (*GetLibInfos)();


typedef struct LibOpr{
    LibInit             init;
    GetLibInfos         infos;
}T_LibOpr,*PT_LibOpr;

typedef struct{
    QString           captureName;
    T_CameraCapbility tCapbility;
}T_CameraCapDesc;

typedef struct{
    bool        bSaveExpoGain;
    bool        bSaveTempAndTint;
    bool        bSaveAdjColor;
}T_SaveItem,*PT_SaveItem;



class VideoManager : public QObject
{
    Q_OBJECT
public:
    static VideoManager* getInstance();

    void  startVideo();
    bool  openCamera();
    bool  saveCameraCfgToFile(const T_SaveItem& tSaveItem,const T_CameraCapbility& tCapbility);
    inline ICaptureOpr* getCaptureOpr(){return curCap->capOpr;}

    ~VideoManager();
    void  snapPic();

    int    managerInit();
    bool   getCaptureCapbility(PT_CameraCapbility pCapbility);
    bool   setCameraArgs(Camera::CameraArgs argId,const QVariant& variant);
    bool   getCameraArgs(Camera::CameraArgs argId,QVariant&value);

    QWidget*    getRender();

    QStringList listSupportCaptureOprs();
    QStringList listSupportProcessers();
    QStringList listSupportRenderOprs();
private:
    explicit VideoManager(QObject *parent = 0);

    IRenderOpr* getRenderOprByName(const QString& name);
    IAlgorithm* getProcesserAlgoByName(const QString& name);

    int         registerCaptureOpr(ICaptureOpr* captureOpr);
    int         registerRenderOpr(IRenderOpr* renderOpr);
    int         registerImageProcesserAlgo(IAlgorithm* );

    int         constructVideoCaptureOprs();
    int         constructVideoRenderOprs();
    int         constructImageProcesserAlgos();
    int         loadLib();

signals:
    void        cameraCapbilityChanaged();

public slots:
private:
    static VideoManager*                instance;
    QList<ICaptureOpr*>                 captureOprList;
    QList<IRenderOpr*>                  rendersOprList;
    QList<IAlgorithm*>                  algosList;

    VideoCapture*                       curCap;
    VideoRender*                        curRender;
    ImageProcesser*                     curProcesser;

    QString                             defAlgName;
    QString                             defRenderName;
    T_CameraCapDesc                     tCapDesc;

    QLibrary                            lib;
    T_LibOpr                            libFunOpr;

};

#endif // VIDEOMANAGER_H
