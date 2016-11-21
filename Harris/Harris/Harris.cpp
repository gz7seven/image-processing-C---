/*原理：
Harris角点检测最直观的解释是:在任意两个相互垂直的方向上，都有较大变化的点。
在harris的角点检测中，使用的是高斯窗口，所以w(x,y)表示的是高斯窗口中的权重。此时 当u和v取两组相互垂直的值时，E(u,v)都有较大值的点。
公式推导：
根据二阶taylor展开式得出：
如何考虑E(u,v)在两个相互垂直的方向上都取得最大值呢？我们可以先求出E(u,v)在一个方向上的最大值为r1，即E(u1,v1) = r1,
然后求其垂直方向上的值r2，我们意外的发现这里的r1和r2分别对应M的两个特征值，对应的向量(u1,v1),(u2,v2)为M的特征向量，
此处的M被称为Hessian矩阵。
判断是否为角点的标准:
r1,r2都很小,对应于图像中的平滑区域
r1,r2都很大，对应于图像中的角点
r1，r2一个很大，一个很小，对应于图像中的边缘
Harris角点量计算公式: R = det(M) – a*trace(M)^2,其中a为经验值，取值范围介于[0.04, 0.06],
另一种角点量计算方法:R = det(M)/trace(M)—这个公式自由发挥，只要能反应角点的特征就可以了
其中det(M) = r1*r2 = AB –C^2
    Trace(M) = r1+r2 =A+B （其中A=Ix^2;;  B = Iy^2;;; C=IxIy）
角点检测的步骤
根据上述的讨论，harris角点检测的步骤可以总结为:
    <1>计算图像I(x,y)在x和y两个方向的梯度Ix,Iy：
    <2>计算梯度方向的乘积
    <3>使用高斯核对Ix^2,Iy^2,IxIy进行加权，计算矩阵M的元素A，B，C
    <4>根据计算角点量，并设定阈值，当角点量小于阈值，不是候选角点
    <5>进行局部极大值抑制
	*/
#include <iostream>  
#include "cv.h"  
#include "cxcore.h"  
#include "highgui.h"  
using namespace std;  

//harris 角点检测需要的参数  
typedef struct HARRISPARAMS  
{  
    int gaussSize; //高斯窗口  
    float gaussSigma; //高斯方差  
    double threshold; //对角点量设定的阈值  
    int maximumSize; //局部极大值抑制时的窗口大小  
      
}HARRISPARAMS,*PHARRISPARAMS;  
  
  
/******************************* 
*对源图像进行卷积运算 
*输入项 
*srcFloat    源图像 
*Ix          卷积的结果 
*dxTemplate  卷积模板 
*widthTemplate 模板的宽度 
*heightTemplate 模板的高度 
********************************/  
void convolution(IplImage* srcFloat,IplImage* Ix,double* dxTemplate , int widthTemplate,int heightTemplate);  
  
  
/*************************** 
*harris角点检测函数 
*输入项 
*srcIn   源图像 
*params  harris角点检测需要的参数 
*corners 存放harris角点坐标 
****************************/  
void getHarrisPoints(IplImage* srcIn,PHARRISPARAMS params,CvSeq* corners);  
  
//主函数  
int main(int argc, char* argv[])  
{  
  
    //相关变量  
    IplImage* src,*srcGray;  
    CvMemStorage* mem = cvCreateMemStorage(0); //开辟内存 
    CvSeq* harrisPoints;  
    HARRISPARAMS harrisParams;  
  
    src = cvLoadImage("1.bmp");//源图像  
      
    srcGray = cvCreateImage(cvGetSize(src),8,1);        // 灰度图像  
    if(!src)  
    {  
        cout << " src is null";  
        return 0;  
    }  
      
    cvCvtColor(src,srcGray,CV_BGR2GRAY);  
  
    //harris角点保存的空间  角点坐标保存在一个序列中
    harrisPoints = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvPoint),mem);  

    //设置相关参数  
    harrisParams.gaussSize = 5;   // 高斯窗口的大小  
    harrisParams.gaussSigma = 0.8;  
    harrisParams.threshold =15000000;  
    harrisParams.maximumSize = 21;  
  
    //进行harris角点检测  
    getHarrisPoints(srcGray,&harrisParams,harrisPoints);  
      
    //获取每一个角点的坐标  
    for(int x=0;x<harrisPoints->total;x++)  
    {  
        //获取第x个角点的坐标  
        CvPoint* pt = (CvPoint*)cvGetSeqElem(harrisPoints,x);  
  
        //以角点坐标为中心  绘制一个半径为2的圆  
		//void cvCircle( CvArr* img, CvPoint center, int radius, CvScalar color, int thickness=1, int line_type=8, int shift=0 ); 
        cvCircle(src,*pt,3,cvScalar(0,0,255,0));
    }  
  
    //cvSaveImage("dst.jpg",src);  
  
    //显示图像  
    cvNamedWindow("dst");  
    cvShowImage("dst",src); 
    cvWaitKey(0);  
  
    //释放资源  
    cvReleaseImage(&src);  
    cvReleaseImage(&srcGray);  
    cvReleaseMemStorage(&mem);  
    return 0;  
}  
  
/*************************** 
*harris角点检测函数 
*输入项 
*srcIn   源图像 
*params  harris角点检测需要的参数 
*corners 存放harris角点坐标 
****************************/  
void getHarrisPoints(IplImage* srcIn,PHARRISPARAMS params,CvSeq* corners)  
{  
    int x,y;  
  
    IplImage* srcFloat;  
      
    srcFloat = cvCreateImage(cvGetSize(srcIn),32,1);  
      
    cvConvertScale(srcIn,srcFloat); //使用线性变换转换数组 
      
    IplImage *Ix,*Iy,*IxIx,*IyIy,*IxIy,*A,*B,*C,*cornerness;  
    double *gaussWindow = new double[sizeof(double)*params->gaussSize*params->gaussSize];  
    //水平方向差分算子并求Ix  
     double dxTemplate[9]={-1,0,1,  
                           -1,0,1,  
                           -1,0,1};  
  
    //垂直方向差分算子并求Iy  
     double dyTemplate[9]={-1,-1,-1,  
                            0, 0, 0,  
                            1, 1, 1};  
      
     //此处内存用得有点奢侈 请大家自行修改 节省内存   
    Ix = cvCreateImage(cvGetSize(srcFloat),32,1);  
    Iy = cvCreateImage(cvGetSize(srcFloat),32,1);  
    IxIx = cvCreateImage(cvGetSize(srcFloat),32,1);  
    IyIy = cvCreateImage(cvGetSize(srcFloat),32,1);  
    IxIy = cvCreateImage(cvGetSize(srcFloat),32,1);  
    A = cvCreateImage(cvGetSize(srcFloat),32,1);  
    B = cvCreateImage(cvGetSize(srcFloat),32,1);  
    C = cvCreateImage(cvGetSize(srcFloat),32,1);  
    cornerness = cvCreateImage(cvGetSize(srcFloat),32,1); //保存角点量  
  
    convolution(srcFloat,Ix,dxTemplate,3,3); //计算Ix  
    convolution(srcFloat,Iy,dyTemplate,3,3); //计算Iy   
  
    //计算Ix2、Iy2、IxIy  
    for(y=0;y<srcFloat->height;y++)  
    {  
        for(x=0;x<srcFloat->width;x++)  
        {  
            float IxValue,IyValue;  
              
            IxValue = cvGetReal2D(Ix,y,x);//返回单通道数组的指定元素 ,y为第一个成员，x为第二个成员
            IyValue = cvGetReal2D(Iy,y,x);  
  
            cvSetReal2D(IxIx,y,x,IxValue*IxValue);  
            cvSetReal2D(IyIy,y,x,IyValue*IyValue);  
            cvSetReal2D(IxIy,y,x,IxValue*IyValue);  
        }  
    }  
  
    //计算高斯窗口  
    for( y=0;y<params->gaussSize;y++)  
    {  
        for( x=0;x<params->gaussSize;x++)  
        {  
              
            float dis,weight;  
              
            dis = (y-params->gaussSize/2)*(y-params->gaussSize/2)+(x-params->gaussSize/2)*(x-params->gaussSize/2);  
            weight = exp(-dis/(2.0*params->gaussSigma));  
            *(gaussWindow+y*params->gaussSize+x)=weight;       
        }  
    }  
       
    convolution(IxIx,A,gaussWindow,params->gaussSize,params->gaussSize);//计算IxIx与高斯的卷积  
    convolution(IyIy,B,gaussWindow,params->gaussSize,params->gaussSize);//计算IyIy与高斯的卷积  
    convolution(IxIy,C,gaussWindow,params->gaussSize,params->gaussSize);//计算IxIy与高斯的卷积  
  
    //计算角点量
    for(y=0;y<srcFloat->height;y++)  
    {  
        for(x=0;x<srcFloat->width;x++)  
        {  
            double cornernessValue,Avalue,Bvalue,Cvalue,a=0.05;  
            Avalue = cvGetReal2D(A,y,x);  
            Bvalue = cvGetReal2D(B,y,x);  
            Cvalue = cvGetReal2D(C,y,x);  
            //Harris角点量计算公式: R = det(M) – a*trace(M)^2,其中a为经验值，取值范围介于[0.04, 0.06],
            //另一种角点量计算方法:R = det(M)/trace(M)—这个公式自由发挥，只要能反应角点的特征就可以了
            //其中det(M) = r1*r2 = AB –C^2,Trace(M) = r1+r2 =A+B （其中A=Ix^2;;  B = Iy^2;;; C=IxIy） 
            //本程序采用第二种方法 
            cornernessValue = (Avalue*Bvalue-Cvalue*Cvalue)-a*(Avalue+Bvalue)*(Avalue+Bvalue);  
              
            cvSetReal2D(cornerness,y,x,cornernessValue);    
        }  
      
    }  
  
    //计算局部极大值 及 极大值是否大于阈值  
    int beginY,endY,beginX,endX;  
    int halfWinSize = params->maximumSize/2;  
  
    beginY = halfWinSize;  
    endY = cornerness->height - halfWinSize;  
  
    beginX = halfWinSize;  
    endX = cornerness->width - halfWinSize;  
      
    for(y=beginY;y<endY;)  
    {  
        for(x=beginX;x<endX;)  
        {  
            //寻找局部极大值 及其位置信息  
            float maxValue=0;  
            int flag = 0 ;  
            CvPoint maxLoc;  
            maxLoc.x = -1;  
            maxLoc.y = -1;  
  
            //首先计算以点(x,y)位中心的maximumSize*maximumSize的窗口内部的局部极大值  
            for(int winy=-halfWinSize;winy<=halfWinSize;winy++)  
            {  
                for(int winx=-halfWinSize;winx<=halfWinSize;winx++)  
                {  
                    float value ;  
                    value = cvGetReal2D(cornerness,y+winy,x+winx);  
                      
                    //计算该窗口内 最大值 保存到max 并保存其坐标到maxLoc  
                    if(value>maxValue)  
                    {  
                        maxValue = value;  
                        maxLoc.x = x+winx;  
                        maxLoc.y = y+winy;  
                        flag = 1;  
                    }  
                }  
            }  
  
              
            //如果找到局部极大值 并且该值大于预先设定的阈值 则认为是角点
			//判断是否为角点的标准:
            //r1,r2都很小,对应于图像中的平滑区域
            //r1,r2都很大，对应于图像中的角点
            //r1，r2一个很大，一个很小，对应于图像中的边缘
            if(flag==1 && maxValue>params->threshold)  
            {  
                cvSeqPush(corners,&maxLoc);  
            }                     
            x = x+params->maximumSize;  
        }        
        y = y + params->maximumSize;  
    }  
      
    delete []gaussWindow;  
    cvReleaseImage(&Ix);  
    cvReleaseImage(&Iy);  
    cvReleaseImage(&IxIx);  
    cvReleaseImage(&IyIy);  
    cvReleaseImage(&IxIy);  
    cvReleaseImage(&A);  
    cvReleaseImage(&B);  
    cvReleaseImage(&C);  
    cvReleaseImage(&cornerness);  
    cvReleaseImage(&srcFloat);  
}  
/******************************* 
*对源图像进行卷积运算 
*输入项 
*srcFloat    源图像 
*Ix          卷积的结果 
*dxTemplate  卷积模板 
*widthTemplate 模板的宽度 
*heightTemplate 模板的高度 
********************************/  
void convolution(IplImage* srcFloat,IplImage* Ix,double* dxTemplate , int widthTemplate,int heightTemplate)  
{  
    int x,y,beginY,endY,beginX,endX;  
      
    beginY = heightTemplate/2;  
    endY = srcFloat->height - heightTemplate/2;  
      
    beginX = widthTemplate/2;  
    endX = srcFloat->width - widthTemplate/2;  
      
    for(y=beginY;y<endY;y++)  
    {  
        for(x=beginX;x<endX;x++)  
        {        
            int i,j;  
            double curDx=0;  
            for(i=0;i<heightTemplate;i++)  
            {  
                for(j=0;j<widthTemplate;j++)  
                {  
                    curDx += cvGetReal2D(srcFloat,y+i-heightTemplate/2,x+j-widthTemplate/2)**(dxTemplate+i*widthTemplate+j);  
                }  
            }         
            cvSetReal2D(Ix,y,x,curDx);  
        }     
    }  
}  