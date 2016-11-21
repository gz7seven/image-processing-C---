#include <stdio.h>
#include <stdlib.h>
 
#pragma pack(2)
 
/*����WORDΪ�����ֽڵ�����*/
typedef unsigned short WORD;
/*����DWORDΪe�ĸ��ֽڵ�����*/
typedef unsigned long DWORD;
 
/*λͼ�ļ�ͷ*/
typedef struct BMP_FILE_HEADER
{
    WORD bType;             /*  �ļ���ʶ��          */
    DWORD bSize;            /*  �ļ��Ĵ�С          */
    WORD bReserved1;        /*  ����ֵ,��������Ϊ0  */       
    WORD bReserved2;        /*  ����ֵ,��������Ϊ0  */
    DWORD bOffset;          /*  �ļ�ͷ�����ͼ������λ��ʼ��ƫ����    */
} BMPFILEHEADER;
 
/*λͼ��Ϣͷ*/
typedef struct BMP_INFO
{
    DWORD bInfoSize;       /*  ��Ϣͷ�Ĵ�С             */
    DWORD bWidth;          /*  ͼ��Ŀ��               */
    DWORD bHeight;         /*  ͼ��ĸ߶�               */
    WORD bPlanes;          /*  ͼ���λ����             */
    WORD bBitCount;        /*  ÿ�����ص�λ��           */
    DWORD bCompression;    /*  ѹ������                 */
    DWORD bmpImageSize;    /*  ͼ��Ĵ�С,���ֽ�Ϊ��λ  */
    DWORD bXPelsPerMeter;  /*  ˮƽ�ֱ���               */
    DWORD bYPelsPerMeter;  /*  ��ֱ�ֱ���               */
    DWORD bClrUsed;        /*  ʹ�õ�ɫ����             */
    DWORD bClrImportant;   /*  ��Ҫ����ɫ��             */
} BMPINF;
 
/*��ɫ��*/
typedef struct RGB_QUAD 
{
    WORD rgbBlue;         /*  ��ɫǿ��  */
    WORD rgbGreen;        /*  ��ɫǿ��  */
    WORD rgbRed;          /*  ��ɫǿ��  */
    WORD rgbReversed;     /*  ����ֵ    */
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
 
    //���㲢���λͼ���ݵ�ƫ������ͼ��Ĵ�С����Ⱥ͸߶ȣ�ÿ�����ص���ռ���ֽ�
    size = fileHeader.bSize;
    offset = fileHeader.bOffset;
    bmpImageSize = infoHeader.bmpImageSize;
    width = infoHeader.bWidth;
    height = infoHeader.bHeight;
    bitCount = infoHeader.bBitCount;
    bytesPerPixel = infoHeader.bBitCount / 8;
     
    printf("%d %d %d %d %d %d\n", size, offset, bmpImageSize, width, height, bitCount, bytesPerPixel);
 
    //���ÿ�����ص���ռ�ֽ��е�����
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