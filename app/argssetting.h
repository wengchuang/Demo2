#ifndef ARGSSETTING_H
#define ARGSSETTING_H

#include "basedialog.h"
#include <QLayout>
#include <QLineEdit>
#include <QSlider>
#include <QLabel>
#include <QRadioButton>
#include <QMap>
#include <QSpinBox>
#include <QComboBox>
#include "mysliderbutton.h"

#include "appconfig.h"
#include "videomanager.h"

class ArgsSetting : public BaseDialog
{
    Q_OBJECT
public:
    explicit ArgsSetting(QWidget *parent = 0);
    void UI_uninit();
protected:
    void showEvent(QShowEvent* ev);

signals:

private slots:
    void UI_chanageCapbility();
    void editingFinished();
    void silderValueChanaged(int value);
    void btnClicked();
    void radioBtnClicked();
    void comBoxIndexChanaged(int index);
    void contrlLineSlot(int index);
private:
    QLayout* createLayout();
    void     createWidgets();

    QLayout* createModeSetLayout();
    QLayout* createSerialLayout();
    QLayout* createOutTimeLayout();
    QLayout* createColorLayout();
    QLayout* createResoultionLayout();
    QLayout* createRedLineCfgLayout(const LineCfg& lineCfg);
    QLayout* createBlackLineCfgLayout(const LineCfg& lineCfg);
    QLayout* createOutputCfgLayout();
    QLayout* debugLineCntLayout();


    QWidget* createAutoWidget();
    QWidget* createExternTriggerWidget();


private:
    QWidget*    cameraArgsWidget;
    QWidget*    IOArgsWidget;
    QWidget*    debugWidget;

    QWidget*    autoWidget;
    QWidget*    externTriggerWidget;
    QLineEdit*  exporesEdit;



    QMap<QString,QSlider*>         sliderMap;
    QMap<QString,QLineEdit*>          valueMap;
    QMap<QString, QPushButton*>    defaultBtns;
    QMap<QString, QPushButton*>    ensureBtns;
    QMap<QString, QSpinBox*>         spBoxs;
    QPushButton* snapBtn;
    QPushButton* pathBtn;
    QPushButton* IOPullBtn;
    QPushButton* portEnsureBtn;
    QComboBox  * portNameComBox;
    QSpinBox*    ioPullTime;
    QSpinBox*    timeOutSpBox;
    QLineEdit*   pathEdit;
    QVBoxLayout* tmpLay;
    QWidget*     curWidget;
    QRadioButton* autoRadioBtn;
    QRadioButton* externRadioBtn;
    QComboBox*      resBox;


    QList<QComboBox*> lineCntrlComList;
    QList<QComboBox*> redLineCntrlComList;
    QList<QComboBox*> blackLineCntrlComList;
    QList<QWidget*>   lineCntrlWidgets;
    QVBoxLayout*      lineCfgInVBox;
    QList<QLayout*>   lineCfgLays;
    mySliderButton* slidBtn;

    T_CameraCapbility tCapbility;
};

#endif // ARGSSETTING_H
