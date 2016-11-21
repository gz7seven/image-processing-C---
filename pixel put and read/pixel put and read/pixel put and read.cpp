#include <stdio.h>
#include <stdlib.h>
 
#pragma pack(2)
 
/*定义WORD为两个字节的类型*/
typedef unsigned short WORD;
/*定义DWORD为e四个字节的类型*/
typedef unsigned long DWORD;
 
/*位图文件头*/
typedef struct BMP_FILE_HEADER
{
    WORD bType;             /*  文件标识符          */
    DWORD bSize;            /*  文件的大小          */
    WORD bReserved1;        /*  保留值,必须设置为0  */       
    WORD bReserved2;        /*  保留值,必须设置为0  */
    DWORD bOffset;          /*  文件头的最后到图像数据位开始的偏移量    */
} BMPFILEHEADER;
 
/*位图信息头*/
typedef struct BMP_INFO
{
    DWORD bInfoSize;       /*  信息头的大小             */
    DWORD bWidth;          /*  图像的宽度               */
    DWORD bHeight;         /*  图像的高度               */
    WORD bPlanes;          /*  图像的位面数             */
    WORD bBitCount;        /*  每个像素的位数           */
    DWORD bCompression;    /*  压缩类型                 */
    DWORD bmpImageSize;    /*  图像的大小,以字节为单位  */
    DWORD bXPelsPerMeter;  /*  水平分辨率               */
    DWORD bYPelsPerMeter;  /*  垂直分辨率               */
    DWORD bClrUsed;        /*  使用的色彩数             */
    DWORD bClrImportant;   /*  重要的颜色数             */
} BMPINF;
 
/*彩色表*/
typedef struct RGB_QUAD 
{
    WORD rgbBlue;         /*  蓝色强度  */
    WORD rgbGreen;        /*  绿色强度  */
    WORD rgbRed;          /*  红色强度  */
    WORD rgbReversed;     /*  保留值    */
} RGBQUAD;
 
int main()
{
    FILE *fp;
    BMPFILEHEADER fileHeader;
    BMPINF infoHeader;
    long offset, bmpImageSize, width, height, bytesPerPixel, size, bitCount;
//    int i, j;
//    unsigned char **p;
    WORD c;
     
    if((fp = fopen("c:\1.bmp", "rb")) == NULL)
    {
        printf("Cann't open the file!\n");
        exit(0);
    }
 
    fseek(fp, 0, 0);
    fread(&fileHeader, sizeof(fileHeader), 1, fp);
    fread(&infoHeader, sizeof(infoHeader), 1, fp);
 
    //计算并输出位图数据的偏移量，图像的大小，宽度和高度，每个像素点所占的字节
    size = fileHeader.bSize;
    offset = fileHeader.bOffset;
    bmpImageSize = infoHeader.bmpImageSize;
    width = infoHeader.bWidth;
    height = infoHeader.bHeight;
    bitCount = infoHeader.bBitCount;
    bytesPerPixel = infoHeader.bBitCount / 8;
     
    printf("%d %d %d %d %d %d\n", size, offset, bmpImageSize, width, height, bitCount, bytesPerPixel);
 
    //输出每个像素点所占字节中的内容
    c = fgetc(fp);
    while (!feof(fp))
    {
        printf("%x ", c);
        c = fgetc(fp);
    }
   
    printf("\n");
 
    fclose(fp);
     
    return 0;
}