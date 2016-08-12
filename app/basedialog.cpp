#include "basedialog.h"
#include <QDebug>

BaseDialog::BaseDialog(QWidget *parent) :
    QDialog(parent),parent(NULL)
{
    this->setBackgroundRole(QPalette::Shadow);
    this->setAutoFillBackground(true);
    if(parent){
        this->parent = parent;
    }
}
BaseDialog::~BaseDialog()
{
    qDebug()<<"~BaseDialog";
}
