// UserDefinedClass.cpp: implementation of the class in UserDefinedClass.h .
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ManyImageProcessingMethod.h"
#include "UserDefinedClass.h"
#include "math.h"

#include <mmsystem.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CImage::CImage()
{
          
}
CImage::~CImage()
{

}


CImage_Class::CImage_Class()
{

}

CImage_Class::~CImage_Class()
{
}

void CImage_Class::Math_Morph_Dilate(Mask msk, CImage img)//二值图像的膨胀
{
//    int my,mx,kk,kk1,y,x,flg;
	BYTE *temp=(BYTE *) malloc(img.ImageHeight*img.ImageWidth);

	for(int i=0;i<img.ImageHeight*img.ImageWidth;i++)	//设缓冲区
		temp[i]=white;

//	my=int(msk.cols /2); mx=int(msk.rows /2);
/*
  0 0 0 0
  0 1 1 0
  0 1 1 0
  0 0 0 0
	*/
	//////////////////////////灰度图象膨胀////////////////////////
    CMor *Mor=new CMor[4];
	Mor[0].x=1;
	Mor[0].y=1;
    Mor[1].x=2;
	Mor[1].y=1;
	Mor[2].x=1;
	Mor[2].y=2;
	Mor[3].x=2;
	Mor[3].y=2;
	int x,y,i;
	for(y=0;y<img.ImageHeight ;y++)		//膨胀运算
	{
		for(x=0;x<img.ImageWidth;x++)
		{
			int cxMax=-32767;
			for(int k=0;k<4;k++)
			{
				int xr=Mor[k].x;
				int yr=Mor[k].y;
				int i=x-xr;
				int j=y-yr;
				i=(i<0)?0:((i>(img.ImageWidth-1))?(img.ImageWidth-1):i);
				j=(j<0)?0:((j>(img.ImageHeight-1))?(img.ImageHeight-1):j);
				int cx=img.data[j*img.ImageWidth+i];
				int c=cx+0;
			    if(c>cxMax)cxMax=c;
			}
			temp[y*img.ImageWidth+x]=cxMax;
		}
	}
	//////////////////////////二值图像的膨胀//////////////////////
/*	int my,mx,kk,kk1,i,j,y,x,flg;
	BYTE *temp=(BYTE *) malloc(img.ImageHeight*img.ImageWidth);

	for(i=0;i<img.ImageHeight*img.ImageWidth;i++)	//设缓冲区
		temp[i]=white;

	my=int(msk.cols /2); mx=int(msk.rows /2);

	for(y=0;y<img.ImageHeight ;y++)		//膨胀运算
		for(x=0;x<img.ImageWidth;x++)
		{
			if((y-my)>=0 && (y+my)<img.ImageHeight && (x-mx)>=0 && (x+mx)<img.ImageWidth)	//在指定区域内
			{
				flg=0;
				for(i=-my;i<=my;i++)
					for(j=-mx;j<=mx;j++)
					{
						kk1=(y+i)*img.ImageWidth+(x+j);
						kk=(i+my)*msk.rows +j+mx;
						if(img.data[kk1]==black && msk.kern [kk]==black)
							flg++;

					}
				if(flg!=0)
				{
					kk1=y*img.ImageWidth+x;
					temp[kk1]=black;
				}
			}
		}
*/
	for(i=0;i<img.ImageHeight*img.ImageWidth;i++)
		img.data[i]=temp[i];
	free(temp);
}

void CImage_Class::Math_Morph_Erode(Mask msk, CImage img)//腐蚀
{
int my,mx,kk,kk1,y,x,flg,i;
	BYTE *temp=(BYTE *) malloc(img.ImageHeight*img.ImageWidth);

	for(int i=0;i<img.ImageHeight*img.ImageWidth;i++)	//设缓冲区
		temp[i]=white;

//	my=int(msk.cols /2); mx=int(msk.rows /2);
/*
  0 0 0 0
  0 1 1 0
  0 1 1 0
  0 0 0 0
	*/
		//////////////////////////灰度图象腐蚀////////////////////////
    CMor *Mor=new CMor[4];
	Mor[0].x=1;
	Mor[0].y=1;
    Mor[1].x=2;
	Mor[1].y=1;
	Mor[2].x=1;
	Mor[2].y=2;
	Mor[3].x=2;
	Mor[3].y=2;
	for(y=0;y<img.ImageHeight ;y++)		//膨胀运算
	{
		for(x=0;x<img.ImageWidth;x++)
		{
			int cxMin=32767;
			for(int k=0;k<4;k++)
			{
				int xr=Mor[k].x;
				int yr=Mor[k].y;
				int i=x+xr;
				int j=y+yr;
				i=(i<0)?0:((i>(img.ImageWidth-1))?(img.ImageWidth-1):i);
				j=(j<0)?0:((j>(img.ImageHeight-1))?(img.ImageHeight-1):j);
				int cx=img.data[j*img.ImageWidth+i];
				int c=cx-12;
			    if(c<cxMin)cxMin=c;
			}
			temp[y*img.ImageWidth+x]=abs(cxMin);
		}
	}
	///////////////////////////////////二值图像的腐蚀/////////////////////////////////
/*	int my,mx,kk,kk1,i,j,y,x,flg;
	BYTE *temp=(BYTE *) malloc(img.ImageHeight*img.ImageWidth);

	for(i=0;i<img.ImageHeight*img.ImageWidth;i++)	//设缓冲区
		temp[i]=white;

	my=int(msk.cols /2); mx=int(msk.rows /2);

	for(y=0;y<img.ImageHeight ;y++)
		for(x=0;x<img.ImageWidth ;x++)
		{
			if((y-my)>=0 && (y+my)<img.ImageHeight && (x-mx)>=0 && (x+mx)<img.ImageWidth)	//在指定区域内
			{
				flg=0;
				for(i=-my;i<=my;i++)
					for(j=-mx;j<=mx;j++)
					{
						kk1=(y+i)*img.ImageWidth+(x+j);
						kk=(i+my)*msk.rows +j+mx;
						if(img.data [kk1]!=black && msk.kern [kk]==black)
							flg++;

					}
				if(flg==0)
				{
					kk1=y*img.ImageWidth+x;
					temp[kk1]=black;
				}
			}
		}*/
	for(i=0;i<img.ImageHeight*img.ImageWidth;i++)
		img.data [i]=temp[i];
	free(temp);
}

void CImage_Class::Edge_Grad(CImage img, CImage Edge)
{

}

void CImage_Class::Edge_Rober(CImage img, CImage Edge)
{

}

void CImage_Class::Edge_Laplace(CImage img, CImage Edge)
{
   	
	int y,x,kk;
	int kk1,kk2,kk3,kk4,kk5,kk6,kk7,kk8;
	int A,B;
	int threshold=120;
	BYTE *Edgedata1;

	Edgedata1=(BYTE *) malloc(img.ImageHeight*img.ImageWidth*sizeof(BYTE));

	for(x=0;x<img.ImageWidth;x++)
		for(y=0;y<img.ImageHeight;y++)
		{
			kk=y*img.ImageWidth+x;
			Edge.data[kk]=black;
		}

	for(y=1;y<img.ImageHeight-1;y++)
	{
		for(x=1;x<img.ImageWidth-1;x++)
		{
			kk=y*img.ImageWidth+x;
			kk1=(y-1)*img.ImageWidth+x-1;
			kk2=(y-1)*img.ImageWidth+x;
			kk3=(y-1)*img.ImageWidth+x+1;
			kk4=y*img.ImageWidth+x-1;
			kk5=y*img.ImageWidth+x+1;
			kk6=(y+1)*img.ImageWidth+x-1;
			kk7=(y+1)*img.ImageWidth+x;
			kk8=(y+1)*img.ImageWidth+x+1;
			A=img.data[kk1]+2*img.data[kk2]+img.data[kk3]-img.data[kk6]-2*img.data[kk7]-img.data[kk8];
			B=img.data[kk1]+2*img.data[kk4]+img.data[kk6]-img.data[kk3]-2*img.data[kk5]-img.data[kk8];

			if(abs(A)>abs(B))
			{Edgedata1[kk]=abs(A);}
			else
				Edgedata1[kk]=abs(B);
		}
	}

	for(y=2;y<img.ImageHeight-2;y++)
		for(x=2;x<img.ImageWidth-2;x++)
		{
			kk=y*img.ImageWidth+x;
			kk1=(y+1)*img.ImageWidth+x;
			kk2=(y-1)*img.ImageWidth+x;
			kk3=y*img.ImageWidth+x+1;
			kk4=y*img.ImageWidth+x-1;
			if(Edgedata1[kk]>threshold && (Edgedata1[kk]>Edgedata1[kk1] && Edgedata1[kk]>Edgedata1[kk2] || Edgedata1[kk]>Edgedata1[kk3] && Edgedata1[kk]>Edgedata1[kk4]))
			{
				Edge.data[kk]=white;
			}
		}
			

}

bool CImage_Class::Load(LPCTSTR filename)
{
   	int y;
	CString str;
	str.Format ("%s",filename);

	if(!ImageFile.Open(filename,CFile::modeRead))
	{
		AfxMessageBox("The file: "+str+"   is not exist!");
		return FALSE;
	}

	ImageFile.Read(&BitmapFileHeader,sizeof(BitmapFileHeader));
	ImageFile.Read(&bitmapinfo,sizeof(BITMAPINFO));
	DWORD off=BitmapFileHeader.bfOffBits-sizeof(BitmapFileHeader)-sizeof(BITMAPINFO);
	ImageFile.Read(&BitmapInfo,off);

	Imagedata[0].ImageWidth =bitmapinfo.bmiHeader.biWidth;
	Imagedata[0].ImageHeight=bitmapinfo.bmiHeader.biHeight;
	Imagedata[0].MinX =Imagedata[0].MinY =0;
	Imagedata[0].MaxX=Imagedata[0].ImageWidth -1;
	Imagedata[0].MaxY=Imagedata[0].ImageHeight-1;
	Imagedata[0].flag=false;
	
//	for(int i=0;i<3;i++)
	Imagedata[0].data=(BYTE*) malloc(Imagedata[0].ImageWidth*Imagedata[0].ImageHeight);
	Edgedata.data=(BYTE*) malloc(Imagedata[0].ImageWidth*Imagedata[0].ImageHeight);
	Edgedata.ImageHeight=Imagedata[0].ImageHeight;
	Edgedata.ImageWidth=Imagedata[0].ImageWidth;
    
	if(BitmapFileHeader.bfType==0x4d42)
	{	
		DWORD offset=BitmapFileHeader.bfOffBits;  
		ImageFile.Seek(offset,CFile::begin);

		for(y=Imagedata[0].ImageHeight-1;y>=0;y--)
			{
				ImageFile.Read(Imagedata[0].data +y*Imagedata[0].ImageWidth,Imagedata[0].ImageWidth);
			}
	}

	Imagedata[1].ImageWidth =bitmapinfo.bmiHeader.biWidth;
	Imagedata[1].ImageHeight=bitmapinfo.bmiHeader.biHeight;
	Imagedata[1].MinX =Imagedata[0].MinY =0;
	Imagedata[1].MaxX=Imagedata[0].ImageWidth -1;
	Imagedata[1].MaxY=Imagedata[0].ImageHeight-1;
	Imagedata[1].flag=false;
    Imagedata[1].data=(BYTE*) malloc(Imagedata[0].ImageWidth*Imagedata[0].ImageHeight);

	Imagedata[2].ImageWidth =bitmapinfo.bmiHeader.biWidth;
	Imagedata[2].ImageHeight=bitmapinfo.bmiHeader.biHeight;
	Imagedata[2].MinX =Imagedata[0].MinY =0;
	Imagedata[2].MaxX=Imagedata[0].ImageWidth-1;
	Imagedata[2].MaxY=Imagedata[0].ImageHeight-1;
	Imagedata[2].flag=false;
    Imagedata[2].data=(BYTE*) malloc(Imagedata[0].ImageWidth*Imagedata[0].ImageHeight);
	ImageFile.Close ();

	return true;
}

void CImage_Class::ShowOri(CDC *pDC)
{
		int kk;
	for(int y=0;y<Imagedata[0].ImageHeight ;y++)
		for(int x=0;x<Imagedata[0].ImageWidth;x++)
		{
			kk=y*Imagedata[0].ImageWidth+x;
			pDC->SetPixel(16+x,y+20,RGB(Imagedata[0].data[kk],Imagedata[0].data[kk],Imagedata[0].data[kk]));
		}
}

void CImage_Class::ShowEdge(CDC *pDC)
{
    int kk;
	for(int y=0;y<Imagedata[0].ImageHeight ;y++)
		for(int x=0;x<Imagedata[0].ImageWidth;x++)
		{
			kk=y*Imagedata[0].ImageWidth+x;
			pDC->SetPixel(16+x,y+20,RGB(Edgedata.data[kk],Edgedata.data[kk],Edgedata.data[kk]));
		}
}

bool CImage_Class::Save(LPCTSTR filename)
{
	int y;

	CFile BitmapFile;

	if(!BitmapFile.Open(filename,CFile::modeWrite | CFile::modeCreate))
	{
		AfxMessageBox("The BitmapFile is not exist!");
		return false;
	}

	BitmapFile.Write(&BitmapFileHeader,sizeof(BITMAPFILEHEADER));
	BitmapFile.Write(&bitmapinfo,sizeof(BITMAPINFO));

	DWORD off=BitmapFileHeader.bfOffBits-sizeof(BitmapFileHeader)-sizeof(BITMAPINFO);
	BitmapFile.Write (&BitmapInfo,off);

	DWORD offset=BitmapFileHeader.bfOffBits;

    BitmapFile.Seek(offset,CFile::begin);
	for(y=Imagedata[0].ImageHeight-1;y>=0;y--)
	{
		BitmapFile.Write(Imagedata[0].data+y*Imagedata[0].ImageWidth,Imagedata[0].ImageWidth);
	}

	BitmapFile.Close ();
	return true;

}
