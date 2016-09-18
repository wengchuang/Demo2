#include "simplealgo.h"
#include <QDebug>


#include<cv.h>
#include<highgui.h>
#include <appconfig.h>
using namespace cv;

static long FindContours(const Mat & bwImg, vector<vector<Point>>& contours)
{
    if (bwImg.empty()||bwImg.channels()!=1)
        return 0;
    contours.clear();
    Mat contourImg;
    bwImg.copyTo(contourImg);
    findContours(contourImg, contours,CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0));
    if (contours.empty())
        return 0;
    return 1;
}
static long FindMaxLenContour(vector<vector<Point>>& contours,long&index)
{
    if(contours.empty())
        return 0;
    long rectWidth =0;
    Rect tempRect;
    index =0;
    for (int i = 0; i<contours.size(); i++)
    {
        tempRect = boundingRect(contours[i]);
        if (tempRect.width>rectWidth)
        {
            rectWidth = tempRect.width;
            index = i;
        }
    }
    return 1;
}
static long DrawContour(Mat& srcImg,vector<Point>& contour,Scalar clr,int thickness,int linetype)
{
    if(contour.empty())
        return 0;
    vector<vector<Point>> contourLst;
    contourLst.push_back(contour);
    drawContours(srcImg,contourLst,0,clr,thickness,linetype);
    return 1;

}
static long GetStartEndRow(Mat&bwImg,long thVal,long &StartRow,long &EndRow)
{
    if(bwImg.empty())
        return 0;
    for(int i=1;i<bwImg.rows-1;i++)
    {
        if(countNonZero(bwImg.rowRange(i-1,i+2))>thVal)
        {
            StartRow = i;
            break;
        }
    }
    for(int i=1;i<bwImg.rows-1;i++)
    {
        if(countNonZero(bwImg.rowRange(bwImg.rows-i-2,bwImg.rows-i+1))>thVal)
        {
            EndRow = bwImg.rows-i-1;
            break;
        }
    }
    return 1;
}
static long GetStartEndCol(Mat&bwImg,long thVal,long &StartCol,long &EndCol)
{
    if(bwImg.empty())
        return 0;
    for(int i=0;i<bwImg.cols;i++)
    {
        if(countNonZero(bwImg.colRange(i,i+1))>thVal)
        {
            StartCol = i;
            break;
        }
    }
    for(int i=0;i<bwImg.cols;i++)
    {
        if(countNonZero(bwImg.colRange(bwImg.cols-i-1,bwImg.cols-i))>thVal)
        {
            EndCol = bwImg.cols-i-1;
            break;
        }
    }
    return 1;

}
static long GetExtendRect(Rect&srcRect,Rect&extendRect,long imgWidth,long imgHeight,long extendX,long extendY)
{
    long left = srcRect.x - extendX>0?srcRect.x - extendX:0;
    long right = srcRect.x+srcRect.width + extendX<imgWidth?srcRect.x+srcRect.width + extendX:imgWidth;
    long top =  srcRect.y - extendY>0?srcRect.y - extendY:0;
    long bottom = srcRect.y +srcRect.height + extendY<imgHeight?srcRect.y +srcRect.height + extendY:imgHeight;
    extendRect.x = left;
    extendRect.y = top;
    extendRect.width = right- left;
    extendRect.height = bottom - top;
    return 1;

}
static long CombineRect(long imgWidth,long imgHeight,vector<Rect>&srcRectLst,vector<Rect>&cmbRectLst)
{
    if(srcRectLst.empty())
        return 0;
    Mat maskImg = Mat::zeros(imgHeight,imgWidth,CV_8UC1);
    Rect extendRect;
    for(int i = 0; i<srcRectLst.size();i++ )
    {
        GetExtendRect(srcRectLst[i],extendRect,imgWidth,imgHeight,7,20);
        Mat(maskImg,extendRect).setTo(255);
    }
    vector<vector<Point>> contours;
    FindContours(maskImg,contours);
    for(int i=0;i<contours.size();i++)
    {
        extendRect = boundingRect(contours[i]);
        cmbRectLst.push_back(extendRect);

    }
    return 1;
}
static long ClasifyRect(Mat& rotateImg,const vector<Rect>&srcRectLst,vector<Rect>&blackRect,vector<Rect>&RedRect)
{
    blackRect.clear();
    RedRect.clear();
    Mat pBgrImg[3];
    for(int i=0;i<srcRectLst.size();i++)
    {
        if (srcRectLst[i].height*1.5<srcRectLst[i].width&&srcRectLst[i].width>50)
            RedRect.push_back(srcRectLst[i]);
        else if(srcRectLst[i].height>srcRectLst[i].width*1.5)
            blackRect.push_back(srcRectLst[i]);
        else
        {
            Mat subImg = Mat(rotateImg,srcRectLst[i]);
            split(subImg,pBgrImg);
            Mat RedImg = pBgrImg[2];
            Mat bwImg;
            threshold(RedImg,bwImg,150,255,CV_THRESH_BINARY);
            if(countNonZero(bwImg)>50)
                RedRect.push_back(srcRectLst[i]);
            else
                blackRect.push_back(srcRectLst[i]);
        }
    }
    return 1;
}
static float GetPntDistance(Point& pt1,Point& pt2)
{
    float d1 = sqrt(float((pt1.x-pt2.x)*(pt1.x-pt2.x)+ (pt1.y-pt2.y)*(pt1.y-pt2.y)));
    return d1;
}

static long GetRoiRect(Mat&grayImg,Rect& detectRect,Rect&roiRect)
{
    if (grayImg.empty())
        return 0;
    Mat canyImg = Mat::zeros(grayImg.size(),CV_8UC1);
    Canny(Mat(grayImg,detectRect), Mat(canyImg,detectRect), 60, 140);
    //imwrite("H:\\lujin\\canyImg0.jpg",canyImg);
    Mat kernelImg = (Mat_<uchar>(3,3) << 1, 1, 1, 1, 1, 1, 1, 1, 1);
    dilate(canyImg, canyImg, kernelImg,Point(-1,-1),3);
    vector<vector<Point>>contours;
    FindContours(canyImg,contours);
    if (contours.empty())
        return 0;
    //imwrite("H:\\lujin\\canyImg1.jpg",canyImg);
    FindContours(canyImg,contours);

    int rectWidth = 0;
    long index = 0;
    Rect tempRect;
    Mat markImg = Mat::zeros(grayImg.size(),CV_8UC1);
    for (int i = 0; i<contours.size(); i++)
    {
        tempRect = boundingRect(contours[i]);
        if (tempRect.width>tempRect.height*3&&tempRect.height<200)
        {
            rectangle(markImg,tempRect,Scalar(255),40);
        }
    }
    //imwrite("H:\\lujin\\markImg.jpg",markImg);
    FindContours(markImg,contours);
    FindMaxLenContour(contours,index);
    roiRect = boundingRect(contours[index]);
    Mat cutImg = Mat::zeros(grayImg.size(),CV_8UC1);
    canyImg.rowRange(roiRect.y,roiRect.y+roiRect.height).copyTo(cutImg.rowRange(roiRect.y,roiRect.y+roiRect.height));
    FindContours(cutImg,contours);
    FindMaxLenContour(contours,index);
    Rect cutRect = boundingRect(contours[index]);
    if(cutRect.width>roiRect.width)
        roiRect = cutRect;
    GetExtendRect(roiRect, roiRect, grayImg.cols, grayImg.rows,0,10);
    if (roiRect.width>grayImg.cols/2 && roiRect.height >30)
        return 1;
    return 0;
}
static long GetTileAngle(Mat&srcImg,Rect& roiRect,float &angle)
{
    vector<vector<Point>>contours;
    Mat grayImg;
    cvtColor(srcImg,grayImg,CV_BGR2GRAY);
    medianBlur(grayImg,grayImg,3);
    Mat canyImg = Mat::zeros(grayImg.size(),CV_8UC1);
    Canny(Mat(grayImg,roiRect), Mat(canyImg,roiRect), 50, 100);
    Mat kernelImg = (Mat_<uchar>(3,3) << 1, 1, 1, 1, 1, 1, 1, 1, 1);
    dilate(canyImg, canyImg, kernelImg,Point(-1,-1),1);
    //imwrite("H:\\lujin\\lineDetect.jpg",canyImg);
    vector<Vec4i> lines;
    HoughLinesP(canyImg, lines, 1, CV_PI/180, 30, 70, 15);
    float lineLen =0;
    long index = 0;
    Point sPnt,ePnt;
    for (size_t i=0; i<lines.size(); i++)
    {
        Vec4i tempLine = lines[i];
        Point pt1, pt2;
        pt1 = Point(tempLine[0], tempLine[1]);
        pt2 = Point(tempLine[2], tempLine[3]);
        if (abs(pt1.y - pt2.y)>abs(pt1.x - pt2.x)/2)
            continue;
        float tempLineLen =GetPntDistance(pt1,pt2);
        if(tempLineLen>lineLen)
        {
            index =i;
            lineLen = tempLineLen;
            sPnt = pt1;
            ePnt = pt2;
        }
    }

    //line(srcImg, sPnt, ePnt, Scalar(0,0,255), 3, CV_AA);
    angle = atan(float(sPnt.y - ePnt.y) / (sPnt.x - ePnt.x));
    angle *= 180 / CV_PI;
    return 1;
}


static long RemoveEdge(Mat& cannyImg,Mat& noEdgeImg)
{
    noEdgeImg = Mat::zeros(cannyImg.size(),CV_8UC1);
    Mat subImg,subImg2;
    long startRow=0;
    long endRow =0;
    long gap = 0;
    for(int i=0;i<8;i++)
    {
        subImg = cannyImg.colRange(cannyImg.cols*i/8,cannyImg.cols*(i+1)/8);
        subImg2 = noEdgeImg.colRange(cannyImg.cols*i/8,cannyImg.cols*(i+1)/8);
        GetStartEndRow(subImg,subImg.cols/2,startRow,endRow);
        startRow+=10;
        endRow-=10;
        gap = endRow - startRow;
        if(gap>15)
            endRow = startRow+15;
        if(endRow-startRow>5)
            subImg.rowRange(startRow,endRow).copyTo(subImg2.rowRange(startRow,endRow));
    }
    return 1;
}



static long DetectTileLine(Mat& srcImg,Rect& roiRect,vector<Rect>&blakRectLst,vector<Rect>&redRectLst)
{
    if(srcImg.empty())
        return 0;
    blakRectLst.clear();
    redRectLst.clear();
    Mat grayImg;
    cvtColor(srcImg,grayImg,CV_BGR2GRAY);
    long width = srcImg.cols;
    long height = srcImg.rows;
    Rect detectRct;
    detectRct.x = 50;
    detectRct.y = height/2 - 400;
    detectRct.width = width - 100;
    detectRct.height = 800;

    rectangle(srcImg,detectRct,Scalar(255,0,255),2);
    //获取瓷砖位置
    if (!GetRoiRect(grayImg,detectRct,roiRect))
        return 0;
    Mat roiImg = Mat(grayImg, roiRect);
    Mat otsuImg = Mat::zeros(grayImg.size(),CV_8UC1);
    threshold(roiImg, Mat(otsuImg, roiRect), 0, 255, CV_THRESH_OTSU);

    float rotAng = 0;
    //获取瓷砖偏移角度
    GetTileAngle(srcImg,roiRect,rotAng);

    Point2f center;
    center.x = float(width / 2.0 + 0.5);
    center.y = float(height / 2.0 + 0.5);
    //计算二维旋转的仿射变换矩阵
    Mat M = getRotationMatrix2D(center, rotAng, 1);
    Mat img_rotate,bgr_rotate;
    //变换图像，并用黑色填充其余值
    warpAffine(grayImg, img_rotate, M, grayImg.size());
    warpAffine(srcImg, bgr_rotate, M, srcImg.size());


    Mat bwImg = Mat::zeros(grayImg.size(),CV_8UC1);
    Canny(Mat(img_rotate,roiRect),Mat(bwImg,roiRect),60,160);
    Mat roiCanny = Mat(bwImg,roiRect);

    Mat Img1 = Mat(bwImg,roiRect);
    //imwrite("H:\\lujin\\Img1.jpg",Img1);
    Mat Img2;
    RemoveEdge(Img1,Img2);
    //imwrite("H:\\lujin\\Img2.jpg",Img2);

    //warpAffine(srcImg,srcImg, M,grayImg.size());
    threshold(Img2,Img2,100,255,CV_THRESH_BINARY);

    rectangle(srcImg,roiRect,Scalar(255,255,0),2);
    vector<vector<Point>>contours;
    FindContours(Img2,contours);
    Rect tempRect;
    vector<Rect> lineRect;
    vector<Rect> cmbRectLst;
    for(int i=0;i<contours.size();i++)
    {
        tempRect = boundingRect(contours[i]);
        if (tempRect.x<50 || tempRect.x + tempRect.width + 50>Img2.cols||(tempRect.width<6&&tempRect.height<6))
            continue;
        tempRect.x+=roiRect.x;
        tempRect.y+=roiRect.y;
        lineRect.push_back(tempRect);

    }

    CombineRect(width,height,lineRect,cmbRectLst);
    ClasifyRect(bgr_rotate,cmbRectLst,blakRectLst,redRectLst);
    for (int i = 0; i<blakRectLst.size(); i++)
    {
        rectangle(srcImg, blakRectLst[i], Scalar(255, 0, 0), 2);
    }
    for (int i = 0; i<redRectLst.size(); i++)
    {
        rectangle(srcImg, redRectLst[i], Scalar(0, 0, 255), 2);
    }
    return 1;
}





SimpleAlgo::SimpleAlgo(const QString& name,QObject *parent) : IAlgorithm(name,parent)
{

}

int SimpleAlgo::constructRetMsg(void* buf, int& length)
{
    length = sizeof(resData);
    memcpy(buf,&resData,length);
    return 0;
}
bool SimpleAlgo::isGood(void* pCompareData)
{
   bool ret = false;
   DebugLineCnt* pLineCnt = (DebugLineCnt*)pCompareData;
   if(pLineCnt->redLineCnt == resData.redLines)
   {
       if(pLineCnt->blackLineCnt == resData.blackLines){
           ret = true;
           if(pLineCnt->redLineCnt == 1){
#if 1
               int circle = resData.redRects[0].x - resData.roiRect.x + resData.redRects[0].width/2;
               int delta = resData.roiRect.width / (Appconfig::lineCfg.redIndexs.count());
               if(pLineCnt->redLinePos == (circle/delta)){
                   ret = true;
               }else{
                   ret = false;
               }
#endif
           }
       }
   }
   return ret;
}
int SimpleAlgo::algoExec(Mat& mat , void* args)
{
    Q_UNUSED(args);
    Rect roiRect;
    vector<Rect>blakRectLst;
    vector<Rect>redRectLst;
    DetectTileLine(mat,roiRect,blakRectLst,redRectLst);
    memset(&resData,0,sizeof(resData));
    resData.roiRect.x = roiRect.x;
    resData.roiRect.y = roiRect.y;
    resData.roiRect.width = roiRect.width;
    resData.roiRect.height = roiRect.height;

    resData.blackLines = blakRectLst.size();
    for(int i=0;i<blakRectLst.size();i++)
    {
        if(i < MAXRECTCNT){
            resData.blackRects[i].x         = blakRectLst[i].x;
            resData.blackRects[i].y         = blakRectLst[i].y;
            resData.blackRects[i].width     = blakRectLst[i].width;
            resData.blackRects[i].height    = blakRectLst[i].height;

        }
    }
    resData.redLines = redRectLst.size();
    for(int i=0;i<redRectLst.size();i++)
    {
        if(i < MAXRECTCNT){
            resData.redRects[i].x         = redRectLst[i].x;
            resData.redRects[i].y         = redRectLst[i].y;
            resData.redRects[i].width     = redRectLst[i].width;
            resData.redRects[i].height    = redRectLst[i].height;

        }
    }
    //imwrite("H:\\detectImg.jpg",mat);
    return 0;
}
SimpleAlgo::~SimpleAlgo()
{

}
