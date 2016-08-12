#include "videorender.h"
#include "datamanager.h"

VideoRender::VideoRender(QWidget *parent) :
    QWidget(parent)
{
    curOpr = NULL;
    item = NULL;
}
#ifdef _WIN32
#define MSG_CAMEVENT			(WM_APP + 1)

bool VideoRender::getWinIdAndEvIdention(WId& id,UINT& evIdention){
    bool ret = true;
    id = this->winId();
    evIdention = MSG_CAMEVENT;
    return ret;
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
    }

}
VideoRender::~VideoRender()
{
    qDebug("~VideoRender");
}
