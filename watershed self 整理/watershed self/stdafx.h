#ifndef WARTERSHED
#define WARTERSHED

#pragma once

#include "targetver.h"
#include <stdio.h>
#include <tchar.h>
#include "stdafx.h"
#include <cstringt.h>
#include<cv.h>  
#include<highgui.h>  
#include<iostream> 
#include <queue>
#include "math.h"

using namespace  std;
using namespace  cv;
typedef int                 INT;
typedef unsigned long       DWORD;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef CStringT< TCHAR, StrTraitMFC_DLL< TCHAR > > CString;
#define MAXV 256

typedef struct tagMyImageGraPt{
	INT   x;            //横向坐标；
	INT   y;            //纵向坐标；
	INT   gradient;     //该点梯度(去掉小数点后的位数，避免因为极小差异导致排序错误)；
}MyImageGraPtWatershed; //用于分水岭分割时记录各点位置及相应梯度等信息，为使MyMath独立，不得不将其从DOC移至此处，因为要对其进行比较排序；

typedef struct tagRgnInfo{
	BOOL  isflag;       //备用标记；
	INT   ptcount;      //该区所含的点数；
	FLOAT   l;          //该区l均值
	FLOAT   u;          //该区u均值
	FLOAT   v;          //该区v均值
}MyRgnInfoWatershed;    //分割后各个区的一些统计信息,图像中各点所属区域的信息存放在flag数组中；

typedef struct tagMyLUV{
	FLOAT l;
	FLOAT u;
	FLOAT v;
}MyLUV;	

void WatershedSegmentVincent(IplImage* pSrc);
void GetGra(IplImage* image, INT* deltar);
void GradSort(INT* deltar,INT width,INT height,MyImageGraPtWatershed*  graposarr,INT* gradientadd);
void Flood(MyImageGraPtWatershed* imiarr, INT* graddarr,INT minh, INT maxh,INT* flagarr, INT& outrgnumber,INT width,INT height);
INT FindMergedRgn(INT idint, INT* mergearr);//找到idint最终区号；

#endif
