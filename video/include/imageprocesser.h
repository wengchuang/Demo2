#ifndef IMAGEPROCESSER_H
#define IMAGEPROCESSER_H

#include <QObject>
#include <QThread>
#include <opencv/cv.h>
#include "ialgorithm.h"
#include "algargs.h"
using namespace cv;

class ImageProcesser : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcesser(QObject *parent = 0);
    void installAlgo(IAlgorithm* algo);
    inline void snapPic(const QString& filePath){this->filePath =  filePath;this->isSnap = true;}
    virtual ~ImageProcesser();
signals:
    void processImageOver();
private:
    bool algoExit();
    bool algoInit();
    void saveSanptoFile(DataItem* item);
protected:
    virtual void recordResault(DataItem* item);
public slots:
    void processImage();
public:
    IAlgorithm*     algoItem;
private:
    QThread         thr;
    bool            isBusy;
    bool            isSnap;
    bool            isAlgoInit;
    QString         filePath;
};

#endif // IMAGEPROCESSER_H
