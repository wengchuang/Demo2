#ifndef CERAMICDETECTALGO_H
#define CERAMICDETECTALGO_H
#ifdef _WIN32
#include "ialgorithm.h"
#include <detect.h>

class CeramicDetectAlgo : public IAlgorithm
{
public:
    explicit CeramicDetectAlgo(const QString& name,QObject *parent = 0);
    virtual ~CeramicDetectAlgo();
    bool   _algoInit();
    int algoExec(Mat& mat , void* args = NULL);
private:
    Detect               m_tileDetectObj;

};
#endif

#endif // CERAMICDETECTALGO_H
