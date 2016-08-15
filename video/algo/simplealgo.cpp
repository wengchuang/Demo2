#include "simplealgo.h"

SimpleAlgo::SimpleAlgo(const QString& name,QObject *parent) : IAlgorithm(name,parent)
{
}
int SimpleAlgo::algoExec( Mat& mat,void* args)
{
    Q_UNUSED(args);
     circle(mat, Point(100,100), 30, Scalar( 255, 0, 0 ),0);
     return 0;
}
SimpleAlgo::~SimpleAlgo()
{

}
