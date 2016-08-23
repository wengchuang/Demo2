#include "videocapture.h"
#include "datamanager.h"
#include "appconfig.h"


#define TDIBWIDTHBYTES(bits)	(((bits) + 31) / 32 * 4)

VideoCapture::VideoCapture(QObject *parent) :
    QObject(parent)
{
    initBase();
}
VideoCapture::VideoCapture(VideoRender*render,ImageProcesser* processer, QObject *parent):videoRender(render),processer(processer),QObject(parent)
{
    if(videoRender){
        installVideoRender(videoRender);
    }
    if(this->processer){
        installImageProcesser(this->processer);
    }
     initBase();
}

void VideoCapture::initBase()
{
    videoRender = NULL;
    processer   = NULL;
    capOpr      = NULL;
    mheader.biSize = sizeof(BITMAPINFOHEADER);
    mheader.biPlanes = 1;
    mheader.biBitCount = 24;
    mheader.biWidth = 0;
    mheader.biHeight = 0;
    bStop = false;
    Appconfig::getInstance()->getCameraMode(curMode);

    //connect(this,SIGNAL(imageComming()),this,SLOT(putImage()));

}
void VideoCapture::setCameraMode(Camera::CameraMode mode)
{
    if(capOpr && videoRender){
        capOpr->setCameraMode(mode);
        if(mode == Camera::MODE_AUTO){
            connect(videoRender,SIGNAL(repaintOver()),this,SLOT(trigger()));
            if(capOpr->isOpened()){
                this->trigger();
            }
        }else if(mode == Camera::MODE_EXTERN){
            disconnect(videoRender,SIGNAL(repaintOver()),this,SLOT(trigger()));
        }

    }
}

void  VideoCapture::installCaptureOpr(ICaptureOpr* opr)
{
    if(opr){
        this->capOpr = opr;
        connect(opr,SIGNAL(imageComming()),this,SLOT(putImage()));
    }
}
#ifdef _WIN32
void VideoCapture::dealRenderAppEvent(int evID)
{
    if(capOpr){
        capOpr->dealAppEvent(evID);
    }
}
#endif

void  VideoCapture::trigger()
{
   if(!capOpr){
       return ;
   }
   if(!bStop){
       if(this->thread() != &thr){
           this->moveToThread(&thr);
           if(!thr.isRunning()){
               thr.start();
           }
       }
       capOpr->trigger2();
   }else{
       thr.quit();
   }


}
void VideoCapture::putImage()
{
    if(!bStop){
        capOpr->chanageReslution();
        if((mheader.biWidth != capOpr->iWidth) || (mheader.biHeight != capOpr->iHeight)){
            if((capOpr->iWidth != -1) && (capOpr->iHeight != -1)){
                mheader.biWidth = capOpr->iWidth;
                mheader.biHeight = capOpr->iHeight;
                mheader.biSizeImage = TDIBWIDTHBYTES(mheader.biWidth * mheader.biBitCount) * mheader.biHeight;
                capOpr->picData.resize(mheader.biSizeImage);
                picHeader.resize(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));

                const char* tmp;
                BITMAPFILEHEADER fheader = {0};
                fheader.bfType = 0x4d42;
                fheader.bfSize = picHeader.size() + capOpr->picData.size();
                fheader.bfOffBits = picHeader.size();
                tmp = (char*)&fheader;
                memcpy(picHeader.data(),tmp,sizeof(fheader));
                tmp = (const char*)&mheader;
                memcpy(picHeader.data()+sizeof(fheader),tmp,sizeof(mheader));
                DataManager::getInstance()->initItems(mheader,fheader);
            }
        }
        if(capOpr->grabFrame()){
            DataItem* item =  DataManager::getInstance()->getPreMatDataItem();
            if(item){
                 memcpy(item->data.data()+item->offset, capOpr->picData.data(), capOpr->picData.size());
                 item->reverseRGB = capOpr->reverseRGB;
                 DataManager::getInstance()->putPreMatDataItem(item);
                 emit capOneFrame();
            }
        }else{
            trigger();
        }
    }else{
        thr.quit();
    }
}
void  VideoCapture::installVideoRender(VideoRender*render)
{
#ifdef _WIN32
    WId wid;
    UINT nMsg;
#endif
    if(processer){
        if(videoRender){
            disconnect(processer,SIGNAL(processImageOver()),videoRender,SLOT(flushImage()));
            disconnect(videoRender,SIGNAL(repaintOver()),this,SLOT(trigger()));
        }
#ifdef _WIN32
        if(videoRender){
            disconnect(videoRender,SIGNAL(appEvent(int)),this,SLOT(dealRenderAppEvent(int)));
        }
        connect(render,SIGNAL(appEvent(int)),this,SLOT(dealRenderAppEvent(int)));
        this->capOpr->setVideoRender(render);
        render->getWinIdAndEvIdention(wid,nMsg);
        this->capOpr->setWinIDAndMsgId(wid,nMsg);
#endif

        videoRender = render;

        connect(this,SIGNAL(capOneFrame()),processer,SLOT(processImage()));
        connect(processer,SIGNAL(processImageOver()),videoRender,SLOT(flushImage()));
        setCameraMode(curMode);
        //connect(videoRender,SIGNAL(repaintOver()),this,SLOT(trigger()));

    }else{
#ifdef _WIN32
        if(videoRender){
            disconnect(videoRender,SIGNAL(appEvent(int)),this,SLOT(dealRenderAppEvent(int)));
        }
        connect(render,SIGNAL(appEvent(int)),this,SLOT(dealRenderAppEvent(int)));
        this->capOpr->setVideoRender(render);
        render->getWinIdAndEvIdention(wid,nMsg);
        this->capOpr->setWinIDAndMsgId(wid,nMsg);
#endif
        videoRender = render;
    }



}
void  VideoCapture::installImageProcesser(ImageProcesser* algprocesser)
{
    if(processer){
        disconnect(this,SIGNAL(capOneFrame()),processer,SLOT(processImage()));
        if(videoRender){
            disconnect(processer,SIGNAL(processImageOver()),videoRender,SLOT(flushImage()));
            disconnect(videoRender,SIGNAL(repaintOver()),this,SLOT(trigger()));
        }

        processer = algprocesser;
        connect(this,SIGNAL(capOneFrame()),processer,SLOT(processImage()));
        if(videoRender){
            connect(processer,SIGNAL(processImageOver()),videoRender,SLOT(flushImage()));
            setCameraMode(curMode);
            //connect(videoRender,SIGNAL(repaintOver()),this,SLOT(trigger()));
        }

    }else{
        processer = algprocesser;
    }
}

VideoCapture::~VideoCapture()
{

    if(processer){
        disconnect(this,SIGNAL(capOneFrame()),processer,SLOT(processImage()));
        if(videoRender){
            disconnect(processer,SIGNAL(processImageOver()),videoRender,SLOT(flushImage()));
        }
    }

    if(thr.isRunning()){
        thr.quit();
        bStop = true;
        while(thr.isRunning());
    }

    INFO_DEBUG("~VideoCapture");
}
