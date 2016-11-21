//vs2010+opencv2.2
//zdd
//Just For Fun
//zddmail@gmail.com
//2012��5��17��16:16:11

#ifndef SIFT_H
#define SIFT_H

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
using namespace cv;

typedef double pixel_t;//��������

//��ʼsigma
#define INIT_SIGMA 0.5

//sigma 
#define SIGMA 1.6

//intervals
#define INTERVALS 3
 
//r
#define RATIO 10
  
#define MAX_INTERPOLATION_STEPS 5 
  
//|D(x^)| < 0.03   0.03��ֵ��̫��
#define DXTHRESHOLD 0.03

//bins = 36
#define ORI_HIST_BINS 36    

#define ORI_SIGMA_TIMES 1.5

#define ORI_WINDOW_RADIUS 3.0 * ORI_SIGMA_TIMES 

#define ORI_SMOOTH_TIMES 2
  
#define ORI_PEAK_RATIO 0.8

#define FEATURE_ELEMENT_LENGTH 128

#define DESCR_HIST_BINS 8

#define IMG_BORDER 5 

#define DESCR_WINDOW_WIDTH 4

#define DESCR_SCALE_ADJUST 3

#define DESCR_MAG_THR 0.2

#define INT_DESCR_FCTR 512.0
  
 
struct Keypoint
{
	int octave; //�ؼ���������
	int interval;// �ؼ������ڲ�

	double offset_interval;//������Ĳ������

	int x; //x,y����,����octave��interval��ȡ�Ĳ���ͼ��
	int y;

	//scale = sigma0*pow(2.0, o+s/S)
	double scale; //�ռ�߶�����

	double dx; //���������꣬�����걻���ų�ԭͼ���С 
	double dy;

	double offset_x;
	double offset_y;

	//��˹���������ڸ���߶����꣬��ͬ�����ͬ���sigmaֵ��ͬ
	//�ؼ�������������ڳ߶�
	double octave_scale; //offset_i;

	double ori;//����

	int descr_length;
	double descriptor[FEATURE_ELEMENT_LENGTH]; //

	double val;//��ֵ
};
void read_features(Vector<Keypoint>&features, const char*file);//xx
void write_features(const Vector<Keypoint>&features, const char*file);

void testInverse3D();

void write_pyr(const Vector<Mat>& pyr, const char* dir);

void DrawKeyPoints(Mat &src, Vector<Keypoint>& keypoints);

const char* GetFileName(const char* dir, int i);

void ConvertToGray(const Mat& src, Mat& dst);

void DownSample(const Mat& src, Mat& dst);

void UpSample(const Mat& src, Mat& dst);

void GaussianTemplateSmooth(const Mat &src, Mat &dst, double);

void GaussianSmooth2D(const Mat &src, Mat &dst, double sigma);

void GaussianSmooth(const Mat &src, Mat &dst, double sigma);

void Sift(const Mat &src, Vector<Keypoint>& features, double sigma=SIGMA, int intervals=INTERVALS);

void CreateInitSmoothGray(const Mat &src, Mat &dst, double);

void GaussianPyramid(const Mat &src, Vector<Mat>&gauss_pyr, int octaves, int intervals, double sigma);

void Sub(const Mat& a, const Mat& b, Mat & c);

void DogPyramid(const Vector<Mat>& gauss_pyr, Vector<Mat>& dog_pyr, int octaves, int intervals);

void DetectionLocalExtrema(const Vector<Mat>& dog_pyr, Vector<Keypoint>& extrema, int octaves, int intervals);

void DrawSiftFeatures(Mat& src, Vector<Keypoint>& features);

#endif