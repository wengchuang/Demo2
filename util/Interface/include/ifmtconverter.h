#ifndef IFMTCONVERTER_H
#define IFMTCONVERTER_H
#if 0
#include <libavutil/pixfmt.h>
class IFmtConverter
{
public:
    virtual bool isSupport(const enum AVPixelFormat& inFmt, const enum AVPixelFormat& outFmt) = 0;
    virtual bool covert(void* inBuf,void* outBuf,const int& iInWidth,const int& iInHeight,const int&  iOutWidth = -1,const  int& iOutHeight = -1) = 0;
    virtual ~IFmtConverter(){}
};

#endif

#endif // IFMTCONVERTER_H
