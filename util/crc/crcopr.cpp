#include "crcopr.h"
namespace Util {

unsigned short CrcOpr::checkCRC(void* buf, int length)
{
    unsigned char uchCRCHi = 0xff;
    unsigned char uchCRCLo = 0xff;
    unsigned short uIndex  = 0;
    unsigned char* msg     = (unsigned char*)buf;

    while(length--)
    {
       uIndex=uchCRCHi^(*msg);
       msg++;
       uchCRCHi=uchCRCLo^auchCRCHi[uIndex];
       uchCRCLo=auchCRCLo[uIndex];

     }
     return (uchCRCLo<<8|uchCRCHi);

}

}


