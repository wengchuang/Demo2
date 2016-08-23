#ifndef SIMPLEALGO_H
#define SIMPLEALGO_H
#include "ialgorithm.h"
#include "SimpleAlgoArgs.h"

class SimpleAlgo : public IAlgorithm
{
public:
   explicit SimpleAlgo(const QString& name,QObject *parent = 0);
   virtual ~SimpleAlgo();
   int algoExec(Mat& mat , void* args = NULL);
   int constructRetMsg(void* buf, int& length);
private:
   AlgResData       resData;

};

#endif // SIMPLEALGO_H
