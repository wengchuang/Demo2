#include "simplerenderopr.h"

SimpleRenderOpr::SimpleRenderOpr(const QString& name,QObject *parent) : IRenderOpr(name,parent)
{
    label = new QLabel;
}
QWidget* SimpleRenderOpr::getRender()
{
    return label;
}
void SimpleRenderOpr::updateImage(DataItem * item)
{
    if(item){
        label->setPixmap(QPixmap::fromImage(item->image).scaled(label->width(),label->height()));
        item = NULL;
    }
}
