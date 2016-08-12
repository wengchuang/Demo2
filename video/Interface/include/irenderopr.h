#ifndef IRENDEROPR_H
#define IRENDEROPR_H
#include <QObject>
#include "algargs.h"
class IRenderOpr : public QObject
{
    Q_OBJECT
public:
    explicit IRenderOpr(const QString& name,QObject *parent = 0):QObject(parent){this->setObjectName(name);}
    virtual QWidget* getRender() = 0;
    virtual ~IRenderOpr(){}
    virtual void  updateImage(DataItem * item) = 0;

};

#endif // IRENDER_H
