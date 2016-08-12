#ifndef IALGORITHM_H
#define IALGORITHM_H
#include <QObject>
#include "algargs.h"
class IAlgorithm : public QObject
{
    Q_OBJECT
public:
    explicit IAlgorithm(const QString& name,QObject *parent = 0):QObject(parent){this->setObjectName(name);}
    virtual ~IAlgorithm(){}
    virtual bool _algoInit(){return true;}
    virtual int algoExec(DataItem* item) = 0;
    virtual bool _algoExit(){return true;}
};
#endif // IALGORITHM_H
