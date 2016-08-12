#include "v4l2captureopr.h"
#ifdef __linux
#include "imageopr.h"
#include <QDebug>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <errno.h>
#include <QImage>

int V4L2CaptureOpr::supportFmts[3]={V4L2_PIX_FMT_YUYV,V4L2_PIX_FMT_RGB565,V4L2_PIX_FMT_MJPEG};

V4L2CaptureOpr::V4L2CaptureOpr(const QString& name,QObject *parent):ICaptureOpr(name,parent)
{
    iFd = -1;
    isStart = false;
    bOpened =false;
    isGrabImage = false;
    this->iVideoBufCnt = 0;
    iCurFmtIndex = 0;
    iCurResIndex = 3;
    bSupportReadIo = false;
}
bool V4L2CaptureOpr::isSupport()
{
    int     iIndex = 0;
    int     iError;
    bool    ret = false;
    struct v4l2_capability tV4l2Cap;

    do{
        memset(strName,0,sizeof(strName));
        sprintf(strName,"/dev/video%d",iIndex++);
        iFd = open(strName,O_RDWR);
    }while((iFd < 0) && (iIndex < MAXDEVNUM));

    if(iFd < 0){
        qDebug("can not open v4l2 device!");
        return ret;
    }

    memset(&tV4l2Cap, 0, sizeof(struct v4l2_capability));
    iError = ioctl(iFd, VIDIOC_QUERYCAP, &tV4l2Cap);
    if (iError) {
        qDebug("Error opening device %s: unable to query device.\n", strName);
        return ret;
    }

    if (!(tV4l2Cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        qDebug("%s is not a video capture device\n", strName);
        return ret;
    }
    if (tV4l2Cap.capabilities & V4L2_CAP_STREAMING) {
        qDebug("%s supports streaming i/o\n", strName);
    }

    if (tV4l2Cap.capabilities & V4L2_CAP_READWRITE) {
        bSupportReadIo = false;
        qDebug("%s supports read i/o\n", strName);
    }
    clearCameraCability(tCapbility);
    iError = enumFrameFormats();

    if(iError == 0){

        for(int i =0; i< tCapbility.fmtsList.count(); i++){
            if(isSupportThisFmt(tCapbility.fmtsList.at(i)->iMediaType)){
                this->iCurFmtIndex = i;
                ret = true;
                break;
            }
        }
    }

    return ret;
}
int V4L2CaptureOpr::enumFrameIntervals(int pixfmt,int width, int height){
    int ret;
    struct v4l2_frmivalenum fival;

    memset(&fival, 0, sizeof(fival));
    fival.index = 0;
    fival.pixel_format = pixfmt;
    fival.width = width;
    fival.height = height;
    qDebug("\tTime interval between frame: ");
    while ((ret = ioctl(iFd, VIDIOC_ENUM_FRAMEINTERVALS, &fival)) == 0) {
        if (fival.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
                qDebug("%u/%u, ",
                        fival.discrete.numerator, fival.discrete.denominator);
        } else if (fival.type == V4L2_FRMIVAL_TYPE_CONTINUOUS) {
                qDebug("{min { %u/%u } .. max { %u/%u } }, ",
                        fival.stepwise.min.numerator, fival.stepwise.min.numerator,
                        fival.stepwise.max.denominator, fival.stepwise.max.denominator);
                break;
        } else if (fival.type == V4L2_FRMIVAL_TYPE_STEPWISE) {
                qDebug("{min { %u/%u } .. max { %u/%u } / "
                        "stepsize { %u/%u } }, ",
                        fival.stepwise.min.numerator, fival.stepwise.min.denominator,
                        fival.stepwise.max.numerator, fival.stepwise.max.denominator,
                        fival.stepwise.step.numerator, fival.stepwise.step.denominator);
                break;
        }
        fival.index++;
    }
    qDebug("\n");
    if (ret != 0 && errno != EINVAL) {
        qDebug("ERROR enumerating frame intervals: %d\n", errno);
        return errno;
    }

    return 0;
}
int V4L2CaptureOpr::enumFrameSizes(int pixfmt,QList<T_CameraImageResolution*>& resList)
{
    int ret;
    struct v4l2_frmsizeenum fsize;
    T_CameraImageResolution* pImageSize;
    memset(&fsize, 0, sizeof(fsize));
    fsize.index = 0;
    fsize.pixel_format = pixfmt;
    while ((ret = ioctl(iFd, VIDIOC_ENUM_FRAMESIZES, &fsize)) == 0) {
            if (fsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                pImageSize = new T_CameraImageResolution;
                memset(pImageSize->acDescription,0,sizeof(pImageSize->acDescription));
                sprintf(pImageSize->acDescription,"%d * %d",fsize.discrete.width,fsize.discrete.height);
                pImageSize->iWidth = fsize.discrete.width;
                pImageSize->iHeight = fsize.discrete.height;
                resList.append(pImageSize);
                qDebug("{ discrete: width = %u, height = %u }\n",
                        fsize.discrete.width, fsize.discrete.height);
                ret = enumFrameIntervals(pixfmt,fsize.discrete.width, fsize.discrete.height);
                if (ret != 0)
                    qDebug("  Unable to enumerate frame sizes.\n");
            } else if (fsize.type == V4L2_FRMSIZE_TYPE_CONTINUOUS) {
                qDebug("{ continuous: min { width = %u, height = %u } .. "
                        "max { width = %u, height = %u } }\n",
                        fsize.stepwise.min_width, fsize.stepwise.min_height,
                        fsize.stepwise.max_width, fsize.stepwise.max_height);
                qDebug("  Refusing to enumerate frame intervals.\n");
                break;
            } else if (fsize.type == V4L2_FRMSIZE_TYPE_STEPWISE) {
                qDebug("{ stepwise: min { width = %u, height = %u } .. "
                        "max { width = %u, height = %u } / "
                        "stepsize { width = %u, height = %u } }\n",
                        fsize.stepwise.min_width, fsize.stepwise.min_height,
                        fsize.stepwise.max_width, fsize.stepwise.max_height,
                        fsize.stepwise.step_width, fsize.stepwise.step_height);
                qDebug("  Refusing to enumerate frame intervals.\n");
                break;
            }else{
                pImageSize = new T_CameraImageResolution;
                memset(pImageSize->acDescription,0,sizeof(pImageSize->acDescription));
                sprintf(pImageSize->acDescription,"%d * %d",fsize.discrete.width,fsize.discrete.height);
                pImageSize->iWidth = fsize.discrete.width;
                pImageSize->iHeight = fsize.discrete.height;
                resList.append(pImageSize);
                qDebug("{ discrete: width = %u, height = %u }\n",
                        fsize.discrete.width, fsize.discrete.height);
                ret = enumFrameIntervals(pixfmt,fsize.discrete.width, fsize.discrete.height);
                if (ret != 0)
                    qDebug("  Unable to enumerate frame sizes.\n");
            }
            fsize.index++;
        }
        if (ret != 0 && errno != EINVAL) {
            qDebug("ERROR enumerating frame sizes: %d\n", errno);
            return errno;
        }
    return 0;
}

void V4L2CaptureOpr::trigger2()
{
    if(isStart){
        emit imageComming();
    }

}
bool V4L2CaptureOpr::grabFrame()
{
    bool ret = false;
    isGrabImage = true;
    int iRet;
    struct v4l2_buffer tV4l2Buf;
    QImage img;
    if(bSupportReadIo){
        qDebug("grub ...");
        iRet = read(iFd, this->pucVideBuf[0], this->iVideoBufMaxLen);
        if (iRet <= 0)
        {
            return ret;
        }

        if(tCapbility.curFmt->iMediaType == V4L2_PIX_FMT_YUYV){
            ImageOpr::YUVS422toRgb24(this->pucVideBuf[0],this->picData.data(),tCapbility.curResoultion->iWidth,tCapbility.curResoultion->iHeight);
        }else if(tCapbility.curFmt->iMediaType == V4L2_PIX_FMT_MJPEG){
            img = QImage::fromData(this->pucVideBuf[0],iRet,"JPEG");
            memcpy(this->picData.data(),img.convertToFormat(QImage::Format_RGB888).bits(),this->picData.size());
            //ImageOpr::RGB32toRGB24(img.bits(),item->image.bits(),imgInfo.iWidth,imgInfo.iHeight,img.bytesPerLine());

        }else{
            qDebug("not support this fmt ...");
        }

     }else{


        memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));
        tV4l2Buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        tV4l2Buf.memory = V4L2_MEMORY_MMAP;
        iRet = ioctl(this->iFd, VIDIOC_DQBUF, &tV4l2Buf);
        if (iRet < 0)
        {
            qDebug("Unable to dequeue buffer.\n");
            return ret;
        }
        this->iVideoBufCurIndex = tV4l2Buf.index;
        this->iByteUsed = tV4l2Buf.bytesused;


        if(tCapbility.curFmt->iMediaType == V4L2_PIX_FMT_YUYV){
            ImageOpr::YUVS422toRgb24(this->pucVideBuf[this->iVideoBufCurIndex],picData.data(),tCapbility.curResoultion->iWidth,tCapbility.curResoultion->iHeight);
        }else if(tCapbility.curFmt->iMediaType == V4L2_PIX_FMT_MJPEG){
            img = QImage::fromData(this->pucVideBuf[this->iVideoBufCurIndex],this->iByteUsed,"JPEG");
            memcpy(this->picData.data(),img.convertToFormat(QImage::Format_RGB888).bits(),this->picData.size());
            //ImageOpr::RGB32toRGB24(img.bits(),item->image.bits(),imgInfo.iWidth,imgInfo.iHeight,img.bytesPerLine());

        }else{
            qDebug("not support this fmt ...");
        }

        memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));

        tV4l2Buf.index	= this->iVideoBufCurIndex;
        tV4l2Buf.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
        tV4l2Buf.memory = V4L2_MEMORY_MMAP;
        iRet = ioctl(this->iFd, VIDIOC_QBUF, &tV4l2Buf);
        if (iRet)
        {
            qDebug("Unable to queue buffer.\n");
        }
        ret = true;

    }
    isGrabImage = false;
    return ret;
}

int V4L2CaptureOpr::enumControls()
{
    struct v4l2_queryctrl tQueryCrl;
    struct v4l2_control   tCrls;
    struct v4l2_input*    getinput;

    getinput = (struct v4l2_input*)calloc(1,sizeof(struct v4l2_input));
    memset(getinput,0,sizeof(struct v4l2_input));
    getinput->index=0;
    ioctl(iFd,VIDIOC_ENUMINPUT , getinput);
    qDebug ("Available controls of device '%s' (Type 1=Integer 2=Boolean 3=Menu 4=Button)\n", getinput->name);


    qDebug ("V4L2_CID_BASE         (predefined controls):\n");
    memset (&tQueryCrl, 0, sizeof (tQueryCrl));
    for (tQueryCrl.id = V4L2_CID_BASE;tQueryCrl.id < V4L2_CID_LASTP1;tQueryCrl.id++) {
      if (0 == ioctl (iFd, VIDIOC_QUERYCTRL, &tQueryCrl)) {
        if (tQueryCrl.flags & V4L2_CTRL_FLAG_DISABLED)
          continue;
        tCrls.id=tQueryCrl.id;
        ioctl(iFd, VIDIOC_G_CTRL, &tCrls);

        switch (tQueryCrl.id) {
        case V4L2_CID_BRIGHTNESS:
            tCapbility.sBrightnessRange.ibUsed = 1;
            tCapbility.sBrightnessRange.iMax   = tQueryCrl.maximum;
            tCapbility.sBrightnessRange.iMin   = tQueryCrl.minimum;
            tCapbility.sBrightnessRange.iDef   = tQueryCrl.default_value;
            tCapbility.sBrightnessRange.iSaved = tQueryCrl.default_value;
            break;
        case V4L2_CID_CONTRAST:
            tCapbility.sContrastRange.ibUsed    = 1;
            tCapbility.sContrastRange.iMax      = tQueryCrl.maximum;
            tCapbility.sContrastRange.iMin      = tQueryCrl.minimum;
            tCapbility.sContrastRange.iDef      = tQueryCrl.default_value;
            tCapbility.sContrastRange.iSaved    = tQueryCrl.default_value;
            break;
        case V4L2_CID_SATURATION:
            tCapbility.sSaturationRange.ibUsed  =  1;
            tCapbility.sSaturationRange.iMax    =  tQueryCrl.maximum;
            tCapbility.sSaturationRange.iMin    =  tQueryCrl.minimum;
            tCapbility.sSaturationRange.iDef    =  tQueryCrl.default_value;
            tCapbility.sSaturationRange.iSaved  =  tQueryCrl.default_value;
            break;
        case V4L2_CID_HUE:
            tCapbility.sShadesRange.ibUsed      = 1;
            tCapbility.sShadesRange.iMax    = tQueryCrl.maximum;
            tCapbility.sShadesRange.iMin    = tQueryCrl.minimum;
            tCapbility.sShadesRange.iDef    = tQueryCrl.default_value;
            tCapbility.sShadesRange.iSaved  = tQueryCrl.default_value;
            break;
        case V4L2_CID_GAMMA:
            tCapbility.sGammaRange.ibUsed   = 1;
            tCapbility.sGammaRange.iMax     = tQueryCrl.maximum;
            tCapbility.sGammaRange.iMin     = tQueryCrl.minimum;
            tCapbility.sGammaRange.iDef     = tQueryCrl.default_value;
            tCapbility.sGammaRange.iSaved   = tQueryCrl.default_value;
            break;
        case V4L2_CID_EXPOSURE:
            tCapbility.sExposeDesc.dfExpLineTime = 1.0;
            tCapbility.sExposeDesc.dfExposeMax   = tQueryCrl.maximum;
            tCapbility.sExposeDesc.dfExposeMin   = tQueryCrl.minimum;
            tCapbility.sExposeDesc.dfExposeDef   = tQueryCrl.default_value;
            tCapbility.sExposeDesc.dfExposeSaved = tQueryCrl.default_value;
            break;
        case V4L2_CID_GAIN:
            tCapbility.sExposeDesc.dfGainMax = tQueryCrl.maximum;
            tCapbility.sExposeDesc.dfGainMin = tQueryCrl.minimum;
            tCapbility.sExposeDesc.dfGainDef   = tQueryCrl.default_value;
            tCapbility.sExposeDesc.dfGainSaved = tQueryCrl.default_value;
        default:
            break;
        }



        qDebug (" index:%-10d name:%-32s type:%d min:%-5d max:%-5d step:%-5d def:%-5d now:%d \n",
                tQueryCrl.id, tQueryCrl.name, tQueryCrl.type, tQueryCrl.minimum,
                tQueryCrl.maximum, tQueryCrl.step, tQueryCrl.default_value, tCrls.value);

      } else {
        if (errno == EINVAL)
          continue;
        qDebug ("error getting base controls");
        goto fatal_controls;
      }
    }


    qDebug ("V4L2_CID_PRIVATE_BASE (driver specific controls):\n");
    for (tQueryCrl.id = V4L2_CID_PRIVATE_BASE;;
       tQueryCrl.id++) {
    if (0 == ioctl (iFd, VIDIOC_QUERYCTRL, &tQueryCrl)) {
      if (tQueryCrl.flags & V4L2_CTRL_FLAG_DISABLED)
        continue;
      tCrls.id=tQueryCrl.id;
      ioctl(iFd, VIDIOC_G_CTRL, &tCrls);
      qDebug (" index:%-10d name:%-32s type:%d min:%-5d max:%-5d step:%-5d def:%-5d now:%d \n",
              tQueryCrl.id, tQueryCrl.name, tQueryCrl.type, tQueryCrl.minimum,
              tQueryCrl.maximum, tQueryCrl.step, tQueryCrl.default_value, tCrls.value);

    } else {
      if (errno == EINVAL)
        break;
      perror ("error getting private base controls");
      goto fatal_controls;
      }
    }
    return 0;
    fatal_controls:
    return -1;
}

int V4L2CaptureOpr::enumFrameFormats()
{
    int iRet;
    T_CameraMediaType*  tmpFmt;
    struct v4l2_fmtdesc tFmtDesc;
    memset(&tFmtDesc,0,sizeof(tFmtDesc));
    tFmtDesc.index = 0;
    tFmtDesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    while((iRet = ioctl(iFd,VIDIOC_ENUM_FMT,&tFmtDesc))==0){
        tmpFmt = new T_CameraMediaType;
        tmpFmt->iIndex = tFmtDesc.index;
        memcpy(tmpFmt->acDescription,tFmtDesc.description,sizeof(tmpFmt->acDescription));
        tmpFmt->iMediaType = tFmtDesc.pixelformat;

        tFmtDesc.index++;

        qDebug("{ pixelformat = '%c%c%c%c', description = '%s' }\n",tFmtDesc.pixelformat & 0xFF,
        (tFmtDesc.pixelformat >> 8) & 0xFF,(tFmtDesc.pixelformat >> 16) & 0xFF,
        (tFmtDesc.pixelformat >> 24) & 0xFF,tFmtDesc.description);

        iRet = enumFrameSizes(tFmtDesc.pixelformat,tmpFmt->ImageSizeList);
        if(iRet != 0){
            qDebug("Unable to enumerate frame sizes.\n");
        }else{
            tCapbility.fmtsList.append(tmpFmt);
        }
    }

    if (errno != EINVAL) {
            qDebug("ERROR enumerating frame formats: %d\n", errno);
            return errno;
    }


    return 0;
}

bool V4L2CaptureOpr::openCapture(void)
{
    int     iError;
    int     i;
    struct v4l2_format  tV4l2Fmt;
    struct v4l2_requestbuffers tV4l2ReqBuffs;
    struct v4l2_buffer tV4l2Buf;
    if(this->iFd != -1){
        close(iFd);
    }


    if(isSupport()){
        iError = enumControls();
        memset(&tV4l2Fmt, 0, sizeof(struct v4l2_format));
        tV4l2Fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        tV4l2Fmt.fmt.pix.pixelformat = tCapbility.fmtsList.at(iCurFmtIndex)->iMediaType;
        tV4l2Fmt.fmt.pix.width       = tCapbility.fmtsList.at(iCurFmtIndex)->ImageSizeList.at(iCurResIndex)->iWidth;
        tV4l2Fmt.fmt.pix.height      = tCapbility.fmtsList.at(iCurFmtIndex)->ImageSizeList.at(iCurResIndex)->iHeight;
        tV4l2Fmt.fmt.pix.field       = V4L2_FIELD_ANY;

        iError = ioctl(iFd, VIDIOC_S_FMT, &tV4l2Fmt);
        if (iError)
        {
            qDebug("Unable to set format\n");
            goto err_exit;
        }
        tCapbility.curFmt = tCapbility.fmtsList.at(iCurFmtIndex);
        tCapbility.curResoultion = tCapbility.fmtsList.at(iCurFmtIndex)->ImageSizeList.at(iCurResIndex);
        iWidth  = tCapbility.fmtsList.at(iCurFmtIndex)->ImageSizeList.at(iCurResIndex)->iWidth;
        iHeight = tCapbility.fmtsList.at(iCurFmtIndex)->ImageSizeList.at(iCurResIndex)->iHeight;




        /* request buffers */

        if(bSupportReadIo){
            iVideoBufMaxLen = iWidth * iHeight * 4;
            this->pucVideBuf[0] = (unsigned char*)malloc(iVideoBufMaxLen);
        }else{
            memset(&tV4l2ReqBuffs, 0, sizeof(struct v4l2_requestbuffers));

            tV4l2ReqBuffs.count = NB_BUFFER;

            tV4l2ReqBuffs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            tV4l2ReqBuffs.memory = V4L2_MEMORY_MMAP;

            iError = ioctl(iFd, VIDIOC_REQBUFS, &tV4l2ReqBuffs);
            if (iError)
            {
                qDebug("Unable to allocate buffers.\n");
                goto err_exit;
            }

            this->iVideoBufCnt = tV4l2ReqBuffs.count;

            /* map the buffers */
            for (i = 0; i < this->iVideoBufCnt; i++)
            {
                memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));
                tV4l2Buf.index = i;
                tV4l2Buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                tV4l2Buf.memory = V4L2_MEMORY_MMAP;
                iError = ioctl(iFd, VIDIOC_QUERYBUF, &tV4l2Buf);
                if (iError)
                {
                    qDebug("Unable to query buffer.\n");
                    goto err_exit;
                }

                this->iVideoBufMaxLen = tV4l2Buf.length;
                this->pucVideBuf[i] = (unsigned char*)mmap(0 /* start anywhere */ ,tV4l2Buf.length, PROT_READ, MAP_SHARED, iFd,tV4l2Buf.m.offset);
                if (this->pucVideBuf[i] == NULL)
                {
                    qDebug("Unable to map buffer\n");
                    goto err_exit;
                }
            }

            /* Queue the buffers. */
            for (i = 0; i < this->iVideoBufCnt; i++)
            {
                memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));
                tV4l2Buf.index = i;
                tV4l2Buf.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                tV4l2Buf.memory = V4L2_MEMORY_MMAP;
                iError = ioctl(iFd, VIDIOC_QBUF, &tV4l2Buf);
                if (iError)
                {
                    qDebug("Unable to queue buffer.\n");
                    goto err_exit;
                }
            }


        }

        int iType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        iError = ioctl(this->iFd, VIDIOC_STREAMON, &iType);
        if (iError)
        {
            qDebug("Unable to start capture.\n");
            goto err_exit;
        }
        isStart = true;
        bOpened = true;
        return true;
    }


 err_exit:
    close(iFd);
    clearCameraCability(tCapbility);
    this->iFd = -1;
    return false;
}
bool V4L2CaptureOpr::isOpened()
{
    return bOpened;
}
int V4L2CaptureOpr::stopCapture()
{
    while(isGrabImage);
    int iType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int iError = 0;
    if(isStart){
        iError = ioctl(this->iFd, VIDIOC_STREAMOFF, &iType);
        if (iError)
        {
            qDebug("Unable to stop capture.\n");
            return -1;
        }
    }
    isStart = false;
    return 0;
}

void V4L2CaptureOpr::closeCapture()
{

    int i;
    stopCapture();
    if(bSupportReadIo){
            free(this->pucVideBuf[0]);
    }else{
        for (i = 0; i < this->iVideoBufCnt; i++)
        {
            if (this->pucVideBuf[i])
            {
                munmap(this->pucVideBuf[i], this->iVideoBufMaxLen);
                this->pucVideBuf[i] = NULL;
            }
        }
    }
    close(this->iFd);
    this->iFd = -1;
    clearCameraCability(tCapbility);
    bOpened =false;

}

void V4L2CaptureOpr::getCameraCapbility(PT_CameraCapbility pCapbility)
{
    memcpy(pCapbility,&tCapbility,sizeof(tCapbility));
}

bool V4L2CaptureOpr::isSupportThisFmt(const int& fmt)
{
    bool ret = false;
    for(int i = 0; i < sizeof(supportFmts)/(sizeof(supportFmts[0])); i++)
    {
        if(supportFmts[i] == fmt)
        {
            ret = true;
            break;
        }
    }
    return ret;
}

int V4L2CaptureOpr::getContrl(const int& contrl)
{
    struct v4l2_queryctrl queryCtrl;
    struct v4l2_control    contros;
    int err;
    if(isv4l2Contrl(contrl,&queryCtrl)<0){
        return -1;
    }
    contros.id = contrl;
    if ((err = ioctl(iFd, VIDIOC_G_CTRL, &contros)) < 0) {
        qDebug("ioctl get control error\n");
        return -1;
    }
    return contros.value;
}
int V4L2CaptureOpr::resetControl(const int &control)
{
    struct v4l2_control control_s;
    struct v4l2_queryctrl queryctrl;
    int val_def;
    int err;
    if (isv4l2Contrl(control, &queryctrl) < 0)
            return -1;
    val_def = queryctrl.default_value;
    control_s.id = control;
    control_s.value = val_def;
    if ((err = ioctl(iFd, VIDIOC_S_CTRL, &control_s)) < 0) {
        printf("ioctl reset control error\n");
        return -1;
    }

    return 0;
}
int V4L2CaptureOpr::isv4l2Contrl(const int& contrl,struct v4l2_queryctrl *queryctrl)
{
    int err = 0;
    queryctrl->id = contrl;
    if((err=ioctl(iFd,VIDIOC_QUERYCTRL,queryctrl))<0){
        qDebug("ioctl querycontrol error %d \n",errno);
    }else if(queryctrl->flags & V4L2_CTRL_FLAG_DISABLED){
        qDebug("control %s disabled \n", (char *) queryctrl->name);
    }else if (queryctrl->flags & V4L2_CTRL_TYPE_BOOLEAN) {
        return 1;
    } else if (queryctrl->type & V4L2_CTRL_TYPE_INTEGER) {
        return 0;
    } else {
        qDebug("contol %s unsupported  \n", (char *) queryctrl->name);
    }
        return -1;
}
int V4L2CaptureOpr::setContrl(const int& contrl,const int& value)
{
    struct v4l2_control control_s;
    struct v4l2_queryctrl queryctrl;
    int min, max, step, val_def;
    int err;
    if (isv4l2Contrl(contrl, &queryctrl) < 0)
        return -1;
    min = queryctrl.minimum;
    max = queryctrl.maximum;
    step = queryctrl.step;
    val_def = queryctrl.default_value;
    if ((value >= min) && (value <= max)) {
        control_s.id = contrl;
        control_s.value = value;
        if ((err = ioctl(iFd, VIDIOC_S_CTRL, &control_s)) < 0) {
            qDebug("ioctl set control error\n");
            return -1;
        }
    }
    return 0;
}

bool V4L2CaptureOpr::setCameraArgs(Camera::CameraArgs argId,const QVariant& variant)
{
    bool ret = false;
    unsigned int ctrlId;
    int tempValue;
    switch(argId){
    case Camera::RESOLUTION:
        tempValue = variant.toInt();
        this->iCurResIndex = tempValue;
        this->closeCapture();
        this->openCapture();
        //this->trigger2();
        break;
    case Camera::EXPORESUEID:
        ctrlId = V4L2_CID_EXPOSURE;
        tempValue=this->setContrl(ctrlId,variant.toInt());
        if(tempValue == 0){
            ret = true;
        }

        break;
    case Camera::GAINID:
        ctrlId = V4L2_CID_GAIN;
        tempValue = this->setContrl(ctrlId,variant.toInt());

        if(tempValue == 0){
            ret = true;
        }

        break;
    case Camera::COLORTMPID:

        break;
    case Camera::TINTID:

        break;
    case Camera::SHADESID:
        ctrlId = V4L2_CID_HUE;
        tempValue = this->setContrl(ctrlId,variant.toInt());

        if(tempValue == 0 ){
            ret = true;
        }

        break;
    case Camera::SATURATIONID:
        ctrlId = V4L2_CID_SATURATION;
        tempValue = this->setContrl(ctrlId,variant.toInt());

        if(tempValue == 0){
            ret = true;
        }
        break;
    case Camera::BRIGHTNESSID:
        ctrlId = V4L2_CID_BRIGHTNESS;
        tempValue = this->setContrl(ctrlId,variant.toInt());

        if(tempValue == 0){
            ret = true;
        }
        break;
    case Camera::CONTRASTID:
        ctrlId = V4L2_CID_CONTRAST;
        tempValue = this->setContrl(ctrlId,variant.toInt());

        if(tempValue == 0){
            ret = true;
        }
        break;
    case Camera::GAMAID:
        ctrlId = V4L2_CID_GAMMA;
        tempValue = this->setContrl(ctrlId,variant.toInt());

        if(tempValue == 0){
            ret = true;
        }
        break;
    default:
        break;
    }
    return ret;
}
bool V4L2CaptureOpr::getCameraArgs(Camera::CameraArgs argId,QVariant&value)
{
    bool ret = false;
    unsigned int ctrlId;
    int tempValue;
    switch(argId){
    case Camera::EXPORESUEID:
        ctrlId = V4L2_CID_EXPOSURE;
        tempValue=this->getContrl(ctrlId);

        if(tempValue != -1){
            ret = true;
            value=QVariant::fromValue(tempValue);
        }

        break;
    case Camera::GAINID:
        ctrlId = V4L2_CID_GAIN;
        tempValue = this->getContrl(ctrlId);

        if(tempValue != -1){
            ret = true;
            value=QVariant::fromValue(tempValue);
        }

        break;
    case Camera::COLORTMPID:

        break;
    case Camera::TINTID:

        break;
    case Camera::SHADESID:
        ctrlId = V4L2_CID_HUE;
        tempValue = this->getContrl(ctrlId);

        if(tempValue != -1){
            ret = true;
            value=QVariant::fromValue(tempValue);
        }

        break;
    case Camera::SATURATIONID:
        ctrlId = V4L2_CID_SATURATION;
        tempValue = this->getContrl(ctrlId);

        if(tempValue != -1){
            ret = true;
            value=QVariant::fromValue(tempValue);
        }
        break;
    case Camera::BRIGHTNESSID:
        ctrlId = V4L2_CID_BRIGHTNESS;
        tempValue = this->getContrl(ctrlId);

        if(tempValue != -1){
            ret = true;
            value=QVariant::fromValue(tempValue);
        }
        break;
    case Camera::CONTRASTID:
        ctrlId = V4L2_CID_CONTRAST;
        tempValue = this->getContrl(ctrlId);

        if(tempValue != -1){
            ret = true;
            value=QVariant::fromValue(tempValue);
        }
        break;
    case Camera::GAMAID:
        ctrlId = V4L2_CID_GAMMA;
        tempValue = this->getContrl(ctrlId);

        if(tempValue != -1){
            ret = true;
            value=QVariant::fromValue(tempValue);
        }
        break;
    default:
        break;
    }
    return ret;
}
V4L2CaptureOpr::~V4L2CaptureOpr()
{
    //this->closeCapture();
    qDebug()<<"V4L2CaptureOpr::~V4L2CaptureOpr()";
}
#endif
