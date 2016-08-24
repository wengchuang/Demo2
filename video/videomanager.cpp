#include "videomanager.h"
#include "datamanager.h"
#include "debugredirect.h"
#include "appconfig.h"


#include "simplealgo.h"
#include "simplerenderopr.h"
#include "scrollrenderopr.h"
#include "cvcaptureopr.h"
#include "fileopr.h"
#include <QDate>


#include "ceramicdetectalgo.h"


#ifdef _WIN32
#include "toupcaptureopr.h"
#endif

#ifdef __linux


#include "v4l2captureopr.h"
#endif
#include "industrycaptureopr.h"
#include "virtualcapopr.h"


VideoManager* VideoManager::instance = NULL;


VideoManager* VideoManager::getInstance()
{
    if(!instance){
        instance = new VideoManager;
    }
    return instance;
}


VideoManager::VideoManager(QObject *parent) :
    QObject(parent)
{
    curCap = new ::VideoCapture;
    curRender = new VideoRender;
    curProcesser = new ImageProcesser;
    libFunOpr.init = NULL;
    libFunOpr.infos = NULL;
    defAlgName = "SimpleAlgo";
    defRenderName = "ScrollRenderOpr";
    tCapDesc.captureName = "NULL";

}
void VideoManager::setCameraMode(Camera::CameraMode mode)
{
    if((mode < Camera::MODE_MAX) && (mode > Camera::MODE_MINI) ){
        this->curCap->setCameraMode(mode);
        Appconfig::getInstance()->setCameraMode(mode);
    }
}

bool VideoManager::saveCameraCfgToFile(const T_SaveItem& tSaveItem,const T_CameraCapbility& tCapbility)
{
    bool ret = false;
    if(curCap->isSupportSaveCfg()){

    }else{
        if(tSaveItem.bSaveExpoGain){
            ret = Appconfig::getInstance()->setExpoGainCfg(this->curCap->capOpr->objectName(),tCapbility);
        }else if(tSaveItem.bSaveTempAndTint){
            ret = Appconfig::getInstance()->setTempColorCfg(this->curCap->capOpr->objectName(),tCapbility);
        }else if(tSaveItem.bSaveAdjColor){
            ret = Appconfig::getInstance()->setAdjColorCfg(this->curCap->capOpr->objectName(),tCapbility);
        }
    }
    return ret;
}
VideoManager::~VideoManager()
{

    delete curCap;

    while(captureOprList.count()){
        delete captureOprList.takeFirst();
    }

    delete curRender;

    while(rendersOprList.count()){
        delete rendersOprList.takeFirst();
    }

    delete curProcesser;
    while(algosList.count()){
        delete algosList.takeFirst();
    }

    delete DataManager::getInstance();
}


bool VideoManager::openCamera()
{
    bool ret = false;
    if(curCap->capOpr){
        if(curCap->capOpr->openCapture()){

            curCap->capOpr->getCameraCapbility(&(tCapDesc.tCapbility));
            tCapDesc.captureName = curCap->capOpr->objectName();
            Appconfig::getInstance()->loadSaveCfg(curCap->capOpr->objectName(),tCapDesc.tCapbility);

            setCameraArgs(Camera::EXPORESUEID,tCapDesc.tCapbility.sExposeDesc.dfExposeSaved*1000.0);
            setCameraArgs(Camera::GAINID,tCapDesc.tCapbility.sExposeDesc.dfGainSaved);
            if(tCapDesc.tCapbility.sColorTemp.ibUsed){
                setCameraArgs(Camera::COLORTMPID,tCapDesc.tCapbility.sColorTemp.iTempSaved);
                setCameraArgs(Camera::TINTID,tCapDesc.tCapbility.sColorTemp.iTintSaved);

            }


            if(tCapDesc.tCapbility.sShadesRange.ibUsed){
                setCameraArgs(Camera::SHADESID,tCapDesc.tCapbility.sColorTemp.iTempSaved);
            }
            if(tCapDesc.tCapbility.sSaturationRange.ibUsed){
                setCameraArgs(Camera::SATURATIONID,tCapDesc.tCapbility.sSaturationRange.iSaved);
            }
            if(tCapDesc.tCapbility.sBrightnessRange.ibUsed){
                setCameraArgs(Camera::BRIGHTNESSID,tCapDesc.tCapbility.sBrightnessRange.iSaved);
            }
            if(tCapDesc.tCapbility.sContrastRange.ibUsed){
                setCameraArgs(Camera::CONTRASTID,tCapDesc.tCapbility.sContrastRange.iSaved);

            }
            if(tCapDesc.tCapbility.sGammaRange.ibUsed){
                setCameraArgs(Camera::GAMAID,tCapDesc.tCapbility.sGammaRange.iSaved);
            }
            emit cameraCapbilityChanaged();

        }
        qDebug()<<"Current video:";
        qDebug()<<curCap->capOpr->objectName()<<">>>>>>>"<<curProcesser->algoItem->objectName()<<">>>>>>>>>>"<<curRender->curOpr->objectName();
        ret = true;
    }
    return ret;
}

void VideoManager::startVideo()
{

    qDebug("videomanager startVideo");
    curCap->trigger();
    qDebug()<<"videomanager startVideo end";

}

QWidget* VideoManager::getRender()
{
    if(curRender->curOpr){
        return curRender->curOpr->getRender();
    }else{
        return NULL;
    }

}
int VideoManager::loadLib()
{
    PT_LibInfo infos;
    QString degStr;

    if(Appconfig::getInstance()->useVideoLib()){

        QStringList fileNames = Appconfig::getInstance()->videoLibsNames();
        foreach (QString name, fileNames) {
            lib.setFileName(name);
            if(lib.load()){
               libFunOpr.init  = (LibInit)lib.resolve("libInit");
               libFunOpr.infos =  (GetLibInfos)lib.resolve("getLibInfos");
               if(libFunOpr.init && libFunOpr.infos){
                   qDebug("load lib successful!");
                   libFunOpr.init();
                   infos = libFunOpr.infos();
                   foreach(ICaptureOpr* opr,infos->capOprsList){
                       if(registerCaptureOpr(opr) < 0){

                           qDebug()<<"register capture:%s failed !"<<opr->objectName();

                       }
                   }

                   foreach(IAlgorithm* algoImp,infos->algosList){
                       if(registerImageProcesserAlgo(algoImp) < 0){

                           qDebug()<<"register capture:%s failed !"<<algoImp->objectName();

                       }
                   }

                   foreach(IRenderOpr* renderOpr,infos->renderOprsList){
                       if(registerRenderOpr(renderOpr) < 0){
                           qDebug()<<"register capture:%s failed !"<<renderOpr->objectName();
                       }
                   }

               }else{
                   qDebug("can't find lib fun!");
               }
            }else{
                qDebug()<<"load lib:"<<name<<"failed!";
            }
        }

    }
    return 0;
}
bool VideoManager::setCameraArgs(Camera::CameraArgs argId,const QVariant& variant)
{
    bool ret = false;
    switch (argId) {
    default:
        if(this->curCap->capOpr){
            if(this->curCap->capOpr->isOpened()){
                ret = this->curCap->capOpr->setCameraArgs(argId,variant);
            }
        }
        break;
    }
    return ret;
}
bool VideoManager::getCameraArgs(Camera::CameraArgs argId,QVariant&value)
{
    bool ret = false;
    switch (argId) {
    default:
        if(this->curCap->capOpr){
            if(this->curCap->capOpr->isOpened()){
                ret = this->curCap->capOpr->getCameraArgs(argId,value);
            }
        }
        break;
    }
    return ret;
}

bool VideoManager::getCaptureCapbility(PT_CameraCapbility pCapbility)
{
    bool ret = false;
    qDebug()<<"tCapDesc.captureName:"<<tCapDesc.captureName;
    if(tCapDesc.captureName != "NULL"){
        memcpy(pCapbility,&tCapDesc.tCapbility,sizeof(T_CameraCapbility));
        ret = true;
    }
    qDebug("VideoManager::getCaptureCapbility");
    return ret;
}
void VideoManager::snapPic()
{
    QString filePath;
    QString dirName;
    Appconfig::getInstance()->getSnapPath(filePath);

    if(curCap->capOpr->isOpened()){
        dirName = filePath + QString("/") + QDate::currentDate().toString("yyyy_MM_dd");
        FileOpr::fileOprMkdir(dirName);
        if(!(curCap->capOpr->snapPic(dirName))){
            curProcesser->snapPic(dirName);
        }
    }
}
int  VideoManager::managerInit()
{
    int ret = -1;

    loadLib();

    constructVideoCaptureOprs();
    constructImageProcesserAlgos();
    constructVideoRenderOprs();

    listSupportCaptureOprs();
    listSupportProcessers();
    listSupportRenderOprs();


    ICaptureOpr* tmpCapOpr;
    foreach (tmpCapOpr, captureOprList) {
       if(tmpCapOpr->isSupport()){
           curCap->installCaptureOpr(tmpCapOpr);
           break;
       }else{
           qDebug()<<tmpCapOpr->objectName()<<": not support this Camera!";
       }
    }


    if(!curCap->capOpr){
        INFO_DEBUG("Not Support this Camera!");
        return -1;
    }
    QString name = Appconfig::getInstance()->getCfgRender();
    if(name.isNull() || name.isEmpty()){
        INFO_DEBUG("Not fount the Config render name,use the default render!");
        name = defRenderName;
    }
    IRenderOpr * renderOpr;
    renderOpr = getRenderOprByName(name);
    if(renderOpr == NULL){
        QString str;
        str.sprintf("Not fount the Config render: %s,use the default render!",name.toUtf8().data());
        INFO_DEBUG(str.toUtf8().data());
        name = defRenderName;
        renderOpr = getRenderOprByName(name);
    }
    Q_ASSERT( renderOpr != NULL);
    curRender->installRenderOpr(renderOpr);


    name = Appconfig::getInstance()->getCfgProcesser();
    if(name.isNull() || name.isEmpty()){
        INFO_DEBUG("Not fount the Config algo name,use the default algo!");
        name = defAlgName;
    }

    IAlgorithm*  algoImp;
    algoImp = getProcesserAlgoByName(name);
    if(algoImp == NULL){
        QString str;
        str.sprintf("Not fount the Config Algo: %s,use the default render!",name.toUtf8().data());
        INFO_DEBUG(str.toUtf8().data());
        name = defAlgName;
        algoImp = getProcesserAlgoByName(name);
    }
    Q_ASSERT( algoImp != NULL);
    curProcesser->installAlgo(algoImp);

    curCap->installImageProcesser(curProcesser);
    curCap->installVideoRender(curRender);
    ret = 0;
    return ret;

}
IAlgorithm* VideoManager::getProcesserAlgoByName(const QString& name)
{
    IAlgorithm* ret = NULL;
    IAlgorithm* imp;
    foreach (imp, algosList) {
        if(imp->objectName() == name){
            ret = imp;
            break;
        }
    }

    return ret;
}
IRenderOpr* VideoManager::getRenderOprByName(const QString& name)
{
    IRenderOpr* ret = NULL;
    IRenderOpr* opr;
    foreach (opr, rendersOprList) {
       if(opr->objectName() == name ){
           ret = opr;
           break;
       }
    }
    return ret;
}

int VideoManager::constructVideoCaptureOprs()
{
    QString degStr;
    ICaptureOpr* captureOpr;

    captureOpr = new IndustryCaptureOpr("IndustryCaptureOpr");
    if(registerCaptureOpr(captureOpr) < 0){
        degStr.sprintf("register capture:%s failed !",captureOpr->objectName().toUtf8().data());
        INFO_DEBUG(degStr.toUtf8().data());
    }


#ifdef __linux
    captureOpr = new V4L2CaptureOpr("V4L2CaptureOpr");
    if(registerCaptureOpr(captureOpr) < 0){
        degStr.sprintf("register capture:%s failed !",captureOpr->objectName().toUtf8().data());
        INFO_DEBUG(degStr.toUtf8().data());
    }

    captureOpr = new VirtualCapOpr("VirtualCapOpr");
    if(registerCaptureOpr(captureOpr) < 0){
        degStr.sprintf("register capture:%s failed !",captureOpr->objectName().toUtf8().data());
        INFO_DEBUG(degStr.toUtf8().data());
    }else{
        QString loadPath;
        if(Appconfig::getInstance()->getVirtualCapoprDir(loadPath));
        VirtualCapOpr* virCapOpr = (VirtualCapOpr*) captureOpr;
        virCapOpr->setLoadPath(loadPath);
    }
#endif

#ifdef _WIN32
    captureOpr = new ToupCaptureOpr("ToupCaptureOpr");
    if(registerCaptureOpr(captureOpr) < 0){
        degStr.sprintf("register capture:%s failed !",captureOpr->objectName().toUtf8().data());
        INFO_DEBUG(degStr.toUtf8().data());
    }

    captureOpr = new VirtualCapOpr("VirtualCapOpr");
    if(registerCaptureOpr(captureOpr) < 0){
        degStr.sprintf("register capture:%s failed !",captureOpr->objectName().toUtf8().data());
        INFO_DEBUG(degStr.toUtf8().data());
    }else{
        QString loadPath;
        if(Appconfig::getInstance()->getVirtualCapoprDir(loadPath));
        VirtualCapOpr* virCapOpr = (VirtualCapOpr*) captureOpr;
        virCapOpr->setLoadPath(loadPath);
    }

    captureOpr = new CVCaptureOpr("CVCaptureOpr");
    if(registerCaptureOpr(captureOpr) < 0){
        qDebug()<<"register capture:%s failed !"<<captureOpr->objectName();

    }

#endif




    return 0;
}
int VideoManager::constructVideoRenderOprs()
{
    QString degStr;
    IRenderOpr* renderOpr = new SimpleRenderOpr("SimpleRenderOpr");
    if(registerRenderOpr(renderOpr) < 0){
       // qDebug()<<simRender->objectName();
        degStr.sprintf("register render:%s failed !",renderOpr->objectName().toUtf8().data());
        INFO_DEBUG(degStr.toUtf8().data());
    }

    renderOpr = new ScrollRenderOpr("ScrollRenderOpr");
    if(registerRenderOpr(renderOpr) < 0){
       // qDebug()<<scrollRender->objectName();
        degStr.sprintf("register render:%s failed !",renderOpr->objectName().toUtf8().data());
        INFO_DEBUG(degStr.toUtf8().data());
    }

    return 0;
}
int VideoManager::constructImageProcesserAlgos()
{
    QString degStr;
    IAlgorithm* algoImpl = new SimpleAlgo("SimpleAlgo");
    if(registerImageProcesserAlgo(algoImpl) < 0){
        degStr.sprintf("register processer: %s failed !",algoImpl->objectName().toUtf8().data());
        INFO_DEBUG(degStr.toUtf8().data());
    }
#ifdef _WIN32
    algoImpl = new CeramicDetectAlgo("CeramicDetectAlgo");
    if(registerImageProcesserAlgo(algoImpl) < 0){
        degStr.sprintf("register processer: %s failed !",algoImpl->objectName().toUtf8().data());
        INFO_DEBUG(degStr.toUtf8().data());
    }
#endif
    return 0;
}




int VideoManager::registerCaptureOpr(ICaptureOpr* captureOpr)
{
    ICaptureOpr* tmpCapOpr;
    foreach (tmpCapOpr, captureOprList) {
       if(tmpCapOpr->objectName() == captureOpr->objectName()){
           return -1;
       }
    }
    captureOprList.append(captureOpr);
    return 0;
}
int VideoManager::registerRenderOpr(IRenderOpr* renderOpr)
{
    int ret = -1;
    IRenderOpr* videoRenderOpr = getRenderOprByName(renderOpr->objectName());
    if(!videoRenderOpr){
        rendersOprList.append(renderOpr);
        ret = 0;
    }
    return ret;
}
int VideoManager::registerImageProcesserAlgo(IAlgorithm* algoImpl)
{
    int ret = -1;
    IAlgorithm* imp = getProcesserAlgoByName(algoImpl->objectName());
    if(!imp){
        algosList.append(algoImpl);
        ret = 0;
    }
    return ret;
}

QStringList VideoManager::listSupportCaptureOprs()
{
    QStringList strList;
    ICaptureOpr* capOpr;
    INFO_DEBUG("Support Captures:");
    foreach (capOpr, captureOprList) {
       strList << capOpr->objectName();
       INFO_DEBUG(("    "+capOpr->objectName()).toUtf8().data());
    }
    return strList;
}
QStringList VideoManager::listSupportProcessers()
{
    QStringList strList;
    IAlgorithm* impl;
    INFO_DEBUG("Support Processers:");
    foreach (impl, algosList) {
       strList << impl->objectName();
       INFO_DEBUG(("    "+impl->objectName()).toUtf8().data());
    }
    return strList;
}
QStringList VideoManager::listSupportRenderOprs()
{
    QStringList strList;
    IRenderOpr* renderOpr;
    INFO_DEBUG("Support Renders:");
    foreach (renderOpr, rendersOprList) {
       strList << renderOpr->objectName();
       INFO_DEBUG(("    "+renderOpr->objectName()).toUtf8().data());
    }
    return strList;
}

