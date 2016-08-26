#include "fileopr.h"
#include "appconfig.h"
#include <QDir>
#include <QDate>
#include <QTime>

namespace FileOpr {

void fileOprMkdir(const QString &dirName)
{
    QDir dir;
    if(!dir.exists(dirName)){
        dir.mkpath(dirName);
    }
}
bool isFileExists(const QString& fileName)
{
    QFile file(fileName);
    return file.exists();
}

bool deleteDir(const QString& path){

    if(path.isEmpty())
        return false;
    QDir dir(path);
    if(!dir.exists()){
        return true;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList) {
        if(fi.isFile()){
            fi.dir().remove(fi.fileName());
        }else{
            deleteDir(fi.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath());

}


}


#if 0
FileOpr::FileOpr()
{
}
#endif
