#ifndef SYSTEMSOURCE_H
#define SYSTEMSOURCE_H

#include <QObject>

class SystemSource : public QObject
{
    Q_OBJECT
public:
    static SystemSource* getInstance();
    bool   sourceInit();
    ~SystemSource();
signals:

public slots:
private:
    explicit SystemSource(QObject *parent = 0);
    static   SystemSource* instance;
    bool     isInit;
};

#endif // SYSTEMSOURCE_H
