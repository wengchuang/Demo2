#ifndef DEBUGREDIRECT_H
#define DEBUGREDIRECT_H
#include <QDebug>

void messageHandler(QtMsgType type,const char*msg);
#define  INFO_DEBUG(msg)  messageHandler(QtDebugMsg,msg);
#define  WARN_DEBUG(msg)  messageHandler(QtWarningMsg,msg);


#endif // DEBUGREDIRECT_H
