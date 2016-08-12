#include "ceramicdetectalgo.h"
#ifdef _WIN32
CeramicDetectAlgo::CeramicDetectAlgo(const QString& name,QObject *parent):IAlgorithm(name,parent)
{
}
bool  CeramicDetectAlgo::_algoInit()
{
    return (m_tileDetectObj.Inintial("../lib/tileDetect.xml")==0)?false:true;
}

int CeramicDetectAlgo::algoExec(DataItem* item)
{
    item->attachData.recordItem.idention = m_tileDetectObj.TileDetect(item->mat);
    return item->attachData.recordItem.idention;
}

CeramicDetectAlgo::~CeramicDetectAlgo()
{

}
#endif
