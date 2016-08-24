#ifndef VIRTUALCAPOPR_H
#define VIRTUALCAPOPR_H
#include "icaptureopr.h"
#include <QStringList>
typedef struct{
    int         iWidth;
    int        iHeight;
}ImgSize;
typedef struct{
    QStringList         files;
    QList<ImgSize>    imageSizes;
}fileImageDesc;

class VirtualCapOpr : public ICaptureOpr
{
public:
    explicit VirtualCapOpr(const QString& name,QObject *parent = 0);
    bool isSupport() ;
    bool openCapture(void) ;
    bool isOpened() ;
    void closeCapture();
    void getCameraCapbility(PT_CameraCapbility pCapbility);
    void setLoadPath(QString path);
    void setCameraMode(Camera::CameraMode mode){this->m_mode = mode;}



    bool setCameraArgs(Camera::CameraArgs argId,const QVariant& variant);
    bool getCameraArgs(Camera::CameraArgs argId,QVariant&value);
    void virtualTrigger(){
        if((m_mode == Camera::MODE_EXTERN) && bOpened)
            emit imageComming();
    }
    QString curFileName(){return fileImgDesc.files.at(curIndex);}

    virtual ~VirtualCapOpr();
protected:
    bool    grabFrame();
    void    trigger2();
    void    chanageReslution();

private:
    QString        loadPath;
    bool           bOpened;
    fileImageDesc  fileImgDesc;
    int curIndex;
    int m_mode;


};

#endif // VIRTUALCAPOPR_H
