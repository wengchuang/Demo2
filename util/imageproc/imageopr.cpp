#include "imageopr.h"
#include "color.h"
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
//#include <jpeglib.h>

#include <QDebug>

namespace ImageOpr {
static bool isInit =false;

//使用整数运算(定点数运算)来代替浮点运算
const int csY_coeff_16 = 1.164383 * (1 << 16);
const int csU_blue_16 = 2.017232 * (1 << 16);
const int csU_green_16 = (-0.391762) * (1 << 16);
const int csV_green_16 = (-0.812968) * (1 << 16);
const int csV_red_16 = 1.596027 * (1 << 16);
//颜色查表
static BYTE _color_table[256 * 3];
static const BYTE* color_table = &_color_table[256];
//查表
static int Ym_tableEx[256];
static int Um_blue_tableEx[256];
static int Um_green_tableEx[256];
static int Vm_green_tableEx[256];
static int Vm_red_tableEx[256];
//颜色饱和函数
inline long border_color(long color) {
    if (color > 255)
        return 255;
    else if (color < 0)
        return 0;
    else
        return color;
}
//采用查找表进行计算时，必须运行的初始化函数
void YUV422P_To_RGB24_init() {
    int i;
    for (i = 0; i < 256 * 3; ++i)
        _color_table[i] = border_color(i - 256);
    for (i = 0; i < 256; ++i) {
        Ym_tableEx[i] = (csY_coeff_16 * (i - 16)) >> 16;
        Um_blue_tableEx[i] = (csU_blue_16 * (i - 128)) >> 16;
        Um_green_tableEx[i] = (csU_green_16 * (i - 128)) >> 16;
        Vm_green_tableEx[i] = (csV_green_16 * (i - 128)) >> 16;
        Vm_red_tableEx[i] = (csV_red_16 * (i - 128)) >> 16;
    }
}
inline void YUVToRGB24_Table(BYTE *p, const BYTE Y0, const BYTE Y1,
        const BYTE U, const BYTE V) {
    int Ye0 = Ym_tableEx[Y0];
    int Ye1 = Ym_tableEx[Y1];
    int Ue_blue = Um_blue_tableEx[U];
    int Ue_green = Um_green_tableEx[U];
    int Ve_green = Vm_green_tableEx[V];
    int Ve_red = Vm_red_tableEx[V];
    int UeVe_green = Ue_green + Ve_green;
    *p = color_table[(Ye0 + Ve_red)];
    *(p + 1) = color_table[(Ye0 + UeVe_green)];
    *(p + 2) = color_table[(Ye0 + Ue_blue)];
    *(p + 3) = color_table[(Ye1 + Ve_red)];
    *(p + 4) = color_table[(Ye1 + UeVe_green)];
    *(p + 5) = color_table[(Ye1 + Ue_blue)];
}
bool YUV422P_To_RGB24(unsigned char* src,char* buf, const int& width,const int& height)
{
    int y, x, x_uv;
    unsigned char* pY=src;
    unsigned char* pU= src + width*height;
    unsigned char* pV= src + width*height*3/2;
    BYTE* pDstLine = (BYTE*)buf;
    if ((width % 2) != 0 || (height % 2) != 0)
        return (-1);
    for (y = 0; y < height; ++y) {
        //DECODE_PlanarYUV211_Common_line(pDstLine, pY, pU, pV,width);
        for (x = 0; x < width; x += 2) {
            x_uv = x >> 1;
            YUVToRGB24_Table(&pDstLine[x * 3], pY[x], pY[x + 1], pU[x_uv],
                    pV[x_uv]);
        }
        pDstLine += width * 3; //RGB888
        pY += width; //YUV422
        if (y % 2 == 1) {
            pU += width / 2;
            pV += width / 2;
        }
    }
    return 0;

}
bool YUVS422toRgb32(unsigned char* src,char* buf,const int& width,const int& height)
{
        unsigned int i, size;
        unsigned char Y, Y1, U, V;
        unsigned char *buff = src;
        unsigned char *output_pt = (unsigned char *)buf;

        //unsigned int r, g, b;
        //unsigned int color;

        size = width * height /2;
        for (i = size; i > 0; i--) {
            /* bgr instead rgb ?? */
            Y = buff[0] ;
            U = buff[1] ;
            Y1 = buff[2];
            V = buff[3];
            buff += 4;

            *output_pt++ = R_FROMYV(Y,V);
            *output_pt++ = 0x0;

            *output_pt++ = B_FROMYU(Y,U);
            *output_pt++ = G_FROMYUV(Y,U,V);




            *output_pt++ = R_FROMYV(Y1,V);
            *output_pt++ = 0x0;
            *output_pt++ = B_FROMYU(Y1,U);
            *output_pt++ = G_FROMYUV(Y1,U,V);



        }

        return 0;
}


bool YUVS422toRgb24(unsigned char* src,char* buf,const int& width,const int& height)
{
    if(!isInit){
        initLut();
        isInit = true;
    }
        unsigned int i, size;
        unsigned char Y, Y1, U, V;
        unsigned char *buff = src;
        unsigned char *output_pt =(unsigned char *)buf;

        size = width * height /2;

        for (i = size; i > 0; i--) {
            /* bgr instead rgb ?? */
            Y = buff[0] ;
            U = buff[1] ;
            Y1 = buff[2];
            V = buff[3];
            buff += 4;

            *output_pt++ = B_FROMYU(Y,U);
            *output_pt++ = G_FROMYUV(Y,U,V);
            *output_pt++ = R_FROMYV(Y,V);


            *output_pt++ = B_FROMYU(Y1,U);
            *output_pt++ = G_FROMYUV(Y1,U,V);
            *output_pt++ = R_FROMYV(Y1,V);
        }

        return 0;
}
bool RGB32toRGB24(unsigned char* src,unsigned char* buf,const int& width,const int& height ,const int &lineBytes)
{
    int iHeight=0;
    int iWidth = 0;
    unsigned int * puiSrc;
    unsigned char*  dest = (unsigned char*)buf;

    for(iHeight = 0; iHeight < height ; iHeight++)
    {
        puiSrc = (unsigned int*)(iHeight*lineBytes+src);
        for(iWidth = 0; iWidth < width; iWidth++)
        {
            *dest++ = (*puiSrc)&0xff;
            *dest++ = (*puiSrc >>8)&0xff;
            *dest++ = (*puiSrc >>16)&0xff;
            puiSrc++;
        }
    }
    return true;
}
bool YUVS422toRgb565(unsigned char* src,char* buf,const int& width,const int& height)
{
        unsigned int i, size;
        unsigned char Y, Y1, U, V;
        unsigned char *buff = src;
        unsigned char *output_pt =(unsigned char *)buf;

        unsigned char r, g, b;
        unsigned int color;

        size = width * height /2;
        for (i = size; i > 0; i--) {
            /* bgr instead rgb ?? */
            Y = buff[0] ;
            U = buff[1] ;
            Y1 = buff[2];
            V = buff[3];
            buff += 4;
#if 1
            r = B_FROMYU(Y,U);
            g = G_FROMYUV(Y,U,V);
            b = R_FROMYV(Y,V);


            r  = r>>3;
            g  = g>>2;
            b  = b>>3;
#endif

            //r = 0x0;
            //g = 0x0;
            //b = 0xf8;
            color = (r<<11)|(g<<5)|(b);

            *output_pt++ = color&0xff;
            *output_pt++ = (color>>8)&0xff;

#if 1
            r = B_FROMYU(Y1,U);
            g = G_FROMYUV(Y1,U,V);
            b = R_FROMYV(Y1,V);

            r  = r>>3;
            g  = g>>2;
            b  = b>>3;
#endif
            //r = 0x0;
            //g = 0x0;
            //b = 0xf8;
            color = (r<<11)|(g<<5)|(b);

            *output_pt++ = color&0xff;
            *output_pt++ = (color>>8)&0xff;

        }

        return 0;
}



}




#if 0
ImageOpr::ImageOpr()
{
}
#endif
