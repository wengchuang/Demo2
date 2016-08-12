#ifndef SIMPLERENDER_H
#define SIMPLERENDER_H

#include "irenderopr.h"
#include <QLabel>

class SimpleRenderOpr : public IRenderOpr
{
public:
    explicit SimpleRenderOpr(const QString& name,QObject *parent = 0);
    void  updateImage(DataItem * item);
    QWidget* getRender();
    virtual ~SimpleRenderOpr(){}
 private:
    QLabel*             label;


};

#endif // SIMPLERENDER_H
