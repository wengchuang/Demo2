#include "scrollrenderopr.h"

#include <QDebug>

ScrollRenderOpr::ScrollRenderOpr(const QString& name,QObject *parent):IRenderOpr(name,parent)
{
    view = new QGraphicsView;
    screen = new QGraphicsScene(view);
    view->setScene(screen);
    pixItem = NULL;
}
QWidget*ScrollRenderOpr::getRender()
{
    return view;
}

void ScrollRenderOpr::updateImage(DataItem* item)
{
    if(item){
        if(pixItem)
        {
            screen->removeItem(pixItem);
            delete pixItem;
            pixItem = 0;
        }

        pixItem = screen->addPixmap(QPixmap::fromImage(item->image));



        screen->setSceneRect(0, 0, item->image.width(), item->image.height());
        item = NULL;
    }

}

