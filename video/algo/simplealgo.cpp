#include "simplealgo.h"

SimpleAlgo::SimpleAlgo(const QString& name,QObject *parent) : IAlgorithm(name,parent)
{
}
int SimpleAlgo::algoExec(DataItem* item)
{
     circle(item->mat, Point(100,100), 30, Scalar( 255, 0, 0 ),0);
     return 0;
}
SimpleAlgo::~SimpleAlgo()
{

}
