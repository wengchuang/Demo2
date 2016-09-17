#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QObject>
#include <QSettings>
#include <QMap>
#include "cameraparameterdef.h"

#define MAXCHANNELCNT 8

typedef struct{
    QList<int>     redIndexs;
    QMap<int,int>  blackmap;
}LineCfg;
typedef struct{
    int redLineCnt;
    int redLinePos;
    int blackLineCnt;
}DebugLineCnt;
class Appconfig : public QObject
{
    Q_OBJECT
public:
    static Appconfig* getInstance();
    bool              useVideoLib();
    bool              setExpoGainCfg(const QString& capName,const T_CameraCapbility& tCapbility);
    bool              setTempColorCfg(const QString& capName,const T_CameraCapbility& tCapbility);
    bool              setAdjColorCfg(const QString& capName,const T_CameraCapbility& tCapbility);
    bool              loadSaveCfg(const QString& capName,T_CameraCapbility& tCapbility);
    bool              getCameraMode(Camera::CameraMode& mode);
    bool              setCameraMode(Camera::CameraMode& mode);
    bool              setSnapPath(const QString& path);
    bool              getSnapPath(QString& path);
    bool              getSerialPortName(QString& portName);
    bool              setSerialPortName(const QString& portName);
    bool              getLineCfg(LineCfg& cfg);
    bool              setLineCfg(const LineCfg& cfg);
    bool              getVirtualCapoprDir(QString& loadPath);
    bool              getTimeOut(int& value);
    bool              setTimeOut(const int& value);
    bool              isDebugMode();

    QStringList       videoLibsNames();
    QString           getCfgProcesser();
    QString           getCfgRender();
    static            LineCfg lineCfg;
    static            DebugLineCnt dbgLineCnt;
    static            int outPutTimeOut;
    static            bool  bDebugMode;
private:
    explicit        Appconfig(QObject *parent = 0);
signals:
    void            lineCfgChanaged();

public slots:
private:
    static Appconfig*   instance;
    QSettings*          writer;

};

#endif // APPCONFIG_H
