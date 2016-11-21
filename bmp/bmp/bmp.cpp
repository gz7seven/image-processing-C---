/**********************************************************

                          作者：fankaipeng          

                          时间：2010-04-27
                          文件名称：zoombmp.cpp
                          描述：c 语言实现24位bmp图片读写，放大缩小。
                          开发工具 microsoft visual c++
                          开发平台 windows

 

***********************************************************/


#include <windows.h>
#include <stdio.h>
#define FXZOOMRATIO 1.5   //x轴放大倍数
#define FYZOOMRATIO 1.5   //y轴放大倍数
unsigned char *pBmpBuf;   //读入图像数据的指针
unsigned char *pNewBmpBuf;
int        bmpWidth;//图像的宽
int        bmpHeight;//图像的高
RGBQUAD    *pColorTable;//颜色表指针
int        biBitCount;//图像类型，每像素位数
long       newBmpWidth;//变化后图像的宽
long       newBmpHeight;//变化后图像的高
long       newLineByte; //变化后图像数据每行的字节数

/****************************************************************************
*函数名称：readBmp()
*函数参数：const char *bmpName 读入bmp格式文件的路径及名称
*函数返回值：0为失败 1为成功
*函数描述：给定文件的名称和路径 读入图像的位图数据，宽，高，及每个像素的位数进内存，保存在全局变量中
*
***************************************************************************/


bool readBmp(const char* bmpName)
{
FILE *fp=fopen(bmpName,"rb");
if(fp==0)
{
   printf("cannot open file");
   return 0;
}
fseek(fp,sizeof(BITMAPFILEHEADER),0);
BITMAPINFOHEADER head;
fread(&head,sizeof(BITMAPINFOHEADER),1,fp);
bmpWidth = head.biWidth;
bmpHeight = head.biHeight;
biBitCount = head.biBitCount;
int lineByte = (bmpWidth *biBitCount/8+3)/4*4;//计算图像每行像素所占的字节数
if(biBitCount == 8)
{
   pColorTable = new RGBQUAD[256];
   fread(pColorTable,sizeof(RGBQUAD),256,fp);
}
pBmpBuf = new unsigned char [lineByte *bmpHeight];
fread(pBmpBuf,1,lineByte *bmpHeight,fp);
fclose(fp);
return 1;

}

/****************************************************************************
*函数名称： saveBmp()
*函数参数： const char *bmpName    写入bmp格式文件的路径及名称
    unsigned char *imgBuf 待存盘的位图数据
    int width,             以像素为单位待存盘的位图宽
    int height,            以像素为单位待存盘的位图高
    int biBitCount,        每个像素占的位数
    RGBQUAD *pColorTable   颜色表指针
*函数返回值：0为失败 1为成功
*函数描述：给定写入bmp文件的名称和路径 要写入图像的位图数据，宽，高，写进文件中
*
***************************************************************************/

 

bool saveBmp(const char* bmpName,unsigned char *imgBuf,int width,int height,int biBitCount,RGBQUAD *pColorTable)
{
if(!imgBuf)//imgBuf 待存盘的位图数据
   return 0;
int colorTablesize = 0;
if(biBitCount == 8)
   colorTablesize =1024;
int lineByte = (width * biBitCount/8+3)/4*4;
FILE *fp = fopen(bmpName,"wb");
if(fp == 0) return 0;
BITMAPFILEHEADER fileHead;
fileHead.bfType= 0x4d42;
fileHead.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER) + colorTablesize + lineByte *height;
fileHead.bfReserved1 = 0;
fileHead.bfReserved2 = 0;
fileHead.bfOffBits = 54 +colorTablesize;
fwrite(&fileHead,sizeof(BITMAPFILEHEADER),1,fp);
BITMAPINFOHEADER head;
head.biBitCount = biBitCount;
head.biClrImportant = 0;
head.biClrUsed = 0;
head.biCompression = 0;
head.biHeight = height;
head.biPlanes =1;
head.biSize = 40;
head.biSizeImage = lineByte *height;
head.biWidth = width;
head.biXPelsPerMeter = 0;
head.biYPelsPerMeter = 0;
fwrite(&head,sizeof(BITMAPINFOHEADER),1,fp);
if(biBitCount == 8)
    fwrite(pColorTable,sizeof(RGBQUAD),256,fp);
fwrite(imgBuf,height * lineByte,1,fp);
fclose(fp);
return 1;

}

/****************************************************************************
*函数名称： bmpzoom()
*函数参数： const char* szSrcBmp 原bmp图片的路径及名称

                  const char* szDstBmp 变化后保存bmp图片后的文件路径及名称
*函数返回值:0为失败 1为成功
*函数描述： 传入图片变化比例系数参数FXZOOMRATIO和FYZOOMRATIO 实现图片放大缩小
*
***************************************************************************/

bool bmpzoom(const char* szSrcBmp, const char* szDstBmp)
{


readBmp(szSrcBmp);
newBmpWidth = (long) ((bmpWidth * FXZOOMRATIO) +0.5);

newBmpHeight = (long) (bmpHeight * FYZOOMRATIO +0.5);

newLineByte = (newBmpWidth * biBitCount/8+3)/4*4;

pNewBmpBuf = new unsigned char [newLineByte * newBmpHeight];


//printf("width = %d, height = %d,biBitCount = %d/n",bmpWidth,bmpHeight,biBitCount);
//printf("newwidth = %d, newheight = %d,biBitCount = %d/n",newBmpWidth,newBmpHeight,biBitCount);
    long i,j,k;
long i0,j0;
int lineByte =(bmpWidth*biBitCount/8+3)/4*4;
if(biBitCount==8)
{
   for(i = 0;i < bmpHeight/2;i++)
   {
    for(j = 0;j<bmpWidth/2;j++)
     *(pBmpBuf+i*lineByte+j) = 0;

   }
}

if(biBitCount == 24)
{
   for(i = 0;i < newBmpHeight;i++)
   {
    for(j = 0; j<newBmpWidth;j++)
     for(k=0;k<3;k++) 
    
    {   
     i0 = (long)(i/FYZOOMRATIO+0.5); 
     j0 = (long)(j/FXZOOMRATIO+0.5);
     if((j0 >=0) && (j0 < bmpWidth) && (i0 >=0)&& (i0 <bmpHeight))
     {
      
      *(pNewBmpBuf+i*newLineByte+j*3+k) = *(pBmpBuf+i0*lineByte+j0*3+k);
     }
     else
     {
      *(pNewBmpBuf+i*newLineByte+j*3+k)=255;
     }
    
    }
   }
}
saveBmp(szDstBmp,pNewBmpBuf,newBmpWidth,newBmpHeight,biBitCount,pColorTable);
delete []pNewBmpBuf;
if(biBitCount == 8)
   delete []pColorTable;
return 1;
}
/***********************************************************************
*函数名称：main()
*参数 空
*返回值 空
*描述：图像变化的访问
***************************************************************************/


void main()
{
char str1[80]; 
char str2[80];
printf("输入bmp文件路径及名称");
gets(str1);
printf("输入bmp文件保存的路径及名称");
gets(str2); 
bmpzoom(str1,str2);}