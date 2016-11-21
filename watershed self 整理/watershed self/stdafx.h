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
	INT   x;            //�������ꣻ
	INT   y;            //�������ꣻ
	INT   gradient;     //�õ��ݶ�(ȥ��С������λ����������Ϊ��С���쵼���������)��
}MyImageGraPtWatershed; //���ڷ�ˮ��ָ�ʱ��¼����λ�ü���Ӧ�ݶȵ���Ϣ��ΪʹMyMath���������ò������DOC�����˴�����ΪҪ������бȽ�����

typedef struct tagRgnInfo{
	BOOL  isflag;       //���ñ�ǣ�
	INT   ptcount;      //���������ĵ�����
	FLOAT   l;          //����l��ֵ
	FLOAT   u;          //����u��ֵ
	FLOAT   v;          //����v��ֵ
}MyRgnInfoWatershed;    //�ָ���������һЩͳ����Ϣ,ͼ���и��������������Ϣ�����flag�����У�

typedef struct tagMyLUV{
	FLOAT l;
	FLOAT u;
	FLOAT v;
}MyLUV;	

void WatershedSegmentVincent(IplImage* pSrc);
void GetGra(IplImage* image, INT* deltar);
void GradSort(INT* deltar,INT width,INT height,MyImageGraPtWatershed*  graposarr,INT* gradientadd);
void Flood(MyImageGraPtWatershed* imiarr, INT* graddarr,INT minh, INT maxh,INT* flagarr, INT& outrgnumber,INT width,INT height);
INT FindMergedRgn(INT idint, INT* mergearr);//�ҵ�idint�������ţ�

#endif
