#ifndef IALGORITHM_H
#define IALGORITHM_H
#include <QObject>
#include <cv.h>
using namespace cv;
//#include "algargs.h"
class IAlgorithm : public QObject
{
    Q_OBJECT
public:
    explicit IAlgorithm(const QString& name,QObject *parent = 0):QObject(parent){this->setObjectName(name);}
    virtual ~IAlgorithm(){}
    virtual bool _algoInit(){return true;}
    virtual bool constructAlgoArgs(void* source,void* args){Q_UNUSED(source);Q_UNUSED(args); return true;}
    virtual int algoExec(Mat& mat , void* args = NULL) = 0;
    virtual int constructRetMsg(void* buf, int& length){Q_UNUSED(buf);Q_UNUSED(length);  return 0;}
    virtual bool _algoExit(){return true;}
};
#endif // IALGORITHM_H
