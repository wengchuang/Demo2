#ifndef SCROLLRENDEROPR_H
#define SCROLLRENDEROPR_H


#include "irenderopr.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>

class ScrollRenderOpr : public IRenderOpr
{
public:
    explicit ScrollRenderOpr(const QString& name,QObject *parent = 0);
    QWidget* getRender();
    void updateImage(DataItem* item);
    virtual ~ScrollRenderOpr(){qDebug()<<"~ScrollRenderOpr()";}

private:
    QGraphicsScene*             screen;
    QGraphicsView*              view;
    QGraphicsPixmapItem*        pixItem;

};


#endif // SCROLLRENDER_H
