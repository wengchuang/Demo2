#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QObject>
#include <QSettings>
#include "cameraparameterdef.h"

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

    QStringList       videoLibsNames();
    QString           getCfgProcesser();
    QString           getCfgRender();
private:
    explicit        Appconfig(QObject *parent = 0);
signals:

public slots:
private:
    static Appconfig*   instance;
    QSettings*          writer;

};

#endif // APPCONFIG_H
