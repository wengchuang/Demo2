#ifndef EXPORTDLG_H
#define EXPORTDLG_H

#include <QLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QDateTime>
#include "barchart.h"

#include "basedialog.h"

class ExportDlg : public BaseDialog
{
    Q_OBJECT
public:
    explicit ExportDlg(QWidget *parent = 0);
private slots:
    void btnClicked();
protected:
    void showEvent(QShowEvent* ev);
private:
    QLayout* createLayout();
    QPushButton* saveBtn;
    QPushButton* searchBtn;
    QPushButton* resetBtn;
    QTableWidget* tableWidget;
    Barchart*     barchart;
    QDateTime     oldTime;
};

#endif // EXPORTDLG_H
