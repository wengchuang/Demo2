#include "mainwindown.h"
#include "videomanager.h"
#include "debugredirect.h"
#include "appconfig.h"
#include <QDebug>




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
    }
}


MainWindown::MainWindown(QWidget *parent) :
    QWidget(parent)
{
    this->setBackgroundRole(QPalette::Shadow);
    this->setAutoFillBackground(true);
    this->resize(1400,1000);
    this->setLayout(createLayout());

    createWidgets();
    this->setWindowFlags(windowFlags() &~ Qt::WindowMinimizeButtonHint);
    connect(runStatusBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(argsSetBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(entryBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(exportBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));



    process = new QProcess(this);


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
    qDebug("11111111111111111111");
    QPushButton* btn = (QPushButton*)sender();
    if(btn == runStatusBtn){
        btn->setEnabled(false);
        ICaptureOpr* capOpr = VideoManager::getInstance()->getCaptureOpr();
        if(capOpr){
            if(capOpr->isOpened()){

                VideoManager::getInstance()->getCaptureOpr()->closeCapture();
                UI_CameraOpened(false);
            }else if(VideoManager::getInstance()->openCamera()){
                qDebug("22222222222222");

                VideoManager::getInstance()->startVideo();
                UI_CameraOpened(true);
            }
        }
        qDebug("3333333333333333");
        btn->setEnabled(true);

    }else if(btn == argsSetBtn){
        settingWidgets->exec();
    }else if(btn == entryBtn){


    }else if(btn == exportBtn){
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
    vLay1->addStretch(1);
    exportBtn = new QPushButton(QString::fromLocal8Bit("报表\n导出"));
    setBtnStyle(exportBtn);
    vLay1->addWidget(exportBtn,1);

    vLay1->addStretch(1);
    argsSetBtn = new QPushButton(QString::fromLocal8Bit("参数\n设置"));
    setBtnStyle(argsSetBtn);
    vLay1->addWidget(argsSetBtn,1);

    vLay1->addStretch(1);
    entryBtn = new QPushButton(QString::fromLocal8Bit("样本\n录入"));
    setBtnStyle(entryBtn);
    vLay1->addWidget(entryBtn,1);

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

#if 0
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
#endif

    groupBox->setLayout(vLay);


    return mainLay;
}

QLayout*MainWindown::createDetectLayout()
{
    QVBoxLayout*vLay = new QVBoxLayout;
    vLay->addSpacing(5);
    QStringList strList;
    strList<<QString::fromLocal8Bit("一号砖计数")<<QString::fromLocal8Bit("二号砖计数")
          <<QString::fromLocal8Bit("三号砖计数")<<QString::fromLocal8Bit("四号砖计数")
         <<QString::fromLocal8Bit("五号砖计数")<<QString::fromLocal8Bit("六号砖计数")
        <<QString::fromLocal8Bit("七号砖计数")<<QString::fromLocal8Bit("八号砖计数")<<QString::fromLocal8Bit("不良砖计数")
       <<QString::fromLocal8Bit("检测砖累计");
    for(int i=0 ;i<strList.count();i++){
        QHBoxLayout* hLay = new QHBoxLayout;
        QLabel* lab = new QLabel(strList.at(i));
        QFont font =lab->font();
        font.setPixelSize(12);
        lab->setFont(font);
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
    //exportDlg      = new ExportDlg(this);
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

    delete VideoManager::getInstance();

}
