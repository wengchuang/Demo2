#include "argssetting.h"
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QSpinBox>
#include <QComboBox>
#include <QFileDialog>
#include <QLineEdit>
#include "cameraparameterdef.h"

#include "debugredirect.h"
#include "appconfig.h"

#define  EXPORESUE      "exporesue"
#define  GAIN           "gain"
#define  COLORTMP       "colortmp"
#define  TINT           "Tint"
#define  SHADES         "shades"
#define  SATURATION     "saturation"
#define  BRIGHTNESS     "brightness"
#define  CONTRAST       "contrast"
#define  GAMA           "gama"

#define  EXPOGAIN       "expogain"
#define  COLORTINT      "colortint"
#define  COLORADJUST    "coloradjust"

ArgsSetting::ArgsSetting(QWidget *parent) :
    BaseDialog(parent)
{
    this->setWindowTitle(QString::fromLocal8Bit("参数设置"));
    createWidgets();
    this->setLayout(createLayout());

    UI_uninit();
    connect(VideoManager::getInstance(),SIGNAL(cameraCapbilityChanaged()),this,SLOT(UI_chanageCapbility()));
}
void ArgsSetting::showEvent(QShowEvent* ev)
{
    if(parent){
        QRect rect = parent->geometry();
        this->setGeometry(rect.x()+rect.width()/4,rect.y()+rect.height()/4,rect.width()/2,rect.height()/2);
    }
}

void ArgsSetting::editingFinished()
{
    QLineEdit* edit = (QLineEdit*)sender();
    QString str=valueMap.key(edit);
    if(str == EXPORESUE){
        sliderMap.value(str)->setValue((int)(edit->text().toDouble()*1000.0/(tCapbility.sExposeDesc.dfExpLineTime)));
    }else if(str == GAIN){
        sliderMap.value(str)->setValue((int)(edit->text().toDouble()));
    }else{
        sliderMap.value(str)->setValue((edit->text().toInt()));
    }
}

void ArgsSetting::silderValueChanaged(int value){
    QVariant variant;
    QSlider* tmpSlider =(QSlider*) sender();
    QString key = sliderMap.key(tmpSlider);
    QLineEdit* edit = valueMap.value(key);
    if(key == EXPORESUE){
       if(VideoManager::getInstance()->setCameraArgs(Camera::EXPORESUEID,value)){
           if(VideoManager::getInstance()->getCameraArgs(Camera::EXPORESUEID,variant)){
               edit->setText(QString::number(variant.toInt()*(tCapbility.sExposeDesc.dfExpLineTime)/1000.0));
           }
       }
    }else if(key == GAIN){
        if(VideoManager::getInstance()->setCameraArgs(Camera::GAINID,value)){
            if(VideoManager::getInstance()->getCameraArgs(Camera::GAINID,variant)){
                edit->setText(QString::number(variant.toInt()));
            }
        }
    }else if(key == COLORTMP){
        if(VideoManager::getInstance()->setCameraArgs(Camera::COLORTMPID,value)){
            if(VideoManager::getInstance()->getCameraArgs(Camera::COLORTMPID,variant)){
                edit->setText(QString::number(variant.toInt()));
            }
        }
    }else if(key == TINT){
        if(VideoManager::getInstance()->setCameraArgs(Camera::TINTID,value)){
            if(VideoManager::getInstance()->getCameraArgs(Camera::TINTID,variant)){
                edit->setText(QString::number(variant.toInt()));
            }
        }
    }else if(key == SHADES){
        if(VideoManager::getInstance()->setCameraArgs(Camera::SHADESID,value)){
            if(VideoManager::getInstance()->getCameraArgs(Camera::SHADESID,variant)){
                edit->setText(QString::number(variant.toInt()));
            }
        }
    }else if(key == SATURATION){
        if(VideoManager::getInstance()->setCameraArgs(Camera::SATURATIONID,value)){
            if(VideoManager::getInstance()->getCameraArgs(Camera::SATURATIONID,variant)){
                edit->setText(QString::number(variant.toInt()));
            }
        }
    }else if(key == BRIGHTNESS){
            if(VideoManager::getInstance()->setCameraArgs(Camera::BRIGHTNESSID,value)){
                if(VideoManager::getInstance()->getCameraArgs(Camera::BRIGHTNESSID,variant)){
                    edit->setText(QString::number(variant.toInt()));
             }
        }
    }else if(key == CONTRAST){
        if(VideoManager::getInstance()->setCameraArgs(Camera::CONTRASTID,value)){
            if(VideoManager::getInstance()->getCameraArgs(Camera::CONTRASTID,variant)){
                edit->setText(QString::number(variant.toInt()));
            }
        }
    }else if(key == GAMA){
        if(VideoManager::getInstance()->setCameraArgs(Camera::GAMAID,value)){
            if(VideoManager::getInstance()->getCameraArgs(Camera::GAMAID,variant)){
                edit->setText(QString::number(variant.toInt()));
            }
        }
    }

}
void ArgsSetting::UI_uninit()
{
    QLineEdit * lineEdit;
    QList<QLineEdit*> edits = valueMap.values();
    foreach(lineEdit,edits){
        lineEdit->clear();
    }

    QSlider* tmpSlider;
    QList<QSlider*> sliders = sliderMap.values();
    foreach(tmpSlider,sliders){
       disconnect(tmpSlider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));
       tmpSlider->setRange(0,0);

    }
    QPushButton* btn;
    QList<QPushButton*> btns = defaultBtns.values();
    foreach (btn, btns) {
        btn->setEnabled(false);
    }
    btns.clear();
    btns = ensureBtns.values();
    foreach(btn,btns){
        btn->setEnabled(false);
    }
    disconnect(resBox,SIGNAL(currentIndexChanged(int)),this,SLOT(comBoxIndexChanaged(int)));
}
void ArgsSetting::UI_chanageCapbility()
{

    QVariant variant;
    int min;
    int max;
    UI_uninit();
    VideoManager::getInstance()->getCaptureCapbility(&tCapbility);
    qDebug()<<"enable brightness"<<tCapbility.sBrightnessRange.ibUsed;
    resBox->clear();
    for(int i=0;i<tCapbility.curFmt->ImageSizeList.count();i++){
        resBox->addItem(QString::fromLocal8Bit(tCapbility.curFmt->ImageSizeList.at(i)->acDescription));
    }
    connect(resBox,SIGNAL(currentIndexChanged(int)),this,SLOT(comBoxIndexChanaged(int)));


    QSlider* tmpSlider = sliderMap.value(EXPORESUE);
    QLineEdit* edit = valueMap.value(EXPORESUE);
    min = int(tCapbility.sExposeDesc.dfExposeMin);
    max = int(tCapbility.sExposeDesc.dfExposeMax);

    tmpSlider->setEnabled(true);
    tmpSlider->setPageStep((max-min)/150);
    tmpSlider->setRange(min,max);
    if(VideoManager::getInstance()->getCameraArgs(Camera::EXPORESUEID,variant)){
        tmpSlider->setValue(variant.toInt());
        edit->setText(QString::number(variant.toInt()*(tCapbility.sExposeDesc.dfExpLineTime)/1000.0));
    }

    connect(tmpSlider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));


    tmpSlider = sliderMap.value(GAIN);
    edit = valueMap.value(GAIN);
    min = int(tCapbility.sExposeDesc.dfGainMin);
    max = int(tCapbility.sExposeDesc.dfGainMax);
    tmpSlider->setEnabled(true);
    tmpSlider->setPageStep((max-min)/5);
    tmpSlider->setRange(min,max);
    if(VideoManager::getInstance()->getCameraArgs(Camera::GAINID,variant)){
        tmpSlider->setValue(variant.toInt());
        edit->setText(QString::number(variant.toInt()));
    }
    defaultBtns.value(EXPOGAIN)->setEnabled(true);
    ensureBtns.value(EXPOGAIN)->setEnabled(true);
    connect(tmpSlider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));

    if(tCapbility.sColorTemp.ibUsed){
        tmpSlider = sliderMap.value(COLORTMP);
        edit = valueMap.value(COLORTMP);
        min = int(tCapbility.sColorTemp.iTempMin);
        max = int(tCapbility.sColorTemp.iTempMax);
        tmpSlider->setEnabled(true);
        tmpSlider->setPageStep((max-min)/10);
        tmpSlider->setRange(min,max);
        if(VideoManager::getInstance()->getCameraArgs(Camera::COLORTMPID,variant)){
            tmpSlider->setValue(variant.toInt());
            edit->setText(QString::number(variant.toInt()));
        }
        connect(tmpSlider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));


        tmpSlider = sliderMap.value(TINT);
        edit = valueMap.value(TINT);
        min = int(tCapbility.sColorTemp.iTintMin);
        max = int(tCapbility.sColorTemp.iTintMax);
        tmpSlider->setEnabled(true);
        tmpSlider->setPageStep((max-min)/10);
        tmpSlider->setRange(min,max);
        if(VideoManager::getInstance()->getCameraArgs(Camera::TINTID,variant)){
            tmpSlider->setValue(variant.toInt());
            edit->setText(QString::number(variant.toInt()));
        }
        connect(tmpSlider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));
        defaultBtns.value(COLORTINT)->setEnabled(true);
        ensureBtns.value(COLORTINT)->setEnabled(true);
    }

    if(tCapbility.sShadesRange.ibUsed){
        tmpSlider = sliderMap.value(SHADES);
        edit = valueMap.value(SHADES);
        min = int(tCapbility.sShadesRange.iMin);
        max = int(tCapbility.sShadesRange.iMax);
        tmpSlider->setEnabled(true);
        tmpSlider->setPageStep((max-min)/10);
        tmpSlider->setRange(min,max);
        if(VideoManager::getInstance()->getCameraArgs(Camera::SHADESID,variant)){
            tmpSlider->setValue(variant.toInt());
            edit->setText(QString::number(variant.toInt()));
        }
        connect(tmpSlider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));
        defaultBtns.value(COLORTINT)->setEnabled(true);
        ensureBtns.value(COLORTINT)->setEnabled(true);

    }

    if(tCapbility.sSaturationRange.ibUsed){
        tmpSlider = sliderMap.value(SATURATION);
        edit = valueMap.value(SATURATION);
        min = int(tCapbility.sSaturationRange.iMin);
        max = int(tCapbility.sSaturationRange.iMax);
        tmpSlider->setEnabled(true);
        tmpSlider->setPageStep((max-min)/10);
        tmpSlider->setRange(min,max);
        if(VideoManager::getInstance()->getCameraArgs(Camera::SATURATIONID,variant)){
            tmpSlider->setValue(variant.toInt());
            edit->setText(QString::number(variant.toInt()));
        }
        connect(tmpSlider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));

    }

    if(tCapbility.sBrightnessRange.ibUsed){
        tmpSlider = sliderMap.value(BRIGHTNESS);
        edit = valueMap.value(BRIGHTNESS);
        min = int(tCapbility.sBrightnessRange.iMin);
        max = int(tCapbility.sBrightnessRange.iMax);
        tmpSlider->setEnabled(true);
        tmpSlider->setPageStep((max-min)/10);
        tmpSlider->setRange(min,max);
        if(VideoManager::getInstance()->getCameraArgs(Camera::BRIGHTNESSID,variant)){
            tmpSlider->setValue(variant.toInt());
            edit->setText(QString::number(variant.toInt()));
        }
        connect(tmpSlider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));

    }

    if(tCapbility.sContrastRange.ibUsed){
        tmpSlider = sliderMap.value(CONTRAST);
        edit = valueMap.value(CONTRAST);
        min = int(tCapbility.sContrastRange.iMin);
        max = int(tCapbility.sContrastRange.iMax);
        tmpSlider->setEnabled(true);
        tmpSlider->setPageStep((max-min)/10);
        tmpSlider->setRange(min,max);
        if(VideoManager::getInstance()->getCameraArgs(Camera::CONTRASTID,variant)){
            tmpSlider->setValue(variant.toInt());
            edit->setText(QString::number(variant.toInt()));
        }
        connect(tmpSlider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));

    }

    if(tCapbility.sGammaRange.ibUsed){
        tmpSlider = sliderMap.value(GAMA);
        edit = valueMap.value(GAMA);
        min = int(tCapbility.sGammaRange.iMin);
        max = int(tCapbility.sGammaRange.iMax);
        tmpSlider->setEnabled(true);
        tmpSlider->setPageStep((max-min)/10);
        tmpSlider->setRange(min,max);
        if(VideoManager::getInstance()->getCameraArgs(Camera::GAMAID,variant)){
            tmpSlider->setValue(variant.toInt());
            edit->setText(QString::number(variant.toInt()));
        }
        connect(tmpSlider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));


    }
    defaultBtns.value(COLORADJUST)->setEnabled(true);
    ensureBtns.value(COLORADJUST)->setEnabled(true);

}
QLayout* ArgsSetting::createResoultionLayout()
{
    QVBoxLayout* mainLay = new QVBoxLayout;
    QHBoxLayout* hLay = new QHBoxLayout;
    hLay->setAlignment(Qt::AlignCenter);
    QHBoxLayout* inhLay = new QHBoxLayout;
    inhLay->setAlignment(Qt::AlignCenter);


    QGroupBox*gbbox = new QGroupBox(QString::fromLocal8Bit("图像分辨率"));

    resBox = new QComboBox;
    resBox->setFixedWidth(300);
    inhLay->addWidget(resBox);


    gbbox->setLayout(inhLay);
    hLay->addWidget(gbbox);
    setGroupBoxStyle(gbbox);
    mainLay->addWidget(gbbox);
    return mainLay;
}
QLayout* ArgsSetting::createColorLayout()
{
    QVBoxLayout* mainLay = new QVBoxLayout;
    QHBoxLayout* hLay = new QHBoxLayout;
    hLay->setAlignment(Qt::AlignCenter);


    QGroupBox*gbbox = new QGroupBox(QString::fromLocal8Bit("曝光与增益"));
    QVBoxLayout* innerVBox =new QVBoxLayout;
    innerVBox->addSpacing(10);
    QHBoxLayout* innerHBox = new QHBoxLayout;
    QLabel* lab =  new QLabel(QString::fromLocal8Bit("曝光时间"));
    innerHBox->addWidget(lab);
    innerHBox->addSpacing(1);
    QLineEdit* lineEdit = new QLineEdit;
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setStyleSheet("color:#000000;border:2px solid #A8B985;background-color:white;border-radius:2px;");
    lineEdit->setFixedWidth(100);

    innerHBox->addWidget(lineEdit);
    valueMap.insert(EXPORESUE,lineEdit);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    QSlider*slider  =new   QSlider(Qt::Horizontal);

    innerHBox->addWidget(slider);
    sliderMap.insert(EXPORESUE,slider);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    lab =  new QLabel(QString::fromLocal8Bit("增益"));
    innerHBox->addWidget(lab);
    innerHBox->addSpacing(1);

    lineEdit = new QLineEdit;
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setStyleSheet("color:#000000;border:2px solid #A8B985;background-color:white;border-radius:2px;");
    lineEdit->setFixedWidth(100);
    innerHBox->addWidget(lineEdit);
    valueMap.insert(GAIN,lineEdit);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    slider  =new   QSlider(Qt::Horizontal);

    slider->setPageStep(1000);
    innerHBox->addWidget(slider);
    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));
    sliderMap.insert(GAIN,slider);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    innerHBox->setAlignment(Qt::AlignCenter);
    QPushButton* btn = new QPushButton(QString::fromLocal8Bit("默认值"));
    setPushBtnStyle(btn);
    defaultBtns.insert(EXPOGAIN,btn);
    connect(btn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    innerHBox->addWidget(btn);

    btn = new QPushButton(QString::fromLocal8Bit("确定"));
    setPushBtnStyle(btn);
    connect(btn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    ensureBtns.insert(EXPOGAIN,btn);
    innerHBox->addWidget(btn);
    innerVBox->addLayout(innerHBox);
    gbbox->setLayout(innerVBox);
    hLay->addWidget(gbbox);
    setGroupBoxStyle(gbbox);

    gbbox = new QGroupBox(QString::fromLocal8Bit("白平衡"));
    innerVBox =new QVBoxLayout;
    innerVBox->addSpacing(10);
    innerHBox = new QHBoxLayout;
    lab =  new QLabel(QString::fromLocal8Bit("色温"));
    innerHBox->addWidget(lab);
    innerHBox->addSpacing(1);
    lineEdit = new QLineEdit;
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setStyleSheet("color:#000000;border:2px solid #A8B985;background-color:white;border-radius:2px;");
    lineEdit->setFixedWidth(100);
    innerHBox->addWidget(lineEdit);
    valueMap.insert(COLORTMP,lineEdit);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    slider  =new   QSlider(Qt::Horizontal);

    slider->setPageStep(1000);
    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));
    innerHBox->addWidget(slider);
    sliderMap.insert(COLORTMP,slider);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    lab =  new QLabel(QString::fromLocal8Bit("Tint"));
    innerHBox->addWidget(lab);
    innerHBox->addSpacing(1);
    lineEdit = new QLineEdit;
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setStyleSheet("color:#000000;border:2px solid #A8B985;background-color:white;border-radius:2px;");
    lineEdit->setFixedWidth(100);
    innerHBox->addWidget(lineEdit);
    valueMap.insert(TINT,lineEdit);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    slider  =new   QSlider(Qt::Horizontal);

    slider->setPageStep(100);
    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));
    innerHBox->addWidget(slider);
    sliderMap.insert(TINT,slider);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    innerHBox->setAlignment(Qt::AlignCenter);
    btn = new QPushButton(QString::fromLocal8Bit("默认值"));
    connect(btn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    defaultBtns.insert(COLORTINT,btn);
    setPushBtnStyle(btn);
    innerHBox->addWidget(btn);

    btn = new QPushButton(QString::fromLocal8Bit("确定"));
    connect(btn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    setPushBtnStyle(btn);
    ensureBtns.insert(COLORTINT,btn);
    innerHBox->addWidget(btn);

    innerVBox->addLayout(innerHBox);
    gbbox->setLayout(innerVBox);
    hLay->addWidget(gbbox);
    setGroupBoxStyle(gbbox);





    gbbox = new QGroupBox(QString::fromLocal8Bit("颜色调整"));
    innerVBox =new QVBoxLayout;
    innerVBox->addSpacing(10);
    innerHBox = new QHBoxLayout;
    lab =  new QLabel(QString::fromLocal8Bit("色度"));
    innerHBox->addWidget(lab);
    innerHBox->addSpacing(1);
    lineEdit = new QLineEdit;
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setStyleSheet("color:#000000;border:2px solid #A8B985;background-color:white;border-radius:2px;");
    lineEdit->setFixedWidth(100);
    innerHBox->addWidget(lineEdit);
    valueMap.insert(SHADES,lineEdit);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    slider  =new   QSlider(Qt::Horizontal);

    slider->setPageStep(10);
    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));
    innerHBox->addWidget(slider);
    sliderMap.insert(SHADES,slider);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    lab =  new QLabel(QString::fromLocal8Bit("饱和度"));
    innerHBox->addWidget(lab);
    innerHBox->addSpacing(1);
    lineEdit = new QLineEdit;
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setStyleSheet("color:#000000;border:2px solid #A8B985;background-color:white;border-radius:2px;");
    lineEdit->setFixedWidth(100);
    innerHBox->addWidget(lineEdit);
    valueMap.insert(SATURATION,lineEdit);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    slider  =new   QSlider(Qt::Horizontal);

    slider->setPageStep(10);
    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));
    innerHBox->addWidget(slider);
    sliderMap.insert(SATURATION,slider);
    innerVBox->addLayout(innerHBox);



    innerHBox = new QHBoxLayout;
    lab =  new QLabel(QString::fromLocal8Bit("亮度"));
    innerHBox->addWidget(lab);
    innerHBox->addSpacing(1);
    lineEdit = new QLineEdit;
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setStyleSheet("color:#000000;border:2px solid #A8B985;background-color:white;border-radius:2px;");
    lineEdit->setFixedWidth(100);
    innerHBox->addWidget(lineEdit);
    valueMap.insert(BRIGHTNESS,lineEdit);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    slider  =new   QSlider(Qt::Horizontal);

    slider->setPageStep(10);
    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));
    innerHBox->addWidget(slider);
    sliderMap.insert(BRIGHTNESS,slider);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    lab =  new QLabel(QString::fromLocal8Bit("对比度"));
    innerHBox->addWidget(lab);
    innerHBox->addSpacing(1);
    lineEdit = new QLineEdit;
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setStyleSheet("color:#000000;border:2px solid #A8B985;background-color:white;border-radius:2px;");
    lineEdit->setFixedWidth(100);
    innerHBox->addWidget(lineEdit);
    valueMap.insert(CONTRAST,lineEdit);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    slider  =new   QSlider(Qt::Horizontal);

    slider->setPageStep(10);
    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));
    innerHBox->addWidget(slider);
    sliderMap.insert(CONTRAST,slider);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    lab =  new QLabel(QString::fromLocal8Bit("伽马"));
    innerHBox->addWidget(lab);
    innerHBox->addSpacing(1);
    lineEdit = new QLineEdit;
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setStyleSheet("color:#000000;border:2px solid #A8B985;background-color:white;border-radius:2px;");
    lineEdit->setFixedWidth(100);
    innerHBox->addWidget(lineEdit);
    valueMap.insert(GAMA,lineEdit);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    slider  =new   QSlider(Qt::Horizontal);

    slider->setPageStep(10);
    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(silderValueChanaged(int)));
    innerHBox->addWidget(slider);
    sliderMap.insert(GAMA,slider);
    innerVBox->addLayout(innerHBox);

    innerHBox = new QHBoxLayout;
    innerHBox->setAlignment(Qt::AlignCenter);
    btn = new QPushButton(QString::fromLocal8Bit("默认值"));
    connect(btn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    defaultBtns.insert(COLORADJUST,btn);
    setPushBtnStyle(btn);
    innerHBox->addWidget(btn);

    btn = new QPushButton(QString::fromLocal8Bit("确定"));
    connect(btn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    setPushBtnStyle(btn);
    ensureBtns.insert(COLORADJUST,btn);
    innerHBox->addWidget(btn);

    innerVBox->addLayout(innerHBox);
    gbbox->setLayout(innerVBox);
    hLay->addWidget(gbbox);
    setGroupBoxStyle(gbbox);
    mainLay->addLayout(hLay);

    QList<QLineEdit*> edits = valueMap.values();
    foreach(lineEdit,edits){
        connect(lineEdit,SIGNAL(returnPressed()),this,SLOT(editingFinished()));
        lineEdit->clear();
    }

    return mainLay;
}
QWidget* ArgsSetting::createAutoWidget()
{
    autoWidget = new QWidget;
    QVBoxLayout* mainLay = new QVBoxLayout;
    mainLay->setAlignment(Qt::AlignCenter);

    QHBoxLayout* hLay = new QHBoxLayout;
    hLay->setAlignment(Qt::AlignCenter);

    QLabel* lab = new QLabel(QString::fromLocal8Bit("抓拍路径："));
    lab->setFixedSize(120,40);
    lab->setAlignment(Qt::AlignRight);
    hLay->addWidget(lab);

    pathEdit = new QLineEdit;
    pathEdit->setStyleSheet("color:#000000;border:2px solid #A8B985;background-color:white;border-radius:2px;");
    pathEdit->setFixedSize(200,40);
    pathEdit->setEnabled(false);
    hLay->addWidget(pathEdit);

    pathBtn = new QPushButton(QString::fromLocal8Bit("设置"));
    connect(pathBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    setPushBtnStyle(pathBtn);
    hLay->addWidget(pathBtn);
    mainLay->addLayout(hLay);

    hLay = new QHBoxLayout;
    hLay->setAlignment(Qt::AlignCenter);
    snapBtn = new QPushButton(QString::fromLocal8Bit("抓拍"));
    connect(snapBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    setPushBtnStyle(snapBtn);
    hLay->addWidget(snapBtn);
    mainLay->addLayout(hLay);
    autoWidget->setLayout(mainLay);
    return autoWidget;

}
QWidget* ArgsSetting::createExternTriggerWidget()
{
    externTriggerWidget = new QWidget;

    QVBoxLayout* mainLay = new QVBoxLayout;
    mainLay->setAlignment(Qt::AlignCenter);
    QHBoxLayout* hLay = new QHBoxLayout;
    hLay->setAlignment(Qt::AlignCenter);
    QLabel* lab = new QLabel(QString::fromLocal8Bit("IO轮询时间间隔："));
    lab->setFixedWidth(120);
    lab->setAlignment(Qt::AlignRight);
    hLay->addWidget(lab);
    hLay->addSpacing(200);
    ioPullTime = new QSpinBox;
    ioPullTime->setFixedHeight(40);
    ioPullTime->setSingleStep(10);
    ioPullTime->setRange(30,1000);

    ioPullTime->setSuffix("ms");
    ioPullTime->setAlignment(Qt::AlignCenter);
    ioPullTime->setFixedWidth(100);
    //sbBox->setEnabled(false);
    hLay->addWidget(ioPullTime);

    hLay->addSpacing(200);
    IOPullBtn = new QPushButton(QString::fromLocal8Bit("确定"));
    connect(IOPullBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    setPushBtnStyle(IOPullBtn);
    hLay->addWidget(IOPullBtn);
    mainLay->addLayout(hLay);


    externTriggerWidget->setLayout(mainLay);

    return externTriggerWidget;
}
void ArgsSetting::comBoxIndexChanaged(int index)
{
    QComboBox* tmpBox = (QComboBox*)sender();
    if(tmpBox == resBox){
            VideoManager::getInstance()->setCameraArgs(Camera::RESOLUTION,index);
    }
}

void ArgsSetting::radioBtnClicked()
{
    QRadioButton* raBtn = (QRadioButton*) sender();
    if(raBtn == autoRadioBtn){
        if(autoRadioBtn->isChecked()){
            curWidget->hide();
            tmpLay->removeWidget(curWidget);
            tmpLay->addWidget(autoWidget);
            curWidget = autoWidget;
            curWidget->show();

        }
    }else{
        if(externRadioBtn->isChecked()){
            curWidget->hide();
            tmpLay->removeWidget(curWidget);
            tmpLay->addWidget(externTriggerWidget);
            curWidget = externTriggerWidget;
            curWidget->show();

        }

    }
}
QLayout* ArgsSetting::createOutTimeLayout()
{
    QVBoxLayout* mainLay = new QVBoxLayout;
    QGroupBox*gbbox = new QGroupBox(QString::fromLocal8Bit("超时设置"));

    QVBoxLayout* inVLay = new QVBoxLayout;
    inVLay->addSpacing(20);
    QHBoxLayout* inHLay = new QHBoxLayout;
    //inHLay->setAlignment(Qt::AlignCenter);
    QLabel* lab = new QLabel(QString::fromLocal8Bit("IO输出超时："));
    lab->setFixedWidth(100);
    QSpinBox* sbBox = new QSpinBox;
    sbBox->setSingleStep(20);
    sbBox->setRange(30,500);
    sbBox->setSuffix("ms");
    sbBox->setFixedWidth(60);
    //sbBox->setEnabled(false);

    inHLay->addWidget(lab);
    inHLay->addWidget(sbBox);
    inVLay->addLayout(inHLay);

    inHLay = new QHBoxLayout;
    inHLay->addStretch(1);
    QPushButton* btn = new QPushButton(QString::fromLocal8Bit("确定"));
    setPushBtnStyle(btn);
    inHLay->addWidget(btn);
    inVLay->addLayout(inHLay);

    gbbox->setLayout(inVLay);

    setGroupBoxStyle(gbbox);
    mainLay->addWidget(gbbox);
    return mainLay;
}
QLayout*ArgsSetting::createSerialLayout()
{
    QVBoxLayout* mainLay = new QVBoxLayout;
    QGroupBox*gbbox = new QGroupBox(QString::fromLocal8Bit("串口设置"));

    QVBoxLayout* inVLay = new QVBoxLayout;
    inVLay->addSpacing(20);
    QHBoxLayout* inHLay = new QHBoxLayout;
    //inHLay->setAlignment(Qt::AlignCenter);
    QLabel* lab = new QLabel(QString::fromLocal8Bit("端口号："));
    lab->setFixedWidth(50);
    portNameComBox = new QComboBox;
    QStringList comList;
    comList<<"com1"<<"com2"<<"com3"<<"com4"<<"com5"<<"com6"<<"com7"<<"com8"
                                      <<"com9";
    portNameComBox->addItems(comList);
    portNameComBox->setFixedWidth(200);
    inHLay->addWidget(lab);
    inHLay->addWidget(portNameComBox);
    inVLay->addLayout(inHLay);




    inHLay = new QHBoxLayout;
    inHLay->addStretch(1);
    portEnsureBtn = new QPushButton(QString::fromLocal8Bit("确定"));
    setPushBtnStyle(portEnsureBtn);
    inHLay->addWidget(portEnsureBtn);
    connect(portEnsureBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    inVLay->addLayout(inHLay);

    gbbox->setLayout(inVLay);

    setGroupBoxStyle(gbbox);
    mainLay->addWidget(gbbox);
    return mainLay;
}
QLayout*ArgsSetting::createModeSetLayout()
{
    QVBoxLayout* mainLay = new QVBoxLayout;
    QGroupBox*gbbox = new QGroupBox(QString::fromLocal8Bit("相机工作模式设置"));

    QVBoxLayout* inVLay = new QVBoxLayout;
    inVLay->addSpacing(20);
    QHBoxLayout* inHLay = new QHBoxLayout;
    autoRadioBtn = new QRadioButton(QString::fromLocal8Bit("自动触发模式"));
    connect(autoRadioBtn,SIGNAL(clicked()),this,SLOT(radioBtnClicked()));
    inHLay->addWidget(autoRadioBtn);
    externRadioBtn = new QRadioButton(QString::fromLocal8Bit("外部触发模式"));
    connect(externRadioBtn,SIGNAL(clicked()),this,SLOT(radioBtnClicked()));
    inHLay->addWidget(externRadioBtn);
    inVLay->addLayout(inHLay);



    tmpLay = new QVBoxLayout;
    createExternTriggerWidget();
    createAutoWidget();

    autoRadioBtn->setChecked(true);
    curWidget = autoWidget;

    tmpLay->addWidget(curWidget);
    inVLay->addLayout(tmpLay);

    gbbox->setLayout(inVLay);
    setGroupBoxStyle(gbbox);
    mainLay->addWidget(gbbox);
    return mainLay;
}
void ArgsSetting::createWidgets()
{
    QVBoxLayout* vLay = new QVBoxLayout;
    cameraArgsWidget = new QWidget;
    vLay->setAlignment(Qt::AlignCenter);
    vLay->addLayout(createColorLayout());
    vLay->addLayout(createResoultionLayout());
    vLay->addLayout(createModeSetLayout());
    cameraArgsWidget->setLayout(vLay);

    IOArgsWidget = new QWidget();
    vLay = new QVBoxLayout;
    vLay->setAlignment(Qt::AlignCenter);
    vLay->addLayout(createSerialLayout());
    vLay->addLayout(createOutTimeLayout());
    IOArgsWidget->setLayout(vLay);

}
void ArgsSetting::btnClicked()
{
    QPushButton* btn = (QPushButton*)sender();
    T_SaveItem tSaveItem;
    memset(&tSaveItem,0,sizeof(T_SaveItem));
    if(btn == snapBtn){
        VideoManager::getInstance()->snapPic();
    }else if(btn == pathBtn){

    }else if(btn == IOPullBtn){

    }else if(btn == portEnsureBtn){


    }else if(btn == defaultBtns.value(EXPOGAIN)){
        sliderMap.value(EXPORESUE)->setValue(tCapbility.sExposeDesc.dfExposeDef);
        sliderMap.value(GAIN)->setValue(tCapbility.sExposeDesc.dfGainDef);

    }else if(btn == defaultBtns.value(COLORTINT)){
        sliderMap.value(COLORTMP)->setValue(tCapbility.sColorTemp.iTempDef);
        sliderMap.value(TINT)->setValue(tCapbility.sColorTemp.iTintDef);
    }else if(btn == defaultBtns.value(COLORADJUST)){
        if(tCapbility.sShadesRange.ibUsed){
            sliderMap.value(SHADES)->setValue(tCapbility.sShadesRange.iDef);
        }
        if(tCapbility.sSaturationRange.ibUsed){
            sliderMap.value(SATURATION)->setValue(tCapbility.sSaturationRange.iDef);
        }
        if(tCapbility.sBrightnessRange.ibUsed){
            sliderMap.value(BRIGHTNESS)->setValue(tCapbility.sBrightnessRange.iDef);
        }
        if(tCapbility.sContrastRange.ibUsed){
            sliderMap.value(CONTRAST)->setValue(tCapbility.sContrastRange.iDef);
        }
        if(tCapbility.sGammaRange.ibUsed){
            sliderMap.value(GAMA)->setValue(tCapbility.sGammaRange.iDef);
        }
    }else if(btn == ensureBtns.value(EXPOGAIN)){
        tSaveItem.bSaveExpoGain = true;
        tCapbility.sExposeDesc.dfExposeSaved = valueMap.value(EXPORESUE)->text().toDouble();
        tCapbility.sExposeDesc.dfGainSaved  = valueMap.value(GAIN)->text().toDouble();
        VideoManager::getInstance()->saveCameraCfgToFile(tSaveItem,tCapbility);
    }else if(btn == ensureBtns.value(COLORTINT)){
        tSaveItem.bSaveTempAndTint = true;
        tCapbility.sColorTemp.iTempSaved = valueMap.value(COLORTMP)->text().toInt();
        tCapbility.sColorTemp.iTintSaved = valueMap.value(TINT)->text().toInt();
        VideoManager::getInstance()->saveCameraCfgToFile(tSaveItem,tCapbility);

    }else if(btn == ensureBtns.value(COLORADJUST)){
        tSaveItem.bSaveAdjColor = true;
        tCapbility.sShadesRange.iSaved = valueMap.value(SHADES)->text().toInt();
        tCapbility.sSaturationRange.iSaved = valueMap.value(SATURATION)->text().toInt();
        tCapbility.sBrightnessRange.iSaved = valueMap.value(BRIGHTNESS)->text().toInt();
        tCapbility.sContrastRange.iSaved = valueMap.value(CONTRAST)->text().toInt();
        tCapbility.sGammaRange.iSaved   = valueMap.value(GAMA)->text().toInt();
        VideoManager::getInstance()->saveCameraCfgToFile(tSaveItem,tCapbility);
    }

}
QLayout* ArgsSetting::createLayout()
{
    QVBoxLayout* mainLay = new QVBoxLayout;
    QTabWidget* tabWidget = new QTabWidget;
    tabWidget->setStyleSheet("QTabWidget::pane{border-width:5px;border: 3px solid #00fff8;border-radius:10px;background: transparent;}QTabWidget::tab-bar{left:20px;border-width:3px;}\
                    QTabBar::tab {background: transparent;\
                    border: 2px solid #00fff8;color:white;\
                    border-bottom-color: #C2C7CB; \
                    border-top-left-radius: 4px;\
                    border-top-right-radius: 4px;\
                    min-width: 8px;padding: 2px;}QTabBar::tab:selected{background:#0000ff;border-color: #00fff8;border-bottom-color: #C2C7CB;}\
                    QTabBar::tab:!selected{margin-top: 2px;}");
    tabWidget->addTab(cameraArgsWidget,QString::fromLocal8Bit("相机参数"));
    tabWidget->addTab(IOArgsWidget,QString::fromLocal8Bit("IO板参数"));
    mainLay->addWidget(tabWidget);
    return mainLay;

}
