#pragma once

typedef unsigned char UCHAR;


#define ReadUint8WithBigEndian(Temp, Data, Index) \
        Temp = Data[Index++] & 0xFF

#define WriteUint8WithBigEndian(Temp, Data, Index) \
        Data[Index++] = (UCHAR) Temp


#define ReadUint16WithBigEndian(Temp, Data, Index) \
        Temp = (Data[Index++] & 0xFF); \
        Temp = ((Temp << 8) | (Data[Index++] & 0xFF))

#define WriteUint16WithBigEndian(Temp, Data, Index) \
        Data[Index++] = (UCHAR) (Temp >> 8); \
        Data[Index++] = (UCHAR) Temp

#define WriteUint32WithBigEndian(Temp, Data, Index) \
        Data[Index++] = (UCHAR) (Temp >> 24); \
        Data[Index++] = (UCHAR) (Temp >> 16); \
        Data[Index++] = (UCHAR) (Temp >> 8); \
        Data[Index++] = (UCHAR) Temp

#define ReadUint32WithBigEndian(Temp, Data, Index) \
        Temp = (Data[Index++] & 0xFF); \
        Temp = ((Temp << 8) | (Data[Index++] & 0xFF)); \
        Temp = ((Temp << 8) | (Data[Index++] & 0xFF)); \
        Temp = ((Temp << 8) | (Data[Index++] & 0xFF))


#define WriteUint48WithBigEndian(Temp, Data, Index) \
        Data[Index++] = (UCHAR) (Temp >> 40); \
        Data[Index++] = (UCHAR) (Temp >> 32); \
        Data[Index++] = (UCHAR) (Temp >> 24); \
        Data[Index++] = (UCHAR) (Temp >> 16); \
        Data[Index++] = (UCHAR) (Temp >> 8); \
        Data[Index++] = (UCHAR) Temp

#define ReadUint48WithBigEndian(Temp, Data, Index) \
        Temp = (Data[Index++] & 0xFF); \
        Temp = ((Temp << 8) | (Data[Index++] & 0xFF)); \
        Temp = ((Temp << 8) | (Data[Index++] & 0xFF)); \
        Temp = ((Temp << 8) | (Data[Index++] & 0xFF))\
        Temp = ((Temp << 8) | (Data[Index++] & 0xFF))\
        Temp = ((Temp << 8) | (Data[Index++] & 0xFF))




