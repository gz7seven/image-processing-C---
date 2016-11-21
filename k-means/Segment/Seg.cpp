#include "cv.h"
#include "highgui.h"
#include "iostream"
#include <stdio.h>
#include <math.h>

#define K_num 11
#define F_default -1
#define D_max 65535

typedef struct AB_Point
{
	char a;
	char b;

}ABP;	


typedef struct K_center 
{
	int num;
	ABP mean;

}Kcenter;

typedef struct Lab_Point 
{
	ABP ab;
	char flag;

}LabP;

float CalDab(ABP p1,ABP p2)
{
	double Da=(p1.a-p2.a)*(p1.a-p2.a);
	double Db=(p1.b-p2.b)*(p1.b-p2.b);
	
//	cout<<pow((Da+Db),0.5)<<endl;
	return pow((Da+Db),0.5);

}

void LoadtoList(LabP *List,IplImage *Lab_pImg)
{
	int len=Lab_pImg->width*Lab_pImg->height;
	int i=0;
	char *temp=Lab_pImg->imageData;
	for(i=0;i<len;i++)
	{
		List[i].ab.a=*(temp+3*i+1);
		List[i].ab.b=*(temp+3*i+2);
		List[i].flag=F_default;
	}
}


void InitKcen(LabP *List,Kcenter *Cen,int len)
{
	int i=0;
	int j=0;
	j=rand();
	for(i=0;i<K_num;i++)
	{

		Cen[i].mean.a=List[i*j%len].ab.a;
		Cen[i].mean.b=List[i*j%len].ab.b;
		Cen[i].num=1;
		
	}

}

float CompareCenter(Kcenter *Cen,Kcenter *CenBack)
{
	float D=0;
	int i=0;
	for(i=0;i<K_num;i++)
	{
		D+=(Cen[i].mean.a-CenBack[i].mean.a)*(Cen[i].mean.a-CenBack[i].mean.a);
	}
	return D;
}

void AddtoCen(Kcenter *Cen,Lab_Point P,int i)
{	
	int Suma,Sumb;	
	
	Suma=Cen[i].mean.a*Cen[i].num+P.ab.a;
	Sumb=Cen[i].mean.b*Cen[i].num+P.ab.b;
	
	Cen[i].num++;
	Cen[i].mean.a=Suma/Cen[i].num;
	Cen[i].mean.b=Sumb/Cen[i].num;
	

}

void WriteImg(LabP *List,Kcenter *Cen,IplImage *pImg,int len)
{
	char *temp=pImg->imageData;
	int i=0;
	for(i=0;i<len;i++)
	{
		*(temp+3*i+1)=Cen[List[i].flag].mean.a;
		*(temp+3*i+2)=Cen[List[i].flag].mean.b;
	}


}


void main()
{
	int i=0;
	int j=0;
	float dis=65535;
	float dist=0;
	int AddN;

	IplImage* repImg;
	IplImage* pImg;
	IplImage* Lab_pImg;
	//repImg=cvLoadImage("r_lena.bmp");
	repImg=cvLoadImage("C:/1.bmp");
	pImg=repImg;

	Lab_pImg=repImg;
	cvSmooth(repImg,pImg,CV_GAUSSIAN,3,0,0,0);
    cvCvtColor(pImg,Lab_pImg,CV_BGR2Lab);
	//cout<<Lab_pImg->channelSeq;
	int len=0;
	len=pImg->width*pImg->height;

	char *Img=Lab_pImg->imageData;
	Kcenter Cen[K_num];
	Kcenter CenBack[K_num];
	LabP *List=new LabP[len];	

	LoadtoList(List,Lab_pImg);
	InitKcen(List,Cen,len);
	

	while(1)
	{	
		for(i=0;i<K_num;i++)
		{	CenBack[i].mean.a=Cen[i].mean.a;
			CenBack[i].mean.b=Cen[i].mean.b;
			CenBack[i].num=Cen[i].num;
		}
	
		for(i=0;i<len;i++)
		{
			dis=65535;
			for(j=0;j<K_num;j++)
			{
				dist=CalDab(Cen[j].mean,List[i].ab);
			//	cout<<dist;
				if(dist<dis)
				{
					dis=dist;
					AddN=j;	
					
				}

			}
			AddtoCen(Cen,List[i],AddN);
			List[i].flag=AddN;
	
		}
		if (CompareCenter(Cen,CenBack)<10)
			break;
	}
	WriteImg(List,Cen,Lab_pImg,len);
	cvCvtColor(pImg,Lab_pImg,CV_Lab2BGR);
	
	cvNamedWindow("Image",1);
	cvShowImage("Image",pImg);
	cvWaitKey(0);
	cvDestroyWindow("Image");
//	printf("%d",List[11].a);
	
	cvReleaseImage(&pImg);
	delete List;
}