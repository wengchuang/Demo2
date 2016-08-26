#ifndef FILEOPR_H
#define FILEOPR_H

#include <QString>

namespace FileOpr {

void fileOprMkdir(const QString& dirName);
bool deleteDir(const QString& path);
bool isFileExists(const QString& fileName);
}


#if 0
class FileOpr
{    
public:
    FileOpr();
};
#endif

#endif // FILEOPR_H
