#ifndef DETECT_H
#define DETECT_H

#include <QtCore/qglobal.h>

#if defined(DETECT_LIBRARY)
#  define DETECTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DETECTSHARED_EXPORT Q_DECL_IMPORT
#endif

#define NOT_DETECT -2
#define Tile_NG -1


#include<cv.h>
#include<ml.h>
using namespace cv;

class DETECTSHARED_EXPORT Detect
{
 public:
    Detect();
    long Inintial(char*szSvmXmlPath);
    //设置感兴趣区域
    long SetRoi(long x,long y,long width,long height);
    //瓷砖检测
    long TileDetect(Mat&bgrImg);
 private:
    CvSVM m_SVM;
    Rect m_roiRect;
};



#endif // DETECT_H
