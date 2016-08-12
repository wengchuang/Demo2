#ifndef LIBDATADEF_H
#define LIBDATADEF_H
#include <QList>
#include "icaptureopr.h"
#include "irenderopr.h"
#include "ialgorithm.h"

typedef struct _LibInfo{
    QList<ICaptureOpr*>         capOprsList;
    QList<IAlgorithm*>          algosList;
    QList<IRenderOpr*>          renderOprsList;
}T_LibInfo,*PT_LibInfo;



#endif // LIBDATADEF_H
