// testcanny.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <stack>
#include <vector> 
#include <windows.h>
#include <math.h>
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

///////////Canny by love6tao//////////
//
//
////////////////////////


//int TraceEdge(int w, int h, int nThrLow, IplImage* pResult, int *pMag)
//{
//	int n,m;
//	char flag = 0;
//    int currentvalue=(uchar)pResult->imageData[w+h*pResult->widthStep];
//	if ( currentvalue== 0)
//	{
//		pResult->imageData[w+h*pResult->widthStep]= 255;
//		flag=0;
//		for (n= -1; n<=1; n++)
//		{
//			for(m= -1; m<=1; m++)
//			{
//				if (n==0 && m==0) continue;
//				int curgrayvalue=(uchar)pResult->imageData[w+n+(h+m)*pResult->widthStep];
//				int curgrdvalue=pMag[w+n+(h+m)*pResult->width];
//				if (curgrayvalue==200&&curgrdvalue>nThrLow)
//					if (TraceEdge(w+n, h+m, nThrLow, pResult, pMag))
//					{
//						flag=1;
//						break;
//					}
//			}
//			if (flag) break;
//		}
//		return(1);
//	}
//	return(0);
//}


void TraceEdge(int w, int h, int nThrLow, IplImage* pResult, int *pMag)  
{ 
	//��8�������ؽ��в�ѯ  
	int xNum[8] = {1,1,0,-1,-1,-1,0,1};  
	int yNum[8] = {0,1,1,1,0,-1,-1,-1};
    int xx=0;
	int yy=0;
	bool change=true;
	while(change)
	{
		change=false;
		for(int k=0;k<8;k++)  
		{  
			xx=w+xNum[k];
			yy=h+yNum[k];
			//   ���������Ϊ���ܵı߽�㣬��û�д����  
			//   �����ݶȴ�����ֵ  
			int curgrayvalue=(uchar)pResult->imageData[xx+yy*pResult->widthStep];
			int curgrdvalue=pMag[xx+yy*pResult->width];
			if(curgrayvalue==200&&curgrdvalue>nThrLow)  
			{  
				change=true;
				//   �Ѹõ����ó�Ϊ�߽��  
				pResult->imageData[xx+yy*pResult->widthStep]=255;
				h=yy;
				w=xx;
				break;
			}  
		}  
	}
}





int _tmain(int argc, _TCHAR* argv[])
{
////////��һ�����ҶȻ� 
///////���ֵ�������� �ߺͿ���� �����Ǹ߲��ܳ�widthStep
	IplImage * ColorImage=cvLoadImage("c:\\lena.bmp",1);
	if (ColorImage==NULL)
	{
		printf("image read error");
		return 0;
	}
	cvNamedWindow("Sourceimg",0);  
	cvShowImage("Sourceimg",ColorImage);               // 
	IplImage * OpenCvGrayImage;
	OpenCvGrayImage=cvCreateImage(cvGetSize(ColorImage),ColorImage->depth,1);
    float data1,data2,data3;
	for (int i=0;i<ColorImage->height;i++)
	{
		for (int j=0;j<ColorImage->width;j++)
		{
			data1=(uchar)(ColorImage->imageData[i*ColorImage->widthStep+j*3+0]);
			data2=(uchar)(ColorImage->imageData[i*ColorImage->widthStep+j*3+1]);
			data3=(uchar)(ColorImage->imageData[i*ColorImage->widthStep+j*3+2]);
			OpenCvGrayImage->imageData[i*OpenCvGrayImage->widthStep+j]=(uchar)(0.07*data1 + 0.71*data2 + 0.21*data3);
		}
	}
	//cvNamedWindow("GrayImage",0);  
	//cvShowImage("GrayImage",OpenCvGrayImage);               //��ʾ�Ҷ�ͼ  
	//cvWaitKey(0);  
	//cvDestroyWindow("GrayImage");  

//////////////////////////////////////////////////////////
////////�ڶ�������˹�˲� 
///////���ֵ�������� forѭ��i,j ��ʱ����Ҫ���� ���Ҷ������������Ŀ
//////����ʱ��widthStep ������width����������ͼ���������4�ı���������־�ݣ�����widthstep�Ͳ������
	double nSigma=0.2;
	int nWindowSize=1+2*ceil(3*nSigma);//ͨ��sigma�õ����ڴ�С
	int nCenter=nWindowSize/2;
	int nWidth=OpenCvGrayImage->width;
	int nHeight=OpenCvGrayImage->height;
	IplImage * pCanny;
	pCanny=cvCreateImage(cvGetSize(ColorImage),ColorImage->depth,1);
	//���ɶ�ά�˲���
    double *pKernel_2=new double[nWindowSize*nWindowSize];
	double d_sum=0.0;
	for(int i=0;i<nWindowSize;i++)
	{
		for (int j=0;j<nWindowSize;j++)
		{
			double n_Disx=i-nCenter;//ˮƽ��������������ؾ���
			double n_Disy=j-nCenter;
			pKernel_2[j*nWindowSize+i]=exp(-0.5*(n_Disx*n_Disx+n_Disy*n_Disy)/(nSigma*nSigma))/(2.0*3.1415926)*nSigma*nSigma; 
			d_sum=d_sum+pKernel_2[j*nWindowSize+i];
		}
	}
	for(int i=0;i<nWindowSize;i++)//��һ������
	{
		for (int j=0;j<nWindowSize;j++)
		{
          pKernel_2[j*nWindowSize+i]=pKernel_2[j*nWindowSize+i]/d_sum;
		}
	}
	//���ģ��
	for (int i=0;i<nWindowSize*nWindowSize;i++)
	{
		if (i%(nWindowSize)==0)
		{
          printf("\n");
		}
		printf("%.10f ",pKernel_2[i]);
	}
	//�˲�����
	for (int s=0;s<nWidth;s++)
	{
		for (int t=0;t<nHeight;t++)
		{
			double dFilter=0;
			double dSum=0;
			//��ǰ���꣨s,t��
			//��ȡ8����
			for (int x=-nCenter;x<=nCenter;x++)
			{
				for (int y=-nCenter;y<=nCenter;y++)
				{
					if ((x+s>=0)&&(x+s<nWidth)&&(y+t>=0)&&(y+t<nHeight))//�ж��Ƿ�Խ��
					{
						double currentvalue=(double)OpenCvGrayImage->imageData[(y+t)*OpenCvGrayImage->widthStep+x+s];
						dFilter+=currentvalue*pKernel_2[x+nCenter+(y+nCenter)*nCenter];
						dSum+=pKernel_2[x+nCenter+(y+nCenter)*nCenter];
					}
				}
			}
			pCanny->imageData[t*pCanny->widthStep+s]=(uchar)(dFilter/dSum);
		}
	}
   


	//cvNamedWindow("GaussImage",0);  
	//cvShowImage("GaussImage",pCanny);               //��ʾ��˹ͼ
	//cvWaitKey(0);  
	//cvDestroyWindow("GaussImage"); 


//////////////////////////////////////////////////////////
////////�������������ݶ�ֵ�ͷ��� 
//////////////////ͬ�������ò�ͬ�ļ����////////////////���ϰ�ͼ��ŵ�0-255֮��//////  
/////    P[i,j]=(S[i+1,j]-S[i,j]+S[i+1,j+1]-S[i,j+1])/2     /////  
/////    Q[i,j]=(S[i,j]-S[i,j+1]+S[i+1,j]-S[i+1,j+1])/2     /////  
///////////////////////////////////////////////////////////////// 
   double *P=new double[nWidth*nHeight];
   double *Q=new double[nWidth*nHeight];
   int *M=new int[nWidth*nHeight];
   //IplImage * M;//�ݶȽ��
   //M=cvCreateImage(cvGetSize(ColorImage),ColorImage->depth,1);
   double *Theta=new double[nWidth*nHeight];
   int nwidthstep=pCanny->widthStep;
   for(int iw=0;iw<nWidth-1;iw++)
   {
	   for (int jh=0;jh<nHeight-1;jh++)
	   {
		   P[jh*nWidth+iw]=(double)(pCanny->imageData[min(iw+1,nWidth-1)+jh*nwidthstep]-pCanny->imageData[iw+jh*nwidthstep]+
		   pCanny->imageData[min(iw+1,nWidth-1)+min(jh+1,nHeight-1)*nwidthstep]-pCanny->imageData[iw+min(jh+1,nHeight-1)*nwidthstep])/2;
		   Q[jh*nWidth+iw]=(double)(pCanny->imageData[iw+jh*nwidthstep]-pCanny->imageData[iw+min(jh+1,nHeight-1)*nwidthstep]+
		   pCanny->imageData[min(iw+1,nWidth-1)+jh*nwidthstep]-pCanny->imageData[min(iw+1,nWidth-1)+min(jh+1,nHeight-1)*nwidthstep])/2;
		}
	}
//�����ֵ�ͷ���
	for(int iw=0;iw<nWidth-1;iw++)
	{
		for (int jh=0;jh<nHeight-1;jh++)
		{
           M[jh*nWidth+iw]=(int)sqrt(P[jh*nWidth+iw]*P[jh*nWidth+iw]+Q[jh*nWidth+iw]*Q[jh*nWidth+iw]+0.5);
           Theta[jh*nWidth+iw]=atan2(Q[jh*nWidth+iw],P[jh*nWidth+iw])*180/3.1415;
		   if (Theta[jh*nWidth+iw]<0)
		   {
			   Theta[jh*nWidth+iw]=360+Theta[jh*nWidth+iw];
		   }
		}
	}
//////////////////////////////////////////////////////////
////////���Ĳ����Ǽ���ֵ����
//ע������ Ȩ�ص�ѡȡ��Ҳ������ý�Ȩ�ش�
///////////////////////////////////////////////////////////////// 
	IplImage * N;//�Ǽ���ֵ���ƽ��
	N=cvCreateImage(cvGetSize(ColorImage),ColorImage->depth,1);
	IplImage * OpencvCannyimg;//�Ǽ���ֵ���ƽ��
	OpencvCannyimg=cvCreateImage(cvGetSize(ColorImage),ColorImage->depth,1);
	int g1=0, g2=0, g3=0, g4=0;                            //���ڽ��в�ֵ���õ������ص�����ֵ   
	double dTmp1=0.0, dTmp2=0.0;                           //�������������ص��ֵ�õ��ĻҶ����� 
    double dWeight=0.0;                                    //��ֵ��Ȩ��  

	for(int i=1;i<nWidth-1;i++)
	{
		for(int j=1;j<nHeight-1;j++)
		{
			//�����ǰ���ݶ�Ϊ0���õ�Ͳ��Ǳ�Ե��
			if (M[i+j*nWidth]==0)
			{
				N->imageData[i+j*nwidthstep]=0;
			}else
			{
				////////�����ж��������������Ȼ����������ֵ///////  
				////////////////////��һ�����///////////////////////  
				/////////       g1  g2                  /////////////  
				/////////           C                   /////////////  
				/////////           g3  g4              /////////////  
				/////////////////////////////////////////////////////  
                if((Theta[i+j*nWidth]>=90&&Theta[i+j*nWidth]<135)||(Theta[i+j*nWidth]>=270&&Theta[i+j*nWidth]<315))
				{
                   g1=M[i-1+(j-1)*nWidth];
				   g2=M[i+(j-1)*nWidth];
				   g3=M[i+(j+1)*nWidth];
				   g4=M[i+1+(j+1)*nWidth];
				   dWeight=fabs(P[i+j*nWidth])/fabs(Q[i+j*nWidth]); 
				   dTmp1=g1*dWeight+(1-dWeight)*g2;
				   dTmp2=g4*dWeight+(1-dWeight)*g3;
				   ////////////////////�ڶ������///////////////////////  
				   /////////       g1                      /////////////  
				   /////////       g2  C   g3              /////////////  
				   /////////               g4              /////////////  
				   /////////////////////////////////////////////////////  
				}else if((Theta[i+j*nWidth]>=135&&Theta[i+j*nWidth]<180)||(Theta[i+j*nWidth]>=315&&Theta[i+j*nWidth]<360))
				{
					g1=M[i-1+(j-1)*nWidth];
					g2=M[i-1+(j)*nWidth];
					g3=M[i+1+(j)*nWidth];
					g4=M[i+1+(j+1)*nWidth];
					dWeight=fabs(Q[i+j*nWidth])/fabs(P[i+j*nWidth]); 
					dTmp1=g1*dWeight+(1-dWeight)*g2;
					dTmp2=g4*dWeight+(1-dWeight)*g3;
					////////////////////���������///////////////////////  
					/////////           g1  g2              /////////////  
					/////////           C                   /////////////  
					/////////       g4  g3                  /////////////  
					/////////////////////////////////////////////////////  
				}else if((Theta[i+j*nWidth]>=45&&Theta[i+j*nWidth]<90)||(Theta[i+j*nWidth]>=225&&Theta[i+j*nWidth]<270))
				{
					g1=M[i+1+(j-1)*nWidth];
					g2=M[i+(j-1)*nWidth];
					g3=M[i+(j+1)*nWidth];
					g4=M[i-1+(j+1)*nWidth];
					dWeight=fabs(P[i+j*nWidth])/fabs(Q[i+j*nWidth]); 
					dTmp1=g1*dWeight+(1-dWeight)*g2;
					dTmp2=g4*dWeight+(1-dWeight)*g3;
					////////////////////���������///////////////////////  
					/////////               g1              /////////////  
					/////////       g4  C   g2              /////////////  
					/////////       g3                      /////////////  
					/////////////////////////////////////////////////////  
				}else if((Theta[i+j*nWidth]>=0&&Theta[i+j*nWidth]<45)||(Theta[i+j*nWidth]>=180&&Theta[i+j*nWidth]<225))
				{
					g1=M[i+1+(j-1)*nWidth];
					g2=M[i+1+(j)*nWidth];
					g3=M[i-1+(j)*nWidth];
					g4=M[i-1+(j+1)*nWidth];
					dWeight=fabs(Q[i+j*nWidth])/fabs(P[i+j*nWidth]); 
					dTmp1=g1*dWeight+(1-dWeight)*g2;
					dTmp2=g4*dWeight+(1-dWeight)*g3;

				}

			}

			if ((M[i+j*nWidth]>=dTmp1)&&(M[i+j*nWidth]>=dTmp2))
			{
                  N->imageData[i+j*nwidthstep]=200;

			}else  N->imageData[i+j*nwidthstep]=0;

		}
	}

	
	//cvNamedWindow("Limteimg",0);  
	//cvShowImage("Limteimg",N);               //��ʾ������
	//cvWaitKey(0);  
	//cvDestroyWindow("Limteimg"); 

//////////////////////////////////////////////////////////
////////���岽��˫��ֵ��ѡȡ
//ע������  ע�����ݶȷ�ֵ����ֵ  
///////////////////////////////////////////////////////////////// 
  int nHist[1024];//ֱ��ͼ
  int nEdgeNum;//���б�Ե�����Ŀ
  int nMaxMag=0;//����ݶȵķ�ֵ
  for(int k=0;k<1024;k++)
  {
	  nHist[k]=0;
  }
  for (int wx=0;wx<nWidth;wx++)
  {
	  for (int hy=0;hy<nHeight;hy++)
	  {
		  if((uchar)N->imageData[wx+hy*N->widthStep]==200)
		  {
			  int Mindex=M[wx+hy*nWidth];
			  nHist[M[wx+hy*nWidth]]++;//��ȡ���ݶ�ֱ��ͼ
			  
		  }
	  }
  }
  nEdgeNum=0;
  for (int index=1;index<1024;index++)
  {
	  if (nHist[index]!=0)
	  {
          nMaxMag=index;
	  }
	  nEdgeNum+=nHist[index];//����non-maximum suppression���ж��ٱ�Ե������  
  }
//����������ֵ ע�����ݶȵ���ֵ
  int nThrHigh;
  int nThrLow;
  double dRateHigh=0.7;
  double dRateLow=0.5;
  int nHightcount=(int)(dRateHigh*nEdgeNum+0.5);
  int count=1;
  nEdgeNum=nHist[1];
  while((nEdgeNum<=nHightcount)&&(count<nMaxMag-1))
  {
      count++;
	  nEdgeNum+=nHist[count];
  }
  nThrHigh=count;
  nThrLow= (int)(nThrHigh*dRateLow+0.5);
  printf("\nֱ��ͼ�ĳ��� %d \n  ",nMaxMag);
  printf("\n�ݶȵ���ֵ��ֵ�� %d С��ֵ %d \n  ",nThrHigh,nThrLow);


 //////////////////////////////////////////////////////////
 ////////����������Ե���
 /////////////////////////////////////////////////////////////////

  for(int is=1;is<nWidth-1;is++)
  {
	  for (int jt=1;jt<nHeight-1;jt++)
	  {
		  //CvScalar s=cvGet2D(N,jt,is);
          //int currentvalue=s.val[0];
		  int currentvalue=(uchar)(N->imageData[is+jt*N->widthStep]);
		  if ((currentvalue==200)&&(M[is+jt*nWidth]>=nThrHigh))
			 //�Ƿ�������ƺ�ĵ��� �ݶȷ�ֵҪ���ڸ���ֵ
		  {
              N->imageData[is+jt*nwidthstep]=255;
			  //������ж�
			   TraceEdge(is, jt, nThrLow, N, M);  
		  }
	  }
  }
    for (int si=1;si<nWidth-1;si++)
    {
		for (int tj=1;tj<nHeight-1;tj++)
		{
			if ((uchar)N->imageData[si+tj*nwidthstep]!=255)
			{
               N->imageData[si+tj*nwidthstep]=0;
			}
		}

    }

	cvNamedWindow("Cannyimg",0);  
	cvShowImage("Cannyimg",N);               

	cvCanny(OpenCvGrayImage,OpencvCannyimg,80,150,3);
	cvNamedWindow("OpencvCannyimg",0);  
	cvShowImage("OpencvCannyimg",OpencvCannyimg);  

	cvWaitKey(0);  
	cvDestroyWindow("Cannyimg"); 
	cvDestroyWindow("OpencvCannyimg"); 


	cvReleaseImage(&pCanny);
	cvReleaseImage(&N);
	cvReleaseImage(&OpenCvGrayImage);
	cvReleaseImage(&ColorImage);
	cvReleaseImage(&OpencvCannyimg);
    delete []pKernel_2;
	delete []P;
	delete []Q;
	delete []M;
	delete []Theta;
	system("pause");
	return 0;
}



