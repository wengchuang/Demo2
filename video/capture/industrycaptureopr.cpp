#include "industrycaptureopr.h"

#include "datamanager.h"
#include <QFuture>
#include <QtConcurrentRun>

using namespace QtConcurrent;
IndustryCaptureOpr::IndustryCaptureOpr(const QString& name,QObject *parent):ICaptureOpr(name,parent)
{
    bSupport = false;
    g_hCamera = -1;
    rawData = NULL;
    bCloseCapture = false;
    bChangeRes = false;
    ispause = false;
}

bool IndustryCaptureOpr::isSupport()
{
    int   iCameraCounts = 4;
    //sdk初始化  0 English 1中文
    CameraSdkInit(1);

    //枚举设备，并建立设备列表
    CameraEnumerateDevice(tCameraEnumList,&iCameraCounts);
    qDebug()<<"iCameraCounts:"<<iCameraCounts;

    //没有连接设备
    if(iCameraCounts==0){
        bSupport = false;
        return false;
    }
    bSupport = true;
    return true;
}
bool IndustryCaptureOpr::openCapture(void)
{
    bool ret = false;
    int  iStatus;
    tSdkImageResolution     tsdkImagResolution;
    if(bSupport){
        //相机初始化。初始化成功后，才能调用任何其他相机相关的操作接口
        iStatus = CameraInit(&tCameraEnumList[0],-1,-1,&g_hCamera);

        //初始化失败
        if(iStatus!=CAMERA_STATUS_SUCCESS){
            return ret;
        }
        clearCameraCability(tCapbility);

        //获得相机的特性描述结构体。该结构体中包含了相机可设置的各种参数的范围信息。决定了相关函数的参数
        CameraGetCapability(g_hCamera,&g_tCapability);

        this->tCapbility.pTriggerDesc   = (T_CameraTrigger*)g_tCapability.pTriggerDesc;
        this->tCapbility.iTriggerDesc   = g_tCapability.iTriggerDesc;

        this->tCapbility.pTriggerDesc   = (T_CameraTrigger*)g_tCapability.pTriggerDesc;
        this->tCapbility.iTriggerDesc   = g_tCapability.iTriggerDesc;
        T_CameraMediaType* fmt = new T_CameraMediaType;
        fmt->iIndex            = g_tCapability.pMediaTypeDesc->iIndex;
        fmt->iMediaType        = g_tCapability.pMediaTypeDesc->iMediaType;
        memcpy(fmt->acDescription,g_tCapability.pMediaTypeDesc->acDescription,sizeof(fmt->acDescription));
        T_CameraImageResolution* res ;
        for(int i = 0; i<g_tCapability.iImageSizeDesc ; i++){
            res = new T_CameraImageResolution;
            res->iIndex = g_tCapability.pImageSizeDesc[i].iIndex;
            res->iWidth = g_tCapability.pImageSizeDesc[i].iWidth;
            res->iHeight = g_tCapability.pImageSizeDesc[i].iHeight;
            memcpy(res->acDescription,g_tCapability.pImageSizeDesc[i].acDescription,sizeof(res->acDescription));
            fmt->ImageSizeList.append(res);

        }
        tCapbility.fmtsList.append(fmt);
        tCapbility.curFmt = tCapbility.fmtsList.at(0);

        this->tCapbility.sColorTemp.ibUsed       = 0;

        this->tCapbility.sColorTemp.iTempMin     =  -1;
        this->tCapbility.sColorTemp.iTempDef     =  -1;
        this->tCapbility.sColorTemp.iTempMax     =  -1;

        this->tCapbility.sColorTemp.iTintMin     =  -1;
        this->tCapbility.sColorTemp.iTintDef     =  -1;
        this->tCapbility.sColorTemp.iTintMax     =  -1;


        this->tCapbility.pPackLenDesc =(T_CameraPackLength  *) g_tCapability.pPackLenDesc;
        this->tCapbility.iPackLenDesc = g_tCapability.iPackLenDesc;

        CameraGetExposureLineTime(g_hCamera, &(tCapbility.sExposeDesc.dfExpLineTime));
        tCapbility.sExposeDesc.dfExposeMin = g_tCapability.sExposeDesc.uiExposeTimeMin;
        tCapbility.sExposeDesc.dfExposeMax = g_tCapability.sExposeDesc.uiExposeTimeMax;
        tCapbility.sExposeDesc.dfExposeDef = ((tCapbility.sExposeDesc.dfExposeMin) + (tCapbility.sExposeDesc.dfExposeMax))/2;

        tCapbility.sExposeDesc.dfGainMin   = g_tCapability.sExposeDesc.uiAnalogGainMin;
        tCapbility.sExposeDesc.dfGainMax   = g_tCapability.sExposeDesc.uiAnalogGainMax;
        tCapbility.sExposeDesc.dfGainDef   = ((tCapbility.sExposeDesc.dfGainMin) + (tCapbility.sExposeDesc.dfGainMax))/2;
        tCapbility.sExposeDesc.fAnalogGainStep = g_tCapability.sExposeDesc.fAnalogGainStep;

        //memcpy(&(tCapbility.sExposeDesc),&(g_tCapability.sExposeDesc),sizeof(T_CameraExpose));



        memcpy(&(tCapbility.sResolutionRange),&(g_tCapability.sResolutionRange),sizeof(T_CameraResolutionRange)); // 分辨率范围描述
        memcpy(&(tCapbility.sRgbGainRange),&(g_tCapability.sRgbGainRange),sizeof(T_CameraGainRange));    // 图像数字增益范围描述
        memcpy(&(tCapbility.sSaturationRange),&(g_tCapability.sSaturationRange),sizeof(T_SaturationRange)); // 饱和度范围描述
        memcpy(&(tCapbility.sGammaRange),&(g_tCapability.sGammaRange),sizeof(T_GammaRange));      // 伽马范围描述
        memcpy(&(tCapbility.sContrastRange),&(g_tCapability.sContrastRange),sizeof(T_ContrastRange));   // 对比度范围描述
        memcpy(&(tCapbility.sSharpnessRange),&(g_tCapability.sSharpnessRange),sizeof(T_SharpnessRange));  // 锐化范围描述



        CameraGetImageResolution(g_hCamera,&tsdkImagResolution);
       // tCapbility.curResoultionIndex = tsdkImagResolution.iIndex;
        tCapbility.curResoultion = tCapbility.fmtsList.at(0)->ImageSizeList.at(tsdkImagResolution.iIndex);



        this->iHeight = tsdkImagResolution.iHeight;
        this->iWidth  = tsdkImagResolution.iWidth;




        //相机是触发模式
        CameraSetTriggerMode(g_hCamera,1);
        CameraSetTriggerCount(g_hCamera,1);
        CameraSetAeState(g_hCamera,false);


        /*让SDK进入工作模式，开始接收来自相机发送的图像
        数据。如果当前相机是触发模式，则需要接收到
        触发帧以后才会更新图像。    */
        CameraPlay(g_hCamera);



        /*
            设置图像处理的输出格式，彩色黑白都支持RGB24位
        */
        if(g_tCapability.sIspCapacity.bMonoSensor){
            CameraSetIspOutFormat(g_hCamera,CAMERA_MEDIA_TYPE_MONO8);
        }else{
            CameraSetIspOutFormat(g_hCamera,CAMERA_MEDIA_TYPE_RGB8);
        }
        ret = true;
    }
    return ret;
}

void IndustryCaptureOpr::getCameraCapbility(PT_CameraCapbility pCapbility)
{
    memcpy(pCapbility,&tCapbility,sizeof(T_CameraCapbility));

}
bool IndustryCaptureOpr::isOpened()
{
    return (g_hCamera != -1)?true:false;
}
void IndustryCaptureOpr::closeCapture()
{
    if(isOpened()){
        bCloseCapture = true;
    }
}
bool IndustryCaptureOpr::setCameraArgs(Camera::CameraArgs argId,const QVariant& variant)
{
    bool ret = false;
    int index = -1;
    bool covRet = false;
    switch(argId){
    case Camera::RESOLUTION:
        index = variant.toInt(&covRet);
        if(covRet){
            if((index < tCapbility.curFmt->ImageSizeList.count()) && (index >=0)){
                qDebug()<<"index:"<<index;
                curImageSizeDesc =(tSdkImageResolution*)(g_tCapability.pImageSizeDesc+index);
                bChangeRes = true;
                ret = true;
            }
        }
        break;
    case Camera::EXPORESUEID:
        if(variant.type() == QVariant::Int){
            ret = (SDK_SUCCESS(CameraSetExposureTime(g_hCamera,variant.toInt()*tCapbility.sExposeDesc.dfExpLineTime)));

        }else{
            qDebug(" QVariant::double ......................");
            ret = (SDK_SUCCESS(CameraSetExposureTime(g_hCamera,variant.toDouble())));
        }

        break;
    case Camera::GAINID:
        ret = SDK_SUCCESS(CameraSetAnalogGain(g_hCamera,variant.toInt()));
        break;
    default:
        break;
    }
    return ret;
}
bool IndustryCaptureOpr::snapPic()
{
    bool ret = false;
    tSdkFrameHead	tFrameHead;
    BYTE			*pbyBuffer;
    BYTE			*pbImgBuffer;
    QString filename="./456.jpg";
    if(CameraSnapToBuffer(g_hCamera,&tFrameHead,&pbyBuffer,1000) == CAMERA_STATUS_SUCCESS)
    {
        //CameraSnapToBuffer抓拍一张图像保存到buffer中
        pbImgBuffer = (unsigned char*)malloc((this->iWidth)*(this->iHeight)*3);
        /*
        将获得的相机原始输出图像数据进行处理，叠加饱和度、
        颜色增益和校正、降噪等处理效果，最后得到RGB888
        格式的图像数据。
        */
        CameraImageProcess(g_hCamera, pbyBuffer,pbImgBuffer,&tFrameHead);


        QImage img(pbImgBuffer,this->iWidth,this->iHeight,QImage::Format_RGB888);
        if(img.save(filename)){
                ret = true;
        }

        //释放由CameraGetImageBuffer获得的缓冲区。
        CameraReleaseImageBuffer(g_hCamera,pbImgBuffer);
        free(pbImgBuffer);
        ret = true;
    }

    qDebug()<<"CameraSnapToBuffer ret "<<ret;
    return ret;
}

bool IndustryCaptureOpr::getCameraArgs(Camera::CameraArgs argId,QVariant&value)
{
    bool ret = false;
    double dfExposureTime;
    int tmp;
    switch(argId){
    case Camera::EXPORESUEID:
           ret =  SDK_SUCCESS(CameraGetExposureTime(g_hCamera, &dfExposureTime));
           tmp = (int)(dfExposureTime/tCapbility.sExposeDesc.dfExpLineTime);
           value = QVariant::fromValue(tmp);
        break;
    case Camera::GAINID:
           ret = SDK_SUCCESS(CameraGetAnalogGain(g_hCamera,&tmp));
           value = QVariant::fromValue(tmp);
        break;
    default:
        break;
    }
    return ret;
}
void IndustryCaptureOpr::chanageReslution()
{
    bool ret = false;
    tSdkImageResolution     tsdkImagResolution;

    if(bChangeRes){
        ret = SDK_SUCCESS(CameraSetImageResolution(g_hCamera,curImageSizeDesc));
        CameraGetImageResolution(g_hCamera,&tsdkImagResolution);
        if(ret){
            this->iWidth = tsdkImagResolution.iWidth;
            this->iHeight = tsdkImagResolution.iHeight;
        }
        bChangeRes = false;

    }

}
bool IndustryCaptureOpr::grabFrame()
{
    bool ret = false;
    if(g_hCamera == -1){
        return ret;
    }
    if(rawData){
        CameraReleaseImageBuffer(g_hCamera,rawData);
        rawData = NULL;
    }



    if (CameraGetImageBuffer(g_hCamera,&g_tFrameHead,&rawData,2000) == CAMERA_STATUS_SUCCESS)
    {
        CameraImageProcess(g_hCamera,rawData,(BYTE*)picData.data(),&g_tFrameHead);
        this->reverseRGB = false;
        ret = true;

#if 0
        if(g_tFrameHead.uiMediaType==CAMERA_MEDIA_TYPE_MONO8){

            memcpy(g_readBuf,g_pRgbBuffer,g_W_H_INFO.buffer_size);


        }else{
            memcpy(g_readBuf,g_pRgbBuffer,g_W_H_INFO.buffer_size*3);

        }
#endif

    }
    if(bCloseCapture){
        CameraUnInit(g_hCamera);
        clearCameraCability(tCapbility);
        g_hCamera=-1;
        bCloseCapture = false;
    }
    return ret;
}
void IndustryCaptureOpr::trigger2()
{
    if(isOpened()){

        if(!ispause){
            //执行一次软触发。执行后，会触发由CameraSetTriggerCount指定的帧数。
            //curTime = QDateTime::currentDateTime();

            CameraSoftTrigger(g_hCamera);
            emit imageComming();
        }else{
            return ;
        }

    }
}
IndustryCaptureOpr::~IndustryCaptureOpr()
{
    if(g_hCamera != -1){
        CameraUnInit(g_hCamera);
        clearCameraCability(tCapbility);
        g_hCamera=-1;
        bCloseCapture = false;
    }
    qDebug("~IndustryCaptureOpr ... ");
}

