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
private:
    QLayout* createLayout();
    void     createWidgets();

    QLayout* createModeSetLayout();
    QLayout* createSerialLayout();
    QLayout* createOutTimeLayout();
    QLayout* createColorLayout();
    QLayout* createResoultionLayout();
    QWidget* createAutoWidget();
    QWidget* createExternTriggerWidget();
private:
    QWidget*    cameraArgsWidget;
    QWidget*    IOArgsWidget;
    QWidget*    autoWidget;
    QWidget*    externTriggerWidget;
    QLineEdit*  exporesEdit;



    QMap<QString,QSlider*>         sliderMap;
    QMap<QString,QLineEdit*>          valueMap;
    QMap<QString, QPushButton*>    defaultBtns;
    QMap<QString, QPushButton*>    ensureBtns;
    QPushButton* snapBtn;
    QPushButton* pathBtn;
    QPushButton* IOPullBtn;
    QPushButton* portEnsureBtn;
    QComboBox  * portNameComBox;
    QSpinBox*    ioPullTime;
    QLineEdit*   pathEdit;
    QVBoxLayout* tmpLay;
    QWidget*     curWidget;
    QRadioButton* autoRadioBtn;
    QRadioButton* externRadioBtn;
    QComboBox*      resBox;

    T_CameraCapbility tCapbility;
};

#endif // ARGSSETTING_H
