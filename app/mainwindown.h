#ifndef MAINWINDOWN_H
#define MAINWINDOWN_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QVector>
#include <QProcess>
#include <QTimerEvent>
#include "argssetting.h"

//#include "exportdlg.h"


class MainWindown : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindown(QWidget *parent = 0);
    ~MainWindown();
    static   void updateLog(QtMsgType type,const QString& msg);

signals:
protected:
    //void timerEvent(QTimerEvent *event);

public slots:
    void btnClicked();

private:
    void UI_CameraOpened(bool isOpened);
private:
    void     createWidgets();
    QLayout* createLayout();
    QVBoxLayout* createLeftLayout();
    QVBoxLayout* createRightLayout();
    QLayout*createDetectLayout();
    QLayout*createRunStatusLayout();
    inline void setBtnStyle(QPushButton* btn){
        QFont font = btn->font();
        font.setPixelSize(20);
        btn->setFont(font);
        btn->setFixedWidth(180);
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setStyleSheet("QPushButton{background-color:rgb(85, 170, 255);color:white;border-radius:2px;border:3px groove gray;\
                                       border-style: outset;}"
                                       "QPushButton:pressed{background-color:rgb(0,0,250);border-style:inset;}");
    }
    inline void setGroupBoxStyle(QGroupBox* gbBox){
        gbBox->setStyleSheet("QGroupBox{color:rgb(168,185,133);border: 3px solid #00fff8;border-radius:10px;}");
        QFont font = gbBox->font();
        font.setPixelSize(15);
        gbBox->setFont(font);
    }
    inline void setLabelStyle(QLabel* lab){
        lab->setAlignment(Qt::AlignRight);
        QFont font = lab->font();
        font.setPixelSize(12);
        lab->setFont(font);
        lab->setFixedSize(50,15);
    }
    inline void setEditStyle(QLineEdit* edit){
        edit->setAlignment(Qt::AlignCenter);
        QFont font = edit->font();
        font.setPixelSize(12);
        edit->setFont(font);
        edit->setEnabled(false);
        edit->setStyleSheet("color:#000000;border:2px solid #A8B985;background-color:white;border-radius:2px;");
        edit->setFixedSize(90,25);
    }

private:
    static QTextEdit*          logTxtEdit;
    int                         timerId;

    QVector<QLineEdit*>        edits;


    QPushButton*               runStatusBtn;
    QPushButton*               argsSetBtn;
    QPushButton*               entryBtn;
    QPushButton*               exportBtn;

    ArgsSetting*               settingWidgets;
    QProcess*                  process;

};

#endif // CENTERWIDGET_H
