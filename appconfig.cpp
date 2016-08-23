#include "appconfig.h"
#include <QStringList>


Appconfig* Appconfig::instance = NULL;

Appconfig* Appconfig::getInstance()
{
    if(!instance){
        instance = new Appconfig;
    }
    return instance;
}

Appconfig::Appconfig(QObject *parent) :
    QObject(parent)
{

    writer = new QSettings("APPCONFIG",QSettings::IniFormat,this);
    if(writer->allKeys().count()==0){
        writer->setValue("LibConfig/useLib",QVariant(0));
        writer->setValue("LibConfig/libsNames",QVariant(QStringList()<<"123"<<"456"));
        writer->setValue("CurVideoCfg/algoName",QVariant("SimpleAlgo"));
        writer->setValue("CurVideoCfg/renderName",QVariant("ScrollRenderOpr"));
        writer->setValue("CurVideoCfg/virCapDir",QVariant("./virtualworkdir"));
        writer->setValue("Camera/mode",QVariant((int) (Camera::MODE_AUTO) ));
        writer->setValue("Camera/snappath","./");

        writer->setValue("LineCfg/RedLineIndexs",QVariant(QStringList()<<"1"<<"2"<<"3"));
        writer->setValue("LineCfg/BlackLineCtrlIndexs",QVariant(QStringList()<<"1"<<"2"<<"3"<<"4"<<"5"));
        writer->setValue("LineCfg/BlackChannelIndexs",QVariant(QStringList()<<"4"<<"5"<<"6"<<"7"<<"8"));

        writer->setValue("Device/SerialPortName","com4");
    }
}
bool  Appconfig::setExpoGainCfg(const QString& capName,const T_CameraCapbility& tCapbility)
{
    writer->setValue(capName+"/Expose",QVariant(tCapbility.sExposeDesc.dfExposeSaved));
    writer->setValue(capName+"/Gain",QVariant(tCapbility.sExposeDesc.dfGainSaved));
    return true;
}
bool Appconfig::setTempColorCfg(const QString& capName,const T_CameraCapbility& tCapbility)
{
    bool ret = false;
    if(tCapbility.sColorTemp.ibUsed){
        writer->setValue(capName+"/Temp",QVariant(tCapbility.sColorTemp.iTempSaved));
        writer->setValue(capName+"/Tint",QVariant(tCapbility.sColorTemp.iTintSaved));
        ret = true;
    }
    return ret;
}
bool Appconfig::getCameraMode(Camera::CameraMode& mode)
{
    mode =(Camera::CameraMode)writer->value("Camera/mode").toInt();
    return true;
}
bool  Appconfig::setCameraMode(Camera::CameraMode& mode)
{
    writer->setValue("Camera/mode",(int)(mode));
    return true;
}

bool Appconfig::setSnapPath(const QString& path)
{
    writer->setValue("Camera/snappath",path);
    return true;
}
bool Appconfig::getSnapPath(QString& path)
{
    path = writer->value("Camera/snappath").toString();
    return true;
}
bool Appconfig::getVirtualCapoprDir(QString& loadPath)
{
    loadPath = writer->value("CurVideoCfg/virCapDir").toString();
    return true;
}

bool Appconfig::getSerialPortName(QString& portName)
{
    portName = writer->value("Device/SerialPortName").toString();
    return true;
}
bool Appconfig::getLineCfg(LineCfg& cfg)
{
    QStringList list1 = writer->value("LineCfg/RedLineIndexs").toStringList();
    QString str1,str2;
    int i,Cnt;
    cfg.redIndexs.clear();
    foreach (str1, list1) {
        cfg.redIndexs.append(str1.toInt());
    }

    list1 = writer->value("LineCfg/BlackLineCtrlIndexs").toStringList();
    QStringList list2 = writer->value("LineCfg/BlackChannelIndexs").toStringList();

    Cnt = list1.count();
    cfg.blackmap.clear();
    for(i = 0; i < Cnt; i++){
        str1 = list1.at(i);
        str2 = list2.at(i);
        cfg.blackmap.insert(str1.toInt(),str2.toInt());
    }


    return true;
}

bool Appconfig::setSerialPortName(const QString& portName)
{
    writer->setValue("Device/SerialPortName",portName);
    return true;
}

bool Appconfig::loadSaveCfg(const QString& capName,T_CameraCapbility& tCapbility)
{
    QVariant variant;
    variant=writer->value(capName+"/Expose");
    if((variant.isValid()) && (!variant.isNull())){
        tCapbility.sExposeDesc.dfExposeSaved = variant.toDouble();
    }

    variant=writer->value(capName+"/Gain");
    if((variant.isValid()) && (!variant.isNull())){
        tCapbility.sExposeDesc.dfGainSaved = variant.toDouble();
    }


    variant=writer->value(capName+"/Temp");
    if((variant.isValid()) && (!variant.isNull())){
        tCapbility.sColorTemp.iTempSaved = variant.toInt();
    }
    variant=writer->value(capName+"/Tint");
    if((variant.isValid()) && (!variant.isNull())){
        tCapbility.sColorTemp.iTintSaved = variant.toInt();
    }

    variant=writer->value(capName+"/Shades");
    if((variant.isValid()) && (!variant.isNull())){
        tCapbility.sShadesRange.iSaved = variant.toInt();
    }

    variant=writer->value(capName+"/Saturation");
    if((variant.isValid()) && (!variant.isNull())){
        tCapbility.sSaturationRange.iSaved = variant.toInt();
    }

    variant=writer->value(capName+"/Brightness");
    if((variant.isValid()) && (!variant.isNull())){
        tCapbility.sBrightnessRange.iSaved = variant.toInt();
    }

    variant=writer->value(capName+"/Contrast");
    if((variant.isValid()) && (!variant.isNull())){
        tCapbility.sContrastRange.iSaved = variant.toInt();
    }

    variant=writer->value(capName+"/Gamma");
    if((variant.isValid()) && (!variant.isNull())){
        tCapbility.sGammaRange.iSaved = variant.toInt();
    }
    return true;

}

bool Appconfig::setAdjColorCfg(const QString& capName,const T_CameraCapbility& tCapbility)
{
    if(tCapbility.sShadesRange.ibUsed){
        writer->setValue(capName+"/Shades",QVariant(tCapbility.sShadesRange.iSaved));
    }
    if(tCapbility.sSaturationRange.ibUsed){
        writer->setValue(capName+"/Saturation",QVariant(tCapbility.sSaturationRange.iSaved));
    }
    if(tCapbility.sBrightnessRange.ibUsed){
        writer->setValue(capName+"/Brightness",QVariant(tCapbility.sBrightnessRange.iSaved));
    }
    if(tCapbility.sContrastRange.ibUsed){
        writer->setValue(capName+"/Contrast",QVariant(tCapbility.sContrastRange.iSaved));
    }
    if(tCapbility.sGammaRange.ibUsed){
        writer->setValue(capName+"/Gamma",QVariant(tCapbility.sGammaRange.iSaved));
    }
    return true;
}

bool  Appconfig::useVideoLib()
{
    return writer->value("LibConfig/useLib").toBool();
}
QStringList Appconfig::videoLibsNames()
{
    return writer->value("LibConfig/libsNames").toStringList();
}

QString  Appconfig::getCfgProcesser()
{
    return writer->value("CurVideoCfg/algoName").toString();
}
QString  Appconfig::getCfgRender()
{
    return writer->value("CurVideoCfg/renderName").toString();
}


