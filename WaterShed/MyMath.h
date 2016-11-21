// MyMath.h: interface for the MyMath class.
/*
   һЩ��ѧ��������ʼ��WS3D�Ŀռ���㡣

   by dzj;
*/
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYMATH_H__CFBFD7BB_A90A_43E8_8018_3CE1AAA3A24E__INCLUDED_)
#define AFX_MYMATH_H__CFBFD7BB_A90A_43E8_8018_3CE1AAA3A24E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PI 3.14159265359


typedef struct tagMyImageGraPt{
	INT   x;//�������ꣻ
	INT   y;//�������ꣻ
	INT   gradient;//�õ��ݶ�(ȥ��С������λ����������Ϊ��С���쵼���������)��
}MyImageGraPt;//���ڷ�ˮ��ָ�ʱ��¼����λ�ü���Ӧ�ݶȵ���Ϣ��ΪʹMyMath���������ò������DOC�����˴�����ΪҪ������бȽ�����

struct  My3DPoint//��ά�ռ��
{
	DOUBLE x;//X����;
	DOUBLE y;//y����;
	DOUBLE h;//Z����;
};

int CompareFloat( const void* in1, const void* in2 );//�Ƚϸ���ֵ��
int CompareGraPt( const void* in1, const void* in2 );//�Ƚ��ݶȣ�
int CompareGraPtInver( const void* in1, const void* in2 );//�Ƚ��ݶ�,�ռ�����


class MyMath  
{
public:
	MyMath();
	virtual ~MyMath();
    void GetNeiInt(INT posx, INT posy, INT currentpos, INT width
		, INT height, INT& left, INT& right, INT& up, INT& down);//�ھ�������pos���ĸ�����
	void QickSort(FLOAT* inArr, LONG inNum);//��������
	void QickSort(MyImageGraPt* inArr, LONG inNum);//���ݶ�ֵ�ռ�˳��Ը�������
	void QickSortInver(MyImageGraPt* inArr, LONG inNum);//���ݶ�ֵ�ռ�����Ը�������
    BYTE GetMaxValue(BYTE* pWinValue,int nWinLength);
	BYTE GetMinValue(BYTE* pWinValue,int nWinLength);
    BYTE GetAveValue(BYTE* pWinValue,int nWinLength);
	BOOL isOdd(INT inInt);//�Ƿ�������
    FLOAT CalcuMatrixMean(FLOAT* inMatrix, INT width, INT height);//������������ƽ��ֵ��
	void ClacuMean(DOUBLE* inData, LONG inNumber, FLOAT& outMean, FLOAT& outSerr);//������������ľ�ֵ�뷽�
    void ClacuMeanPositive(FLOAT* inData, LONG inNumber, FLOAT& outMean, FLOAT& outSerr);//ֻ������ֵԪ�صľ�ֵ�뷽�
	void RevertCopyMatrix(BYTE* inMatrix, INT width, BYTE* outMatrix, INT sita, INT mode);//sita 0-4��Ӧ0,45,90��135��mode 0��Ӧ��-�£���-�ң�mode 1��Ӧ��-��,��-��,����������鲻����ͬ��
	INT FindMaxInThree(DOUBLE inval1, DOUBLE inval2, DOUBLE inval3);//��������������ߣ�����Ϊ����ߵ���ţ���0��ʼ��ţ�
	DOUBLE ReturnMinInThree(DOUBLE inval1, DOUBLE inval2, DOUBLE inval3);//������������С�ߣ������䷵�أ�
	DOUBLE ReturnMaxInThree(DOUBLE inval1, DOUBLE inval2, DOUBLE inval3);//��������������ߣ������䷵�أ�

    DOUBLE GetARandom();//�õ�һ��0-1֮����������
	DOUBLE PointLineDistance(My3DPoint point, My3DPoint linePt1, My3DPoint linePt2);//����
	DOUBLE LineDistance(My3DPoint inPoint1, My3DPoint inPoint2);//ֱ�߾��룻
	DOUBLE TriangleArea(My3DPoint inPoint1, My3DPoint inPoint2, My3DPoint inPoint3);//�����������
	DOUBLE TetrahedronVol(My3DPoint inPoint1,
		My3DPoint inPoint2, My3DPoint inPoint3,
		My3DPoint inPoint4);//�����������
	DOUBLE PrismVol(My3DPoint upPoint1, My3DPoint upPoint2, 
		My3DPoint upPoint3,	My3DPoint downPoint1, 
		My3DPoint downPoint2, My3DPoint downPoint3);//�����������
	BOOL  GetBinAt(LONG inLong, INT inPos);//����һ�����������ظ�����inPos���Ķ�����ֵ��inPosλ�ô�������
	DOUBLE Gaussian(DOUBLE inputx, DOUBLE mean, DOUBLE diff);//�����˹����ֵ��

};

#endif // !defined(AFX_MYMATH_H__CFBFD7BB_A90A_43E8_8018_3CE1AAA3A24E__INCLUDED_)
