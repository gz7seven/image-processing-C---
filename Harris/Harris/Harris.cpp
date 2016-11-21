/*ԭ��
Harris�ǵ�����ֱ�۵Ľ�����:�����������໥��ֱ�ķ����ϣ����нϴ�仯�ĵ㡣
��harris�Ľǵ����У�ʹ�õ��Ǹ�˹���ڣ�����w(x,y)��ʾ���Ǹ�˹�����е�Ȩ�ء���ʱ ��u��vȡ�����໥��ֱ��ֵʱ��E(u,v)���нϴ�ֵ�ĵ㡣
��ʽ�Ƶ���
���ݶ���taylorչ��ʽ�ó���
��ο���E(u,v)�������໥��ֱ�ķ����϶�ȡ�����ֵ�أ����ǿ��������E(u,v)��һ�������ϵ����ֵΪr1����E(u1,v1) = r1,
Ȼ�����䴹ֱ�����ϵ�ֵr2����������ķ��������r1��r2�ֱ��ӦM����������ֵ����Ӧ������(u1,v1),(u2,v2)ΪM������������
�˴���M����ΪHessian����
�ж��Ƿ�Ϊ�ǵ�ı�׼:
r1,r2����С,��Ӧ��ͼ���е�ƽ������
r1,r2���ܴ󣬶�Ӧ��ͼ���еĽǵ�
r1��r2һ���ܴ�һ����С����Ӧ��ͼ���еı�Ե
Harris�ǵ������㹫ʽ: R = det(M) �C a*trace(M)^2,����aΪ����ֵ��ȡֵ��Χ����[0.04, 0.06],
��һ�ֽǵ������㷽��:R = det(M)/trace(M)�������ʽ���ɷ��ӣ�ֻҪ�ܷ�Ӧ�ǵ�������Ϳ�����
����det(M) = r1*r2 = AB �CC^2
    Trace(M) = r1+r2 =A+B ������A=Ix^2;;  B = Iy^2;;; C=IxIy��
�ǵ���Ĳ���
�������������ۣ�harris�ǵ���Ĳ�������ܽ�Ϊ:
    <1>����ͼ��I(x,y)��x��y����������ݶ�Ix,Iy��
    <2>�����ݶȷ���ĳ˻�
    <3>ʹ�ø�˹�˶�Ix^2,Iy^2,IxIy���м�Ȩ���������M��Ԫ��A��B��C
    <4>���ݼ���ǵ��������趨��ֵ�����ǵ���С����ֵ�����Ǻ�ѡ�ǵ�
    <5>���оֲ�����ֵ����
	*/
#include <iostream>  
#include "cv.h"  
#include "cxcore.h"  
#include "highgui.h"  
using namespace std;  

//harris �ǵ�����Ҫ�Ĳ���  
typedef struct HARRISPARAMS  
{  
    int gaussSize; //��˹����  
    float gaussSigma; //��˹����  
    double threshold; //�Խǵ����趨����ֵ  
    int maximumSize; //�ֲ�����ֵ����ʱ�Ĵ��ڴ�С  
      
}HARRISPARAMS,*PHARRISPARAMS;  
  
  
/******************************* 
*��Դͼ����о������ 
*������ 
*srcFloat    Դͼ�� 
*Ix          ����Ľ�� 
*dxTemplate  ���ģ�� 
*widthTemplate ģ��Ŀ�� 
*heightTemplate ģ��ĸ߶� 
********************************/  
void convolution(IplImage* srcFloat,IplImage* Ix,double* dxTemplate , int widthTemplate,int heightTemplate);  
  
  
/*************************** 
*harris�ǵ��⺯�� 
*������ 
*srcIn   Դͼ�� 
*params  harris�ǵ�����Ҫ�Ĳ��� 
*corners ���harris�ǵ����� 
****************************/  
void getHarrisPoints(IplImage* srcIn,PHARRISPARAMS params,CvSeq* corners);  
  
//������  
int main(int argc, char* argv[])  
{  
  
    //��ر���  
    IplImage* src,*srcGray;  
    CvMemStorage* mem = cvCreateMemStorage(0); //�����ڴ� 
    CvSeq* harrisPoints;  
    HARRISPARAMS harrisParams;  
  
    src = cvLoadImage("1.bmp");//Դͼ��  
      
    srcGray = cvCreateImage(cvGetSize(src),8,1);        // �Ҷ�ͼ��  
    if(!src)  
    {  
        cout << " src is null";  
        return 0;  
    }  
      
    cvCvtColor(src,srcGray,CV_BGR2GRAY);  
  
    //harris�ǵ㱣��Ŀռ�  �ǵ����걣����һ��������
    harrisPoints = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvPoint),mem);  

    //������ز���  
    harrisParams.gaussSize = 5;   // ��˹���ڵĴ�С  
    harrisParams.gaussSigma = 0.8;  
    harrisParams.threshold =15000000;  
    harrisParams.maximumSize = 21;  
  
    //����harris�ǵ���  
    getHarrisPoints(srcGray,&harrisParams,harrisPoints);  
      
    //��ȡÿһ���ǵ������  
    for(int x=0;x<harrisPoints->total;x++)  
    {  
        //��ȡ��x���ǵ������  
        CvPoint* pt = (CvPoint*)cvGetSeqElem(harrisPoints,x);  
  
        //�Խǵ�����Ϊ����  ����һ���뾶Ϊ2��Բ  
		//void cvCircle( CvArr* img, CvPoint center, int radius, CvScalar color, int thickness=1, int line_type=8, int shift=0 ); 
        cvCircle(src,*pt,3,cvScalar(0,0,255,0));
    }  
  
    //cvSaveImage("dst.jpg",src);  
  
    //��ʾͼ��  
    cvNamedWindow("dst");  
    cvShowImage("dst",src); 
    cvWaitKey(0);  
  
    //�ͷ���Դ  
    cvReleaseImage(&src);  
    cvReleaseImage(&srcGray);  
    cvReleaseMemStorage(&mem);  
    return 0;  
}  
  
/*************************** 
*harris�ǵ��⺯�� 
*������ 
*srcIn   Դͼ�� 
*params  harris�ǵ�����Ҫ�Ĳ��� 
*corners ���harris�ǵ����� 
****************************/  
void getHarrisPoints(IplImage* srcIn,PHARRISPARAMS params,CvSeq* corners)  
{  
    int x,y;  
  
    IplImage* srcFloat;  
      
    srcFloat = cvCreateImage(cvGetSize(srcIn),32,1);  
      
    cvConvertScale(srcIn,srcFloat); //ʹ�����Ա任ת������ 
      
    IplImage *Ix,*Iy,*IxIx,*IyIy,*IxIy,*A,*B,*C,*cornerness;  
    double *gaussWindow = new double[sizeof(double)*params->gaussSize*params->gaussSize];  
    //ˮƽ���������Ӳ���Ix  
     double dxTemplate[9]={-1,0,1,  
                           -1,0,1,  
                           -1,0,1};  
  
    //��ֱ���������Ӳ���Iy  
     double dyTemplate[9]={-1,-1,-1,  
                            0, 0, 0,  
                            1, 1, 1};  
      
     //�˴��ڴ��õ��е��ݳ� ���������޸� ��ʡ�ڴ�   
    Ix = cvCreateImage(cvGetSize(srcFloat),32,1);  
    Iy = cvCreateImage(cvGetSize(srcFloat),32,1);  
    IxIx = cvCreateImage(cvGetSize(srcFloat),32,1);  
    IyIy = cvCreateImage(cvGetSize(srcFloat),32,1);  
    IxIy = cvCreateImage(cvGetSize(srcFloat),32,1);  
    A = cvCreateImage(cvGetSize(srcFloat),32,1);  
    B = cvCreateImage(cvGetSize(srcFloat),32,1);  
    C = cvCreateImage(cvGetSize(srcFloat),32,1);  
    cornerness = cvCreateImage(cvGetSize(srcFloat),32,1); //����ǵ���  
  
    convolution(srcFloat,Ix,dxTemplate,3,3); //����Ix  
    convolution(srcFloat,Iy,dyTemplate,3,3); //����Iy   
  
    //����Ix2��Iy2��IxIy  
    for(y=0;y<srcFloat->height;y++)  
    {  
        for(x=0;x<srcFloat->width;x++)  
        {  
            float IxValue,IyValue;  
              
            IxValue = cvGetReal2D(Ix,y,x);//���ص�ͨ�������ָ��Ԫ�� ,yΪ��һ����Ա��xΪ�ڶ�����Ա
            IyValue = cvGetReal2D(Iy,y,x);  
  
            cvSetReal2D(IxIx,y,x,IxValue*IxValue);  
            cvSetReal2D(IyIy,y,x,IyValue*IyValue);  
            cvSetReal2D(IxIy,y,x,IxValue*IyValue);  
        }  
    }  
  
    //�����˹����  
    for( y=0;y<params->gaussSize;y++)  
    {  
        for( x=0;x<params->gaussSize;x++)  
        {  
              
            float dis,weight;  
              
            dis = (y-params->gaussSize/2)*(y-params->gaussSize/2)+(x-params->gaussSize/2)*(x-params->gaussSize/2);  
            weight = exp(-dis/(2.0*params->gaussSigma));  
            *(gaussWindow+y*params->gaussSize+x)=weight;       
        }  
    }  
       
    convolution(IxIx,A,gaussWindow,params->gaussSize,params->gaussSize);//����IxIx���˹�ľ��  
    convolution(IyIy,B,gaussWindow,params->gaussSize,params->gaussSize);//����IyIy���˹�ľ��  
    convolution(IxIy,C,gaussWindow,params->gaussSize,params->gaussSize);//����IxIy���˹�ľ��  
  
    //����ǵ���
    for(y=0;y<srcFloat->height;y++)  
    {  
        for(x=0;x<srcFloat->width;x++)  
        {  
            double cornernessValue,Avalue,Bvalue,Cvalue,a=0.05;  
            Avalue = cvGetReal2D(A,y,x);  
            Bvalue = cvGetReal2D(B,y,x);  
            Cvalue = cvGetReal2D(C,y,x);  
            //Harris�ǵ������㹫ʽ: R = det(M) �C a*trace(M)^2,����aΪ����ֵ��ȡֵ��Χ����[0.04, 0.06],
            //��һ�ֽǵ������㷽��:R = det(M)/trace(M)�������ʽ���ɷ��ӣ�ֻҪ�ܷ�Ӧ�ǵ�������Ϳ�����
            //����det(M) = r1*r2 = AB �CC^2,Trace(M) = r1+r2 =A+B ������A=Ix^2;;  B = Iy^2;;; C=IxIy�� 
            //��������õڶ��ַ��� 
            cornernessValue = (Avalue*Bvalue-Cvalue*Cvalue)-a*(Avalue+Bvalue)*(Avalue+Bvalue);  
              
            cvSetReal2D(cornerness,y,x,cornernessValue);    
        }  
      
    }  
  
    //����ֲ�����ֵ �� ����ֵ�Ƿ������ֵ  
    int beginY,endY,beginX,endX;  
    int halfWinSize = params->maximumSize/2;  
  
    beginY = halfWinSize;  
    endY = cornerness->height - halfWinSize;  
  
    beginX = halfWinSize;  
    endX = cornerness->width - halfWinSize;  
      
    for(y=beginY;y<endY;)  
    {  
        for(x=beginX;x<endX;)  
        {  
            //Ѱ�Ҿֲ�����ֵ ����λ����Ϣ  
            float maxValue=0;  
            int flag = 0 ;  
            CvPoint maxLoc;  
            maxLoc.x = -1;  
            maxLoc.y = -1;  
  
            //���ȼ����Ե�(x,y)λ���ĵ�maximumSize*maximumSize�Ĵ����ڲ��ľֲ�����ֵ  
            for(int winy=-halfWinSize;winy<=halfWinSize;winy++)  
            {  
                for(int winx=-halfWinSize;winx<=halfWinSize;winx++)  
                {  
                    float value ;  
                    value = cvGetReal2D(cornerness,y+winy,x+winx);  
                      
                    //����ô����� ���ֵ ���浽max �����������굽maxLoc  
                    if(value>maxValue)  
                    {  
                        maxValue = value;  
                        maxLoc.x = x+winx;  
                        maxLoc.y = y+winy;  
                        flag = 1;  
                    }  
                }  
            }  
  
              
            //����ҵ��ֲ�����ֵ ���Ҹ�ֵ����Ԥ���趨����ֵ ����Ϊ�ǽǵ�
			//�ж��Ƿ�Ϊ�ǵ�ı�׼:
            //r1,r2����С,��Ӧ��ͼ���е�ƽ������
            //r1,r2���ܴ󣬶�Ӧ��ͼ���еĽǵ�
            //r1��r2һ���ܴ�һ����С����Ӧ��ͼ���еı�Ե
            if(flag==1 && maxValue>params->threshold)  
            {  
                cvSeqPush(corners,&maxLoc);  
            }                     
            x = x+params->maximumSize;  
        }        
        y = y + params->maximumSize;  
    }  
      
    delete []gaussWindow;  
    cvReleaseImage(&Ix);  
    cvReleaseImage(&Iy);  
    cvReleaseImage(&IxIx);  
    cvReleaseImage(&IyIy);  
    cvReleaseImage(&IxIy);  
    cvReleaseImage(&A);  
    cvReleaseImage(&B);  
    cvReleaseImage(&C);  
    cvReleaseImage(&cornerness);  
    cvReleaseImage(&srcFloat);  
}  
/******************************* 
*��Դͼ����о������ 
*������ 
*srcFloat    Դͼ�� 
*Ix          ����Ľ�� 
*dxTemplate  ���ģ�� 
*widthTemplate ģ��Ŀ�� 
*heightTemplate ģ��ĸ߶� 
********************************/  
void convolution(IplImage* srcFloat,IplImage* Ix,double* dxTemplate , int widthTemplate,int heightTemplate)  
{  
    int x,y,beginY,endY,beginX,endX;  
      
    beginY = heightTemplate/2;  
    endY = srcFloat->height - heightTemplate/2;  
      
    beginX = widthTemplate/2;  
    endX = srcFloat->width - widthTemplate/2;  
      
    for(y=beginY;y<endY;y++)  
    {  
        for(x=beginX;x<endX;x++)  
        {        
            int i,j;  
            double curDx=0;  
            for(i=0;i<heightTemplate;i++)  
            {  
                for(j=0;j<widthTemplate;j++)  
                {  
                    curDx += cvGetReal2D(srcFloat,y+i-heightTemplate/2,x+j-widthTemplate/2)**(dxTemplate+i*widthTemplate+j);  
                }  
            }         
            cvSetReal2D(Ix,y,x,curDx);  
        }     
    }  
}  