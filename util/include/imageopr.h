#ifndef IMAGEOPR_H
#define IMAGEOPR_H
//#include "v4l2videodevice.h"

namespace ImageOpr {
typedef unsigned char BYTE;
void YUV422P_To_RGB24_init();
bool YUV422P_To_RGB24(unsigned char* src,char* buf, const int& width,const int& height);

bool YUVS422toRgb32(unsigned char* src,char* buf,const int& width,const int& height);
bool YUVS422toRgb24(unsigned char* src,char* buf,const int& width,const int& height);
bool YUVS422toRgb565(unsigned char* src,char* buf,const int& width,const int& height);
bool RGB32toRGB24(unsigned char* src,unsigned char* buf,const int& width,const int& height,const int &lineBytes);

//bool Mjpeg2Rgb24(PT_VideoBuf ptVideoBufIn,PT_VideoBuf ptVideoBufOut);

}



#if 0
class ImageOpr
{
public:
    ImageOpr();
};
#endif

#endif // IMAGEOPR_H
