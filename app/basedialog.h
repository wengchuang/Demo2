#ifndef BASEDIALOG_H
#define BASEDIALOG_H

#include <QDialog>
#include <QGroupBox>
#include <QPushButton>

class BaseDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BaseDialog(QWidget *parent = 0);
    ~BaseDialog();

signals:

public slots:
protected:
    inline void setGroupBoxStyle(QGroupBox* gbbox){
        gbbox->setStyleSheet("QGroupBox{color:rgb(168,185,133);border: 3px solid #00fff8;border-radius:10px;}");
        QFont font = gbbox->font();
        font.setPixelSize(15);
        gbbox->setFont(font);
    }
    inline void setPushBtnStyle(QPushButton* btn){
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setFixedSize(80,40);
        btn->setStyleSheet("QPushButton{background-color:rgb(85, 170, 255);color:white;border-radius:2px;border:3px groove gray;\
                                       border-style: outset;}"
                                       "QPushButton:pressed{background-color:rgb(0,0,250);border-style:inset;}");
    }
protected:
    QWidget* parent;
private:


};

#endif // BASEDIALOG_H
