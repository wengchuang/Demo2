#include "mainwindown.h"
#include "videomanager.h"
#include "debugredirect.h"
#include "appconfig.h"
#include <QString>




QTextEdit* MainWindown::logTxtEdit = NULL;

void MainWindown::updateLog(QtMsgType type, const QString &msg)
{
    if(logTxtEdit){
        switch(type){
        case QtDebugMsg:
            logTxtEdit->setTextColor(QColor(0,0,0));
            break;
        case QtWarningMsg:
        case QtCriticalMsg:
            logTxtEdit->setTextColor(QColor(255,0,0));
            break;
        }
        logTxtEdit->append(msg);
        logTxtEdit->moveCursor(QTextCursor::End);
    }
}


MainWindown::MainWindown(QWidget *parent) :
    QWidget(parent)
{
    this->setBackgroundRole(QPalette::Shadow);
    this->setAutoFillBackground(true);

    this->setLayout(createLayout());

    createWidgets();
    this->setWindowFlags(windowFlags() &~ Qt::WindowMinimizeButtonHint);
    connect(runStatusBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(argsSetBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(exportBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(HandlerManager::getInstance(),SIGNAL(hasDataForUI(HandleMsg*)),this,SLOT(upDateUI(HandleMsg*)));

    //process = new QProcess(this);


}
void MainWindown::upDateUI(HandleMsg *msg)
{
    if(msg->handlerId == 1){
        int* frameRate = (int*) msg->context;
        this->gaugeWidget->setValue(*frameRate);
    }else if(msg->handlerId == 0){
        char* data = (char*)msg->context;

        if(*data >0){
            edits.at((*data)-1)->setText(QString::number(edits.at((*data)-1)->text().toInt()+1));
            edits.at(9)->setText(QString::number(edits.at(9)->text().toInt()+1));
        }
    }
    msg->action = UIDEALDONE;
}

void MainWindown::UI_CameraOpened(bool isOpened)
{
    if(isOpened){
        runStatusBtn->setText(QString::fromLocal8Bit("关闭\n摄像头"));
    }else{
        runStatusBtn->setText(QString::fromLocal8Bit("打开\n摄像头"));
        settingWidgets->UI_uninit();

    }
}
void MainWindown::btnClicked()
{
    QPushButton* btn = (QPushButton*)sender();
    if(btn == runStatusBtn){
        btn->setEnabled(false);
        ICaptureOpr* capOpr = VideoManager::getInstance()->getCaptureOpr();
        if(capOpr){
            if(capOpr->isOpened()){

                VideoManager::getInstance()->getCaptureOpr()->closeCapture();
                UI_CameraOpened(false);
                INFO_DEBUG(QString::fromLocal8Bit("相机已关闭！").toLocal8Bit().data());
            }else if(VideoManager::getInstance()->openCamera()){

                VideoManager::getInstance()->startVideo();
                UI_CameraOpened(true);
                INFO_DEBUG(QString::fromLocal8Bit("相机已打开！").toLocal8Bit().data());
            }
        }
        btn->setEnabled(true);
    }else if(btn == argsSetBtn){
        settingWidgets->exec();
    }else if(btn == entryBtn){

    }else if(btn == exportBtn){
        exportDlg->exec();
    }
}


QVBoxLayout* MainWindown::createLeftLayout()
{
    QVBoxLayout* mainLay = new QVBoxLayout;
    mainLay->setMargin(0);
    QVBoxLayout* vLay;

    QGroupBox* groupBox = new QGroupBox(QString::fromLocal8Bit("菜单选项"));
    setGroupBoxStyle(groupBox);

    QVBoxLayout* vLay1 = new QVBoxLayout;
    vLay1->setAlignment(Qt::AlignCenter);

    vLay1->addSpacing(10);
#if 1
    vLay1->addStretch(1);
    exportBtn = new QPushButton(QString::fromLocal8Bit("报表\n导出"));
    setBtnStyle(exportBtn);
    vLay1->addWidget(exportBtn,1);
#endif

    vLay1->addStretch(1);
    argsSetBtn = new QPushButton(QString::fromLocal8Bit("参数\n设置"));
    setBtnStyle(argsSetBtn);
    vLay1->addWidget(argsSetBtn,1);
#if 0
    vLay1->addStretch(1);
    entryBtn = new QPushButton(QString::fromLocal8Bit("样本\n录入"));
    setBtnStyle(entryBtn);
    vLay1->addWidget(entryBtn,1);
#endif

    vLay1->addStretch(1);
    runStatusBtn = new QPushButton(QString::fromLocal8Bit("打开\n摄像头"));
    setBtnStyle(runStatusBtn);
    vLay1->addWidget(runStatusBtn,1);
    vLay1->addStretch(1);

    groupBox->setLayout(vLay1);

    mainLay->addWidget(groupBox);




    vLay = new QVBoxLayout;
    vLay->setAlignment(Qt::AlignLeft);
    groupBox = new QGroupBox;
    setGroupBoxStyle(groupBox);
    groupBox->setTitle(QString::fromLocal8Bit("运行信息"));
    vLay->addWidget(groupBox);
    mainLay->addLayout(vLay);

    vLay = new QVBoxLayout;
    vLay->addSpacing(10);
  #if 0
    QHBoxLayout*hLay = new QHBoxLayout;
    QLabel* lab = new QLabel(QString::fromLocal8Bit("操作员："));
    setLabelStyle(lab);
    hLay->addWidget(lab);

    QLineEdit*edit = new QLineEdit;
    edit->setText(QString::fromLocal8Bit("甲乙丙丁"));
    setEditStyle(edit);
    hLay->addWidget(edit);
    vLay->addLayout(hLay);
#endif
    QHBoxLayout*hLay = new QHBoxLayout;
    hLay->setAlignment(Qt::AlignCenter);
    QGroupBox* gbBox = new QGroupBox(QString::fromLocal8Bit("统计信息"));
    gbBox->setStyleSheet("QGroupBox{color:rgb(168,185,133);border: 2px solid #00fff8;border-radius:5px;}");
    QFont font = gbBox->font();
    font.setPixelSize(15);
    gbBox->setFont(font);
    gbBox->setLayout(createDetectLayout());

    hLay->addWidget(gbBox);
    vLay->addLayout(hLay);

    hLay = new QHBoxLayout;
    hLay->setAlignment(Qt::AlignCenter);
    gbBox = new QGroupBox(QString::fromLocal8Bit("状态信息"));
    gbBox->setStyleSheet("QGroupBox{color:rgb(168,185,133);border: 2px solid #00fff8;border-radius:5px;}");
    font = gbBox->font();
    font.setPixelSize(15);
    gbBox->setFont(font);
    gbBox->setLayout(createRunStatusLayout());
    hLay->addWidget(gbBox);
    vLay->addLayout(hLay);

    groupBox->setLayout(vLay);


    return mainLay;
}
QLayout*MainWindown::createRunStatusLayout()
{
    QVBoxLayout* vLay = new QVBoxLayout;
    gaugeWidget = new myGauge2(40,QString::fromLocal8Bit("帧率"));
    vLay->addWidget(gaugeWidget);

    return vLay;
}

QLayout*MainWindown::createDetectLayout()
{
    QVBoxLayout*vLay = new QVBoxLayout;
    vLay->addSpacing(5);
    QStringList strList;
    strList<<QString::fromLocal8Bit("一号通道计数")<<QString::fromLocal8Bit("二号通道计数")
          <<QString::fromLocal8Bit("三号通道计数")<<QString::fromLocal8Bit("四号通道计数")
         <<QString::fromLocal8Bit("五号通道计数")<<QString::fromLocal8Bit("六号通道计数")
        <<QString::fromLocal8Bit("七号通道计数")<<QString::fromLocal8Bit("八号通道计数")<<QString::fromLocal8Bit("不良砖计数")
       <<QString::fromLocal8Bit("检测累计");
    for(int i=0 ;i<strList.count();i++){
        QHBoxLayout* hLay = new QHBoxLayout;
        QLabel* lab = new QLabel(strList.at(i));
        QFont font =lab->font();
        font.setPixelSize(12);
        lab->setFont(font);
        lab->setFixedWidth(80);
        hLay->addWidget(lab);
        QLineEdit* edit = new QLineEdit;
        edit->setEnabled(false);
        edit->setAlignment(Qt::AlignCenter);
        edit->setStyleSheet("color:#000000;border:2px solid #A8B985;background-color:white;border-radius:2px;");
        hLay->addWidget(edit);
        edits.append(edit);
        vLay->addLayout(hLay);
    }

    return vLay;
}

QVBoxLayout* MainWindown::createRightLayout()
{
    QVBoxLayout* vLay = new QVBoxLayout;
    QVBoxLayout* vLay1;

    QGroupBox* gbbox = new QGroupBox(QString::fromLocal8Bit("预览"));
    setGroupBoxStyle(gbbox);
    vLay1 = new QVBoxLayout;
    vLay1->addSpacing(10);
    vLay1->addWidget(VideoManager::getInstance()->getRender());
    gbbox->setLayout(vLay1);
    vLay->addWidget(gbbox,3);

    gbbox = new QGroupBox(QString::fromLocal8Bit("系统提示信息"));
    setGroupBoxStyle(gbbox);
    vLay1 = new QVBoxLayout;
    vLay1->addSpacing(10);
    logTxtEdit = new QTextEdit;
    logTxtEdit->setReadOnly(true);
    vLay1->addWidget(logTxtEdit);
    gbbox->setLayout(vLay1);
    vLay->addWidget(gbbox,1);


    return vLay;
}
void  MainWindown::createWidgets()
{
    settingWidgets = new ArgsSetting(this);
    exportDlg      = new ExportDlg(this);
}
QLayout* MainWindown::createLayout()
{
    QHBoxLayout* mainLay = new QHBoxLayout;
    QVBoxLayout* leftLay = createLeftLayout();
    mainLay->addLayout(leftLay,1);
    QVBoxLayout* rightLay = createRightLayout();
    mainLay->addLayout(rightLay,5);
    return mainLay;
}
MainWindown::~MainWindown()
{


}
