#include"stdafx.h"

int main(int argc,char argv[])
{
    int x,y,i; 
    IplImage* pSrc = cvLoadImage("street.jpg", 1);             // 载入图像 ;
    int width = pSrc->width;                                   //图像宽度
    int height = pSrc->height;                                 //图像高度
    int depth = pSrc->depth;                                   //图像位深(IPL_DEPTH_8U...)
    int channels = pSrc->nChannels;                            //图像通道数(1、2、3、4)
    int pSrcSize = pSrc->imageSize;                            //图像大小 imageSize = height*widthStep
    int step = pSrc->widthStep/sizeof(uchar);   
    uchar* data    = (uchar *)pSrc->imageData;
    int imageLen = width*height;   
    
    //============① 获取原彩色图像的梯度值(先转灰度)===========//
	LONG imagelen = width*height;
    INT* deltar = new INT[imageLen];                            //梯度模数组
    IplImage* gray  = cvCreateImage(cvGetSize(pSrc),pSrc->depth,1);
    cvCvtColor(pSrc,gray,CV_BGR2GRAY);
    GetGra(gray,deltar);

    //=======================② 梯度值排序======================//
	MyImageGraPtWatershed*  graposarr = new MyImageGraPtWatershed[imageLen];
    INT*  gradientadd = new INT[257];                           //各梯度起终位置；
    memset( gradientadd, 0, 257*sizeof(INT));
	LONG xstart, imagepos, deltapos;
    xstart = imagepos = deltapos = 0;
	GradSort(deltar, width, height, graposarr, gradientadd); 
 
    //=============③ 泛洪得每个像素所属区域标记flag==============//
    INT rgnumber = 0;                                             //分割后的区域数
    INT*   flag = new INT[imageLen];                              //各点标识数组
    Flood(graposarr, gradientadd, 0, 255, flag, rgnumber, width, height);   
 
    //===================④ 确定各个像素所属区域===================//
	INT* mergearr = new INT[rgnumber+1];
    for (y=0; y<(height); y++)
    {
        xstart = y*width;
        for ( x=0; x<(width); x++)
        {
            INT pos = xstart + x;
            INT rgid = flag[pos];                                   //该点所属区域；
            flag[pos] = FindMergedRgn(rgid, mergearr);
        }
    }
    delete [] mergearr;
    mergearr = NULL;
 
    //===================⑤ 绘制区域轮廓，保存结果===================//
    //以下根据标识数组查找边界点（不管四边点以减小复杂度）
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
 
    //  释放资源
    delete [] gradientadd; 
	gradientadd = NULL;//大小256
    delete [] deltar;    
	deltar    = NULL;//大小imageLen
    delete [] graposarr; 
	graposarr = NULL;//大小imageLen
    cvReleaseImage(&pSrc);
    cvReleaseImage(&gray); 
     
}