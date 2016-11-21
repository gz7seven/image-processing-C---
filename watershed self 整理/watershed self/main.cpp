#include"stdafx.h"

int main(int argc,char argv[])
{
    int x,y,i; 
    IplImage* pSrc = cvLoadImage("street.jpg", 1);             // ����ͼ�� ;
    int width = pSrc->width;                                   //ͼ����
    int height = pSrc->height;                                 //ͼ��߶�
    int depth = pSrc->depth;                                   //ͼ��λ��(IPL_DEPTH_8U...)
    int channels = pSrc->nChannels;                            //ͼ��ͨ����(1��2��3��4)
    int pSrcSize = pSrc->imageSize;                            //ͼ���С imageSize = height*widthStep
    int step = pSrc->widthStep/sizeof(uchar);   
    uchar* data    = (uchar *)pSrc->imageData;
    int imageLen = width*height;   
    
    //============�� ��ȡԭ��ɫͼ����ݶ�ֵ(��ת�Ҷ�)===========//
	LONG imagelen = width*height;
    INT* deltar = new INT[imageLen];                            //�ݶ�ģ����
    IplImage* gray  = cvCreateImage(cvGetSize(pSrc),pSrc->depth,1);
    cvCvtColor(pSrc,gray,CV_BGR2GRAY);
    GetGra(gray,deltar);

    //=======================�� �ݶ�ֵ����======================//
	MyImageGraPtWatershed*  graposarr = new MyImageGraPtWatershed[imageLen];
    INT*  gradientadd = new INT[257];                           //���ݶ�����λ�ã�
    memset( gradientadd, 0, 257*sizeof(INT));
	LONG xstart, imagepos, deltapos;
    xstart = imagepos = deltapos = 0;
	GradSort(deltar, width, height, graposarr, gradientadd); 
 
    //=============�� �����ÿ����������������flag==============//
    INT rgnumber = 0;                                             //�ָ���������
    INT*   flag = new INT[imageLen];                              //�����ʶ����
    Flood(graposarr, gradientadd, 0, 255, flag, rgnumber, width, height);   
 
    //===================�� ȷ������������������===================//
	INT* mergearr = new INT[rgnumber+1];
    for (y=0; y<(height); y++)
    {
        xstart = y*width;
        for ( x=0; x<(width); x++)
        {
            INT pos = xstart + x;
            INT rgid = flag[pos];                                   //�õ���������
            flag[pos] = FindMergedRgn(rgid, mergearr);
        }
    }
    delete [] mergearr;
    mergearr = NULL;
 
    //===================�� ��������������������===================//
    //���¸��ݱ�ʶ������ұ߽�㣨�����ıߵ��Լ�С���Ӷȣ�
    IplImage* dstContour = cvCreateImage(cvGetSize(gray),IPL_DEPTH_8U,3);
    for (y=1; y<(height-1); y++)
    {
        xstart = y*width;
        for ( x=1; x<(width-1); x++)
        {
            INT pos = xstart + x;
            INT imagepos = pos * 3;
            INT left = pos - 1;
            INT up = pos - width;
            INT right = pos +1;
            INT down = pos + width;
            if ( ( flag[right]!=flag[pos] ) || ( flag[down]!=flag[pos] ) )
            {
                ((uchar *)(dstContour->imageData+y*dstContour->widthStep))[x*dstContour->nChannels+0] = 0;
                ((uchar *)(dstContour->imageData+y*dstContour->widthStep))[x*dstContour->nChannels+1] = 0;
                ((uchar *)(dstContour->imageData+y*dstContour->widthStep))[x*dstContour->nChannels+2] = 255;
            }
        }
    }
    
    IplImage* dstContourGray= cvCreateImage(cvGetSize(dstContour),8,1);
    cvCvtColor(dstContour,dstContourGray,CV_BGR2GRAY);

	cvNamedWindow("src");
    cvShowImage("src",pSrc);
	cvNamedWindow("gray");
    cvShowImage("gray",gray);
	cvNamedWindow("dstContour",1);
    cvShowImage("dstContour",dstContour);
    cvNamedWindow("dstContourGray",1);
    cvShowImage("dstContourGray",dstContourGray); 
    cvWaitKey(0);
    cvDestroyAllWindows();
 
    //  �ͷ���Դ
    delete [] gradientadd; 
	gradientadd = NULL;//��С256
    delete [] deltar;    
	deltar    = NULL;//��СimageLen
    delete [] graposarr; 
	graposarr = NULL;//��СimageLen
    cvReleaseImage(&pSrc);
    cvReleaseImage(&gray); 
     
}