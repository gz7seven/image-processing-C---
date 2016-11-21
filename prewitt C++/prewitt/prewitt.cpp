#include <cstringt.h>
#include<cv.h>  
#include<highgui.h>  
#include<iostream> 
#include <queue>
#include "math.h"

using namespace  std;
using namespace  cv;

int main(int argc,char argv[])
{
	string filename = "e.JPG";
	Mat src = imread(filename,0);
	Mat dst = Mat::zeros(src.rows,src.cols,CV_8UC1);

	int rows = src.rows;
	int cols = src.cols;
	int step = src.step;
	uchar* data_src = src.data;
	uchar* data_dst = dst.data;

	int i = 0;  
    int j = 0;  
    int dx = 0;  
    int dy = 0;  
    int nValue = 0; 
	
    unsigned char *pLine[3] = { NULL, NULL, NULL };  
    for (j = 1; j < rows - 1; j++)  
    {  
        pLine[0] = data_src + step * (j - 1);  
        pLine[1] = data_src + step * j;  
        pLine[2] = data_src + step * (j + 1);  
        for (i = 1; i < cols - 1; i++)  
        {  
            dx =  
                pLine[0][i+1] - pLine[0][i-1] +  
                pLine[1][i+1] - pLine[1][i-1] +  
                pLine[2][i+1] - pLine[2][i-1];  
            dy =  
                pLine[2][i-1] - pLine[0][i-1] +  
                pLine[2][i]   - pLine[0][i]   +  
                pLine[2][i+1] - pLine[0][i+1];  
            nValue = (int) sqrt((float) (dx * dx + dy * dy));  
            if (nValue > 0xFF)  
            {  
                nValue = 0xFF;  
            }
			
			data_dst[j*step+i]=(uchar) nValue;
    
        }  
    } 
	imshow("edgeImg",dst);
	waitKey(0);
	getchar();
	return 0;

}