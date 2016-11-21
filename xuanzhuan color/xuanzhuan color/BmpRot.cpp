#include <stdio.h>  
#include "BmpRot.h"  
#include "stdlib.h"  
#include "math.h"  
#include <iostream>  
  
#define PI 3.14159//Բ���ʺ궨��  
#define LENGTH_NAME_BMP 30//bmpͼƬ�ļ�������󳤶�  
  
using namespace std;  
  
//��������  
BITMAPFILEHEADER strHead;  
RGBQUAD strPla[256];//256ɫ��ɫ��  
BITMAPINFOHEADER strInfo;  
  
//��ʾλͼ�ļ�ͷ��Ϣ  
void showBmpHead(BITMAPFILEHEADER pBmpHead){  
    cout<<"λͼ�ļ�ͷ:"<<endl;  
    cout<<"�ļ���С:"<<pBmpHead.bfSize<<endl;  
    cout<<"������_1:"<<pBmpHead.bfReserved1<<endl;  
    cout<<"������_2:"<<pBmpHead.bfReserved2<<endl;  
    cout<<"ʵ��λͼ���ݵ�ƫ���ֽ���:"<<pBmpHead.bfOffBits<<endl<<endl;  
}  
  
void showBmpInforHead(tagBITMAPINFOHEADER pBmpInforHead){  
    cout<<"λͼ��Ϣͷ:"<<endl;  
    cout<<"�ṹ��ĳ���:"<<pBmpInforHead.biSize<<endl;  
    cout<<"λͼ��:"<<pBmpInforHead.biWidth<<endl;  
    cout<<"λͼ��:"<<pBmpInforHead.biHeight<<endl;  
    cout<<"biPlanesƽ����:"<<pBmpInforHead.biPlanes<<endl;  
    cout<<"biBitCount������ɫλ��:"<<pBmpInforHead.biBitCount<<endl;  
    cout<<"ѹ����ʽ:"<<pBmpInforHead.biCompression<<endl;  
    cout<<"biSizeImageʵ��λͼ����ռ�õ��ֽ���:"<<pBmpInforHead.biSizeImage<<endl;  
    cout<<"X����ֱ���:"<<pBmpInforHead.biXPelsPerMeter<<endl;  
    cout<<"Y����ֱ���:"<<pBmpInforHead.biYPelsPerMeter<<endl;  
    cout<<"ʹ�õ���ɫ��:"<<pBmpInforHead.biClrUsed<<endl;  
    cout<<"��Ҫ��ɫ��:"<<pBmpInforHead.biClrImportant<<endl;  
}  
  
  
int main(){  
    char strFile[LENGTH_NAME_BMP];//bmp�ļ���  
    IMAGEDATA *imagedata = NULL;//��̬����洢ԭͼƬ��������Ϣ�Ķ�ά����  
    IMAGEDATA *imagedataRot = NULL;//��̬����洢��ת���ͼƬ��������Ϣ�Ķ�ά����  
    int width,height;//ͼƬ�Ŀ�Ⱥ͸߶�  
    //cout<<"��������Ҫ��ȡ���ļ���:"<<endl;  
    //cin>>strFile;  
    FILE *fpi,*fpw;  
    fpi=fopen("C:/12.bmp","rb");  
    if(fpi != NULL){  
        //�ȶ�ȡ�ļ�����  
        WORD bfType;  
        fread(&bfType,1,sizeof(WORD),fpi);  
        if(0x4d42!=bfType)  
        {  
            cout<<"the file is not a bmp file!"<<endl;  
            return NULL;  
        }  
        //��ȡbmp�ļ����ļ�ͷ����Ϣͷ  
        fread(&strHead,1,sizeof(tagBITMAPFILEHEADER),fpi);  
        //showBmpHead(strHead);//��ʾ�ļ�ͷ  
        fread(&strInfo,1,sizeof(tagBITMAPINFOHEADER),fpi);  
        //showBmpInforHead(strInfo);//��ʾ�ļ���Ϣͷ  
  
        //��ȡ��ɫ��  
        for(unsigned int nCounti=0;nCounti<strInfo.biClrUsed;nCounti++)  
        {  
            //�洢��ʱ��һ��ȥ��������rgbReserved  
            fread((char *)&strPla[nCounti].rgbBlue,1,sizeof(BYTE),fpi);  
            fread((char *)&strPla[nCounti].rgbGreen,1,sizeof(BYTE),fpi);  
            fread((char *)&strPla[nCounti].rgbRed,1,sizeof(BYTE),fpi);  
            cout<<"strPla[nCounti].rgbBlue"<<strPla[nCounti].rgbBlue<<endl;  
            cout<<"strPla[nCounti].rgbGreen"<<strPla[nCounti].rgbGreen<<endl;  
            cout<<"strPla[nCounti].rgbRed"<<strPla[nCounti].rgbRed<<endl;  
        }  
  
        width = strInfo.biWidth;  
        height = strInfo.biHeight;  
        imagedata = (IMAGEDATA*)malloc(width * height * sizeof(IMAGEDATA));  
        imagedataRot = (IMAGEDATA*)malloc(2 * width * 2 * height * sizeof(IMAGEDATA));  
        //��ʼ��ԭʼͼƬ����������  
        for(int i = 0;i < height;++i)  
        {  
            for(int j = 0;j < width;++j)  
            {  
                (*(imagedata + i * width + j)).blue = 0;  
                (*(imagedata + i * width + j)).green = 0;  
                (*(imagedata + i *  width + j)).red = 0;  
            }  
        }  
        //��ʼ����ת��ͼƬ����������  
        for(int i = 0;i < 2 * height;++i)  
        {  
            for(int j = 0;j < 2 * width;++j)  
            {  
                (*(imagedataRot + i * 2 * width + j)).blue = 0;  
                (*(imagedataRot + i * 2 * width + j)).green = 0;  
                (*(imagedataRot + i * 2 * width + j)).red = 0;  
            }  
        }  
        //fseek(fpi,54,SEEK_SET);  
        //����ͼƬ����������  
        fread(imagedata,sizeof(struct tagIMAGEDATA) * width,height,fpi);  
        /* 
        for(int i = 0;i < height;++i) 
        { 
            fread(imagedata + i * width * sizeof(IMAGEDATA),sizeof(struct tagIMAGEDATA) * width,height,fpi); 
        }*/  
        fclose(fpi);  
    }  
    else  
    {  
        cout<<"file open error!"<<endl;  
        return NULL;  
    }  
  
    //ͼƬ��ת����  
    int RotateAngle;//Ҫ��ת�ĽǶ���  
    double angle;//Ҫ��ת�Ļ�����  
    int midX_pre,midY_pre,midX_aft,midY_aft;//��ת��Χ�Ƶ����ĵ������  
    midX_pre = width / 2;  
    midY_pre = height / 2;  
    midX_aft = width;  
    midY_aft = height;  
    int pre_i,pre_j,after_i,after_j;//��תǰ���Ӧ�����ص�����  
    cout<<"����Ҫ��ת�ĽǶȣ�0�ȵ�360�ȣ���ʱ����ת����"<<endl;  
    cin>>RotateAngle;  
    angle = 1.0 * RotateAngle * PI / 180;  
    for(int i = 0;i < 2 * height;++i)  
    {  
        for(int j = 0;j < 2 * width;++j)  
        {  
            after_i = i - midY_aft;//����任  
            after_j = j - midX_aft;  
            pre_i = (int)(cos((double)angle) * after_i - sin((double)angle) * after_j) + midY_pre;  
            pre_j = (int)(sin((double)angle) * after_i + cos((double)angle) * after_j) + midX_pre;  
            if(pre_i >= 0 && pre_i < height && pre_j >= 0 && pre_j < width)//��ԭͼ��Χ��  
                *(imagedataRot + i * 2 * width + j) = *(imagedata + pre_i * width + pre_j);  
        }  
    }  
  
    //����bmpͼƬ  
    if((fpw=fopen("b.bmp","wb"))==NULL)  
    {  
        cout<<"create the bmp file error!"<<endl;  
        return NULL;  
    }  
    WORD bfType_w=0x4d42;  
    fwrite(&bfType_w,1,sizeof(WORD),fpw);  
    //fpw +=2;  
    fwrite(&strHead,1,sizeof(tagBITMAPFILEHEADER),fpw);  
    strInfo.biWidth = 2 * width;  
    strInfo.biHeight = 2 * height;  
    fwrite(&strInfo,1,sizeof(tagBITMAPINFOHEADER),fpw);  
    //�����ɫ������  
    for(unsigned int nCounti=0;nCounti<strInfo.biClrUsed;nCounti++)  
    {  
        fwrite(&strPla[nCounti].rgbBlue,1,sizeof(BYTE),fpw);  
        fwrite(&strPla[nCounti].rgbGreen,1,sizeof(BYTE),fpw);  
        fwrite(&strPla[nCounti].rgbRed,1,sizeof(BYTE),fpw);  
    }  
    //������������  
    for(int i =0;i < 2 * height;++i)  
    {  
        for(int j = 0;j < 2 * width;++j)  
        {  
            fwrite( &(*(imagedataRot + i * 2 * width + j)).red,1,sizeof(BYTE),fpw);//ע����������˳�򣺷�����ɫ�ᷢ���仯  
            fwrite( &(*(imagedataRot + i * 2 * width + j)).green,1,sizeof(BYTE),fpw);  
            fwrite( &(*(imagedataRot + i * 2 * width + j)).blue,1,sizeof(BYTE),fpw);  
        }  
    }  
    fclose(fpw);  
  
    //�ͷ��ڴ�  
    delete[] imagedata;  
    delete[] imagedataRot;  
}  