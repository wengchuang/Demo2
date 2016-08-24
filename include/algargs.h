#ifndef ALGARGS_H
#define ALGARGS_H
#include <opencv/cv.h>
#include <QImage>
using namespace cv;

typedef struct RecordItem{
    char  idention;
    unsigned int   status;
}T_RecordItem,*PT_RecordItem;
typedef struct AlgArgs{
    int args;
}T_AlgArgs,*PT_AlgArgs;
typedef struct AttachData{
    T_RecordItem   recordItem;
    T_AlgArgs      algArgs;
}T_AttachData,*PT_AttachData;

struct DataItem{
    QByteArray              data;
    QImage                  image;
    QByteArray              orgData;
    Mat                     orgMat;

    Mat                      mat;
    unsigned long           offset;
    T_AttachData            attachData;
    int                     videoMode;
    bool                    reverseRGB;

};

#endif // ALGARGS_H
