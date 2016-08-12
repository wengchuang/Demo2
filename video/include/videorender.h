#ifndef VIDEORENDER_H
#define VIDEORENDER_H

#include <QWidget>
#include "irenderopr.h"

#ifdef _WIN32
#include <Windows.h>
#endif

class VideoRender : public QWidget
{
    Q_OBJECT
public:
    explicit VideoRender(QWidget *parent = 0);
#ifdef _WIN32
    bool     getWinIdAndEvIdention(WId& id,UINT& evIdention);
#endif
    void    installRenderOpr(IRenderOpr* opr);

    virtual ~VideoRender();

signals:
#ifdef _WIN32
    void appEvent(int eventId);
#endif
    void     repaintOver();
protected:
#ifdef _WIN32
    bool winEvent(MSG *message, long *result);
#endif
protected:
    DataItem * item;

public slots:
    void flushImage();
public:
    IRenderOpr*         curOpr;

};

#endif // VIDEORENDER_H
