#ifndef MUDBUSDEF_H
#define MUDBUSDEF_H

#define SENDBUFSIZE         50
#define RECVBUFSIZE         50
typedef struct _FunData
{
    unsigned char  slaveId;
    unsigned char  funCode;
    unsigned short startAddr;
    unsigned short count;//传送的数据

}T_FunData,*PT_FunData;

#define be16tole16(x) \
    ((unsigned short)(\
    (((unsigned short)(x)&(unsigned short)0x00ff)<<8) | \
    (((unsigned short)(x)&(unsigned short)0xff00)>>8)))



#define be32tole32(x) \
    ((unsigned int)(\
    (((unsigned int)(x)&(unsigned int)0x000000ffUL)<<24) | \
    (((unsigned int)(x)&(unsigned int)0x0000ff00UL)<<8) | \
    (((unsigned int)(x)&(unsigned int)0x00ff0000UL)>>8) | \
    (((unsigned int)(x)&(unsigned int)0xff000000UL)>>24)))



#endif // MUDBUSDEF_H
