#include <cstringt.h>
#include<cv.h>  
#include<highgui.h>  
#include<iostream> 
#include <queue>
#include "math.h"
#include<iostream>
#include<algorithm>

using namespace cv;
using namespace std;

//通过sobel获取灰度图像image的梯度值存数组deltar
int main(int argc,char argv[])

{
	Mat src= imread("f.JPG",0);
	Mat dst = Mat::zeros(src.rows,src.cols,CV_8UC1);
	Mat grad_x = Mat::zeros(src.rows,src.cols,CV_8UC1);
	Mat grad_y = Mat::zeros(src.rows,src.cols,CV_8UC1);
    int height = src.rows;
    int width = src.cols;
	int step = src.step;
    int* deltar;
	uchar* data_dst = dst.data;
	uchar* data_src = src.data;
	uchar* data_dx = grad_x.data;
	uchar* data_dy = grad_y.data;
 
    static int nWeight[2][3][3];
    nWeight[0][0][0]=-1;
    nWeight[0][0][1]=0;
    nWeight[0][0][2]=1;
    nWeight[0][1][0]=-2;
    nWeight[0][1][1]=0;
    nWeight[0][1][2]=2;
    nWeight[0][2][0]=-1;
    nWeight[0][2][1]=0;
    nWeight[0][2][2]=1;
    nWeight[1][0][0]=1;
    nWeight[1][0][1]=2;
    nWeight[1][0][2]=1;
    nWeight[1][1][0]=0;
    nWeight[1][1][1]=0;
    nWeight[1][1][2]=0;
    nWeight[1][2][0]=-1;
    nWeight[1][2][1]=-2;
    nWeight[1][2][2]=-1;
     
    int nTmp[3][3];
    int x,y;
    int* gra=new int[height*width];
    int gr;
    for (y=0;y<height;y++)
    {
        for (x=0;x<width;x++)
        {
            int image_pos=width*y+x;
            gr = ((uchar *)(src.data))[y*src.step+x];
            gra[image_pos]=gr;
        }
    }
 
    //暂不计算边缘点
    for (y=1;y<height-1;y++)
    {
        for (x=1;x<width-1;x++)
        {
			int curPos = y*step+x;
			int dx=0,dy=0;
            nTmp[0][0]=gra[(y-1)*width+x-1];
            nTmp[0][1]=gra[(y-1)*width+x];
            nTmp[0][2]=gra[(y-1)*width+x+1];
            nTmp[1][0]=gra[y*width+x-1];
            nTmp[1][1]=gra[y*width+x];
            nTmp[1][2]=gra[y*width+x+1];
            nTmp[2][0]=gra[(y+1)*width+x-1];
            nTmp[2][1]=gra[(y+1)*width+x];
            nTmp[2][2]=gra[(y+1)*width+x+1];
            for (int yy=0;yy<3;yy++)
            {
                for (int xx=0;xx<3;xx++)
                {
                    dx+=nTmp[yy][xx]*nWeight[0][yy][xx];//X轴方向
                    dy+=nTmp[yy][xx]*nWeight[1][yy][xx];//Y轴方向
                }
            }
			data_dx[curPos] = dx;
			data_dy[curPos] = dy;
			int nValue = sqrtf(dx*dx + dy*dy);
			if (nValue > 0xFF)  
            {  
                nValue = 0xFF;  
            }
			data_dst[curPos] = nValue; 		
        }
    }
	imshow("原图",src);
    imshow("dstImg",dst);
    waitKey(0);
    getchar();
    return 0;
}