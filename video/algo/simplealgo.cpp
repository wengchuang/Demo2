#include "simplealgo.h"
#include <QDebug>


#include<cv.h>
#include<highgui.h>
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
    for(int i=0;i<bwImg.rows;i++)
    {
        if(countNonZero(bwImg.rowRange(i,i+1))>thVal)
        {
            StartRow = i;
            break;
        }
    }
    for(int i=0;i<bwImg.rows;i++)
    {
        if(countNonZero(bwImg.rowRange(bwImg.rows-i-1,bwImg.rows-i))>thVal)
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
        GetExtendRect(srcRectLst[i],extendRect,imgWidth,imgHeight,10,20);
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
static long ClasifyRect(const vector<Rect>&srcRectLst,vector<Rect>&blackRect,vector<Rect>&RedRect)
{
    blackRect.clear();
    RedRect.clear();
    for(int i=0;i<srcRectLst.size();i++)
    {
        if(srcRectLst[i].height>srcRectLst[i].width)
            blackRect.push_back(srcRectLst[i]);
        else
            RedRect.push_back(srcRectLst[i]);
    }
    return 1;
}

static long GetRoiRect(Mat&grayImg,Rect&roiRect)
{
    if (grayImg.empty())
        return 0;
    Mat canyImg;
    Canny(grayImg, canyImg, 80, 160);

    uchar pData[9] = { 0, 0, 0, 1, 1, 1, 0, 0, 0 };
    Mat kernelImg = Mat(3, 3, CV_8UC1, pData);
    dilate(canyImg, canyImg, kernelImg);

    vector<vector<Point>>contours;
    FindContours(canyImg,contours);
    if (contours.empty())
        return 0;

    Rect tempRect;
    int rectWidth = 0;
    int index = 0;
    for (int i = 0; i<contours.size(); i++)
    {
        tempRect = boundingRect(contours[i]);
        if (tempRect.width>rectWidth)
        {
            rectWidth = tempRect.width;
            index = i;
        }
    }
    roiRect = boundingRect(contours[index]);
    if (roiRect.width < grayImg.cols * 3 / 4 || roiRect.height < 30)
        return 0;
    return 1;
}

static long GetTileAngle(Mat&srcImg,Mat&otsuImg,float &angle)
{
    vector<vector<Point>>contours;
    FindContours(otsuImg, contours);
    Rect tempRect;
    int rectWidth = 0;
    int index = 0;
    for (int i = 0; i<contours.size(); i++)
    {
        tempRect = boundingRect(contours[i]);
        if (tempRect.width>rectWidth)
        {
            rectWidth = tempRect.width;
            index = i;
        }
    }
    RotatedRect rtRect = minAreaRect(contours[index]);
    Point2f pntLst[4];
    rtRect.points(pntLst);

    for (int i = 0; i<4; i++)
    {
        line(srcImg, pntLst[i], pntLst[(i + 1) % 4], Scalar(0, 255, 0), 2);
    }
    if (fabs(pntLst[0].x - pntLst[1].x)>fabs(pntLst[0].x - pntLst[3].x))
        angle = atan((pntLst[0].y - pntLst[1].y) / (pntLst[0].x - pntLst[1].x));
    else
        angle = atan((pntLst[0].y - pntLst[3].y) / (pntLst[0].x - pntLst[3].x));
    angle *= 180 / CV_PI;
    return 1;
}

static long DetectTileLine(Mat& srcImg,Rect& roiRect,vector<Rect>&blakRectLst,vector<Rect>&redRectLst)
{
    long width = srcImg.cols;
    long height = srcImg.rows;
    Mat grayImg;
    cvtColor(srcImg,grayImg,CV_BGR2GRAY);
    if (!GetRoiRect(grayImg, roiRect))
        return 0;
    Mat roiImg = Mat(grayImg, roiRect);
    Mat otsuImg = Mat::zeros(grayImg.size(),CV_8UC1);
    threshold(roiImg, Mat(otsuImg, roiRect), 0, 255, CV_THRESH_OTSU);

    float rotAng = 0;
    GetTileAngle(srcImg, otsuImg, rotAng);

    Point2f center;
    center.x = float(width / 2.0 + 0.5);
    center.y = float(height / 2.0 + 0.5);
    //计算二维旋转的仿射变换矩阵
    Mat M = getRotationMatrix2D(center, rotAng, 1);
    Mat img_rotate;
    //变换图像，并用黑色填充其余值
    warpAffine(grayImg, img_rotate, M, grayImg.size());
    //threshold(img_rotate,otsuImg,0,255,CV_THRESH_OTSU);

    Mat bwImg = Mat::zeros(grayImg.size(),CV_8UC1);
    Mat bwImg2 = Mat::zeros(grayImg.size(),CV_8UC1);

    threshold(Mat(img_rotate,roiRect),Mat(bwImg,roiRect),30,255,CV_THRESH_OTSU);
    Canny(Mat(img_rotate,roiRect),Mat(bwImg2,roiRect),80,160);
    Mat roiCanny = Mat(bwImg2,roiRect);

    //imwrite("H:\\otsuImg.jpg",otsuImg);
    //imwrite("H:\\bwImg.jpg",bwImg);
    //imwrite("H:\\bwImg2.jpg",bwImg2);

    Mat Img1 = Mat(bwImg,roiRect);
    long startRow =0;
    long endRow = 0;
    long startCol = 0;
    long endCol = 0;
    GetStartEndRow(Img1,Img1.cols/2,startRow,endRow);
    GetStartEndCol(Img1,20,startCol,endCol);
    startRow+=10;
    endRow-=10;

    if(endRow - startRow <10)
           return 0;

    Mat Img2 = Mat::zeros(Img1.size(),CV_8UC1);
    roiCanny.rowRange(startRow,endRow).colRange(startCol,endCol).copyTo(Img2.rowRange(startRow,endRow).colRange(startCol,endCol));

    //imwrite("H:\\Img2.jpg",Img2);

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
        if(tempRect.x<50||tempRect.x+tempRect.width+20>Img2.cols)
            continue;
        tempRect.x+=roiRect.x;
        tempRect.y+=roiRect.y;
        lineRect.push_back(tempRect);

    }

    CombineRect(width,height,lineRect,cmbRectLst);
    ClasifyRect(cmbRectLst,blakRectLst,redRectLst);
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

        rectangle(mat,blakRectLst[i],Scalar(255,0,0),2);

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
        rectangle(mat,redRectLst[i],Scalar(0,0,255),2);
    }
    //imwrite("H:\\detectImg.jpg",mat);
    return 0;
}
SimpleAlgo::~SimpleAlgo()
{

}
