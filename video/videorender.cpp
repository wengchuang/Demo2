#include "videorender.h"
#include "datamanager.h"
#include "handle/handlermanager.h"

VideoRender::VideoRender(QWidget *parent) :
    QWidget(parent)
{
    curOpr = NULL;
    item = NULL;
    frames = 0;
    connect(&timer,SIGNAL(timeout()),this,SLOT(dealTimerOut()));
    timer.start(1000);

}
#ifdef _WIN32
#define MSG_CAMEVENT			(WM_APP + 1)

bool VideoRender::getWinIdAndEvIdention(WId& id,UINT& evIdention){
    bool ret = true;
    id = this->winId();
    evIdention = MSG_CAMEVENT;
    return ret;
}

void VideoRender::dealTimerOut()
{
    int frameRate = frames;
    HandleMsg msg;
    msg.context = &frameRate;
    msg.contextLen = sizeof(frameRate);
    msg.handlerId = 1;
    msg.type = IHandler::HANDLE_UI;
    HandlerManager::getInstance()->constructHandleMsg(msg);

    frames = 0;

}
bool VideoRender::winEvent(MSG *message, long *result)
{
    if(message->message == MSG_CAMEVENT ){
        emit appEvent(message->wParam);
        return true;
    }
    return QWidget::winEvent(message, result);
}
#endif

void VideoRender::installRenderOpr(IRenderOpr* opr)
{
    if(opr){
        this->curOpr = opr;
    }
}

void VideoRender::flushImage()
{
    item = DataManager::getInstance()->getImage();
    if(item){
        this->curOpr->updateImage(item);
        emit repaintOver();
        frames++;

    }

}
VideoRender::~VideoRender()
{
    qDebug("~VideoRender");
}
