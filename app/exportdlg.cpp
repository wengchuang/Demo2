#include "exportdlg.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollBar>
#include <cv.h>
#include <highgui.h>
#include "debugredirect.h"
//#include "fileopr.h"
#include "videomanager.h"
//#include "filemanager.h"
#include "appconfig.h"

ExportDlg::ExportDlg(QWidget *parent):BaseDialog(parent)
{
    tableWidget = NULL;
    barchart = NULL;
    this->setWindowTitle(QString::fromLocal8Bit("导出报表"));
    this->setLayout(createLayout());
    oldTime = QDateTime::currentDateTime();
}
void ExportDlg::setSamples(const QVector<double>& samples)
{
    this->samples = samples;
}

void ExportDlg::showEvent(QShowEvent *ev)
{
    if(parent){
        QRect rect = parent->geometry();
        this->setGeometry(rect.x()+rect.width()/4,rect.y()+rect.height()/4,rect.width()/2,510);
    }

    if(barchart){
        barchart->repaintChart("生产报表 "+oldTime.toString("yyyy-MM-dd hh:mm - ")+
                               QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"),this->samples);
    }


    QWidget::showEvent(ev);
}

void ExportDlg::btnClicked()
{
    QPushButton* btn =(QPushButton*)sender();
    if(btn == saveBtn){
        barchart->exportChart();

    }else if(btn == resetBtn){
        QVector<double> samples;
        barchart->repaintChart(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"),samples);
        emit resetReport();

    }else if(btn == searchBtn){

    }
}

QLayout* ExportDlg::createLayout()
{
    QVBoxLayout* mainLay = new QVBoxLayout;
    QHBoxLayout* hLay = new QHBoxLayout;
    hLay->setAlignment(Qt::AlignCenter);
    mainLay->setAlignment(Qt::AlignCenter);
    this->barchart = new Barchart(this);
    hLay->addWidget(barchart);
    mainLay->addLayout(hLay);


    hLay = new QHBoxLayout;
    hLay->addStretch(1);
    hLay->setAlignment(Qt::AlignCenter);
    mainLay->addLayout(hLay);

    saveBtn = new QPushButton(QString::fromLocal8Bit("保存报表"));
    setPushBtnStyle(saveBtn);
    hLay->addWidget(saveBtn);
    connect(saveBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    hLay->addStretch(1);
#if 0
    searchBtn = new QPushButton(QString::fromLocal8Bit("历史报表"));
    setPushBtnStyle(searchBtn);
    hLay->addWidget(searchBtn);
    connect(searchBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    hLay->addStretch(1);
#endif


    resetBtn = new QPushButton(QString::fromLocal8Bit("重置报表"));
    setPushBtnStyle(resetBtn);
    hLay->addWidget(resetBtn);
    connect(resetBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    hLay->addStretch(1);

    return mainLay;
}

