// MyMath.cpp: implementation of the MyMath class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaterShed.h"
#include "MyMath.h"
#include <MATH.H>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyMath::MyMath()
{
	srand( (unsigned)time( NULL ) );
}

MyMath::~MyMath()
{

}

DOUBLE MyMath::LineDistance(My3DPoint inPoint1, My3DPoint inPoint2)
{
	DOUBLE tempoffx = inPoint1.x - inPoint2.x;
	DOUBLE tempoffy = inPoint1.y - inPoint2.y;
	DOUBLE tempoffh = inPoint1.h - inPoint2.h;

	return sqrt(tempoffx*tempoffx + tempoffy*tempoffy + 
		tempoffh*tempoffh);
}

DOUBLE MyMath::TriangleArea(My3DPoint inPoint1, 
	My3DPoint inPoint2, My3DPoint inPoint3)
{
	//����߳���
	DOUBLE a = LineDistance(inPoint1, inPoint2);
	DOUBLE b = LineDistance(inPoint2, inPoint3);
	DOUBLE c = LineDistance(inPoint3, inPoint1);

	//�����ܳ���1/2��
	DOUBLE s = (a+b+c) / 2;

	//���������
	return sqrt( s * (s-a) * (s-b) * (s-c) );
}

DOUBLE MyMath::TetrahedronVol(My3DPoint inPoint1,
		My3DPoint inPoint2, My3DPoint inPoint3,
		My3DPoint inPoint4)
{		
	//�����������
	DOUBLE xa,ya,ha,xb,yb,hb,xc,yc,hc,xd,yd,hd;//�ĸ������ά���ꣻ
	xa = inPoint1.x;
	ya = inPoint1.y;
	ha = inPoint1.h;
	xb = inPoint2.x;
	yb = inPoint2.y;
	hb = inPoint2.h;
	xc = inPoint3.x;
	yc = inPoint3.y;
	hc = inPoint3.h;
	xd = inPoint4.x;
	yd = inPoint4.y;
	hd = inPoint4.h;

	//��������ľ���
	DOUBLE m11 = xb - xa;
	DOUBLE m12 = yb - ya;
	DOUBLE m13 = hb - ha;
	DOUBLE m21 = xc - xa;
	DOUBLE m22 = yc - ya;
	DOUBLE m23 = hc - ha;
	DOUBLE m31 = xd - xa;
	DOUBLE m32 = yd - ya;
	DOUBLE m33 = hd - ha;

	//������ʽ��
	DOUBLE determinant = fabs ( m11 * (m22*m33 - m32*m23) -
		m21 * (m12*m33 - m32*m13) +
		m31 * (m12*m23 - m22*m13) );

	//���������
    return determinant / 6;	
}

DOUBLE MyMath::PointLineDistance(My3DPoint point, My3DPoint linePt1, My3DPoint linePt2)
//���߾���;
{
	DOUBLE area = TriangleArea(point, linePt1, linePt2);
	DOUBLE edgelength = LineDistance(linePt1, linePt2);
	return area/edgelength * 2;
}

DOUBLE MyMath::PrismVol(My3DPoint upPoint1, My3DPoint upPoint2, 
		My3DPoint upPoint3,	My3DPoint downPoint1, 
		My3DPoint downPoint2, My3DPoint downPoint3)
//�����������
{
	DOUBLE tetra1, tetra2, tetra3;//�ָ������������������
	tetra1 = TetrahedronVol(downPoint1, downPoint2, 
		                    downPoint3, upPoint1);
	tetra2 = TetrahedronVol(upPoint1, upPoint2, 
		                    upPoint3, downPoint2);
	tetra3 = TetrahedronVol(upPoint1, upPoint3, 
		                    downPoint2, downPoint3);
	return tetra1 + tetra2 + tetra3;
}

BOOL  MyMath::GetBinAt(LONG inLong, INT inPos)
//����һ�����������ظ�����inPos���Ķ�����ֵ��inPosλ�ô�������
//���ڴ˺�����Ϊ������֤�����۵���Ϲ�ʽ���������ܲ��˱���ͨ��ת������
//�ƺ������ã�������и���ת����Ҫ��дת��������
{
	LONG backin1 = inLong;
	LONG backin2 = backin1;

	for (int i=0; i<=inPos; i++)
	{
		backin1 = backin2;
		backin2 = backin1 / 2;	
	}

	if ( backin2*2 != backin1 )
	{
		return 1;
	}else
	{
		return 0;
	}
}

DOUBLE MyMath::Gaussian(DOUBLE inputx, DOUBLE mean, DOUBLE diff)
//�����˹����ֵ��
{
	return exp( -1.0 * (inputx - mean) * (inputx - mean) / 
		( 2 * diff * diff) );
}

DOUBLE MyMath::GetARandom()
//�õ�һ������0-1֮���˫�����������
{
	return  ( (DOUBLE)rand() / (DOUBLE)RAND_MAX );
}

void MyMath::RevertCopyMatrix(BYTE* inMatrix, INT width, BYTE* outMatrix, INT sita, INT mode)
//sita 0-4��Ӧ0,45,90��135��mode 0��Ӧ��-�£���-��
//mode 1��Ӧ��-��,��-���������Ϊ�����׷���
{
	INT pos = -1;
	INT replacepos = -1;
	if (sita==0)
	{
		if (mode==0)
		{
			//0����-��
			INT tempi = INT ( width/2 );
			for (INT y=0; y<width; y++)
			{
				for (INT x=0; x<width; x++)
				{
					pos = y*width + x;
					if (y>tempi)
					{						
						replacepos = (width-y)*width + x;
						outMatrix[pos] = inMatrix[replacepos];
					}else
					{
						outMatrix[pos] = inMatrix[pos];
					}
				}
			}
		}else
		{
			//0����-��
			INT tempi = INT ( width/2 );
			for (INT y=0; y<width; y++)
			{
				for (INT x=0; x<width; x++)
				{
					pos = y*width + x;
					if (y<tempi)
					{						
						replacepos = (width-y)*width + x;
						outMatrix[pos] = inMatrix[replacepos];
					}else
					{
						outMatrix[pos] = inMatrix[pos];
					}
				}
			}
		}
	}

	if (sita==1)
	{
		//45��
		if (mode==0)
		{
			//45����-��
			for (INT y=0; y<width; y++)
			{
				for (INT x=0; x<width; x++)
				{
					pos = y*width + x;
					INT tempoff = abs( y - (width-x) );
					if (y>(width-x))
					{						
						replacepos = (y-tempoff)*width + (x-tempoff);
						outMatrix[pos] = inMatrix[replacepos];
					}else
					{
						outMatrix[pos] = inMatrix[pos];
					}
				}
			}
		}else
		{
			//45����-��
			for (INT y=0; y<width; y++)
			{
				for (INT x=0; x<width; x++)
				{
					pos = y*width + x;
					INT tempoff = abs( y - (width-x) );
					if (y<(width-x))
					{						
						replacepos = (y+tempoff)*width + (x+tempoff);
						outMatrix[pos] = inMatrix[replacepos];
					}else
					{
						outMatrix[pos] = inMatrix[pos];
					}
				}
			}
		}
	}

	if (sita==2)
	{
		//90��
		if (mode==0)
		{
			//90����-��
			INT tempi = INT ( width/2 );
			for (INT y=0; y<width; y++)
			{
				for (INT x=0; x<width; x++)
				{
					pos = y*width + x;
					if (x>tempi)
					{						
						replacepos = y*width + (width-x);
						outMatrix[pos] = inMatrix[replacepos];
					}else
					{
						outMatrix[pos] = inMatrix[pos];
					}
				}
			}
		}else
		{
			//90����-��
			INT tempi = INT ( width/2 );
			for (INT y=0; y<width; y++)
			{
				for (INT x=0; x<width; x++)
				{
					pos = y*width + x;
					if (x<tempi)
					{						
						replacepos = y*width + (width-x);
						outMatrix[pos] = inMatrix[replacepos];
					}else
					{
						outMatrix[pos] = inMatrix[pos];
					}
				}
			}
		}
	}

	if (sita==3)
	{
		//135��
		if (mode==0)
		{
			//135����-��
			for (INT y=0; y<width; y++)
			{
				for (INT x=0; x<width; x++)
				{
					pos = y*width + x;
					if (y>x)
					{
						//XY��ת��
						replacepos = x*width + y;
						outMatrix[pos] = inMatrix[replacepos];
					}else
					{
						outMatrix[pos] = inMatrix[pos];
					}
				}
			}
		}else
		{
			//135����-��
			for (INT y=0; y<width; y++)
			{
				for (INT x=0; x<width; x++)
				{
					pos = y*width + x;
					if (y<x)
					{
						//XY��ת��
						replacepos = x*width + y;
						outMatrix[pos] = inMatrix[replacepos];
					}else
					{
						outMatrix[pos] = inMatrix[pos];
					}
				}
			}
		}
	}
}

FLOAT MyMath::CalcuMatrixMean(FLOAT* inMatrix, INT width, INT height)
//������������ƽ��ֵ��
{
	INT count = width * height;
	FLOAT added = 0;

	for (INT pos=0; pos<count; pos++)
	{
		added += inMatrix[pos];
	}

	return added / (FLOAT)count;
}

INT MyMath::FindMaxInThree(DOUBLE inval1, DOUBLE inval2, DOUBLE inval3)
//��������������ߣ�����Ϊ����ߵ���ţ���0��ʼ��ţ�
{
	if (inval1>inval2)
	{
		if (inval1>inval3)
		{
			return 0;
		}else
		{
			return 2;
		}
	}else
	{
		if (inval2>inval3)
		{
			return 1;
		}else
		{
			return 2;
		}
	}
}

DOUBLE MyMath::ReturnMinInThree(DOUBLE inval1, DOUBLE inval2, DOUBLE inval3)
//������������С�ߣ������䷵�أ�
{
	if (inval1<inval2)
	{
		if (inval1<inval3)
		{
			return inval1;
		}else
		{
			return inval3;
		}
	}else
	{
		if (inval2<inval3)
		{
			return inval2;
		}else
		{
			return inval3;
		}
	}
}

DOUBLE MyMath::ReturnMaxInThree(DOUBLE inval1, DOUBLE inval2, DOUBLE inval3)
//��������������ߣ������䷵�أ�
{
	if (inval1>inval2)
	{
		if (inval1>inval3)
		{
			return inval1;
		}else
		{
			return inval3;
		}
	}else
	{
		if (inval2>inval3)
		{
			return inval2;
		}else
		{
			return inval3;
		}
	}
}

void MyMath::ClacuMean(DOUBLE* inData, LONG inNumber, FLOAT& outMean, FLOAT& outSerr)
//������������ľ�ֵ�뷽�
{
	DOUBLE total = 0;
	//�����ֵ��
	for (INT i=0; i<inNumber; i++)
	{
		total += inData[i];
	}
	outMean = (FLOAT) ( total / (FLOAT)inNumber );

	//���㷽�
	total = 0;
	for (i=0; i<inNumber; i++)
	{
		total += (inData[i] - outMean) * (inData[i] - outMean);
	}
	total = total / (DOUBLE)inNumber;//���ƽ���͵�������
	outSerr = (FLOAT) ( sqrt(total) );//��׼�
}

void MyMath::ClacuMeanPositive(FLOAT* inData, LONG inNumber, FLOAT& outMean, FLOAT& outSerr)
//������������ľ�ֵ�뷽��,ֻ������ֵԪ�أ�
{
	FLOAT total = 0;
	FLOAT count = 0;
	//�����ֵ��
	for (INT i=0; i<inNumber; i++)
	{
		if (inData[i]>0)
		{
			total += inData[i];
			count++;
		}
		
	}

	outMean = (FLOAT) ( total / (FLOAT)count );

	//���㷽�
	total = 0;
	for (i=0; i<inNumber; i++)
	{
		if (inData[i]>0)
		{
			total += (inData[i] - outMean) 
				* (inData[i] - outMean);
		}		
	}

	total = (FLOAT) ( total / (FLOAT)count );//���ƽ���͵�������
	outSerr = (FLOAT) ( sqrt(total) );//��׼�
}

BOOL MyMath::isOdd(INT inInt)
//�Ƿ�������
{
	INT tempi = (INT) (inInt/2);
	if (tempi*2 == inInt)
	{
		//ż����
		return FALSE;
	}

	//������
	return TRUE;
}

/*******************************************************************************
* �������ƣ�GetMaxValue()
*
* ������    BYTE* pWinValue    -- ָ�������ָ��
*           int nWinLength     -- ����ĳ���
*
* ����ֵ��  BYTE               -- ��������ֵ
*
* ˵����    �ú������ڵõ�һ����������ֵ���ɺ��� LocalThresholding() ���á�
********************************************************************************/

BYTE MyMath::GetMaxValue(BYTE* pWinValue,int nWinLength)
{
	int i;    // ѭ������
	BYTE Tmp = 0; // ��ʱ����
    for(i=0;i<nWinLength;i++)
	{
		if(pWinValue[i] > Tmp)
		{
			Tmp = pWinValue[i];
		}
	}
	return Tmp;
}

/*******************************************************************************
* �������ƣ�GetMinValue()
*
* ������    BYTE* pWinValue    -- ָ�������ָ��
*           int nWinLength     -- ����ĳ���
*
* ����ֵ��  BYTE               -- �������Сֵ
*
* ˵����    �ú������ڵõ�һ���������Сֵ���ɺ��� LocalThresholding() ���á�
********************************************************************************/

BYTE MyMath::GetMinValue(BYTE* pWinValue,int nWinLength)
{
	int i;    // ѭ������
	BYTE Tmp = 255; // ��ʱ����
    for(i=0;i<nWinLength;i++)
	{
		if(pWinValue[i] < Tmp)
		{
			Tmp = pWinValue[i];
		}
	}

	return Tmp;
}

/*******************************************************************************
* �������ƣ�GetAveValue()
*
* ������    BYTE* pWinValue    -- ָ�������ָ��
*           int nWinLength     -- ����ĳ���
*
* ����ֵ��  BYTE               -- ����ľ�ֵ
*
* ˵����    �ú������ڵõ�һ�������ڷ�0���ľ�ֵ���ɺ��� LocalThresholding() ���á�
********************************************************************************/

BYTE MyMath::GetAveValue(BYTE* pWinValue,int nWinLength)
{
	int i;    // ѭ������
	int nNonZeroNum = 0;  // ��0����Ŀ
	int nTotal = 0;       // ��ʱ��������ֵ֮��
	BYTE Average;
	double dAverage;
    for(i=0;i<nWinLength;i++)
	{
		if(pWinValue[i] != 0)
		{
			nTotal += pWinValue[i];
			nNonZeroNum++;
		}
	}
	dAverage = (double)nTotal/nNonZeroNum;
	if(dAverage - (int)dAverage != 0.0)
	{
		Average = (int)dAverage + 1;
	}
	else
	{
		Average = (int)dAverage;
	}

	return Average;
}

void MyMath::QickSort(FLOAT* inArr, LONG inNum)
//��������
{
	qsort( inArr, inNum, sizeof(FLOAT), CompareFloat );
}

void MyMath::QickSort(MyImageGraPt* inArr, LONG inNum)
//���ݶ�ֵ���ռ�˳��Ը�������
{
    qsort( inArr, inNum, sizeof(MyImageGraPt), CompareGraPt );
}

void MyMath::QickSortInver(MyImageGraPt* inArr, LONG inNum)
//���ݶ�ֵ���ռ�����Ը�������
{
    qsort( inArr, inNum, sizeof(MyImageGraPt), CompareGraPtInver );
}

int CompareFloat( const void* in1, const void* in2 )
//�Ƚϣ�
{
	FLOAT tempf = *((FLOAT*)in1) - *((FLOAT*)in2);

	if (tempf==0)
	{
		return 0;
	}	
	return (INT) (tempf/fabs(tempf));
}

int CompareGraPt( const void* in1, const void* in2 )
//�Ƚ��ݶȣ�
{
	MyImageGraPt pt1, pt2;
	pt1 = *((MyImageGraPt*)in1);
	pt2 = *((MyImageGraPt*)in2);
	FLOAT tempf = (FLOAT) ( pt1.gradient - pt2.gradient );
	FLOAT tempx = (FLOAT) ( pt1.x - pt2.x );
	FLOAT tempy = (FLOAT) ( pt1.y - pt2.y );

	//����ݶ�ֵһ�������տռ�λ������
	if (tempf==0)
	{
		if (tempy==0)
		{
			if (tempx==0)
			{
				return 0;
			}else
			{
				tempf = tempx;
			}
		}else
		{
			tempf = tempy;
		}
	}
	
	INT tempi = (INT) (tempf/fabs(tempf));
	return tempi;
}

int CompareGraPtInver( const void* in1, const void* in2 )
//�Ƚ��ݶȣ�
{
	MyImageGraPt pt1, pt2;
	pt1 = *((MyImageGraPt*)in1);
	pt2 = *((MyImageGraPt*)in2);
	FLOAT tempf = (FLOAT) ( pt1.gradient - pt2.gradient );
	FLOAT tempx = (FLOAT) ( pt1.x - pt2.x );
	FLOAT tempy = (FLOAT) ( pt1.y - pt2.y );

	//����ݶ�ֵһ�������տռ�λ�÷�������
	if (tempf==0)
	{
		if (tempx==0)
		{
			if (tempy==0)
			{
				return 0;
			}else
			{
				tempf = -tempy;//�ռ�λ�÷�������
			}
		}else
		{
			tempf = -tempx;//�ռ�λ�÷�������
		}
	}

	INT tempi = (INT) (tempf/fabs(tempf));
	return tempi;
}

void MyMath::GetNeiInt(INT posx, INT posy, INT currentpos, INT width, INT height
		, INT& left, INT& right, INT& up, INT& down)
//�ھ�������pos���ĸ�����
{
	//INT curpos = posy*width + posx;
	if ( posx-1 >= 0 )
	{
		left = currentpos - 1;
	}else
	{
		left = -1;
	}

	if ( posx+1 < width)
	{
		right = currentpos + 1;
	}else
	{
		right = -1;
	}

	if ( posy-1 >= 0 )
	{
		up = currentpos - width;
	}else
	{
		up = -1;
	}

	if ( posy+1 < height )
	{
		down = currentpos + width;
	}else
	{
		down = -1;
	}
}

